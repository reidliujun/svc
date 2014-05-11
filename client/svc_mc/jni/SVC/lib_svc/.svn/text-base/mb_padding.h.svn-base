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

#ifndef MB_PAD
#define MB_PAD

#include "clip.h"
#include "svc_type.h"

//Mb size
#define LumaBlockSize 16
#define ChromaBlockSize 8

//Border Size
#define LeftBorder 8
#define RightBorder 8
#define TopBorder 4
#define BottomBorder 8
#define LumaStride (LeftBorder + LumaBlockSize + RightBorder)
#define ChromaStride ((LeftBorder >> 1) + ChromaBlockSize + (RightBorder >> 1))




void edges_limit(const NAL *Nal, int AvailMask, short x_mbPartIdx, short y_mbPartIdx, 
				 unsigned char tmp_luma[], unsigned char base_luma[], 
				 unsigned char tmp_Cb[], unsigned char base_Cb[], 
				 unsigned char tmp_Cr[], unsigned char base_Cr[]);


static __inline void merge(short ysize, short xsize, short PicWidthInPix, unsigned char pix[], unsigned char dst[], int corner, int stride, int add){
	
	
	int x, y, xo;
	unsigned char pPelH[RightBorder + 1];
	unsigned char pPelV[BottomBorder + 1];

	for( x = 0; x <= xsize; x++ ){
		pPelH[x] = pix[(x - 1) * add];
	}

	for( y = 0; y <= ysize; y++ ){
		pPelV[y] = pix[y*PicWidthInPix - add];
	}

	if( !corner ){
		pPelV[0] = pPelH[0] = (pPelH[1] + pPelV[1] + 1) >> 1;
	}

	for( y = 0; y < ysize; y++, dst += stride ) {
		for( xo = 0, x = 0; x < xsize; x++, xo += add )  {
			const int iOffset = x - y;
			if( iOffset > 0 )  {
				dst[xo] = (pPelH[iOffset - 1] + 2*pPelH[iOffset] + pPelH[iOffset + 1] + 2) >> 2;

			}  else if( iOffset < 0 ) {
				dst[xo] = (pPelV[-iOffset - 1] + 2*pPelV[-iOffset] + pPelV[-iOffset + 1] + 2) >> 2;

			}   else {
				//dst[xo] = (pPelH[x + 1] + 2*pPelV[0] + pPelV[y + 1] + 2) >> 2;
				dst[xo] = (pPelH[1] + 2*pPelV[0] + pPelV[1] + 2) >> 2;  //Fix Danny  
			}
		}
	}
}




static __inline void copy_1d(short y_size, short x_size, short src_width, short dst_width, short src_inc, unsigned char *src, unsigned char *dst){

	int i,j;
	for(i = 0; i < y_size; i++){
		for(j = 0; j < x_size; j++){
			dst[i * dst_width + j] = src[i * src_width + j * src_inc];
		}
	}
}

static __inline void copy_2d(short y_size, short x_size, short src_width, short dst_width, unsigned char *src, unsigned char *dst){

	int i,j;
	for(i = 0; i < y_size; i++){
		for(j = 0; j < x_size; j++){
			dst[i * dst_width + j] = src[j + i * src_width];
		}
	}
}


static __inline void add_clip_2d(short y_size, short x_size, short src_width, short dst_width, short *src, unsigned char *dst){

	int i,j;
	for(i = 0; i < y_size; i++){
		for(j = 0; j < x_size; j++){
			dst[i * dst_width + j] = (unsigned char) CLIP255( dst[i * dst_width + j] + src[j + i * src_width]);
		}
	}
}
#endif
