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
#include "TchannelSwapSettings.h"
#include "TsampleFormat.h"
#include "TaudioFilterChannelSwap.h"
#include "CchannelSwap.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TchannelSwapSettings::idffs=
{
 /*name*/      _l("Swap channels"),
 /*id*/        IDFF_filterChannelSwap,
 /*is*/        IDFF_isChannelSwap,
 /*order*/     IDFF_orderChannelSwap,
 /*show*/      IDFF_showChannelSwap,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_CHANNELSWAP,
};

TchannelSwapSettings::TchannelSwapSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TchannelSwapSettings> iopts[]=
  {
   IDFF_isChannelSwap     ,&TchannelSwapSettings::is    ,0,0,_l(""),1,
     _l("isChannelSwap"),0,
   IDFF_showChannelSwap   ,&TchannelSwapSettings::show  ,0,0,_l(""),1,
     _l("showChannelSwap"),1,
   IDFF_orderChannelSwap  ,&TchannelSwapSettings::order ,1,1,_l(""),1,
     _l("orderChannelSwap"),0,
   IDFF_channelSwapL      ,&TchannelSwapSettings::l     ,1,1,_l(""),1,
     _l("channelSwapL"),SPEAKER_FRONT_LEFT,
   IDFF_channelSwapR      ,&TchannelSwapSettings::r     ,1,1,_l(""),1,
     _l("channelSwapR"),SPEAKER_FRONT_RIGHT,
   IDFF_channelSwapC      ,&TchannelSwapSettings::c     ,1,1,_l(""),1,
     _l("channelSwapC"),SPEAKER_FRONT_CENTER,
   IDFF_channelSwapSL     ,&TchannelSwapSettings::sl    ,1,1,_l(""),1,
     _l("channelSwapSL"),SPEAKER_BACK_LEFT,
   IDFF_channelSwapSR     ,&TchannelSwapSettings::sr    ,1,1,_l(""),1,
     _l("channelSwapSR"),SPEAKER_BACK_RIGHT,
   IDFF_channelSwapREAR   ,&TchannelSwapSettings::rear  ,1,1,_l(""),1,
     _l("channelSwapRear"),SPEAKER_BACK_CENTER,
   IDFF_channelSwapLFE    ,&TchannelSwapSettings::lfe   ,1,1,_l(""),1,
     _l("channelSwapLFE"),SPEAKER_LOW_FREQUENCY,
   IDFF_channelSwapAL     ,&TchannelSwapSettings::al    ,1,1,_l(""),1,
     _l("channelSwapAL"),SPEAKER_SIDE_LEFT,
   IDFF_channelSwapAR     ,&TchannelSwapSettings::ar    ,1,1,_l(""),1,
     _l("channelSwapAR"),SPEAKER_SIDE_RIGHT,
   0
  };
 addOptions(iopts);
}

void TchannelSwapSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TaudioFilterChannelSwap>(filtersorder,filters,queue);
}
void TchannelSwapSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TchannelSwapPage>(&idffs);
}

const int* TchannelSwapSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_channelSwapL,IDFF_channelSwapC,IDFF_channelSwapR,IDFF_channelSwapSL,IDFF_channelSwapSR,IDFF_channelSwapLFE,IDFF_channelSwapAL,IDFF_channelSwapAR,0};
 return idResets;
}

bool TchannelSwapSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("%s -> %s\n%s -> %s\n%s -> %s\n%s -> %s\n%s -> %s\n%s -> %s\n%s -> %s"),TsampleFormat::getSpeakerName(SPEAKER_FRONT_LEFT),TsampleFormat::getSpeakerName(l),TsampleFormat::getSpeakerName(SPEAKER_FRONT_CENTER),TsampleFormat::getSpeakerName(c),TsampleFormat::getSpeakerName(SPEAKER_FRONT_RIGHT),TsampleFormat::getSpeakerName(r),TsampleFormat::getSpeakerName(SPEAKER_BACK_LEFT),TsampleFormat::getSpeakerName(sl),TsampleFormat::getSpeakerName(SPEAKER_BACK_CENTER),TsampleFormat::getSpeakerName(rear),TsampleFormat::getSpeakerName(SPEAKER_BACK_RIGHT),TsampleFormat::getSpeakerName(sr),TsampleFormat::getSpeakerName(SPEAKER_LOW_FREQUENCY),TsampleFormat::getSpeakerName(lfe));
 return true;
}
