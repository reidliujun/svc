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

#ifndef INTERPOL_0_N_H
#define INTERPOL_0_N_H

void luma_sample_interp_1_1(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT], const short PicWidthSamples, const short stride);
void luma_sample_interp_1_3(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT], const short PicWidthSamples, const short stride);
void luma_sample_interp_3_1(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride);
void luma_sample_interp_3_3(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride);

void luma_sample_interp_0_n(unsigned char *ptr_ref, unsigned char *ptr_img, const short PicWidthSamples, const short stride);

void luma_sample_interp_0_0(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_0_1(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_0_2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_0_3(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);

#ifdef MMXi
void luma_sample_interp_0_0_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_0_1_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_0_2_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_0_3_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
#endif

#ifdef TI_OPTIM
void luma_sample_interp_0_0_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_0_1_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_0_2_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_0_3_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
#endif


static __inline void luma_sample_interp_1_n(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT], const short PicWidthSamples, const short stride)
{

	int i, j;

	for (j = 0; j < 4; j++, image += stride, refPicLXl += PicWidthSamples){		
		for (i = 0; i < 4; i++)	{
			unsigned char A = refPicLXl[0 + i];
			unsigned char B = refPicLXl[1 + i]; 
			unsigned char C = refPicLXl[2 + i];
			unsigned char D = refPicLXl[3 + i];
			unsigned char E = refPicLXl[4 + i];
			unsigned char F = refPicLXl[5 + i];

			int result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;
			image[i] = (unsigned char) CLIP255_16(result >> 5);
		}
	}
}


static __inline void luma_sample_interp_1_n_2pass(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT], const short PicWidthSamples, const short stride)
{
	int i, j, result;

	for (i = 0; i < 4; image++, i++)	{
		int pres_x = i - 2 * PicWidthSamples; 
		for (j = 0; j < (stride << 2); j += stride)	{
			unsigned char A,B,C,D,E,F;
			A = refPicLXl[pres_x];
			pres_x+=PicWidthSamples;
			B = refPicLXl[pres_x];
			pres_x+=PicWidthSamples;
			C = refPicLXl[pres_x];
			pres_x+=PicWidthSamples;
			D = refPicLXl[pres_x];
			pres_x+=PicWidthSamples;
			E = refPicLXl[pres_x];
			pres_x+=PicWidthSamples;
			F = refPicLXl[pres_x];
			pres_x-=(PicWidthSamples<<2);
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;

			image[j] = (unsigned char) ((image[j] + CLIP255_16(result >> 5) + 1) >> 1);
		}
	}
}
#endif
