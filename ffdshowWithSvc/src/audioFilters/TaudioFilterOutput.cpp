/*
 * Copyright (c) 2004-2006 Milan Cutka
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
#include "TaudioFilterOutput.h"
#include "ToutputAudioSettings.h"
#include "Tconfig.h"
#include "IffdshowBase.h"
#include "IffdshowDecAudio.h"
#include "Tlibavcodec.h"
#include "libavcodec/ac3.h"
#include "liba52/a52.h"
#include "TffdshowDecAudioInputPin.h"

TaudioFilterOutput::TaudioFilterOutput(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 libavcodec=NULL;avctx=NULL;
 oldsf.freq=0;
 ac3inited=false;
 ac3buf=(uint8_t*)malloc(AC3_MAX_CODED_FRAME_SIZE);
}
TaudioFilterOutput::~TaudioFilterOutput()
{
 free(ac3buf);
 if (libavcodec) libavcodec->Release();
}

void TaudioFilterOutput::done(void)
{
 if (avctx)
  {
   if (ac3inited) libavcodec->avcodec_close(avctx);
   libavcodec->av_free(avctx);
   avctx=NULL;
  }
 ac3inputbuf.clear();
 ac3inited=false;
}

int TaudioFilterOutput::getSupportedFormats(const TfilterSettingsAudio *cfg0,bool *honourPreferred,const TsampleFormat &sf) const
{
 *honourPreferred=false;
 const ToutputAudioSettings *cfg=(ToutputAudioSettings*)cfg0;
 if (cfg->outsfs & TsampleFormat::SF_AC3)
  {
   if (cfg->outAC3EncodeMode)
    {
     if (sf.nchannels == 6)
      return TsampleFormat::SF_PCM16;
     int outsfs = cfg->outsfs & ~TsampleFormat::SF_AC3;
     if (outsfs)
      return outsfs;
     else
      return TsampleFormat::SF_PCM16 | TsampleFormat::SF_PCM24 | TsampleFormat::SF_PCM32;
    }
   else
    return TsampleFormat::SF_PCM16;
  }
 if (cfg->outsfs==TsampleFormat::SF_LPCM16)
  return TsampleFormat::SF_PCM16;
 else
  return cfg->outsfs;
}

bool TaudioFilterOutput::getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg0)
{
 const ToutputAudioSettings *cfg=(ToutputAudioSettings*)cfg0;
 if (cfg->outsfs==TsampleFormat::SF_LPCM16)
  fmt.sf=TsampleFormat::SF_LPCM16;
 else if (cfg->outsfs & TsampleFormat::SF_AC3)
  {
   if (cfg->outAC3EncodeMode && fmt.nchannels != 6)
    {
     int outsfs = cfg->outsfs & ~TsampleFormat::SF_AC3;
     if (outsfs == 0)
      outsfs = TsampleFormat::SF_PCM16 | TsampleFormat::SF_PCM24 | TsampleFormat::SF_PCM32;
     if ((fmt.sf & outsfs)==0)
      fmt.sf = TsampleFormat::sf_bestMatch(fmt.sf, outsfs);
    }
   else
    {
     fmt.sf = TsampleFormat::SF_AC3;
    }
  }
 else if ((fmt.sf & cfg->outsfs)==0)
  fmt.sf=TsampleFormat::sf_bestMatch(fmt.sf,cfg->outsfs);
 return true;
}

HRESULT TaudioFilterOutput::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const ToutputAudioSettings *cfg=(ToutputAudioSettings*)cfg0;
 samples=init(cfg,fmt,samples,numsamples);
 
 // Change to PCM format if AC3 encode is requested only for multichannel streams and codec is not AC3/DTS
 bool changeFormat = false;
 const TffdshowDecAudioInputPin *pin =  deciA->GetCurrentPin();
 if (   pin
     && pin->audio
     && cfg->outAC3EncodeMode == 1
     && (cfg->outsfs & TsampleFormat::SF_AC3)
     && fmt.nchannels != 6
    )
  changeFormat = true;

 if (cfg->outsfs==TsampleFormat::SF_LPCM16)
  {
   fmt.sf=TsampleFormat::SF_LPCM16;
   int16_t *samples16=(int16_t*)samples;
   for (size_t i=0;i<numsamples*fmt.nchannels;i++)
    bswap(samples16[i]);
  }
 else if ((cfg->outsfs & TsampleFormat::SF_AC3) && parent->config->isDecoder[IDFF_MOVIE_LAVC]  && !changeFormat)
  {
   if (!libavcodec) deci->getLibavcodec(&libavcodec);
   if (oldsf!=fmt)
    {
     done();
     oldsf=fmt;
     int channelmask=fmt.makeChannelMask();
     struct Tac3channels
      {
       int ac3mode;
       int speakers[6];
       int mask;
      };
     static const Tac3channels ac3channels[]=
      {
       A52_MONO   ,SPEAKER_FRONT_CENTER,0                   ,0                  ,0                  ,0                 ,0,SPEAKER_FRONT_CENTER,
       A52_STEREO ,SPEAKER_FRONT_LEFT  ,SPEAKER_FRONT_RIGHT ,0                  ,0                  ,0                 ,0,SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT,
       A52_3F     ,SPEAKER_FRONT_LEFT  ,SPEAKER_FRONT_CENTER,SPEAKER_FRONT_RIGHT,0                  ,0                 ,0,SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER,
       A52_2F1R   ,SPEAKER_FRONT_LEFT  ,SPEAKER_FRONT_RIGHT ,SPEAKER_BACK_CENTER,0                  ,0                 ,0,SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_BACK_CENTER,
       A52_3F1R   ,SPEAKER_FRONT_LEFT  ,SPEAKER_FRONT_CENTER,SPEAKER_FRONT_RIGHT,SPEAKER_BACK_CENTER,0                 ,0,SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_BACK_CENTER,
       A52_2F2R   ,SPEAKER_FRONT_LEFT  ,SPEAKER_FRONT_RIGHT ,SPEAKER_BACK_LEFT  ,SPEAKER_BACK_RIGHT ,0                 ,0,SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT,
       A52_3F2R   ,SPEAKER_FRONT_LEFT  ,SPEAKER_FRONT_CENTER,SPEAKER_FRONT_RIGHT,SPEAKER_BACK_LEFT  ,SPEAKER_BACK_RIGHT,0,SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT,

       A52_MONO  |A52_LFE ,SPEAKER_FRONT_CENTER,SPEAKER_LOW_FREQUENCY,0                    ,0                    ,0                    ,0                    ,SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY,
       A52_STEREO|A52_LFE ,SPEAKER_FRONT_LEFT  ,SPEAKER_FRONT_RIGHT  ,SPEAKER_LOW_FREQUENCY,0                    ,0                    ,0                    ,SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_LOW_FREQUENCY,
       A52_3F    |A52_LFE ,SPEAKER_FRONT_LEFT  ,SPEAKER_FRONT_CENTER ,SPEAKER_FRONT_RIGHT  ,SPEAKER_LOW_FREQUENCY,0                    ,0                    ,SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY,
       A52_2F1R  |A52_LFE ,SPEAKER_FRONT_LEFT  ,SPEAKER_FRONT_RIGHT  ,SPEAKER_BACK_CENTER  ,SPEAKER_LOW_FREQUENCY,0                    ,0                    ,SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_BACK_CENTER|SPEAKER_LOW_FREQUENCY,
       A52_3F1R  |A52_LFE ,SPEAKER_FRONT_LEFT  ,SPEAKER_FRONT_CENTER ,SPEAKER_FRONT_RIGHT  ,SPEAKER_BACK_CENTER  ,SPEAKER_LOW_FREQUENCY,0                    ,SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_BACK_CENTER|SPEAKER_LOW_FREQUENCY,
       A52_2F2R  |A52_LFE ,SPEAKER_FRONT_LEFT  ,SPEAKER_FRONT_RIGHT  ,SPEAKER_BACK_LEFT    ,SPEAKER_BACK_RIGHT   ,SPEAKER_LOW_FREQUENCY,0                    ,SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT|SPEAKER_LOW_FREQUENCY,
       A52_3F2R  |A52_LFE ,SPEAKER_FRONT_LEFT  ,SPEAKER_FRONT_CENTER ,SPEAKER_FRONT_RIGHT  ,SPEAKER_BACK_LEFT    ,SPEAKER_BACK_RIGHT   ,SPEAKER_LOW_FREQUENCY,SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT|SPEAKER_LOW_FREQUENCY,
      };
     for (int i=0;i<countof(ac3channels);i++)
      if (channelmask==ac3channels[i].mask)
       {
        AVCodec *avcodec=libavcodec->avcodec_find_encoder(CODEC_ID_AC3);
        avctx=libavcodec->avcodec_alloc_context();
        avctx->sample_rate=fmt.freq;
        avctx->channels=fmt.nchannels;
        avctx->bit_rate=cfg->outAC3bitrate*1000;
        avctx->ac3mode=ac3channels[i].ac3mode&~A52_LFE;
        avctx->ac3lfe=ac3channels[i].ac3mode&A52_LFE?1:0;
        for (unsigned int j=0;j<fmt.nchannels;j++)
         avctx->ac3channels[j]=fmt.findSpeaker(ac3channels[i].speakers[j]);
        ac3inited=libavcodec->avcodec_open(avctx,avcodec)>=0;
        break;
       }
    }
   if (ac3inited)
    {
     ac3inputbuf.append(samples,numsamples*fmt.blockAlign());
     TbyteBuffer::iterator inputsamples=ac3inputbuf.begin();
     int inputsize=(int)ac3inputbuf.size();
     int ac3framesize=avctx->frame_size*fmt.blockAlign();
     while (inputsize>=ac3framesize)
      {
       int ret=libavcodec->avcodec_encode_audio(avctx,ac3buf,AC3_MAX_CODED_FRAME_SIZE,(const short*)&*inputsamples);
       HRESULT hr;
	   if ((hr=deciA->deliverSampleSPDIF(ac3buf,ret,avctx->bit_rate, avctx->sample_rate,0x0001,false))!=S_OK)
        {
         numsamples=0;
         return hr;
        }
       inputsamples+=ac3framesize;inputsize-=ac3framesize;
      }
     if (inputsamples!=ac3inputbuf.begin())
      ac3inputbuf.erase(ac3inputbuf.begin(),inputsamples);
     numsamples=0;
    }
  }
 return parent->deliverSamples(++it,fmt,samples,numsamples);
}

void TaudioFilterOutput::onSeek(void)
{
 ac3inputbuf.clear();
}
