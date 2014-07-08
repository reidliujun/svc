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
#include "TaudioFilterResample.h"
#include "resample/resample2.h"
#include "Tdll.h"
#include "resample/libsamplerate/samplerate.h"

//===================================== TaudioFilterResampleBase =====================================
TaudioFilterResampleBase::TaudioFilterResampleBase(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 old.freq=0;oldfreq=0;oldnchannels=0;
}

bool TaudioFilterResampleBase::is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg0)
{
 const TresampleSettings *cfg=(const TresampleSettings*)cfg0;
 return super::is(fmt,cfg) && cfg->isResample(fmt);
}
bool TaudioFilterResampleBase::getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg0)
{
 if (super::getOutputFmt(fmt,cfg0))
  {
   const TresampleSettings *cfg=(const TresampleSettings*)cfg0;
   fmt.freq=cfg->freq;
   return true;
  }
 else
  return false;
}

//===================================== TaudioFilterResampleLavc =====================================
TaudioFilterResampleLavc::TaudioFilterResampleLavc(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilterResampleBase(Ideci,Iparent)
{
 memset(ctxsInt,0,sizeof(ctxsInt));memset(ctxsFloat,0,sizeof(ctxsFloat));
 oldsf=-1;
}

void TaudioFilterResampleLavc::done(void)
{
 for (int i=0;i<8;i++)
  {
   if (ctxsInt[i])
    {
     delete ctxsInt[i];
     ctxsInt[i]=NULL;
    }
   if (ctxsFloat[i])
    {
     delete ctxsFloat[i];
     ctxsFloat[i]=NULL;
    }
  }
}

template<class sample_t,class Tctx> void TaudioFilterResampleLavc::resample(TsampleFormat &fmt,sample_t* &samples,size_t &numsamples,const TresampleSettings *cfg,Tctx ctxs[])
{
 unsigned int srcfreq=fmt.freq;
 size_t lenout=numsamples*cfg->freq/fmt.freq*2;
 sample_t *samples1=(sample_t*)init(cfg,fmt,samples,numsamples);
 fmt.freq=cfg->freq;
 sample_t *samples2=(sample_t*)alloc_buffer(fmt,lenout,buf);

 if (numsamples>0)
  {
   buffIn.alloc(sizeof(sample_t)*numsamples);
   buffOut.alloc(sizeof(sample_t)*lenout);
   int ret=0;
   for (unsigned int ch=0;ch<fmt.nchannels;ch++)
    {
     memset(buffIn,0,numsamples*sizeof(int16_t));memset(buffOut,0,lenout*sizeof(int16_t));
     for (size_t i=0;i<numsamples;i++)
      ((sample_t*)buffIn)[i]=samples1[i*fmt.nchannels+ch];
     ret=ctxs[ch]->audio_resample((sample_t*)buffOut,(sample_t*)buffIn,numsamples);
     for (int j=0;j<ret;j++)
      samples2[j*fmt.nchannels+ch]=((sample_t*)buffOut)[j];
    }
   numsamples=ret;
  }
 samples=samples2;
}
HRESULT TaudioFilterResampleLavc::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TresampleSettings *cfg=(const TresampleSettings*)cfg0;
 if (is(fmt,cfg))
  {
   if (!cfg->equal(old) || oldfreq!=fmt.freq || oldnchannels!=fmt.nchannels || oldsf!=fmt.sf)
    {
     old=*cfg;oldfreq=fmt.freq;oldnchannels=fmt.nchannels;oldsf=fmt.sf;
     done();
     buffIn.clear();buffOut.clear();
     bool libsamplerateok=false;
     int mode=limit(cfg->mode,(int)TresampleSettings::RESAMPLE_LAVC_NORMAL,(int)TresampleSettings::RESAMPLE_LAVC_HIGHEST);
     TsampleFormat fmtOut=fmt;
     getOutputFmt(fmtOut,cfg);
     if (fmtOut.sf==TsampleFormat::SF_PCM16)
      for (unsigned int i=0;i<fmt.nchannels;i++)
       switch (mode)
        {
         case TresampleSettings::RESAMPLE_LAVC_NORMAL :ctxsInt[i]=new TreSampleContext<int16_t>(1,cfg->freq,fmt.freq,16         ,10,0,1.0,15);break;
         case TresampleSettings::RESAMPLE_LAVC_HIGH   :ctxsInt[i]=new TreSampleContext<int16_t>(1,cfg->freq,fmt.freq,16         ,10,1,1.0,22);break;
         case TresampleSettings::RESAMPLE_LAVC_HIGHEST:ctxsInt[i]=new TreSampleContext<int16_t>(1,cfg->freq,fmt.freq,int(16*2.2),10,1,1.0,22);break;
        }
     else
      for (unsigned int i=0;i<fmt.nchannels;i++)
       switch (mode)
        {
         case TresampleSettings::RESAMPLE_LAVC_NORMAL :ctxsFloat[i]=new TreSampleContext<float>(1,cfg->freq,fmt.freq,16         ,10,0,1.0,0);break;
         case TresampleSettings::RESAMPLE_LAVC_HIGH   :ctxsFloat[i]=new TreSampleContext<float>(1,cfg->freq,fmt.freq,16         ,10,1,1.0,0);break;
         case TresampleSettings::RESAMPLE_LAVC_HIGHEST:ctxsFloat[i]=new TreSampleContext<float>(1,cfg->freq,fmt.freq,int(16*2.2),10,1,1.0,0);break;
        }
    }

   if (ctxsInt[0])
    resample(fmt,(int16_t*&)samples,numsamples,cfg,ctxsInt);
   else
    resample(fmt,(float*&)samples,numsamples,cfg,ctxsFloat);
  }
 return parent->deliverSamples(++it,fmt,samples,numsamples);
}

