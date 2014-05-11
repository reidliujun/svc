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
#include "interpolation_n_n.h"



/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 1 and yFracl = 1.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_1_1(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples,const short stride){

#ifdef MMX

	short j, pres_x;
	unsigned char *tmp_ref = &refPicLXl[- 2];
	unsigned char *ptr_img = &image[0];

	__m64 ref64_1, ref64_2, ref64_3, ref64_4;
	const __m64 cst16 = _mm_set_pi32 (0x00100010, 0x00100010);
	const __m64 cst0 = _mm_setzero_si64();
	const __m64 cst1 = _mm_set_pi32 (0x00010001, 0x00010001);

	 
	for (j = 0; j < 8; j++,ptr_img += stride, tmp_ref += PicWidthSamples){		

#ifdef POCKET_PC
		ref64_1 = _mm_set_pi8 (tmp_ref[7],tmp_ref[6],tmp_ref[5],tmp_ref[4],tmp_ref[3],tmp_ref[2],tmp_ref[1],tmp_ref[0]);
		ref64_2 = _mm_set_pi8 (0,0,0,tmp_ref[12],tmp_ref[11],tmp_ref[10],tmp_ref[9],tmp_ref[8]);
#else
		ref64_1 = *((__m64*)&tmp_ref[0]);
		ref64_2 = *((__m64*)&tmp_ref[8]);
#endif

		ref64_3.m64_i16[0] = ref64_1.m64_u8[0] + ref64_1.m64_u8[5] - 5 * (short)(ref64_1.m64_u8[1] + ref64_1.m64_u8[4]) + 20 * (short)(ref64_1.m64_u8[2] + ref64_1.m64_u8[3]);
		ref64_3.m64_i16[1] = ref64_1.m64_u8[1] + ref64_1.m64_u8[6] - 5 * (short)(ref64_1.m64_u8[2] + ref64_1.m64_u8[5]) + 20 * (short)(ref64_1.m64_u8[3] + ref64_1.m64_u8[4]);
		ref64_3.m64_i16[2] = ref64_1.m64_u8[2] + ref64_1.m64_u8[7] - 5 * (short)(ref64_1.m64_u8[3] + ref64_1.m64_u8[6]) + 20 * (short)(ref64_1.m64_u8[4] + ref64_1.m64_u8[5]);
		ref64_3.m64_i16[3] = ref64_1.m64_u8[3] + ref64_2.m64_u8[0] - 5 * (short)(ref64_1.m64_u8[4] + ref64_1.m64_u8[7]) + 20 * (short)(ref64_1.m64_u8[5] + ref64_1.m64_u8[6]);
		ref64_4.m64_i16[0] = ref64_1.m64_u8[4] + ref64_2.m64_u8[1] - 5 * (short)(ref64_1.m64_u8[5] + ref64_2.m64_u8[0]) + 20 * (short)(ref64_1.m64_u8[6] + ref64_1.m64_u8[7]);
		ref64_4.m64_i16[1] = ref64_1.m64_u8[5] + ref64_2.m64_u8[2] - 5 * (short)(ref64_1.m64_u8[6] + ref64_2.m64_u8[1]) + 20 * (short)(ref64_1.m64_u8[7] + ref64_2.m64_u8[0]);
		ref64_4.m64_i16[2] = ref64_1.m64_u8[6] + ref64_2.m64_u8[3] - 5 * (short)(ref64_1.m64_u8[7] + ref64_2.m64_u8[2]) + 20 * (short)(ref64_2.m64_u8[0] + ref64_2.m64_u8[1]);
		ref64_4.m64_i16[3] = ref64_1.m64_u8[7] + ref64_2.m64_u8[4] - 5 * (short)(ref64_2.m64_u8[0] + ref64_2.m64_u8[3]) + 20 * (short)(ref64_2.m64_u8[1] + ref64_2.m64_u8[2]);
		
		ref64_3 = _mm_add_pi16(ref64_3,cst16);
		ref64_3 = _mm_srai_pi16(ref64_3,5);
		ref64_4 = _mm_add_pi16(ref64_4,cst16);
		ref64_4 = _mm_srai_pi16(ref64_4,5);
		ref64_3 = _mm_packs_pu16(ref64_3,ref64_4);

#ifdef POCKET_PC
		*((unsigned int*)&ptr_img[0]) = ref64_3.m64_i32[0];
		*((unsigned int*)&ptr_img[4]) = ref64_3.m64_i32[1];
#else
		*((__m64*)&ptr_img[0]) = ref64_3;
#endif
	}


	ptr_img = &image[0];

	for (j = 0; j < 8; j ++)
	{
		__m64 A64,B64,C64,D64,E64,F64;
		pres_x =  (j - 2) * PicWidthSamples; 

#ifdef POCKET_PC
		A64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		B64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		C64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		D64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		E64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		F64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
#else
		A64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		B64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		C64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		D64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		E64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		F64 = *((__m64*)&refPicLXl[pres_x]);
#endif

		ref64_1.m64_i16[0] = A64.m64_u8[0] + F64.m64_u8[0] - 5 * (short)(B64.m64_u8[0] + E64.m64_u8[0]) + 20 * (short)(C64.m64_u8[0] + D64.m64_u8[0]);
		ref64_1.m64_i16[1] = A64.m64_u8[1] + F64.m64_u8[1] - 5 * (short)(B64.m64_u8[1] + E64.m64_u8[1]) + 20 * (short)(C64.m64_u8[1] + D64.m64_u8[1]);
		ref64_1.m64_i16[2] = A64.m64_u8[2] + F64.m64_u8[2] - 5 * (short)(B64.m64_u8[2] + E64.m64_u8[2]) + 20 * (short)(C64.m64_u8[2] + D64.m64_u8[2]);
		ref64_1.m64_i16[3] = A64.m64_u8[3] + F64.m64_u8[3] - 5 * (short)(B64.m64_u8[3] + E64.m64_u8[3]) + 20 * (short)(C64.m64_u8[3] + D64.m64_u8[3]);
		ref64_2.m64_i16[0] = A64.m64_u8[4] + F64.m64_u8[4] - 5 * (short)(B64.m64_u8[4] + E64.m64_u8[4]) + 20 * (short)(C64.m64_u8[4] + D64.m64_u8[4]);
		ref64_2.m64_i16[1] = A64.m64_u8[5] + F64.m64_u8[5] - 5 * (short)(B64.m64_u8[5] + E64.m64_u8[5]) + 20 * (short)(C64.m64_u8[5] + D64.m64_u8[5]);
		ref64_2.m64_i16[2] = A64.m64_u8[6] + F64.m64_u8[6] - 5 * (short)(B64.m64_u8[6] + E64.m64_u8[6]) + 20 * (short)(C64.m64_u8[6] + D64.m64_u8[6]);
		ref64_2.m64_i16[3] = A64.m64_u8[7] + F64.m64_u8[7] - 5 * (short)(B64.m64_u8[7] + E64.m64_u8[7]) + 20 * (short)(C64.m64_u8[7] + D64.m64_u8[7]);

		ref64_1 = _mm_add_pi16(ref64_1,cst16);
		ref64_1 = _mm_srai_pi16(ref64_1,5);
		ref64_2 = _mm_add_pi16(ref64_2,cst16);
		ref64_2 = _mm_srai_pi16(ref64_2,5);
		ref64_1 = _mm_packs_pu16(ref64_1,ref64_2);

		ref64_2 =_mm_unpackhi_pi8(ref64_1,cst0);
		ref64_1 =_mm_unpacklo_pi8(ref64_1,cst0);

#ifdef POCKET_PC
		ref64_3 = _mm_set_pi32 (*((unsigned int*)&ptr_img[j*stride + 4]),*((unsigned int*)&ptr_img[j*stride]));
#else
		ref64_3 = *((__m64*)&ptr_img[j*stride]);
#endif
		ref64_4 =_mm_unpackhi_pi8(ref64_3,cst0);
		ref64_3 =_mm_unpacklo_pi8(ref64_3,cst0);

		ref64_1 = _mm_add_pi16(ref64_1,ref64_3);
		ref64_1 = _mm_add_pi16(ref64_1,cst1);
		ref64_1 = _mm_srai_pi16(ref64_1,1);

		ref64_2 = _mm_add_pi16(ref64_2,ref64_4);
		ref64_2 = _mm_add_pi16(ref64_2,cst1);
		ref64_2 = _mm_srai_pi16(ref64_2,1);

		ref64_1 = _mm_packs_pu16(ref64_1,ref64_2);

#ifdef POCKET_PC
		*((unsigned int*)&ptr_img[j * stride]) = ref64_1.m64_u32[0];
		*((unsigned int*)&ptr_img[j * stride + 4]) = ref64_1.m64_u32[1];
#else
		*((__m64*)&ptr_img[j * stride]) = ref64_1;
#endif

	}

	empty();    
#else

	short i,j, result;
	unsigned char *tmp_ref = &refPicLXl[- 2];
	unsigned char *ptr_img = &image[0];
	 
	for (j = 0; j < 8; j++,ptr_img += stride, tmp_ref += PicWidthSamples){		
		for (i = 0; i < 8; i++)	{
			unsigned char A,B,C,D,E,F;
			A = tmp_ref[0 + i];
			B = tmp_ref[1 + i]; 
			C = tmp_ref[2 + i];
			D = tmp_ref[3 + i];
			E = tmp_ref[4 + i];
			F = tmp_ref[5 + i];

			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;
			ptr_img[i] = (unsigned char) CLIP255_16(result >> 5);
		}
	}

	ptr_img = &image[0];
	for (i = 0; i < 8; ptr_img++, i++)	{
		short pres_x =  i - 2 * PicWidthSamples; 

		for (j = 0; j < (stride << 3); j += stride)		{
			unsigned char A,B,C,D,E,F;
			result = 0;
			A = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			B = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			C = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			D = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			E = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			F = refPicLXl[pres_x];
			pres_x -= (PicWidthSamples<<2);
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;
	
			ptr_img[j] = (unsigned char) ((ptr_img[j] + CLIP255_16(result >> 5) + 1) >> 1);
		}
	}

#endif

}






