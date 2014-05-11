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

//AVC Files
#include "clip.h"
#include "symbol.h"

//SVC Files
#include "svc_type.h"
#include "svc_data.h"
#include "MergeMvIdx.h"



void GetRef4x4(REFPART RefPartition[16], short RefInded4x4[16], short *ref_l0){


	short i;
	for ( i = 0; i < 16; i++){
		int Index = RefPartition [i] . RefIndex;
		RefInded4x4[i] = (short) ref_l0[Index];
	}
}

void SetRef8x8(short RefIndex8x8[], short RefInded4x4[]){
	
	short i;
	for( i = 0; i < 4; i ++){
		RefIndex8x8[i] = MinRef4x4(&RefInded4x4[((i & 1)<< 1)+((i&2)<<2)]);
	}
}


void MergePartition4x4(REFPART RefPartition[16], short RefIndex8x8[], short RefInded4x4[]){

	short i, j;

	//Try to remove block 4x4 predicted in intra mode in each block 8x8
	for ( i = 0; i < 4; i++){
		short *Ref4x4 = &RefInded4x4[((i & 1)<< 1)+((i & 2) << 2)];
		REFPART *RefPart8x8 = &RefPartition[((i & 1) << 1) + ((i & 2) << 2)];


		for( j = 0; j < 4; j++){
			short Ref = Ref4x4 [(j & 1) + ((j & 2) << 1)];
			REFPART *RefPart4x4 = &RefPart8x8 [(j & 1) + ((j & 2)<< 1)];
			
			//If Current block 4x4 reference is different from the 8x8 referenc, try to merge
			if(Ref != RefIndex8x8 [i]){
				//Set blk4x4 reference from current blk8x8
				short Ref1 = Ref4x4 [((j - 1) & 1) + ((j & 2) << 1)];
				short Ref2 = Ref4x4 [((2 - (j & 2) ) << 1) +(j & 1)];

				if ( Ref1 == RefIndex8x8 [i]){
					memcpy(RefPart4x4, &RefPart4x4[1 - ((j & 1) << 1)], sizeof(REFPART));
				}else if ( Ref2 == RefIndex8x8 [i]){
					memcpy(RefPart4x4, &RefPart4x4[(1 - (j & 2)) << 2], sizeof(REFPART));
				}else {
					memcpy(RefPart4x4, &RefPart4x4[((1 - (j & 2)) << 2) + 1 - ((j & 1) << 1)], sizeof(REFPART));
				}
			}
		}
	}
}


void MergePartition8x8(REFPART RefPartition[16], short RefIndex8x8[]){

	short i;
	short Changed8x8[4] = { 0, 0, 0, 0};

	//Try to remove block 8x8 predicted in intra mode
	for ( i = 0; i < 4; i++){
		REFPART *RefPart8x8 = &RefPartition[((i & 1) << 1) + ((i & 2) << 2)];
		Changed8x8[i] = 0;
		//Only for block 8x8 whcich are intra predicted
		if ( RefIndex8x8 [i] == -1 && RefPart8x8 -> RefIdc == -2){
			REFPART *XInv8x8 = &RefPartition[2 -  (i & 0x01) +( (i & 0x02) << 2)];
			REFPART *YInv8x8 = &RefPartition[8 + ((i & 0x01) << 1)  - ((i & 0x02) << 1)];
			REFPART *XYInv8x8 = &RefPartition[10 - (i & 1) - ((i & 2) << 1)];
			Changed8x8[i] = 1;
			if (!Changed8x8[1 - (i & 0x01) + (i & 0x02)] && (XInv8x8 -> RefIdc != -2)){
				// i = 0 -> Right block 8x8 is not intra predicted
				// i = 1 -> Left block 8x8 is not intra predicted
				RefPart8x8 -> MvIndex = RefPart8x8[1] . MvIndex = XInv8x8 -> MvIndex;
				RefPart8x8 -> RefIndex = RefPart8x8[1] . RefIndex = XInv8x8 -> RefIndex;
				RefPart8x8[4] . MvIndex = RefPart8x8[5] . MvIndex = XInv8x8 [4] . MvIndex;
				RefPart8x8[4] . RefIndex = RefPart8x8[5] . RefIndex = XInv8x8 [4] . RefIndex;
			} else if (!Changed8x8[2 + (i & 0x01) - (i & 0x02)] && (YInv8x8 -> RefIdc != -2)){
				RefPart8x8 -> MvIndex = RefPart8x8[4] . MvIndex = YInv8x8 -> MvIndex;
				RefPart8x8 -> RefIndex = RefPart8x8[4] . RefIndex =  YInv8x8 -> RefIndex;
				RefPart8x8[1] . MvIndex = RefPart8x8[5] . MvIndex = YInv8x8 [1] . MvIndex;
				RefPart8x8[1] . RefIndex = RefPart8x8[5] . RefIndex = YInv8x8 [1] . RefIndex;
			}else if (!Changed8x8[3 - (i & 1) - (i & 2)] && (XYInv8x8 -> RefIdc != -2)){
				RefPart8x8 -> MvIndex = RefPart8x8[1] . MvIndex = RefPart8x8[4] . MvIndex = RefPart8x8[5] . MvIndex = XYInv8x8 -> MvIndex;
				RefPart8x8 -> RefIndex = RefPart8x8[1] . RefIndex = RefPart8x8[4] . RefIndex = RefPart8x8[5] . RefIndex = XYInv8x8 -> RefIndex;
			}
		}
	}
}



