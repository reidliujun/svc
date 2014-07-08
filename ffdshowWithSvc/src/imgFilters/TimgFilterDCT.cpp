/*
 * Copyright (c) 2002-2006 Milan Cutka
 * Copyright (c) 2002 Tom Barry.  All rights reserved.
 *      trbarry@trbarry.com
 * idct, fdct, quantization and dequantization routines from XviD
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Tconfig.h"
#include "TimgFilterDCT.h"
#include "TdctSettings.h"

#pragma warning(disable:4799)

extern "C" void Skl_IDct16_MMX (short *block);
extern "C" void Skl_IDct16_SSE (short *block);
extern "C" void fdct_mmx_skal (short *block);
extern "C" void fdct_xmm_skal (short *block);

// MSVC71 has problem in compiling sse2 code of this file.
// MSVC8 and ICL10 works.
#if defined(__SSE2__) && ((_MSC_VER>=1400) || defined(__INTEL_COMPILER))
#define DCT_SSE2
#endif

TimgFilterDCT::TimgFilterDCT(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
#ifdef DCT_SSE2
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  {
   fdct=fdct_sse2;
   idct=idct_sse2;
  }
#endif
#ifndef WIN64
 #ifdef DCT_SSE2
 else
 #endif
 if (Tconfig::cpu_flags&FF_CPU_MMXEXT)
  {
   fdct=fdct_xmm_skal;
   idct=Skl_IDct16_SSE;
  }
 else if (Tconfig::cpu_flags&FF_CPU_MMX)
  {
   fdct=fdct_mmx_skal;
   idct=Skl_IDct16_MMX;
  }
#endif
 else
  {
   fdct=fdct_c;
   idct=idct_c;
   idct_c_init();
  }
 oldfac[0]=INT_MAX;oldMode=-1;oldmatrix[0]=0;
 pWorkArea=(short*)aligned_malloc(64*sizeof(short),16);
}
TimgFilterDCT::~TimgFilterDCT()
{
 aligned_free(pWorkArea);
}

void TimgFilterDCT::multiply(void)
{
 const char * const factors8=(const char*)&factors[0][0];

 *(__m64*)(pWorkArea+0*8+0)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+0*8+0),*(__m64*)(factors8+0*16  )),3);
 *(__m64*)(pWorkArea+0*8+4)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+0*8+4),*(__m64*)(factors8+0*16+8)),3);

 *(__m64*)(pWorkArea+1*8+0)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+1*8+0),*(__m64*)(factors8+1*16  )),3);
 *(__m64*)(pWorkArea+1*8+4)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+1*8+4),*(__m64*)(factors8+1*16+8)),3);

 *(__m64*)(pWorkArea+2*8+0)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+2*8+0),*(__m64*)(factors8+2*16  )),3);
 *(__m64*)(pWorkArea+2*8+4)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+2*8+4),*(__m64*)(factors8+2*16+8)),3);

 *(__m64*)(pWorkArea+3*8+0)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+3*8+0),*(__m64*)(factors8+3*16  )),3);
 *(__m64*)(pWorkArea+3*8+4)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+3*8+4),*(__m64*)(factors8+3*16+8)),3);

 *(__m64*)(pWorkArea+4*8+0)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+4*8+0),*(__m64*)(factors8+4*16  )),3);
 *(__m64*)(pWorkArea+4*8+4)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+4*8+4),*(__m64*)(factors8+4*16+8)),3);

 *(__m64*)(pWorkArea+5*8+0)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+5*8+0),*(__m64*)(factors8+5*16  )),3);
 *(__m64*)(pWorkArea+5*8+4)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+5*8+4),*(__m64*)(factors8+5*16+8)),3);

 *(__m64*)(pWorkArea+6*8+0)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+6*8+0),*(__m64*)(factors8+6*16  )),3);
 *(__m64*)(pWorkArea+6*8+4)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+6*8+4),*(__m64*)(factors8+6*16+8)),3);

 *(__m64*)(pWorkArea+7*8+0)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+7*8+0),*(__m64*)(factors8+7*16  )),3);
 *(__m64*)(pWorkArea+7*8+4)=_mm_srai_pi16(_mm_mullo_pi16(*(__m64*)(pWorkArea+7*8+4),*(__m64*)(factors8+7*16+8)),3);
}

void TimgFilterDCT::quant_h263_inter(int16_t * coeff,const uint32_t quant, const uint16_t *)
{
 #define SCALEBITS       16
 #define FIX(X)          ((1L << SCALEBITS) / (X) + 1)
 static const uint32_t multipliers[32] =
  {
   0,       FIX(2),  FIX(4),  FIX(6),
   FIX(8),  FIX(10), FIX(12), FIX(14),
   FIX(16), FIX(18), FIX(20), FIX(22),
   FIX(24), FIX(26), FIX(28), FIX(30),
   FIX(32), FIX(34), FIX(36), FIX(38),
   FIX(40), FIX(42), FIX(44), FIX(46),
   FIX(48), FIX(50), FIX(52), FIX(54),
   FIX(56), FIX(58), FIX(60), FIX(62)
  };
 #undef FIX

 const uint32_t mult = multipliers[quant];
 const uint16_t quant_m_2 = uint16_t(quant << 1);
 const uint16_t quant_d_2 = uint16_t(quant >> 1);
 uint32_t sum = 0;
 uint32_t i;

 for (i = 0; i < 64; i++) {
         int16_t acLevel = coeff[i];

         if (acLevel < 0) {
                 acLevel = (-acLevel) - quant_d_2;
                 if (acLevel < quant_m_2) {
                         coeff[i] = 0;
                         continue;
                 }

                 acLevel = int16_t((acLevel * mult) >> SCALEBITS);
                 sum += acLevel;         /* sum += |acLevel| */
                 coeff[i] = -acLevel;
         } else {
                 acLevel = int16_t(acLevel-quant_d_2);
                 if (acLevel < quant_m_2) {
                         coeff[i] = 0;
                         continue;
                 }
                 acLevel = int16_t((acLevel * mult) >> SCALEBITS);
                 sum += acLevel;
                 coeff[i] = acLevel;
         }
 }
 #undef SCALEBITS
}

void TimgFilterDCT::dequant_h263_inter(int16_t * data,const uint32_t quant,const uint16_t *)
{
 const uint16_t quant_m_2 = uint16_t(quant << 1);
 const uint16_t quant_add = uint16_t (quant & 1 ? quant : quant - 1);
 int i;

 for (i = 0; i < 64; i++) {
         int16_t acLevel = data[i];

         if (acLevel == 0) {
                 data[i] = 0;
         } else if (acLevel < 0) {
                 acLevel = acLevel * quant_m_2 - quant_add;
                 data[i] = (acLevel >= -2048 ? acLevel : -2048);
         } else {
                 acLevel = acLevel * quant_m_2 + quant_add;
                 data[i] = (acLevel <= 2047 ? acLevel : 2047);
         }
 }
}

void TimgFilterDCT::h263(void)
{
 quant_h263_inter(pWorkArea,quant);
 dequant_h263_inter(pWorkArea,quant);
}