/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 1 and yFracl = 3.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_1_3(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples,const short stride){

#ifdef MMX

	short j, pres_x;
	unsigned char *tmp_ref = &refPicLXl[PicWidthSamples - 2];
	unsigned char *ptr_img = &image[0];
	 
	__m64 ref64_1, ref64_2, ref64_3, ref64_4;
	const __m64 cst16 = _mm_set_pi32 (0x00100010, 0x00100010);
	const __m64 cst0 = _mm_setzero_si64();
	const __m64 cst1 = _mm_set_pi32 (0x00010001, 0x00010001);

	for (j = 0; j < 8; j++,ptr_img += stride, tmp_ref += PicWidthSamples){		

#ifdef POCKET_PC
		ref64_1 = _mm_set_pi8 (tmp_ref[7],tmp_ref[6],tmp_ref[5],tmp_ref[4],tmp_ref[3],tmp_ref[2],tmp_ref[1],tmp_ref[0]);
		ref64_2 = _mm_set_pi8 (0,0,0,tmp_ref[12],tmp_ref[11],tmp_ref[10],tmp_ref[9],tmp_ref[8]);
#else
		ref64_1 = *((__m64*)&tmp_ref[0]);
		ref64_2 = *((__m64*)&tmp_ref[8]);
#endif

		ref64_3.m64_i16[0] = ref64_1.m64_u8[0] + ref64_1.m64_u8[5] - 5 * (short)(ref64_1.m64_u8[1] + ref64_1.m64_u8[4]) + 20 * (short)(ref64_1.m64_u8[2] + ref64_1.m64_u8[3]);
		ref64_3.m64_i16[1] = ref64_1.m64_u8[1] + ref64_1.m64_u8[6] - 5 * (short)(ref64_1.m64_u8[2] + ref64_1.m64_u8[5]) + 20 * (short)(ref64_1.m64_u8[3] + ref64_1.m64_u8[4]);
		ref64_3.m64_i16[2] = ref64_1.m64_u8[2] + ref64_1.m64_u8[7] - 5 * (short)(ref64_1.m64_u8[3] + ref64_1.m64_u8[6]) + 20 * (short)(ref64_1.m64_u8[4] + ref64_1.m64_u8[5]);
		ref64_3.m64_i16[3] = ref64_1.m64_u8[3] + ref64_2.m64_u8[0] - 5 * (short)(ref64_1.m64_u8[4] + ref64_1.m64_u8[7]) + 20 * (short)(ref64_1.m64_u8[5] + ref64_1.m64_u8[6]);
		ref64_4.m64_i16[0] = ref64_1.m64_u8[4] + ref64_2.m64_u8[1] - 5 * (short)(ref64_1.m64_u8[5] + ref64_2.m64_u8[0]) + 20 * (short)(ref64_1.m64_u8[6] + ref64_1.m64_u8[7]);
		ref64_4.m64_i16[1] = ref64_1.m64_u8[5] + ref64_2.m64_u8[2] - 5 * (short)(ref64_1.m64_u8[6] + ref64_2.m64_u8[1]) + 20 * (short)(ref64_1.m64_u8[7] + ref64_2.m64_u8[0]);
		ref64_4.m64_i16[2] = ref64_1.m64_u8[6] + ref64_2.m64_u8[3] - 5 * (short)(ref64_1.m64_u8[7] + ref64_2.m64_u8[2]) + 20 * (short)(ref64_2.m64_u8[0] + ref64_2.m64_u8[1]);
		ref64_4.m64_i16[3] = ref64_1.m64_u8[7] + ref64_2.m64_u8[4] - 5 * (short)(ref64_2.m64_u8[0] + ref64_2.m64_u8[3]) + 20 * (short)(ref64_2.m64_u8[1] + ref64_2.m64_u8[2]);
		
		ref64_3 = _mm_add_pi16(ref64_3,cst16);
		ref64_3 = _mm_srai_pi16(ref64_3,5);
		ref64_4 = _mm_add_pi16(ref64_4,cst16);
		ref64_4 = _mm_srai_pi16(ref64_4,5);
		ref64_3 = _mm_packs_pu16(ref64_3,ref64_4);

#ifdef POCKET_PC
		*((unsigned int*)&ptr_img[0]) = ref64_3.m64_i32[0];
		*((unsigned int*)&ptr_img[4]) = ref64_3.m64_i32[1];
#else
		*((__m64*)&ptr_img[0]) = ref64_3;
#endif
	}

	ptr_img = &image[0];

	for (j = 0; j < 8; j ++)
	{
		__m64 A64,B64,C64,D64,E64,F64;
		pres_x =  (j - 2) * PicWidthSamples; 

#ifdef POCKET_PC
		A64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		B64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		C64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		D64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		E64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		F64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
#else
		A64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		B64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		C64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		D64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		E64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		F64 = *((__m64*)&refPicLXl[pres_x]);
#endif

		ref64_1.m64_i16[0] = A64.m64_u8[0] + F64.m64_u8[0] - 5 * (short)(B64.m64_u8[0] + E64.m64_u8[0]) + 20 * (short)(C64.m64_u8[0] + D64.m64_u8[0]);
		ref64_1.m64_i16[1] = A64.m64_u8[1] + F64.m64_u8[1] - 5 * (short)(B64.m64_u8[1] + E64.m64_u8[1]) + 20 * (short)(C64.m64_u8[1] + D64.m64_u8[1]);
		ref64_1.m64_i16[2] = A64.m64_u8[2] + F64.m64_u8[2] - 5 * (short)(B64.m64_u8[2] + E64.m64_u8[2]) + 20 * (short)(C64.m64_u8[2] + D64.m64_u8[2]);
		ref64_1.m64_i16[3] = A64.m64_u8[3] + F64.m64_u8[3] - 5 * (short)(B64.m64_u8[3] + E64.m64_u8[3]) + 20 * (short)(C64.m64_u8[3] + D64.m64_u8[3]);
		ref64_2.m64_i16[0] = A64.m64_u8[4] + F64.m64_u8[4] - 5 * (short)(B64.m64_u8[4] + E64.m64_u8[4]) + 20 * (short)(C64.m64_u8[4] + D64.m64_u8[4]);
		ref64_2.m64_i16[1] = A64.m64_u8[5] + F64.m64_u8[5] - 5 * (short)(B64.m64_u8[5] + E64.m64_u8[5]) + 20 * (short)(C64.m64_u8[5] + D64.m64_u8[5]);
		ref64_2.m64_i16[2] = A64.m64_u8[6] + F64.m64_u8[6] - 5 * (short)(B64.m64_u8[6] + E64.m64_u8[6]) + 20 * (short)(C64.m64_u8[6] + D64.m64_u8[6]);
		ref64_2.m64_i16[3] = A64.m64_u8[7] + F64.m64_u8[7] - 5 * (short)(B64.m64_u8[7] + E64.m64_u8[7]) + 20 * (short)(C64.m64_u8[7] + D64.m64_u8[7]);

		ref64_1 = _mm_add_pi16(ref64_1,cst16);
		ref64_1 = _mm_srai_pi16(ref64_1,5);
		ref64_2 = _mm_add_pi16(ref64_2,cst16);
		ref64_2 = _mm_srai_pi16(ref64_2,5);
		ref64_1 = _mm_packs_pu16(ref64_1,ref64_2);

		ref64_2 =_mm_unpackhi_pi8(ref64_1,cst0);
		ref64_1 =_mm_unpacklo_pi8(ref64_1,cst0);

#ifdef POCKET_PC
		ref64_3 = _mm_set_pi32 (*((unsigned int*)&ptr_img[j*stride + 4]),*((unsigned int*)&ptr_img[j*stride]));
#else
		ref64_3 = *((__m64*)&ptr_img[j*stride]);
#endif
		ref64_4 =_mm_unpackhi_pi8(ref64_3,cst0);
		ref64_3 =_mm_unpacklo_pi8(ref64_3,cst0);

		ref64_1 = _mm_add_pi16(ref64_1,ref64_3);
		ref64_1 = _mm_add_pi16(ref64_1,cst1);
		ref64_1 = _mm_srai_pi16(ref64_1,1);

		ref64_2 = _mm_add_pi16(ref64_2,ref64_4);
		ref64_2 = _mm_add_pi16(ref64_2,cst1);
		ref64_2 = _mm_srai_pi16(ref64_2,1);

		ref64_1 = _mm_packs_pu16(ref64_1,ref64_2);

#ifdef POCKET_PC
		*((unsigned int*)&ptr_img[j * stride]) = ref64_1.m64_u32[0];
		*((unsigned int*)&ptr_img[j * stride + 4]) = ref64_1.m64_u32[1];
#else
		*((__m64*)&ptr_img[j * stride]) = ref64_1;
#endif

	}

	empty();    

#else

	short i,j, result, pres_x;
	unsigned char *tmp_ref = &refPicLXl[PicWidthSamples - 2];
	unsigned char *ptr_img = &image[0];
	 
	for (j = 0; j < 8; j++,ptr_img += stride, tmp_ref += PicWidthSamples){		
		for (i = 0; i < 8; i++)	{
			unsigned char A,B,C,D,E,F;
			A = tmp_ref[0 + i];
			B = tmp_ref[1 + i]; 
			C = tmp_ref[2 + i];
			D = tmp_ref[3 + i];
			E = tmp_ref[4 + i];
			F = tmp_ref[5 + i];

			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;
			ptr_img[i] = (unsigned char) CLIP255_16(result >> 5);
		}
	}

	ptr_img = &image[0];
	for (i = 0; i < 8; ptr_img++, i++)	{
		pres_x =  i - 2 * PicWidthSamples; 

		for (j = 0; j < (stride << 3); j += stride)		{
			unsigned char A,B,C,D,E,F;
			result = 0;
			A = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
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
	
			ptr_img[j] = (unsigned char) ((ptr_img[j] + CLIP255_16(result >> 5) + 1) >> 1);
		}
	}

#endif

}




