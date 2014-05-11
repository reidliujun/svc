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

#ifndef INTERPOL_N_0_H
#define INTERPOL_N_0_H

void luma_sample_interp_1_0(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_2_0(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_3_0(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);


#ifdef MMXi
void luma_sample_interp_1_0_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_2_0_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_3_0_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
#endif


#ifdef TI_OPTIM
void luma_sample_interp_1_0_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_2_0_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_3_0_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
#endif


static __inline void luma_sample_interp_n_0(unsigned char ptr_ref [RESTRICT], unsigned char ptr_imge [RESTRICT], const short PicWidthSamples, const short stride){

	int k;

	for (k = 0; k < 4; ptr_ref += PicWidthSamples, ptr_imge += stride, k++) {

		unsigned char A = ptr_ref[0];
		unsigned char B = ptr_ref[1];
		unsigned char C = ptr_ref[2];
		unsigned char D = ptr_ref[3];
		unsigned char E = ptr_ref[4];
		unsigned char F = ptr_ref[5];
		unsigned char G = ptr_ref[6];
		unsigned char H = ptr_ref[7];
		unsigned char I = ptr_ref[8];

		int result0 = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;
		int result1 = B + G - 5 * (short)(C + F) + 20 * (short)(D + E) + 16;
		int result2 = C + H - 5 * (short)(D + G) + 20 * (short)(E + F) + 16;
		int result3 = D + I - 5 * (short)(E + H) + 20 * (short)(F + G) + 16;

		ptr_imge[0] = (unsigned char) CLIP255_16((result0) >> 5);
		ptr_imge[1] = (unsigned char) CLIP255_16((result1) >> 5);
		ptr_imge[2] = (unsigned char) CLIP255_16((result2) >> 5);
		ptr_imge[3] = (unsigned char) CLIP255_16((result3) >> 5);
	}
}


static __inline void luma_sample_interp_2pass(unsigned char ptr_rf [RESTRICT], unsigned char ptr_img [RESTRICT], const short PicWidthSamples, const short stride)
{
	int j;

	for ( j = 0; j < 4;j++, ptr_rf += PicWidthSamples, ptr_img += stride)	{
		ptr_img[0] = (unsigned char) ((ptr_img[0] + ptr_rf[0] + 1) >> 1);
		ptr_img[1] = (unsigned char) ((ptr_img[1] + ptr_rf[1] + 1) >> 1);
		ptr_img[2] = (unsigned char) ((ptr_img[2] + ptr_rf[2] + 1) >> 1);
		ptr_img[3] = (unsigned char) ((ptr_img[3] + ptr_rf[3] + 1) >> 1);
	}	
}
#endif
