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
#include "clip.h"
#include "interpolation_8x8.h"




/**
This function allows to get the 8x8 chrominance prediction of an non IDR picture. 
//8.4.2.2


@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
@param refPicLXCb Table of the reference decoded picture buffer.
@param refPicLXCr Table of the reference decoded picture buffer.
@param xFracl Horizontal fractionnal part.
@param yFracl Vertical fractionnal part.
@param PicWidthSamples Stride of the reference buffer.
*/

void chroma_SampleInterpolation8x8(unsigned char *image_Cb, unsigned char *image_Cr, unsigned char *refPicLXCb, 
								   unsigned char *refPicLXCr, short xFracl, short yFracl, int PicWidthSamples)
{


	short cst1 = xFracl * yFracl;
	short cst2 = cst1 - ((xFracl + yFracl)<<3) + 64;
	short cst3 = (xFracl << 3) - cst1;
	short cst4 = (yFracl << 3) - cst1;
	int j,i;
	unsigned char *ptr_img_Cb = refPicLXCb;
	unsigned char *ptr_img_Cr = refPicLXCr;

	for(j = 0; j < 4; j++)	{
		for(i = 0; i < 4; i++){
			unsigned char A = *ptr_img_Cb;
			unsigned char B = *(ptr_img_Cb + 1);
			unsigned char C = *(ptr_img_Cb + PicWidthSamples);
			unsigned char D = *(ptr_img_Cb + 1 + PicWidthSamples);
			unsigned char E = *ptr_img_Cr;
			unsigned char F = *(ptr_img_Cr + 1);
			unsigned char G = *(ptr_img_Cr + PicWidthSamples);
			unsigned char H = *(ptr_img_Cr + 1 + PicWidthSamples);

			image_Cb[(j * PicWidthSamples) + i] = (unsigned char) ((cst2*A + cst3*B + cst4*C + cst1*D + 32) >> 6);
			image_Cr[(j * PicWidthSamples) + i] = (unsigned char) ((cst2*E + cst3*F + cst4*G + cst1*H + 32) >> 6);
			*ptr_img_Cb++;
			*ptr_img_Cr++;
		}
		ptr_img_Cb += PicWidthSamples - 4;
		ptr_img_Cr += PicWidthSamples - 4;

	}	
}








/**
This function allows to execute the 8x8 prediction. 
//8.4.2.2


@param DpbLuma Table of the reference decoded picture buffer.
@param DpbCb Table of the reference decoded picture buffer.
@param DpbCr Table of the reference decoded picture buffer.
@param image Table of current frame.
@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
@param mvL0 The motion vector of a 8x8 block.
@param PicWidthInPix Width in pixel of the current frame.
@param interpol Function pointer of 8x8 interpoaltion.
*/
void interpolation_8x8(unsigned char *DpbLuma, unsigned char *DpbCb,unsigned char *DpbCr, 
					   unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
					   short mvL0[2], short PicWidthInPix, const interpol_8x8 *interpol)
{


	//Luminance interpolation 
	short xFracl = mvL0[0] & 3;
	short yFracl = mvL0[1] & 3;
	interpol[xFracl + (yFracl << 2)](image, DpbLuma, PicWidthInPix, PicWidthInPix);


	//Chrominance interpolation
	xFracl = mvL0[0] & 7;
	yFracl = mvL0[1] & 7;
	chroma_SampleInterpolation8x8(image_Cb, image_Cr, DpbCb, DpbCr, xFracl, yFracl, PicWidthInPix >> 1);
}



/**
This function allows to get the prediction of an non IDR picture. 
//8.4.2.2


@param DpbLuma Table of the reference decoded picture buffer.
@param DpbCb Table of the reference decoded picture buffer.
@param DpbCr Table of the reference decoded picture buffer.
@param image Table of current frame.
@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
@param mvLX The motion vector are stocked for each 4x4 block of each macroblock.
@param ref_cache A cache table where the reference is stocked for each 4x4 block of each macroblock.
@param x X-coordinate.
@param y Y-coordinate.
@param PicWidthInPix Width in pixel of the current frame.
@param PicHeightInPix Height in pixel of the current frame.
@prama RefPicListL0 List l0 of reference image.
@param interpol Function pointer of 8x8 interpoaltion.
*/
void SampleInterpolation8x8(unsigned char *DpbLuma, unsigned char *DpbCb, unsigned char *DpbCr, 
							unsigned char *Luma_l0, unsigned char *Chroma_Cb_l0, unsigned char *Chroma_Cr_l0, 
							short mvL0[][2], short *ref_cache_l0, int x, int y, short PicWidthInPix, 
							short PicHeightInPix, const LIST_MMO *RefPicListL0, const interpol_8x8 *interpol)
{



	//Recovery of  the prediction
	int address = RefPicListL0[ref_cache_l0[12]] . MemoryAddress;	
	int xIntl = CLIP3(-11, PicWidthInPix - 27, x + (mvL0[12][0] >> 2));
	int yIntl = CLIP3(-11, PicHeightInPix - 27, y + (mvL0[12][1] >> 2));

	interpolation_8x8(&DpbLuma[address + xIntl + yIntl * PicWidthInPix], 
		&DpbCb[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
		&DpbCr[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
		Luma_l0, Chroma_Cb_l0, Chroma_Cr_l0, mvL0[12], PicWidthInPix, interpol);


	address = RefPicListL0[ref_cache_l0[14]] . MemoryAddress;	
	xIntl = CLIP3(-11, PicWidthInPix - 27, x + 8 +(mvL0[14][0] >> 2));
	yIntl = CLIP3(-11, PicHeightInPix - 27, y + (mvL0[14][1] >> 2));

	interpolation_8x8(&DpbLuma[address + xIntl + yIntl * PicWidthInPix], 
		&DpbCb[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
		&DpbCr[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
		&Luma_l0[8], &Chroma_Cb_l0[4], &Chroma_Cr_l0[4], mvL0[14], PicWidthInPix, interpol);

	address = RefPicListL0[ref_cache_l0[28]] . MemoryAddress;	
	xIntl = CLIP3(-11, PicWidthInPix - 27, x + (mvL0[28][0] >> 2));
	yIntl = CLIP3(-11, PicHeightInPix - 27, y  + 8 + (mvL0[28][1] >> 2));

	interpolation_8x8(&DpbLuma[address + xIntl + yIntl * PicWidthInPix], 
		&DpbCb[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
		&DpbCr[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
		&Luma_l0[8 * PicWidthInPix], &Chroma_Cb_l0[2 * PicWidthInPix], 
		&Chroma_Cr_l0[2 * PicWidthInPix], mvL0[28], PicWidthInPix, interpol);

	address = RefPicListL0[ref_cache_l0[30]] . MemoryAddress;	
	xIntl = CLIP3(-11, PicWidthInPix - 27, x + 8 + (mvL0[30][0] >> 2));
	yIntl = CLIP3(-11, PicHeightInPix - 27, y + 8 + (mvL0[30][1] >> 2));


	interpolation_8x8(&DpbLuma[address + xIntl + yIntl * PicWidthInPix], 
		&DpbCb[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
		&DpbCr[(address >> 2) + (xIntl >> 1) + (yIntl >> 1) * (PicWidthInPix >> 1)], 
		&Luma_l0[8 + 8 * PicWidthInPix], &Chroma_Cb_l0[4 + 2 * PicWidthInPix], 
		&Chroma_Cr_l0[4 + 2 * PicWidthInPix], mvL0[30], PicWidthInPix, interpol);
}





