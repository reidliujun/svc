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


#ifndef MB_B_H
#define MB_B_H


#include "svc_type.h"
#include "svc_data.h"





void B_svc_init( B_svc Ptr_decode_B[4], Sub_B_svc Ptr_decode_B_sub[13]);



void B_svc_8x8(short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short *aio_tref_cache_l0,short *aio_tref_cache_l1
						, const RESIDU *CurrResidu, const DATA *aio_pBlock, Sub_B_svc aio_tdecode_sub[]
						, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
						, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr
						, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
						, const NAL *Nal, LIST_MMO *   RefPicListL0, LIST_MMO *  RefPicListL1
						, short *mv_bas_l0, short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1, const STRUCT_PF *vectors
						, REFPART RefPartition[16]);

void B_svc_8x16(short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short *aio_tref_cache_l0, short *aio_tref_cache_l1
						, const RESIDU *CurrResidu, const DATA *aio_pBlock, Sub_B_svc aio_tdecode_sub[]
						, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
						, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr
						, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
						, const NAL *Nal, LIST_MMO *   RefPicListL0, LIST_MMO *  RefPicListL1
						, short *mv_bas_l0, short *mv_bas_l1v, short *ref_bas_l0, short *ref_bas_l1, const STRUCT_PF *vectors
						, REFPART RefPartition[16]);

void B_svc_16x8(short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short *aio_tref_cache_l0, short *aio_tref_cache_l1
						, const RESIDU *CurrResidu, const DATA *aio_pBlock, Sub_B_svc aio_tdecode_sub[]
						, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
						, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr
						, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
						, const NAL *Nal, LIST_MMO *   RefPicListL0, LIST_MMO *  RefPicListL1
						, short *mv_bas_l0, short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1, const STRUCT_PF *vectors
						, REFPART RefPartition[16]);

void B_svc_16x16(short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short *aio_tref_cache_l0, short *aio_tref_cache_l1
						, const RESIDU *CurrResidu, const DATA *aio_pBlock, Sub_B_svc aio_tdecode_sub[]
						, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
						, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr
						, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
						, const NAL *Nal, LIST_MMO *   RefPicListL0, LIST_MMO *  RefPicListL1
						, short *mv_bas_l0, short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1, const STRUCT_PF *vectors
						, REFPART RefPartition[16]);

void B_svc_Skip(short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short *aio_tref_cache_l0, short *aio_tref_cache_l1
						, const RESIDU *CurrResidu, const DATA *aio_pBlock, Sub_B_svc aio_tdecode_sub[]
						, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
						, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr
						, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
						, const NAL *Nal, LIST_MMO *   RefPicListL0, LIST_MMO *  RefPicListL1
						, short *mv_bas_l0, short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1, const STRUCT_PF *vectors
						, REFPART RefPartition[16]);

void Sub_B_svc_4x4 ( short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short *aio_tref_cache_l0, short *aio_tref_cache_l1
						, const DATA *aio_pBlock, int mbPartIdx, int SubMbType
						, short *mv_bas_l0, short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1
						, const NAL *Nal, REFPART RefPartition[16]);


void Sub_B_svc_4x8 ( short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short *aio_tref_cache_l0, short *aio_tref_cache_l1
						, const DATA *aio_pBlock, int mbPartIdx, int SubMbType
						, short *mv_bas_l0, short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1
						, const NAL *Nal, REFPART RefPartition[16]);

void Sub_B_svc_8x4 ( short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short *aio_tref_cache_l0, short *aio_tref_cache_l1
						, const DATA *aio_pBlock, int mbPartIdx, int SubMbType
						, short *mv_bas_l0, short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1
						, const NAL *Nal, REFPART RefPartition[16]);

void Sub_B_svc_8x8 ( short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short *aio_tref_cache_l0, short *aio_tref_cache_l1
						, const DATA *aio_pBlock, int mbPartIdx, int SubMbType
						, short *mv_bas_l0, short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1
						, const NAL *Nal, REFPART RefPartition[16]);
#endif
