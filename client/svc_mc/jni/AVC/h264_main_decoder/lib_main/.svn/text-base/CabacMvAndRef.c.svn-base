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
* but WITHOUT ANY WARRANTY; without even the implied warranty off
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

//AVC Files
#include "type.h"
#include "main_data.h"
#include "vlc_cabac.h"
#include "mb_pred_main.h"
#include "motion_vector.h"
#include "CabacMvAndRef.h"




/**
To determine the reference index for a P macroblock.
*/
int GetCabacPRefList(CABACContext *c, unsigned char *state, char *refLx, short *RefCacheLx,
					 char *MotionPred, int Mode, int NumRefLx, int index)
{

	//Read one reference list and a second one for 16x8 or 8x16 block
	if ( NumRefLx > 0) {
		short refa, refb;

		if(!MotionPred[0]){
			refa = RefCacheLx[12 - 1];
			refb = RefCacheLx[12 - 8];

			refLx [0] = decode_cabac_mb_ref( c, state, refa, refb); 
			FillRefIdx(refLx[0], &RefCacheLx[12], Mode);
		}

		if (Mode != 3 && !MotionPred[1]){
			refa = RefCacheLx[index - 1];
			refb = RefCacheLx[index - 8];
			refLx [1] = decode_cabac_mb_ref(  c, state, refa, refb); 
			FillRefIdx(refLx[1], &RefCacheLx[index], Mode);
		}
#ifdef ERROR_DETECTION
		//Error detection
		if(ErrorsCheckRefLx(refLx, NumRefLx)){
			return 1;
		}
#endif
	}
	return 0;
}


/**
To determine the reference index for a B8x8 macroblock.
*/
int GetCabacBSubRefList(CABACContext *c, unsigned char *state, char *refLx, short *RefCacheLx,
						char *MotionPred, unsigned char *SubMbType, int NumRefLx, int Pred_Lx)
{

	int mbPartIdx;
	//Recovery of the image reference of each sub-macroblock
	if  (NumRefLx > 0) {
		for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ ) {
			int sub_mb_part = sub_mb_b_name[SubMbType [mbPartIdx]];
			int index = SCAN8(mbPartIdx << 2);
			if (!MotionPred[mbPartIdx] && (sub_mb_part != B_direct) && (sub_mb_part != Pred_Lx )) {
				int refa = RefCacheLx[index - 1];
				int refb = RefCacheLx[index - 8];
				refLx [mbPartIdx] = decode_cabac_mb_ref( c, state, refa, refb); 
			}
			refill_ref(2, 2, refLx[mbPartIdx], &RefCacheLx[index]); 
		}
	}else {
		//refLx already set to zero if NumRefLx == 0
		refill_ref(4, 4, refLx[0], &RefCacheLx[12]); 
	}

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckSubRefLx(refLx, NumRefLx)){
		return 1;
	}
#endif
	return 0;
}


/**
To determine the reference index for a B16x16 macroblock.
*/
int GetCabacBRefList(CABACContext *c, unsigned char *state, char *refLx,short *RefCacheLx, char *MotionPred,
					 int Mode, int NumRefLx, unsigned char *MbPartMode, short Pred_Lx, int index)
{

	//Read one reference list and a second one for 16x8 or 8x16 block
	if ( NumRefLx > 0) {
		if(MbPartMode [0] != Pred_Lx && !MotionPred[0]){
			int refa = RefCacheLx[12 - 1];
			int refb = RefCacheLx[12 - 8];
			refLx [0] = decode_cabac_mb_ref( c, state, refa, refb); 
			FillRefIdx(refLx[0], &RefCacheLx[12], Mode);
		}else {
			refLx [0] = -1; //mbl check
		}

		if (Mode != 3 && MbPartMode [1] != Pred_Lx && !MotionPred[1]){
			int refa = RefCacheLx[index - 1];
			int refb = RefCacheLx[index - 8];
			refLx [1] = decode_cabac_mb_ref( c, state, refa, refb); 
			FillRefIdx(refLx[1], &RefCacheLx[index], Mode);
		}else {
			refLx [1] = -1;//mbl check
		}

#ifdef ERROR_DETECTION
		//Error detection
		if(ErrorsCheckRefLx(refLx, NumRefLx)){
			return 1;
		}
#endif
	}
	return 0;
}

