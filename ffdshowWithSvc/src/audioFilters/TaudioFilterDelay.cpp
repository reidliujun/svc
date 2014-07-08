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
#include "TaudioFilterDelay.h"

TaudioFilterDelay::TaudioFilterDelay(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 oldcfg.l=-1;
 oldfmt.freq=0;
}

void TaudioFilterDelay::TdelayBuffer::init(unsigned int ms,const TsampleFormat &Isf)
{
 sf=Isf;
 len=sf.freq*ms/1000;
 if (len)
  {
   Bpp=sf.bitsPerSample()/8;
   buf=realloc(buf,len*Bpp);
   memset(buf,0,len*Bpp);
   pos=0;
  }
}
void TaudioFilterDelay::TdelayBuffer::clear(void)
{
 if (len&Bpp) memset(buf,0,len*Bpp);
 pos=0;
}

HRESULT TaudioFilterDelay::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples0,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TdelaySettings *cfg=(const TdelaySettings*)cfg0;

 if (!cfg->equal(oldcfg) || oldfmt!=fmt)
  {
   oldcfg=*cfg;oldfmt=fmt;
   for (unsigned int i=0;i<fmt.nchannels;i++)
    {
     int ms;
     if (fmt.speakers[i]&SPEAKER_FRONT_LEFT)
      ms=cfg->l;
     else if (fmt.speakers[i]&SPEAKER_FRONT_RIGHT)
      ms=cfg->r;
     else if (fmt.speakers[i]&SPEAKER_FRONT_CENTER)
      ms=cfg->c;
     else if (fmt.speakers[i]&SPEAKER_BACK_LEFT)
      ms=cfg->sl;
     else if (fmt.speakers[i]&SPEAKER_BACK_RIGHT)
      ms=cfg->sr;
     else if (fmt.speakers[i]&SPEAKER_BACK_CENTER)
      ms=cfg->bc;
     else if (fmt.speakers[i]&SPEAKER_LOW_FREQUENCY)
      ms=cfg->lfe;
     else if (fmt.speakers[i]&SPEAKER_SIDE_LEFT)
      ms=cfg->al;
     else if (fmt.speakers[i]&SPEAKER_SIDE_RIGHT)
      ms=cfg->ar;
     else
      ms=0;
     delay[i].init(ms,fmt);
    }
  }

 switch (fmt.sf)
  {
   case TsampleFormat::SF_PCM16:
    for (unsigned int i=0;i<fmt.nchannels;i++)
     delay[i].delay((int16_t*)samples0+i,numsamples);
    break;
   case TsampleFormat::SF_PCM24:
    for (unsigned int i=0;i<fmt.nchannels;i++)
     delay[i].delay((int24_t*)samples0+i,numsamples);
    break;
   case TsampleFormat::SF_PCM32:
    for (unsigned int i=0;i<fmt.nchannels;i++)
     delay[i].delay((int32_t*)samples0+i,numsamples);
    break;
   case TsampleFormat::SF_FLOAT32:
    for (unsigned int i=0;i<fmt.nchannels;i++)
     delay[i].delay((float*)samples0+i,numsamples);
    break;
  }
 return parent->deliverSamples(++it,fmt,samples0,numsamples);
}

void TaudioFilterDelay::onSeek(void)
{
 for (int i=0;i<8;i++)
  delay[i].clear();
}
