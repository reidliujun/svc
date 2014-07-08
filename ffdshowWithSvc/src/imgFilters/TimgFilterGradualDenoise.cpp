/*
 * Copyright (c) 2002-2006 Milan Cutka
 * algorithm and implementation by Lindsey Dubb
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
#include "TimgFilterGradualDenoise.h"
#include "TblurSettings.h"
#include "Tconfig.h"
#include "simd.h"

template<class _mm> void TimgFilterGradualDenoise::denoise(int gradualStrength,unsigned int dx1,unsigned int dy1,const unsigned char *srcY,stride_t stride1,unsigned char *dstY,stride_t stride2)
{
 typedef typename _mm::__m __m;
 if (_mm::align && (intptr_t(srcY)&15 || stride1&15 || intptr_t(dstY)&15 || stride2&15))
  {
   denoise<typename _mm::T64>(gradualStrength,dx1,dy1,srcY,stride1,dstY,stride2);
   return;
  }
 const unsigned char *srcY0=srcY;
 unsigned char *dstY0=dstY;
 unsigned int cycles=dx1&~(_mm::size-1);
 __int64 noiseMultiplier64=(0x10000+(gradualStrength/2))/gradualStrength;
 __align16(__int64,noiseMultiplier_2[2])={noiseMultiplier64,noiseMultiplier64};
 __m noiseMultiplier=*(__m*)noiseMultiplier_2;
 __m m0=_mm::setzero_si64(),mOnes=_mm::set1_pi8(1),mChromaMask=_mm::set_pi8(-1,0,-1,0,-1,0,-1,0);
 unsigned char *prevY=prevImg;
 static const int PREFETCH_STRIDE=128;
 for (const unsigned char *srcYend=srcY+dy1*stride1;srcY!=srcYend;srcY+=stride1,dstY+=stride2,prevY+=prevStride)
  for (unsigned int x=0;x<cycles;x+=_mm::size)
   {
    __m mm0=*(__m*)(srcY+x);
    __m mm1=*(__m*)(prevY+x);
    __m mm2=mm0;
    _mm::psadbw(mm2,mm1);// _mm::sad_pu8(mm0,mm1);
    mm2=_mm::madd_pi16(mm2,noiseMultiplier);
    _mm::prefetchnta(srcY+x+PREFETCH_STRIDE);

    __m mm3=mm2;
    mm2=_mm::cmpgt_pi16(mm2,m0);
    mm2=_mm::srli_si64(mm2,16);
    mm3=_mm::or_si64(mm3,mm2);
    mm2=_mm::shuffle_pi16_0(mm3); //mm2=_mm::shuffle_pi16(mm3,0);

    __m mm4;
    mm4=_mm::subs_pu8(mm1,mm0);
    mm0=_mm::subs_pu8(mm0,mm1);
    mm4=_mm::or_si64(mm4,mm0);
    mm0=_mm::cmpeq_pi8(mm0,m0);
    __m mm5;
    mm5=_mm::andnot_si64(_mm::cmpeq_pi8(mm4,m0),mOnes);

    __m mm6;
    mm6=_mm::cmpeq_pi8(m0,m0);
    mm6=_mm::cmpeq_pi16(mm6,mm2);
    mm6=_mm::and_si64(mm6,mm4);
    _mm::pmaxub(mm6,mm5);// mm6=_mm::max_pu8(mm6,mm5);

    mm3=mChromaMask;
    __m mm7=_mm::and_si64(mm4,mm3);
    _mm::pmulhuw(mm7,mm2);// mm7=_mm::mulhi_pu16(mm7,mm2);
    mm7=_mm::and_si64(mm7,mm3);

    mm3=_mm::andnot_si64(mm3,mm4);
    _mm::pmulhuw(mm3,mm2);//mm3=_mm::mulhi_pu16(mm3,mm2);
    mm7=_mm::or_si64(mm7,mm3);
    _mm::pmaxub(mm7,mm6);// mm7=_mm::max_pu8(mm7,mm6);

    mm3=mm7;
    mm7=_mm::and_si64(mm7,mm0);
    mm1=_mm::subs_pu8(mm1,mm7);
    mm0=_mm::andnot_si64(mm0,mm3);
    mm1=_mm::adds_pu8(mm1,mm0);
    *(__m*)(prevY+x)=*(__m*)(dstY+x)=mm1; //this causes ICE on GCC 3.4.2
   }
 _mm::empty();
 if (dx1&(_mm::size-1))
  TffPict::copy(dstY0+cycles,stride2,srcY0+cycles,stride1,dx1&(_mm::size-1),dy1);
}

TimgFilterGradualDenoise::TimgFilterGradualDenoise(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 prevImg=NULL;
#ifdef __SSE2__
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  denoiseFc=&TimgFilterGradualDenoise::denoise<Tsse2>;
 else
#endif
 if (Tconfig::cpu_flags&FF_CPU_MMXEXT)
  denoiseFc=&TimgFilterGradualDenoise::denoise<Tmmxext>;
 else
  denoiseFc=&TimgFilterGradualDenoise::denoise<Tmmx>;
}
void TimgFilterGradualDenoise::done(void)
{
 if (prevImg) aligned_free(prevImg);prevImg=NULL;
}
void TimgFilterGradualDenoise::onSizeChange(void)
{
 done();
}

bool TimgFilterGradualDenoise::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TblurSettings *cfg=(const TblurSettings*)cfg0;
 return super::is(pict,cfg) && cfg->isGradual && cfg->gradualStrength;
}

HRESULT TimgFilterGradualDenoise::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TblurSettings *cfg=(const TblurSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   const unsigned char *srcY;
   getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&srcY,NULL,NULL,NULL);
   unsigned char *dstY;
   getCurNext(csp1,pict,cfg->full,COPYMODE_NO,&dstY,NULL,NULL,NULL);

   if (!prevImg)
    prevImg=(unsigned char*)aligned_calloc(prevStride=(dx1[0]/16+2)*16,dy1[0]);

   (this->*denoiseFc)(cfg->gradualStrength,dx1[0],dy1[0],srcY,stride1[0],dstY,stride2[0]);
  }
 return parent->deliverSample(++it,pict);
}

void TimgFilterGradualDenoise::onSeek(void)
{
 done();
}
