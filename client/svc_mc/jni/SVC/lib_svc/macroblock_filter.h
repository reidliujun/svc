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


#ifndef MACRO_FILTER_H
#define MACRO_FILTER_H


#include "svc_type.h"



void UpsamplinfFilterMb( unsigned char *image, unsigned char *image_Cb , unsigned char *image_Cr, const PPS *pps, 
			   short mb_xy, short PicWidthInMbs, const int transform_8x8_size , int spatial_scalable_flg, 
			   int DeblockingFilterIdc, int AlphaOffset, int BetaOffset, const short x, const short y, const RESIDU *residu) ;




void filter_mb_svc( unsigned char *image, unsigned char *image_Cb , unsigned char *image_Cr, SLICE *slice, PPS *pps, 
				   unsigned char *SliceTable, short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, 
				   short *ref_cache_l1,  short mb_xy,  short PicWidthInMbs, short poc_l0[], short poc_l1[], 
				   const short x, const short y, RESIDU *residu, const NAL *Nal, short Pass, unsigned char ProfileIdc);
#endif
