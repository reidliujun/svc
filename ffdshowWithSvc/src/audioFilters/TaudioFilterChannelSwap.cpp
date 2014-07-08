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
#include "TaudioFilterChannelSwap.h"

TaudioFilterChannelSwap::TaudioFilterChannelSwap(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 oldcfg.l=-1;
 oldfmt.freq=0;
 newchannelmask=0;
}

bool TaudioFilterChannelSwap::sortSpeakers(const Tspeaker &spk1,const Tspeaker &spk2)
{
 return spk1.speaker<spk2.speaker;
}

void TaudioFilterChannelSwap::makeMap(const TsampleFormat &fmt,const TchannelSwapSettings *cfg)
{
 oldcfg = *cfg;
 oldfmt = fmt;
 old_nchannels = fmt.nchannels;
 newchannelmask = 0;
 for (unsigned int i = 0 ; i < fmt.nchannels ; i++)
  {
   if (     fmt.speakers[i] == SPEAKER_FRONT_LEFT)
    speakers[i].speaker = cfg->l;
   else if (fmt.speakers[i] == SPEAKER_FRONT_RIGHT)
    speakers[i].speaker = cfg->r;
   else if (fmt.speakers[i] == SPEAKER_FRONT_CENTER)
    speakers[i].speaker = cfg->c;
   else if (fmt.speakers[i] == SPEAKER_BACK_LEFT)
    speakers[i].speaker = cfg->sl;
   else if (fmt.speakers[i] == SPEAKER_BACK_RIGHT)
    speakers[i].speaker = cfg->sr;
   else if (fmt.speakers[i] == SPEAKER_BACK_CENTER)
    speakers[i].speaker = cfg->rear;
   else if (fmt.speakers[i] == SPEAKER_LOW_FREQUENCY)
    speakers[i].speaker = cfg->lfe;
   else if (fmt.speakers[i] == SPEAKER_SIDE_LEFT)
    speakers[i].speaker = cfg->al;
   else if (fmt.speakers[i] == SPEAKER_SIDE_RIGHT)
    speakers[i].speaker = cfg->ar;
   speakers[i].i = i;
   newchannelmask |= speakers[i].speaker;
  }
 std::sort(speakers+0, speakers+fmt.nchannels, sortSpeakers);
}

HRESULT TaudioFilterChannelSwap::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TchannelSwapSettings *cfg=(const TchannelSwapSettings*)cfg0;

 if (!cfg->equal(oldcfg) || oldfmt!=fmt || old_nchannels!=fmt.nchannels)
  makeMap(fmt,cfg);

 fmt.channelmask=newchannelmask;
 switch (fmt.sf)
  {
   case TsampleFormat::SF_PCM16:swapChannels((int16_t*)samples,numsamples);break;
   case TsampleFormat::SF_PCM24:swapChannels((int24_t*)samples,numsamples);break;
   case TsampleFormat::SF_PCM32:swapChannels((int32_t*)samples,numsamples);break;
   case TsampleFormat::SF_FLOAT32:swapChannels((float*)samples,numsamples);break;
  }
 return parent->deliverSamples(++it,fmt,samples,numsamples);
}

bool TaudioFilterChannelSwap::getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg0)
{
 if (super::getOutputFmt(fmt, cfg0))
  {
   makeMap(fmt, (const TchannelSwapSettings*)cfg0);
   fmt.channelmask = newchannelmask;
   return true;
  }
 else
  return false;
}
