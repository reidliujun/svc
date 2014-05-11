/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *              http://www.ietr.org/
   *
   *
   *
   *
   *
   * This library is free software; you can redistribute it and/or
   * modify it under the terms of the GNU Lesser General Public
   * License as published by the Free Software Foundation; either
   * version 2 of the License, or (at your option) any later version.
   *
   * This library is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   * Lesser General Public License for more details.
   *
   * You should have received a copy of the GNU Lesser General Public
   * License along with this library; if not, write to the Free Software
   * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
   *
   *
   * $Id$
   *
   **************************************************************************/

#include "string.h"

//SVC Files
#include "clip.h"
#include "data.h"
#include "svc_data.h"
#include "svc_type.h"
#include "MergeMvIdx.h"
#include "Coordonates.h"
#include "PartitionType.h"



	//From JSVM
const short NxNPartIdx[5][4] =
    {
      { 0, 1, 2, 3 }, // MODE_8x8       or    BLK_4x4
      { 0, 1, 0, 1 }, // MODE_8x16      or    BLK_4x8
      { 0, 0, 2, 2 }, // MODE_16x8      or    BLK_8x4
      { 0, 0, 0, 0 }, // MODE_16x16     or    BLK_8x8
	  { 0, 0, 0, 0 }, // MODE_SKIP 
    };





/**
Determine in which block 4x4, the prediction is in the base macroblock 
*/
void GetRefIdc(const NAL *Nal, short x, short y, REFPART *RefPartition, RESIDU *BaseResidu){

	
	short iBaseX, iBaseY, BaseX, BaseY, BaseAddr;
	if (Nal -> SpatialScalability){
		 iBaseX = (short) RESCALE_POSITION((x - Nal -> left_offset), Nal -> down_Width, (Nal -> up_Width >> 1), Nal -> up_Width);	
		 iBaseY = (short) RESCALE_POSITION((y - Nal -> top_offset), Nal -> down_Height, (Nal -> up_Height >> 1), Nal -> up_Height); 
	}else{
		iBaseX = x;
		iBaseY = y;
	}

	BaseX = iBaseX >> 4;
	BaseY = iBaseY >> 4;
	BaseAddr = BaseX + BaseY * (Nal -> base_Width >> 4);

		
	iBaseX -= (BaseX << 4);
	iBaseY -= (BaseY << 4);
	//If it's an intra macroblock, no need to continue
	if ( IS_I_SVC (BaseResidu [BaseAddr] . MbType)){
		short Index8x8 = ((iBaseY >> 3) << 1) + (iBaseX >> 3);
		short Index4x4 = (((iBaseY & 7) >> 2) << 1) + ((iBaseX & 7) >> 2);
		short BlkX4x4 = ((Index8x8  & 1) << 1) + (Index4x4  & 1);
		short BlkY4x4 = ((Index8x8 >> 1) << 1) + (Index4x4 >> 1);
		RefPartition -> MvIndex = (((BaseX << 2) + BlkX4x4) << 1) + ((BlkY4x4 + (BaseY << 2)) * Nal -> b_stride);
		RefPartition -> RefIndex = ((BaseX << 2) + BlkX4x4) + (BlkY4x4 + (BaseY << 2)) * Nal -> b8_stride;
		RefPartition -> PartIdc = (BaseAddr << 4) + (BlkY4x4 << 2) + BlkX4x4;
		RefPartition -> RefIdc = -2;

	}else{
		//Determine macroblock partition and sub partition
		short MbPartIdx4x4 = 0;
		short BlkX4x4, BlkY4x4;
		short Index8x8 = ((iBaseY >> 3) << 1) + (iBaseX >> 3);
		short Index4x4 = (((iBaseY & 7) >> 2) << 1) + ((iBaseX & 7) >> 2);
		short MbPartIdx8x8 = NxNPartIdx [BaseResidu [BaseAddr] . Mode] [Index8x8];
		short Mode8x8 = (short) (BaseResidu [BaseAddr] . SubMbType [Index8x8]);

		if ( BaseResidu [BaseAddr] . Mode == 0){
			if ( IS_P(BaseResidu [BaseAddr] . MbType)){
				MbPartIdx4x4 = NxNPartIdx [3 - Mode8x8] [Index4x4];
			}else if (Mode8x8 == 0){
				//B skip
				MbPartIdx4x4 = Index4x4;
			}else{
				char x8 = (char) (Mode8x8 < 4);
				char x4 = (char) (Mode8x8 > 9);
				short Mode = 3 * x4 + (!x4 && !x8) * (((Mode8x8 & 1) << 1) + 1);
				MbPartIdx4x4 = NxNPartIdx [3 - Mode] [Index4x4];
			}
		}

		BlkX4x4 = ((MbPartIdx8x8  & 1) << 1) + (MbPartIdx4x4  & 1);
		BlkY4x4 = ((MbPartIdx8x8 >> 1) << 1) + (MbPartIdx4x4 >> 1);
		RefPartition -> MvIndex = (((BaseX << 2) + BlkX4x4) << 1) + ((BlkY4x4 + (BaseY << 2)) * Nal -> b_stride);
		RefPartition -> RefIndex = ((BaseX << 2) + BlkX4x4) + (BlkY4x4 + (BaseY << 2)) * Nal -> b8_stride;
		RefPartition -> PartIdc = (BaseAddr << 4) + (BlkY4x4 << 2) + BlkX4x4;
		RefPartition -> RefIdc = 0;
	}
}





