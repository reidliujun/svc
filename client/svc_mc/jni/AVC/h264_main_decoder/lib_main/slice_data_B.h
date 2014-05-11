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

#include "type.h"
#include "data.h"
#include "decode_mb_I.h"
#include "decode_mb_P.h"
#include "fill_caches.h"
#include "quantization.h"
#include "neighbourhood.h"
#include "SliceGroupMap.h"
#include "slice_data_cabac.h"
#include "fill_caches_cabac.h"
#include "interpolation_main.h"




void slice_data_B ( const SPS *ai_stSps, const PPS *ai_stPps, SLICE *ai_stSlice, const unsigned char  *ai_tSliceTable, 
				   RESIDU *picture_residu, MAIN_STRUCT_PF *main_vectors, LIST_MMO *ai_pRefPicListL0, 
				   LIST_MMO *ai_pRefPicListL1, short *aio_tMv_l0, short *aio_tMv_l1, short *aio_tref_l0, 
				   short *aio_tref_l1, unsigned char *aio_tDpb_luma, unsigned char *aio_tDpb_Cb, unsigned char *aio_tDpb_Cr,
				   unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, const W_TABLES *quantif);

void non_IDR_main ( SPS *ai_stSps, PPS *ai_stPps, SLICE *ai_stSlice, unsigned char *ai_tSliceTable, 
				   RESIDU *picture_residu, MAIN_STRUCT_PF *main_vector, LIST_MMO *ai_pRefPicListL0, LIST_MMO *ai_pRefPicListL1, 
				   LIST_MMO *Current_pic, const W_TABLES *quantif, short *aio_tMv_l0, short *aio_tMv_l1, short *aio_tref_l0, 
				   short *aio_tref_l1, unsigned char *aio_tDpb_luma, unsigned char *aio_tDpb_Cb, unsigned char *aio_tDpb_Cr);
