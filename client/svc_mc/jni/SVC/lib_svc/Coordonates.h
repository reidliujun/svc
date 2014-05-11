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


#ifndef COORDONATES_H
#define COORDONATES_H

#include "math.h"

//SVC Files
#include "svc_data.h"



void GetRefIdc(const NAL *Nal, short x, short y, REFPART *RefPartition, RESIDU *BaseResidu);

short GetBasePosition(short x, short y, short *x_base, short *y_base, short *x_mbPartIdx, short *y_mbPartIdx,
						const NAL *Nal, REFPART RefPartition[16], const DATA *macroblock, RESIDU *residu, 
						RESIDU *base_residu, short *mv_l0, short *ref_l0, short mv_cache_l0[][2]);


short GetBasePositionB(const int Direct8x8xFlag, short x, short y, short *x_base, short *y_base, short *x_mbPartIdx, short *y_mbPartIdx, 
						  const NAL *Nal, REFPART RefPartition[16], const DATA *macroblock, RESIDU *residu, RESIDU *base_residu, 
						  short *mv_l0, short *ref_l0, short *mv_l1, short *ref_l1, short mv_cache_l0[][2], short mv_cache_l1[][2]);






/**
Fill table according to the macroblock type and partionning.
*/
static __inline void FullMotionPred(const int MbType, const int Mode, const char *MotionPredFlag, char *MotionPred)
{
	if ( IS_BL(MbType)){
		MotionPred[0] = MotionPred[1] = MotionPred[2] = MotionPred[3] = 1;
	}else if (Mode == 3){
		MotionPred[0] = MotionPred[1] = MotionPred[2] = MotionPred[3] = MotionPredFlag[0];
	}else if (Mode == 2){
		MotionPred[0] = MotionPred[1] = MotionPredFlag[0];
		MotionPred[2] = MotionPred[3] = MotionPredFlag[1];
	}else if (Mode == 1){
		MotionPred[0] = MotionPred[2] = MotionPredFlag[0];
		MotionPred[1] = MotionPred[3] = MotionPredFlag[1];
	}else{
		MotionPred[0] = MotionPredFlag[0];
		MotionPred[1] = MotionPredFlag[1];
		MotionPred[2] = MotionPredFlag[2];
		MotionPred[3] = MotionPredFlag[3];
	}
}

/**
Initialize parameters according to current macroblock.
*/
static __inline void GetBaseMbCoord(const DATA *Macroblock, short *x_base, short *y_base,
									short *x_mbPartIdx, short *y_mbPartIdx)
{

	*x_base = Macroblock -> xBase;
	*y_base = Macroblock -> yBase;

	//Get which block 8x8 
	*x_mbPartIdx = Macroblock -> xBasePart;
	*y_mbPartIdx = Macroblock -> yBasePart;
}


/**
Compute the base macroblock addresses.
*/
static __inline short GetBaseMbAddr(const NAL *Nal, DATA *Macroblock, const short x, const short y){

	short x_base, y_base;
	
	if(Nal -> SpatialScalability){
		x_base = Macroblock -> xBase = (short) RESCALE_POSITION((x - Nal -> left_offset + 1), Nal -> down_Width, (Nal -> up_Width >> 1), Nal -> up_Width);
		y_base = Macroblock -> yBase = (short) RESCALE_POSITION((y - Nal -> top_offset + 1), Nal -> down_Height, (Nal -> up_Height >> 1), Nal -> up_Height);
		Macroblock -> xBasePart = ((x_base) - ((x_base >> 4) << 4)) >> 2;
		Macroblock -> yBasePart = ((y_base) - ((y_base >> 4) << 4)) >> 2;
	}else {
		x_base = x;
		y_base = y;
	}

	//Initialize used data
	Macroblock -> xBase = x_base >> 4;
	Macroblock -> yBase = y_base >> 4;		
	Macroblock -> MbBaseAddr = Macroblock -> xBase + (Macroblock -> yBase * (Nal -> base_Width >> 4));

	return Macroblock -> MbBaseAddr;
}


/**
Define the current macroblock type according to the base layer.
*/
static __inline unsigned char GetBaseType(const NAL *Nal, RESIDU *residu, DATA *Macroblock, const short x, const short y){

	short mbBaseAddr = GetBaseMbAddr(Nal, Macroblock, x << 4, y << 4);

	if(IS_I_SVC(residu[mbBaseAddr] . MbType))
		return I_BL;
	else  if(IS_P_SVC(residu[mbBaseAddr] . MbType))
		return P_BL;
	else
		return B_BL;
}


#endif
