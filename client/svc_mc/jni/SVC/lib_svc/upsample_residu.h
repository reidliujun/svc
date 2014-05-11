
/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric Blestel <mblestel@insa-rennes.Fr>
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


#ifndef UPSAMPLE_RES_H
#define UPSAMPLE_RES_H


#include "type.h"
#include "svc_type.h"


void Upsample_residu(const int layer_inc, short first_mb_in_slice, const NAL *Nal,const  SPS *sps_id, 
					 const RESIDU *current_residu, const RESIDU *base_residu, short *x16, short *y16, 
					 short *xk16, short *xp16, short *yk16, short *yp16, short *luma_tmp, short *luma_in, 
					 short *Cb_in, short *Cr_in, const unsigned char *ai_tiMbToSliceGroupMap);

void upsample_luma_residu( short *luma_in, short *luma_out, int transform_8x8_size, const NAL *Nal);
void upsample_chroma_residu( short *luma_in, short *luma_out, const NAL *Nal);
void resize_residu(const  SPS *sps, RESIDU *residu, short *luma_residu, short *Cb_residu, short *Cr_residu, 
				   int x_mbPartIdx, int y_mbPartIdx,int transform_8x8_size, const short base_Width, 
				   const short base_Height,const short curr_Width,const short curr_Height, int *cbp);




/**
Update blk4x4 bloc mode.
*/
static __inline void update_bl4x4_mode ( RESIDU *current_residu, RESIDU *base_residu, int x_mbPartIdx, int y_mbPartIdx, short *base_index){



	int i;
	int blk_0[4];
	int blk_1[4];
	int blk_2[4];
	int blk_3[4];
	int LumaIdx = x_mbPartIdx + 4 * y_mbPartIdx;

	
	//First line of the macroblock
	for ( i = 0; i < 4; i++){
		if((x_mbPartIdx + (base_index[i] >> 1)) > 3){
			blk_0[i] = (-((base_residu[1] . blk4x4 & (1 << (LumaIdx + (base_index[i] >> 1)- 4))) >> (LumaIdx + (base_index[i] >> 1) - 4)));
		}else {
			blk_0[i] = (-((base_residu -> blk4x4 & (1 << (LumaIdx + (base_index[i] >> 1)))) >> (LumaIdx + (base_index[i] >> 1))));
		}
	}

	for ( i = 0; i < 4; i++){
		int decal = LumaIdx + (base_index[4 + i] >> 2) + (base_index[i] >> 1);
		if((x_mbPartIdx + (base_index[i] >> 1)) > 3){
			decal -= 4;
			blk_1[i] = (-((base_residu[1] . blk4x4 & (1 << decal)) >> decal));
		}else {
			blk_1[i] = (-((base_residu -> blk4x4 & (1 << decal)) >> decal));
		}
	}


	for ( i = 0; i < 4; i++){
		int decal = LumaIdx + (base_index[8 + i] >> 2) + (base_index[i] >> 1);
		if((x_mbPartIdx + (base_index[i] >> 1)) > 3){
			decal -= 4;
			blk_2[i] = (-((base_residu[1] . blk4x4 & (1 << decal)) >> decal));
		}else {
			blk_2[i] =(-((base_residu -> blk4x4 & (1 << decal)) >> decal));
		}
	}

	for ( i = 0; i < 4; i++){
		int decal = LumaIdx + (base_index[12 + i] >> 2) + (base_index[i] >> 1);
		if((x_mbPartIdx + (base_index[i] >> 1)) > 3){
			decal -= 4;
			blk_3[i] = (-((base_residu[1] . blk4x4 & (1 << decal)) >> decal));
		}else {
			blk_3[i] =(-((base_residu -> blk4x4 & (1 << decal)) >> decal));
		}
	}


	current_residu -> blk4x4 = (blk_0[0] & 0x01)   + (blk_0[1] & 0x02)	 + (blk_1[0] & 0x10)   + (blk_1[1] & 0x20) +
							   (blk_0[2] & 0x04)   + (blk_0[3] & 0x08)   + (blk_1[2] & 0x40)   + (blk_1[3] & 0x80) +
							   (blk_2[0] & 0x0100) + (blk_2[1] & 0x0200) + (blk_3[0] & 0x1000) + (blk_3[1] & 0x2000) +
							   (blk_2[2] & 0x0400) + (blk_2[3] & 0x0800) + (blk_3[2] & 0x4000) + (blk_3[3] & 0x8000);
}



/**
Set luma position.
*/
static __inline void init_residu_luma_tab(int curr_width, int Scale, int Add, int shift, int deltaX, short *x16, short *k16, short *p16){
		
	short i, p, i1;
	for(i = 0; i < curr_width; i++){
		short k = (short) (((i*Scale + Add) >> (shift - 4)) - deltaX);
		if(k < 0)
			k = 0;
		i1 = k >> 4;
		k -= i1 * 16;
		p = ( k > 7 && (i1 + 1) < curr_width ) ? ( i1 + 1 ) : i1;
		p = p < 0 ? 0 : p;
		x16[i] = i1; k16[i] = k; p16[i] = p;
	}
}
#endif
