/*
 * Copyright (c) 2004-2006 Milan Cutka
 * uses code from supereq Avisynth plugin
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
#include "TaudioFilterEQsuper.h"
#include "supereq.h"

TaudioFilterEQsuper::TaudioFilterEQsuper(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 old.eq0=INT_MAX;oldnchannels=0;
 memset(eqs,0,sizeof(eqs));
}
TaudioFilterEQsuper::~TaudioFilterEQsuper()
{
 for (int i=0;i<8;i++) if (eqs[i]) delete eqs[i];
}

HRESULT TaudioFilterEQsuper::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TeqSettings *cfg=(const TeqSettings*)cfg0;

 if (!eqs[0] || oldnchannels!=fmt.nchannels || !cfg->equal(old))
  {
   old=*cfg;oldnchannels=fmt.nchannels;
   float bands[supereq::NBANDS+1];
   for (unsigned int b=0;b<=supereq::NBANDS;b++)
    {
     float db=(cfg->highdb-cfg->lowdb)*(&cfg->eq0)[b]/200.0f+cfg->lowdb;
     bands[b]=db2value(db,100);// pow(10.0,db/(100*20.0));
    }
   for(unsigned int ch=0;ch<fmt.nchannels;ch++)
    {
     if (eqs[ch]) delete eqs[ch];
     eqs[ch]=new supereq;
     eqs[ch]->equ_makeTable(bands,(float)fmt.freq,&cfg->f0);
    }
  }

 float *in=(float*)init(cfg,fmt,samples,numsamples),*out=NULL;
 int samples_out=0;
 for (unsigned int ch=0;ch<fmt.nchannels;ch++)
  {
   eqs[ch]->write_samples(in+ch,(int)numsamples,fmt.nchannels);
   const float *eqout=eqs[ch]->get_output(&samples_out);
   if (!out) out=(float*)alloc_buffer(fmt,samples_out,buf);
   for (int i=0;i<samples_out;i++)
    out[ch+i*fmt.nchannels]=eqout[i];
  }
 return parent->deliverSamples(++it,fmt,out,samples_out);
}

void TaudioFilterEQsuper::onSeek(void)
{
 for (int i=0;i<8;i++)
  if (eqs[i])
   {
    delete eqs[i];
    eqs[i]=NULL;
   }
}