/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 3 and yFracl = 1.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_3_1(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples,const short stride){

#ifdef MMX

	short j, pres_x;
	unsigned char *tmp_ref = &refPicLXl[- 2];
	unsigned char *ptr_img = &image[0];

	__m64 ref64_1, ref64_2, ref64_3, ref64_4;
	const __m64 cst16 = _mm_set_pi32 (0x00100010, 0x00100010);
	const __m64 cst0 = _mm_setzero_si64();
	const __m64 cst1 = _mm_set_pi32 (0x00010001, 0x00010001);

	for (j = 0; j < 8; j++,ptr_img += stride, tmp_ref += PicWidthSamples){		

#ifdef POCKET_PC
		ref64_1 = _mm_set_pi8 (tmp_ref[7],tmp_ref[6],tmp_ref[5],tmp_ref[4],tmp_ref[3],tmp_ref[2],tmp_ref[1],tmp_ref[0]);
		ref64_2 = _mm_set_pi8 (0,0,0,tmp_ref[12],tmp_ref[11],tmp_ref[10],tmp_ref[9],tmp_ref[8]);
#else
		ref64_1 = *((__m64*)&tmp_ref[0]);
		ref64_2 = *((__m64*)&tmp_ref[8]);
#endif

		ref64_3.m64_i16[0] = ref64_1.m64_u8[0] + ref64_1.m64_u8[5] - 5 * (short)(ref64_1.m64_u8[1] + ref64_1.m64_u8[4]) + 20 * (short)(ref64_1.m64_u8[2] + ref64_1.m64_u8[3]);
		ref64_3.m64_i16[1] = ref64_1.m64_u8[1] + ref64_1.m64_u8[6] - 5 * (short)(ref64_1.m64_u8[2] + ref64_1.m64_u8[5]) + 20 * (short)(ref64_1.m64_u8[3] + ref64_1.m64_u8[4]);
		ref64_3.m64_i16[2] = ref64_1.m64_u8[2] + ref64_1.m64_u8[7] - 5 * (short)(ref64_1.m64_u8[3] + ref64_1.m64_u8[6]) + 20 * (short)(ref64_1.m64_u8[4] + ref64_1.m64_u8[5]);
		ref64_3.m64_i16[3] = ref64_1.m64_u8[3] + ref64_2.m64_u8[0] - 5 * (short)(ref64_1.m64_u8[4] + ref64_1.m64_u8[7]) + 20 * (short)(ref64_1.m64_u8[5] + ref64_1.m64_u8[6]);
		ref64_4.m64_i16[0] = ref64_1.m64_u8[4] + ref64_2.m64_u8[1] - 5 * (short)(ref64_1.m64_u8[5] + ref64_2.m64_u8[0]) + 20 * (short)(ref64_1.m64_u8[6] + ref64_1.m64_u8[7]);
		ref64_4.m64_i16[1] = ref64_1.m64_u8[5] + ref64_2.m64_u8[2] - 5 * (short)(ref64_1.m64_u8[6] + ref64_2.m64_u8[1]) + 20 * (short)(ref64_1.m64_u8[7] + ref64_2.m64_u8[0]);
		ref64_4.m64_i16[2] = ref64_1.m64_u8[6] + ref64_2.m64_u8[3] - 5 * (short)(ref64_1.m64_u8[7] + ref64_2.m64_u8[2]) + 20 * (short)(ref64_2.m64_u8[0] + ref64_2.m64_u8[1]);
		ref64_4.m64_i16[3] = ref64_1.m64_u8[7] + ref64_2.m64_u8[4] - 5 * (short)(ref64_2.m64_u8[0] + ref64_2.m64_u8[3]) + 20 * (short)(ref64_2.m64_u8[1] + ref64_2.m64_u8[2]);
		
		ref64_3 = _mm_add_pi16(ref64_3,cst16);
		ref64_3 = _mm_srai_pi16(ref64_3,5);
		ref64_4 = _mm_add_pi16(ref64_4,cst16);
		ref64_4 = _mm_srai_pi16(ref64_4,5);
		ref64_3 = _mm_packs_pu16(ref64_3,ref64_4);

#ifdef POCKET_PC
		*((unsigned int*)&ptr_img[0]) = ref64_3.m64_i32[0];
		*((unsigned int*)&ptr_img[4]) = ref64_3.m64_i32[1];
#else
		*((__m64*)&ptr_img[0]) = ref64_3;
#endif
	}

	ptr_img = &image[0];

	for (j = 0; j < 8; j ++)
	{
		__m64 A64,B64,C64,D64,E64,F64;
		pres_x =  1 + (j - 2) * PicWidthSamples; 

#ifdef POCKET_PC
		A64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		B64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		C64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		D64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		E64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		F64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
#else
		A64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		B64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		C64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		D64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		E64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		F64 = *((__m64*)&refPicLXl[pres_x]);
#endif

		ref64_1.m64_i16[0] = A64.m64_u8[0] + F64.m64_u8[0] - 5 * (short)(B64.m64_u8[0] + E64.m64_u8[0]) + 20 * (short)(C64.m64_u8[0] + D64.m64_u8[0]);
		ref64_1.m64_i16[1] = A64.m64_u8[1] + F64.m64_u8[1] - 5 * (short)(B64.m64_u8[1] + E64.m64_u8[1]) + 20 * (short)(C64.m64_u8[1] + D64.m64_u8[1]);
		ref64_1.m64_i16[2] = A64.m64_u8[2] + F64.m64_u8[2] - 5 * (short)(B64.m64_u8[2] + E64.m64_u8[2]) + 20 * (short)(C64.m64_u8[2] + D64.m64_u8[2]);
		ref64_1.m64_i16[3] = A64.m64_u8[3] + F64.m64_u8[3] - 5 * (short)(B64.m64_u8[3] + E64.m64_u8[3]) + 20 * (short)(C64.m64_u8[3] + D64.m64_u8[3]);
		ref64_2.m64_i16[0] = A64.m64_u8[4] + F64.m64_u8[4] - 5 * (short)(B64.m64_u8[4] + E64.m64_u8[4]) + 20 * (short)(C64.m64_u8[4] + D64.m64_u8[4]);
		ref64_2.m64_i16[1] = A64.m64_u8[5] + F64.m64_u8[5] - 5 * (short)(B64.m64_u8[5] + E64.m64_u8[5]) + 20 * (short)(C64.m64_u8[5] + D64.m64_u8[5]);
		ref64_2.m64_i16[2] = A64.m64_u8[6] + F64.m64_u8[6] - 5 * (short)(B64.m64_u8[6] + E64.m64_u8[6]) + 20 * (short)(C64.m64_u8[6] + D64.m64_u8[6]);
		ref64_2.m64_i16[3] = A64.m64_u8[7] + F64.m64_u8[7] - 5 * (short)(B64.m64_u8[7] + E64.m64_u8[7]) + 20 * (short)(C64.m64_u8[7] + D64.m64_u8[7]);

		ref64_1 = _mm_add_pi16(ref64_1,cst16);
		ref64_1 = _mm_srai_pi16(ref64_1,5);
		ref64_2 = _mm_add_pi16(ref64_2,cst16);
		ref64_2 = _mm_srai_pi16(ref64_2,5);
		ref64_1 = _mm_packs_pu16(ref64_1,ref64_2);

		ref64_2 =_mm_unpackhi_pi8(ref64_1,cst0);
		ref64_1 =_mm_unpacklo_pi8(ref64_1,cst0);

#ifdef POCKET_PC
		ref64_3 = _mm_set_pi32 (*((unsigned int*)&ptr_img[j*stride + 4]),*((unsigned int*)&ptr_img[j*stride]));
#else
		ref64_3 = *((__m64*)&ptr_img[j*stride]);
#endif
		ref64_4 =_mm_unpackhi_pi8(ref64_3,cst0);
		ref64_3 =_mm_unpacklo_pi8(ref64_3,cst0);

		ref64_1 = _mm_add_pi16(ref64_1,ref64_3);
		ref64_1 = _mm_add_pi16(ref64_1,cst1);
		ref64_1 = _mm_srai_pi16(ref64_1,1);

		ref64_2 = _mm_add_pi16(ref64_2,ref64_4);
		ref64_2 = _mm_add_pi16(ref64_2,cst1);
		ref64_2 = _mm_srai_pi16(ref64_2,1);

		ref64_1 = _mm_packs_pu16(ref64_1,ref64_2);

#ifdef POCKET_PC
		*((unsigned int*)&ptr_img[j * stride]) = ref64_1.m64_u32[0];
		*((unsigned int*)&ptr_img[j * stride + 4]) = ref64_1.m64_u32[1];
#else
		*((__m64*)&ptr_img[j * stride]) = ref64_1;
#endif

	}


	empty();     

#else

	short i,j, result, pres_x;
	unsigned char *tmp_ref = &refPicLXl[- 2];
	unsigned char *ptr_img = &image[0];
	 
	for (j = 0; j < 8; j++,ptr_img += stride, tmp_ref += PicWidthSamples){		
		for (i = 0; i < 8; i++)	{
			unsigned char A,B,C,D,E,F;
			A = tmp_ref[0 + i];
			B = tmp_ref[1 + i]; 
			C = tmp_ref[2 + i];
			D = tmp_ref[3 + i];
			E = tmp_ref[4 + i];
			F = tmp_ref[5 + i];

			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;
			ptr_img[i] = (unsigned char) CLIP255_16(result>>5);
		}
	}

	ptr_img = &image[0];
	for (i = 0; i < 8; ptr_img++, i++)	{
		pres_x =  1 + i - 2 * PicWidthSamples; 

		for (j = 0; j < (stride << 3); j += stride)		{
			unsigned char A,B,C,D,E,F;
			result = 0;
			A = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			B = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			C = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			D = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			E = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			F = refPicLXl[pres_x];
			pres_x -= (PicWidthSamples << 2);
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;
	
			ptr_img[j] = (unsigned char) ((ptr_img[j] + CLIP255_16(result >> 5) + 1) >> 1);
		}
	}

#endif

}



