/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *			  Maxime  PELCAT <mpelcat@insa-rennes.Fr>
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
#include "interpol_0_n.h"
#include "interpol_n_0.h"



void luma_sample_interp_0_0(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride){

	int j;
	/* fullpel position */
	for (j = 0; j < 4; j++){
		*((unsigned int*) image) = (refPicLXl[0]) + (refPicLXl[1] << 8) + (refPicLXl[2] << 16) + (refPicLXl[3] << 24);
		image += stride;
		refPicLXl += PicWidthSamples;
	}
}




void luma_sample_interp_0_n(unsigned char *ptr_ref, unsigned char *ptr_img, const short PicWidthSamples, const short stride)
{
	int k;
	for (k = 0; k < 4; k++, ptr_ref += PicWidthSamples, ptr_img += stride) 	 {

		unsigned char *ptr = ptr_ref;

		// looks awful but very fast on c64
		int result1 = ptr[0];
		int result2 = ptr[1];
		int result3 = ptr[2];
		int result4 = ptr[3];

		ptr += PicWidthSamples;
		result1 += ptr[0]*(-5);
		result2 += ptr[1]*(-5);
		result3 += ptr[2]*(-5);
		result4 += ptr[3]*(-5);

		ptr += PicWidthSamples;
		result1 += ptr[0]*20;
		result2 += ptr[1]*20;
		result3 += ptr[2]*20;
		result4 += ptr[3]*20;

		ptr += PicWidthSamples;
		result1 += ptr[0]*20;
		result2 += ptr[1]*20;
		result3 += ptr[2]*20;
		result4 += ptr[3]*20;

		ptr += PicWidthSamples;
		result1 += ptr[0]*(-5);
		result2 += ptr[1]*(-5);
		result3 += ptr[2]*(-5);
		result4 += ptr[3]*(-5);

		ptr += PicWidthSamples;
		result1 += ptr[0];
		result2 += ptr[1];
		result3 += ptr[2];
		result4 += ptr[3];

		ptr_img[0] =  CLIP255_16((result1 + 16) >> 5);
		ptr_img[1] =  CLIP255_16((result2 + 16) >> 5);
		ptr_img[2] =  CLIP255_16((result3 + 16) >> 5);
		ptr_img[3] =  CLIP255_16((result4 + 16) >> 5);
	}
}


/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 0 and yFracl = 1.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_0_1(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride){

	/* No horizontal interpolation */

	unsigned char *ptr_ref = &refPicLXl[- (PicWidthSamples << 1)];
	unsigned char *ptr_img = &image[0];
	luma_sample_interp_0_n(ptr_ref, ptr_img, PicWidthSamples, stride);
	

	ptr_ref = &refPicLXl[0];
	ptr_img = &image[0];
	luma_sample_interp_2pass(ptr_ref, ptr_img, PicWidthSamples, stride);
}




/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 0 and yFracl = 2.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_0_2(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{

	/* No horizontal interpolation */
	unsigned char *ptr_ref = &refPicLXl[- (PicWidthSamples << 1)];
	unsigned char *ptr_img = &image[0];

	luma_sample_interp_0_n(ptr_ref, ptr_img, PicWidthSamples, stride);
}

/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 0 and yFracl = 3.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_0_3(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{

	/* No horizontal interpolation */
	unsigned char *ptr_ref = &refPicLXl[- (PicWidthSamples << 1)];
	unsigned char *ptr_img = &image[0];
	luma_sample_interp_0_n(ptr_ref, ptr_img, PicWidthSamples, stride);


	ptr_ref = &refPicLXl[PicWidthSamples];
	ptr_img = &image[0];
	luma_sample_interp_2pass(ptr_ref, ptr_img, PicWidthSamples, stride);
}