/**
Homogeneises INTER and INTRA mode into 8x8 blocks.
*/
void SetInterIntra(const NAL *Nal, short x, short y, REFPART RefPartition[16], RESIDU *BaseResidu)
{

	short i;
	REFPART Partition[4];
	for ( i = 0; i < 4; i++){
		GetRefIdc(Nal, x + (i & 1) * 15, y + ((i & 2) >> 1) * 15, &Partition[i], BaseResidu);
		if (Partition [i] . RefIdc != RefPartition [((i & 1) << 1) + ((i & 2) << 2)] . RefIdc){
			//just copy the RefIdc ( to indicate intra prediction), and the RefIndex, to not to do the inter prediction 
			RefPartition [((i & 1) << 1) + ((i & 2) << 2)] . RefIdc = Partition [i] . RefIdc;
			RefPartition [((i & 1) << 1) + ((i & 2) << 2)] . RefIndex = Partition [i] . RefIndex;
			RefPartition [((i & 1) << 1) + ((i & 2) << 2) + 1] . RefIdc = Partition [i] . RefIdc;
			RefPartition [((i & 1) << 1) + ((i & 2) << 2) + 1] . RefIndex = Partition [i] . RefIndex;
			RefPartition [((i & 1) << 1) + ((i & 2) << 2) + 4] . RefIdc = Partition [i] . RefIdc;
			RefPartition [((i & 1) << 1) + ((i & 2) << 2) + 4] . RefIndex = Partition [i] . RefIndex;
			RefPartition [((i & 1) << 1) + ((i & 2) << 2) + 5] . RefIdc = Partition [i] . RefIdc;
			RefPartition [((i & 1) << 1) + ((i & 2) << 2) + 5] . RefIndex = Partition [i] . RefIndex;
		}
	}
}





/**
Gives for each block 4x4, the base position and its reference block 4x4 in the base macroblock
*/
unsigned char InitializeBaseIndex(const NAL *Nal, short x, short y, REFPART RefPartition[16], RESIDU *BaseResidu, RESIDU *Residu)
{

	short IntraBL = 1;
	short i;
	for ( i = 0; i < 16; i++){
		GetRefIdc(Nal, x + 1 + POSX(i), y + 1 + POSY(i), &RefPartition[i], BaseResidu);
		IntraBL = IntraBL && ( RefPartition[i] . RefIdc == -2);
	}

	if ( IS_BL(Residu -> MbType)){
		if(IntraBL){
			return I_BL;
		}else{
			return P_BL;
		}
	}else{
		return Residu -> MbType;
	}
}






/**
Computes for each macroblock the mode of the macroblock according to base layer macroblock.
*/
short GetBasePosition(short x, short y, short *x_base, short *y_base, short *x_mbPartIdx, short *y_mbPartIdx, 
						const NAL *Nal, REFPART RefPartition[16], const DATA *Macroblock, RESIDU *residu, 
						RESIDU *base_residu, short *mv_l0, short *ref_l0, short mv_cache_l0[][2])
{



	if ( residu -> InCropWindow){
		//Get the base position and the base macroblock coordonates
		GetBaseMbCoord(Macroblock, x_base, y_base, x_mbPartIdx, y_mbPartIdx);

		//In case of motion prediction, initialize a tab to indicate in which block 4x4 the prediction will be done.
		if (IS_BL(residu -> MbType) || Macroblock -> MotionPredL0[0] || Macroblock -> MotionPredL0[1] || Macroblock -> MotionPredL0[2] || Macroblock -> MotionPredL0[3]
			||  Macroblock -> MotionPredL1[0] || Macroblock -> MotionPredL1[1] || Macroblock -> MotionPredL1[2] || Macroblock -> MotionPredL1[3]){
			
			residu -> MbType = InitializeBaseIndex(Nal, x, y, RefPartition, base_residu, residu);

			//In case of an Inter macroblock, we have to get the partionning 
			if ( residu -> MbType != I_BL){
				if ( Nal -> SpatialScalabilityType > 2){
					char ModeIndex8x8[4];
					char MotionPredL0[4]; 

					FullMotionPred(residu -> MbType, residu -> Mode, Macroblock -> MotionPredL0, MotionPredL0);
					//Reference and motion prediction are done with mv_cache
					SetPartPred(RefPartition, ref_l0);
					MergeMv(Nal, 0, RefPartition, MotionPredL0, mv_cache_l0, ModeIndex8x8, mv_l0);
					CombineMv(ModeIndex8x8, mv_cache_l0, MotionPredL0);
					//Last check for intra prediction
					SetInterIntra(Nal, x, y, RefPartition, base_residu);		
				}
				if ( IS_BL(residu -> MbType)){
					SetPartionning(0, residu, RefPartition);
				}
			}
		}
	} 
	return Macroblock -> MbBaseAddr;
}