void TimgFilterDCT::quant_mpeg_inter(int16_t * coeff,const uint32_t quant,const uint16_t * mpeg_quant_matrices)
{
 #define SCALEBITS 17
 #define FIX(X)	  ((1UL << SCALEBITS) / (X) + 1)
 static const uint32_t multipliers[32] =
  {
   0,       FIX(2),  FIX(4),  FIX(6),
   FIX(8),	 FIX(10), FIX(12), FIX(14),
   FIX(16), FIX(18), FIX(20), FIX(22),
   FIX(24), FIX(26), FIX(28), FIX(30),
   FIX(32), FIX(34), FIX(36), FIX(38),
   FIX(40), FIX(42), FIX(44), FIX(46),
   FIX(48), FIX(50), FIX(52), FIX(54),
   FIX(56), FIX(58), FIX(60), FIX(62)
  };
 #undef FIX
 #undef SCALEBITS

 const uint32_t mult = multipliers[quant];
 const uint16_t *inter_matrix = mpeg_quant_matrices;
 uint32_t sum = 0;
 int i;

 for (i = 0; i < 64; i++) {
         if (coeff[i] < 0) {
                 uint32_t level = -coeff[i];

                 level = ((level << 4) + (inter_matrix[i] >> 1)) / inter_matrix[i];
                 level = (level * mult) >> 17;
                 sum += level;
                 coeff[i] = -(int16_t) level;
         } else if (coeff[i] > 0) {
                 uint32_t level = coeff[i];

                 level = ((level << 4) + (inter_matrix[i] >> 1)) / inter_matrix[i];
                 level = (level * mult) >> 17;
                 sum += level;
                 coeff[i] = int16_t(level);
         } else {
                 coeff[i] = 0;
         }
 }
}

void TimgFilterDCT::dequant_mpeg_inter(int16_t * data,const uint32_t quant,const uint16_t * mpeg_quant_matrices)
{
 uint32_t sum = 0;
 const uint16_t *inter_matrix = (mpeg_quant_matrices);
 int i;

 for (i = 0; i < 64; i++) {
         if (data[i] == 0) {
                 data[i] = 0;
         } else if (data[i] < 0) {
                 int32_t level = -data[i];

                 level = ((2 * level + 1) * inter_matrix[i] * quant) >> 4;
                 data[i] = int16_t(level <= 2048 ? -level : -2048);
         } else {
                 uint32_t level = data[i];

                 level = ((2 * level + 1) * inter_matrix[i] * quant) >> 4;
                 data[i] = int16_t(level <= 2047 ? level : 2047);
         }

         sum ^= data[i];
 }

 /*      mismatch control */
 if ((sum & 1) == 0) {
         data[63] ^= 1;
 }
}

void TimgFilterDCT::mpeg(void)
{
 quant_mpeg_inter(pWorkArea,quant,(const uint16_t*)&factors[0][0]);
 dequant_mpeg_inter(pWorkArea,quant,(const uint16_t*)&factors[0][0]);
}

HRESULT TimgFilterDCT::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TdctSettings *cfg=(const TdctSettings*)cfg0;
 init(pict,cfg->full,cfg->half);
 if (pictRect.dx>=8 && pictRect.dy>=8)
  {
   bool modechange=oldMode!=cfg->mode;
   if (modechange)
    switch (oldMode=cfg->mode)
     {
      case 1:processDct=&TimgFilterDCT::h263;break;
      case 2:processDct=&TimgFilterDCT::mpeg;break;
      default:
      case 0:processDct=&TimgFilterDCT::multiply;break;
     }
   if (oldMode==0 && (modechange || memcmp(oldfac,&cfg->fac0,sizeof(oldfac))!=0))
    {
     memcpy(oldfac,&cfg->fac0,sizeof(oldfac));
     for (int i=0;i<=7;i++)
      for (int j=0;j<=7;j++)
       factors[i][j]=(short)((oldfac[i]/1000.0) * (oldfac[j]/1000.0) * 8);
    }
   if (oldMode==2 && (modechange || memcpy(oldmatrix,&cfg->matrix0,sizeof(oldmatrix))!=0))
    {
     memcpy(oldmatrix,&cfg->matrix0,sizeof(oldmatrix));
     const unsigned char *m=(const unsigned char*)&cfg->matrix0;
     for (int i=0;i<8;i++)
      for (int j=0;j<8;j++)
       factors[i][j]=(short)limit<int>(*m++,1,255);
    }
   quant=cfg->quant;
   const unsigned char *srcY;
   getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&srcY,NULL,NULL,NULL);
   unsigned char *dstY;
   getNext(csp1,pict,cfg->full,&dstY,NULL,NULL,NULL);

   unsigned int cycles=dx1[0]&~7;

   if (dx1[0]&7)
    TffPict::copy(dstY+cycles,stride2[0],srcY+cycles,stride1[0],dx1[0]&7,dy1[0]);

   __m64 m0=_mm_setzero_si64();
   const stride_t stride1_0=stride1[0],stride2_0=stride2[0];
   for (unsigned int y=0;y<=dy1[0]-7;srcY+=8*stride1_0,dstY+=8*stride2_0,y+=8)
    {
     const unsigned char *srcLn=srcY;unsigned char *dstLn=dstY,*dstLnEnd=dstLn+cycles;
     for (;dstLn<dstLnEnd;srcLn+=8,dstLn+=8)
      {
       __m64 mm0=*(__m64*)(srcLn+0*stride1_0);
       __m64 mm2=*(__m64*)(srcLn+1*stride1_0);
       *(__m64*)(pWorkArea+ 0)=_mm_unpacklo_pi8(mm0,m0);
       *(__m64*)(pWorkArea+ 4)=_mm_unpackhi_pi8(mm0,m0);
       *(__m64*)(pWorkArea+ 8)=_mm_unpacklo_pi8(mm2,m0);
       *(__m64*)(pWorkArea+12)=_mm_unpackhi_pi8(mm2,m0);

       mm0=*(__m64*)(srcLn+2*stride1_0);
       mm2=*(__m64*)(srcLn+3*stride1_0);
       *(__m64*)(pWorkArea+16)=_mm_unpacklo_pi8(mm0,m0);
       *(__m64*)(pWorkArea+20)=_mm_unpackhi_pi8(mm0,m0);
       *(__m64*)(pWorkArea+24)=_mm_unpacklo_pi8(mm2,m0);
       *(__m64*)(pWorkArea+28)=_mm_unpackhi_pi8(mm2,m0);

       mm0=*(__m64*)(srcLn+4*stride1_0);
       mm2=*(__m64*)(srcLn+5*stride1_0);
       *(__m64*)(pWorkArea+32)=_mm_unpacklo_pi8(mm0,m0);
       *(__m64*)(pWorkArea+36)=_mm_unpackhi_pi8(mm0,m0);
       *(__m64*)(pWorkArea+40)=_mm_unpacklo_pi8(mm2,m0);
       *(__m64*)(pWorkArea+44)=_mm_unpackhi_pi8(mm2,m0);

       mm0=*(__m64*)(srcLn+6*stride1_0);
       mm2=*(__m64*)(srcLn+7*stride1_0);
       *(__m64*)(pWorkArea+48)=_mm_unpacklo_pi8(mm0,m0);
       *(__m64*)(pWorkArea+52)=_mm_unpackhi_pi8(mm0,m0);
       *(__m64*)(pWorkArea+56)=_mm_unpacklo_pi8(mm2,m0);
       *(__m64*)(pWorkArea+60)=_mm_unpackhi_pi8(mm2,m0);

       fdct(pWorkArea);
       (this->*processDct)();
       idct(pWorkArea);

       *(__m64*)(dstLn+0*stride2_0)=_mm_packs_pu16(*(__m64*)(pWorkArea+0*8),*(__m64*)(pWorkArea+0*8+4));
       *(__m64*)(dstLn+1*stride2_0)=_mm_packs_pu16(*(__m64*)(pWorkArea+1*8),*(__m64*)(pWorkArea+1*8+4));
       *(__m64*)(dstLn+2*stride2_0)=_mm_packs_pu16(*(__m64*)(pWorkArea+2*8),*(__m64*)(pWorkArea+2*8+4));
       *(__m64*)(dstLn+3*stride2_0)=_mm_packs_pu16(*(__m64*)(pWorkArea+3*8),*(__m64*)(pWorkArea+3*8+4));
       *(__m64*)(dstLn+4*stride2_0)=_mm_packs_pu16(*(__m64*)(pWorkArea+4*8),*(__m64*)(pWorkArea+4*8+4));
       *(__m64*)(dstLn+5*stride2_0)=_mm_packs_pu16(*(__m64*)(pWorkArea+5*8),*(__m64*)(pWorkArea+5*8+4));
       *(__m64*)(dstLn+6*stride2_0)=_mm_packs_pu16(*(__m64*)(pWorkArea+6*8),*(__m64*)(pWorkArea+6*8+4));
       *(__m64*)(dstLn+7*stride2_0)=_mm_packs_pu16(*(__m64*)(pWorkArea+7*8),*(__m64*)(pWorkArea+7*8+4));
      }
    }
   _mm_empty();
   if (dy1[0]&7)
    TffPict::copy(dstY,stride2[0],srcY,stride1[0],dx1[0],dy1[0]&7);
  }
 return parent->deliverSample(++it,pict);
}

