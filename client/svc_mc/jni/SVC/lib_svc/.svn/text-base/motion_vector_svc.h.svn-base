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



#ifndef MOTION_H
#define MOTION_H


#include "svc_type.h"


short SvcSkipMotionVector(short mv_cache[][2], const short MvdLX[2], const short *ref_cache, short mvLX[2], int index8, int partwidth);


void get_base_P_dyadic(short mv_cache [][2], short *ref_cache, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
					   unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, short x, short y, 
					   short PicWidthInPix, short PicHeightInPix, LIST_MMO *RefPicListL0, short *mv_l0, short *ref_base, 
					   REFPART *RefPartition, const NAL *Nal, const interpol_4x4 *interpol_4x4);

void get_base_P_non_dyadic(short mv_cache [][2], short *ref_cache, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
						   unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, short x, short y, 
						   short PicWidthInPix, short PicHeightInPix, LIST_MMO * RefPicListL0, short *ref_base, const NAL *Nal, 
						   const interpol_4x4 *interpol_4x4, REFPART *RefPartition);


void get_base_B_mv(short mv_cache_l0 [][2], short mv_cache_l1 [][2], short ref_cache_l0 [], short ref_cache_l1 [], 
				   unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
				   unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
				   const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
				   LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1, REFPART *RefPartitionL0, 
				   short *mv_l0, short mv_l1[], short *ref_l0, short *ref_l1, const NAL *Nal,const interpol_4x4 *interpol_4x4);

void GetBaseBMvNonDiadic(short mv_cache_l0 [][2], short mv_cache_l1 [][2], short ref_cache_l0 [], short ref_cache_l1 [], 
						 unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
						 unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
						 const short x, const short y, const short PicWidthInPix, const short PicHeightInPix, 
						 LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1, REFPART *RefPartitionL0, 
						 short *ref_l0, short *ref_l1, const NAL *Nal,const interpol_4x4 *interpol_4x4);
#endif