void TaudioFilterResampleLavc::onSeek(void)
{
 // TODO: clear libavcodec resample
}

//=================================== TaudioFilterResampleSRC ====================================
const char_t* TaudioFilterResampleSRC::dllname=_l("ff_samplerate.dll");
TaudioFilterResampleSRC::TaudioFilterResampleSRC(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilterResampleBase(Ideci,Iparent)
{
 dll=new Tdll(dllname,parent->config);
 dll->loadFunction(src_new,"src_new");
 dll->loadFunction(src_delete,"src_delete");
 dll->loadFunction(src_process,"src_process");
 dll->loadFunction(src_reset,"src_reset");
 state=NULL;
}
TaudioFilterResampleSRC::~TaudioFilterResampleSRC()
{
 delete dll;
}
void TaudioFilterResampleSRC::done(void)
{
 if (dll->ok && state) src_delete(state);state=NULL;
}

bool TaudioFilterResampleSRC::is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg)
{
 return dll->ok && super::is(fmt,cfg);
}

HRESULT TaudioFilterResampleSRC::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TresampleSettings *cfg=(const TresampleSettings*)cfg0;
 if (is(fmt,cfg))
  {
   if (!cfg->equal(old) || oldfreq!=fmt.freq || oldnchannels!=fmt.nchannels)
    {
     old=*cfg;oldfreq=fmt.freq;oldnchannels=fmt.nchannels;
     done();
     state=src_new(cfg->mode-TresampleSettings::RESAMPLE_SRC_SINC_BEST_QUALITY,fmt.nchannels,NULL);
    }

   if (state)
    {
     unsigned int srcfreq=fmt.freq;
     size_t lenout=numsamples*cfg->freq/fmt.freq*2;
     int16_t *samples1=(int16_t*)init(cfg,fmt,samples,numsamples);
     fmt.freq=cfg->freq;
     int16_t *samples2=(int16_t*)alloc_buffer(fmt,lenout,buf);

     if (numsamples>0 && state)
      {
       SRC_DATA src;
       src.data_in=(float*)samples1;
       src.input_frames=(long)numsamples;
       src.data_out=(float*)samples2;
       src.output_frames=(long)lenout;
       src.src_ratio=(double)cfg->freq/srcfreq;
       src.end_of_input=0;
       numsamples=0;
       do
        {
         int error=src_process(state,&src);
         if (error) break;
         numsamples+=src.output_frames_gen;
         if (src.end_of_input) break;
         src.data_in+=src.input_frames_used*fmt.nchannels;
         src.input_frames-=src.input_frames_used;
         src.data_out+=src.output_frames_gen*fmt.nchannels;
        } while (src.input_frames>0);
      }
     samples=samples2;
    }
  }
 return parent->deliverSamples(++it,fmt,samples,numsamples);
}

void TaudioFilterResampleSRC::onSeek(void)
{
 if (dll->ok) src_reset(state);
}
