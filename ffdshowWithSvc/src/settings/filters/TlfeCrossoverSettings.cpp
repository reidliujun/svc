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
#include "TlfeCrossoverSettings.h"
#include "TaudioFilterLFEcrossover.h"
#include "TffdshowPageDec.h"
#include "ClfeCrossover.h"

const TfilterIDFF TlfeCrossoverSettings::idffs=
{
 /*name*/      _l("LFE Crossover"),
 /*id*/        IDFF_filterLFEcrossover,
 /*is*/        IDFF_isLFEcrossover,
 /*order*/     IDFF_orderLFEcrossover,
 /*show*/      IDFF_showLFEcrossover,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_LFECROSSOVER,
};

TlfeCrossoverSettings::TlfeCrossoverSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TlfeCrossoverSettings> iopts[]=
  {
   IDFF_isLFEcrossover    ,&TlfeCrossoverSettings::is    ,0,0,_l(""),1,
     _l("isLFEcrossover"),0,
   IDFF_showLFEcrossover  ,&TlfeCrossoverSettings::show  ,0,0,_l(""),1,
     _l("showLFEcrossover"),1,
   IDFF_orderLFEcrossover ,&TlfeCrossoverSettings::order ,1,1,_l(""),1,
     _l("orderLFEcrossover"),0,
   IDFF_LFEcrossoverFreq  ,&TlfeCrossoverSettings::freq  ,1,300,_l(""),1,
     _l("LFEcrossoverFreq"),180,
   IDFF_LFEcrossoverGain  ,&TlfeCrossoverSettings::gain  ,-1000,1000,_l(""),1,
     _l("LFEcrossoverGain"),-200,
   IDFF_LFEcutLR          ,&TlfeCrossoverSettings::cutLR ,0,0,_l(""),1,
     _l("IDFF_LFEcutLR"),0,
   0
  };
 addOptions(iopts);
}

void TlfeCrossoverSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TaudioFilterLFEcrossover>(filtersorder,filters,queue);
}
void TlfeCrossoverSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TlfeCrossoverPage>(&idffs);
}

const int* TlfeCrossoverSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_LFEcrossoverFreq,IDFF_LFEcrossoverGain,IDFF_LFEcutLR,0};
 return idResets;
}

bool TlfeCrossoverSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("lowpass frequency: %i\ngain: %.2f"), freq, gain/100.0f);
 return true;
}
