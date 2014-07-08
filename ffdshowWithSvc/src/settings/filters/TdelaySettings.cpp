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
#include "TdelaySettings.h"
#include "TaudioFilterDelay.h"
#include "Cdelay.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TdelaySettings::idffs=
{
 /*name*/      _l("Delay"),
 /*id*/        IDFF_filterDelay,
 /*is*/        IDFF_isDelay,
 /*order*/     IDFF_orderDelay,
 /*show*/      IDFF_showDelay,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_DELAY,
};

TdelaySettings::TdelaySettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TdelaySettings> iopts[]=
  {
   IDFF_isDelay    ,&TdelaySettings::is   ,0,0,_l(""),1,
     _l("isDelay"),0,
   IDFF_showDelay  ,&TdelaySettings::show ,0,0,_l(""),1,
     _l("showDelay"),1,
   IDFF_orderDelay ,&TdelaySettings::order,1,1,_l(""),1,
     _l("orderDelay"),0,
   IDFF_delayL     ,&TdelaySettings::l    ,0,20000,_l(""),1,
     _l("delayL"),0,
   IDFF_delayC     ,&TdelaySettings::c    ,0,20000,_l(""),1,
     _l("delayC"),0,
   IDFF_delayR     ,&TdelaySettings::r    ,0,20000,_l(""),1,
     _l("delayR"),0,
   IDFF_delaySL    ,&TdelaySettings::sl   ,0,20000,_l(""),1,
     _l("delaySL"),0,
   IDFF_delaySR    ,&TdelaySettings::sr   ,0,20000,_l(""),1,
     _l("delaySR"),0,
   IDFF_delayBC    ,&TdelaySettings::bc   ,0,20000,_l(""),1,
     _l("delayBC"),0,
   IDFF_delayLFE   ,&TdelaySettings::lfe  ,0,20000,_l(""),1,
     _l("delayLFE"),0,
   IDFF_delayAL    ,&TdelaySettings::al   ,0,20000,_l(""),1,
     _l("delayAL"),0,
   IDFF_delayAR    ,&TdelaySettings::ar   ,0,20000,_l(""),1,
     _l("delayAR"),0,
   0
  };
 addOptions(iopts);
}

void TdelaySettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TaudioFilterDelay>(filtersorder,filters,queue);
}
void TdelaySettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TdelayPage>(&idffs);
}

const int* TdelaySettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_delayL,IDFF_delayC,IDFF_delayR,IDFF_delaySL,IDFF_delaySR,IDFF_delayLFE,IDFF_delayAL,IDFF_delayAR,0};
 return idResets;
}

bool TdelaySettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("delays: L:%ims, C:%ims, R:%ims, SL:%ims, SR:%ims, LFE:%ims"), l, c, r, sl, sr, lfe);
 return true;
}
