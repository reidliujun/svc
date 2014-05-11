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


#ifndef FILL_H
#define FILL_H





void fill_caches_svc ( const SLICE *ai_pstSlice, const RESIDU *residu, const RESIDU *BaseResidu, const int EntropyCodingModeFlog, 
					  unsigned char NonZeroCountCache [], const unsigned char *SliceTable, const DATA *CurrBlock,
					  short aio_tiIntra4x4_pred_mode_cache[], const short ai_iMb_x , const short ai_iMb_y,	
					  const int ai_iConstrained_intra_pred, const int level_prediction_flag);

void fill_caches_motion_vector_full_ref(  const SLICE *ai_pstSlice, const int ai_iB_stride, const int ai_iB8_stride, 
										short aio_tiRef_cache_l0 [  ], short aio_tiMv_cache_l0[  ][2], 
										const unsigned char *SliceTable, const RESIDU CurrResidu [ ], short ai_tiMv[  ], 
										const short *ai_tiRef,  const int ai_iX, const int ai_iY);

void fill_caches_full_ref(  const SLICE *ai_pstSlice, const int ai_iB_stride, const int ai_iB8_stride, short aio_tiRef_cache_l0 [ ], 
						  short aio_tiMv_cache_l0[ ][2], const unsigned char *SliceTable, const RESIDU CurrResidu [ ],
						  short ai_tiMv[ ], const short *ai_tiRef, const int ai_iX, const int ai_iY);


void fill_caches_motion_vector_B_full_ref(  SLICE *slice, int b_stride, int b8_stride, short ref_cache_l0 [], short ref_cache_l1 [], 
										  short  mv_cache_l0[][2], short  mv_cache_l1[][2], unsigned char *SliceTable, const RESIDU CurrResidu [ ], 
										  short *mv_l0, short mv_l1[], short ref_l0[], short ref_l1[], int iCurrMbAddr, int ai_iX);

void mergeMotionData(int *mb_mode, int macroblock_mode[4], short *mv_l0, short *ref_l0, short base_index_l0[], int mv_inc, short mv_cache_l0[][2]);








#include "symbol.h"
#include <math.h>
#include "svc_type.h"
#include "svc_data.h"

#define ABS_MV(X,Y) (abs(X[0] - Y[0]) + abs(X[1] - Y[1]))








static __inline void set_macroblock_mode(int *mode, int macroblock_mode[4], int aui4x4Idx[16]){


	int bR1 = (macroblock_mode[0] == macroblock_mode[1]) && (macroblock_mode[0] == BLK_8x8) && (aui4x4Idx[0] == aui4x4Idx[4]);
	int bR2 = (macroblock_mode[2] == macroblock_mode[3]) && (macroblock_mode[2] == BLK_8x8) && (aui4x4Idx[8] == aui4x4Idx[12]);
	int bC1 = (macroblock_mode[0] == macroblock_mode[2]) && (macroblock_mode[0] == BLK_8x8) && (aui4x4Idx[0] == aui4x4Idx[8]);
	int bC2 = (macroblock_mode[1] == macroblock_mode[3]) && (macroblock_mode[1] == BLK_8x8) && (aui4x4Idx[4] == aui4x4Idx[12]);

	*mode = MBLK_16x16;

	if( ! bC1 || ! bC2 )  {
		*mode = MBLK_16x8 ;
	}

	if( ! bR1 || ! bR2 )  {
		*mode = ( *mode == MBLK_16x16 ) ? MBLK_8x16  : MBLK_8x8;
	}
}

#endif
