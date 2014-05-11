/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *			  Alain  MACCARI <alain.maccari@ens.insa-rennes.fr>
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
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 1 and yFracl = 0.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_1_0(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{

#ifdef TI_OPTIM
	int b1,b2,b3,b4,b5,b6,b7,b8;
	unsigned int i_tempC, i_tempD;
	int 		 i_tempA, i_tempB;
	unsigned char *ptr_deb = &refPicLXl[-2];
	unsigned char *ptr_imge = &image[0];
	char i;
	#pragma MUST_ITERATE(8,8,8)
	for (i=0; i<8; i++) {
		{
			unsigned int uc_Ref1, uc_Ref2, uc_Ref3, uc_Ref4;
			unsigned int uc_Ref5, uc_Ref6, uc_Ref7, uc_Ref8;
			unsigned int uc_Ref9, uc_Ref10, uc_Ref11, uc_Ref12;//, uc_Ref13;
			unsigned int ui_Ref2By5,ui_Ref3By5,ui_Ref4By5,ui_Ref5By5,ui_Ref6By5,ui_Ref7By5,ui_Ref8By5;
			unsigned int ui_Ref9By5,ui_Ref10By5,ui_Ref11By5,ui_Ref12By5;
			{
				unsigned long long ull_load = _mem8(ptr_deb);
				i_tempD = (ull_load)>>32; 			// |L |K |J |I |
				i_tempC = (unsigned int) ull_load;	// |H |G |F |E |
			}
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

			b1 = uc_Ref1 -ui_Ref2By5 +((ui_Ref3By5 +ui_Ref4By5)<<2) -ui_Ref5By5 +uc_Ref6;
			b2 = uc_Ref2 -ui_Ref3By5 +((ui_Ref4By5 +ui_Ref5By5)<<2) -ui_Ref6By5 +uc_Ref7;
			b3 = uc_Ref3 -ui_Ref4By5 +((ui_Ref5By5 +ui_Ref6By5)<<2) -ui_Ref7By5 +uc_Ref8;
			b4 = uc_Ref4 -ui_Ref5By5 +((ui_Ref6By5 +ui_Ref7By5)<<2) -ui_Ref8By5;

			i_tempA = _mem4_const(ptr_deb+8);	// |P |O |N |M |

			uc_Ref9  = _extu(i_tempA,24,24) ;	// M
			uc_Ref10 = _extu(i_tempA,16,24);	// N
			uc_Ref11 = _extu(i_tempA,8,24);		// O
			uc_Ref12 = _extu(i_tempA,0,24);		// P
			ui_Ref9By5  = _mpyu(5,uc_Ref9);		// 5*M
			ui_Ref10By5 = _mpyu(5,uc_Ref10);	// 5*N
			ui_Ref11By5 = _mpyu(5,uc_Ref11);	// 5*O
			ui_Ref12By5 = _mpyu(5,uc_Ref12);	// 5*P
//			uc_Ref13 = *(ptr_deb+12);			// Q
			b4 +=  uc_Ref9;
			b5 = uc_Ref5 -ui_Ref6By5 +((ui_Ref7By5 +ui_Ref8By5)<<2) -ui_Ref9By5  +uc_Ref10;
			b6 = uc_Ref6 -ui_Ref7By5 +((ui_Ref8By5  +ui_Ref9By5)<<2)  -ui_Ref10By5 +uc_Ref11;
			b7 = uc_Ref7 -ui_Ref8By5 +((ui_Ref9By5  +ui_Ref10By5)<<2) -ui_Ref11By5 +uc_Ref12;
			b8 = uc_Ref8 -ui_Ref9By5 +((ui_Ref10By5 +ui_Ref11By5)<<2) -ui_Ref12By5 +*(ptr_deb+12);
		}
		i_tempD = _packlh2(i_tempA,i_tempD) ; 	// |NMLK|

		i_tempA = _pack2(b2,b1);				//|pix2| pix1|
		i_tempA = _add2(0x00100010,i_tempA);	// b1 +16
		i_tempA = _shr2(i_tempA,5);				// b = (b1 +16)>>5

		i_tempB = _pack2(b4,b3);
		i_tempB = _add2(0x00100010,i_tempB);	// b1 +16
		i_tempB = _shr2(i_tempB,5);				// b = (b1 +16)>>5

		i_tempB = _spacku4(i_tempB,i_tempA);	// Clip((b1 +16)>>5)
		i_tempA = _avgu4(i_tempB,i_tempC);		// a = (G +b +1)>>1

		_mem4(ptr_imge) = i_tempA ;

		i_tempA = _pack2(b6,b5);				//|pix6| pix5|
		i_tempA = _add2(0x00100010,i_tempA);	// b1 +16
		i_tempA = _shr2(i_tempA,5);				// b = (b1 +16)>>5

		i_tempB = _pack2(b8,b7);
		i_tempB = _add2(0x00100010,i_tempB);	// b1 +16
		i_tempB = _shr2(i_tempB,5);				// b = (b1 +16)>>5

		i_tempB = _spacku4(i_tempB,i_tempA);	// Clip((b1 +16)>>5)
		i_tempB = _avgu4(i_tempB,i_tempD);		// a = (G +b +1)>>1

		_mem4(ptr_imge+4) = i_tempB ;

		ptr_imge+=stride;
		ptr_deb +=PicWidthSamples;
	}
#else
#ifdef MMX

	unsigned char *ptr_img =  &refPicLXl[-2];
	unsigned char *ptr_imge = &image[0];

	__m64 temp64,temp64_2,temp64_3,temp64_4;

	int j,i;
	for (i = 0; i < 8; ptr_img += PicWidthSamples, ptr_imge += stride, i++) {

#ifdef POCKET_PC
		temp64 = _mm_set_pi8 (ptr_img[7],ptr_img[6],ptr_img[5],ptr_img[4],ptr_img[3],ptr_img[2],ptr_img[1],ptr_img[0]);
		temp64_2 = _mm_set_pi8 (0,0,0,ptr_img[12],ptr_img[11],ptr_img[10],ptr_img[9],ptr_img[8]);
#else
		temp64 = *((__m64*)&ptr_img[0]);
		temp64_2 = *((__m64*)&ptr_img[8]);
#endif

		temp64_3.m64_i16[0] = temp64.m64_u8[0] + temp64.m64_u8[5] - 5 * (short)(temp64.m64_u8[1] + temp64.m64_u8[4]) + 20 * (short)(temp64.m64_u8[2] + temp64.m64_u8[3]);
		temp64_3.m64_i16[1] = temp64.m64_u8[1] + temp64.m64_u8[6] - 5 * (short)(temp64.m64_u8[2] + temp64.m64_u8[5]) + 20 * (short)(temp64.m64_u8[3] + temp64.m64_u8[4]);
		temp64_3.m64_i16[2] = temp64.m64_u8[2] + temp64.m64_u8[7] - 5 * (short)(temp64.m64_u8[3] + temp64.m64_u8[6]) + 20 * (short)(temp64.m64_u8[4] + temp64.m64_u8[5]);
		temp64_3.m64_i16[3] = temp64.m64_u8[3] + temp64_2.m64_u8[0] - 5 * (short)(temp64.m64_u8[4] + temp64.m64_u8[7]) + 20 * (short)(temp64.m64_u8[5] + temp64.m64_u8[6]);
		temp64_4.m64_i16[0] = temp64.m64_u8[4] + temp64_2.m64_u8[1] - 5 * (short)(temp64.m64_u8[5] + temp64_2.m64_u8[0]) + 20 * (short)(temp64.m64_u8[6] + temp64.m64_u8[7]);
		temp64_4.m64_i16[1] = temp64.m64_u8[5] + temp64_2.m64_u8[2] - 5 * (short)(temp64.m64_u8[6] + temp64_2.m64_u8[1]) + 20 * (short)(temp64.m64_u8[7] + temp64_2.m64_u8[0]);
		temp64_4.m64_i16[2] = temp64.m64_u8[6] + temp64_2.m64_u8[3] - 5 * (short)(temp64.m64_u8[7] + temp64_2.m64_u8[2]) + 20 * (short)(temp64_2.m64_u8[0] + temp64_2.m64_u8[1]);
		temp64_4.m64_i16[3] = temp64.m64_u8[7] + temp64_2.m64_u8[4] - 5 * (short)(temp64_2.m64_u8[0] + temp64_2.m64_u8[3]) + 20 * (short)(temp64_2.m64_u8[1] + temp64_2.m64_u8[2]);
		
		temp64_3 = _mm_add_pi16(temp64_3,_mm_set_pi32 (0x00100010, 0x00100010));
		temp64_3 = _mm_srai_pi16(temp64_3,5);
		
		temp64_4 = _mm_add_pi16(temp64_4,_mm_set_pi32 (0x00100010, 0x00100010));
		temp64_4 = _mm_srai_pi16(temp64_4,5);
		
#ifdef POCKET_PC
		temp64_3 = _mm_packs_pu16(temp64_3,temp64_4);
		*((unsigned int*)&ptr_imge[0]) = temp64_3.m64_i32[0];
		*((unsigned int*)&ptr_imge[4]) = temp64_3.m64_i32[1];
#else
		*((__m64*)&ptr_imge[0]) = _mm_packs_pu16(temp64_3,temp64_4);
#endif
	}
	
	empty();    

	ptr_img = &refPicLXl[0];
	for (j = 0; j < 8; j++){
		image[0] = ( image[0] + ptr_img[0] +1) >> 1;
		image[1] = ( image[1] + ptr_img[1] + 1) >> 1;
		image[2] = ( image[2] + ptr_img[2] + 1) >> 1;
		image[3] = ( image[3] + ptr_img[3] + 1) >> 1;
		image[4] = ( image[4] + ptr_img[4] + 1) >> 1;
		image[5] = ( image[5] + ptr_img[5] + 1) >> 1;
		image[6] = ( image[6] + ptr_img[6] + 1) >> 1;
		image[7] = ( image[7] + ptr_img[7] + 1) >> 1;
		ptr_img += PicWidthSamples;
		image += stride;
	}


#else

	unsigned char *ptr_img =  &refPicLXl[-2];
	unsigned char *ptr_imge = &image[0];
	unsigned char A,B,C,D,E,F,G,H,I,J,K,L,M;
	short result0,result1,result2,result3,result4,result5,result6,result7;

	int j,i;
	for (i = 0; i < 8; ptr_img += PicWidthSamples, ptr_imge += stride, i++) {
				
		A = ptr_img[0];
		B = ptr_img[1];
		C = ptr_img[2];
		D = ptr_img[3];
		E = ptr_img[4];
		F = ptr_img[5];
		G = ptr_img[6];
		H = ptr_img[7];
		I = ptr_img[8];
		J = ptr_img[9];
		K = ptr_img[10];
		L = ptr_img[11];
		M = ptr_img[12];

		result0 = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;
		result1 = B + G - 5 * (short)(C + F) + 20 * (short)(D + E) + 16;
		result2 = C + H - 5 * (short)(D + G) + 20 * (short)(E + F) + 16;
		result3 = D + I - 5 * (short)(E + H) + 20 * (short)(F + G) + 16;
		result4 = E + J - 5 * (short)(F + I) + 20 * (short)(G + H) + 16;
		result5 = F + K - 5 * (short)(G + J) + 20 * (short)(H + I) + 16;
		result6 = G + L - 5 * (short)(H + K) + 20 * (short)(I + J) + 16;
		result7 = H + M - 5 * (short)(I + L) + 20 * (short)(J + K) + 16;

		ptr_imge[0] = (unsigned char) CLIP255_16((result0) >> 5);
		ptr_imge[1] = (unsigned char) CLIP255_16((result1) >> 5);
		ptr_imge[2] = (unsigned char) CLIP255_16((result2) >> 5);
		ptr_imge[3] = (unsigned char) CLIP255_16((result3) >> 5);
		ptr_imge[4] = (unsigned char) CLIP255_16((result4) >> 5);
		ptr_imge[5] = (unsigned char) CLIP255_16((result5) >> 5);
		ptr_imge[6] = (unsigned char) CLIP255_16((result6) >> 5);
		ptr_imge[7] = (unsigned char) CLIP255_16((result7) >> 5);
	}
	
	ptr_img = &refPicLXl[0];
	for (j = 0; j < 8; j++){
		image[0] = (unsigned char) ((image[0] + ptr_img[0] + 1) >> 1);
		image[1] = (unsigned char) ((image[1] + ptr_img[1] + 1) >> 1);
		image[2] = (unsigned char) ((image[2] + ptr_img[2] + 1) >> 1);
		image[3] = (unsigned char) ((image[3] + ptr_img[3] + 1) >> 1);
		image[4] = (unsigned char) ((image[4] + ptr_img[4] + 1) >> 1);
		image[5] = (unsigned char) ((image[5] + ptr_img[5] + 1) >> 1);
		image[6] = (unsigned char) ((image[6] + ptr_img[6] + 1) >> 1);
		image[7] = (unsigned char) ((image[7] + ptr_img[7] + 1) >> 1);
		ptr_img += PicWidthSamples;
		image += stride;
	}

#endif
#endif

}






/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 2 and yFracl = 0.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_2_0(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{
#ifdef TI_OPTIM
	int b1,b2,b3,b4,b5,b6,b7,b8;
	unsigned int i_tempC, i_tempD;
	int 		 i_tempA, i_tempB;
	unsigned char *ptr_deb = &refPicLXl[-2];
	unsigned char *ptr_imge = &image[0];
	char i;
	#pragma MUST_ITERATE(8,8,8)
	for (i=0; i<8; i++) {
		{
			unsigned int uc_Ref1, uc_Ref2, uc_Ref3, uc_Ref4;
			unsigned int uc_Ref5, uc_Ref6, uc_Ref7, uc_Ref8;
			unsigned int uc_Ref9, uc_Ref10, uc_Ref11, uc_Ref12;//, uc_Ref13;
			unsigned int ui_Ref2By5,ui_Ref3By5,ui_Ref4By5,ui_Ref5By5,ui_Ref6By5,ui_Ref7By5,ui_Ref8By5;
			unsigned int ui_Ref9By5,ui_Ref10By5,ui_Ref11By5,ui_Ref12By5;
			{
				unsigned long long ull_load = _mem8(ptr_deb);
				i_tempD = (ull_load)>>32; 			// |L |K |J |I |
				i_tempC = (unsigned int) ull_load;	// |H |G |F |E |
			}
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

			b1 = uc_Ref1 -ui_Ref2By5 +((ui_Ref3By5 +ui_Ref4By5)<<2) -ui_Ref5By5 +uc_Ref6;
			b2 = uc_Ref2 -ui_Ref3By5 +((ui_Ref4By5 +ui_Ref5By5)<<2) -ui_Ref6By5 +uc_Ref7;
			b3 = uc_Ref3 -ui_Ref4By5 +((ui_Ref5By5 +ui_Ref6By5)<<2) -ui_Ref7By5 +uc_Ref8;
			b4 = uc_Ref4 -ui_Ref5By5 +((ui_Ref6By5 +ui_Ref7By5)<<2) -ui_Ref8By5;

			i_tempA = _mem4_const(ptr_deb+8);	// |P |O |N |M |

			uc_Ref9  = _extu(i_tempA,24,24) ;	// M
			uc_Ref10 = _extu(i_tempA,16,24);	// N
			uc_Ref11 = _extu(i_tempA,8,24);		// O
			uc_Ref12 = _extu(i_tempA,0,24);		// P
			ui_Ref9By5  = _mpyu(5,uc_Ref9);		// 5*M
			ui_Ref10By5 = _mpyu(5,uc_Ref10);	// 5*N
			ui_Ref11By5 = _mpyu(5,uc_Ref11);	// 5*O
			ui_Ref12By5 = _mpyu(5,uc_Ref12);	// 5*P
//			uc_Ref13 = *(ptr_deb+12);			// Q
			b4 +=  uc_Ref9;
			b5 = uc_Ref5 -ui_Ref6By5 +((ui_Ref7By5 +ui_Ref8By5)<<2) -ui_Ref9By5  +uc_Ref10;
			b6 = uc_Ref6 -ui_Ref7By5 +((ui_Ref8By5 +ui_Ref9By5)<<2) -ui_Ref10By5 +uc_Ref11;
			b7 = uc_Ref7 -ui_Ref8By5 +((ui_Ref9By5 +ui_Ref10By5)<<2)-ui_Ref11By5 +uc_Ref12;
			b8 = uc_Ref8 -ui_Ref9By5 +((ui_Ref10By5+ui_Ref11By5)<<2)-ui_Ref12By5 +*(ptr_deb+12);
		}
		i_tempD = _packlh2(i_tempA,i_tempD) ; 	// |NMLK|

		i_tempA = _pack2(b2,b1);				//|pix2| pix1|
		i_tempA = _add2(0x00100010,i_tempA);	// b1 +16
		i_tempA = _shr2(i_tempA,5);				// b = (b1 +16)>>5

		i_tempB = _pack2(b4,b3);
		i_tempB = _add2(0x00100010,i_tempB);	// b1 +16
		i_tempB = _shr2(i_tempB,5);				// b = (b1 +16)>>5

		i_tempB = _spacku4(i_tempB,i_tempA);	// Clip((b1 +16)>>5)

		_mem4(ptr_imge) = i_tempB ;

		i_tempA = _pack2(b6,b5);				//|pix6| pix5|
		i_tempA = _add2(0x00100010,i_tempA);	// b1 +16
		i_tempA = _shr2(i_tempA,5);				// b = (b1 +16)>>5

		i_tempB = _pack2(b8,b7);
		i_tempB = _add2(0x00100010,i_tempB);	// b1 +16
		i_tempB = _shr2(i_tempB,5);				// b = (b1 +16)>>5

		i_tempB = _spacku4(i_tempB,i_tempA);	// Clip((b1 +16)>>5)

		_mem4(ptr_imge+4) = i_tempB ;

		ptr_imge+=stride;
		ptr_deb +=PicWidthSamples;
	}
#else

#ifdef MMX


	unsigned char *ptr_img =  &refPicLXl[-2];
	unsigned char *ptr_imge = &image[0];

	__m64 temp64,temp64_2,temp64_3,temp64_4;

	int i;
	for (i = 0; i < 8; ptr_img += PicWidthSamples, ptr_imge += stride, i++) {
		
#ifdef POCKET_PC
		temp64 = _mm_set_pi8 (ptr_img[7],ptr_img[6],ptr_img[5],ptr_img[4],ptr_img[3],ptr_img[2],ptr_img[1],ptr_img[0]);
		temp64_2 = _mm_set_pi8 (0,0,0,ptr_img[12],ptr_img[11],ptr_img[10],ptr_img[9],ptr_img[8]);
#else
		temp64 = *((__m64*)&ptr_img[0]);
		temp64_2 = *((__m64*)&ptr_img[8]);
#endif

		temp64_3.m64_i16[0] = temp64.m64_u8[0] + temp64.m64_u8[5] - 5 * (short)(temp64.m64_u8[1] + temp64.m64_u8[4]) + 20 * (short)(temp64.m64_u8[2] + temp64.m64_u8[3]);
		temp64_3.m64_i16[1] = temp64.m64_u8[1] + temp64.m64_u8[6] - 5 * (short)(temp64.m64_u8[2] + temp64.m64_u8[5]) + 20 * (short)(temp64.m64_u8[3] + temp64.m64_u8[4]);
		temp64_3.m64_i16[2] = temp64.m64_u8[2] + temp64.m64_u8[7] - 5 * (short)(temp64.m64_u8[3] + temp64.m64_u8[6]) + 20 * (short)(temp64.m64_u8[4] + temp64.m64_u8[5]);
		temp64_3.m64_i16[3] = temp64.m64_u8[3] + temp64_2.m64_u8[0] - 5 * (short)(temp64.m64_u8[4] + temp64.m64_u8[7]) + 20 * (short)(temp64.m64_u8[5] + temp64.m64_u8[6]);
		temp64_4.m64_i16[0] = temp64.m64_u8[4] + temp64_2.m64_u8[1] - 5 * (short)(temp64.m64_u8[5] + temp64_2.m64_u8[0]) + 20 * (short)(temp64.m64_u8[6] + temp64.m64_u8[7]);
		temp64_4.m64_i16[1] = temp64.m64_u8[5] + temp64_2.m64_u8[2] - 5 * (short)(temp64.m64_u8[6] + temp64_2.m64_u8[1]) + 20 * (short)(temp64.m64_u8[7] + temp64_2.m64_u8[0]);
		temp64_4.m64_i16[2] = temp64.m64_u8[6] + temp64_2.m64_u8[3] - 5 * (short)(temp64.m64_u8[7] + temp64_2.m64_u8[2]) + 20 * (short)(temp64_2.m64_u8[0] + temp64_2.m64_u8[1]);
		temp64_4.m64_i16[3] = temp64.m64_u8[7] + temp64_2.m64_u8[4] - 5 * (short)(temp64_2.m64_u8[0] + temp64_2.m64_u8[3]) + 20 * (short)(temp64_2.m64_u8[1] + temp64_2.m64_u8[2]);
		
		temp64_3 = _mm_add_pi16(temp64_3,_mm_set_pi32 (0x00100010, 0x00100010));
		temp64_3 = _mm_srai_pi16(temp64_3,5);
		
		temp64_4 = _mm_add_pi16(temp64_4,_mm_set_pi32 (0x00100010, 0x00100010));
		temp64_4 = _mm_srai_pi16(temp64_4,5);
		
#ifdef POCKET_PC
		temp64_3 = _mm_packs_pu16(temp64_3,temp64_4);
		*((unsigned int*)&ptr_imge[0]) = temp64_3.m64_i32[0];
		*((unsigned int*)&ptr_imge[4]) = temp64_3.m64_i32[1];
#else
		*((__m64*)&ptr_imge[0]) = _mm_packs_pu16(temp64_3,temp64_4);
#endif
	}


	empty();    

#else

	unsigned char *ptr_img =  &refPicLXl[-2];
	unsigned char *ptr_imge = &image[0];
	unsigned char A,B,C,D,E,F,G,H,I,J,K,L,M;
	short result0,result1,result2,result3,result4,result5,result6,result7;

	int i;
	for (i = 0; i < 8; ptr_img += PicWidthSamples, ptr_imge += stride, i++) {
				
		A = ptr_img[0];
		B = ptr_img[1];
		C = ptr_img[2];
		D = ptr_img[3];
		E = ptr_img[4];
		F = ptr_img[5];
		G = ptr_img[6];
		H = ptr_img[7];
		I = ptr_img[8];
		J = ptr_img[9];
		K = ptr_img[10];
		L = ptr_img[11];
		M = ptr_img[12];

		result0 = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;
		result1 = B + G - 5 * (short)(C + F) + 20 * (short)(D + E) + 16;
		result2 = C + H - 5 * (short)(D + G) + 20 * (short)(E + F) + 16;
		result3 = D + I - 5 * (short)(E + H) + 20 * (short)(F + G) + 16;
		result4 = E + J - 5 * (short)(F + I) + 20 * (short)(G + H) + 16;
		result5 = F + K - 5 * (short)(G + J) + 20 * (short)(H + I) + 16;
		result6 = G + L - 5 * (short)(H + K) + 20 * (short)(I + J) + 16;
		result7 = H + M - 5 * (short)(I + L) + 20 * (short)(J + K) + 16;

		ptr_imge[0] = (unsigned char) CLIP255_16((result0) >> 5);
		ptr_imge[1] = (unsigned char) CLIP255_16((result1) >> 5);
		ptr_imge[2] = (unsigned char) CLIP255_16((result2) >> 5);
		ptr_imge[3] = (unsigned char) CLIP255_16((result3) >> 5);
		ptr_imge[4] = (unsigned char) CLIP255_16((result4) >> 5);
		ptr_imge[5] = (unsigned char) CLIP255_16((result5) >> 5);
		ptr_imge[6] = (unsigned char) CLIP255_16((result6) >> 5);
		ptr_imge[7] = (unsigned char) CLIP255_16((result7) >> 5);
	}

#endif
#endif

}







/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 3 and yFracl = 0.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_3_0(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],	const short PicWidthSamples, const short stride)
{
#ifdef TI_OPTIM
	int b1,b2,b3,b4,b5,b6,b7,b8;
	unsigned int i_tempC, i_tempD;
	int 		 i_tempA, i_tempB;
	unsigned char *ptr_deb = &refPicLXl[-2];
	unsigned char *ptr_imge = &image[0];
	char i;
	#pragma MUST_ITERATE(8,8,8)
	for (i=0; i<8; i++) {
		{
			unsigned int uc_Ref1, uc_Ref2, uc_Ref3, uc_Ref4;
			unsigned int uc_Ref5, uc_Ref6, uc_Ref7, uc_Ref8;
			unsigned int uc_Ref9, uc_Ref10, uc_Ref11, uc_Ref12;//, uc_Ref13;
			unsigned int ui_Ref2By5,ui_Ref3By5,ui_Ref4By5,ui_Ref5By5,ui_Ref6By5,ui_Ref7By5,ui_Ref8By5;
			unsigned int ui_Ref9By5,ui_Ref10By5,ui_Ref11By5,ui_Ref12By5;
			{
				unsigned long long ull_load = _mem8(ptr_deb);
				i_tempD = (ull_load)>>32; 			// |L |K |J |I |
				i_tempC = (unsigned int) ull_load;	// |H |G |F |E |
			}
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

			i_tempC = _extu(i_tempD,8,0) + uc_Ref4; // |KJIH|

			b1 = uc_Ref1 -ui_Ref2By5 +((ui_Ref3By5 +ui_Ref4By5)<<2) -ui_Ref5By5 +uc_Ref6;
			b2 = uc_Ref2 -ui_Ref3By5 +((ui_Ref4By5 +ui_Ref5By5)<<2) -ui_Ref6By5 +uc_Ref7;
			b3 = uc_Ref3 -ui_Ref4By5 +((ui_Ref5By5 +ui_Ref6By5)<<2) -ui_Ref7By5 +uc_Ref8;
			b4 = uc_Ref4 -ui_Ref5By5 +((ui_Ref6By5 +ui_Ref7By5)<<2) -ui_Ref8By5;

			i_tempA = _mem4_const(ptr_deb+8);	// |P |O |N |M |

			uc_Ref9  = _extu(i_tempA,24,24) ;	// M
			uc_Ref10 = _extu(i_tempA,16,24);	// N
			uc_Ref11 = _extu(i_tempA,8,24);		// O
			uc_Ref12 = _extu(i_tempA,0,24);		// P
			ui_Ref9By5  = _mpyu(5,uc_Ref9);		// 5*M
			ui_Ref10By5 = _mpyu(5,uc_Ref10);	// 5*N
			ui_Ref11By5 = _mpyu(5,uc_Ref11);	// 5*O
			ui_Ref12By5 = _mpyu(5,uc_Ref12);	// 5*P
//			uc_Ref13 = *(ptr_deb+12);			// Q
			b4 +=  uc_Ref9;
			b5 = uc_Ref5 -ui_Ref6By5 +((ui_Ref7By5 +ui_Ref8By5)<<2) -ui_Ref9By5  +uc_Ref10;
			b6 = uc_Ref6 -ui_Ref7By5 +((ui_Ref8By5 +ui_Ref9By5)<<2) -ui_Ref10By5 +uc_Ref11;
			b7 = uc_Ref7 -ui_Ref8By5 +((ui_Ref9By5 +ui_Ref10By5)<<2)-ui_Ref11By5 +uc_Ref12;
			b8 = uc_Ref8 -ui_Ref9By5 +((ui_Ref10By5+ui_Ref11By5)<<2)-ui_Ref12By5 +*(ptr_deb+12);

			i_tempD = _extu(i_tempA,8,0) +uc_Ref8 ; 	// |ONML|
		}

		i_tempA = _pack2(b2,b1);				//|pix2| pix1|
		i_tempA = _add2(0x00100010,i_tempA);	// b1 +16
		i_tempA = _shr2(i_tempA,5);				// b = (b1 +16)>>5

		i_tempB = _pack2(b4,b3);
		i_tempB = _add2(0x00100010,i_tempB);	// b1 +16
		i_tempB = _shr2(i_tempB,5);				// b = (b1 +16)>>5

		i_tempB = _spacku4(i_tempB,i_tempA);	// Clip((b1 +16)>>5)
		i_tempA = _avgu4(i_tempB,i_tempC);		// a = (G +b +1)>>1

		_mem4(ptr_imge) = i_tempA ;

		i_tempA = _pack2(b6,b5);				//|pix6| pix5|
		i_tempA = _add2(0x00100010,i_tempA);	// b1 +16
		i_tempA = _shr2(i_tempA,5);				// b = (b1 +16)>>5

		i_tempB = _pack2(b8,b7);
		i_tempB = _add2(0x00100010,i_tempB);	// b1 +16
		i_tempB = _shr2(i_tempB,5);				// b = (b1 +16)>>5

		i_tempB = _spacku4(i_tempB,i_tempA);	// Clip((b1 +16)>>5)
		i_tempB = _avgu4(i_tempB,i_tempD);		// a = (G +b +1)>>1

		_mem4(ptr_imge+4) = i_tempB ;

		ptr_imge+=stride;
		ptr_deb +=PicWidthSamples;
	}
#else
#ifdef MMX

	unsigned char *ptr_img =  &refPicLXl[-2];
	unsigned char *ptr_imge = &image[0];
	__m64 temp64,temp64_2,temp64_3,temp64_4;

	int j,i;
	for (i = 0; i < 8; ptr_img += PicWidthSamples, ptr_imge += stride, i++) {
		
#ifdef POCKET_PC
		temp64 = _mm_set_pi8 (ptr_img[7],ptr_img[6],ptr_img[5],ptr_img[4],ptr_img[3],ptr_img[2],ptr_img[1],ptr_img[0]);
		temp64_2 = _mm_set_pi8 (0,0,0,ptr_img[12],ptr_img[11],ptr_img[10],ptr_img[9],ptr_img[8]);
#else
		temp64 = *((__m64*)&ptr_img[0]);
		temp64_2 = *((__m64*)&ptr_img[8]);
#endif

		temp64_3.m64_i16[0] = temp64.m64_u8[0] + temp64.m64_u8[5] - 5 * (short)(temp64.m64_u8[1] + temp64.m64_u8[4]) + 20 * (short)(temp64.m64_u8[2] + temp64.m64_u8[3]);
		temp64_3.m64_i16[1] = temp64.m64_u8[1] + temp64.m64_u8[6] - 5 * (short)(temp64.m64_u8[2] + temp64.m64_u8[5]) + 20 * (short)(temp64.m64_u8[3] + temp64.m64_u8[4]);
		temp64_3.m64_i16[2] = temp64.m64_u8[2] + temp64.m64_u8[7] - 5 * (short)(temp64.m64_u8[3] + temp64.m64_u8[6]) + 20 * (short)(temp64.m64_u8[4] + temp64.m64_u8[5]);
		temp64_3.m64_i16[3] = temp64.m64_u8[3] + temp64_2.m64_u8[0] - 5 * (short)(temp64.m64_u8[4] + temp64.m64_u8[7]) + 20 * (short)(temp64.m64_u8[5] + temp64.m64_u8[6]);
		temp64_4.m64_i16[0] = temp64.m64_u8[4] + temp64_2.m64_u8[1] - 5 * (short)(temp64.m64_u8[5] + temp64_2.m64_u8[0]) + 20 * (short)(temp64.m64_u8[6] + temp64.m64_u8[7]);
		temp64_4.m64_i16[1] = temp64.m64_u8[5] + temp64_2.m64_u8[2] - 5 * (short)(temp64.m64_u8[6] + temp64_2.m64_u8[1]) + 20 * (short)(temp64.m64_u8[7] + temp64_2.m64_u8[0]);
		temp64_4.m64_i16[2] = temp64.m64_u8[6] + temp64_2.m64_u8[3] - 5 * (short)(temp64.m64_u8[7] + temp64_2.m64_u8[2]) + 20 * (short)(temp64_2.m64_u8[0] + temp64_2.m64_u8[1]);
		temp64_4.m64_i16[3] = temp64.m64_u8[7] + temp64_2.m64_u8[4] - 5 * (short)(temp64_2.m64_u8[0] + temp64_2.m64_u8[3]) + 20 * (short)(temp64_2.m64_u8[1] + temp64_2.m64_u8[2]);
		
		temp64_3 = _mm_add_pi16(temp64_3,_mm_set_pi32 (0x00100010, 0x00100010));
		temp64_3 = _mm_srai_pi16(temp64_3,5);
		
		temp64_4 = _mm_add_pi16(temp64_4,_mm_set_pi32 (0x00100010, 0x00100010));
		temp64_4 = _mm_srai_pi16(temp64_4,5);
		
#ifdef POCKET_PC
		temp64_3 = _mm_packs_pu16(temp64_3,temp64_4);
		*((unsigned int*)&ptr_imge[0]) = temp64_3.m64_i32[0];
		*((unsigned int*)&ptr_imge[4]) = temp64_3.m64_i32[1];
#else
		*((__m64*)&ptr_imge[0]) = _mm_packs_pu16(temp64_3,temp64_4);
#endif
	}


	empty();      

	ptr_img = &refPicLXl[1];
	for (j = 0; j < 8; j++){
		image[0] = ( image[0] + ptr_img[0] +1) >> 1;
		image[1] = ( image[1] + ptr_img[1] + 1) >> 1;
		image[2] = ( image[2] + ptr_img[2] + 1) >> 1;
		image[3] = ( image[3] + ptr_img[3] + 1) >> 1;
		image[4] = ( image[4] + ptr_img[4] + 1) >> 1;
		image[5] = ( image[5] + ptr_img[5] + 1) >> 1;
		image[6] = ( image[6] + ptr_img[6] + 1) >> 1;
		image[7] = ( image[7] + ptr_img[7] + 1) >> 1;
		ptr_img += PicWidthSamples;
		image += stride;

	}

#else

	unsigned char *ptr_img =  &refPicLXl[-2];
	unsigned char *ptr_imge = &image[0];
	unsigned char A,B,C,D,E,F,G,H,I,J,K,L,M;
	short result0,result1,result2,result3,result4,result5,result6,result7;
	int j,i;

	for (i = 0; i < 8; ptr_img += PicWidthSamples, ptr_imge += stride, i++) {
				
		A = ptr_img[0];
		B = ptr_img[1];
		C = ptr_img[2];
		D = ptr_img[3];
		E = ptr_img[4];
		F = ptr_img[5];
		G = ptr_img[6];
		H = ptr_img[7];
		I = ptr_img[8];
		J = ptr_img[9];
		K = ptr_img[10];
		L = ptr_img[11];
		M = ptr_img[12];

		result0 = A + F - 5 * (short)(B + E) + 20 * (short)(C + D) + 16;
		result1 = B + G - 5 * (short)(C + F) + 20 * (short)(D + E) + 16;
		result2 = C + H - 5 * (short)(D + G) + 20 * (short)(E + F) + 16;
		result3 = D + I - 5 * (short)(E + H) + 20 * (short)(F + G) + 16;
		result4 = E + J - 5 * (short)(F + I) + 20 * (short)(G + H) + 16;
		result5 = F + K - 5 * (short)(G + J) + 20 * (short)(H + I) + 16;
		result6 = G + L - 5 * (short)(H + K) + 20 * (short)(I + J) + 16;
		result7 = H + M - 5 * (short)(I + L) + 20 * (short)(J + K) + 16;

		ptr_imge[0] = (unsigned char) CLIP255_16((result0) >> 5);
		ptr_imge[1] = (unsigned char) CLIP255_16((result1) >> 5);
		ptr_imge[2] = (unsigned char) CLIP255_16((result2) >> 5);
		ptr_imge[3] = (unsigned char) CLIP255_16((result3) >> 5);
		ptr_imge[4] = (unsigned char) CLIP255_16((result4) >> 5);
		ptr_imge[5] = (unsigned char) CLIP255_16((result5) >> 5);
		ptr_imge[6] = (unsigned char) CLIP255_16((result6) >> 5);
		ptr_imge[7] = (unsigned char) CLIP255_16((result7) >> 5);
	}

	ptr_img = &refPicLXl[1];
	for (j = 0; j < 8; j++){
		image[0] = ( image[0] + ptr_img[0] +1) >> 1;
		image[1] = ( image[1] + ptr_img[1] + 1) >> 1;
		image[2] = ( image[2] + ptr_img[2] + 1) >> 1;
		image[3] = ( image[3] + ptr_img[3] + 1) >> 1;
		image[4] = ( image[4] + ptr_img[4] + 1) >> 1;
		image[5] = ( image[5] + ptr_img[5] + 1) >> 1;
		image[6] = ( image[6] + ptr_img[6] + 1) >> 1;
		image[7] = ( image[7] + ptr_img[7] + 1) >> 1;
		ptr_img += PicWidthSamples;
		image += stride;

	}

#endif		
#endif	

}
