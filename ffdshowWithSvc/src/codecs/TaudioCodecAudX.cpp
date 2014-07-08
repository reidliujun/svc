/*
 * Copyright (c) 2006 Milan Cutka
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
#include "TaudioCodecAudX.h"
#include "mp3header.h"
#include "Tconfig.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"
#include "dsutil.h"
#include "Tdll.h"

const char_t* TaudioCodecAudX::dllname=_l("audxlib.dll");

TaudioCodecAudX::TaudioCodecAudX(IffdshowBase *deci,IdecAudioSink *Isink):
 Tcodec(deci),
 TaudioCodec(deci,Isink)
{
 layer=0;oldmode=-1;ctx=-1;
 dll=NULL;
}
bool TaudioCodecAudX::init(const CMediaType &mt)
{
 dll=new Tdll(dllname,config);
 dll->loadFunction(initDecoding,"initDecoding");
 dll->loadFunction(decodeAudXframe,"decodeAudXframe");
 dll->loadFunction(resetDecoder,"resetDecoder");
 if (!dll->ok)
  return false;

 hdrlayer=0;
 Textradata extradata(mt);
 if (extradata.size==sizeof(MPEG1WAVEFORMAT)-sizeof(WAVEFORMATEX))
  {
   const MPEG1WAVEFORMAT *m1wf=(const MPEG1WAVEFORMAT*)(extradata.data-sizeof(WAVEFORMATEX));
   switch (m1wf->fwHeadLayer)
    {
     case ACM_MPEG_LAYER1:hdrlayer=layer=mp3header::MAD_LAYER_I;break;
     case ACM_MPEG_LAYER2:hdrlayer=layer=mp3header::MAD_LAYER_II;break;
     case ACM_MPEG_LAYER3:hdrlayer=layer=mp3header::MAD_LAYER_III;break;
    }
  }
 ctx=initDecoding();
 fmt.sf=TsampleFormat::SF_PCM16;
 fmt.freq=mp3header::findNearestFreq(fmt.freq);
 return true;
}
TaudioCodecAudX::~TaudioCodecAudX()
{
 if (ctx) resetDecoder(ctx);
 if (dll) delete dll;
}
void TaudioCodecAudX::getInputDescr1(char_t *buf,size_t buflen) const
{
 if (layer)
  tsnprintf_s(buf, buflen, _TRUNCATE,_l("mp%i"),layer);
 else
  tsnprintf_s(buf, buflen, _TRUNCATE, _l("mp1/2/3"));
}
HRESULT TaudioCodecAudX::decode(TbyteBuffer &src0)
{
 TbyteBuffer::iterator src=src0.begin();
 int srcLength=(int)src0.size();
 for (;srcLength>4;)
  {
   mp3header hdr(&*src,srcLength);
   if (!hdr.ok || (hdrlayer!=mp3header::MAD_LAYER_I && hdr.layer==mp3header::MAD_LAYER_I) || (layer!=0 && hdr.layer!=layer) || hdr.samplerate!=fmt.freq/* || (oldmode!=-1 && hdr.mode!=oldmode)*/)
    {
     src++;
     srcLength--;
     continue;
    }
   bpssum+=(lastbps=hdr.bitrate/1000);numframes++;
   layer=hdr.layer;oldmode=hdr.mode;
   int framelen=hdr.calc_frame_len();
   if (srcLength<framelen)
    break;
   int used_bytes=0;
   long dstsize;
   short *dst=(short*)getDst(dstsize=102400);
   int isAudX=0;
   int decoded=decodeAudXframe(ctx,&*src,framelen,dst,&dstsize,&isAudX);
   if (isAudX)
    fmt.setChannels(6);
   if (decoded<0)
    break;
   else if (decoded>0)
    {
     used_bytes=framelen;
     if (dstsize)
      {
       HRESULT hr=sinkA->deliverDecodedSample(dst,dstsize/fmt.blockAlign(),fmt,1);
       if (hr!=S_OK)
        return hr;
      }
    }
   src+=used_bytes;srcLength-=used_bytes;
  }
 if (src!=src0.begin())
  src0.erase(src0.begin(),src);
 return S_OK;
}
bool TaudioCodecAudX::onSeek(REFERENCE_TIME segmentStart)
{
 if (ctx)
  {
   return true;
  }
 else
  return false;
}

bool TaudioCodecAudX::getVersion(const Tconfig *config,ffstring &vers,ffstring &license)
{
 if (Tdll::check(dllname,config))
  {
   vers=_l("present");
   return true;
  }
 else
  {
   vers=_l("not found");
   return false;
  }
}
