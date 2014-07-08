/*
 * Copyright (c) 2003-2006 Milan Cutka
 * implementation (c) 2001 Anders Johansson ajh@atri.curtin.edu.au
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
#include "TaudioFilterEQ.h"

const double TaudioFilterEQ::Q=1.2247449; // Q value for band-pass filters 1.2247=(3/2)^(1/2) gives 4dB suppression @ Fc*2 and Fc/2

TaudioFilterEQ::TaudioFilterEQ(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 old.eq0=INT_MAX;
 oldsf=0;
}

void TaudioFilterEQ::Tequalizer::bp2(float* a, float* b, double fc, double q)
{
 double th=2*M_PI*fc;
 double C=(1-tan(th*q/2))/(1+tan(th*q/2));
 static const double bp2mult=1;

 a[0]=float(bp2mult*(1+C)*cos(th));
 a[1]=float(bp2mult*-1*C);

 b[0]=float(bp2mult*(1.0-C)/2.0);
 b[1]=float(bp2mult*-1.0050);
}
void TaudioFilterEQ::Tequalizer::init(const TeqSettings *cfg,double freq)
{
 reset();
 static const double qmult=1;
 for(int i=0;i<AF_NCH;i++)
  for(int j=0;j<KM;j++)
   {
    float db=(cfg->highdb-cfg->lowdb)*(&cfg->eq0)[j]/200.0f+cfg->lowdb;
    g[i][j]=(float)(qmult*( db2value(db,100)/* pow(10.0,db/(100*20.0))*/ -1.0));
   }
  K=KM;
  while (((&cfg->f0)[K-1]/100.0)>freq/2.3)
   K--;
  for (int k=0;k<K;k++)
   bp2(a[k],b[k],((&cfg->f0)[k]/100.0)/freq,Q);
}
template<class sample_t> float TaudioFilterEQ::Tequalizer::equalize(const float *g,sample_t in,unsigned int ci)
{
 float yt=(float)in;
 for (int k=0;k<K;k++)
  {
   float *wq = this->wq[ci][k];
   float w=yt*b[k][0] + wq[0]*a[k][0] + wq[1]*a[k][1];
   yt+=(w + wq[1]*b[k][1])*g[k];
   wq[1] = wq[0];
   wq[0] = w;
  }
 return yt;
}
template<class sample_t> void TaudioFilterEQ::Tequalizer::process(sample_t *samples,size_t numsamples,unsigned int nchannels)
{
 unsigned int ci=nchannels;
 while (ci--)
  {
   const float *g=this->g[ci];
   sample_t *inout=samples+ci;
   sample_t *end=inout+nchannels*numsamples;
   while (inout<end)
    {
     float yt=equalize(g,*inout,ci);
     *inout=TsampleFormatInfo<sample_t>::limit(yt);
     inout+=nchannels;
    }
  }
}

HRESULT TaudioFilterEQ::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TeqSettings *cfg=(const TeqSettings*)cfg0;

 samples=init(cfg,fmt,samples,numsamples);

 if (!cfg->equal(old))
  {
   old=*cfg;
   eq.init(cfg,fmt.freq);
  }

 if (oldsf!=fmt.sf)
  {
   oldsf=fmt.sf;
   eq.reset();
  }

 switch (fmt.sf)
  {
   case TsampleFormat::SF_PCM16:eq.process((int16_t*)samples,numsamples,fmt.nchannels);break;
   case TsampleFormat::SF_PCM32:eq.process((int32_t*)samples,numsamples,fmt.nchannels);break;
   case TsampleFormat::SF_FLOAT32:eq.process((float*)samples,numsamples,fmt.nchannels);break;
  }
 return parent->deliverSamples(++it,fmt,samples,numsamples);
}

void TaudioFilterEQ::onSeek(void)
{
 eq.reset();
}
