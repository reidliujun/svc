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


#ifndef SLICE_DATA_AVC_H
#define SLICE_DATA_AVC_H


//AVC Files
#include "type.h"
#include "main_data.h"


//SVC Files
#include "svc_type.h"

void Decode_I_avc( const SPS * Sps, const PPS * Pps, const SLICE * ai_pstSlice, const unsigned char *SliceTable, 
				  RESIDU *picture_residu, const STRUCT_PF * ai_struct_pf, 
				  const W_TABLES *quantif, NAL *Nal, unsigned char *Y, unsigned char *U, unsigned char *V);

void Decode_P_avc( const SPS *Sps, const PPS *Pps, const SLICE *ai_pstSlice, const unsigned char *SliceTable, 
				  RESIDU *picture_residu, const STRUCT_PF * ai_struct_pf, LIST_MMO *ai_pstRefPicListL0, 
				  LIST_MMO * Current_pic, const W_TABLES *quantif, const NAL *Nal, short *aio_tiMv, short *aio_tiRef, 
				  unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char  *DpbCr, 
				  short *Residu_Luma, short *Residu_Cb, short *Residu_Cr);


void Decode_B_avc ( SPS *ai_stSps, PPS *ai_stPps, SLICE *ai_stSlice, unsigned char *ai_tSliceTable, 
				   RESIDU *picture_residu, MAIN_STRUCT_PF *main_vectors, LIST_MMO *ai_pRefPicListL0, 
				   LIST_MMO *ai_pRefPicListL1, LIST_MMO *Current_pic, const W_TABLES *quantif, 
				   const NAL *Nal, short Mv_l0[], short Mv_l1[], short ref_l0[], short ref_l1[], 
				   unsigned char *aio_tDpb_luma, unsigned char *aio_tDpb_Cb,unsigned char *aio_tDpb_Cr, 
				   short *Residu_Luma, short *Residu_Cb, short *Residu_Cr);
#endif
