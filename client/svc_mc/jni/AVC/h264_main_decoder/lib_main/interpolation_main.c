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

#include "type.h"
#include "data.h"
#include "interpolation.h"

/**
This function allows to get the prediction of an non IDR picture. 
//8.4.2.2


@param DpbLuma Table of the reference decoded picture buffer.
@param DpbCb Table of the reference decoded picture buffer.
@param DpbCr Table of the reference decoded picture buffer.
@param Luma_l0 Table of current frame.
@param Luma_l1 Table of current frame.
@param Chroma_Cb_l0 Table of current frame.
@param Chroma_Cb_l1 Table of current frame.
@param Chroma_Cr_l0 Table of current frame.
@param Chroma_Cr_l1 Table of current frame.
@param mvL0 The motion vector are stocked for each 4x4 block of each macroblock.
@param mvL1 The motion vector are stocked for each 4x4 block of each macroblock.
@param ref_cache_l0 A cache table where the reference is stocked for each 4x4 block of each macroblock.
@param ref_cache_l1 A cache table where the reference is stocked for each 4x4 block of each macroblock.
@param x X-coordinate.
@param y Y-coordinate.
@param PicWidthInPix Width in pixel of the current frame.
@param PicHeightInPix Height in pixel of the current frame.
@prama RefPicListL0 List l0 of reference image.
@prama RefPicListL1 List l1 of reference image.
@param interpol Function pointer of 4x4 interpoaltion.
*/
void sample_interpolation_main(unsigned char *DpbLuma, unsigned char *DpbCb,unsigned char *DpbCr, 
							   unsigned char *Luma_l0, unsigned char *Luma_l1 , unsigned char *Chroma_Cb_l0, 
							   unsigned char *Chroma_Cb_l1, unsigned char *Chroma_Cr_l0 , unsigned char *Chroma_Cr_l1, 
							   short mvL0[][2], short mvL1[][2], short *ref_cache_l0, short *ref_cache_l1, short x, short y,
							   const short PicWidthInPix, const short PicHeightInPix , const LIST_MMO *RefPicListL0, 
							   const LIST_MMO * RefPicListL1, const interpol_4x4 *interpol)
{

	int i;


	for( i = 0; i < 16; i++){
		short index8 = SCAN8(i);
		const short Currentx = x + LOCX(i);
		const short Currenty = y + LOCY(i);

		if ( ref_cache_l0[index8] >= 0){
			 Interpolate(ref_cache_l0[index8], Currentx, Currenty, &mvL0[index8][0], 4, PicWidthInPix, PicHeightInPix, RefPicListL0, 
				 &Luma_l0[i << 4], &Chroma_Cb_l0[i << 2], &Chroma_Cr_l0[i << 2], DpbLuma, DpbCb, DpbCr, interpol);
		}

		if ( ref_cache_l1[index8] >= 0){
			 Interpolate(ref_cache_l1[index8], Currentx, Currenty, &mvL1[index8][0], 4, PicWidthInPix, PicHeightInPix, RefPicListL1, 
				 &Luma_l1[i << 4], &Chroma_Cb_l1[i << 2], &Chroma_Cr_l1[i << 2], DpbLuma, DpbCb, DpbCr, interpol);
		}
	}	
}
