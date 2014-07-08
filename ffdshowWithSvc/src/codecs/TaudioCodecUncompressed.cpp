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
#include "TaudioCodecUncompressed.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"
#include "ffdshow_mediaguids.h"

TaudioCodecUncompressed::TaudioCodecUncompressed(IffdshowBase *deci,IdecAudioSink *Isink):
 Tcodec(deci),
 TaudioCodec(deci,Isink)
{
}

bool TaudioCodecUncompressed::init(const CMediaType &mt)
{
 fmt=TsampleFormat(mt);
 bit8=lpcm20=lpcm24=be=float64=false;
 if (codecId==CODEC_ID_PCM)
  {
   if (fmt.sf==TsampleFormat::SF_PCM8)
    {
     fmt.sf=TsampleFormat::SF_PCM16;
     bit8=true;
    }
   if (fmt.sf==TsampleFormat::SF_FLOAT64)
    {
     fmt.sf=TsampleFormat::SF_FLOAT32;
     float64=true;
    }
   if (fmt.pcm_be)
    {
     be=true;
     fmt.pcm_be=false;
    }
  }
 else if (codecId==CODEC_ID_LPCM)
  {
   if (fmt.sf==TsampleFormat::SF_LPCM20)
    {
     fmt.sf=TsampleFormat::SF_PCM32;
     lpcm20=true;
    }
   if (fmt.sf==TsampleFormat::SF_PCM24)
    {
     fmt.sf=TsampleFormat::SF_PCM32;
     lpcm24=true;
    }
  }
 return true;
}
TaudioCodecUncompressed::~TaudioCodecUncompressed()
{
}
void TaudioCodecUncompressed::getInputDescr1(char_t *buf,size_t buflen) const
{
 if (lpcm20)
  ff_strncpy(buf,_l("20-bit lpcm"),buflen);
 else if (lpcm24)
  ff_strncpy(buf,_l("24-bit lpcm"),buflen);
 else
  ff_strncpy(buf,_l("pcm"),buflen);
 buf[buflen-1]='\0';
}

template<class sample_t> void TaudioCodecUncompressed::swapbe(sample_t *dst,size_t size)
{
 for (size_t i=0;i<size/sizeof(sample_t);i++)
  bswap(dst[i]);
}

HRESULT TaudioCodecUncompressed::decode(TbyteBuffer &src)
{
 void *samples=&*src.begin();size_t numsamples=src.size()/fmt.blockAlign();
 if (codecId==CODEC_ID_LPCM)
  {
   if (lpcm20)
    {
     int32_t *dst=(int32_t*)getDst(16*src.size()/10),*dst0=dst;
     for (size_t i=0;i<src.size();i+=10)
      {
        *dst++ = (src[i+0]<<24)+(src[i+1]<<16)+((src[i+8] & 0xf0)<<8);
        *dst++ = (src[i+2]<<24)+(src[i+3]<<16)+((src[i+8] << 4  )<<8);
        *dst++ = (src[i+4]<<24)+(src[i+5]<<16)+((src[i+9] & 0xf0)<<8);
        *dst++ = (src[i+6]<<24)+(src[i+7]<<16)+((src[i+9] << 4  )<<8);
      }
     samples=dst0;numsamples=sizeof(int32_t)*(dst-dst0)/fmt.blockAlign();
    }
   else if (lpcm24)
    {
     int32_t *dst=(int32_t*)getDst(4*src.size()/3),*dst0=dst;
     for (size_t i=0;i<src.size();i+=12)
      {
       *dst++=(src[i  ]<<24)+(src[i+1]<<16)+(src[i+ 8]<<8);
       *dst++=(src[i+2]<<24)+(src[i+3]<<16)+(src[i+ 9]<<8);
       *dst++=(src[i+4]<<24)+(src[i+5]<<16)+(src[i+10]<<8);
       *dst++=(src[i+6]<<24)+(src[i+7]<<16)+(src[i+11]<<8);
      }
     samples=dst0;numsamples=sizeof(int32_t)*(dst-dst0)/fmt.blockAlign();
    }
   else if (fmt.sf==TsampleFormat::SF_PCM16)
    {
     for (size_t i=0;i<src.size()/2;i++)
      std::swap(src[2*i],src[2*i+1]);
    }
  }
 else if (bit8)
  {
   int16_t *dst=(int16_t*)getDst(2*src.size());
   for (size_t i=0;i<src.size();i++)
    dst[i]=int16_t((src[i]-128)<<8);
   samples=dst;numsamples=2*src.size()/fmt.blockAlign();
  }
 else if (be)
  {
   if (float64)
    swapbe((double*)samples,src.size());
   else
    switch (fmt.sf)
     {
      case TsampleFormat::SF_PCM16:swapbe((int16_t*)samples,src.size());break;
      case TsampleFormat::SF_PCM24:swapbe((int24_t*)samples,src.size());break;
      case TsampleFormat::SF_PCM32:swapbe((int32_t*)samples,src.size());break;
      case TsampleFormat::SF_FLOAT32:swapbe((float*)samples,src.size());break;
     }
  }

 if (float64)
  {
   for (size_t i=0;i<numsamples;i++)
    ((float*)samples)[i]=(float)((double*)samples)[i];
   numsamples/=2;
  }

 HRESULT hr=sinkA->deliverDecodedSample(samples,numsamples,fmt,1);
 src.clear();
 return hr;
}
