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
#include "TaudioFilterWinamp2.h"
#include "IffdshowDecAudio.h"
#include "winamp2/Twinamp2.h"

TaudioFilterWinamp2::TaudioFilterWinamp2(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 old.flnm[0]=old.modulename[0]='\0';
 winamp2=NULL;filter=NULL;
 old_nchannels=0;
}
void TaudioFilterWinamp2::done(void)
{
 if (filter)
  {
   filter->done();
   filter=NULL;
  }
}

bool TaudioFilterWinamp2::is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg)
{
 return super::is(fmt,cfg);
}

HRESULT TaudioFilterWinamp2::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples0,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 if (!is(fmt,cfg0))
  {
   if (filter)
    {
     done();
     old.flnm[0]=old.modulename[0]='\0';
    }
   return parent->deliverSamples(++it,fmt,samples0,numsamples);
  }

 const Twinamp2settings *cfg=(const Twinamp2settings*)cfg0;
 if (!cfg->equal(old))
  {
   done();
   old=*cfg;
   deciA->getWinamp2(&winamp2);
   filter=winamp2->getFilter(cfg,fmt.nchannels);
   if (filter) filter->init();
  }

 if (old_nchannels!=fmt.nchannels)
  {
   old_nchannels=fmt.nchannels;
   if (filter && fmt.nchannels>2 && !filter->isMultichannelAllowed(cfg->allowMultichannelOnlyIn))
    done();
   if (!filter)
    {
     deciA->getWinamp2(&winamp2);
     filter=winamp2->getFilter(cfg,fmt.nchannels);
     if (filter) filter->init();
    }
  }

 if (filter)
  {
   int16_t *samples1=(int16_t*)(samples0=init(cfg,fmt,samples0,numsamples,numsamples*2));
   numsamples=filter->process(samples1,numsamples,16,fmt.nchannels,fmt.freq);
  }
 return parent->deliverSamples(++it,fmt,samples0,numsamples);
}
