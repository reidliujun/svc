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

#ifndef LOOP_FILTER_H
#define LOOP_FILTER_H

#include "data.h"
#include "type.h"
#include "filter.h"
#include "fill_caches.h"
#include "fill_caches_cabac.h"

#ifdef TI_OPTIM
	#define GetBoudaryStrenght GetBoudaryStrenght_TI
#else
	#define GetBoudaryStrenght GetBoudaryStrenght_C
#endif


void filter_mb_B( unsigned char *image, unsigned char *image_Cb , unsigned char *image_Cr, SLICE *slice, PPS *pps, 
				 unsigned char *SliceTable, short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, 
				 short *ref_cache_l1,  int mb_xy,  short PicWidthInMbs, short poc_l0[], short poc_l1[], 
				 const int transform_8x8_size, const int x, const int y, RESIDU *residu)  ;



void filter_B( SPS *Sps, PPS *pps,SLICE *slice, LIST_MMO *Current_pic, unsigned char *SliceTable, DATA *Tab_Block, 
			  short *aio_tiMv_l0, short *aio_tiMv_l1, short *aio_tiRef_l0, short *aio_tiRef_l1, RESIDU *residu, 
			  unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr);

char GetBoudaryStrenght( short mv_cache_l0[][2], short mv_cache_l1[][2], short *ref_cache_l0, short *ref_cache_l1, 
					   short poc_l0[], short poc_l1[], int b_idx, int bn_idx, int slice_type, int mb_type, int mbm_type);
#endif
