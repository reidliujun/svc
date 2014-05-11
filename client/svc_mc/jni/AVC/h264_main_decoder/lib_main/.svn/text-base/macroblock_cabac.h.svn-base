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


#ifndef MACRO_CABAC_P
#define MACRO_CABAC_P


#include "data.h"
#include "cavlc.h"
#include "cabac.h"
#include "bitstream.h"
#include "vlc_cabac.h"
#include "fill_caches_cabac.h"



int decode_cabac_intra_mb_type ( CABACContext *c, unsigned char *state, RESIDU *CurrResidu, int intra_slice, const int mb_stride);
int decode_cabac_mb_type	   ( CABACContext *c, unsigned char *state, RESIDU *CurrResidu, unsigned char *SliceTable,
								 int mb_xy,  int slice_type, const int mb_stride);


char macroblock_layer_I_cabac (RESIDU *picture_residu, SLICE *slice, DATA *Tab_block, unsigned char non_zero_count_cache [], 
							  unsigned char *SliceTable, int iCurrMbAddr, CABACContext *cabac, unsigned char *cabac_state, CABAC_NEIGH *neigh, 
							  short x, short y, int constrained_intra_pred_flag, int dct_allowed, unsigned char *last_QP, int *mb_qp_delta);



char macroblock_layer_P_cabac (RESIDU *picture_residu, SLICE *slice, DATA *Tab_block, unsigned char non_zero_count_cache [], 
							  unsigned char *SliceTable, int iCurrMbAddr, CABACContext *cabac, unsigned char *cabac_state, CABAC_NEIGH *neigh, 
							  short mv_cache_l0[][2], short *ref_cache_l0, short x, short y, int constrained_intra_pred_flag, 
							  int dct_allowed, unsigned char *last_QP, int *mb_qp_delta, short mvl0_cache[][2], short refl0_cache[]);


char macroblock_layer_B_cabac (RESIDU *picture_residu, SLICE *slice, DATA *Tab_block, unsigned char non_zero_count_cache [], 
							   unsigned char *SliceTable, int iCurrMbAddr, CABACContext *cabac, unsigned char *cabac_state, CABAC_NEIGH *neigh, 
							   short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1, 
							   short x, short y, int constrained_intra_pred_flag, int direct_8x8_inference_flag, int dct_allowed, 
							   unsigned char *last_QP, int *mb_qp_delta, int is_svc, short mvl0_cache[][2], short mvl1_cache[][2], 
							   short *refl0_cache, short *refl1_cache, int long_term, int slice_type, short *mvl1_l0, 
							   short *mvl1_l1, short *refl1_l0, short *refl1_l1);

#endif
