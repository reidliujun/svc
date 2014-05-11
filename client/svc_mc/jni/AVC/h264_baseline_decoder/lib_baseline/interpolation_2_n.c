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
#include "interpolation_2_n.h"




/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 2 and yFracl = 1.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_2_1(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{
#ifdef MMX

	// Vertical & horizontal interpolation

	int i,Offset,j,result;
	short tmp_resul[8*13];
	unsigned char *ptr_ref = &refPicLXl[-((PicWidthSamples+1)<<1)];
	short *temp = &tmp_resul[0];
	unsigned char *ptr_mg = &image[0];

	__m64 ref64_1, ref64_2, ref64_3, ref64_4;
	const __m64 cst16 = _mm_set_pi32 (0x00100010, 0x00100010);
	const __m64 cst0 = _mm_setzero_si64();
	const __m64 cst1 = _mm_set_pi32 (0x00010001, 0x00010001);


	
	for (j = 0; j < 13; j++, ptr_ref += PicWidthSamples) {
		unsigned char *ptr = &ptr_ref[0];
		for (i = 0; i < 104; i += 13, ptr ++)	{
			unsigned char A,B,C,D,E,F;

			A = ptr[0];
			B = ptr[1];
			C = ptr[2];
			D = ptr[3];
			E = ptr[4];
			F = ptr[5];
			tmp_resul[j + i] = A + F - 5 * (short)(B + E) + 20 * (short)(C + D);
		}
	}


	for (i = 0; i < 8; i++)	{
		short *tmp = &temp[13 * i];
		unsigned char *ptr_img = &image[i];
		for (j = 0; j < 8; j++, ptr_img += stride, tmp ++) {			
			short A,B,C,D,E,F;
			
			A = tmp[0];
			B = tmp[1];
			C = tmp[2];
			D = tmp[3];
			E = tmp[4];
			F = tmp[5];
			
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
			ptr_img[0] = (unsigned char) CLIP255_16(result >> 10);
		} 
	}
		
	Offset = 2;
	for (j = 0; j < 8; j++, Offset+=1-(7*13), ptr_mg += stride)
	{
#ifdef POCKET_PC
		ref64_1 = _mm_set_pi32 (*((unsigned int*)&ptr_mg[4]),*((unsigned int*)&ptr_mg[0]));
#else
		ref64_1 = *((__m64*)&ptr_mg[0]);
#endif
		ref64_2 =_mm_unpackhi_pi8(ref64_1,cst0);
		ref64_1 =_mm_unpacklo_pi8(ref64_1,cst0);

		ref64_3.m64_i16[0] = tmp_resul[Offset];
		Offset += 13;
		ref64_3.m64_i16[1] = tmp_resul[Offset];
		Offset += 13;
		ref64_3.m64_i16[2] = tmp_resul[Offset];
		Offset += 13;
		ref64_3.m64_i16[3] = tmp_resul[Offset];
		Offset += 13;
		
		ref64_4.m64_i16[0] = tmp_resul[Offset];
		Offset += 13;
		ref64_4.m64_i16[1] = tmp_resul[Offset];
		Offset += 13;
		ref64_4.m64_i16[2] = tmp_resul[Offset];
		Offset += 13;
		ref64_4.m64_i16[3] = tmp_resul[Offset];

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
		*((unsigned int*)&ptr_mg[0]) = ref64_3.m64_u32[0];
		*((unsigned int*)&ptr_mg[4]) = ref64_3.m64_u32[1];
#else
		*((__m64*)&ptr_mg[0]) = ref64_3;
#endif
	}

	empty();    
 

#else

	// Vertical & horizontal interpolation

	int i,Offset,j,result;
	short tmp_resul[8*13];
	unsigned char *ptr_ref = &refPicLXl[-((PicWidthSamples+1)<<1)];
	short *temp = &tmp_resul[0];
	unsigned char *ptr_mg = &image[0];

	for (j = 0; j < 13; j++, ptr_ref += PicWidthSamples) {
		unsigned char *ptr = &ptr_ref[0];
		for (i = 0; i < 104; i += 13, ptr ++)	{
			unsigned char A,B,C,D,E,F;

			A = ptr[0];
			B = ptr[1];
			C = ptr[2];
			D = ptr[3];
			E = ptr[4];
			F = ptr[5];
			tmp_resul[j + i] = A + F - 5 * (short)(B + E) + 20 * (short)(C + D);
		}
	}
	
	
	for (i = 0; i < 8; i++)	{
		short *tmp = &temp[13 * i];
		unsigned char *ptr_img = &image[i];
		for (j = 0; j < 8; j++, ptr_img += stride, tmp ++) {			
			short A,B,C,D,E,F;
			
			A = tmp[0];
			B = tmp[1];
			C = tmp[2];
			D = tmp[3];
			E = tmp[4];
			F = tmp[5];
			
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
			ptr_img[0] = (unsigned char) CLIP255_16(result >> 10);
		} 
	}
		
		
	Offset = 2;
	for (j = 0; j < 8; j++, Offset+=1-(7*13), ptr_mg += stride)
	{
		ptr_mg[0] = (unsigned char) ((ptr_mg[0] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[1] = (unsigned char) ((ptr_mg[1] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[2] = (unsigned char) ((ptr_mg[2] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[3] = (unsigned char) ((ptr_mg[3] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[4] = (unsigned char) ((ptr_mg[4] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[5] = (unsigned char) ((ptr_mg[5] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[6] = (unsigned char) ((ptr_mg[6] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[7] = (unsigned char) ((ptr_mg[7] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
	}

#endif
}


/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 2 and yFracl = 2.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/

void luma_sample_interp_8x8_2_2(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{

#ifdef MMX
	int i,j,result;
	short tmp_resul[8*13];
	unsigned char *ptr_ref = &refPicLXl[-((PicWidthSamples+1)<<1)];
	short *temp = &tmp_resul[0];
	
	for (j = 0; j < 13; j++, ptr_ref += PicWidthSamples) {
		unsigned char *ptr = &ptr_ref[0];
		for (i = 0; i < 104; i += 13, ptr ++)	{
			unsigned char A,B,C,D,E,F;

			A = ptr[0];
			B = ptr[1];
			C = ptr[2];
			D = ptr[3];
			E = ptr[4];
			F = ptr[5];
			tmp_resul[j + i] = A + F - 5 * (short)(B + E) + 20 * (short)(C + D);
		}
	}

	for (i = 0; i < 8; i++)	{
		short *tmp = &temp[13 * i];
		unsigned char *ptr_img = &image[i];
		for (j = 0; j < 8; j++, ptr_img += stride, tmp ++) {			
			short A,B,C,D,E,F;
			
			A = tmp[0];
			B = tmp[1];
			C = tmp[2];
			D = tmp[3];
			E = tmp[4];
			F = tmp[5];
			
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
			ptr_img[0] = (unsigned char) CLIP255_16(result >> 10);
		} 
	}


	empty();    
 
#else
#ifdef TI_OPTIM
	// Vertical & horizontal interpolation 
	int j;
	short tmp_resul[8*13];
	short *temp;
  {
	unsigned int i_tempC, i_tempD;
	int 		 i_tempA;
	unsigned int uc_Ref1,uc_Ref2,uc_Ref3,uc_Ref4,uc_Ref5,uc_Ref6,uc_Ref7,uc_Ref8,uc_Ref9,uc_Ref10,uc_Ref11, uc_Ref12;
	unsigned int ui_Ref2By5,ui_Ref3By5,ui_Ref4By5,ui_Ref5By5,ui_Ref6By5,ui_Ref7By5,ui_Ref8By5;
	unsigned int ui_Ref9By5,ui_Ref10By5,ui_Ref11By5,ui_Ref12By5;
	unsigned char *ptr_ref = &refPicLXl[-((PicWidthSamples+1)<<1)];

	#pragma MUST_ITERATE(13,13,13)
	for (j = 0; j < 13; j++, ptr_ref += PicWidthSamples) {
		{
			long long ll_load = _mem8_const(ptr_ref);
			i_tempD = (ll_load>>32); 			// |L |K |J |I |
			i_tempC = (unsigned int)(ll_load);	// |H |G |F |E |
			//i_tempC = _mem4(ptr_ref); //i_tempD = _mem4(ptr_ref+4);
		}
		temp = &tmp_resul[j];
		uc_Ref1 = _extu(i_tempC,24,24);		// E
		uc_Ref2 = _extu(i_tempC,16,24);		// F
		uc_Ref3 = _extu(i_tempC,8,24);		// G
		uc_Ref4 = _extu(i_tempC,0,24);		// H
		ui_Ref2By5 = _mpyu(5,uc_Ref2);		// 5*F
		ui_Ref3By5 = _mpyu(5,uc_Ref3);		// 5*G
		ui_Ref4By5 = _mpyu(5,uc_Ref4);		// 5*H

		uc_Ref5 = _extu(i_tempD,24,24);		// I
		uc_Ref6 = _extu(i_tempD,16,24);		// J
		uc_Ref7 = _extu(i_tempD,8,24);		// K
		uc_Ref8 = _extu(i_tempD,0,24);		// L
		ui_Ref5By5  = _mpyu(5,uc_Ref5);		// 5*I
		ui_Ref6By5  = _mpyu(5,uc_Ref6);		// 5*J
		ui_Ref7By5  = _mpyu(5,uc_Ref7);		// 5*K
		ui_Ref8By5  = _mpyu(5,uc_Ref8);		// 5*L

		i_tempC = _packlh2(i_tempD,i_tempC); // |JIHG|

		*temp = uc_Ref1 -ui_Ref2By5 +((ui_Ref3By5 +ui_Ref4By5)<<2) -ui_Ref5By5 +uc_Ref6; //b1
		temp += 13;
		*temp = uc_Ref2 -ui_Ref3By5 +((ui_Ref4By5 +ui_Ref5By5)<<2) -ui_Ref6By5 +uc_Ref7; //b2
		temp += 13;
		*temp = uc_Ref3 -ui_Ref4By5 +((ui_Ref5By5 +ui_Ref6By5)<<2) -ui_Ref7By5 +uc_Ref8; //b3
		temp += 13;

		i_tempA = _mem4(ptr_ref+8);	// |P |O |N |M |

		uc_Ref9  = _extu(i_tempA,24,24) ;	// M
		uc_Ref10 = _extu(i_tempA,16,24);	// N
		uc_Ref11 = _extu(i_tempA,8,24);		// O
		uc_Ref12 = _extu(i_tempA,0,24);		// P
		ui_Ref9By5  = _mpyu(5,uc_Ref9);		// 5*M
		ui_Ref10By5 = _mpyu(5,uc_Ref10);	// 5*N
		ui_Ref11By5 = _mpyu(5,uc_Ref11);	// 5*O	//	ui_Ref11By20= _mpyu(20,uc_Ref11);	// 20*O
		ui_Ref12By5 = _mpyu(5,uc_Ref12);	// 5*P

		*temp = uc_Ref4 -ui_Ref5By5 +((ui_Ref6By5 +ui_Ref7By5)<<2) -ui_Ref8By5 +uc_Ref9;			//b4
		temp += 13;
		*temp = uc_Ref5 -ui_Ref6By5 +((ui_Ref7By5 +ui_Ref8By5)<<2) -ui_Ref9By5  +uc_Ref10; 		//b5 
		temp += 13;
		*temp = uc_Ref6 -ui_Ref7By5 +((ui_Ref8By5 +ui_Ref9By5)<<2) -ui_Ref10By5 +uc_Ref11; 		//b6 
		temp += 13;
		*temp = uc_Ref7 -ui_Ref8By5 +((ui_Ref9By5 +ui_Ref10By5)<<2)-ui_Ref11By5 +uc_Ref12;	 	//b7 
		temp += 13;
		*temp = uc_Ref8 -ui_Ref9By5 +((ui_Ref10By5+ui_Ref11By5)<<2)-ui_Ref12By5+*(ptr_ref+12); 	//b8 
	}
  }
	// Calcul des valeurs de j	
  {
	int A,B,C,D,E,F,G,H; 
	short *tmp = &tmp_resul[0];
	#pragma MUST_ITERATE(8,8,8)
	for (j = 0; j < 8; j++)	{
		long long ll_load;
		int i_high, i_low;
		int I,J,K,L;//,M;
		int B5,C5,D5,E5,F5,G5,H5,I5,J5,K5,L5;
		unsigned char *ptr_img = &image[j];

		ll_load = _mem8(tmp);
		i_high = ll_load>>32;	i_low = (int)ll_load;
		A = (short)(i_low);		B = (short)(i_low>>16);
		C = (short)(i_high);	D = (short)(i_high>>16);
		B5 = _mpylh(5,i_low);
		C5 = _mpy(5,i_high);
		D5 = _mpylh(5,i_high);
		ll_load = _mem8(tmp+4);
		i_high = ll_load>>32;	i_low = (int)ll_load;
		E = (short)(i_low);		F = (short)(i_low>>16);
		G = (short)(i_high);	H = (short)(i_high>>16);
		E5 = _mpy(5,i_low);
		F5 = _mpylh(5,i_low);
		G5 = _mpy(5,i_high);
		H5 = _mpylh(5,i_high);
		ll_load = _mem8(tmp+8);
		i_high = ll_load>>32;	i_low = (int)ll_load;
		I = (short)(i_low);		J = (short)(i_low>>16);
		K = (short)(i_high);	L = (short)(i_high>>16);
//			M = tmp[12];
		I5 = _mpy(5,i_low);
		J5 = _mpylh(5,i_low);
		K5 = _mpy(5,i_high);
		L5 = _mpylh(5,i_high);

		A = A +F -(B5 +E5) +((C5 +D5)<<2) +512;
		*ptr_img = _spacku4(0,A >> 10); ptr_img +=stride;
		B = B +G -(C5 +F5) +((D5 +E5)<<2) +512;
		*ptr_img = _spacku4(0,B >> 10); ptr_img +=stride;
		C = C +H -(D5 +G5) +((E5 +F5)<<2) +512;
		*ptr_img = _spacku4(0,C >> 10); ptr_img +=stride;
		D = D +I -(E5 +H5) +((F5 +G5)<<2) +512;
		*ptr_img = _spacku4(0,D >> 10); ptr_img +=stride;
		E = E +J -(F5 +I5) +((G5 +H5)<<2) +512;
		*ptr_img = _spacku4(0,E >> 10); ptr_img +=stride;
		F = F +K -(G5 +J5) +((H5 +I5)<<2) +512;
		*ptr_img = _spacku4(0,F >> 10); ptr_img +=stride;
		G = G +L -(H5 +K5) +((I5 +J5)<<2) +512;
		*ptr_img = _spacku4(0,G >> 10); ptr_img +=stride;
		H = H +tmp[12] -(I5 +L5) +((J5 +K5)<<2) +512;
		*ptr_img = _spacku4(0,H >> 10);

		tmp +=13;
	}
  }
#else
	// Vertical & horizontal interpolation
	int i,j,result;
	short tmp_resul[8*13];
	unsigned char *ptr_ref = &refPicLXl[-((PicWidthSamples+1)<<1)];
	short *temp = &tmp_resul[0];

	for (j = 0; j < 13; j++, ptr_ref += PicWidthSamples) {
		unsigned char *ptr = &ptr_ref[0];
		for (i = 0; i < 104; i += 13, ptr ++)	{
			unsigned char A,B,C,D,E,F;

			A = ptr[0];
			B = ptr[1];
			C = ptr[2];
			D = ptr[3];
			E = ptr[4];
			F = ptr[5];
			tmp_resul[j + i] = A + F - 5 * (short)(B + E) + 20 * (short)(C + D);
		}
	}
	
	
	for (i = 0; i < 8; i++)	{
		short *tmp = &temp[13 * i];
		unsigned char *ptr_img = &image[i];
		for (j = 0; j < 8; j++, ptr_img += stride, tmp ++) {			
			short A,B,C,D,E,F;
			
			A = tmp[0];
			B = tmp[1];
			C = tmp[2];
			D = tmp[3];
			E = tmp[4];
			F = tmp[5];
			
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
			ptr_img[0] = (unsigned char) CLIP255_16(result >> 10);
		} 
	}
#endif
#endif
}



/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 2 and yFracl = 3.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_2_3(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{
#ifdef MMX

	int i,Offset,j,result;
	short tmp_resul[8*13];
	unsigned char *ptr_ref = &refPicLXl[-((PicWidthSamples+1)<<1)];
	short *temp = &tmp_resul[0];
	unsigned char *ptr_mg = &image[0];

	__m64 ref64_1, ref64_2, ref64_3, ref64_4;
	const __m64 cst16 = _mm_set_pi32 (0x00100010, 0x00100010);
	const __m64 cst0 = _mm_setzero_si64();
	const __m64 cst1 = _mm_set_pi32 (0x00010001, 0x00010001);
	
	for (j = 0; j < 13; j++, ptr_ref += PicWidthSamples) {
		unsigned char *ptr = &ptr_ref[0];
		for (i = 0; i < 104; i += 13, ptr ++)	{
			unsigned char A,B,C,D,E,F;

			A = ptr[0];
			B = ptr[1];
			C = ptr[2];
			D = ptr[3];
			E = ptr[4];
			F = ptr[5];
			tmp_resul[j + i] = A + F - 5 * (short)(B + E) + 20 * (short)(C + D);
		}
	}

	for (i = 0; i < 8; i++)	{
		short *tmp = &temp[13 * i];
		unsigned char *ptr_img = &image[i];
		for (j = 0; j < 8; j++, ptr_img += stride, tmp ++) {			
			short A,B,C,D,E,F;
			
			A = tmp[0];
			B = tmp[1];
			C = tmp[2];
			D = tmp[3];
			E = tmp[4];
			F = tmp[5];
			
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
			ptr_img[0] = (unsigned char) CLIP255_16(result >> 10);
		} 
	}

	Offset = 3;
	for (j = 0; j < 8; j++, Offset+=1-(7*13), ptr_mg += stride)
	{
#ifdef POCKET_PC
		ref64_1 = _mm_set_pi32 (*((unsigned int*)&ptr_mg[4]),*((unsigned int*)&ptr_mg[0]));
#else
		ref64_1 = *((__m64*)&ptr_mg[0]);
#endif
		ref64_2 =_mm_unpackhi_pi8(ref64_1,cst0);
		ref64_1 =_mm_unpacklo_pi8(ref64_1,cst0);

		ref64_3.m64_i16[0] = tmp_resul[Offset];
		Offset += 13;
		ref64_3.m64_i16[1] = tmp_resul[Offset];
		Offset += 13;
		ref64_3.m64_i16[2] = tmp_resul[Offset];
		Offset += 13;
		ref64_3.m64_i16[3] = tmp_resul[Offset];
		Offset += 13;
		
		ref64_4.m64_i16[0] = tmp_resul[Offset];
		Offset += 13;
		ref64_4.m64_i16[1] = tmp_resul[Offset];
		Offset += 13;
		ref64_4.m64_i16[2] = tmp_resul[Offset];
		Offset += 13;
		ref64_4.m64_i16[3] = tmp_resul[Offset];

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
		*((unsigned int*)&ptr_mg[0]) = ref64_3.m64_u32[0];
		*((unsigned int*)&ptr_mg[4]) = ref64_3.m64_u32[1];
#else
		*((__m64*)&ptr_mg[0]) = ref64_3;
#endif
	}


	empty();    
#else

	// Vertical & horizontal interpolation

	int i,Offset,j,result;
	short tmp_resul[8*13];
	unsigned char *ptr_ref = &refPicLXl[-((PicWidthSamples+1)<<1)];
	short *temp = &tmp_resul[0];
	unsigned char *ptr_mg = &image[0];

	for (j = 0; j < 13; j++, ptr_ref += PicWidthSamples) {
		unsigned char *ptr = &ptr_ref[0];
		for (i = 0; i < 104; i += 13, ptr ++)	{
			unsigned char A,B,C,D,E,F;

			A = ptr[0];
			B = ptr[1];
			C = ptr[2];
			D = ptr[3];
			E = ptr[4];
			F = ptr[5];
			tmp_resul[j + i] = A + F - 5 * (short)(B + E) + 20 * (short)(C + D);
		}
	}
	
	
	for (i = 0; i < 8; i++)	{
		short *tmp = &temp[13 * i];
		unsigned char *ptr_img = &image[i];
		for (j = 0; j < 8; j++, ptr_img += stride, tmp ++) {			
			short A,B,C,D,E,F;
			
			A = tmp[0];
			B = tmp[1];
			C = tmp[2];
			D = tmp[3];
			E = tmp[4];
			F = tmp[5];
			
			result = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 512;
			ptr_img[0] = (unsigned char) CLIP255_16(result >> 10);
		} 
	}
		
	Offset = 3;
	for (j = 0; j < 8; j++, Offset+=1-(7*13), ptr_mg += stride)
	{
		ptr_mg[0] = (unsigned char) ((ptr_mg[0] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[1] = (unsigned char) ((ptr_mg[1] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[2] = (unsigned char) ((ptr_mg[2] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[3] = (unsigned char) ((ptr_mg[3] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[4] = (unsigned char) ((ptr_mg[4] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[5] = (unsigned char) ((ptr_mg[5] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[6] = (unsigned char) ((ptr_mg[6] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
		Offset += 13;
		ptr_mg[7] = (unsigned char) ((ptr_mg[7] + CLIP255_16((tmp_resul[Offset] + 16) >> 5) + 1) >> 1);
	}

#endif
}

