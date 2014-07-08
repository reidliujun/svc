/*
 * Copyright (c) 2003-2006 Milan Cutka
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
#include "TaudioFilter.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"
#include "dither.h"
#include "TfilterSettings.h"
#include "Tconfig.h"
#include "TpresetSettingsAudio.h"
#include "simd.h"

TaudioFilter::TaudioFilter(IffdshowBase *Ideci,Tfilters *Iparent):Tfilter(Ideci),parent((TaudioFilters*)Iparent)
{
 deciA=deci;
 dither=NULL;oldnoiseshaping=oldsfout=-1;
}
TaudioFilter::~TaudioFilter()
{
 if (dither) delete dither;
}

void* TaudioFilter::alloc_buffer(const TsampleFormat &sf,size_t numsamples,Tbuffer &buf)
{
 size_t neededlen=std::max(size_t(1),numsamples)*sf.blockAlign();
 return buf.alloc(neededlen);
}

//----------------------- generic integer conversions --------------------
template<class Tin,class Tout> static Tout* convert(const Tin *in,Tout * const out,size_t count)
{
 unsigned int bpsIn =TsampleFormatInfo<Tin >::bps();
 unsigned int bpsOut=TsampleFormatInfo<Tout>::bps();
 if (bpsIn<bpsOut)
  for (size_t i=0;i<count;i++)
   out[i]=Tout(Tout(in[i])<<(bpsOut-bpsIn));
 else
  for (size_t i=0;i<count;i++)
   out[i]=Tout(in[i]>>(bpsIn-bpsOut));
 return out;
}

//---------------------------- int16 -> int32 ----------------------------
#ifndef WIN64
extern "C" void convert_16_32_mmx(const int16_t *inbuf,int32_t *outbuf,unsigned int c_loop);
#else
static void convert_16_32_mmx(const int16_t *inbuf,int32_t *outbuf,unsigned int c_loop)
{
 int eax=0;
 int ebx=c_loop;
 ebx<<=1;
 const unsigned char *esi=(const unsigned char*)inbuf;
 __m64 mm0=_mm_setzero_si64(),mm1=mm0;
 for (unsigned char *edi=(unsigned char*)outbuf;eax!=ebx;eax+=8)
  {
   punpcklwd (mm0,esi+eax);
   punpcklwd (mm1,esi+eax+4);
   movq (edi+2*eax,mm0);
   movq (edi+2*eax+8,mm1);
  }
 _mm_empty();
}
#endif
template<> int32_t* convert<int16_t,int32_t>(const int16_t *inbuf,int32_t * const outbuf,size_t count)
{
 size_t c_miss=Tconfig::cpu_flags&FF_CPU_MMX?count&3:count;
 size_t c_loop=count-c_miss;
 if (c_loop)
  convert_16_32_mmx(inbuf,outbuf,(unsigned int)c_loop);
 for (size_t i=0;i<c_miss;i++)
  outbuf[i+c_loop]=int32_t(inbuf[i+c_loop])<<16;
 return outbuf;
}

//---------------------------- int32 -> int16 ----------------------------
template<class _mm> static void convert_32_16_simd(const int32_t *inbuf,int16_t *outbuf,unsigned int c_loop)
{
 int eax=0;
 int ebx=c_loop;
 ebx<<=1;
 const unsigned char *esi=(const unsigned char*)inbuf;
 for (unsigned char *edi=(unsigned char*)outbuf;eax!=ebx;eax+=_mm::size)
  {
   typename _mm::__m mm0,mm1;
   movq (mm0,esi+2*eax);
   movq (mm1,esi+2*eax+_mm::size);
   psrad (mm0,16);
   psrad (mm1,16);
   packssdw (mm0,mm1);
   movq (edi+eax,mm0);
  }
 _mm::empty();
}
template<> int16_t* convert<int32_t,int16_t>(const int32_t *inbuf,int16_t * const outbuf,size_t count)
{
#ifdef __SSE2__
 bool sse2=Tconfig::cpu_flags&FF_CPU_SSE2 && (intptr_t(inbuf)&15)==0 &&(intptr_t(outbuf)&15)==0;
#else
 bool sse2=false;
#endif
 size_t c_miss=sse2?count&7:(Tconfig::cpu_flags&FF_CPU_MMX?count&3:count);
 size_t c_loop=count-c_miss;
 if (c_loop)
  #ifdef __SSE2__
  if (sse2)
   convert_32_16_simd<Tsse2>(inbuf,outbuf,(unsigned int)c_loop);
  else
  #endif
   convert_32_16_simd<Tmmx>(inbuf,outbuf,(unsigned int)c_loop);
 for (size_t i=0;i<c_miss;i++)
  outbuf[i+c_loop]=int16_t(inbuf[i+c_loop]>>16);
 return outbuf;
}

//---------------------------- float -> int16 ----------------------------
#ifndef WIN64
 extern "C" void convert_float_16_3dnow(const float *inbuf,int16_t *samples,unsigned int c_loop);
#else
 #define convert_float_16_3dnow NULL
#endif
static void convert_float_16_sse(const float *inbuf,int16_t *samples,unsigned int c_loop)
{
 const __m128 multiplier_float_16=_mm_set_ps1(32768.0);
 __m128 xmm7;
 movaps (xmm7, multiplier_float_16);
 const unsigned char *eax=(const unsigned char*)inbuf;
 int ebx=0;
 int ecx=c_loop;
 ecx<<=1;
 for (unsigned char *edx=(unsigned char*)samples;ecx!=ebx;ebx+=8)
  {
   __m128 xmm0,xmm1;
   movups  (xmm0, eax+ebx*2);
   mulps   (xmm0, xmm7);
   minps   (xmm0, xmm7);//                  ; x=min(x, MAX_SHORT)  --  +ve Signed Saturation > 2^31
   movhlps (xmm1, xmm0);
   __m64 mm0,mm1;
   cvtps2pi( mm0, xmm0);
   cvtps2pi( mm1, xmm1);
   packssdw (mm0, mm1);
   movq (edx+ebx, mm0);
  }
 _mm_empty();
}
#ifdef __SSE2__
static void convert_float_16_sse2(const float *inbuf,int16_t *samples,unsigned int c_loop)
{
 const __m128 multiplier_float_16=_mm_set_ps1(32768.0);
 __m128 xmm7;
 movaps (xmm7, multiplier_float_16);
 const unsigned char *eax=(const unsigned char*)inbuf;
 int ecx=c_loop;
 int edx=0;
 ecx<<=1;
 for (unsigned char  *edi=(unsigned char*)samples;ecx!=edx;edx+=16)
  {
   __m128 xmm0,xmm1;
   movups   (xmm0, eax+edx*2);           // xd | xc | xb | xa
   movups   (xmm1, eax+edx*2+16);        // xh | xg | xf | xe
   mulps    (xmm0, xmm7);                  // *= MAX_SHORT
   mulps    (xmm1, xmm7);                  // *= MAX_SHORT
   minps    (xmm0, xmm7);                  // x=min(x, MAX_SHORT)  --  +ve Signed Saturation > 2^31
   minps    (xmm1, xmm7);                // x=min(x, MAX_SHORT)  --  +ve Signed Saturation > 2^31
   __m128i xmm2,xmm3;
   cvtps2dq (xmm2, xmm0);                 // float -> dd | cc | bb | aa
   cvtps2dq (xmm3, xmm1);                  // float -> hh | gg | ff | ee
   packssdw (xmm2, xmm3);                  // h g | f e | d c | b a  --  +/-ve Signed Saturation > 2^15
   movdqa   (edi+edx-16+16, xmm2);          // store h g | f e | d c | b a
  }
}
#else
 #define convert_float_16_sse2 NULL
#endif

//---------------------------- float -> int32 ----------------------------
#ifndef WIN64
 extern "C" void convert_float_32_3dnow(const float *inbuf,int32_t *samples,unsigned int c_loop);
#else
 #define convert_float_32_3dnow NULL
#endif
static void convert_float_32_sse(const float *inbuf,int32_t *samples,unsigned int c_loop)
{
 const __m128 multiplier_float_32=_mm_set_ps1(2147483647.0f);
 __m128 xmm7=multiplier_float_32;
 const unsigned char *eax=(const unsigned char*)inbuf;
 int ecx= c_loop;
 int edx=0;
 ecx<<=2;
 for (unsigned char *edi=(unsigned char*)samples;ecx!=edx;edx+=16)
  {
   __m128 xmm0,xmm1;
   movups   (xmm0, eax+edx);             //; xd | xc | xb | xa
   mulps    (xmm0, xmm7   );               //; *= MAX_INT
   movhlps  (xmm1, xmm0   );               //; xx | xx | xd | xc
   __m64 mm0,mm1;
   cvtps2pi (mm0, xmm0    );               //; float -> bb | aa  --  -ve Signed Saturation
   cmpnltps (xmm0, xmm7   );      //;!(xd | xc | xb | xa < MAX_INT)
   cvtps2pi (mm1, xmm1    );               // float -> dd | cc  --  -ve Signed Saturation
   // md | mc | mb | ma                          -- YUCK!!!
   pxor     (mm0, &xmm0);         // 0x80000000 -> 0x7FFFFFFF if +ve saturation
   pxor     (mm1, (uint8_t*)&xmm0+8);
   movq     (edi+edx-16+16, mm0);           //store bb | aa
   movq     (edi+edx-8+16, mm1);            // store dd | cc
  }
 _mm_empty();
}
#ifdef __SSE2__
static void convert_float_32_sse2(const float *inbuf,int32_t *samples,unsigned int c_loop)
{
 const __m128 multiplier_float_32=_mm_set_ps1(2147483647.0f);
 __m128 xmm7=multiplier_float_32;
 //movss    (xmm7, multiplier_float_32);
 //shufps   xmm7, xmm7, 00000000b
 const unsigned char *eax=(const unsigned char*)inbuf;
 int ecx=c_loop;
 int edx=0;
 ecx<<= 2;
 for (unsigned char *edi=(unsigned char*)samples;ecx!=edx;edx+=32)
  {
   __m128 xmm0,xmm1;
   movups   (xmm0, eax+edx);             // xd | xc | xb | xa
   movups   (xmm1, eax+edx+16);          // xh | xg | xf | xe
   mulps    (xmm0, xmm7);                  // *= MAX_INT
   mulps    (xmm1, xmm7);                  // *= MAX_INT
   // Bloody Intel and their "indefinite integer value" it
   // is no use to man or beast, we need proper saturation
   // like AMD does with their 3DNow instructions. Grrr!!!
   __m128i xmm2,xmm3;
   cvtps2dq (xmm2, xmm0);                  // float -> dd | cc | bb | aa  --  -ve Signed Saturation
   cvtps2dq (xmm3, xmm1);                  // float -> hh | gg | ff | ee  --  -ve Signed Saturation
   cmpnltps (xmm0, xmm7);                  // !(xd | xc | xb | xa < MAX_INT)
   cmpnltps (xmm1, xmm7);                  // !(xh | xg | xf | xe < MAX_INT)

   #if (_MSC_VER >= 1500)
   pxor     (xmm2, _mm_castps_si128((const __m128)xmm0));                  // 0x80000000 -> 0x7FFFFFFF if +ve saturation
   pxor     (xmm3, _mm_castps_si128((const __m128)xmm1));
   #else
   pxor     (xmm2, _mm_castps_si128(xmm0));                  // 0x80000000 -> 0x7FFFFFFF if +ve saturation
   pxor     (xmm3, _mm_castps_si128(xmm1));
   #endif
   movdqa   (edi+edx-32+32, xmm2);          // store dd | cc | bb | aa
   movdqa   (edi+edx-16+32, xmm3);          // store hh | gg | ff | ee
  }
}
#else
 #define convert_float_32_sse2 NULL
#endif

template<class Tout> struct TconvertFromFloat
{
 typedef void (*TconvertFromFloatFc)(const float *inbuf,Tout *samples,unsigned int c_loop);
 static Tout* convert(const float *inbuf,Tout *samples,size_t count,TconvertFromFloatFc fc_3dnow,TconvertFromFloatFc fc_sse,TconvertFromFloatFc fc_sse2)
  {
   size_t c_miss,c_loop;
   if (fc_sse2 && Tconfig::cpu_flags&FF_CPU_SSE2 && !((intptr_t)samples&3))
    {
     while (((intptr_t)samples&15) && count)
      {
       *samples++=TsampleFormatInfo<Tout>::limit(*inbuf++*TsampleFormatInfo<Tout>::max());
       count-=1;
      }
     c_miss=count&7;
     c_loop=count-c_miss;
     if (c_loop)
      fc_sse2(inbuf,samples,(unsigned int)c_loop);
    }
   #ifndef WIN64
   else if (fc_3dnow && Tconfig::cpu_flags&FF_CPU_3DNOW)
    {
     c_miss=count&3;
     c_loop=count-c_miss;
     if (c_loop)
      fc_3dnow(inbuf,samples,(unsigned int)c_loop);
    }
   #endif
   else if (fc_sse && Tconfig::cpu_flags&FF_CPU_SSE) //TODO fix?
    {
     c_miss=count&3;
     c_loop=count-c_miss;
     if (c_loop)
      fc_sse(inbuf,samples,(unsigned int)c_loop);
    }
   else
    {
     c_miss=count;
     c_loop=0;
    }
   for (size_t i=0;i<c_miss;i++)
    samples[i+c_loop]=TsampleFormatInfo<Tout>::limit(inbuf[i+c_loop]*TsampleFormatInfo<Tout>::max());
   return samples;
  }
};

//---------------------------- int16 -> float ----------------------------
#ifndef WIN64
 extern "C" void convert_16_float_3dnow(const int16_t*inbuf,float *samples,unsigned int c_loop);
#else
 #define convert_16_float_3dnow NULL
#endif
static void convert_16_float_sse(const int16_t*inbuf,float *samples,unsigned int c_loop)
{
 int eax=0;
 int edx=c_loop;
 const unsigned char *esi=(const unsigned char*)inbuf;
 edx<<=1;// Number of input bytes.
 const __m128 divisor_float_16=_mm_set_ps1(0.000030517578125f);
 __m128 xmm7=divisor_float_16;
 for (unsigned char *edi=(unsigned char*)samples;eax!=edx;eax+=8)
  {
   __m64 mm0,mm1;
   movq      (mm1, esi+eax);            //;  d c | b a
   punpcklwd (mm0, mm1  );                //;  b x | a x
   punpckhwd (mm1, mm1  );                //;  d d | c c
   psrad     (mm0, 16   );                //;  sign extend
   psrad     (mm1, 16   );                //;  sign extend
   __m128 xmm0,xmm1;
   cvtpi2ps  (xmm0, mm0 );                //;  - b | - a -> float
   cvtpi2ps  (xmm1, mm1 );                //;  - d | - c -> float
   movlhps   (xmm0, xmm1);                //;  xd  xc || xb  xa
   mulps     (xmm0, xmm7);                //;  *=1/MAX_SHORT
   movups    (edi+(eax+8)*2-16, xmm0);      //;  store xd | xc | xb | xa
  }
 _mm_empty();
}
#ifdef __SSE2__
static void convert_16_float_sse2(const int16_t*inbuf,float *samples,unsigned int c_loop)
{
 int eax=0;
 int edx=c_loop;
 const unsigned char *esi=(const unsigned char*)inbuf;
 edx<<= 1;  // Number of input bytes.
 __m128 xmm7=_mm_set_ps1(0.000030517578125f);
 for (unsigned char *edi=(unsigned char*)samples;eax!=edx;eax+=16)
  {
   __m128i xmm0,xmm1;
   movdqu    (xmm1, esi+eax);           //  h g | f e | d c | b a

   punpcklwd (xmm0, xmm1);                //  d x | c x | b x | a x
   punpckhwd (xmm1, xmm1);                //  h h | g g | f f | e e
   psrad     (xmm0, 16 );                 //  sign extend
   psrad     (xmm1, 16 );                 //  sign extend
   __m128 xmm2,xmm3;
   cvtdq2ps  (xmm2, xmm0);                //  - d | - c | - b | - a -> float
   cvtdq2ps  (xmm3, xmm1);                //  - h | - g | - f | - e -> float
   mulps     (xmm2, xmm7);                //  *=1/MAX_SHORT
   mulps     (xmm3, xmm7);                //  *=1/MAX_SHORT
   movaps    (edi+(eax+16)*2-32, xmm2);      //  store xd | xc | xb | xa
   movaps    (edi+(eax+16)*2-16, xmm3);      //  store xh | xg | xf | xe
  }
}
#else
 #define convert_16_float_sse2 NULL
#endif

//---------------------------- int32 -> float ----------------------------
#ifndef WIN64
 extern "C" void convert_32_float_3dnow(const int32_t*inbuf,float *samples,unsigned int c_loop);
#else
 #define convert_32_float_3dnow NULL
#endif
static void convert_32_float_sse(const int32_t*inbuf,float *samples,unsigned int c_loop)
{
 int eax=0;
 int edx= c_loop;
 const unsigned char *esi=(const unsigned char*)inbuf;
 edx<<=2;//  in input bytes (*4)
 const __m128 divisor_float_32=_mm_set_ps1(4.656612875245796924105750827168e-10f);
 __m128 xmm7=divisor_float_32;
 for (unsigned char *edi=(unsigned char*)samples;eax!=edx;eax+= 16)
  {
   __m64 mm0,mm1;
   movq     (mm0, esi+eax);        // b b | a a
   movq     (mm1, esi+eax+8);      // d d | c c
   __m128 xmm0,xmm1;
   cvtpi2ps (xmm0, mm0);             // b b | a a -> float
   cvtpi2ps (xmm1, mm1);             // d d | c c -> float
   movlhps  (xmm0, xmm1);            // xd  xc || xb  xa
   mulps    (xmm0, xmm7);            // *=1/MAX_INT
   movups   (edi+eax-16+16, xmm0);    // store xd | xc | xb | xa
  }
 _mm_empty();
}
#ifdef __SSE2__
static void convert_32_float_sse2(const int32_t*inbuf,float *samples,unsigned int c_loop)
{
 int eax=0;                 //  count
 int edx= c_loop;
 const unsigned char *esi=(const unsigned char*)inbuf;
 edx<<= 2;                     //  in input bytes (*4)
 __m128 xmm7=_mm_set_ps1(4.656612875245796924105750827168e-10f);
 for (unsigned char *edi=(unsigned char*)samples;eax!= edx;eax+=32)
  {
   __m128i xmm0,xmm1;
   movdqu   (xmm0, esi+eax);        //  dd | cc | bb | aa
   movdqu   (xmm1, esi+eax+16);     //  hh | gg | ff | ee
   __m128 xmm2,xmm3;
   cvtdq2ps (xmm2, xmm0);             //  xd | xc | xb | xa
   cvtdq2ps (xmm3, xmm1);             //  xh | xg | xf | xe
   mulps    (xmm2, xmm7);             //  *=1/MAX_INT
   mulps    (xmm3, xmm7);             //  *=1/MAX_INT
   movaps   (edi+eax+32-32, xmm2);     //  store xd | xc | xb | xa
   movaps   (edi+eax+32-16, xmm3);     //  store xh | xg | xf | xe
  }
}
#else
 #define convert_32_float_sse2 NULL
#endif

template<class Tin> struct TconvertToFloat
{
 typedef void (*TconvertToFloatFc)(const Tin*inbuf,float *samples,unsigned int c_loop);
 static float* convert(const Tin *samples,float *outbuf,size_t count,TconvertToFloatFc fc_3dnow,TconvertToFloatFc fc_sse,TconvertToFloatFc fc_sse2)
  {
   static const float divisor=1.0f/-TsampleFormatInfo<Tin>::min();
   size_t c_miss,c_loop;
   if (fc_sse2 && Tconfig::cpu_flags&FF_CPU_SSE2)
    {
     while (((intptr_t)outbuf&15) && count)
      {
       *outbuf++=divisor*(float)*samples++;
       count-=1;
      }
     c_miss=count&7;
     c_loop=count-c_miss;
     if (c_loop)
      fc_sse2(samples,outbuf,(unsigned int)c_loop);
    }
   #ifndef WIN64
   else if (fc_3dnow && Tconfig::cpu_flags&FF_CPU_3DNOW)
    {
     c_miss=count&3;
     c_loop=count-c_miss;
     if (c_loop)
      fc_3dnow(samples,outbuf,(unsigned int)c_loop);
    }
   #endif
   else if (fc_sse && Tconfig::cpu_flags&FF_CPU_SSE)
    {
     c_miss=count&3;
     c_loop=count-c_miss;
     if (c_loop)
      fc_sse(samples,outbuf,(unsigned int)c_loop);
    }
   else
    {
     c_miss=count;
     c_loop=0;
    }
   for (size_t i=0;i<c_miss;i++)
    outbuf[i+c_loop]=(float)samples[i+c_loop]*divisor;
   return outbuf;
  }
};

void* TaudioFilter::convertTo(const TsampleFormat &sfIn,const void *bufIn,size_t numsamples,const TsampleFormat &sfOut,size_t samplestoalloc)
{
 if (sfIn.sf==sfOut.sf || numsamples==0) return (void*)bufIn;
 void *bufOut=alloc_buffer(sfOut,samplestoalloc?samplestoalloc:numsamples);
 const size_t count=sfIn.nchannels*numsamples;
 switch (sfIn.sf)
  {
   case TsampleFormat::SF_FLOAT32:
    {
     int dithering=deci->getParam2(IDFF_dithering),noiseshaping=deci->getParam2(IDFF_noiseShaping); //TODO don't read this always
     if (dithering || noiseshaping)
      {
       if (!dither || oldnoiseshaping!=noiseshaping || oldsfout!=sfOut.sf)
        {
         oldnoiseshaping=noiseshaping;oldsfout=sfOut.sf;
         if (dither) delete dither;
         dither=new Tdither(sfOut.bitsPerSample(),noiseshaping);
        }
       switch (sfOut.sf)
        {
         case TsampleFormat::SF_PCM16:return dither->process((const float*)bufIn,(int16_t*)bufOut,sfOut.nchannels,numsamples,dithering);
         case TsampleFormat::SF_PCM24:return dither->process((const float*)bufIn,(int24_t*)bufOut,sfOut.nchannels,numsamples,dithering);
         case TsampleFormat::SF_PCM32:return dither->process((const float*)bufIn,(int32_t*)bufOut,sfOut.nchannels,numsamples,dithering);
        }
      }
     else
      switch (sfOut.sf)
       {
        case TsampleFormat::SF_PCM16:return TconvertFromFloat<int16_t>::convert((const float*)bufIn,(int16_t*)bufOut,count,convert_float_16_3dnow,convert_float_16_sse,convert_float_16_sse2);
        case TsampleFormat::SF_PCM24:return TconvertFromFloat<int24_t>::convert((const float*)bufIn,(int24_t*)bufOut,count,NULL,NULL,NULL);
        case TsampleFormat::SF_PCM32:return TconvertFromFloat<int32_t>::convert((const float*)bufIn,(int32_t*)bufOut,count,convert_float_32_3dnow,convert_float_32_sse,convert_float_32_sse2);
       }
     break;
    }
   case TsampleFormat::SF_PCM16:
    switch (sfOut.sf)
     {
      case TsampleFormat::SF_PCM24:return convert((const int16_t*)bufIn,(int24_t*)bufOut,count);
      case TsampleFormat::SF_PCM32:return convert((const int16_t*)bufIn,(int32_t*)bufOut,count);
      case TsampleFormat::SF_FLOAT32:return TconvertToFloat<int16_t>::convert((const int16_t*)bufIn,(float*)bufOut,count,convert_16_float_3dnow,convert_16_float_sse,convert_16_float_sse2);
     }
    break;
   case TsampleFormat::SF_PCM24:
    switch (sfOut.sf)
     {
      case TsampleFormat::SF_PCM16:return convert((const int24_t*)bufIn,(int16_t*)bufOut,count);
      case TsampleFormat::SF_PCM32:return convert((const int24_t*)bufIn,(int32_t*)bufOut,count);
      case TsampleFormat::SF_FLOAT32:return TconvertToFloat<int24_t>::convert((const int24_t*)bufIn,(float*)bufOut,count,NULL,NULL,NULL);
     }
    break;
   case TsampleFormat::SF_PCM32:
    switch (sfOut.sf)
     {
      case TsampleFormat::SF_PCM16:return convert((const int32_t*)bufIn,(int16_t*)bufOut,count);
      case TsampleFormat::SF_PCM24:return convert((const int32_t*)bufIn,(int24_t*)bufOut,count);
      case TsampleFormat::SF_FLOAT32:return TconvertToFloat<int32_t>::convert((const int32_t*)bufIn,(float*)bufOut,count,convert_32_float_3dnow,convert_32_float_sse,convert_32_float_sse2);
     }
    break;
  }
 return bufOut;
}

void* TaudioFilter::init(const TfilterSettingsAudio *cfg,TsampleFormat &sf,const void *bufIn,size_t numsamples,size_t samplestoalloc)
{
 bool honourPreferred=true;
 int supSF=getSupportedFormats(cfg,&honourPreferred,sf);
 int honouredSF=honourPreferred?(parent->preferredsfs|TsampleFormat::SF_PCM24):TsampleFormat::SF_ALL;
 if (supSF&honouredSF)
  supSF&=honouredSF;
 if (sf.sf&supSF)
  return (void*)bufIn;
 TsampleFormat sfIn=sf;
 sf.sf=TsampleFormat::sf_bestMatch(sf.sf,supSF);
 return convertTo(sfIn,bufIn,numsamples,sf,samplestoalloc);
}

bool TaudioFilter::is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg)
{
 return cfg->is && cfg->show;
}

bool TaudioFilter::getOutputFmt(TsampleFormat &sf,const TfilterSettingsAudio *cfg)
{
 if (!is(sf,cfg)) return false;
 bool honourPreferred=true;
 int supSF=getSupportedFormats(cfg,&honourPreferred,sf);
 int honouredSF=honourPreferred?(parent->preferredsfs|TsampleFormat::SF_PCM24):TsampleFormat::SF_ALL;
 if (supSF&honouredSF)
  supSF&=honouredSF;
 if ((sf.sf&supSF)==0)
  sf.sf=TsampleFormat::sf_bestMatch(sf.sf,supSF);
 return true;
}

HRESULT TaudioFilter::flush(TfilterQueue::iterator it,TsampleFormat &fmt,const TfilterSettingsAudio *cfg0)
{
 return parent->deliverSamples(++it,fmt,NULL,0);
}
