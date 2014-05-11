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


#ifndef VL_CABAC
#define VL_CABAC



#include "type.h"
#include "data.h"
#include "cabac.h"
#include "cavlc.h"
#include "mb_pred.h"
#include "decode_mb_I.h"



char mb_pred_cabac ( CABACContext *c, unsigned char *state, SLICE *slice, RESIDU *CurrResidu, DATA *macroblock, 
					short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1, 
					short mvl0_cache[][2], short mvl1_cache[][2], short *refl0_cache, short *refl1_cache);

char mb_pred_P_cabac (CABACContext *c, unsigned char *state, SLICE *slice, RESIDU *CurrResidu, short mv_cache_l0[][2], 
					  short *ref_cache_l0, short mvl0_cache[][2], short refl0_cache[]);

char sub_mb_pred_cabac (int *dct_allowed, CABACContext *c, unsigned char *state, SLICE *slice, RESIDU *Current_residu, 
						short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1, 
						short mvl0_cache[][2], short mvl1_cache[][2], short *refl0_cache, short *refl1_cache, 
						short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1, int direct_8x8_inference_flag, 
						int long_term, int slice_type, int is_svc);

char sub_mb_P_cabac ( int *dct_allowed, CABACContext *c, unsigned char *state, SLICE *slice, RESIDU *Current_residu, 
					 short mv_cache_l0[][2], short *ref_cache_l0, short mv_l0_cache[][2], short *refl0_cache);


void get_mv_neighbourhood( int *mva, int *mvb, short mv_cache[][2], int sub_mb_type, int mbPartIdx, int SubMbPart, int compIdx, int slice_type);


int get_cabac_cbf_ctx( unsigned char non_zero_count_cache[],int cat, int idx, int left_cbp, int top_cbp );

void mb_pred_intra_cabac			( CABACContext *c, unsigned char *state, short *PredMode, short *intra4x4_pred_mode_cache);
void mb_pred_intra_8x8_cabac		( CABACContext *c, unsigned char *state, short *PredMode, short *intra4x4_pred_mode_cache);
int decode_cabac_mb_cbp_chroma		( CABACContext *c, unsigned char *state, CABAC_NEIGH *neigh);
int decode_cabac_mb_dqp				( CABACContext *c, unsigned char *state, CABAC_NEIGH *neigh, int mb_type_left);
unsigned char decode_cabac_mb_chroma_pre_mode ( CABACContext *c, unsigned char *state, int AvailMask, CABAC_NEIGH *neigh) ;
int decode_cabac_mb_cbp_luma		( CABACContext *c, unsigned char *state, int AvailMask, CABAC_NEIGH *neigh, int mb_x, int mb_y);
int decode_cabac_mb_skip			( CABACContext *c, unsigned char *state, int AvailMask, unsigned char *SliceTable,  RESIDU *CurrResidu, int slice_type, int mb_xy, int mb_stride);
unsigned char decode_cabac_mb_ref				( CABACContext *c, unsigned char *state, int refa, int refb);
void decode_significance			( CABACContext *c, unsigned char *state, int index[], int significant_coeff_ctx_base[], 
									 int last_coeff_ctx_base[], int coeff_count, int max_coeff, int coefs, int sig_off, int last_off );
int residual_block_cabac			( CABACContext *c, unsigned char *state, short *coeffLevel, unsigned char non_zero_count_cache[], 
									 const int *scan_table, CABAC_NEIGH *neigh, int cat, int n, int max_coeff, int *cbp, int offset );
void residual_cabac					( CABACContext *c, unsigned char *state, RESIDU *CurrResidu, CABAC_NEIGH *neigh,  unsigned char non_zero_count_cache [], int *cbp);
short decode_cabac_mb_mvd				( CABACContext *c, unsigned char *state, int mva, int mvb,int l );






static __inline int decode_cabac_mb_transform_size(CABACContext *c, unsigned char *state, DATA *Tab_block, int AvailMask, int mb_xy, int mb_stride ){


	int neighour_8x8 = 0; 
	if ( (AvailMask & 0x01) && Tab_block[mb_xy - 1] . Transform8x8 == INTRA_8x8){
		neighour_8x8++;
	}
	if ( ((AvailMask >> 1) & 0x01) && Tab_block[mb_xy - mb_stride] . Transform8x8  == INTRA_8x8){
		neighour_8x8++;
	}

	return get_cabac( c, &state[399 + neighour_8x8] );
}



#endif
