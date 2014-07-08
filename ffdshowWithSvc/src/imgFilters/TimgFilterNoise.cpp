/*
 * Copyright (c) 2002-2006 Milan Cutka
 * noise algorithm 1 by avih
 * noise algorithm 2 by Michael Niedermayer and ...
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
#include "TimgFilterNoise.h"
#include "Tconfig.h"
#include "TimgFilterLuma.h"
#include "TimgFilterOffset.h"

#pragma warning(push)
#pragma warning(disable:4799)

//================================= TimgFilterNoiseMplayer::Tprocess ===============================
TimgFilterNoiseMplayer::Tprocess::Tprocess(void)
{
 noise=NULL;
 nonTempRandShift[0]=-1;
 lineNoise=lineNoise_C;
 lineNoiseAvg=lineNoiseAvg_C;
 if (Tconfig::cpu_flags&FF_CPU_MMX)
  {
   lineNoise=lineNoise_simd<Tmmx>;
   lineNoiseAvg=lineNoiseAvg_simd<Tmmx>;
  }
 if (Tconfig::cpu_flags&FF_CPU_MMXEXT)
  lineNoise=lineNoise_simd<Tmmxext>;
#ifdef __SSE2__
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  {
   lineNoise=lineNoise_simd<Tsse2>;
   lineNoiseAvg=lineNoiseAvg_simd<Tsse2>;
  }
#endif
}

void TimgFilterNoiseMplayer::Tprocess::done(void)
{
 if (noise) aligned_free(noise);noise=NULL;
 nonTempRandShift[0]=-1;
}

void TimgFilterNoiseMplayer::Tprocess::init(int strength,const TnoiseSettings *Icfg)
{
 cfg=*Icfg;
 noise=(int8_t*)aligned_malloc(MAX_NOISE*sizeof(int8_t));
 int i,j;
 for (i=0,j=0;i<MAX_NOISE;i++,j++)
  {
   if(cfg.uniform)
    if (cfg.averaged)
     if (cfg.pattern)
      noise[i]=int8_t((RAND_N(strength)-strength/2)/6+TimgFilterNoise::patt[j%4]*strength*0.25/3);
     else
      noise[i]=int8_t((RAND_N(strength)-strength/2)/3);
    else
     if (cfg.pattern)
      noise[i]=int8_t((RAND_N(strength)-strength/2)/2+TimgFilterNoise::patt[j%4]*strength*0.25);
     else
      noise[i]=int8_t(RAND_N(strength)-strength/2);
   else
    {
     double x1, x2, w, y1;
     do
      {
       x1=2.0*rand()/(float)RAND_MAX-1.0;
       x2=2.0*rand()/(float)RAND_MAX-1.0;
       w=x1*x1+x2*x2;
      } while (w>=1.0);

     w=sqrt((-2.0*log(w))/w);
     y1=x1*w;
     y1*=strength/sqrt(3.0);
     if (cfg.pattern)
      {
       y1/=2;
       y1+=TimgFilterNoise::patt[j%4]*strength*0.35;
      }
     if      (y1<-128) y1=-128;
     else if (y1> 127) y1= 127;
     if (cfg.averaged) y1/=3.0;
     noise[i]=(int8_t)y1;
    }
   if (RAND_N(6)==0) j--;
  }

 for (i=0;i<MAX_RES;i++)
  for (j=0;j<3;j++)
   prev_shift[i][j]=noise+(rand()&(MAX_SHIFT-1));

 if (nonTempRandShift[0]==-1)
  for (i=0;i<MAX_RES;i++)
   nonTempRandShift[i]=rand()&(MAX_SHIFT-1);

 shiftptr=0;
}

void TimgFilterNoiseMplayer::Tprocess::process(const uint8_t *src,uint8_t *dst,stride_t dstStride,stride_t srcStride,unsigned int width,unsigned int height)
{
 int shift=0;
 for (unsigned int y=0;y<height;y++)
  {
   if (cfg.mplayerTemporal)
    shift=rand()&(MAX_SHIFT-1);
   else
    shift=nonTempRandShift[y];

   if (cfg.mplayerQuality==0)
    shift&=~7;

   if (cfg.averaged)
    {
     lineNoiseAvg(dst,src,width,prev_shift[y]);
     prev_shift[y][shiftptr]=noise+shift;
    }
   else
    lineNoise(dst,src,noise,width,shift);
   dst+=dstStride;
   src+=srcStride;
  }
 shiftptr++;
 if (shiftptr==3) shiftptr=0;
}

void TimgFilterNoiseMplayer::Tprocess::lineNoise_C(uint8_t *dst,const uint8_t *src, int8_t *noise, int len, int shift)
{
 noise+=shift;
 for (int i=0;i<len;i++)
  {
   int v=src[i]+noise[i];
   if (v>255)   dst[i]=255;
   else if(v<0) dst[i]=0;
   else         dst[i]=uint8_t(v);
  }
}

void TimgFilterNoiseMplayer::Tprocess::lineNoiseAvg_C(uint8_t *dst,const uint8_t *src, int len, int8_t **shift)
{
 const int8_t *src2=(const int8_t*)src;
 for (int i=0;i<len;i++)
  {
   int n=shift[0][i]+shift[1][i]+shift[2][i];
   dst[i]=uint8_t(src2[i]+((n*src2[i])>>7));
  }
}

template<class _mm> void TimgFilterNoiseMplayer::Tprocess::lineNoise_simd(uint8_t *dst,const uint8_t *src, int8_t *noise, int len, int shift)
{
 if (_mm::align && (intptr_t(src)&15 || intptr_t(dst)&15))
  {
   lineNoise_simd<typename _mm::T64>(dst,src,noise,len,shift);
   return;
  }
 int mmx_len=len&(~(_mm::size-1));
 noise+=shift;

 typename _mm::__m mm7;
 pcmpeqb(mm7,mm7);
 psllw(mm7,15);
 packsswb(mm7,mm7);
 for (int x=-mmx_len;x<0;x+=_mm::size)
  {
   //".balign 16			\n\t"
   typename _mm::__m mm0,mm1;
   movq   (mm0,src+mmx_len+x);
   movdqu (mm1,noise+mmx_len+x); //SSE3
   pxor   (mm0,mm7);
   paddsb (mm0,mm1);
   pxor   (mm0,mm7);
   _mm::movntq(dst+mmx_len+x,mm0);
  }

 if (mmx_len!=len)
  lineNoise_C(dst+mmx_len,src+mmx_len,noise+mmx_len,len-mmx_len,0);
}
template<class _mm> void TimgFilterNoiseMplayer::Tprocess::lineNoiseAvg_simd(uint8_t *dst,const uint8_t *src, int len, int8_t **shift_)
{
 if (_mm::align && (intptr_t(src)&15 || intptr_t(dst)&15))
  {
   lineNoiseAvg_simd<typename _mm::T64>(dst,src,len,shift_);
   return;
  }
 const int mmx_len=len&(~(_mm::size-1));

 int8_t *shift2[3]={shift_[0]+mmx_len, shift_[1]+mmx_len, shift_[2]+mmx_len};
 for (int x=-mmx_len;x<0;x+=_mm::size)
  {
   //".balign 16                   \n\t"
   typename _mm::__m mm0,mm1,mm2,mm3;
   movdqu (mm1,shift2[0]+mmx_len+x);
   movq (mm0,src+mmx_len+x);
   typename _mm::__m shift1_8=_mm::loadU(shift2[1]+mmx_len+x);
   paddb (mm1,shift1_8);
   typename _mm::__m shift2_8=_mm::loadU(shift2[2]+mmx_len+x);
   paddb (mm1,shift2_8);
   movq (mm2,mm0);
   movq (mm3,mm1);
   punpcklbw (mm0,mm0);
   punpckhbw (mm2,mm2);
   punpcklbw (mm1,mm1);
   punpckhbw (mm3,mm3);
   pmulhw (mm1,mm0);
   pmulhw (mm3,mm2);
   paddw (mm1,mm1);
   paddw (mm3,mm3);
   paddw (mm1,mm0);
   paddw (mm3,mm2);
   psrlw (mm1,8);
   psrlw (mm3,8);
   packuswb (mm1,mm3);
   movq (dst+mmx_len+x,mm1);
  }

 if (mmx_len!=len)
  lineNoiseAvg_C(dst+mmx_len, src+mmx_len, len-mmx_len, shift2);
}

TimgFilterNoiseMplayer::TimgFilterNoiseMplayer(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 oldnoise.strength=-1;oldcsp=FF_CSP_NULL;
}
void TimgFilterNoiseMplayer::done(void)
{
 y.done();
 doneChroma();
}
void TimgFilterNoiseMplayer::doneChroma(void)
{
 uv.done();
}
bool TimgFilterNoiseMplayer::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TnoiseSettings *cfg=(const TnoiseSettings*)cfg0;
 return super::is(pict,cfg0) && (cfg->strength || cfg->strengthChroma);
}
HRESULT TimgFilterNoiseMplayer::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TnoiseSettings *cfg=(const TnoiseSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   if (!cfg->equal(oldnoise))
    {
     oldnoise=*cfg;
     y.done();uv.done();
    }
   int csp=0;
   if (cfg->strength)
    {
     const unsigned char *srcY;
     getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&srcY,NULL,NULL,NULL);
     unsigned char *dstY;
     getCurNext(csp=csp1,pict,cfg->full,COPYMODE_NO,&dstY,NULL,NULL,NULL);
     if (!y)
      y.init(cfg->strength/(cfg->averaged?1:3),cfg);
     y.process(srcY,dstY,stride2[0],stride1[0],dx1[0],dy1[0]);
     _mm_empty();
    }
   if (cfg->strengthChroma)
    {
     const unsigned char *srcU,*srcV;
     getCur(csp?csp:FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,NULL,&srcU,&srcV,NULL);
     unsigned char *dstU,*dstV;
     getCurNext(csp1,pict,cfg->full,COPYMODE_NO,NULL,&dstU,&dstV,NULL);
     if (oldcsp!=csp1)
      {
       oldcsp=csp1;
       doneChroma();
      }
     if (!uv)
      uv.init(cfg->strengthChroma/(cfg->averaged?1:2),cfg);
     uv.process(srcU,dstU,stride2[1],stride1[1],dx1[1],dy1[1]);
     uv.process(srcV,dstV,stride2[2],stride1[2],dx1[2],dy1[2]);
     _mm_empty();
    }
  }
 return parent->deliverSample(++it,pict);
}

//========================================= TimgFilterNoise =========================================
__m64 TimgFilterNoise::noiseConst64,TimgFilterNoise::noisenext64;
template<> __m64& TimgFilterNoise::Tnoise<Tmmx>::noiseConst() {return noiseConst64;}
template<> __m64& TimgFilterNoise::Tnoise<Tmmx>::noisenext() {return noisenext64;}
template<> void TimgFilterNoise::Tnoise<Tmmx>::init(void)
{
 noiseConst64=Tmmx::set1_pi64(6364136223846793005LL);
}
template<> void TimgFilterNoise::Tnoise<Tmmx>::reset(void)
{
 noisenext64=Tmmx::set1_pi64(time(NULL));
}

#ifdef __SSE2__
__m128i TimgFilterNoise::noiseConst128,TimgFilterNoise::noisenext128;
template<> __m128i& TimgFilterNoise::Tnoise<Tsse2>::noiseConst() {return noiseConst128;}
template<> __m128i& TimgFilterNoise::Tnoise<Tsse2>::noisenext() {return noisenext128;}
template<> void TimgFilterNoise::Tnoise<Tsse2>::init(void)
{
 noiseConst128=_mm_set_epi32(0xaa294349,0x6f4805c,0x987ec766,0xcf68ec93);
}
template<> void TimgFilterNoise::Tnoise<Tsse2>::reset(void)
{
 __align16(int64_t,t[])={0,time(NULL)};
 noisenext128=*(__m128i*)t;
}
#endif

const int TimgFilterNoise::patt[4]={-2,0,2,0};

TimgFilterNoise::TimgFilterNoise(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 noiseMask[0]=noiseMask[1]=noiseMask[2]=NULL;
 oldnoise.strength=-1;oldcsp=FF_CSP_NULL;
#ifdef __SSE2__
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  {
   Tnoise<Tsse2>::init();
   processLumaFc=&TimgFilterNoise::processLuma<Tsse2>;
   processChromaFc=&TimgFilterNoise::processChroma<Tsse2>;
  }
 else
#endif
  {
   processLumaFc=&TimgFilterNoise::processLuma<Tmmx>;
   processChromaFc=&TimgFilterNoise::processChroma<Tmmx>;
  }
 Tnoise<Tmmx>::init();
 _mm_empty();
}
void TimgFilterNoise::done(void)
{
 if (noiseMask[0]) aligned_free(noiseMask[0]);noiseMask[0]=NULL;
 doneChroma();
}
void TimgFilterNoise::doneChroma(void)
{
 for (int i=1;i<3;i++)
  if (noiseMask[i])
   {
    aligned_free(noiseMask[i]);
    noiseMask[i]=NULL;
   }
}
void TimgFilterNoise::onSizeChange(void)
{
 done();
 noiseCount[0]=noiseCount[1]=noiseCount[2]=-1;
 noiseAvihStrength=0;oldPattern=-1;
#ifdef __SSE2__
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  Tnoise<Tsse2>::reset();
#endif
 Tnoise<Tmmx>::reset();
 _mm_empty();
}

template<class _mm,bool chroma,bool avih,bool uniform> void TimgFilterNoise::addNoise(const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,unsigned int dx,unsigned int dy,short *noiseMask,stride_t noiseMaskStride,int noiseStrength)
{
 typename _mm::__m noisenext64;
 if (!avih)
  noisenext64=Tnoise<_mm>::noisenext();
 typename _mm::__m noiseStrength64=_mm::set1_pi16((short)noiseStrength);
 typename _mm::__m m0=_mm::setzero_si64(),m255=_mm::set1_pi16(255),m128=_mm::set1_pi16(128);
 for (const unsigned char *srcEnd=src+srcStride*dy;src!=srcEnd;src+=srcStride,dst+=dstStride,noiseMask+=noiseMaskStride)
  {
   int x=0;
   for (;x<int(dx-_mm::size/2+1);x+=_mm::size/2)
    {
     typename _mm::__m noise;
     if (!avih)
      {
       noise=noisenext64=_mm::add_pi16(_mm::madd_pi16(noisenext64,Tnoise<_mm>::noiseConst()),Tnoise<_mm>::noiseConst());
       noise=_mm::subs_pi16(_mm::and_si64(noise,m255),m128);
      }
     else
      noise=_mm::loadU(noiseMask+x); //SSE3
     typename _mm::__m noiseMul,src8=_mm::unpacklo_pi8(_mm::load2(src+x),m0);
     if (uniform)
      noiseMul=noise;
     else
      {
       noiseMul=src8;
       if (chroma)
        {
         noiseMul=_mm::subs_pi16(noiseMul,m128);
         noiseMul=_mm::slli_pi16(noiseMul,3);
        }
       noiseMul=_mm::srai_pi16(_mm::mullo_pi16(noiseMul,noise),8);
      }
     if (!avih)
      _mm::storeU(noiseMask+x,noiseMul=_mm::srai_pi16(_mm::mullo_pi16(noiseMul,noiseStrength64),8));
     _mm::store2(dst+x,_mm::packs_pu16(_mm::add_pi16(noiseMul,src8),m0));
    }
   for (;x<(int)dx;x++)
    {
     int mm1;
     if (!avih)
      {
       typename _mm::integer2_t val;
       _mm::store2(&val,noisenext64=_mm::add_pi16(_mm::madd_pi16(noisenext64,Tnoise<_mm>::noiseConst()),Tnoise<_mm>::noiseConst()));
       mm1=(char)val;
      }
     else
      mm1=(char)noiseMask[x];
     int mm0,mm7=src[x];
     if (uniform)
      mm0=mm1;
     else
      {
       mm0=mm7;
       if (chroma)
        mm0-=128;
       mm0=(mm0*mm1)>>8;
      }
     if (!avih)
      noiseMask[x]=(short)(mm0=(mm0*noiseStrength)>>8);
     dst[x]=limit_uint8(mm0+mm7);
    }
  }
 if (!avih)
  Tnoise<_mm>::noisenext()=noisenext64;
}
template<class _mm,bool chroma,bool avih> void TimgFilterNoise::generateAddNoise(const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,unsigned int dx,unsigned int dy,int noiseStrength,int uniformNoise,short *noiseMask,stride_t noiseMaskStride,int noiseCount)
{
 if (_mm::align && (intptr_t(src)&15 || intptr_t(dst)&15 || srcStride&15 || dstStride&15))
  {
   generateAddNoise<Tmmx,chroma,avih>(src,srcStride,dst,dstStride,dx,dy,noiseStrength,uniformNoise,noiseMask,noiseMaskStride,noiseCount);
   return;
  }
 typename _mm::__m m0=_mm::setzero_si64();
 if (!avih && noiseCount&1)
  {
   for (const unsigned char *srcEnd=src+srcStride*dy;src!=srcEnd;src+=srcStride,dst+=dstStride,noiseMask+=noiseMaskStride)
    {
     int x=0;
     for (;x<int(dx-_mm::size/2+1);x+=_mm::size/2)
      _mm::store2(dst+x,_mm::packs_pu16(_mm::add_pi16(_mm::unpacklo_pi8(_mm::load2(src+x),m0),*(typename _mm::__m*)(noiseMask+x)),m0));
     for (;x<(int)dx;x++)
      dst[x]=limit_uint8(src[x]+noiseMask[x]);
    }
   return;
  }
 if (!uniformNoise)
  addNoise<_mm,chroma,avih,false>(src,srcStride,dst,dstStride,dx,dy,noiseMask,noiseMaskStride,noiseStrength);
 else
  addNoise<_mm,chroma,avih,true >(src,srcStride,dst,dstStride,dx,dy,noiseMask,noiseMaskStride,noiseStrength);
}

void TimgFilterNoise::genAVIHnoise(int plane,int strength)
{
 short *dst1=noiseMask[plane],*dst2=dst1+noiseMaskStride[plane]*dy1[plane];
 for (unsigned int i=0,j=0;i<noiseMaskStride[plane]*dy1[plane];i++,j++)
  {
   dst1[i]=dst2[i]=short((((rand()&255)-128)*strength)/256+(oldPattern?patt[j%4]*strength/12:0));
   if (!(rand()%6)) j--;
  }
}
template<class _mm> void TimgFilterNoise::processLuma(const TnoiseSettings *cfg,const unsigned char *srcY,unsigned char *dstY)
{
 switch (cfg->method)
  {
   case TnoiseSettings::NOISE_FF:
    {
     noiseCount[0]++;
     if (noiseAvihStrength) noiseCount[0]=0;
     generateAddNoise<_mm,false,false>(srcY,stride1[0],dstY,stride2[0],dx1[0],dy1[0],cfg->strength,cfg->uniform,noiseMask[0],noiseMaskStride[0],noiseCount[0]);
     noiseAvihStrength=0;
     break;
    }
   case TnoiseSettings::NOISE_AVIH:
    {
     if (cfg->strength!=noiseAvihStrength)
      genAVIHnoise(0,noiseAvihStrength=cfg->strength);
     generateAddNoise<_mm,false,true>(srcY,stride1[0],dstY,stride2[0],dx1[0],dy1[0],0,cfg->uniform,noiseMask[0]+((rand()%(dx1[0]*dy1[0]))&~15),noiseMaskStride[0],0);
     break;
    }
  }
 _mm::empty();
}
template<class _mm> void TimgFilterNoise::processChroma(const TnoiseSettings *cfg,const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstU,unsigned char *dstV)
{
 switch (cfg->method)
  {
   case TnoiseSettings::NOISE_FF:
    {
     noiseCount[1]++;noiseCount[2]++;
     if (noiseAvihStrengthChroma) noiseCount[1]=noiseCount[2]=0;
     generateAddNoise<_mm,true,false>(srcU,stride1[1],dstU,stride2[1],dx1[1],dy1[1],cfg->strengthChroma,cfg->uniform,noiseMask[1],noiseMaskStride[1],noiseCount[1]);
     generateAddNoise<_mm,true,false>(srcV,stride1[2],dstV,stride2[2],dx1[2],dy1[2],cfg->strengthChroma,cfg->uniform,noiseMask[2],noiseMaskStride[2],noiseCount[2]);
     noiseAvihStrengthChroma=0;
     break;
    }
   case TnoiseSettings::NOISE_AVIH:
    {
     if (cfg->strengthChroma!=noiseAvihStrengthChroma)
      {
       noiseAvihStrengthChroma=cfg->strengthChroma;
       genAVIHnoise(1,noiseAvihStrengthChroma);
       genAVIHnoise(2,noiseAvihStrengthChroma);
      }
     generateAddNoise<_mm,true,true>(srcU,stride1[1],dstU,stride2[1],dx1[1],dy1[1],0,cfg->uniform,noiseMask[1]+rand()%(noiseMaskStride[1]*dy1[1]),noiseMaskStride[1],0);
     generateAddNoise<_mm,true,true>(srcV,stride1[2],dstV,stride2[2],dx1[2],dy1[2],0,cfg->uniform,noiseMask[2]+rand()%(noiseMaskStride[2]*dy1[2]),noiseMaskStride[2],0);
     break;
    }
  }
 _mm::empty();
}

bool TimgFilterNoise::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TnoiseSettings *cfg=(const TnoiseSettings*)cfg0;
 return super::is(pict,cfg) && (cfg->strength || cfg->strengthChroma);
}

HRESULT TimgFilterNoise::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TnoiseSettings *cfg=(const TnoiseSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   if (oldPattern!=cfg->pattern)
    {
     oldPattern=cfg->pattern;
     noiseAvihStrength=noiseAvihStrengthChroma=-1;
    }
   int csp=0;
   if (cfg->strength)
    {
     const unsigned char *srcY;
     getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&srcY,NULL,NULL,NULL);
     unsigned char *dstY;
     getCurNext(csp=csp1,pict,cfg->full,COPYMODE_NO,&dstY,NULL,NULL,NULL);
     if (!noiseMask[0])
      {
       noiseMaskStride[0]=(dx1[0]/16+2)*16;
       noiseMask[0]=(short*)aligned_calloc(noiseMaskStride[0]*dy1[0]*2,2);
      }
     (this->*processLumaFc)(cfg,srcY,dstY);
    }
   if (cfg->strengthChroma)
    {
     const unsigned char *srcU,*srcV;
     getCur(csp?csp:FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,NULL,&srcU,&srcV,NULL);
     unsigned char *dstU,*dstV;
     getCurNext(csp1,pict,cfg->full,COPYMODE_NO,NULL,&dstU,&dstV,NULL);
     if (oldcsp!=csp1)
      {
       oldcsp=csp1;
       doneChroma();
      }
     if (!noiseMask[1])
      {
       noiseMaskStride[1]=(dx1[1]/16+2)*16;
       noiseMask[1]=(short*)aligned_calloc(noiseMaskStride[1]*dy1[2]*2,2);
      }
     if (!noiseMask[2])
      {
       noiseMaskStride[2]=(dx1[2]/16+2)*16;
       noiseMask[2]=(short*)aligned_calloc(noiseMaskStride[2]*dy1[2]*2,2);
      }
     (this->*processChromaFc)(cfg,srcU,srcV,dstU,dstV);
    }
  }
 _mm_empty();
 return parent->deliverSample(++it,pict);
}
#pragma warning(pop)

//======================================== TimgFilterFlicker ========================================
TimgFilterFlicker::TimgFilterFlicker(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 luma=NULL;lumaSettings.lumGain=lumaSettings.lumGainDef;lumaSettings.is=lumaSettings.show=1;lumaSettings.gammaCorrection=lumaSettings.gammaCorrectionDef;lumaT=0;
}
TimgFilterFlicker::~TimgFilterFlicker()
{
 if (luma) delete luma;
}
void TimgFilterFlicker::done(void)
{
 if (luma) luma->done();
}
bool TimgFilterFlicker::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TnoiseSettings *cfg=(const TnoiseSettings*)cfg0;
 return super::is(pict,cfg0) && cfg->flickerA;
}
HRESULT TimgFilterFlicker::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TnoiseSettings *cfg=(const TnoiseSettings*)cfg0;
   lumaSettings.full=cfg->full;lumaSettings.half=cfg->half;
   lumaSettings.lumOffset=int(cfg->flickerA*sin(lumaT/(2*M_PI)));
   lumaT+=rand()%cfg->flickerF;
   if (!luma) luma=new TimgFilterLuma(deci,parent);
   return luma->process(it,pict,&lumaSettings);
  }
 else
  return parent->deliverSample(++it,pict);
}

//========================================= TimgFilterShake =========================================
TimgFilterShake::TimgFilterShake(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 offset=NULL;offsetSettings.is=offsetSettings.show=1;offsetTX=offsetTY=0;
}
TimgFilterShake::~TimgFilterShake()
{
 if (offset) delete offset;
}
void TimgFilterShake::done(void)
{
 if (offset) offset->done();
}
bool TimgFilterShake::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TnoiseSettings *cfg=(const TnoiseSettings*)cfg0;
 return super::is(pict,cfg0) && cfg->shakeA;
}
HRESULT TimgFilterShake::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TnoiseSettings *cfg=(const TnoiseSettings*)cfg0;
   offsetSettings.full=cfg->full;offsetSettings.half=cfg->half;
   int offsetX=int((cfg->shakeA+1)*sin(offsetTX/(2*M_PI)));
   int offsetY=int((cfg->shakeA+1)*sin(offsetTY/(2*M_PI)));
   offsetSettings.Y_X=offsetSettings.U_X=offsetSettings.V_X=offsetX;
   offsetSettings.Y_Y=offsetSettings.U_Y=offsetSettings.V_Y=offsetY;
   if ((rand()%cfg->shakeF)>cfg->shakeF/2)
    {
     offsetTX+=(rand()%cfg->shakeF)-cfg->shakeF/2;
     offsetTY+=(rand()%cfg->shakeF)-cfg->shakeF/2;
    }
   else
    offsetTX=offsetTY=0;
   if (!offset) offset=new TimgFilterOffset(deci,parent);
   return offset->process(it,pict,&offsetSettings);
  }
 else
  return parent->deliverSample(++it,pict);
}

//======================================= TimgFilterFilmLines =======================================
TimgFilterFilmLines::TimgFilterFilmLines(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 linesT=0;
}
void TimgFilterFilmLines::done(void)
{
 lines.clear();
}
bool TimgFilterFilmLines::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TnoiseSettings *cfg=(const TnoiseSettings*)cfg0;
 return super::is(pict,cfg0) && cfg->linesF;
}
HRESULT TimgFilterFilmLines::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TnoiseSettings *cfg=(const TnoiseSettings*)cfg0;
   int linesF=101-cfg->linesF;
   if (rand()%linesF>linesF-linesT)
    {
     lines.push_back(Tline(cfg));
     linesT=0;
    }
   else
    linesT++;
   if (!lines.empty())
    {
     init(pict,cfg->full,cfg->half);
     unsigned char *dst;
     getCurNext(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,COPYMODE_DEF,&dst,NULL,NULL,NULL);
     int maxtransp=std::min(3*cfg->linesTransparency/2,250);
     for (Tlines::iterator l=lines.begin();l!=lines.end();)
      {
       if (l->firsttime)
        {
         l->setDx(dx2[0]);
         l->firsttime=false;
        }
       unsigned char *p=dst;
       int x=l->x>>16;
       for (unsigned int y=0;y<dy2[0];y++,p+=stride2[0])
        {
         p[x]=uint8_t(((255-l->strength)*p[x]+l->strength*l->color)/256);
         l->strength=limit(l->strength+rand()%5-2,0,maxtransp);
        }
       if (l->update()) l=lines.erase(l);else l++;
      }
    }
  }
 return parent->deliverSample(++it,pict);
}
void TimgFilterFilmLines::onSeek(void)
{
 done();
}
void TimgFilterFilmLines::onSizeChange(void)
{
 done();
}

TimgFilterFilmLines::Tline::Tline(const TnoiseSettings *cfg):firsttime(true)
{
 life=rand()%cfg->linesA+1;
 if (rand()%256>cfg->linesC)
  color=(unsigned char)(20+rand()%30);
 else
  color=(unsigned char)(210-rand()%50);
 strength=rand()%cfg->linesTransparency;
}
void TimgFilterFilmLines::Tline::setDx(unsigned int Idx)
{
 dx=Idx;
 x=(rand()%dx)<<16;
 diffx=rand()%4096-2048;
}
bool TimgFilterFilmLines::Tline::update(void)
{
 if (!--life) return true;
 x+=diffx;
 if (!isIn(x>>16,0,int(dx-1))) return true;
 return false;
}

//==================================== TimgFilterFilmSchratches =====================================
TimgFilterScratches::TimgFilterScratches(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 scratchesT=0;
}
bool TimgFilterScratches::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TnoiseSettings *cfg=(const TnoiseSettings*)cfg0;
 return super::is(pict,cfg0) && cfg->scratchesF;
}
HRESULT TimgFilterScratches::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TnoiseSettings *cfg=(const TnoiseSettings*)cfg0;
   int scratchesF=101-cfg->scratchesF;
   if (rand()%scratchesF>scratchesF-scratchesT-2)
    {
     unsigned char *dst=NULL;
     init(pict,cfg->full,cfg->half);
     getCurNext(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,COPYMODE_DEF,&dst,NULL,NULL,NULL);
     unsigned int numscratches=1+((rand()%100)/scratchesF)/30;
     int maxtransp=std::min(3*cfg->scratchesTransparency/2,250);
     for (unsigned int s=0;s<numscratches;s++)
      {
       int sx=(rand()%(dx2[0]-10)+5)<<16,sy=(rand()%(dy2[0]-10)+5)<<16;
       unsigned int length=rand()%30+2*cfg->scratchesA;
       unsigned char color;
       if (rand()%256>cfg->scratchesC)
        color=(unsigned char)(10+rand()%30);
       else
        color=(unsigned char)(210-rand()%30);
       int strength=rand()%20;
       int rx=rand()%400,ry=rand()%400;
       for (unsigned int i=1;i<length;i++)
        {
         unsigned char *p=dst+(sx>>16)+(sy>>16)*stride2[0];
         *p=uint8_t(((255-strength)**p+strength*color)/256);
         sx+=int((1<<15)*sin(rx+rand()%(length-i)/(M_PI*2)));
         sy+=int((1<<15)*cos(ry+rand()%(length-i)/(M_PI*2)));
         if (!isIn(sx,0,int(dx2[0]-1)<<16) || !isIn(sy,0,int(dy2[0]-1)<<16))
          break;
         strength=limit(strength+rand()%16-7,0,maxtransp);
        }
      }
     scratchesT=0;
    }
   else
    scratchesT++;
  }
 return parent->deliverSample(++it,pict);
}