void MergeMv(const NAL *Nal, const int Direct8x8Inference, REFPART RefPartition[16], const char *MotionPred, short mv_cache_l0[][2], char Mode[4], short *mv_l0){

	short i,j;
	short Threshold = 1;
	
	//Charge Mv in Mvcache for each 8x8 block
	for ( i = 0; i < 4; i++){
		//for each 4x4 block
		for ( j = 0; j < 4; j++){
			if ( MotionPred[(i & 2) + ((j & 2) >> 1)]){
				short BlkIndex = (i << 2) + j;
				short CacheIndex = 12 + j + (i << 3);
				int MvIndex = RefPartition [BlkIndex] . MvIndex;
				mv_cache_l0[CacheIndex][0] = MV_CAL(mv_l0[MvIndex]	  , Nal -> MvCalx [Nal -> LayerId]);
				mv_cache_l0[CacheIndex][1] = MV_CAL(mv_l0[MvIndex + 1], Nal -> MvCaly [Nal -> LayerId]);
			}
		}
	}

	//only for slice B
	if (Direct8x8Inference){
		for ( i = 0; i < 4; i++){
			if ( MotionPred[i]){
				//for each 8x8 block
				short XPart = ( i  & 1 ) * 3;
				short YPart = ( i  >> 1 ) * 3;
				short CacheIndex = 12 + ((i & 1) << 1) + ((i & 2) << 3);
				short Index = 12 + XPart + (YPart << 3);
				mv_cache_l0[CacheIndex + 1][0] = mv_cache_l0[CacheIndex + 8][0] = mv_cache_l0[CacheIndex + 9][0] = mv_cache_l0[CacheIndex][0] = mv_cache_l0[Index][0];
				mv_cache_l0[CacheIndex + 1][1] = mv_cache_l0[CacheIndex + 8][1] = mv_cache_l0[CacheIndex + 9][1] = mv_cache_l0[CacheIndex][1] = mv_cache_l0[Index][1];
			}
		}
	}

	
	//Determine type for each 8x8 block
	for (i = 0; i < 4; i++){
		if ( MotionPred[i]){
			short CacheIndex = 12 + ((i & 2) << 3) + ((i & 1) << 1);
			short Hor1 = MvDiff(&mv_cache_l0[CacheIndex][0], &mv_cache_l0[CacheIndex + 1][0]) <= Threshold;
			short Hor2 = MvDiff(&mv_cache_l0[CacheIndex + 8][0], &mv_cache_l0[CacheIndex + 9][0]) <= Threshold;
			short Vert1 = MvDiff(&mv_cache_l0[CacheIndex][0], &mv_cache_l0[CacheIndex + 8][0]) <= Threshold;
			short Vert2 = MvDiff(&mv_cache_l0[CacheIndex + 1][0], &mv_cache_l0[CacheIndex + 9][0]) <= Threshold;
			short Diag = MvDiff(&mv_cache_l0[CacheIndex][0], &mv_cache_l0[CacheIndex + 9][0]) <= Threshold;

			short b8x8 = Hor1 && Vert1 && Diag;
			short b8x4 = Hor1 && Hor2;
			short b4x8 = Vert1 && Vert2;
			
			Mode [i] = b8x8 ? 0: b8x4 ? 1: b4x8 ? 2: 3;
		}
	}
}




