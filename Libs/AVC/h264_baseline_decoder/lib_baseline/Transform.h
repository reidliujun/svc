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





#ifndef TRANSFORM_H
#define TRANSFORM_H


#include "type.h"
#include "clip.h"

#define ict_4x4_residual ict_4x4_residual_C



#ifdef MMX
#ifndef POCKET_PC
#undef ict_4x4_residual
#define ict_4x4_residual ict_4x4_residual_MMX
#endif
#endif


#ifdef TI_OPTIM
#define SPACKU4(X,Y)  _spacku4 (X, Y);
#undef ict_4x4_residual
#define ict_4x4_residual ict_4x4_residual_TI
#else
#define SPACKU4(X,Y) ((CLIP255_16((X&0xffff0000)>>16)<<24) + (CLIP255_16(X&0x0000ffff)<<16) + (CLIP255_16((Y&0xffff0000)>>16)<<8) + (CLIP255_16(Y&0x0000ffff)));
#endif


void ict_4x4_dc_lum ( const short *  block, short * dst);

/**
   This function permits to transform from frequential to spatial domain.
 @param block Table of transform coefficients.
 @param dst Table of transform coefficients.
 */
static __inline void ict_4x4_dc_chroma ( const short * RESTRICT  block, short * RESTRICT  dst)
{
    const short   z0 = block [0] + block [2];
    const short   z1 = block [0] - block [2];
    const short   z2 = block [1] - block [3];
    const short   z3 = block [1] + block [3];
    
    dst [0] = z0 + z3 ;
    dst [1] = z0 - z3 ;
    dst [2] = z1 + z2 ;
    dst [3] = z1 - z2 ;
}




#ifdef MMX