short TimgFilterDCT::iclip[1024],*TimgFilterDCT::iclp;

void TimgFilterDCT::idct_c_init(void)
{
	iclp = iclip + 512;
	for (int i = -512; i < 512; i++)
		iclp[i] = (short)limit(i,-256,255);
}
void TimgFilterDCT::idct_c(short *block)
{

	/*
	 * idct_int32_init() must be called before the first call to this
	 * function!
	 */

static const int W1=2841;				/* 2048*sqrt(2)*cos(1*pi/16) */
static const int W2=2676;				/* 2048*sqrt(2)*cos(2*pi/16) */
static const int W3=2408;				/* 2048*sqrt(2)*cos(3*pi/16) */
static const int W5=1609;				/* 2048*sqrt(2)*cos(5*pi/16) */
static const int W6=1108;				/* 2048*sqrt(2)*cos(6*pi/16) */
static const int W7=565;				/* 2048*sqrt(2)*cos(7*pi/16) */

	short *blk;
	long i;
	long X0, X1, X2, X3, X4, X5, X6, X7, X8;


	for (i = 0; i < 8; i++)		/* idct rows */
	{
		blk = block + (i << 3);
		if (!
			((X1 = blk[4] << 11) | (X2 = blk[6]) | (X3 = blk[2]) | (X4 =
																	blk[1]) |
			 (X5 = blk[7]) | (X6 = blk[5]) | (X7 = blk[3]))) {
			blk[0] = blk[1] = blk[2] = blk[3] = blk[4] = blk[5] = blk[6] =
				blk[7] = blk[0] << 3;
			continue;
		}

		X0 = (blk[0] << 11) + 128;	/* for proper rounding in the fourth stage  */

		/* first stage  */
		X8 = W7 * (X4 + X5);
		X4 = X8 + (W1 - W7) * X4;
		X5 = X8 - (W1 + W7) * X5;
		X8 = W3 * (X6 + X7);
		X6 = X8 - (W3 - W5) * X6;
		X7 = X8 - (W3 + W5) * X7;

		/* second stage  */
		X8 = X0 + X1;
		X0 -= X1;
		X1 = W6 * (X3 + X2);
		X2 = X1 - (W2 + W6) * X2;
		X3 = X1 + (W2 - W6) * X3;
		X1 = X4 + X6;
		X4 -= X6;
		X6 = X5 + X7;
		X5 -= X7;

		/* third stage  */
		X7 = X8 + X3;
		X8 -= X3;
		X3 = X0 + X2;
		X0 -= X2;
		X2 = (181 * (X4 + X5) + 128) >> 8;
		X4 = (181 * (X4 - X5) + 128) >> 8;

		/* fourth stage  */

		blk[0] = (short) ((X7 + X1) >> 8);
		blk[1] = (short) ((X3 + X2) >> 8);
		blk[2] = (short) ((X0 + X4) >> 8);
		blk[3] = (short) ((X8 + X6) >> 8);
		blk[4] = (short) ((X8 - X6) >> 8);
		blk[5] = (short) ((X0 - X4) >> 8);
		blk[6] = (short) ((X3 - X2) >> 8);
		blk[7] = (short) ((X7 - X1) >> 8);

	}							/* end for ( i = 0; i < 8; ++i ) IDCT-rows */



	for (i = 0; i < 8; i++)		/* idct columns */
	{
		blk = block + i;
		/* shortcut  */
		if (!
			((X1 = (blk[8 * 4] << 8)) | (X2 = blk[8 * 6]) | (X3 =
															 blk[8 *
																 2]) | (X4 =
																		blk[8 *
																			1])
			 | (X5 = blk[8 * 7]) | (X6 = blk[8 * 5]) | (X7 = blk[8 * 3]))) {
			blk[8 * 0] = blk[8 * 1] = blk[8 * 2] = blk[8 * 3] = blk[8 * 4] =
				blk[8 * 5] = blk[8 * 6] = blk[8 * 7] =
				iclp[(blk[8 * 0] + 32) >> 6];
			continue;
		}

		X0 = (blk[8 * 0] << 8) + 8192;

		/* first stage  */
		X8 = W7 * (X4 + X5) + 4;
		X4 = (X8 + (W1 - W7) * X4) >> 3;
		X5 = (X8 - (W1 + W7) * X5) >> 3;
		X8 = W3 * (X6 + X7) + 4;
		X6 = (X8 - (W3 - W5) * X6) >> 3;
		X7 = (X8 - (W3 + W5) * X7) >> 3;

		/* second stage  */
		X8 = X0 + X1;
		X0 -= X1;
		X1 = W6 * (X3 + X2) + 4;
		X2 = (X1 - (W2 + W6) * X2) >> 3;
		X3 = (X1 + (W2 - W6) * X3) >> 3;
		X1 = X4 + X6;
		X4 -= X6;
		X6 = X5 + X7;
		X5 -= X7;

		/* third stage  */
		X7 = X8 + X3;
		X8 -= X3;
		X3 = X0 + X2;
		X0 -= X2;
		X2 = (181 * (X4 + X5) + 128) >> 8;
		X4 = (181 * (X4 - X5) + 128) >> 8;

		/* fourth stage  */
		blk[8 * 0] = iclp[(X7 + X1) >> 14];
		blk[8 * 1] = iclp[(X3 + X2) >> 14];
		blk[8 * 2] = iclp[(X0 + X4) >> 14];
		blk[8 * 3] = iclp[(X8 + X6) >> 14];
		blk[8 * 4] = iclp[(X8 - X6) >> 14];
		blk[8 * 5] = iclp[(X0 - X4) >> 14];
		blk[8 * 6] = iclp[(X3 - X2) >> 14];
		blk[8 * 7] = iclp[(X7 - X1) >> 14];
	}

}								/* end function idct_int32(block) */

