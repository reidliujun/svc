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




#ifndef SLICE_B_H
#define SLICE_B_H


#include "svc_type.h"
#include "svc_data.h"


void Decode_B_svc ( const int size_Mb, SPS *ai_pstSps, PPS *ai_pstPps, SLICE *Slice, NAL *Nal, unsigned char *SliceTable, 
				   const DATA *ai_tstTab_Block, RESIDU *AllResidu, const MAIN_STRUCT_PF *vectors, LIST_MMO *ai_pstRefPicListL0,	
				   LIST_MMO *ai_pstRefPicListL1, LIST_MMO *Current_pic, W_TABLES *quantif, SVC_VECTORS *svc_vectors, short *px, 
				   short *py, short *Upsampling_tmp, short *xk16, short *xp16, short *yk16, short *yp16, short *aio_tiMv_l0, 
				   short *aio_tiMv_l1, short *aio_tiRef_l0, short *aio_tiRef_l1, unsigned char *DpbLuma, 
				   unsigned char *DpbCb, unsigned char *DpbCr, short *luma_residu, short *Cb_residu, short *Cr_residu);

#endif