static __inline void ict_4x4_residual_MMX ( short * RESTRICT  block, unsigned char * RESTRICT  image, const short PicWidthInPix)
{
	

#ifdef POCKET_PC
		
	__m64 res1, res2, res3, res4;
	__m64 inter1,inter2,inter3,inter4;

	unsigned char *img = image;
	unsigned int int32;

	int shft1 = 1;
	int shft2 = 6;
	int nb1,nb2;

	block [0] += 32;
	nb1 = *((int*)(block));
	nb2 = *((int*)(block + 2));
	res1 = _mm_set_pi32(nb2,nb1);
	nb1 = *((int*)(block + 4)); 
	nb2 = *((int*)(block + 6));
	res2 = _mm_set_pi32(nb2,nb1);
	nb1 = *((int*)(block + 8));
	nb2 = *((int*)(block + 10));
	res3 = _mm_set_pi32(nb2,nb1);
	nb1 = *((int*)(block + 12));
	nb2 = *((int*)(block + 14));
	res4 = _mm_set_pi32(nb2,nb1);

	inter1 = _m_punpcklwd(res1,res2);
	inter2 = _m_punpckhwd(res1,res2);
	inter3 = _m_punpcklwd(res3,res4);
	inter4 = _m_punpckhwd(res3,res4);

	res1 = _m_punpckldq(inter1,inter3);
	res2 = _m_punpckldq(inter2,inter4);
	res3 = _m_punpckhdq(inter1,inter3);
	res4 = _m_punpckhdq(inter2,inter4);

	inter1 = _mm_add_pi16(res1,res2);
	inter2 = _mm_sub_pi16(res1,res2);

	res1 = _m_psrawi(res3,shft1); // buffer reuse
	res2 = _m_psrawi(res4,shft1); // buffer reuse

	inter3 = _mm_sub_pi16(res1,res4);
	inter4 = _mm_add_pi16(res3,res2);

	res1 = _mm_add_pi16(inter1,inter4);
	res2 = _mm_sub_pi16(inter1,inter4);
	res3 = _mm_add_pi16(inter2,inter3);
	res4 =  _mm_sub_pi16(inter2,inter3);


	inter1 = _m_punpcklwd(res1,res3);
	inter2 = _m_punpckhwd(res1,res3);
	inter3 = _m_punpcklwd(res4,res2);
	inter4 = _m_punpckhwd(res4,res2);

	res1 = _m_punpckldq(inter1,inter3);
	res2 = _m_punpckldq(inter2,inter4);
	res3 = _m_punpckhdq(inter1,inter3);
	res4 = _m_punpckhdq(inter2,inter4);

	//2eme boucle

	inter1 = _mm_add_pi16(res1,res2);
	inter2 = _mm_sub_pi16(res1,res2);

	res1 = _m_psrawi(res3,shft1); // buffer reuse
	res2 = _m_psrawi(res4,shft1); // buffer reuse

	inter3 = _mm_sub_pi16(res1,res4);
	inter4 = _mm_add_pi16(res3,res2);

	// adding to img and clip
	res2 = _mm_setzero_si64();

	//_mm_prefetch(((char* )img) , _MM_HINT_T0 );

	int32 = *((unsigned int* )img);

	res1 = _mm_add_pi16(inter1,inter4);
	res1 = _m_psrawi(res1,shft2);

	res3 = _mm_cvtsi32_si64(int32);
	res3 =  _m_punpcklbw(res3,res2);
	res1 = _mm_add_pi16(res1,res3);
	res1 = _m_packuswb(res1,res2);
	*((unsigned int* )img) = _mm_cvtsi64_si32(res1);
	img += PicWidthInPix;

	//_mm_prefetch(((char* )img) , _MM_HINT_T0 );
	int32 = *((unsigned int* )img);

	res1 = _mm_add_pi16(inter2,inter3);
	res1 = _m_psrawi(res1,shft2);

	res3 = _mm_cvtsi32_si64(int32);
	res3 =  _m_punpcklbw(res3,res2);
	res1 = _mm_add_pi16(res1,res3);
	res1 = _m_packuswb(res1,res2);
	*((unsigned int* )img) = _mm_cvtsi64_si32(res1);
	img += PicWidthInPix;

	//_mm_prefetch(((char* )img) , _MM_HINT_T0 );
	int32 = *((unsigned int* )img);

	res1 =  _mm_sub_pi16(inter2,inter3);
	res1 = _m_psrawi(res1,shft2);

	res3 = _mm_cvtsi32_si64(int32);
	res3 =  _m_punpcklbw(res3,res2);
	res1 = _mm_add_pi16(res1,res3);
	res1 = _m_packuswb(res1,res2);
	*((unsigned int* )img) = _mm_cvtsi64_si32(res1);
	img += PicWidthInPix;

	//_mm_prefetch(((char* )img) , _MM_HINT_T0 );
	int32 = *((unsigned int* )img);

	res1 = _mm_sub_pi16(inter1,inter4);
	res1 = _m_psrawi(res1,shft2);

	res3 = _mm_cvtsi32_si64(int32);
	res3 =  _m_punpcklbw(res3,res2);
	res1 = _mm_add_pi16(res1,res3);
	res1 = _m_packuswb(res1,res2);
	*((unsigned int* )img) = _mm_cvtsi64_si32(res1);
#else

	__m64 res1, res2, res3, res4;
	__m64 inter1,inter2,inter3,inter4;

	unsigned char *img = image;
	unsigned int int32;

	int shft1 = 1;
	int shft2 = 6;
	
	block [0] += 32;
	res1 = *((__m64* )(block));
	res2 = *((__m64* )(block + 4));
	res3 = *((__m64* )(block + 8));
	res4 = *((__m64* )(block + 12));

	inter1 = _m_punpcklwd(res1,res2);
	inter2 = _m_punpckhwd(res1,res2);
	inter3 = _m_punpcklwd(res3,res4);
	inter4 = _m_punpckhwd(res3,res4);

	res1 = _m_punpckldq(inter1,inter3);
	res2 = _m_punpckldq(inter2,inter4);
	res3 = _m_punpckhdq(inter1,inter3);
	res4 = _m_punpckhdq(inter2,inter4);

	inter1 = _mm_add_pi16(res1,res2);
	inter2 = _mm_sub_pi16(res1,res2);

	res1 = _m_psrawi(res3,shft1); // buffer reuse
	res2 = _m_psrawi(res4,shft1); // buffer reuse

	inter3 = _mm_sub_pi16(res1,res4);
	inter4 = _mm_add_pi16(res3,res2);

	res1 = _mm_add_pi16(inter1,inter4);
	res2 = _mm_sub_pi16(inter1,inter4);
	res3 = _mm_add_pi16(inter2,inter3);
	res4 =  _mm_sub_pi16(inter2,inter3);


	inter1 = _m_punpcklwd(res1,res3);
	inter2 = _m_punpckhwd(res1,res3);
	inter3 = _m_punpcklwd(res4,res2);
	inter4 = _m_punpckhwd(res4,res2);

	res1 = _m_punpckldq(inter1,inter3);
	res2 = _m_punpckldq(inter2,inter4);
	res3 = _m_punpckhdq(inter1,inter3);
	res4 = _m_punpckhdq(inter2,inter4);

	//2eme boucle

	inter1 = _mm_add_pi16(res1,res2);
	inter2 = _mm_sub_pi16(res1,res2);

	res1 = _m_psrawi(res3,shft1); // buffer reuse
	res2 = _m_psrawi(res4,shft1); // buffer reuse

	inter3 = _mm_sub_pi16(res1,res4);
	inter4 = _mm_add_pi16(res3,res2);

	// adding to img and clip
	res2 = _mm_setzero_si64();

	//_mm_prefetch(((char* )img) , _MM_HINT_T0 );

	int32 = *((unsigned int* )img);

	res1 = _mm_add_pi16(inter1,inter4);
	res1 = _m_psrawi(res1,shft2);

	res3 = _mm_cvtsi32_si64(int32);
	res3 =  _m_punpcklbw(res3,res2);
	res1 = _mm_add_pi16(res1,res3);
	res1 = _m_packuswb(res1,res2);
	*((unsigned int* )img) = _mm_cvtsi64_si32(res1);
	img += PicWidthInPix;

	//_mm_prefetch(((char* )img) , _MM_HINT_T0 );
	int32 = *((unsigned int* )img);

	res1 = _mm_add_pi16(inter2,inter3);
	res1 = _m_psrawi(res1,shft2);

	res3 = _mm_cvtsi32_si64(int32);
	res3 =  _m_punpcklbw(res3,res2);
	res1 = _mm_add_pi16(res1,res3);
	res1 = _m_packuswb(res1,res2);
	*((unsigned int* )img) = _mm_cvtsi64_si32(res1);
	img += PicWidthInPix;

	//_mm_prefetch(((char* )img) , _MM_HINT_T0 );
	int32 = *((unsigned int* )img);

	res1 =  _mm_sub_pi16(inter2,inter3);
	res1 = _m_psrawi(res1,shft2);

	res3 = _mm_cvtsi32_si64(int32);
	res3 =  _m_punpcklbw(res3,res2);
	res1 = _mm_add_pi16(res1,res3);
	res1 = _m_packuswb(res1,res2);
	*((unsigned int* )img) = _mm_cvtsi64_si32(res1);
	img += PicWidthInPix;

	//_mm_prefetch(((char* )img) , _MM_HINT_T0 );
	int32 = *((unsigned int* )img);

	res1 = _mm_sub_pi16(inter1,inter4);
	res1 = _m_psrawi(res1,shft2);

	res3 = _mm_cvtsi32_si64(int32);
	res3 =  _m_punpcklbw(res3,res2);
	res1 = _mm_add_pi16(res1,res3);
	res1 = _m_packuswb(res1,res2);
	*((unsigned int* )img) = _mm_cvtsi64_si32(res1);

	empty();    

#endif // PPC

}
#endif


