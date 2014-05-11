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
#include "interpol_n_2.h"



static __inline void luma_sample_interp_1_2_2pass(short tmp_res [RESTRICT], unsigned char ptr_imag[RESTRICT], const short stride)
{
	int j;

	for (j = 0; j < 4;j++, tmp_res += 9, ptr_imag += stride)
	{
		ptr_imag[0] = (unsigned char) ((ptr_imag[0] + CLIP255_16((tmp_res[0] + 16) >> 5) + 1) >> 1);
		ptr_imag[1] = (unsigned char) ((ptr_imag[1] + CLIP255_16((tmp_res[1] + 16) >> 5) + 1) >> 1);
		ptr_imag[2] = (unsigned char) ((ptr_imag[2] + CLIP255_16((tmp_res[2] + 16) >> 5) + 1) >> 1);
		ptr_imag[3] = (unsigned char) ((ptr_imag[3] + CLIP255_16((tmp_res[3] + 16) >> 5) + 1) >> 1);
	}
}



void luma_sample_interp_1_2_v(short tmp_res [RESTRICT], unsigned char refPicLXl[RESTRICT], const short PicWidthSamples)
{
	int i, j;

	for (i = 0; i < 9; i++)	{
		unsigned char *ptr_ref = &refPicLXl[i - 2 -(PicWidthSamples << 1)];

		for (j = 0; j < 4; j++)
		{
			unsigned char A,B,C,D,E,F;
			A = ptr_ref[0];
			ptr_ref += PicWidthSamples;
			B = ptr_ref[0];
			ptr_ref += PicWidthSamples;
			C = ptr_ref[0];
			ptr_ref += PicWidthSamples;
			D = ptr_ref[0];
			ptr_ref += PicWidthSamples;
			E = ptr_ref[0];
			ptr_ref += PicWidthSamples;
			F = ptr_ref[0];
			ptr_ref -= (PicWidthSamples<<2);

			tmp_res[j*9+i] = A + F - 5 * (short)(B + E) + 20 * (short)(C + D);
		}
	}
}


void luma_sample_interp_1_2_h(short tmp_res [RESTRICT], unsigned char image[RESTRICT], const short stride)
{
	int i,j;

	for (j = 0; j < 4; j++, image += stride, tmp_res += 5) {
		for (i = 0; i < 4; i++, tmp_res++)	{
			short A = tmp_res[0];
			short B = tmp_res[1];
			short C = tmp_res[2];
			short D = tmp_res[3];
			short E = tmp_res[4];
			short F = tmp_res[5];
			int result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;

			image[i] = (unsigned char) CLIP255_16(result >> 10);
		}
	}
}



/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 1 and yFracl = 2.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_1_2(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{ 


	short tmp_res[4*9];
	luma_sample_interp_1_2_v(tmp_res, refPicLXl, PicWidthSamples);	
	luma_sample_interp_1_2_h(tmp_res, image, stride);
	luma_sample_interp_1_2_2pass(&tmp_res[2], image, stride);
}


/**
This function allows to get the luminance prediction of a non IDR picture when xFracl = 3 and yFracl = 2.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_3_2(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{ 

	short tmp_res[4*9];
	luma_sample_interp_1_2_v(tmp_res, refPicLXl, PicWidthSamples);	
	luma_sample_interp_1_2_h(tmp_res, image, stride);
	luma_sample_interp_1_2_2pass(&tmp_res[3], image, stride);
}

