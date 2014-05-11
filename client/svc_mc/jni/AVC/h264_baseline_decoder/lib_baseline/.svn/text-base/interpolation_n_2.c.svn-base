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
#include "interpolation_n_0.h"





/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 1 and yFracl = 2.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_1_2(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{ 
#ifdef MMX

	int i,j,result;
	unsigned char *ptr_img = &image[0];
	short tmp_res[8*13]; 
    short *temp = &tmp_res[0];

	__m64 ref64_1, ref64_2, ref64_3, ref64_4;
	const __m64 cst16 = _mm_set_pi32 (0x00100010, 0x00100010);
	const __m64 cst0 = _mm_setzero_si64();
	const __m64 cst1 = _mm_set_pi32 (0x00010001, 0x00010001);

	for (j = 0; j < 8; j++, temp = &tmp_res[j * 13]){
		for (i = -2; i < 8+3; i++){
			temp[i+2] =  ( refPicLXl[(j -2)*(PicWidthSamples)+i] + refPicLXl[(j +3)*(PicWidthSamples)+i]);
			temp[i+2] += ( refPicLXl[(j -1)*(PicWidthSamples)+i] + refPicLXl[(j +2)*(PicWidthSamples)+i]) * (-5);
			temp[i+2] += ( refPicLXl[(j )*(PicWidthSamples)+i] + refPicLXl[(j +1)*(PicWidthSamples)+i]) * 20;
		}
	}

	temp = &tmp_res[0];
	for (j = 0; j < 8; j++, ptr_img += stride, temp += 5) {
		for (i = 0; i < 8; i++, temp++)
		{
			short A,B,C,D,E,F;
			A = temp[0];
			B = temp[1];
			C = temp[2];
			D = temp[3];
			E = temp[4];
			F = temp[5];
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;

			ptr_img[i] = (unsigned char) CLIP255_16(result >> 10);
		}
	}

	temp = &tmp_res[0];
	for (j = 0; j < 8; j++, temp = &tmp_res[j * 13])
	{
#ifdef POCKET_PC
		ref64_1 = _mm_set_pi32 (*((unsigned int*)&image[j * stride + 4]),*((unsigned int*)&image[j * stride]));
#else
		ref64_1 = *((__m64*)&image[j * stride]);
#endif
		ref64_2 =_mm_unpackhi_pi8(ref64_1,cst0);
		ref64_1 =_mm_unpacklo_pi8(ref64_1,cst0);

#ifdef POCKET_PC
		ref64_3 = _mm_set_pi16 (temp[5],temp[4],temp[3],temp[2]);
		ref64_4 = _mm_set_pi16 (temp[9],temp[8],temp[7],temp[6]);
#else
		ref64_3 = *((__m64*)&(temp[2]));
		ref64_4 = *((__m64*)&(temp[6]));
#endif

		ref64_3 = _mm_add_pi16(ref64_3,cst16);
		ref64_3 = _mm_srai_pi16(ref64_3,5);
		
		ref64_4 = _mm_add_pi16(ref64_4,cst16);
		ref64_4 = _mm_srai_pi16(ref64_4,5);

		ref64_3 = _mm_packs_pu16(ref64_3,ref64_4);
		ref64_4 =_mm_unpackhi_pi8(ref64_3,cst0);
		ref64_3 =_mm_unpacklo_pi8(ref64_3,cst0);
		
		ref64_3 = _mm_add_pi16(ref64_3,cst1);
		ref64_3 = _mm_add_pi16(ref64_3,ref64_1);
		ref64_3 = _mm_srai_pi16(ref64_3,1);

		ref64_4 = _mm_add_pi16(ref64_4,cst1);
		ref64_4 = _mm_add_pi16(ref64_4,ref64_2);
		ref64_4 = _mm_srai_pi16(ref64_4,1);
		
		ref64_3 = _mm_packs_pu16(ref64_3,ref64_4);
#ifdef POCKET_PC
		*((unsigned int*)&image[j * stride]) = ref64_3.m64_u32[0];
		*((unsigned int*)&image[j * stride + 4]) = ref64_3.m64_u32[1];
#else
		*((__m64*)&image[j * stride]) = ref64_3;
#endif
	}

	empty();    

#else

	int i,j,result;
	unsigned char *ptr_img = &image[0];
	short tmp_res[8*13]; 
    short *temp = &tmp_res[0];

	for (j = 0; j < 8; j++, temp = &tmp_res[j * 13]){
		for (i = -2; i < 8+3; i++){
			temp[i+2] =  ( refPicLXl[(j -2)*(PicWidthSamples)+i] + refPicLXl[(j +3)*(PicWidthSamples)+i]);
			temp[i+2] += ( refPicLXl[(j -1)*(PicWidthSamples)+i] + refPicLXl[(j +2)*(PicWidthSamples)+i]) * (-5);
			temp[i+2] += ( refPicLXl[(j )*(PicWidthSamples)+i] + refPicLXl[(j +1)*(PicWidthSamples)+i]) * 20;
		}
	}

	temp = &tmp_res[0];
	for (j = 0; j < 8; j++, ptr_img += stride, temp += 5) {
		for (i = 0; i < 8; i++, temp++)
		{
			short A,B,C,D,E,F;
			A = temp[0];
			B = temp[1];
			C = temp[2];
			D = temp[3];
			E = temp[4];
			F = temp[5];
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;

			ptr_img[i] = (unsigned char) CLIP255_16(result>>10);
		}
	}


	temp = &tmp_res[0];
	for (j = 0; j < 8; j++, temp = &tmp_res[j * 13]){
		image[(j * stride)]	   = (unsigned char) (( image[(j * stride)]	   + CLIP255_16((temp[2] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 1] = (unsigned char) (( image[(j * stride) + 1] + CLIP255_16((temp[3] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 2] = (unsigned char) (( image[(j * stride) + 2] + CLIP255_16((temp[4] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 3] = (unsigned char) (( image[(j * stride) + 3] + CLIP255_16((temp[5] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 4] = (unsigned char) (( image[(j * stride) + 4] + CLIP255_16((temp[6] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 5] = (unsigned char) (( image[(j * stride) + 5] + CLIP255_16((temp[7] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 6] = (unsigned char) (( image[(j * stride) + 6] + CLIP255_16((temp[8] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 7] = (unsigned char) (( image[(j * stride) + 7] + CLIP255_16((temp[9] + 16) >> 5) + 1) >> 1);
	}

#endif
}








/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 3 and yFracl = 2.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_3_2(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{ 
#ifdef MMX

	int i,j,result;
	unsigned char *ptr_img = &image[0];
	short tmp_res[8*13]; 
    short *temp = &tmp_res[0];

	__m64 ref64_1, ref64_2, ref64_3, ref64_4;
	const __m64 cst16 = _mm_set_pi32 (0x00100010, 0x00100010);
	const __m64 cst0 = _mm_setzero_si64();
	const __m64 cst1 = _mm_set_pi32 (0x00010001, 0x00010001);


	for (j = 0; j < 8; j++, temp = &tmp_res[j * 13]){
		for (i = -2; i < 8+3; i++){
			temp[i+2] =  ( refPicLXl[(j - 2) * (PicWidthSamples) + i] + refPicLXl[(j + 3) * (PicWidthSamples) + i]);
			temp[i+2] += ( refPicLXl[(j - 1) * (PicWidthSamples) + i] + refPicLXl[(j + 2) * (PicWidthSamples) + i]) * (-5);
			temp[i+2] += ( refPicLXl[(j )	 * (PicWidthSamples) + i] + refPicLXl[(j + 1) * (PicWidthSamples) + i]) * 20;
		}
	}

	temp = &tmp_res[0];
	for (j = 0; j < 8; j++, ptr_img += stride, temp += 5) {
		for (i = 0; i < 8; i++, temp++)
		{
			short A,B,C,D,E,F;
			A = temp[0];
			B = temp[1];
			C = temp[2];
			D = temp[3];
			E = temp[4];
			F = temp[5];
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;

			ptr_img[i] = (unsigned char) CLIP255_16(result >> 10);
		}
	}
 
	temp = &tmp_res[0];
	for (j = 0; j < 8; j++, temp = &tmp_res[j * 13])
	{
#ifdef POCKET_PC
		ref64_1 = _mm_set_pi32 (*((unsigned int*)&image[j * stride + 4]),*((unsigned int*)&image[j * stride]));
#else
		ref64_1 = *((__m64*)&image[j * stride]);
#endif
		ref64_2 =_mm_unpackhi_pi8(ref64_1,cst0);
		ref64_1 =_mm_unpacklo_pi8(ref64_1,cst0);

#ifdef POCKET_PC
		ref64_3 = _mm_set_pi16 (temp[6],temp[5],temp[4],temp[3]);
		ref64_4 = _mm_set_pi16 (temp[10],temp[9],temp[8],temp[7]);
#else
		ref64_3 = *((__m64*)&(temp[3]));
		ref64_4 = *((__m64*)&(temp[7]));
#endif

		ref64_3 = _mm_add_pi16(ref64_3,cst16);
		ref64_3 = _mm_srai_pi16(ref64_3,5);
		
		ref64_4 = _mm_add_pi16(ref64_4,cst16);
		ref64_4 = _mm_srai_pi16(ref64_4,5);

		ref64_3 = _mm_packs_pu16(ref64_3,ref64_4);
		ref64_4 =_mm_unpackhi_pi8(ref64_3,cst0);
		ref64_3 =_mm_unpacklo_pi8(ref64_3,cst0);
		
		ref64_3 = _mm_add_pi16(ref64_3,cst1);
		ref64_3 = _mm_add_pi16(ref64_3,ref64_1);
		ref64_3 = _mm_srai_pi16(ref64_3,1);

		ref64_4 = _mm_add_pi16(ref64_4,cst1);
		ref64_4 = _mm_add_pi16(ref64_4,ref64_2);
		ref64_4 = _mm_srai_pi16(ref64_4,1);
		
		ref64_3 = _mm_packs_pu16(ref64_3,ref64_4);
#ifdef POCKET_PC
		*((unsigned int*)&image[j * stride]) = ref64_3.m64_u32[0];
		*((unsigned int*)&image[j * stride + 4]) = ref64_3.m64_u32[1];
#else
		*((__m64*)&image[j * stride]) = ref64_3;
#endif
	}


	empty();    
#else

	int i,j,result;
	unsigned char *ptr_img = &image[0];
	short tmp_res[8*13]; 
    short *temp = &tmp_res[0];


	for (j = 0; j < 8; j++, temp = &tmp_res[j * 13]){
		for (i = -2; i < 8 + 3; i++){
			temp[i+2] =  ( refPicLXl[(j - 2) * (PicWidthSamples) + i] + refPicLXl[(j + 3) * (PicWidthSamples) + i]);
			temp[i+2] += ( refPicLXl[(j - 1) * (PicWidthSamples) + i] + refPicLXl[(j + 2) * (PicWidthSamples) + i]) * (-5);
			temp[i+2] += ( refPicLXl[(j )	 * (PicWidthSamples) + i] + refPicLXl[(j + 1) * (PicWidthSamples) + i]) * 20;
		}
	}

	temp = &tmp_res[0];
	for (j = 0; j < 8; j++, ptr_img += stride, temp += 5) {
		for (i = 0; i < 8; i++, temp++)
		{
			short A,B,C,D,E,F;
			A = temp[0];
			B = temp[1];
			C = temp[2];
			D = temp[3];
			E = temp[4];
			F = temp[5];
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;

			ptr_img[i] = (unsigned char) CLIP255_16(result >> 10);
		}
	}
 

	temp = &tmp_res[0];
	for (j = 0; j < 8; j++, temp = &tmp_res[j * 13]){
		image[(j * stride)]	   = (unsigned char) (( image[(j * stride)]	   + CLIP255_16((temp[3] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 1] = (unsigned char) (( image[(j * stride) + 1] + CLIP255_16((temp[4] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 2] = (unsigned char) (( image[(j * stride) + 2] + CLIP255_16((temp[5] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 3] = (unsigned char) (( image[(j * stride) + 3] + CLIP255_16((temp[6] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 4] = (unsigned char) (( image[(j * stride) + 4] + CLIP255_16((temp[7] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 5] = (unsigned char) (( image[(j * stride) + 5] + CLIP255_16((temp[8] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 6] = (unsigned char) (( image[(j * stride) + 6] + CLIP255_16((temp[9] + 16) >> 5) + 1) >> 1);
		image[(j * stride)+ 7] = (unsigned char) (( image[(j * stride) + 7] + CLIP255_16((temp[10] + 16) >> 5) + 1) >> 1);
	}

#endif
}

