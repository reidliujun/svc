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
#include "interpol_n_0.h"








/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 1 and yFracl = 0.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_1_0(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{

	/* No vertical interpolation */
	unsigned char *ptr_ref = &refPicLXl[-2];
	unsigned char *ptr_imge = &image[0];
	unsigned char *ptr_rf = &refPicLXl[0];
	unsigned char *ptr_img = &image[0];


	/* No vertical interpolation */
	luma_sample_interp_n_0(ptr_ref, ptr_imge, PicWidthSamples, stride);
	luma_sample_interp_2pass(ptr_rf, ptr_img, PicWidthSamples, stride);
}





/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 2 and yFracl = 0.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_2_0(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{

	/* No vertical interpolation */
	unsigned char *ptr_ref = &refPicLXl[-2];
	unsigned char *ptr_imge = &image[0];
	luma_sample_interp_n_0(ptr_ref, ptr_imge, PicWidthSamples, stride);
}




/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 3 and yFracl = 0.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_3_0(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride){

	/* No vertical interpolation */
	unsigned char *ptr_ref = &refPicLXl[-2];
	unsigned char *ptr_imge = &image[0];
	unsigned char *ptr_rf = &refPicLXl[1];
	unsigned char *ptr_img = &image[0];

	/* No vertical interpolation */
	luma_sample_interp_n_0(ptr_ref, ptr_imge, PicWidthSamples, stride);
	luma_sample_interp_2pass(ptr_rf, ptr_img, PicWidthSamples, stride);
}