void TimgFilterDCT::fdct_c(short *block)
{
 static const int CONST_BITS =13;
 static const int PASS1_BITS =2;

 static const int FIX_0_298631336= 2446;	/* FIX(0.298631336) */
 static const int FIX_0_390180644= 3196;	/* FIX(0.390180644) */
 static const int FIX_0_541196100= 4433;	/* FIX(0.541196100) */
 static const int FIX_0_765366865= 6270;	/* FIX(0.765366865) */
 static const int FIX_0_899976223= 7373;	/* FIX(0.899976223) */
 static const int FIX_1_175875602= 9633;	/* FIX(1.175875602) */
 static const int FIX_1_501321110=12299;	/* FIX(1.501321110) */
 static const int FIX_1_847759065=15137;	/* FIX(1.847759065) */
 static const int FIX_1_961570560=16069;	/* FIX(1.961570560) */
 static const int FIX_2_053119869=16819;	/* FIX(2.053119869) */
 static const int FIX_2_562915447=20995;	/* FIX(2.562915447) */
 static const int FIX_3_072711026=25172;	/* FIX(3.072711026) */

	int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	int tmp10, tmp11, tmp12, tmp13;
	int z1, z2, z3, z4, z5;
	short *blkptr;
	int *dataptr;
	int data[64];
	int i;

	/* Pass 1: process rows. */
	/* Note results are scaled up by sqrt(8) compared to a true DCT; */
	/* furthermore, we scale the results by 2**PASS1_BITS. */

	dataptr = data;
	blkptr = block;
	for (i = 0; i < 8; i++) {
		tmp0 = blkptr[0] + blkptr[7];
		tmp7 = blkptr[0] - blkptr[7];
		tmp1 = blkptr[1] + blkptr[6];
		tmp6 = blkptr[1] - blkptr[6];
		tmp2 = blkptr[2] + blkptr[5];
		tmp5 = blkptr[2] - blkptr[5];
		tmp3 = blkptr[3] + blkptr[4];
		tmp4 = blkptr[3] - blkptr[4];

		/* Even part per LL&M figure 1 --- note that published figure is faulty;
		 * rotator "sqrt(2)*c1" should be "sqrt(2)*c6".
		 */

		tmp10 = tmp0 + tmp3;
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;

		dataptr[0] = (tmp10 + tmp11) << PASS1_BITS;
		dataptr[4] = (tmp10 - tmp11) << PASS1_BITS;

		z1 = (tmp12 + tmp13) * FIX_0_541196100;
		dataptr[2] =
			roundRshift(z1 + tmp13 * FIX_0_765366865, CONST_BITS - PASS1_BITS);
		dataptr[6] =
			roundRshift(z1 + tmp12 * (-FIX_1_847759065), CONST_BITS - PASS1_BITS);

		/* Odd part per figure 8 --- note paper omits factor of sqrt(2).
		 * cK represents cos(K*pi/16).
		 * i0..i3 in the paper are tmp4..tmp7 here.
		 */

		z1 = tmp4 + tmp7;
		z2 = tmp5 + tmp6;
		z3 = tmp4 + tmp6;
		z4 = tmp5 + tmp7;
		z5 = (z3 + z4) * FIX_1_175875602;	/* sqrt(2) * c3 */

		tmp4 *= FIX_0_298631336;	/* sqrt(2) * (-c1+c3+c5-c7) */
		tmp5 *= FIX_2_053119869;	/* sqrt(2) * ( c1+c3-c5+c7) */
		tmp6 *= FIX_3_072711026;	/* sqrt(2) * ( c1+c3+c5-c7) */
		tmp7 *= FIX_1_501321110;	/* sqrt(2) * ( c1+c3-c5-c7) */
		z1 *= -FIX_0_899976223;	/* sqrt(2) * (c7-c3) */
		z2 *= -FIX_2_562915447;	/* sqrt(2) * (-c1-c3) */
		z3 *= -FIX_1_961570560;	/* sqrt(2) * (-c3-c5) */
		z4 *= -FIX_0_390180644;	/* sqrt(2) * (c5-c3) */

		z3 += z5;
		z4 += z5;

		dataptr[7] = roundRshift(tmp4 + z1 + z3, CONST_BITS - PASS1_BITS);
		dataptr[5] = roundRshift(tmp5 + z2 + z4, CONST_BITS - PASS1_BITS);
		dataptr[3] = roundRshift(tmp6 + z2 + z3, CONST_BITS - PASS1_BITS);
		dataptr[1] = roundRshift(tmp7 + z1 + z4, CONST_BITS - PASS1_BITS);

		dataptr += 8;			/* advance pointer to next row */
		blkptr += 8;
	}

	/* Pass 2: process columns.
	 * We remove the PASS1_BITS scaling, but leave the results scaled up
	 * by an overall factor of 8.
	 */

	dataptr = data;
	for (i = 0; i < 8; i++) {
		tmp0 = dataptr[0] + dataptr[56];
		tmp7 = dataptr[0] - dataptr[56];
		tmp1 = dataptr[8] + dataptr[48];
		tmp6 = dataptr[8] - dataptr[48];
		tmp2 = dataptr[16] + dataptr[40];
		tmp5 = dataptr[16] - dataptr[40];
		tmp3 = dataptr[24] + dataptr[32];
		tmp4 = dataptr[24] - dataptr[32];

		/* Even part per LL&M figure 1 --- note that published figure is faulty;
		 * rotator "sqrt(2)*c1" should be "sqrt(2)*c6".
		 */

		tmp10 = tmp0 + tmp3;
		tmp13 = tmp0 - tmp3;
		tmp11 = tmp1 + tmp2;
		tmp12 = tmp1 - tmp2;

		dataptr[0] = roundRshift(tmp10 + tmp11, PASS1_BITS);
		dataptr[32] = roundRshift(tmp10 - tmp11, PASS1_BITS);

		z1 = (tmp12 + tmp13) * FIX_0_541196100;
		dataptr[16] =
			roundRshift(z1 + tmp13 * FIX_0_765366865, CONST_BITS + PASS1_BITS);
		dataptr[48] =
			roundRshift(z1 + tmp12 * (-FIX_1_847759065), CONST_BITS + PASS1_BITS);

		/* Odd part per figure 8 --- note paper omits factor of sqrt(2).
		 * cK represents cos(K*pi/16).
		 * i0..i3 in the paper are tmp4..tmp7 here.
		 */

		z1 = tmp4 + tmp7;
		z2 = tmp5 + tmp6;
		z3 = tmp4 + tmp6;
		z4 = tmp5 + tmp7;
		z5 = (z3 + z4) * FIX_1_175875602;	/* sqrt(2) * c3 */

		tmp4 *= FIX_0_298631336;	/* sqrt(2) * (-c1+c3+c5-c7) */
		tmp5 *= FIX_2_053119869;	/* sqrt(2) * ( c1+c3-c5+c7) */
		tmp6 *= FIX_3_072711026;	/* sqrt(2) * ( c1+c3+c5-c7) */
		tmp7 *= FIX_1_501321110;	/* sqrt(2) * ( c1+c3-c5-c7) */
		z1 *= -FIX_0_899976223;	/* sqrt(2) * (c7-c3) */
		z2 *= -FIX_2_562915447;	/* sqrt(2) * (-c1-c3) */
		z3 *= -FIX_1_961570560;	/* sqrt(2) * (-c3-c5) */
		z4 *= -FIX_0_390180644;	/* sqrt(2) * (c5-c3) */

		z3 += z5;
		z4 += z5;

		dataptr[56] = roundRshift(tmp4 + z1 + z3, CONST_BITS + PASS1_BITS);
		dataptr[40] = roundRshift(tmp5 + z2 + z4, CONST_BITS + PASS1_BITS);
		dataptr[24] = roundRshift(tmp6 + z2 + z3, CONST_BITS + PASS1_BITS);
		dataptr[8] = roundRshift(tmp7 + z1 + z4, CONST_BITS + PASS1_BITS);

		dataptr++;				/* advance pointer to next column */
	}
	/* descale */
	for (i = 0; i < 64; i++)
		block[i] = (short int) roundRshift(data[i], 3);
}
#ifdef DCT_SSE2
void TimgFilterDCT::fdct_sse2(short *block)
{
 static __align16(const unsigned short,fTab1[])=
  {
   0x4000, 0x4000, 0x58c5, 0x4b42,
   0xdd5d, 0xac61, 0xa73b, 0xcdb7,
   0x4000, 0x4000, 0x3249, 0x11a8,
   0x539f, 0x22a3, 0x4b42, 0xee58,
   0x4000, 0xc000, 0x3249, 0xa73b,
   0x539f, 0xdd5d, 0x4b42, 0xa73b,
   0xc000, 0x4000, 0x11a8, 0x4b42,
   0x22a3, 0xac61, 0x11a8, 0xcdb7
  };
 static __align16(const unsigned short,fTab2[])=
  {
   0x58c5, 0x58c5, 0x7b21, 0x6862,
   0xcff5, 0x8c04, 0x84df, 0xba41,
   0x58c5, 0x58c5, 0x45bf, 0x187e,
   0x73fc, 0x300b, 0x6862, 0xe782,
   0x58c5, 0xa73b, 0x45bf, 0x84df,
   0x73fc, 0xcff5, 0x6862, 0x84df,
   0xa73b, 0x58c5, 0x187e, 0x6862,
   0x300b, 0x8c04, 0x187e, 0xba41
  };
 static __align16(const unsigned short,fTab3[])=
  {
   0x539f, 0x539f, 0x73fc, 0x6254,
   0xd2bf, 0x92bf, 0x8c04, 0xbe4d,
   0x539f, 0x539f, 0x41b3, 0x1712,
   0x6d41, 0x2d41, 0x6254, 0xe8ee,
   0x539f, 0xac61, 0x41b3, 0x8c04,
   0x6d41, 0xd2bf, 0x6254, 0x8c04,
   0xac61, 0x539f, 0x1712, 0x6254,
   0x2d41, 0x92bf, 0x1712, 0xbe4d
  };
 static __align16(const unsigned short,fTab4[])=
  {
   0x4b42, 0x4b42, 0x6862, 0x587e,
   0xd746, 0x9dac, 0x979e, 0xc4df,
   0x4b42, 0x4b42, 0x3b21, 0x14c3,
   0x6254, 0x28ba, 0x587e, 0xeb3d,
   0x4b42, 0xb4be, 0x3b21, 0x979e,
   0x6254, 0xd746, 0x587e, 0x979e,
   0xb4be, 0x4b42, 0x14c3, 0x587e,
   0x28ba, 0x9dac, 0x14c3, 0xc4df
  };

 static __align16(const unsigned short,Fdct_Rnd0[])={ 6,8,8,8, 6,8,8,8};
 static __align16(const unsigned short,Fdct_Rnd1[])={ 8,8,8,8, 8,8,8,8};
 static __align16(const unsigned short,Fdct_Rnd2[])={10,8,8,8, 8,8,8,8};

 struct Tfdct
  {
   static __forceinline void fLLM_PASS(unsigned char *src,int shift,__m128i &xmm0,__m128i &xmm1,__m128i &xmm2,__m128i &xmm3,__m128i &xmm4,__m128i &xmm5,__m128i &xmm6,__m128i &xmm7)
    {
     movdqa (xmm0, src+0*16);   // In0
     movdqa (xmm2, src+2*16);   // In2
     movdqa (xmm3, xmm0    );
     movdqa (xmm4, xmm2    );
     movdqa (xmm7, src+7*16);   // In7
     movdqa (xmm5, src+5*16);   // In5

     psubsw (xmm0, xmm7);         // t7 = In0-In7
     paddsw (xmm7, xmm3);         // t0 = In0+In7
     psubsw (xmm2, xmm5);         // t5 = In2-In5
     paddsw (xmm5, xmm4);         // t2 = In2+In5

     movdqa (xmm3, src+3*16);   // In3
     movdqa (xmm4, src+4*16);   // In4
     movdqa (xmm1, xmm3);
     psubsw (xmm3, xmm4);         // t4 = In3-In4
     paddsw (xmm4, xmm1);         // t3 = In3+In4
     movdqa (xmm6, src+6*16);   // In6
     movdqa (xmm1, src+1*16);   // In1
     psubsw (xmm1, xmm6);         // t6 = In1-In6
     paddsw (xmm6, src+1*16);   // t1 = In1+In6

     psubsw (xmm7, xmm4);         // tm03 = t0-t3
     psubsw (xmm6, xmm5);         // tm12 = t1-t2
     paddsw (xmm4, xmm4);         // 2.t3
     paddsw (xmm5, xmm5);         // 2.t2
     paddsw (xmm4, xmm7);         // tp03 = t0+t3
     paddsw (xmm5, xmm6);         // tp12 = t1+t2

     psllw  (xmm2, shift+1);        // shift t5 (shift +1 to..
     psllw  (xmm1, shift+1);        // shift t6  ..compensate cos4/2)
     psllw  (xmm4, shift);          // shift t3
     psllw  (xmm5, shift);          // shift t2
     psllw  (xmm7, shift);          // shift t0
     psllw  (xmm6, shift);          // shift t1
     psllw  (xmm3, shift);          // shift t4
     psllw  (xmm0, shift);          // shift t7

     psubsw (xmm4, xmm5);         // out4 = tp03-tp12
     psubsw (xmm1, xmm2);         // xmm1: t6-t5
     paddsw (xmm5, xmm5);
     paddsw (xmm2, xmm2);
     paddsw (xmm5, xmm4);         // out0 = tp03+tp12
     movdqa (src+4*16, xmm4);   // => out4
     paddsw (xmm2, xmm1    );     // xmm2: t6+t5
     movdqa (src+0*16, xmm5);   // => out0

     const __m128i tan1=_mm_set1_epi16(0x32ec);    // tan( pi/16))
     const __m128i tan2=_mm_set1_epi16(0x6a0a);    // tan(2pi/16)  (=sqrt(2)-1)
     const __m128i tan3=_mm_set1_epi16(0xab0e);    // tan(3pi/16)-1
     const __m128i sqrt2=_mm_set1_epi16(0x5a82);    // 0.5/sqrt(2)
     const __m128i Rounder1=_mm_set_epi16(1,1,1,1, 1,1,1,1);

     movdqa (xmm4, tan2);      // xmm4 <= tan2
     pmulhw (xmm4, xmm7);         // tm03*tan2
     movdqa (xmm5, tan2);      // xmm5 <= tan2
     psubsw (xmm4, xmm6);         // out6 = tm03*tan2 - tm12
     pmulhw (xmm5, xmm6);         // tm12*tan2
     paddsw (xmm5, xmm7);         // out2 = tm12*tan2 + tm03

     movdqa (xmm6, sqrt2);
     movdqa (xmm7, Rounder1);

     pmulhw (xmm2, xmm6);         // xmm2: tp65 = (t6 + t5)*cos4
     por    (xmm5, xmm7);         // correct out2
     por    (xmm4, xmm7);         // correct out6
     pmulhw (xmm1, xmm6);         // xmm1: tm65 = (t6 - t5)*cos4
     por    (xmm2, xmm7);         // correct tp65

     movdqa (src+2*16, xmm5);   // => out2
     movdqa (xmm5, xmm3    );     // save t4
     movdqa (src+6*16, xmm4);   // => out6
     movdqa (xmm4, xmm0    );     // save t7

     psubsw (xmm3, xmm1);         // xmm3: tm465 = t4 - tm65
     psubsw (xmm0, xmm2);         // xmm0: tm765 = t7 - tp65
     paddsw (xmm2, xmm4);         // xmm2: tp765 = t7 + tp65
     paddsw (xmm1, xmm5);         // xmm1: tp465 = t4 + tm65

     movdqa (xmm4, tan3);      // tan3 - 1
     movdqa (xmm5, tan1);      // tan1

     movdqa (xmm7, xmm3);         // save tm465
     pmulhw (xmm3, xmm4);         // tm465*(tan3-1)
     movdqa (xmm6, xmm1);         // save tp465
     pmulhw (xmm1, xmm5);         // tp465*tan1

     paddsw (xmm3, xmm7);         // tm465*tan3
     pmulhw (xmm4, xmm0);         // tm765*(tan3-1)
     paddsw (xmm4, xmm0);         // tm765*tan3
     pmulhw (xmm5, xmm2);         // tp765*tan1

     paddsw (xmm1, xmm2);         // out1 = tp765 + tp465*tan1
     psubsw (xmm0, xmm3);         // out3 = tm765 - tm465*tan3
     paddsw (xmm7, xmm4);         // out5 = tm465 + tm765*tan3
     psubsw (xmm5, xmm6);         // out7 =-tp465 + tp765*tan1

     movdqa (src+1*16, xmm1);   // => out1
     movdqa (src+3*16, xmm0);   // => out3
     movdqa (src+5*16, xmm7);   // => out5
     movdqa (src+7*16, xmm5);   // => out7
    }
   static __forceinline void fMTX_MULT(unsigned char *ecx,int src,const unsigned char *Coeffs,const unsigned char *rounders,__m128i &xmm0,__m128i &xmm1,__m128i &xmm2,__m128i &xmm3)
    {
     movdqa  ( xmm0, ecx+src*16+0);   // xmm0 = 01234567
     xmm1=_mm_shufflehi_epi16(xmm0,0x1b);// pshufhw ( xmm1, xmm0, 00011011b); // xmm1 = ----7654
     xmm0=_mm_shuffle_epi32(xmm0,0x44); //pshufd  ( xmm0, xmm0, 01000100b);
     xmm1=_mm_shuffle_epi32(xmm1,0xee); //pshufd  ( xmm1, xmm1, 11101110b);

     movdqa  ( xmm2, xmm0);
     paddsw  (xmm0, xmm1);              // xmm0 = a0 a1 a2 a3
     psubsw  (xmm2, xmm1);              // xmm2 = b0 b1 b2 b3

     punpckldq (xmm0, xmm2);            // xmm0 = a0 a1 b0 b1a2 a3 b2 b3
     xmm2=_mm_shuffle_epi32(xmm0,0x4e);//pshufd    (xmm2, xmm0, 01001110b); // xmm2 = a2 a3 b2 b3a0 a1 b0 b1

       //  M00 M01    M16 M17 M06 M07    M22 M23  x mm0 = 0 /1 /2'/3'
       //  M02 M03    M18 M19 M04 M05    M20 M21  x mm2 = 0'/1'/2 /3
       //  M08 M09    M24 M25 M14 M15    M30 M31  x mm0 = 4 /5 /6'/7'
       //  M10 M11    M26 M27 M12 M13    M28 M29  x mm2 = 4'/5'/6 /7

     movdqa  (xmm1, Coeffs+16);
     movdqa  (xmm3, Coeffs+32);
     pmaddwd (xmm1, xmm2);
     pmaddwd (xmm3, xmm0);
     pmaddwd (xmm2, Coeffs+48);
     pmaddwd (xmm0, Coeffs+ 0);

     paddd   (xmm0, xmm1);             //   out0 | out1  out2 | out3
     paddd   (xmm2, xmm3);             //   out4 | out5  out6 | out7
     psrad   (xmm0, 16  );
     psrad   (xmm2, 16  );

     packssdw (xmm0, xmm2);            //   out0 .. out7
     paddsw   (xmm0, rounders);            //  Round

     psraw    (xmm0, 4);               // => -2048, 2047

     movdqa  (ecx+src*16+0, xmm0);
    }
  };

 __m128i xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7;
 unsigned char *ecx=(unsigned char*)block;
 Tfdct::fLLM_PASS(ecx,3,xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7);
 Tfdct::fMTX_MULT(ecx,0, (const unsigned char*)fTab1, (const unsigned char*)Fdct_Rnd0,xmm0,xmm1,xmm2,xmm3);
 Tfdct::fMTX_MULT(ecx,1, (const unsigned char*)fTab2, (const unsigned char*)Fdct_Rnd2,xmm0,xmm1,xmm2,xmm3);
 Tfdct::fMTX_MULT(ecx,2, (const unsigned char*)fTab3, (const unsigned char*)Fdct_Rnd1,xmm0,xmm1,xmm2,xmm3);
 Tfdct::fMTX_MULT(ecx,3, (const unsigned char*)fTab4, (const unsigned char*)Fdct_Rnd1,xmm0,xmm1,xmm2,xmm3);
 Tfdct::fMTX_MULT(ecx,4, (const unsigned char*)fTab1, (const unsigned char*)Fdct_Rnd0,xmm0,xmm1,xmm2,xmm3);
 Tfdct::fMTX_MULT(ecx,5, (const unsigned char*)fTab4, (const unsigned char*)Fdct_Rnd1,xmm0,xmm1,xmm2,xmm3);
 Tfdct::fMTX_MULT(ecx,6, (const unsigned char*)fTab3, (const unsigned char*)Fdct_Rnd1,xmm0,xmm1,xmm2,xmm3);
 Tfdct::fMTX_MULT(ecx,7, (const unsigned char*)fTab2, (const unsigned char*)Fdct_Rnd1,xmm0,xmm1,xmm2,xmm3);
}

