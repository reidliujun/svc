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
#include "interpol_2_n.h"



/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 2 and yFracl = 1.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_2_1(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT], const short PicWidthSamples, const short stride)
{


	short tmp_res[4*9]; 

	/*Vertical & horizontal interpolation */
	unsigned char *ptr_ref = &refPicLXl[-((PicWidthSamples + 1) << 1)];

	luma_sample_interp_2_n_h(ptr_ref, tmp_res, PicWidthSamples);
	luma_sample_interp_2_n_v(image, tmp_res, stride);

	luma_sample_interp_2_n_2pass(image, &tmp_res[2], stride);
}




/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 2 and yFracl = 2.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_2_2(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{

	short tmp_res[4*9]; 

	// Vertical & horizontal interpolation 
	unsigned char *ptr_ref = &refPicLXl[-((PicWidthSamples + 1) << 1)];
	luma_sample_interp_2_n_h(ptr_ref, tmp_res, PicWidthSamples);
	luma_sample_interp_2_n_v(image, tmp_res, stride);
}







/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 2 and yFracl = 3.

@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_2_3(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{
	short tmp_res[4*9]; 

	/* Vertical & horizontal interpolation */
	unsigned char *ptr_ref = &refPicLXl[-((PicWidthSamples + 1) << 1)];
	luma_sample_interp_2_n_h(ptr_ref, tmp_res, PicWidthSamples);
	luma_sample_interp_2_n_v(image, tmp_res, stride);

	luma_sample_interp_2_n_2pass(image, &tmp_res[3], stride);
}
