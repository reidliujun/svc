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



#ifndef NON_IDR_PICTURE_H
#define NON_IDR_PICTURE_H

#include "type.h"
#include "data.h"
#include "filter.h"


void Slice_data_P( const SPS *Sps, const PPS *Pps,const SLICE *Slice, const unsigned char *SliceTable, 
				  RESIDU *picture_residu, const STRUCT_PF * ai_STRUCT_PF, LIST_MMO *ai_pstRefPicListL0, short *aio_tiMv,
				  short *aio_tiRef, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char  *DpbCr, 
				  unsigned char *aio_tucImage, unsigned char *aio_tucImage_Cb, unsigned char  *aio_tucImage_Cr, const W_TABLES *quantif);



void non_IDR_picture ( const SPS *Sps, const PPS *Pps,const SLICE *Slice, const unsigned char *SliceTable, 
					  RESIDU *picture_residu, const STRUCT_PF *All_PF_struct, LIST_MMO *ai_pstRefPicListL0,
					  LIST_MMO *Current_pic, const W_TABLES *quantif, short *aio_tiMv, short *aio_tiRef, 
					  unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char  *DpbCr);
#endif
