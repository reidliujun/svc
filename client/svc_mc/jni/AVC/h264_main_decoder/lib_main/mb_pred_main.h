/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *			  Maxime  PELCAT <mpelcat@insa-rennes.Fr>
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

#ifndef MB_PRED_MAIN_H
#define MB_PRED_MAIN_H

#include "type.h"
#include "bitstream.h"
#include "ErrorsDetection.h"


extern const int sub_mb_b_name[13];

char sub_mb_pred_B (  int *dct_allowed,const unsigned char *ai_pcData, int *aio_piPosition, 
				   const SLICE *Slice, RESIDU *Current_residu, short mv_cache_l0[][2], 
				   short *ref_cache_l0, short mv_cache_l1[][2], short *ref_cache_l1, 
				   short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1, 
				   int direct_8x8_inference_flag, int long_term, int slice_type, int is_svc);


char mb_pred_B ( const unsigned char *ai_pcData, int *aio_piPosition, const SLICE *Slice, 
				DATA * aio_pstMacroblock, RESIDU *CurrResidu, short mv_cache_l0[][2], 
				short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1);


void motion_vector_B_skip(short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], 
						  short *aio_tref_cache_l0, short *aio_tref_cache_l1, 
						  short *aio_tMv_l0, short *aio_tMv_l1, short *ref_l1_l0, short *ref_l1_l1, 
						  const SLICE *slice,  int direct_8x8_inference_flag,int long_term, int slice_type, int is_svc);





/**
This functions decodes from the bitstream reference index.
*/
static __inline int GetCavlcBRefLx(const unsigned char *Data, int *Position, unsigned char *SubMbType, char *RefIdxLx, int NumRefIdxLx, int PredLx){

	int mbPartIdx;
	for ( mbPartIdx = 0 ; mbPartIdx < 4 ; mbPartIdx++ )         {
		char sub_mb_type = SubMbType[mbPartIdx];
		if ((sub_mb_b_name[sub_mb_type] != B_direct) &&	(sub_mb_b_name[sub_mb_type] != PredLx) )  {
			RefIdxLx [mbPartIdx] = (char ) read_te(Data, Position, NumRefIdxLx);
		}else {
			RefIdxLx [mbPartIdx] = 0;
		}
	}

#ifdef ERROR_DETECTION
	//error detection
	if(ErrorsCheckSubRefLx(RefIdxLx, NumRefIdxLx)){
		return 1;
	}
#endif
	return 0;
}
#endif
