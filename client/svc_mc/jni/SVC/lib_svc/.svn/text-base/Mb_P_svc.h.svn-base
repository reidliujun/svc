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



#ifndef MB_P_SVC_H
#define MB_P_SVC_H

#include "svc_type.h"




void P_svc_init( P_svc pf[4], Sub_P_svc pf_sub[4]);

void P_svc_8x8( short mv_cache [][2],	short *ref_cache, const RESIDU *CurrResidu, const DATA *Block, const Sub_P_svc decode_sub[]
	, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
	, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr
	, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
	, const NAL *Nal, LIST_MMO *   RefPicListL0
	, short *mv_l0,  short *ref_bas_l0, const STRUCT_PF *vectors, REFPART RefPartition[16]) ;

void P_svc_8x16 ( short mv_cache [][2], short *ref_cache, const RESIDU *CurrResidu, const DATA *Block, const Sub_P_svc decode_sub[]
	, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
	, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr
	, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
	, const NAL *Nal, LIST_MMO *   RefPicListL0
	, short *mv_l0,  short *ref_bas_l0, const STRUCT_PF *vectors, REFPART RefPartition[16]);

void P_svc_16x8 ( short mv_cache [][2],	short *ref_cache, const RESIDU *CurrResidu, const DATA *Block, const Sub_P_svc decode_sub[]
	, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
	, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr
	, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
	, const NAL *Nal, LIST_MMO *   RefPicListL0
	, short *mv_l0,  short *ref_bas_l0, const STRUCT_PF *vectors, REFPART RefPartition[16]);

void P_svc_16x16 ( short mv_cache [][2],	short *ref_cache, const RESIDU *CurrResidu, const DATA *Block, const Sub_P_svc decode_sub[]
	, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
	, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr
	, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
	, const NAL *Nal, LIST_MMO *   RefPicListL0
	, short *mv_l0,  short *ref_bas_l0, const STRUCT_PF *vectors, REFPART RefPartition[16]);

void P_svc_skip ( short mv_cache [][2],	short *ref_cache, const RESIDU *CurrResidu, const DATA *Block, const Sub_P_svc decode_sub[]
	, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
	, unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr
	, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
	, const NAL *Nal, LIST_MMO *   RefPicListL0
	, short *mv_l0,  short *ref_bas_l0, const STRUCT_PF *vectors, REFPART RefPartition[16]);


void Sub_P_svc_8x8(short mv_cache [][2], short *ref_cache, const DATA *Block, int mbPartIdx, short *mv_l0,  short *ref_bas_l0, const NAL *Nal, REFPART RefPartition[16]);
void Sub_P_svc_8x4(short mv_cache [][2], short *ref_cache, const DATA *Block, int mbPartIdx, short *mv_l0,  short *ref_bas_l0, const NAL *Nal, REFPART RefPartition[16]);
void Sub_P_svc_4x8(short mv_cache [][2], short *ref_cache, const DATA *Block, int mbPartIdx, short *mv_l0,  short *ref_bas_l0, const NAL *Nal, REFPART RefPartition[16]);
void Sub_P_svc_4x4(short mv_cache [][2], short *ref_cache, const DATA *Block, int mbPartIdx, short *mv_l0,  short *ref_bas_l0, const NAL *Nal, REFPART RefPartition[16]);


/**
Allow to set Ref_cache and Mv_cache for one block 4x4. 
This function scales the base layer motion vector to set the current layer one.

*/

static __inline void SetInterParam(const NAL *Nal, short mv_cache[2], short *ref_cache, const short *MvdL0, short *mv_l0, short *ref_bas_l0){

	ref_cache [0] = ref_bas_l0[0];
	mv_cache [0] = MV_CAL(mv_l0[0], Nal -> MvCalx [Nal -> LayerId]) + MvdL0 [0];
	mv_cache [1] = MV_CAL(mv_l0[1], Nal -> MvCaly [Nal -> LayerId]) + MvdL0 [1];
}


/**
Allow to set Ref_cache and Mv_cache for one block 8x4. 
This function scales the base layer motion vector to set the current layer one.

*/
static __inline void SetInterParamHori(const NAL *Nal, short mv_cache[][2], short *ref_cache, const short *MvdL0, short *mv_l0, short *ref_bas_l0){

	ref_cache [0] = ref_cache [1] = ref_bas_l0[0];
	mv_cache [0][0] = mv_cache [1][0] = MV_CAL(mv_l0[0], Nal -> MvCalx [Nal -> LayerId]) + MvdL0 [0];
	mv_cache [0][1] = mv_cache [1][1] = MV_CAL(mv_l0[1], Nal -> MvCaly [Nal -> LayerId]) + MvdL0 [1];
}

/**
Allow to set Ref_cache and Mv_cache for one block 4x8. 
This function scales the base layer motion vector to set the current layer one.

*/
static __inline void SetInterParamVert(const NAL *Nal, short mv_cache[][2], short *ref_cache, const short *MvdL0, short *mv_l0, short *ref_bas_l0){


	ref_cache [0] = ref_cache [8] = ref_bas_l0[0];
	mv_cache [0][0] = mv_cache [8][0] = MV_CAL(mv_l0[0], Nal -> MvCalx [Nal -> LayerId]) + MvdL0 [0];
	mv_cache [0][1] = mv_cache [8][1] = MV_CAL(mv_l0[1], Nal -> MvCaly [Nal -> LayerId]) + MvdL0 [1];
}
#endif