/**
This function allows to get the luminance prediction of a non IDR picture when xFracl=1 or 3 and yFracl=1 or 3. 

*/
void luma_sample_interp_8x8_3_3(unsigned char image [RESTRICT],	unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples,const short stride){

#ifdef MMX

	short j, pres_x;
	unsigned char *tmp_ref = &refPicLXl[PicWidthSamples - 2];
	unsigned char *ptr_img = &image[0];

	__m64 ref64_1, ref64_2, ref64_3, ref64_4;
	const __m64 cst16 = _mm_set_pi32 (0x00100010, 0x00100010);
	const __m64 cst0 = _mm_setzero_si64();
	const __m64 cst1 = _mm_set_pi32 (0x00010001, 0x00010001);

	for (j = 0; j < 8; j++,ptr_img += stride, tmp_ref += PicWidthSamples){		

#ifdef POCKET_PC
		ref64_1 = _mm_set_pi8 (tmp_ref[7],tmp_ref[6],tmp_ref[5],tmp_ref[4],tmp_ref[3],tmp_ref[2],tmp_ref[1],tmp_ref[0]);
		ref64_2 = _mm_set_pi8 (0,0,0,tmp_ref[12],tmp_ref[11],tmp_ref[10],tmp_ref[9],tmp_ref[8]);
#else
		ref64_1 = *((__m64*)&tmp_ref[0]);
		ref64_2 = *((__m64*)&tmp_ref[8]);
#endif

		ref64_3.m64_i16[0] = ref64_1.m64_u8[0] + ref64_1.m64_u8[5] - 5 * (short)(ref64_1.m64_u8[1] + ref64_1.m64_u8[4]) + 20 * (short)(ref64_1.m64_u8[2] + ref64_1.m64_u8[3]);
		ref64_3.m64_i16[1] = ref64_1.m64_u8[1] + ref64_1.m64_u8[6] - 5 * (short)(ref64_1.m64_u8[2] + ref64_1.m64_u8[5]) + 20 * (short)(ref64_1.m64_u8[3] + ref64_1.m64_u8[4]);
		ref64_3.m64_i16[2] = ref64_1.m64_u8[2] + ref64_1.m64_u8[7] - 5 * (short)(ref64_1.m64_u8[3] + ref64_1.m64_u8[6]) + 20 * (short)(ref64_1.m64_u8[4] + ref64_1.m64_u8[5]);
		ref64_3.m64_i16[3] = ref64_1.m64_u8[3] + ref64_2.m64_u8[0] - 5 * (short)(ref64_1.m64_u8[4] + ref64_1.m64_u8[7]) + 20 * (short)(ref64_1.m64_u8[5] + ref64_1.m64_u8[6]);
		ref64_4.m64_i16[0] = ref64_1.m64_u8[4] + ref64_2.m64_u8[1] - 5 * (short)(ref64_1.m64_u8[5] + ref64_2.m64_u8[0]) + 20 * (short)(ref64_1.m64_u8[6] + ref64_1.m64_u8[7]);
		ref64_4.m64_i16[1] = ref64_1.m64_u8[5] + ref64_2.m64_u8[2] - 5 * (short)(ref64_1.m64_u8[6] + ref64_2.m64_u8[1]) + 20 * (short)(ref64_1.m64_u8[7] + ref64_2.m64_u8[0]);
		ref64_4.m64_i16[2] = ref64_1.m64_u8[6] + ref64_2.m64_u8[3] - 5 * (short)(ref64_1.m64_u8[7] + ref64_2.m64_u8[2]) + 20 * (short)(ref64_2.m64_u8[0] + ref64_2.m64_u8[1]);
		ref64_4.m64_i16[3] = ref64_1.m64_u8[7] + ref64_2.m64_u8[4] - 5 * (short)(ref64_2.m64_u8[0] + ref64_2.m64_u8[3]) + 20 * (short)(ref64_2.m64_u8[1] + ref64_2.m64_u8[2]);
		
		ref64_3 = _mm_add_pi16(ref64_3,cst16);
		ref64_3 = _mm_srai_pi16(ref64_3,5);
		ref64_4 = _mm_add_pi16(ref64_4,cst16);
		ref64_4 = _mm_srai_pi16(ref64_4,5); 
		ref64_3 = _mm_packs_pu16(ref64_3,ref64_4);

#ifdef POCKET_PC
		*((unsigned int*)&ptr_img[0]) = ref64_3.m64_i32[0];
		*((unsigned int*)&ptr_img[4]) = ref64_3.m64_i32[1];
#else
		*((__m64*)&ptr_img[0]) = ref64_3;
#endif
	}

	ptr_img = &image[0];

	for (j = 0; j < 8; j ++)
	{
		__m64 A64,B64,C64,D64,E64,F64;
		pres_x =  1 + (j - 2) * PicWidthSamples; 

#ifdef POCKET_PC
		A64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		B64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		C64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		D64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		E64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
		pres_x += PicWidthSamples;
		F64 = _mm_set_pi8 (refPicLXl[pres_x+7],refPicLXl[pres_x+6],refPicLXl[pres_x+5],refPicLXl[pres_x+4],refPicLXl[pres_x+3],refPicLXl[pres_x+2],refPicLXl[pres_x+1],refPicLXl[pres_x+0]);
#else
		A64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		B64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		C64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		D64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		E64 = *((__m64*)&refPicLXl[pres_x]);
		pres_x += PicWidthSamples;
		F64 = *((__m64*)&refPicLXl[pres_x]);
#endif

		ref64_1.m64_i16[0] = A64.m64_u8[0] + F64.m64_u8[0] - 5 * (short)(B64.m64_u8[0] + E64.m64_u8[0]) + 20 * (short)(C64.m64_u8[0] + D64.m64_u8[0]);
		ref64_1.m64_i16[1] = A64.m64_u8[1] + F64.m64_u8[1] - 5 * (short)(B64.m64_u8[1] + E64.m64_u8[1]) + 20 * (short)(C64.m64_u8[1] + D64.m64_u8[1]);
		ref64_1.m64_i16[2] = A64.m64_u8[2] + F64.m64_u8[2] - 5 * (short)(B64.m64_u8[2] + E64.m64_u8[2]) + 20 * (short)(C64.m64_u8[2] + D64.m64_u8[2]);
		ref64_1.m64_i16[3] = A64.m64_u8[3] + F64.m64_u8[3] - 5 * (short)(B64.m64_u8[3] + E64.m64_u8[3]) + 20 * (short)(C64.m64_u8[3] + D64.m64_u8[3]);
		ref64_2.m64_i16[0] = A64.m64_u8[4] + F64.m64_u8[4] - 5 * (short)(B64.m64_u8[4] + E64.m64_u8[4]) + 20 * (short)(C64.m64_u8[4] + D64.m64_u8[4]);
		ref64_2.m64_i16[1] = A64.m64_u8[5] + F64.m64_u8[5] - 5 * (short)(B64.m64_u8[5] + E64.m64_u8[5]) + 20 * (short)(C64.m64_u8[5] + D64.m64_u8[5]);
		ref64_2.m64_i16[2] = A64.m64_u8[6] + F64.m64_u8[6] - 5 * (short)(B64.m64_u8[6] + E64.m64_u8[6]) + 20 * (short)(C64.m64_u8[6] + D64.m64_u8[6]);
		ref64_2.m64_i16[3] = A64.m64_u8[7] + F64.m64_u8[7] - 5 * (short)(B64.m64_u8[7] + E64.m64_u8[7]) + 20 * (short)(C64.m64_u8[7] + D64.m64_u8[7]);

		ref64_1 = _mm_add_pi16(ref64_1,cst16);
		ref64_1 = _mm_srai_pi16(ref64_1,5);
		ref64_2 = _mm_add_pi16(ref64_2,cst16);
		ref64_2 = _mm_srai_pi16(ref64_2,5);
		ref64_1 = _mm_packs_pu16(ref64_1,ref64_2);

		ref64_2 =_mm_unpackhi_pi8(ref64_1,cst0);
		ref64_1 =_mm_unpacklo_pi8(ref64_1,cst0);

#ifdef POCKET_PC
		ref64_3 = _mm_set_pi32 (*((unsigned int*)&ptr_img[j*stride + 4]),*((unsigned int*)&ptr_img[j*stride]));
#else
		ref64_3 = *((__m64*)&ptr_img[j*stride]);
#endif
		ref64_4 =_mm_unpackhi_pi8(ref64_3,cst0);
		ref64_3 =_mm_unpacklo_pi8(ref64_3,cst0);

		ref64_1 = _mm_add_pi16(ref64_1,ref64_3);
		ref64_1 = _mm_add_pi16(ref64_1,cst1);
		ref64_1 = _mm_srai_pi16(ref64_1,1);

		ref64_2 = _mm_add_pi16(ref64_2,ref64_4);
		ref64_2 = _mm_add_pi16(ref64_2,cst1);
		ref64_2 = _mm_srai_pi16(ref64_2,1);

		ref64_1 = _mm_packs_pu16(ref64_1,ref64_2);

#ifdef POCKET_PC
		*((unsigned int*)&ptr_img[j * stride]) = ref64_1.m64_u32[0];
		*((unsigned int*)&ptr_img[j * stride + 4]) = ref64_1.m64_u32[1];
#else
		*((__m64*)&ptr_img[j * stride]) = ref64_1;
#endif

	}


	empty();    

#else

	short i,j, result, pres_x;
	unsigned char *tmp_ref = &refPicLXl[PicWidthSamples - 2];
	unsigned char *ptr_img = &image[0];

	for (j = 0; j < 8; j++,ptr_img += stride, tmp_ref += PicWidthSamples){		
		for (i = 0; i < 8; i++)	{
			unsigned char A,B,C,D,E,F;
			A = tmp_ref[0 + i];
			B = tmp_ref[1 + i]; 
			C = tmp_ref[2 + i];
			D = tmp_ref[3 + i];
			E = tmp_ref[4 + i];
			F = tmp_ref[5 + i];

			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;
			ptr_img[i] = (unsigned char) CLIP255_16(result>>5);
		}
	}

	ptr_img = &image[0];
	for (i = 0; i < 8; ptr_img++, i++)	{
		pres_x =  1 + i - 2 * PicWidthSamples; 

		for (j = 0; j < (stride << 3); j += stride)		{
			unsigned char A,B,C,D,E,F;
			result = 0;
			A = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			B = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			C = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			D = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			E = refPicLXl[pres_x];
			pres_x += PicWidthSamples;
			F = refPicLXl[pres_x];
			pres_x -= (PicWidthSamples << 2);
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;
	
			ptr_img[j] = (unsigned char) ((ptr_img[j] + CLIP255_16(result >> 5) + 1) >> 1);
		}
	}

#endif

}