/**
Computes for each B macroblock the mode of the macroblock according to base layer macroblock.
*/
short GetBasePositionB(const int Direct8x8xFlag, short x, short y, short *x_base, short *y_base, short *x_mbPartIdx, short *y_mbPartIdx, 
						  const NAL *Nal, REFPART RefPartition[32], const DATA *Macroblock, RESIDU *residu, RESIDU *base_residu, 
						  short *mv_l0, short *ref_l0, short *mv_l1, short *ref_l1, short mv_cache_l0[][2], short mv_cache_l1[][2])
{


	
	if ( residu -> InCropWindow){
		//Get the base position and the base macroblock coordonates
		GetBaseMbCoord(Macroblock, x_base, y_base, x_mbPartIdx, y_mbPartIdx);

		//In case of motion prediction, initialize a tab to indicate in which block 4x4 the prediction will be done.
		if (IS_BL(residu -> MbType) ||  Macroblock -> MotionPredL0[0] || Macroblock -> MotionPredL0[1] || Macroblock -> MotionPredL0[2] || Macroblock -> MotionPredL0[3]
		||  Macroblock -> MotionPredL1[0] || Macroblock -> MotionPredL1[1] || Macroblock -> MotionPredL1[2] || Macroblock -> MotionPredL1[3]){
			
			residu -> MbType = InitializeBaseIndex(Nal, x, y, RefPartition, base_residu, residu);

			if ( IS_P(residu -> MbType)){
				residu -> MbType += 1;
			}

			//In case of an Inter macroblock, we have to get the partionning 
			if ( residu -> MbType != I_BL){
				int i;
				int BaseMbAddr = *x_base + (*y_base * (Nal -> base_Width >> 4));
				memcpy(&RefPartition[16], RefPartition, 16 * sizeof(REFPART));
				if ( IS_P(base_residu[BaseMbAddr] . MbType)){
					//When Base slice is an P, and current is B. So no L1 prediction for Interlayer prediction
					*ref_l1 = -1;
					for ( i = 0; i < 16; i++){
						RefPartition[16 + i] . RefIndex = 0;
					}
				}
					
				if ( Nal -> SpatialScalabilityType > 2){
					char ModeIndex8x8[4];
					char MotionPredL0[4]; 
					char MotionPredL1[4]; 

					FullMotionPred(residu -> MbType, residu -> Mode, Macroblock -> MotionPredL0, MotionPredL0);
					FullMotionPred(residu -> MbType, residu -> Mode, Macroblock -> MotionPredL1, MotionPredL1);

					//Reference and motion prediction are done with mv_cache
					SetPartPred(RefPartition, ref_l0);
					SetPartPred(&RefPartition[16], ref_l1);
					MergeMv(Nal, Direct8x8xFlag, RefPartition, MotionPredL0, mv_cache_l0, ModeIndex8x8, mv_l0);
					MergeMv(Nal, Direct8x8xFlag, &RefPartition[16], MotionPredL1, mv_cache_l1, ModeIndex8x8, mv_l1);
					CombineMv(ModeIndex8x8, mv_cache_l0, MotionPredL0);
					CombineMv(ModeIndex8x8, mv_cache_l1, MotionPredL1);
					//Last check for intra prediction
					SetInterIntra(Nal, x, y, RefPartition, base_residu);		
				}
				if ( IS_BL(residu -> MbType)){
					SetPartionning(1, residu, RefPartition);
				}
			}
		}
	} 
	return Macroblock -> MbBaseAddr;
}
