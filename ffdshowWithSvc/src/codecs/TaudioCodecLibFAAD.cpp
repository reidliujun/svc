/*
 * Copyright (c) 2003-2006 Milan Cutka
 * based on CoreAAC - AAC DirectShow Decoder Filter  (C) 2003 Robert Cioch
 *      and MpaDecFilter by Gabest
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
#include "TaudioCodecLibFAAD.h"
#include "Tdll.h"
#include "ffdebug.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"
#include "dsutil.h"

const char_t* TaudioCodecLibFAAD::dllname=_l("ff_libfaad2.dll");

TaudioCodecLibFAAD::TaudioCodecLibFAAD(IffdshowBase *deci,IdecAudioSink *Isink):
 Tcodec(deci),
 TaudioCodec(deci,Isink)
{
 dll=NULL;m_decHandle=NULL;
 inited=false;
 ps=sbr=false;
 chmask[FRONT_CHANNEL_CENTER]=SPEAKER_FRONT_CENTER;
 chmask[FRONT_CHANNEL_LEFT  ]=SPEAKER_FRONT_LEFT;
 chmask[FRONT_CHANNEL_RIGHT ]=SPEAKER_FRONT_RIGHT;
 chmask[SIDE_CHANNEL_LEFT   ]=SPEAKER_SIDE_LEFT;
 chmask[SIDE_CHANNEL_RIGHT  ]=SPEAKER_SIDE_RIGHT;
 chmask[BACK_CHANNEL_LEFT   ]=SPEAKER_BACK_LEFT;
 chmask[BACK_CHANNEL_RIGHT  ]=SPEAKER_BACK_RIGHT;
 chmask[BACK_CHANNEL_CENTER ]=SPEAKER_BACK_CENTER;
 chmask[LFE_CHANNEL         ]=SPEAKER_LOW_FREQUENCY;
}

bool TaudioCodecLibFAAD::init(const CMediaType &mt)
{
 dll=new Tdll(dllname,config);
 dll->loadFunction(NeAACDecOpen,"NeAACDecOpen");
 dll->loadFunction(NeAACDecClose,"NeAACDecClose");
 dll->loadFunction(NeAACDecInit2,"NeAACDecInit2");
 dll->loadFunction(NeAACDecAudioSpecificConfig,"NeAACDecAudioSpecificConfig");
 dll->loadFunction(NeAACDecDecode,"NeAACDecDecode");
 dll->loadFunction(NeAACDecGetErrorMessage,"NeAACDecGetErrorMessage");
 dll->loadFunction(NeAACDecGetCurrentConfiguration,"NeAACDecGetCurrentConfiguration");
 dll->loadFunction(NeAACDecSetConfiguration,"NeAACDecSetConfiguration");
 if (dll->ok)
  {
   m_decHandle=NeAACDecOpen();
   unsigned long SamplesPerSec=0;
   unsigned char channels=0;
   Textradata extradata(mt);
   if (NeAACDecInit2(m_decHandle,extradata.data,(unsigned long)extradata.size,&SamplesPerSec,&channels)<0)
    return false;
   fmt.freq=SamplesPerSec;
   fmt.setChannels(channels);
   NeAACDecAudioSpecificConfig(extradata.data,(unsigned long)extradata.size,&info);
   NeAACDecConfigurationPtr c=NeAACDecGetCurrentConfiguration(m_decHandle);
   c->outputFormat=FAAD_FMT_FLOAT;fmt.sf=TsampleFormat::SF_FLOAT32;
   NeAACDecSetConfiguration(m_decHandle,c);
   inited=true;
   return true;
  }
 else
  return false;
}
TaudioCodecLibFAAD::~TaudioCodecLibFAAD()
{
 if (m_decHandle)
  NeAACDecClose(m_decHandle);
 if (dll) delete dll;
}

void TaudioCodecLibFAAD::getInputDescr1(char_t *buf,size_t buflen) const
{
 if (!inited) return;
 tsnprintf_s(buf, buflen, _TRUNCATE,_l("%s%sAAC"),sbr?_l("SBR "):_l(""),ps?_l("PS "):_l(""));
}

HRESULT TaudioCodecLibFAAD::decode(TbyteBuffer &src)
{
 NeAACDecFrameInfo frameInfo;
 float *outsamples=(float*)NeAACDecDecode(m_decHandle,&frameInfo,&*src.begin(),(unsigned long)src.size());
 src.clear();
 if (frameInfo.error)
  {
   DPRINTF(_l("AAC: Error %d [%s]\n"),frameInfo.error,(const char_t*)text<char_t>(NeAACDecGetErrorMessage(frameInfo.error)));
   return S_OK;//S_FALSE
  }
 else
  if (outsamples && frameInfo.samples)
   {
    ps=!!frameInfo.ps;sbr=!!frameInfo.sbr;
    numframes++;bpssum+=(lastbps=8*fmt.freq*frameInfo.bytesconsumed/(frameInfo.samples/fmt.nchannels)/1000);
    if (frameInfo.channels==2 && frameInfo.channel_position[1]==UNKNOWN_CHANNEL)
     {
      frameInfo.channel_position[0]=FRONT_CHANNEL_LEFT;
      frameInfo.channel_position[1]=FRONT_CHANNEL_RIGHT;
     }

    fmt=this->fmt;
    fmt.channelmask=0;
    for (int i=0;i<frameInfo.channels;i++)
     fmt.channelmask|=chmask[frameInfo.channel_position[i]];

    int chmap[countof(frameInfo.channel_position)];memset(chmap,0,sizeof(chmap));

    for (int i=0;i<frameInfo.channels;i++)
     {
      unsigned int ch=0;int mask=chmask[frameInfo.channel_position[i]];
      for (int j=0;j<32;j++)
        if (fmt.channelmask&(1<<j))
         {
          if ((1<<j)==mask)
           {
            chmap[i]=ch;
            break;
           }
          ch++;
         }
     }

    if (frameInfo.channels<=2) fmt.channelmask=0;

    float *dst,*dst0;dst=dst0=(float*)getDst(frameInfo.samples*sizeof(float));
    for (unsigned int j=0;j<frameInfo.samples;j+=frameInfo.channels,dst+=frameInfo.channels)
     for (int i=0;i<frameInfo.channels;i++)
      dst[chmap[i]]=*outsamples++;

    return sinkA->deliverDecodedSample(dst0,frameInfo.samples/frameInfo.channels,fmt,1);
   }
  else
   return S_OK;
}
bool TaudioCodecLibFAAD::onSeek(REFERENCE_TIME segmentStart)
{
 return false;
}
