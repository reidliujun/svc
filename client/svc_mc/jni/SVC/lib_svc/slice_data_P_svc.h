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


#ifndef SLICE_P_H
#define SLICE_P_H


#include "svc_type.h"
#include "svc_data.h"



void Decode_P_svc ( const int size_Mb,	const SPS *ai_pstSps, const PPS *ai_pstPps,const SLICE *Slice, const NAL *Nal, 
				   const unsigned char *SliceTable, const DATA *ai_tstTab_Block, RESIDU *all_residu, 
				   const STRUCT_PF *baseline_vector, LIST_MMO *ai_pstRefPicListL0, LIST_MMO *Current_pic, 
				   W_TABLES *quantif, SVC_VECTORS *svc_vectors, short *px, short *py, short *Upsampling_tmp, short *xk16, 
				   short *xp16, short *yk16, short *yp16, short *aio_tiMv, short aio_tiRef[ ], 
				   unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				   short *luma_residu, short *Cb_residu, short *Cr_residu);


void GetBasePrediction(const NAL *Nal, const SLICE *Slice, const PPS *Pps, const SPS *Sps, const SPS *Sps_id, 
					   RESIDU *CurrResidu, RESIDU *BaseResidu, const int size_Mb, int iCurrMbAddr, 
					   const unsigned char *ai_tiMbToSliceGroupMap, short *x16, short *y16, short *xk16, 
					   short *xp16, short *yk16, short *yp16, short *Upsampling_tmp, 
					   unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV, 
					   short *Luma_Residu, short *Cb_Residu, short *Cr_Residu);

#endif