void CombineMv(char Mode8x8[4], short mv_cache_l0[][2], const char *MotionPred){

	short i;
	short Mvx, Mvy;
	for ( i = 0; i < 4; i++){
		if (MotionPred [i]){
			short Index = 12 + ((i & 1) << 1) + ((i & 2) << 3);
			switch(Mode8x8 [i]){
				case 0:	//8x8
					Mvx = (short) ((mv_cache_l0[Index][0] + mv_cache_l0[Index + 1][0] + mv_cache_l0[Index + 8][0] + mv_cache_l0[Index + 9][0] + 2) >> 2);
					Mvy = (short) ((mv_cache_l0[Index][1] + mv_cache_l0[Index + 1][1] + mv_cache_l0[Index + 8][1] + mv_cache_l0[Index + 9][1] + 2) >> 2);
					mv_cache_l0[Index][0] = mv_cache_l0[Index + 1][0] = mv_cache_l0[Index + 8][0] = mv_cache_l0[Index + 9][0] = Mvx;
					mv_cache_l0[Index][1] = mv_cache_l0[Index + 1][1] = mv_cache_l0[Index + 8][1] = mv_cache_l0[Index + 9][1] = Mvy;
					break;
				case 1: //8x4
					Mvx = (short) ((mv_cache_l0[Index][0] + mv_cache_l0[Index + 1][0] + 1) >> 1);
					Mvy = (short) ((mv_cache_l0[Index][1] + mv_cache_l0[Index + 1][1] + 1) >> 1);
					mv_cache_l0[Index][0] = mv_cache_l0[Index + 1][0] = Mvx;
					mv_cache_l0[Index][1] = mv_cache_l0[Index + 1][1] = Mvy;
					
					Mvx = (short) ((mv_cache_l0[Index + 8][0] + mv_cache_l0[Index + 9][0] + 1) >> 1);
					Mvy = (short) ((mv_cache_l0[Index + 8][1] + mv_cache_l0[Index + 9][1] + 1) >> 1);
					mv_cache_l0[Index + 8][0] = mv_cache_l0[Index + 9][0] = Mvx;
					mv_cache_l0[Index + 8][1] = mv_cache_l0[Index + 9][1] = Mvy;
					break;
				case 2: //4x8
					Mvx = (short) ((mv_cache_l0[Index][0] + mv_cache_l0[Index + 8][0] + 1) >> 1);
					Mvy = (short) ((mv_cache_l0[Index][1] + mv_cache_l0[Index + 8][1] + 1) >> 1);
					mv_cache_l0[Index][0] = mv_cache_l0[Index + 8][0] = Mvx;
					mv_cache_l0[Index][1] = mv_cache_l0[Index + 8][1] = Mvy;
					
					Mvx = (short) ((mv_cache_l0[Index + 1][0] + mv_cache_l0[Index + 9][0] + 1) >> 1);
					Mvy = (short) ((mv_cache_l0[Index + 1][1] + mv_cache_l0[Index + 9][1] + 1) >> 1);
					mv_cache_l0[Index + 1][0] = mv_cache_l0[Index + 9][0] = Mvx;
					mv_cache_l0[Index + 1][1] = mv_cache_l0[Index + 9][1] = Mvy;
					break;
			}
		}
	}
}



void SetPartPred( REFPART RefPartition[16], short *ref_l0){

	short RefIndex8x8[4];
	short RefInded4x4[16];
	//Get all reference index for all block 4x4
	GetRef4x4(RefPartition, RefInded4x4, ref_l0);
	SetRef8x8(RefIndex8x8, RefInded4x4);
	MergePartition4x4(RefPartition, RefIndex8x8, RefInded4x4);
	MergePartition8x8(RefPartition, RefIndex8x8);
}
