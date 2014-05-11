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

#ifndef B_BL_H
#define B_BL_H


void Decode_B_BL(const PPS * Pps, const SPS * Sps, const NAL *Nal, RESIDU *residu,  RESIDU *base_residu,const STRUCT_PF *baseline_vector
				 , const W_TABLES *quantif, LIST_MMO * ai_pstRefPicListL0, LIST_MMO * ai_pstRefPicListL1 , short x, short y, short x_base
				 ,	short y_base, short x_mbPartIdx, short y_mbPartIdx, short PicWidthInPix, REFPART RefPartitionL0[16]
				 , short *ref_cache_l0,  short *ref_cache_l1, short mv_cache_l0 [][2], short mv_cache_l1 [][2]
				 , short *mv_l0, short *mv_l1, short *ref_base_l0, short *ref_base_l1, unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr
			     , unsigned char Y[ ], unsigned char U[ ], unsigned char V[ ], short *Residu_Luma, short *Residu_Cb, short *Residu_Cr);
#endif
