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
#include "TaudioFilterFreeverb.h"
#include "freeverb/revmodel.h"

TaudioFilterFreeverb::TaudioFilterFreeverb(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 rev=NULL;
 old.damp=INT_MAX;
}

TaudioFilterFreeverb::~TaudioFilterFreeverb()
{
 if (rev) delete rev;
}

bool TaudioFilterFreeverb::is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg)
{
 return  super::is(fmt,cfg) && fmt.nchannels==2;
}

HRESULT TaudioFilterFreeverb::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples0,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TfreeverbSettings *cfg=(const TfreeverbSettings*)cfg0;

 if (!rev || !cfg->equal(old))
  {
   old=*cfg;
   bool wasfirst;
   if (!rev)
    {
     rev=new revmodel;
     wasfirst=true;
    }
   else wasfirst=false;

   rev->setwet(cfg->wet/1000.0f);
   rev->setroomsize(cfg->roomsize/1000.0f);
   rev->setdry(cfg->dry/1000.0f);
   rev->setdamp(cfg->damp/1000.0f);
   rev->setwidth(cfg->width/1000.0f);
   rev->setmode(cfg->mode/1000.0f);

   if (wasfirst) rev->mute();
  }

 if (is(fmt,cfg))
  {
   float *samples=(float*)(samples0=init(cfg,fmt,samples0,numsamples));
   rev->processreplace(samples,samples+1,(int)numsamples,2);
  }

 return parent->deliverSamples(++it,fmt,samples0,numsamples);
}

void TaudioFilterFreeverb::onSeek(void)
{
 if (rev) rev->mute();
}