#ifdef TI_OPTIM
	
static __inline void ict_4x4_residual_TI ( short * RESTRICT  block, unsigned char * RESTRICT  image, const short PicWidthInPix){
	unsigned int uiTabTmp0,uiTabTmp1,uiTabTmp2,uiTabTmp3,uiTabTmp4,uiTabTmp5,uiTabTmp6,uiTabTmp7;
    
	block [0] += 32;

	{
		unsigned int uiB1B0,uiB3B2,uiZ3Z0,uiZ2Z1,uiZ0Z1,uiZ3Z2;
		short * psBlock = block;

		// first 4 values
		uiB1B0 = _amem4(psBlock);
		psBlock += 2;
		uiB3B2 = _amem4(psBlock);
		psBlock += 2;

		uiZ3Z0 = _add2(uiB1B0,_extu(uiB3B2,16,16)+((((int)uiB3B2)>>1)&0xffff0000));
		uiZ2Z1 = _sub2(_extu(uiB1B0,16,16)+((((int)uiB1B0)>>1)&0xffff0000),uiB3B2);
		uiZ0Z1 = _pack2(uiZ3Z0,uiZ2Z1);
		uiZ3Z2 = _packh2(uiZ3Z0,uiZ2Z1);

		uiTabTmp0 = _add2(uiZ0Z1,uiZ3Z2);
		uiTabTmp0 = _packlh2(uiTabTmp0,uiTabTmp0);
		uiTabTmp1 = _sub2(uiZ0Z1,uiZ3Z2);

		// second 4 values
		uiB1B0 = _amem4(psBlock);
		psBlock += 2;
		uiB3B2 = _amem4(psBlock);
		psBlock += 2;

		uiZ3Z0 = _add2(uiB1B0,_extu(uiB3B2,16,16)+((((int)uiB3B2)>>1)&0xffff0000));
		uiZ2Z1 = _sub2(_extu(uiB1B0,16,16)+((((int)uiB1B0)>>1)&0xffff0000),uiB3B2);
		uiZ0Z1 = _pack2(uiZ3Z0,uiZ2Z1);
		uiZ3Z2 = _packh2(uiZ3Z0,uiZ2Z1);

		uiTabTmp2 = _add2(uiZ0Z1,uiZ3Z2);
		uiTabTmp2 = _packlh2(uiTabTmp2,uiTabTmp2);
		uiTabTmp3 = _sub2(uiZ0Z1,uiZ3Z2);

		// third 4 values
		uiB1B0 = _amem4(psBlock);
		psBlock += 2;
		uiB3B2 = _amem4(psBlock);
		psBlock += 2;

		uiZ3Z0 = _add2(uiB1B0,_extu(uiB3B2,16,16)+((((int)uiB3B2)>>1)&0xffff0000));
		uiZ2Z1 = _sub2(_extu(uiB1B0,16,16)+((((int)uiB1B0)>>1)&0xffff0000),uiB3B2);
		uiZ0Z1 = _pack2(uiZ3Z0,uiZ2Z1);
		uiZ3Z2 = _packh2(uiZ3Z0,uiZ2Z1);

		uiTabTmp4 = _add2(uiZ0Z1,uiZ3Z2);
		uiTabTmp4 = _packlh2(uiTabTmp4,uiTabTmp4);
		uiTabTmp5 = _sub2(uiZ0Z1,uiZ3Z2);

		// fourth 4 values
		uiB1B0 = _amem4(psBlock);
		psBlock += 2;
		uiB3B2 = _amem4(psBlock);
		psBlock += 2;

		uiZ3Z0 = _add2(uiB1B0,_extu(uiB3B2,16,16)+((((int)uiB3B2)>>1)&0xffff0000));
		uiZ2Z1 = _sub2(_extu(uiB1B0,16,16)+((((int)uiB1B0)>>1)&0xffff0000),uiB3B2);
		uiZ0Z1 = _pack2(uiZ3Z0,uiZ2Z1);
		uiZ3Z2 = _packh2(uiZ3Z0,uiZ2Z1);

		uiTabTmp6 = _add2(uiZ0Z1,uiZ3Z2);
		uiTabTmp6 = _packlh2(uiTabTmp6,uiTabTmp6);
		uiTabTmp7 = _sub2(uiZ0Z1,uiZ3Z2);
	}

	{
		unsigned int A,B,C,D;
		unsigned int AH,AL,BH,BL2,CH,CL,DH,DL;
		int inc = PicWidthInPix>>2;
		unsigned int* pImg = ((unsigned int*)&image [0]);

		unsigned int uiZ0Z0,uiZ3Z3,uiZ1Z1,uiZ2Z2;
		unsigned int ATmp,BTmp,CTmp,DTmp;

		A = *pImg; 
		pImg += inc;
		B = *pImg; 
		pImg += inc;
		C = *pImg; 
		pImg += inc;
		D = *pImg;

		uiZ0Z0 = _add2(uiTabTmp0,uiTabTmp4);
		uiZ3Z3 = _add2(uiTabTmp2,_shr2(uiTabTmp6,1));
		uiZ1Z1 = _sub2(uiTabTmp0,uiTabTmp4);
		uiZ2Z2 = _sub2(_shr2(uiTabTmp2,1),uiTabTmp6);

		AL = _shr2(_add2(uiZ0Z0,uiZ3Z3),6);
		BL2 = _shr2(_add2(uiZ1Z1,uiZ2Z2),6);
		CL = _shr2(_sub2(uiZ1Z1,uiZ2Z2),6);
		DL = _shr2(_sub2(uiZ0Z0,uiZ3Z3),6);

		ATmp = _unpklu4(A);
		BTmp = _unpklu4(B);
		CTmp = _unpklu4(C);
		DTmp = _unpklu4(D);

		AL = _add2(AL,ATmp);
		BL2 = _add2(BL2,BTmp);
		CL = _add2(CL,CTmp);
		DL = _add2(DL,DTmp);

		uiZ0Z0 = _add2(uiTabTmp1,uiTabTmp5);
		uiZ3Z3 = _add2(uiTabTmp3,_shr2(uiTabTmp7,1));
		uiZ1Z1 = _sub2(uiTabTmp1,uiTabTmp5);
		uiZ2Z2 = _sub2(_shr2(uiTabTmp3,1),uiTabTmp7);

		AH = _shr2(_add2(uiZ0Z0,uiZ3Z3),6);
		BH = _shr2(_add2(uiZ1Z1,uiZ2Z2),6);
		CH = _shr2(_sub2(uiZ1Z1,uiZ2Z2),6);
		DH = _shr2(_sub2(uiZ0Z0,uiZ3Z3),6);

		ATmp = _unpkhu4(A);
		BTmp = _unpkhu4(B);
		CTmp = _unpkhu4(C);
		DTmp = _unpkhu4(D);

		AH = _add2(AH,ATmp);
		BH = _add2(BH,BTmp);
		CH = _add2(CH,CTmp);
		DH = _add2(DH,DTmp);

		*pImg = SPACKU4 (DH, DL);
		pImg -= inc;
		*pImg = SPACKU4 (CH, CL);
		pImg -= inc;
		*pImg = SPACKU4 (BH, BL2);
		pImg -= inc;
		*pImg = SPACKU4 (AH, AL);

	}
}

#endif

void ict_4x4_residual_C ( short * RESTRICT  block, unsigned char * RESTRICT  image, const short PicWidthInPix);


#endif