void TimgFilterDCT::idct_sse2(short *block)
{
 static __align16(const unsigned short,iTab1[])=
  {
   0x4000, 0x539f, 0x4000, 0x22a3,
   0x4000, 0xdd5d, 0x4000, 0xac61,
   0x4000, 0x22a3, 0xc000, 0xac61,
   0xc000, 0x539f, 0x4000, 0xdd5d,
   0x58c5, 0x4b42, 0x4b42, 0xee58,
   0x3249, 0xa73b, 0x11a8, 0xcdb7,
   0x3249, 0x11a8, 0xa73b, 0xcdb7,
   0x11a8, 0x4b42, 0x4b42, 0xa73b
  };
 static __align16(const unsigned short,iTab2[])=
  {
   0x58c5, 0x73fc, 0x58c5, 0x300b,
   0x58c5, 0xcff5, 0x58c5, 0x8c04,
   0x58c5, 0x300b, 0xa73b, 0x8c04,
   0xa73b, 0x73fc, 0x58c5, 0xcff5,
   0x7b21, 0x6862, 0x6862, 0xe782,
   0x45bf, 0x84df, 0x187e, 0xba41,
   0x45bf, 0x187e, 0x84df, 0xba41,
   0x187e, 0x6862, 0x6862, 0x84df
  };
 static __align16(const unsigned short,iTab3[])=
  {
   0x539f, 0x6d41, 0x539f, 0x2d41,
   0x539f, 0xd2bf, 0x539f, 0x92bf,
   0x539f, 0x2d41, 0xac61, 0x92bf,
   0xac61, 0x6d41, 0x539f, 0xd2bf,
   0x73fc, 0x6254, 0x6254, 0xe8ee,
   0x41b3, 0x8c04, 0x1712, 0xbe4d,
   0x41b3, 0x1712, 0x8c04, 0xbe4d,
   0x1712, 0x6254, 0x6254, 0x8c04
  };
 static __align16(const unsigned short,iTab4[])=
  {
   0x4b42, 0x6254, 0x4b42, 0x28ba,
   0x4b42, 0xd746, 0x4b42, 0x9dac,
   0x4b42, 0x28ba, 0xb4be, 0x9dac,
   0xb4be, 0x6254, 0x4b42, 0xd746,
   0x6862, 0x587e, 0x587e, 0xeb3d,
   0x3b21, 0x979e, 0x14c3, 0xc4df,
   0x3b21, 0x14c3, 0x979e, 0xc4df,
   0x14c3, 0x587e, 0x587e, 0x979e
  };

 static __align16(const uint32_t,Idct_Rnd0[])={65535, 65535, 65535, 65535};
 static __align16(const uint32_t,Idct_Rnd1[])={ 3612,  3612,  3612,  3612};
 static __align16(const uint32_t,Idct_Rnd2[])={ 2271,  2271,  2271,  2271};
 static __align16(const uint32_t,Idct_Rnd3[])={ 1203,  1203,  1203,  1203};
 static __align16(const uint32_t,Idct_Rnd4[])={ 1023,  1023,  1023,  1023};
 static __align16(const uint32_t,Idct_Rnd5[])={  102,   102,   102,   102};
 static __align16(const uint32_t,Idct_Rnd6[])={  398,   398,   398,   398};
 static __align16(const uint32_t,Idct_Rnd7[])={  469,   469,   469,   469};

 struct Tidct
  {
   static __forceinline void iMTX_MULT(unsigned char *ecx,int src,const unsigned char *Table,const unsigned char *rounder,int shift,__m128i &xmm0,__m128i &xmm4,__m128i &xmm5,__m128i &xmm6,__m128i &xmm7)
    {
     movdqa  (xmm0, ecx+src*16);     // xmm0 = 01234567

     xmm0=_mm_shufflelo_epi16(xmm0,0xd8);// pshuflw xmm0, xmm0, 11011000b // 02134567  // these two shufflings could be
     xmm0=_mm_shufflehi_epi16(xmm0,0xd8);// pshufhw xmm0, xmm0, 11011000b // 02134657  // integrated in zig-zag orders

     xmm4=_mm_shuffle_epi32(xmm0,0x00);// pshufd  xmm4, xmm0, 00000000b // 02020202
     xmm5=_mm_shuffle_epi32(xmm0,0xaa);// pshufd  xmm5, xmm0, 10101010b // 46464646
     xmm6=_mm_shuffle_epi32(xmm0,0x55);// pshufd  xmm6, xmm0, 01010101b // 13131313
     xmm7=_mm_shuffle_epi32(xmm0,0xff);// pshufd  xmm7, xmm0, 11111111b // 57575757

     pmaddwd (xmm4, Table+ 0);   // dot M00,M01M04,M05M08,M09M12,M13
     pmaddwd (xmm5, Table+16);   // dot M02,M03M06,M07M10,M11M14,M15
     pmaddwd (xmm6, Table+32);   // dot M16,M17M20,M21M24,M25M28,M29
     pmaddwd (xmm7, Table+48);   // dot M18,M19M22,M23M26,M27M30,M31
     paddd   (xmm4, rounder);      // Round

     paddd   (xmm6, xmm7);      // b0|b1|b2|b3
     paddd   (xmm4, xmm5);      // a0|a1|a2|a3

     movdqa  (xmm7, xmm6);
     paddd   (xmm6, xmm4);      // mm6=a+b
     psubd   (xmm4, xmm7);      // mm4=a-b
     psrad   (xmm6, shift);        // => out 0123
     psrad   (xmm4, shift);        // => out 7654

     packssdw (xmm6, xmm4);     // 01237654

     xmm6=_mm_shufflehi_epi16(xmm6,0x1b);// pshufhw (xmm6, xmm6, 00011011b // 01234567

     movdqa  (ecx+src*16, xmm6);
    }
   static __forceinline void iLLM_PASS(unsigned char *src,__m128i &xmm0,__m128i &xmm1,__m128i &xmm2,__m128i &xmm3,__m128i &xmm4,__m128i &xmm5,__m128i &xmm6,__m128i &xmm7)
    {
     const __m128i tan1=_mm_set1_epi16(0x32ec);    // tan( pi/16))
     const __m128i tan2=_mm_set1_epi16(0x6a0a);    // tan(2pi/16)  (=sqrt(2)-1)
     const __m128i tan3=_mm_set1_epi16(0xab0e);    // tan(3pi/16)-1
     const __m128i sqrt2=_mm_set1_epi16(0x5a82);    // 0.5/sqrt(2)
     movdqa (xmm0, tan3);     // t3-1
     movdqa (xmm3, src+16*3);  // x3
     movdqa (xmm1, xmm0    );   // t3-1
     movdqa (xmm5, src+16*5);  // x5

     movdqa (xmm4, tan1    ); // t1
     movdqa (xmm6, src+16*1);  // x1
     movdqa (xmm7, src+16*7);  // x7
     movdqa (xmm2, xmm4    );   // t1

     pmulhw (xmm0, xmm3);       // x3*(t3-1)
     pmulhw (xmm1, xmm5);       // x5*(t3-1)
     paddsw (xmm0, xmm3);       // x3*t3
     paddsw (xmm1, xmm5);       // x5*t3
     psubsw (xmm0, xmm5);       // x3*t3-x5 = tm35
     paddsw (xmm1, xmm3);       // x3+x5*t3 = tp35

     pmulhw (xmm4, xmm7);       // x7*t1
     pmulhw (xmm2, xmm6);       // x1*t1
     paddsw (xmm4, xmm6);       // x1+t1*x7 = tp17
     psubsw (xmm2, xmm7);       // x1*t1-x7 = tm17


     movdqa (xmm3, sqrt2);
     movdqa (xmm7, xmm4 );
     movdqa (xmm6, xmm2 );
     psubsw (xmm4, xmm1 );      // tp17-tp35 = t1
     psubsw (xmm2, xmm0 );      // tm17-tm35 = b3
     paddsw (xmm1, xmm7 );      // tp17+tp35 = b0
     paddsw (xmm0, xmm6 );      // tm17+tm35 = t2

       // xmm1 = b0, xmm2 = b3. preserved

     movdqa (xmm6, xmm4 );
     psubsw (xmm4, xmm0 );      // t1-t2
     paddsw (xmm0, xmm6 );      // t1+t2

     pmulhw (xmm4, xmm3 );      // (t1-t2)/(2.sqrt2)
     pmulhw (xmm0, xmm3 );      // (t1+t2)/(2.sqrt2)

     paddsw (xmm0, xmm0 );      // 2.(t1+t2) = b1
     paddsw (xmm4, xmm4    );   // 2.(t1-t2) = b2

     movdqa (xmm7, tan2    ); // t2
     movdqa (xmm3, src+2*16);  // x2
     movdqa (xmm6, src+6*16);  // x6
     movdqa (xmm5, xmm7    );   // t2

     pmulhw (xmm7, xmm6    );   // x6*t2
     pmulhw (xmm5, xmm3    );   // x2*t2

     paddsw (xmm7, xmm3    );   // x2+x6*t2 = tp26
     psubsw (xmm5, xmm6    );   // x2*t2-x6 = tm26


      // use:xmm3,xmm5,xmm6,xmm7   frozen: xmm0,xmm4,xmm1,xmm2

     movdqa (xmm3, src+0*16); // x0
     movdqa (xmm6, src+4*16); // x4

       // we spill 1 reg to perform safe butterflies
     movdqa( src   , xmm2);

     movdqa( xmm2, xmm3  );
     psubsw( xmm3, xmm6  ); // x0-x4 = tm04
     paddsw( xmm6, xmm2  ); // x0+x4 = tp04

     movdqa( xmm2, xmm6  );
     psubsw( xmm6, xmm7  );
     paddsw( xmm7, xmm2  );
     movdqa( xmm2, xmm3  );
     psubsw( xmm3, xmm5  );
     paddsw( xmm5, xmm2  );

     movdqa( xmm2, xmm5  );
     psubsw( xmm5, xmm0  );
     paddsw( xmm0, xmm2  );
     movdqa( xmm2, xmm3  );
     psubsw( xmm3, xmm4  );
     paddsw( xmm4, xmm2  );

     movdqa( xmm2, src   );

     psraw ( xmm5, 6);      // out6
     psraw ( xmm3, 6);      // out5
     psraw ( xmm0, 6);      // out1
     psraw ( xmm4, 6);      // out2

     movdqa( src+6*16, xmm5);
     movdqa( src+5*16, xmm3);
     movdqa( src+1*16, xmm0);
     movdqa( src+2*16, xmm4);

       // reminder: xmm1=b0, xmm2=b3, xmm7=a0, xmm6=a3

     movdqa( xmm0, xmm7);
     movdqa( xmm4, xmm6);
     psubsw( xmm7, xmm1);   // a0-b0
     psubsw( xmm6, xmm2);   // a3-b3
     paddsw( xmm1, xmm0);   // a0+b0
     paddsw( xmm2, xmm4);   // a3+b3

     psraw ( xmm1, 6   );   // out0
     psraw ( xmm7, 6   );   // out7
     psraw ( xmm2, 6   );   // out3
     psraw ( xmm6, 6   );   // out4

       // combine result
     movdqa( src+0*16, xmm1);
     movdqa( src+3*16, xmm2);
     movdqa( src+4*16, xmm6);
     movdqa( src+7*16, xmm7);
    };
  };

 unsigned char *ecx=(unsigned char*)block;
 __m128i xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7;
 Tidct::iMTX_MULT(ecx,0, (const unsigned char*)iTab1, (const unsigned char*)Idct_Rnd0, 11,xmm0,xmm4,xmm5,xmm6,xmm7);
 Tidct::iMTX_MULT(ecx,1, (const unsigned char*)iTab2, (const unsigned char*)Idct_Rnd1, 11,xmm0,xmm4,xmm5,xmm6,xmm7);
 Tidct::iMTX_MULT(ecx,2, (const unsigned char*)iTab3, (const unsigned char*)Idct_Rnd2, 11,xmm0,xmm4,xmm5,xmm6,xmm7);
 Tidct::iMTX_MULT(ecx,3, (const unsigned char*)iTab4, (const unsigned char*)Idct_Rnd3, 11,xmm0,xmm4,xmm5,xmm6,xmm7);
 Tidct::iMTX_MULT(ecx,4, (const unsigned char*)iTab1, (const unsigned char*)Idct_Rnd4, 11,xmm0,xmm4,xmm5,xmm6,xmm7);
 Tidct::iMTX_MULT(ecx,5, (const unsigned char*)iTab4, (const unsigned char*)Idct_Rnd5, 11,xmm0,xmm4,xmm5,xmm6,xmm7);
 Tidct::iMTX_MULT(ecx,6, (const unsigned char*)iTab3, (const unsigned char*)Idct_Rnd6, 11,xmm0,xmm4,xmm5,xmm6,xmm7);
 Tidct::iMTX_MULT(ecx,7, (const unsigned char*)iTab2, (const unsigned char*)Idct_Rnd7, 11,xmm0,xmm4,xmm5,xmm6,xmm7);
 Tidct::iLLM_PASS(ecx,xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7);
}
#endif
