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


#ifndef INTERPOL_2_1_H
#define INTERPOL_2_1_H

void luma_sample_interp_2_1(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);

void luma_sample_interp_2_2(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);

void luma_sample_interp_2_3(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);



#ifdef TI_OPTIM
void luma_sample_interp_2_1_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_2_2_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_2_3_TI(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
#endif

#ifdef MMXi
void luma_sample_interp_2_1_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_2_2_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
void luma_sample_interp_2_3_MMX(unsigned char *image, unsigned char *refPicLXl,	const short PicWidthSamples, const short stride);
#endif


static __inline void luma_sample_interp_2_n_h(unsigned char ptr_ref [RESTRICT], short tmp_res [RESTRICT], const short PicWidthSamples)
{
	int i, j;

	for (j = 0; j < 9; j++, ptr_ref += PicWidthSamples) {
		unsigned char *ptr = &ptr_ref[0];
		for (i = 0; i < 36; i += 9, ptr ++)	{
			unsigned char A = ptr[0];
			unsigned char B = ptr[1];
			unsigned char C = ptr[2];
			unsigned char D = ptr[3];
			unsigned char E = ptr[4];
			unsigned char F = ptr[5];
			tmp_res[j + i] = A + F - 5 * (short)(B + E) + 20 * (short)(C + D);
		}
	}
}


static __inline void luma_sample_interp_2_n_v(unsigned char image [RESTRICT], short tmp_res [RESTRICT], const short stride)
{
	int i, j;
	for (i = 0; i < 4; i++)	{
		short *tmp = &tmp_res[9 * i];
		unsigned char *ptr_img = &image[i];
		for (j = 0; j < 4; j++, ptr_img += stride, tmp ++) {			
		
			short A = tmp[0];
			short B = tmp[1];
			short C = tmp[2];
			short D = tmp[3];
			short E = tmp[4];
			short F = tmp[5];
			
			int result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
			ptr_img[0] = CLIP255_16(result >> 10);
		} 
	}
}



static __inline void luma_sample_interp_2_n_2pass(unsigned char ptr_mg [RESTRICT], short tmp_res [RESTRICT], const short stride)
{
	int j;

	for (j = 0; j < 4; j++, tmp_res += 1 - (3 * 9), ptr_mg += stride){
		ptr_mg[0] = (ptr_mg[0] + CLIP255_16((tmp_res[0] + 16) >> 5) + 1) >> 1;
		tmp_res += 9;
		ptr_mg[1] = (ptr_mg[1] + CLIP255_16((tmp_res[0] + 16) >> 5) + 1) >> 1;
		tmp_res += 9;
		ptr_mg[2] = (ptr_mg[2] + CLIP255_16((tmp_res[0] + 16) >> 5) + 1) >> 1;
		tmp_res += 9;
		ptr_mg[3] = (ptr_mg[3] + CLIP255_16((tmp_res[0] + 16) >> 5) + 1) >> 1;
	}
}
#endif
