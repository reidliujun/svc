/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *			  Alain MACCARI <alain.maccari@ens.insa-rennes.fr>
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

#include "clip.h"
#include "type.h"
#include "interpolation_0_n.h"
#include "interpolation_8x8.h"





/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 0 and yFracl = 1.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_0_1(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],const short PicWidthSamples, const short stride)
{

#ifdef TI_OPTIM

	{
		unsigned int uiLine_Alo, uiLine_Ahi;
		unsigned int uiLine_Clo, uiLine_Chi;
		unsigned int uiLine_Glo, uiLine_Ghi;
		unsigned int uiLine_Mlo, uiLine_Mhi;
		unsigned int uiLine_Rlo, uiLine_Rhi;
		unsigned int uiLine_Tlo, uiLine_Thi;
		int iT_2, iT_1,iA_2, iA_1;	

		int iPix1, iPix2, iPix3, iPix4,iPix5, iPix6, iPix7, iPix8;
		int i_h_lo, i_h_hi, i_h;
		int iTmp1, iTmp2, iTmp3, iTmp4;
		int iTmpAdd1, iTmpAdd2;

		unsigned char *pRefImgPtr, *ptr_img,j;

		pRefImgPtr	= (&refPicLXl[0]-(PicWidthSamples<<1));
		ptr_img		= &image[0];
		#pragma MUST_ITERATE(8,8,8)
		for (j = 8; j !=0; j--){
	// Load and unpack
	{
		unsigned long long ull_Tmp1;//, ull_Tmp2;
		ull_Tmp1	= _mem8(pRefImgPtr);	// Load A line
		uiLine_Alo	= (unsigned int) ull_Tmp1;	//|A4|A3|A2|A1|
		uiLine_Ahi	= ull_Tmp1>>32;				//|A8|A7|A6|A5|
		pRefImgPtr	+= PicWidthSamples;

		ull_Tmp1 	= _mem8(pRefImgPtr);	// Load C line
		uiLine_Clo 	= (unsigned int)(ull_Tmp1);	//|C4|C3|C2|C1|
		uiLine_Chi 	= ull_Tmp1>>32;				//|C8|C7|C6|C5|
		pRefImgPtr	+= PicWidthSamples;

		ull_Tmp1	= _mem8(pRefImgPtr);	// Load G line |G8|G7|G6|G5|G4|G3|G2|G1|
		uiLine_Glo	= (unsigned int)(ull_Tmp1);	//|G4|G3|G2|G1|
		uiLine_Ghi 	= ull_Tmp1>>32;				//|G8|G7|G6|G5|
		pRefImgPtr	+= PicWidthSamples;

		ull_Tmp1 	= _mem8(pRefImgPtr);	// Load M line
		uiLine_Mlo 	= (unsigned int)(ull_Tmp1);	//|M4|M3|M2|M1|
		uiLine_Mhi 	= ull_Tmp1>>32;				//|M8|M7|M6|M5|
		pRefImgPtr	+= PicWidthSamples;

		ull_Tmp1	= _mem8(pRefImgPtr);	// Load R line
		uiLine_Rlo	= (unsigned int)(ull_Tmp1);	//|R4|R3|R2|R1|
		uiLine_Rhi	= ull_Tmp1>>32;				//|R8|R7|R6|R5|
		pRefImgPtr	+= PicWidthSamples;

		ull_Tmp1	= _mem8(pRefImgPtr);	// Load T line
		uiLine_Tlo	= (unsigned int)(ull_Tmp1);	//|T4|T3|T2|T1|
		uiLine_Thi	= ull_Tmp1>>32;				//|T8|T7|T6|T5|
	}
	// PIX 4-3-2-1
	// Add A+T
		iT_1	=	_unpklu4(uiLine_Tlo);	//|T2|T1|
		iT_2	=	_unpkhu4(uiLine_Tlo);	//|T4|T3|
		iA_1	=	_unpklu4(uiLine_Alo);	//|A2|A1|
		iA_2	=	_unpkhu4(uiLine_Alo);	//|A4|A3|
		iTmpAdd1 = _add2(iT_1,iA_1);		//|T2+A2|T1+A1|
		iTmpAdd1 = _add2(iTmpAdd1,0x00100010);	//|T2+A2+16|T1+A1+16|
		iTmpAdd2 = _add2(iT_2,iA_2);				//|T4+A4|T3+A3|
		iTmpAdd2 = _add2(iTmpAdd2,0x00100010);	//|T4+A4+16|T3+A3+16|
	// Pack and dot product
		iTmp1 = _packl4(uiLine_Mlo,uiLine_Rlo);	//|M3|M1|R3|R1|
		iTmp2 = _packl4(uiLine_Clo,uiLine_Glo);	//|C3|C1|G3|G1|
		iTmp3 = _packl4(iTmp2,iTmp1);			//|C1|G1|M1|R1|
		iTmp4 = _packh4(iTmp2,iTmp1);			//|C3|G3|M3|R3|

		iPix1 = _dotpsu4(0xFB1414FB,iTmp3);		//|-5C1 + 20G1 + 20M1 -5R1|
		iPix3 = _dotpsu4(0xFB1414FB,iTmp4);		//|-5C3 + 20G3 + 20M3 -5R3|

		iTmp1 = _packh4(uiLine_Mlo,uiLine_Rlo);	//|M4|M2|R4|R2|
		iTmp2 = _packh4(uiLine_Clo,uiLine_Glo);	//|C4|C2|G4|G2|
		iTmp3 = _packl4(iTmp2,iTmp1);			//|C2|G2|M2|R2|
		iTmp4 = _packh4(iTmp2,iTmp1);			//|C4|G4|M4|R4|

		iPix2 = _dotpsu4(0xFB1414FB,iTmp3);		//|-5C2 + 20G2 + 20M2 -5R2|
		iPix4 = _dotpsu4(0xFB1414FB,iTmp4);		//|-5C4 + 20G4 + 20M4 -5R4|
	// Pix2 and Pix1, d_2 d_1
		i_h_lo = _pack2(iPix2,iPix1);			//|-5C2 +20G2 +20M2 -5R2|-5C1 + 20G1 + 20M1 -5R1| 
		i_h_lo = _shr2(_add2(i_h_lo,iTmpAdd1),5);	// (h1 )>>5, lo |h1_2 | h1_1|
	// Pix4 and Pix3, d_4 d_3
		i_h_hi = _pack2(iPix4,iPix3);			//|-5C4 +20G4 +20M4 -5R4|-5C3 + 20G3 + 20M3 -5R3| 
		i_h_hi= _shr2(_add2(i_h_hi,iTmpAdd2),5);	//(h1 + 16)>>5 hi | h1_4 | h1_3|
		i_h = _spacku4(i_h_hi,i_h_lo);			// Clip1(|h4|h3|h2|h1|)
		i_h = _avgu4(i_h, uiLine_Glo);				//|d_4|d_3|d_2|d_1|
	// store |d_4|d_3|d_2|d_1|
		_mem4(ptr_img) = i_h;
	// PIX 8-7-6-5
	// Add A+T
		iT_1	=	_unpklu4(uiLine_Thi);	//|T6|T5|
		iT_2	=	_unpkhu4(uiLine_Thi);	//|T8|T7|
		iA_1	=	_unpklu4(uiLine_Ahi);	//|A6|A5|
		iA_2	=	_unpkhu4(uiLine_Ahi);	//|A8|A7|
		iTmpAdd1 = _add2(iT_1,iA_1);		//|T6+A6|T5+A5|
		iTmpAdd1 = _add2(iTmpAdd1,0x00100010);	//|T6+A6+16|T5+A5+16|
		iTmpAdd2 = _add2(iT_2,iA_2);				//|T8+A8|T7+A7|
		iTmpAdd2 = _add2(iTmpAdd2,0x00100010);	//|T8+A8+16|T7+A7+16|
	// Pack and dot product
		iTmp1 = _packl4(uiLine_Mhi,uiLine_Rhi);	//|M7|M5|R7|R5|
		iTmp2 = _packl4(uiLine_Chi,uiLine_Ghi);	//|C7|C5|G7|G5|
		iTmp3 = _packl4(iTmp2,iTmp1);			//|C5|G5|M5|R5|
		iTmp4 = _packh4(iTmp2,iTmp1);			//|C7|G7|M7|R7|

		iPix5 = _dotpsu4(0xFB1414FB,iTmp3);		//|-5C5 + 20G5 + 20M5 -5R5|
		iPix7 = _dotpsu4(0xFB1414FB,iTmp4);		//|-5C7 + 20G7 + 20M7 -5R7|

		iTmp1 = _packh4(uiLine_Mhi,uiLine_Rhi);	//|M8|M6|R8|R6|
		iTmp2 = _packh4(uiLine_Chi,uiLine_Ghi);	//|C8|C6|G8|G6|
		iTmp3 = _packl4(iTmp2,iTmp1);			//|C6|G6|M6|R6|
		iTmp4 = _packh4(iTmp2,iTmp1);			//|C8|G8|M8|R8|

		iPix6 = _dotpsu4(0xFB1414FB,iTmp3);		//|-5C6 + 20G6 + 20M6 -5R6|
		iPix8 = _dotpsu4(0xFB1414FB,iTmp4);		//|-5C8 + 20G8 + 20M8 -5R8|
	// Pix6 and Pix5, d_6 d_5
		i_h_lo = _pack2(iPix6,iPix5);		//|-5C6 +20G6 +20M6 -5R6|-5C5 + 20G5 + 20M5 -5R5| 
		i_h_lo = _shr2(_add2(i_h_lo,iTmpAdd1),5);	//(h1 + 16)>>5,lo,| h1_6 | h1_5|
	// Pix8 and Pix7, d_8 d_7
		i_h_hi = _pack2(iPix8,iPix7);			//|-5C8 +20G8 +20M8 -5R8|-5C7 + 20G7 + 20M7 -5R7| 
		i_h_hi = _shr2(_add2(i_h_hi,iTmpAdd2),5);	//(h1 + 16)>>5,hi,| h1_8 | h1_7|
		i_h = _spacku4(i_h_hi,i_h_lo);
		i_h = _avgu4(i_h, uiLine_Ghi);			//|h_8 +G8 +1)>>1|(h_7 +G7 +1)>>1|
		_mem4(ptr_img+4) = i_h;
	// Pointers incrementation
		pRefImgPtr -= (PicWidthSamples<<2);
		ptr_img += stride;
	 }	
	}

#else

#ifdef MMX

	int j,i;
	unsigned char *ptr_img; 

	unsigned int ld32;
	__m64 res64_1, res64_2;
	__m64 A64, B64, C64, D64, E64, F64;
	const __m64 cst0 = _mm_setzero_si64();
	const __m64 cstm5 = _mm_set_pi32 (0xFFFBFFFB, 0xFFFBFFFB);
	const __m64 cst20 = _mm_set_pi32 (0x00140014, 0x00140014);
	const __m64 cst16 = _mm_set_pi32 (0x00100010, 0x00100010);
	unsigned char* tmpref;

	
	for (i = 0; i < 8; i+=4) 
	{

#ifdef POCKET_PC
		tmpref = &refPicLXl[( - 2) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		A64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[( - 1) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		B64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		C64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		D64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[( 2) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		E64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[( 3) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		F64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
#else
		ld32 = *((unsigned int*)&refPicLXl[( - 2) * PicWidthSamples + i]);
		A64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[( - 1) * PicWidthSamples + i]);
		B64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[i]);
		C64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[PicWidthSamples + i]);
		D64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[( 2) * PicWidthSamples + i]);
		E64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[( 3) * PicWidthSamples + i]);
		F64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
#endif

		res64_1 = _mm_add_pi16(A64,F64);

		res64_2 = _mm_add_pi16(B64,E64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(C64,D64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(0 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 4) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		A64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(B64,A64);

		res64_2 = _mm_add_pi16(C64,F64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(D64,E64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(1 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 5) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		B64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(C64,B64);

		res64_2 = _mm_add_pi16(D64,A64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(E64,F64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(2 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 6) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		C64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(D64,C64);

		res64_2 = _mm_add_pi16(E64,B64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(F64,A64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(3 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 7) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		D64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(E64,D64);

		res64_2 = _mm_add_pi16(F64,C64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(A64,B64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(4 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 8) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		E64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(F64,E64);

		res64_2 = _mm_add_pi16(A64,D64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(B64,C64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(5 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 9) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		F64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(A64,F64);

		res64_2 = _mm_add_pi16(B64,E64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(C64,D64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(6 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 10) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		A64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(B64,A64);

		res64_2 = _mm_add_pi16(C64,F64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(D64,E64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(7 * stride) + i]) =  res64_1.m64_u32[0];
	}

	ptr_img = &refPicLXl[0];
	for (j = 0; j < 8; j++){
		image[(j * stride)]		= ( image[(j * stride)] + *(ptr_img) + 1)>>1;
		image[(j * stride) + 1] = ( image[(j * stride) + 1] + *(ptr_img + 1) +1)>>1;
		image[(j * stride) + 2] = ( image[(j * stride) + 2] + *(ptr_img + 2) +1)>>1;
		image[(j * stride) + 3] = ( image[(j * stride) + 3] + *(ptr_img + 3) +1)>>1;
		image[(j * stride) + 4] = ( image[(j * stride) + 4] + *(ptr_img + 4) +1)>>1;
		image[(j * stride) + 5] = ( image[(j * stride) + 5] + *(ptr_img + 5) +1)>>1;
		image[(j * stride) + 6] = ( image[(j * stride) + 6] + *(ptr_img + 6) +1)>>1;
		image[(j * stride) + 7] = ( image[(j * stride) + 7] + *(ptr_img + 7) +1)>>1;

		ptr_img += PicWidthSamples;
	}

	/* pas l'air mieux
	ptr_img = &refPicLXl[0];
	for (j = 0; j < 8; j++){
		
		temp64 = *((__m64*)&image[(j * stride)]);
		temp64_2 =_mm_unpackhi_pi8(temp64,cst0);
		temp64 =_mm_unpacklo_pi8(temp64,cst0);
		temp64_3 = *((__m64*)ptr_img);
		temp64_4 =_mm_unpackhi_pi8(temp64_3,cst0);
		temp64_3 =_mm_unpacklo_pi8(temp64_3,cst0);
		temp64 = _mm_add_pi16(temp64,temp64_3);
		temp64 = _mm_add_pi16(temp64,cst1);
		temp64 = _mm_srai_pi16(temp64,1);
		temp64_2 = _mm_add_pi16(temp64_2,temp64_4);
		temp64_2 = _mm_add_pi16(temp64_2,cst1);
		temp64_2 = _mm_srai_pi16(temp64_2,1);
		temp64 = _mm_packs_pu16(temp64,temp64_2);
		*((__m64*)&image[(j * stride)]) = temp64;

		ptr_img += PicWidthSamples;
	}
	*/
	empty();    
#else

	int j,i;
	short result;
	unsigned char *ptr_img =  refPicLXl;

 
	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			result =  ( refPicLXl[(j - 2) * PicWidthSamples + i] + refPicLXl[(j + 3) * PicWidthSamples + i] );
			result += ( refPicLXl[(j - 1) * PicWidthSamples + i] + refPicLXl[(j + 2) * PicWidthSamples + i] )* (-5);
			result += ( refPicLXl[j		  * PicWidthSamples + i] + refPicLXl[(j + 1) * PicWidthSamples + i] ) * 20;

			image[(j * stride) + i] =  (unsigned char) CLIP255_16((result+16) >> 5);
		}
	}

	ptr_img = &refPicLXl[0];
	for (j = 0; j < 8; j++){
		image[(j * stride)]		= (unsigned char) (( image[(j * stride)] + *(ptr_img) + 1) >> 1);
		image[(j * stride) + 1] = (unsigned char) (( image[(j * stride) + 1] + *(ptr_img + 1) + 1) >> 1);
		image[(j * stride) + 2] = (unsigned char) (( image[(j * stride) + 2] + *(ptr_img + 2) + 1) >> 1);
		image[(j * stride) + 3] = (unsigned char) (( image[(j * stride) + 3] + *(ptr_img + 3) + 1) >> 1);
		image[(j * stride) + 4] = (unsigned char) (( image[(j * stride) + 4] + *(ptr_img + 4) + 1) >> 1);
		image[(j * stride) + 5] = (unsigned char) (( image[(j * stride) + 5] + *(ptr_img + 5) + 1) >> 1);
		image[(j * stride) + 6] = (unsigned char) (( image[(j * stride) + 6] + *(ptr_img + 6) + 1) >> 1);
		image[(j * stride) + 7] = (unsigned char) (( image[(j * stride) + 7] + *(ptr_img + 7) + 1) >> 1);

		ptr_img += PicWidthSamples;
	}

#endif
#endif
}





/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 0 and yFracl = 2.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_0_2(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],const short PicWidthSamples, const short stride)
{
#ifdef TI_OPTIM
{
	unsigned int uiLine_Alo, uiLine_Ahi;
	unsigned int uiLine_Clo, uiLine_Chi;
	unsigned int uiLine_Glo, uiLine_Ghi;
	unsigned int uiLine_Mlo, uiLine_Mhi;
	unsigned int uiLine_Rlo, uiLine_Rhi;
	unsigned int uiLine_Tlo, uiLine_Thi;

	int iT_2, iT_1,iA_2, iA_1;	
	int iPix1, iPix2, iPix3, iPix4 ,iPix5, iPix6, iPix7, iPix8;
	int i_h_lo, i_h_hi, i_h;
	int iTmp1, iTmp2, iTmp3, iTmp4;
	int iTmpAdd1, iTmpAdd2;

	unsigned char *pRefImgPtr, *ptr_img,j;

	pRefImgPtr	= (&refPicLXl[0]-(PicWidthSamples<<1));
	ptr_img		= &image[0];
	#pragma MUST_ITERATE(8,8,8)
	for (j = 8; j !=0; j--){
// Load and unpack
{
	unsigned long long ull_Tmp1;//, ull_Tmp2;
	ull_Tmp1	= _mem8(pRefImgPtr);	// Load A line
	uiLine_Alo	= (unsigned int) ull_Tmp1;	//|A4|A3|A2|A1|
	uiLine_Ahi	= ull_Tmp1>>32;				//|A8|A7|A6|A5|
	pRefImgPtr	+= PicWidthSamples;

	ull_Tmp1 	= _mem8(pRefImgPtr);	// Load C line
	uiLine_Clo 	= (unsigned int)(ull_Tmp1);	//|C4|C3|C2|C1|
	uiLine_Chi 	= ull_Tmp1>>32;				//|C8|C7|C6|C5|
	pRefImgPtr	+= PicWidthSamples;

	ull_Tmp1	= _mem8(pRefImgPtr);	// Load G line |G8|G7|G6|G5|G4|G3|G2|G1|
	uiLine_Glo	= (unsigned int)(ull_Tmp1);	//|G4|G3|G2|G1|
	uiLine_Ghi 	= ull_Tmp1>>32;				//|G8|G7|G6|G5|
	pRefImgPtr	+= PicWidthSamples;

	ull_Tmp1 	= _mem8(pRefImgPtr);	// Load M line
	uiLine_Mlo 	= (unsigned int)(ull_Tmp1);	//|M4|M3|M2|M1|
	uiLine_Mhi 	= ull_Tmp1>>32;				//|M8|M7|M6|M5|
	pRefImgPtr	+= PicWidthSamples;

	ull_Tmp1	= _mem8(pRefImgPtr);	// Load R line
	uiLine_Rlo	= (unsigned int)(ull_Tmp1);	//|R4|R3|R2|R1|
	uiLine_Rhi	= ull_Tmp1>>32;				//|R8|R7|R6|R5|
	pRefImgPtr	+= PicWidthSamples;

	ull_Tmp1	= _mem8(pRefImgPtr);	// Load T line
	uiLine_Tlo	= (unsigned int)(ull_Tmp1);	//|T4|T3|T2|T1|
	uiLine_Thi	= ull_Tmp1>>32;				//|T8|T7|T6|T5|
}
// PIX 4-3-2-1
// Add A+T
	iT_1	=	_unpklu4(uiLine_Tlo);	//|T2|T1|
	iT_2	=	_unpkhu4(uiLine_Tlo);	//|T4|T3|
	iA_1	=	_unpklu4(uiLine_Alo);	//|A2|A1|
	iA_2	=	_unpkhu4(uiLine_Alo);	//|A4|A3|
	iTmpAdd1 = _add2(iT_1,iA_1);		//|T2+A2|T1+A1|
	iTmpAdd1 = _add2(iTmpAdd1,0x00100010);	//|T2+A2+16|T1+A1+16|
	iTmpAdd2 = _add2(iT_2,iA_2);				//|T4+A4|T3+A3|
	iTmpAdd2 = _add2(iTmpAdd2,0x00100010);	//|T4+A4+16|T3+A3+16|
// Pack and dot product
	iTmp1 = _packl4(uiLine_Mlo,uiLine_Rlo);	//|M3|M1|R3|R1|
	iTmp2 = _packl4(uiLine_Clo,uiLine_Glo);	//|C3|C1|G3|G1|
	iTmp3 = _packl4(iTmp2,iTmp1);			//|C1|G1|M1|R1|
	iTmp4 = _packh4(iTmp2,iTmp1);			//|C3|G3|M3|R3|

	iPix1 = _dotpsu4(0xFB1414FB,iTmp3);		//|-5C1 + 20G1 + 20M1 -5R1|
	iPix3 = _dotpsu4(0xFB1414FB,iTmp4);		//|-5C3 + 20G3 + 20M3 -5R3|

	iTmp1 = _packh4(uiLine_Mlo,uiLine_Rlo);	//|M4|M2|R4|R2|
	iTmp2 = _packh4(uiLine_Clo,uiLine_Glo);	//|C4|C2|G4|G2|
	iTmp3 = _packl4(iTmp2,iTmp1);			//|C2|G2|M2|R2|
	iTmp4 = _packh4(iTmp2,iTmp1);			//|C4|G4|M4|R4|

	iPix2 = _dotpsu4(0xFB1414FB,iTmp3);		//|-5C2 + 20G2 + 20M2 -5R2|
	iPix4 = _dotpsu4(0xFB1414FB,iTmp4);		//|-5C4 + 20G4 + 20M4 -5R4|
// Pix2 and Pix1, d_2 d_1
	i_h_lo = _pack2(iPix2,iPix1);			//|-5C2 +20G2 +20M2 -5R2|-5C1 + 20G1 + 20M1 -5R1| 
	i_h_lo = _shr2(_add2(i_h_lo,iTmpAdd1),5);	// (h1 )>>5, lo |h1_2 | h1_1|
// Pix4 and Pix3, d_4 d_3
	i_h_hi = _pack2(iPix4,iPix3);			//|-5C4 +20G4 +20M4 -5R4|-5C3 + 20G3 + 20M3 -5R3| 
	i_h_hi= _shr2(_add2(i_h_hi,iTmpAdd2),5);	//(h1 + 16)>>5 hi | h1_4 | h1_3|
	i_h = _spacku4(i_h_hi,i_h_lo);			// Clip1(|h4|h3|h2|h1|)
// store |d_4|d_3|d_2|d_1|
	_mem4(ptr_img) = i_h;
// PIX 8-7-6-5
// Add A+T
	iT_1	=	_unpklu4(uiLine_Thi);	//|T6|T5|
	iT_2	=	_unpkhu4(uiLine_Thi);	//|T8|T7|
	iA_1	=	_unpklu4(uiLine_Ahi);	//|A6|A5|
	iA_2	=	_unpkhu4(uiLine_Ahi);	//|A8|A7|
	iTmpAdd1 = _add2(iT_1,iA_1);		//|T6+A6|T5+A5|
	iTmpAdd1 = _add2(iTmpAdd1,0x00100010);	//|T6+A6+16|T5+A5+16|
	iTmpAdd2 = _add2(iT_2,iA_2);				//|T8+A8|T7+A7|
	iTmpAdd2 = _add2(iTmpAdd2,0x00100010);	//|T8+A8+16|T7+A7+16|
// Pack and dot product
	iTmp1 = _packl4(uiLine_Mhi,uiLine_Rhi);	//|M7|M5|R7|R5|
	iTmp2 = _packl4(uiLine_Chi,uiLine_Ghi);	//|C7|C5|G7|G5|
	iTmp3 = _packl4(iTmp2,iTmp1);			//|C5|G5|M5|R5|
	iTmp4 = _packh4(iTmp2,iTmp1);			//|C7|G7|M7|R7|

	iPix5 = _dotpsu4(0xFB1414FB,iTmp3);		//|-5C5 + 20G5 + 20M5 -5R5|
	iPix7 = _dotpsu4(0xFB1414FB,iTmp4);		//|-5C7 + 20G7 + 20M7 -5R7|

	iTmp1 = _packh4(uiLine_Mhi,uiLine_Rhi);	//|M8|M6|R8|R6|
	iTmp2 = _packh4(uiLine_Chi,uiLine_Ghi);	//|C8|C6|G8|G6|
	iTmp3 = _packl4(iTmp2,iTmp1);			//|C6|G6|M6|R6|
	iTmp4 = _packh4(iTmp2,iTmp1);			//|C8|G8|M8|R8|

	iPix6 = _dotpsu4(0xFB1414FB,iTmp3);		//|-5C6 + 20G6 + 20M6 -5R6|
	iPix8 = _dotpsu4(0xFB1414FB,iTmp4);		//|-5C8 + 20G8 + 20M8 -5R8|
// Pix6 and Pix5, d_6 d_5
	i_h_lo = _pack2(iPix6,iPix5);		//|-5C6 +20G6 +20M6 -5R6|-5C5 + 20G5 + 20M5 -5R5| 
	i_h_lo = _shr2(_add2(i_h_lo,iTmpAdd1),5);	//(h1 + 16)>>5,lo,| h1_6 | h1_5|
// Pix8 and Pix7, d_8 d_7
	i_h_hi = _pack2(iPix8,iPix7);			//|-5C8 +20G8 +20M8 -5R8|-5C7 + 20G7 + 20M7 -5R7| 
	i_h_hi = _shr2(_add2(i_h_hi,iTmpAdd2),5);	//(h1 + 16)>>5,hi,| h1_8 | h1_7|
	i_h = _spacku4(i_h_hi,i_h_lo);
	_mem4(ptr_img+4) = i_h;
// Pointers incrementation
	pRefImgPtr -= (PicWidthSamples<<2);
	ptr_img += stride;
 }	
}

#else

#ifdef MMX

	unsigned int ld32;
	__m64 res64_1, res64_2;
	__m64 A64, B64, C64, D64, E64, F64;
	const __m64 cst0 = _mm_setzero_si64();
	const __m64 cstm5 = _mm_set_pi32 (0xFFFBFFFB, 0xFFFBFFFB);
	const __m64 cst20 = _mm_set_pi32 (0x00140014, 0x00140014);
	const __m64 cst16 = _mm_set_pi32 (0x00100010, 0x00100010);
	int i;
	unsigned char* tmpref;
	
	for (i = 0; i < 8; i+=4) 
	{
#ifdef POCKET_PC
		tmpref = &refPicLXl[( - 2) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		A64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[( - 1) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		B64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		C64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		D64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[( 2) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		E64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[( 3) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		F64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
#else
		ld32 = *((unsigned int*)&refPicLXl[( - 2) * PicWidthSamples + i]);
		A64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[( - 1) * PicWidthSamples + i]);
		B64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[i]);
		C64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[PicWidthSamples + i]);
		D64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[( 2) * PicWidthSamples + i]);
		E64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[( 3) * PicWidthSamples + i]);
		F64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
#endif



		res64_1 = _mm_add_pi16(A64,F64);

		res64_2 = _mm_add_pi16(B64,E64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(C64,D64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(0 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 4) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		A64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(B64,A64);

		res64_2 = _mm_add_pi16(C64,F64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(D64,E64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(1 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 5) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		B64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(C64,B64);

		res64_2 = _mm_add_pi16(D64,A64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(E64,F64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(2 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 6) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		C64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(D64,C64);

		res64_2 = _mm_add_pi16(E64,B64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(F64,A64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(3 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 7) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		D64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(E64,D64);

		res64_2 = _mm_add_pi16(F64,C64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(A64,B64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(4 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 8) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		E64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(F64,E64);

		res64_2 = _mm_add_pi16(A64,D64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(B64,C64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(5 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 9) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		F64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(A64,F64);

		res64_2 = _mm_add_pi16(B64,E64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(C64,D64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(6 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 10) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		A64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(B64,A64);

		res64_2 = _mm_add_pi16(C64,F64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(D64,E64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(7 * stride) + i]) =  res64_1.m64_u32[0];
	}

	empty();    

#else

	int j,i;
	short result;


	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			result =  ( refPicLXl[(j - 2) * PicWidthSamples + i] + refPicLXl[(j + 3) * PicWidthSamples + i] );
			result += ( refPicLXl[(j - 1) * PicWidthSamples + i] + refPicLXl[(j + 2) * PicWidthSamples + i] )* (-5);
			result += ( refPicLXl[j       * PicWidthSamples + i] + refPicLXl[(j + 1) * PicWidthSamples + i] ) * 20;

			image[(j * stride) + i] =  (unsigned char) CLIP255_16((result + 16) >> 5);
		}
	}
#endif
#endif
}






/**
This function allows to get the luminance 8x8 prediction of a non IDR picture when xFracl = 0 and yFracl = 3.


@param image Table of current frame.
@param refPicLXl Table of the reference decoded picture buffer.
@param PicWidthSamples Stride of the reference buffer.
@param stride Stride of the current image.
*/
void luma_sample_interp_8x8_0_3(unsigned char image [RESTRICT], unsigned char refPicLXl[RESTRICT],const short PicWidthSamples, const short stride)
{
#ifdef TI_OPTIM
{
	unsigned int uiLine_Alo, uiLine_Ahi;
	unsigned int uiLine_Clo, uiLine_Chi;
	unsigned int uiLine_Glo, uiLine_Ghi;
	unsigned int uiLine_Mlo, uiLine_Mhi;
	unsigned int uiLine_Rlo, uiLine_Rhi;
	unsigned int uiLine_Tlo, uiLine_Thi;
	int iT_2, iT_1,iA_2, iA_1;	

	int iPix1, iPix2, iPix3, iPix4,iPix5, iPix6, iPix7, iPix8;
	int i_h_lo, i_h_hi, i_h;
	int iTmp1, iTmp2, iTmp3, iTmp4;
	int iTmpAdd1, iTmpAdd2;

	unsigned char *pRefImgPtr, *ptr_img,j;

	pRefImgPtr	= (&refPicLXl[0]-(PicWidthSamples<<1));
	ptr_img		= &image[0];
	#pragma MUST_ITERATE(8,8,8)
	for (j = 8; j !=0; j--){
// Load and unpack
{
	unsigned long long ull_Tmp1;//, ull_Tmp2;
	ull_Tmp1	= _mem8(pRefImgPtr);	// Load A line
	uiLine_Alo	= (unsigned int) ull_Tmp1;	//|A4|A3|A2|A1|
	uiLine_Ahi	= ull_Tmp1>>32;				//|A8|A7|A6|A5|
	pRefImgPtr	+= PicWidthSamples;

	ull_Tmp1 	= _mem8(pRefImgPtr);	// Load C line
	uiLine_Clo 	= (unsigned int)(ull_Tmp1);	//|C4|C3|C2|C1|
	uiLine_Chi 	= ull_Tmp1>>32;				//|C8|C7|C6|C5|
	pRefImgPtr	+= PicWidthSamples;

	ull_Tmp1	= _mem8(pRefImgPtr);	// Load G line |G8|G7|G6|G5|G4|G3|G2|G1|
	uiLine_Glo	= (unsigned int)(ull_Tmp1);	//|G4|G3|G2|G1|
	uiLine_Ghi 	= ull_Tmp1>>32;				//|G8|G7|G6|G5|
	pRefImgPtr	+= PicWidthSamples;

	ull_Tmp1 	= _mem8(pRefImgPtr);	// Load M line
	uiLine_Mlo 	= (unsigned int)(ull_Tmp1);	//|M4|M3|M2|M1|
	uiLine_Mhi 	= ull_Tmp1>>32;				//|M8|M7|M6|M5|
	pRefImgPtr	+= PicWidthSamples;

	ull_Tmp1	= _mem8(pRefImgPtr);	// Load R line
	uiLine_Rlo	= (unsigned int)(ull_Tmp1);	//|R4|R3|R2|R1|
	uiLine_Rhi	= ull_Tmp1>>32;				//|R8|R7|R6|R5|
	pRefImgPtr	+= PicWidthSamples;

	ull_Tmp1	= _mem8(pRefImgPtr);	// Load T line
	uiLine_Tlo	= (unsigned int)(ull_Tmp1);	//|T4|T3|T2|T1|
	uiLine_Thi	= ull_Tmp1>>32;				//|T8|T7|T6|T5|
}
// PIX 4-3-2-1
// Add A+T
	iT_1	=	_unpklu4(uiLine_Tlo);	//|T2|T1|
	iT_2	=	_unpkhu4(uiLine_Tlo);	//|T4|T3|
	iA_1	=	_unpklu4(uiLine_Alo);	//|A2|A1|
	iA_2	=	_unpkhu4(uiLine_Alo);	//|A4|A3|
	iTmpAdd1 = _add2(iT_1,iA_1);		//|T2+A2|T1+A1|
	iTmpAdd1 = _add2(iTmpAdd1,0x00100010);	//|T2+A2+16|T1+A1+16|
	iTmpAdd2 = _add2(iT_2,iA_2);				//|T4+A4|T3+A3|
	iTmpAdd2 = _add2(iTmpAdd2,0x00100010);	//|T4+A4+16|T3+A3+16|
// Pack and dot product
	iTmp1 = _packl4(uiLine_Mlo,uiLine_Rlo);	//|M3|M1|R3|R1|
	iTmp2 = _packl4(uiLine_Clo,uiLine_Glo);	//|C3|C1|G3|G1|
	iTmp3 = _packl4(iTmp2,iTmp1);			//|C1|G1|M1|R1|
	iTmp4 = _packh4(iTmp2,iTmp1);			//|C3|G3|M3|R3|

	iPix1 = _dotpsu4(0xFB1414FB,iTmp3);		//|-5C1 + 20G1 + 20M1 -5R1|
	iPix3 = _dotpsu4(0xFB1414FB,iTmp4);		//|-5C3 + 20G3 + 20M3 -5R3|

	iTmp1 = _packh4(uiLine_Mlo,uiLine_Rlo);	//|M4|M2|R4|R2|
	iTmp2 = _packh4(uiLine_Clo,uiLine_Glo);	//|C4|C2|G4|G2|
	iTmp3 = _packl4(iTmp2,iTmp1);			//|C2|G2|M2|R2|
	iTmp4 = _packh4(iTmp2,iTmp1);			//|C4|G4|M4|R4|

	iPix2 = _dotpsu4(0xFB1414FB,iTmp3);		//|-5C2 + 20G2 + 20M2 -5R2|
	iPix4 = _dotpsu4(0xFB1414FB,iTmp4);		//|-5C4 + 20G4 + 20M4 -5R4|
// Pix2 and Pix1, d_2 d_1
	i_h_lo = _pack2(iPix2,iPix1);			//|-5C2 +20G2 +20M2 -5R2|-5C1 + 20G1 + 20M1 -5R1| 
	i_h_lo = _shr2(_add2(i_h_lo,iTmpAdd1),5);	// (h1 )>>5, lo |h1_2 | h1_1|
// Pix4 and Pix3, d_4 d_3
	i_h_hi = _pack2(iPix4,iPix3);			//|-5C4 +20G4 +20M4 -5R4|-5C3 + 20G3 + 20M3 -5R3| 
	i_h_hi= _shr2(_add2(i_h_hi,iTmpAdd2),5);	//(h1 + 16)>>5 hi | h1_4 | h1_3|
	i_h = _spacku4(i_h_hi,i_h_lo);			// Clip1(|h4|h3|h2|h1|)
	i_h = _avgu4(i_h, uiLine_Mlo);				//|d_4|d_3|d_2|d_1|
// store |d_4|d_3|d_2|d_1|
	_mem4(ptr_img) = i_h;
// PIX 8-7-6-5
// Add A+T
	iT_1	=	_unpklu4(uiLine_Thi);	//|T6|T5|
	iT_2	=	_unpkhu4(uiLine_Thi);	//|T8|T7|
	iA_1	=	_unpklu4(uiLine_Ahi);	//|A6|A5|
	iA_2	=	_unpkhu4(uiLine_Ahi);	//|A8|A7|
	iTmpAdd1 = _add2(iT_1,iA_1);		//|T6+A6|T5+A5|
	iTmpAdd1 = _add2(iTmpAdd1,0x00100010);	//|T6+A6+16|T5+A5+16|
	iTmpAdd2 = _add2(iT_2,iA_2);				//|T8+A8|T7+A7|
	iTmpAdd2 = _add2(iTmpAdd2,0x00100010);	//|T8+A8+16|T7+A7+16|
// Pack and dot product
	iTmp1 = _packl4(uiLine_Mhi,uiLine_Rhi);	//|M7|M5|R7|R5|
	iTmp2 = _packl4(uiLine_Chi,uiLine_Ghi);	//|C7|C5|G7|G5|
	iTmp3 = _packl4(iTmp2,iTmp1);			//|C5|G5|M5|R5|
	iTmp4 = _packh4(iTmp2,iTmp1);			//|C7|G7|M7|R7|

	iPix5 = _dotpsu4(0xFB1414FB,iTmp3);		//|-5C5 + 20G5 + 20M5 -5R5|
	iPix7 = _dotpsu4(0xFB1414FB,iTmp4);		//|-5C7 + 20G7 + 20M7 -5R7|

	iTmp1 = _packh4(uiLine_Mhi,uiLine_Rhi);	//|M8|M6|R8|R6|
	iTmp2 = _packh4(uiLine_Chi,uiLine_Ghi);	//|C8|C6|G8|G6|
	iTmp3 = _packl4(iTmp2,iTmp1);			//|C6|G6|M6|R6|
	iTmp4 = _packh4(iTmp2,iTmp1);			//|C8|G8|M8|R8|

	iPix6 = _dotpsu4(0xFB1414FB,iTmp3);		//|-5C6 + 20G6 + 20M6 -5R6|
	iPix8 = _dotpsu4(0xFB1414FB,iTmp4);		//|-5C8 + 20G8 + 20M8 -5R8|
// Pix6 and Pix5, d_6 d_5
	i_h_lo = _pack2(iPix6,iPix5);		//|-5C6 +20G6 +20M6 -5R6|-5C5 + 20G5 + 20M5 -5R5| 
	i_h_lo = _shr2(_add2(i_h_lo,iTmpAdd1),5);	//(h1 + 16)>>5,lo,| h1_6 | h1_5|
// Pix8 and Pix7, d_8 d_7
	i_h_hi = _pack2(iPix8,iPix7);			//|-5C8 +20G8 +20M8 -5R8|-5C7 + 20G7 + 20M7 -5R7| 
	i_h_hi = _shr2(_add2(i_h_hi,iTmpAdd2),5);	//(h1 + 16)>>5,hi,| h1_8 | h1_7|
	i_h = _spacku4(i_h_hi,i_h_lo);
	i_h = _avgu4(i_h, uiLine_Mhi);			//|h_8 +G8 +1)>>1|(h_7 +G7 +1)>>1|
	_mem4(ptr_img+4) = i_h;
// Pointers incrementation
	pRefImgPtr -= (PicWidthSamples<<2);
	ptr_img += stride;
 }	
}
#else
#ifdef MMX


	int j,i;
	unsigned char *ptr_img =  refPicLXl;

	
	unsigned int ld32;
	__m64 res64_1, res64_2;
	__m64 A64, B64, C64, D64, E64, F64;
	const __m64 cst0 = _mm_setzero_si64();
	const __m64 cstm5 = _mm_set_pi32 (0xFFFBFFFB, 0xFFFBFFFB);
	const __m64 cst20 = _mm_set_pi32 (0x00140014, 0x00140014);
	const __m64 cst16 = _mm_set_pi32 (0x00100010, 0x00100010);
	unsigned char* tmpref;

	
	for (i = 0; i < 8; i+=4) 
	{
#ifdef POCKET_PC
		tmpref = &refPicLXl[( - 2) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		A64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[( - 1) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		B64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		C64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		D64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[( 2) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		E64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		tmpref = &refPicLXl[( 3) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		F64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
#else
		ld32 = *((unsigned int*)&refPicLXl[( - 2) * PicWidthSamples + i]);
		A64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[( - 1) * PicWidthSamples + i]);
		B64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[i]);
		C64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[PicWidthSamples + i]);
		D64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[( 2) * PicWidthSamples + i]);
		E64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
		ld32 = *((unsigned int*)&refPicLXl[( 3) * PicWidthSamples + i]);
		F64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);
#endif



		res64_1 = _mm_add_pi16(A64,F64);

		res64_2 = _mm_add_pi16(B64,E64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(C64,D64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(0 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 4) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		A64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(B64,A64);

		res64_2 = _mm_add_pi16(C64,F64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(D64,E64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(1 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 5) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		B64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(C64,B64);

		res64_2 = _mm_add_pi16(D64,A64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(E64,F64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(2 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 6) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		C64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(D64,C64);

		res64_2 = _mm_add_pi16(E64,B64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(F64,A64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(3 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 7) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		D64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(E64,D64);

		res64_2 = _mm_add_pi16(F64,C64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(A64,B64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(4 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 8) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		E64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(F64,E64);

		res64_2 = _mm_add_pi16(A64,D64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(B64,C64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(5 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 9) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		F64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(A64,F64);

		res64_2 = _mm_add_pi16(B64,E64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(C64,D64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(6 * stride) + i]) =  res64_1.m64_u32[0];

		tmpref = &refPicLXl[( 10) * PicWidthSamples + i];
		ld32 = (*tmpref) + ((*(tmpref+1)) << 8) + ((*(tmpref+2)) << 16) + ((*(tmpref+3)) << 24);
		A64 =_mm_unpacklo_pi8(_mm_cvtsi32_si64(ld32),cst0);

		res64_1 = _mm_add_pi16(B64,A64);

		res64_2 = _mm_add_pi16(C64,F64);
		res64_2 = _mm_mullo_pi16(res64_2, cstm5);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_2 = _mm_add_pi16(D64,E64);
		res64_2 = _mm_mullo_pi16(res64_2, cst20);
		
		res64_1 = _mm_add_pi16(res64_1,res64_2);

		res64_1 = _mm_add_pi16(res64_1,cst16);
		res64_1 = _mm_srai_pi16(res64_1,5);
		res64_1 = _mm_packs_pu16(res64_1,_mm_setzero_si64());

		*((unsigned int*)&image[(7 * stride) + i]) =  res64_1.m64_u32[0];
	}

	ptr_img = &refPicLXl[PicWidthSamples];
	for (j = 0; j < 8; j++){
		image[(j * stride)]		= ( image[(j * stride)] + *(ptr_img) + 1)>>1;
		image[(j * stride) + 1] = ( image[(j * stride) + 1] + *(ptr_img + 1) +1)>>1;
		image[(j * stride) + 2] = ( image[(j * stride) + 2] + *(ptr_img + 2) +1)>>1;
		image[(j * stride) + 3] = ( image[(j * stride) + 3] + *(ptr_img + 3) +1)>>1;
		image[(j * stride) + 4] = ( image[(j * stride) + 4] + *(ptr_img + 4) +1)>>1;
		image[(j * stride) + 5] = ( image[(j * stride) + 5] + *(ptr_img + 5) +1)>>1;
		image[(j * stride) + 6] = ( image[(j * stride) + 6] + *(ptr_img + 6) +1)>>1;
		image[(j * stride) + 7] = ( image[(j * stride) + 7] + *(ptr_img + 7) +1)>>1;

		ptr_img += PicWidthSamples;
	}

	empty();    
#else

	int j,i;
	short result;
	unsigned char *ptr_img =  refPicLXl;


	for (j = 0; j < 8; j++) {

		for (i = 0; i < 8; i++) {
			result =  ( refPicLXl[(j - 2) * PicWidthSamples + i] + refPicLXl[(j + 3) * PicWidthSamples + i] );
			result += ( refPicLXl[(j - 1) * PicWidthSamples + i] + refPicLXl[(j + 2) * PicWidthSamples + i] )* (-5);
			result += ( refPicLXl[j       * PicWidthSamples + i] + refPicLXl[(j + 1) * PicWidthSamples + i] ) * 20;

			image[(j * stride) + i] =  (unsigned char) CLIP255_16((result + 16) >> 5);
		}
	}

	ptr_img = &refPicLXl[PicWidthSamples];
	for (j = 0; j < 8; j++){
		image[(j * stride)]		= (unsigned char) (( image[(j * stride)] + *(ptr_img) + 1) >> 1);
		image[(j * stride) + 1] = (unsigned char) (( image[(j * stride) + 1] + *(ptr_img + 1) + 1) >> 1);
		image[(j * stride) + 2] = (unsigned char) (( image[(j * stride) + 2] + *(ptr_img + 2) + 1) >> 1);
		image[(j * stride) + 3] = (unsigned char) (( image[(j * stride) + 3] + *(ptr_img + 3) + 1) >> 1);
		image[(j * stride) + 4] = (unsigned char) (( image[(j * stride) + 4] + *(ptr_img + 4) + 1) >> 1);
		image[(j * stride) + 5] = (unsigned char) (( image[(j * stride) + 5] + *(ptr_img + 5) + 1) >> 1);
		image[(j * stride) + 6] = (unsigned char) (( image[(j * stride) + 6] + *(ptr_img + 6) + 1) >> 1);
		image[(j * stride) + 7] = (unsigned char) (( image[(j * stride) + 7] + *(ptr_img + 7) + 1) >> 1);

		ptr_img += PicWidthSamples;
	}

#endif
#endif

}



