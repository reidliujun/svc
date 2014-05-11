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

#ifndef INTERPOLATION_8x8_H
#define INTERPOLATION_8x8_H

#include "type.h"
#include "data.h"




void luma_sample_interp_8x8_0_0_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_0_1_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_0_2_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_0_3_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_1_0_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_2_0_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_3_0_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_1_2_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_2_2_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_3_2_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_1_1_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_1_3_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_3_1_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_3_3_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_2_1_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_8x8_2_3_SSE2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);


static __inline void luma_sample_interp_8x8_0_0(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride){

#ifdef MMX
#ifdef POCKET_PC

	int j;
	unsigned int temp32_1, temp32_2;

	/* fullpel position */
	for (j = 0; j < 8; j++ )
	{
		temp32_1 = (refPicLXl[0]) + ((refPicLXl[1]) << 8) + ((refPicLXl[2]) << 16) + ((refPicLXl[3]) << 24);
		temp32_2 = (refPicLXl[4]) + ((refPicLXl[5]) << 8) + ((refPicLXl[6]) << 16) + ((refPicLXl[7]) << 24);

		*((unsigned int*)image) =  temp32_1;
		*((unsigned int*)(image+4)) =  temp32_2;

		image += stride;
		refPicLXl += PicWidthSamples;
	}

#else

	int j;
	/* fullpel position */
	for (j = 0; j < 8; j++ )
	{
		*((__m64*)image) = *((__m64*)refPicLXl);
		image += stride;
		refPicLXl += PicWidthSamples;
	}

#endif
	empty();    
 
#else

	int j;
	/* fullpel position */
	for (j = 0; j < 8; j++ ){
		image[0] = *refPicLXl++;
		image[1] = *refPicLXl++;
		image[2] = *refPicLXl++;
		image[3] = *refPicLXl++;
		image[4] = *refPicLXl++;
		image[5] = *refPicLXl++;
		image[6] = *refPicLXl++;
		image[7] = *refPicLXl;
		image += stride;
		refPicLXl += PicWidthSamples - 7;

	}

#endif

}







void SampleInterpolation8x8(unsigned char *DpbLuma, unsigned char *DpbCb,unsigned char *DpbCr, 
							unsigned char *Luma_l0, unsigned char *Chroma_Cb_l0, unsigned char *Chroma_Cr_l0, 
							short mvL0[][2], short *ref_cache_l0, int x, int y, short PicWidthInPix, 
							short PicHeightInPix , const LIST_MMO *RefPicListL0, const interpol_8x8 *interpol);

void interpolation_8x8(unsigned char *DpbLuma, unsigned char *DpbCb,unsigned char *DpbCr, 
					   unsigned char *image, unsigned char *image_Cb, unsigned char *image_Cr, 
					   short mvL0[2], short PicWidthInPix, const interpol_8x8 *interpol);


#endif
