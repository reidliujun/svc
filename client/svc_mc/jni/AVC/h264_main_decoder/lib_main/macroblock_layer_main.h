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

#ifndef MACRO_MAYER_MAIN_H
#define MACRO_MAYER_MAIN_H

#include "type.h"

char macroblock_layer_B (const PPS *Pps, RESIDU *picture_residu,  const unsigned char *ai_pcData,  int * aio_piPosition, SLICE *Slice, 
						DATA *aio_tstBlock, const VLC_TABLES * Vlc, unsigned char *NonZeroCountCache, unsigned char *SliceTable, 
						const short ai_iMb_x, const short ai_iMb_y, int direct_8x8_inference_flag, int long_term, int slice_type, int is_svc, 
						unsigned char *last_QP, short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1, 
						short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1);


char macroblock_B_partitionning (const PPS *Pps, RESIDU *picture_residu, const unsigned char *ai_pcData, int *aio_piPosition, SLICE *Slice, 
								DATA *aio_pstBlock, const VLC_TABLES *Vlc, unsigned char *NonZeroCountCache, unsigned char  *SliceTable, 
								const short ai_iMb_x, const short ai_iMb_y, int direct_8x8_inference_flag, int long_term, int slice_type, int is_svc, 
								unsigned char *last_QP, int iCurrMbAddr, short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1, 
								short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1);
#endif
