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


#ifndef DIRECT_PRED_H
#define DIRECT_PRED_H

#include "type.h"
#include "data.h"
#include "motion_vector.h"


void spatial_direct_prediction(short mv_cache_l0[][2], short mv_cache_l1[][2], short *mv_l0, short mv_l1[], short *ref_cache_l0
		, short *ref_cache_l1, short *ref_l1_l0, short *ref_l1_l1, int mbPartIdx, int b_stride	
		, int b8_stride, int direct_8x8_inference_flag, int long_term, int is_svc, int slice_type);



void temporal_direct_prediction(short mv_cache_l0[][2], short mv_cache_l1[][2], short *mv_l0
		, short *ref_cache_l0, short *ref_cache_l1, short *ref_l1_l0, short *ref_l1_l1
		, const SLICE *Slice, int MbParIdx, int direct_8x8_inference_flag, int is_svc , const int slice_type);



void temporal_direct_prediction_l0(short mv_cache_l0[][2], short *mv_l0
		, short *ref_cache_l0, short *ref_l1_l0, short *ref_l1_l1, const SLICE *Slice
		, int MbParIdx, int direct_8x8_inference_flag, int is_svc, const int slice_type);

void temporal_direct_prediction_l1(short mv_cache_l0[][2], short *mv_l0
		, short *ref_cache_l0, short *ref_l1_l0, short *ref_l1_l1, const SLICE *Slice
		, int MbParIdx, int direct_8x8_inference_flag, int is_svc, const int slice_type);



void spatial_direct_prediction_l0(short mv_cache_l0[][2], short *mv_l0, short mv_l1[], short *ref_cache_l0, short *ref_cache_l1
		, short *ref_l1_l0, short *ref_l1_l1, int mbPartIdx, int b_stride	
		, int b8_stride, int direct_8x8_inference_flag, int long_term, int is_svc, int slice_type);

void spatial_direct_prediction_l1(short mv_cache_l1[][2], short *mv_l0, short mv_l1[], short *ref_cache_l0, short *ref_cache_l1
		, short *ref_l1_l0, short *ref_l1_l1, int mbPartIdx	, int b_stride	
		, int b8_stride, int direct_8x8_inference_flag, int long_term, int is_svc, int slice_type);
#endif
