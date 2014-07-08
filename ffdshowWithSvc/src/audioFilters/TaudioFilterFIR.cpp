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
#include "TaudioFilterFIR.h"
#include "IffdshowBase.h"
#include "IffdshowDecAudio.h"
#include "fftsg.h"

TaudioFilterFir::TaudioFilterFir(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 old.type=-1;
 ip[0]=0;
 oldfreq=oldchannels=0;
 fir=NULL;
 memset(filterbuf,0,sizeof(filterbuf));
 tapes=0;
}

bool TaudioFilterFir::is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg)
{
 return !!cfg->is;
}

void TaudioFilterFir::done(void)
{
 if (fir) aligned_free(fir);fir=NULL;
 for (int i=0;i<8;i++)
  if (filterbuf[i])
   {
    delete []filterbuf[i];
    filterbuf[i]=NULL;
   }
}

HRESULT TaudioFilterFir::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples0,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 float *samples=NULL;
 const TfirSettings *cfg=(const TfirSettings*)cfg0;
 if (is(fmt,cfg))
  {
   samples=(float*)(samples0=init(cfg,fmt,samples0,numsamples));
   if (!cfg->equal(old) || oldfreq!=fmt.freq || oldchannels!=fmt.nchannels)
    {
     done();
     old=*cfg;oldfreq=fmt.freq;oldchannels=fmt.nchannels;
     TfirFilter::_ftype_t freq[2];
     TfirFilter::_ftype_t allfreq=fmt.freq/2.0f;
     if (cfg->type==TfirSettings::BANDPASS || cfg->type==TfirSettings::BANDSTOP)
      {
       freq[0]=(cfg->freq-cfg->width/2)/allfreq;
       freq[1]=(cfg->freq+cfg->width/2)/allfreq;
      }
     else
      freq[0]=cfg->freq/allfreq;
     tapes=cfg->taps;
     fir=TfirFilter::design_fir(&tapes,freq,cfg->type,cfg->window,cfg->kaiserbeta/1000.0f);
     for (unsigned int i=0;i<fmt.nchannels;i++)
      {
       filterbuf[i]=new float[tapes];
       memset(filterbuf[i],0,sizeof(float)*tapes);
      }
     filterpos=0;
    }

   unsigned int pos=0;
   for (unsigned int ch=0;ch<fmt.nchannels;ch++)
    {
     float *sch=samples+ch;
     pos=filterpos;
     for (unsigned int i=0;i<numsamples;i++,sch+=fmt.nchannels)
      {
       filterbuf[ch][pos]=*sch;
       *sch=TfirFilter::firfilter(filterbuf[ch],pos,tapes,tapes,fir);
       pos++;if (pos==tapes) pos=0;
      }
    }
   filterpos=pos;
  }

 if (numsamples>1)
  {
   storedfft.freq=fmt.freq;
   if (deci->getParam2(IDFF_showCurrentFFT) && deci->getCfgDlgHwnd())
    {
     //float *samples=_alloca(numsamples*fmt.bitsPerSample()>>3);
     TsampleFormat oldfmt=fmt;
     if (!samples) samples=(float*)init(cfg,fmt,samples0,numsamples);
     fmt=oldfmt;
     int num=1<<av_log2((unsigned int)numsamples);
     float *fftsamples=(float*)_alloca(num*sizeof(float));
     memset(fftsamples,0,num*sizeof(float));
     for (unsigned int ch=0;ch<fmt.nchannels;ch++)
      {
       float *sch=samples+ch;
       for (int i=0;i<num;i++,sch+=fmt.nchannels)
        fftsamples[i]+=*sch;
      }
     for (int i=0;i<num;i++)
      fftsamples[i]/=fmt.nchannels;
     rdft(num,1,fftsamples,ip,w);
     //ddct(num,-1,fftsamples,ip,w);
     for (int i=0;i<num/2;i++)
      fftsamples[i]=(float)_hypot(fftsamples[2*i+2],fftsamples[2*i+2+1]);
     fftsamples[num/2-1]/=4;
     CAutoLock lock(&csFFT);
     num/=2;
     if (storedfft.datalen<num*sizeof(float))
      storedfft.data=(float*)realloc(storedfft.data,storedfft.datalen=num*sizeof(float));
     storedfft.num=num;
     memcpy(storedfft.data,fftsamples,num*sizeof(float));
     storedfft.have=true;
    }
  }

 return parent->deliverSamples(++it,fmt,samples0,numsamples);
}

void TaudioFilterFir::onSeek(void)
{
 if (tapes)
  {
   for (unsigned int i=0;i<8;i++)
    if (filterbuf[i])
     memset(filterbuf[i],0,sizeof(float)*tapes);
   filterpos=0;
  }
}

HRESULT TaudioFilterFir::queryInterface(const IID &iid,void **ptr) const
{
 if (iid==IID_IaudioFilterFir) {*ptr=(IaudioFilterFir*)this;return S_OK;}
 else return E_NOINTERFACE;
}

STDMETHODIMP_(int) TaudioFilterFir::getFFTdataNum(void)
{
 return storedfft.have?storedfft.num:0;
}
STDMETHODIMP TaudioFilterFir::getFFTdata(float *fftdata)
{
 if (!fftdata) return E_INVALIDARG;
 if (!storedfft.have) return E_UNEXPECTED;
 CAutoLock lock(&csFFT);
 memcpy(fftdata,storedfft.data,storedfft.num*sizeof(float));
 return S_OK;
}
STDMETHODIMP_(int) TaudioFilterFir::getFFTfreq(void)
{
 return storedfft.freq;
}