/**
To determine the motion vector for a P8x8 macroblock.
*/
void ReadAndComputeCabac8x8MotionVector( CABACContext *c, unsigned char *state, int mbPartIdx, int sub_mb_type, char *RefLx, 
							  short *RefLxCache, short MvCacheLx[][2], short MvLxCache[][2], int SliceType)
{
	int subMbPartIdx;
	short MvdL0[2];
	for ( subMbPartIdx = 0 ; subMbPartIdx < sub_num_b_part[sub_mb_type] ; subMbPartIdx++ ) {
		int xpart = (sub_mb_type < 5 || sub_mb_type == 6 || sub_mb_type == 8) ? 2: 1;
		int ypart = (sub_mb_type < 4 || sub_mb_type == 5 || sub_mb_type == 7 || sub_mb_type == 9) ? 2:1;
		int index = SCAN8(( mbPartIdx << 2) + subMbPartIdx * xpart);

		ReadCabacSubMotionVector(c, state, MvCacheLx, MvdL0, mbPartIdx, subMbPartIdx, sub_mb_type, index, SliceType);

		//Compute Mv
		refill_ref(xpart, ypart, RefLx[mbPartIdx], &RefLxCache[index]);  
		motion_vector(MvLxCache, MvdL0, RefLxCache, &MvLxCache[index][0], RefLx[mbPartIdx], index, xpart);
	}
	refill_motion_vector(1, sub_mb_type, &MvLxCache[SCAN8(( mbPartIdx << 2))]);
}



/**
To determine the motion vector for a P16x16 macroblock.
*/
void ReadAndComputeCabac16x16MotionVector( CABACContext *c, unsigned char *state, char *RefLx, short *RefLxCache,
								short MvCacheLx[][2], short mvLx_cache[][2])
{
	short MvdLX[2];

	//Decode the MV
	ReadCabacMotionVector(c, state, MvCacheLx, MvdLX, 3, 12);

	//Compute Mv
	refill_ref(4, 4, RefLx[0], &RefLxCache[12]); 
	motion_vector(mvLx_cache, MvdLX, RefLxCache, &mvLx_cache[12][0], RefLxCache[12], 12, 4);
	refill_mv(4, 4, mvLx_cache[12], &mvLx_cache[12]);
}




/**
To determine the motion vector for a P16x16 macroblock.
*/
void ReadAndComputeCabac8x16MotionVector( CABACContext *c, unsigned char *state, char *RefLx, short *RefLxCache,
							   short MvCacheLx[][2], short mvLx_cache[][2], int index)
{

	short MvdLX[2];

	//Decode the MV
	ReadCabacMotionVector(c, state, MvCacheLx, MvdLX, 1, index);

	//Compute Mv
	refill_ref(2, 4, RefLx[0], &RefLxCache[index]); 
	motion_vector_8x16(mvLx_cache, MvdLX, RefLxCache, &mvLx_cache[index][0], RefLx [0], index);
	refill_mv(2, 4, mvLx_cache[index], &mvLx_cache[index]);
}



/**
To determine the motion vector for a P16x16 macroblock.
*/
void ReadAndComputeCabac16x8MotionVector( CABACContext *c, unsigned char *state, char *RefLx, short *RefLxCache,
							   short MvCacheLx[][2], short mvLx_cache[][2], int index)
{


	short MvdLX[2];
		
	//Decode the MV
	ReadCabacMotionVector(c, state, MvCacheLx, MvdLX, 2, index);

	//Compute Mv
	refill_ref(4,2, RefLx[0], &RefLxCache[index]); 	
	motion_vector_16x8(mvLx_cache, MvdLX, RefLxCache, &mvLx_cache[index][0], RefLx[0], index);
	refill_mv(4, 2, &mvLx_cache[index][0], &mvLx_cache[index]); 
}
