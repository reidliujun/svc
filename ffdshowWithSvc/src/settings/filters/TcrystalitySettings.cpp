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
#include "TcrystalitySettings.h"
#include "TaudioFilterCrystality.h"
#include "Ccrystality.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TcrystalitySettings::idffs=
{
 /*name*/      _l("Crystality"),
 /*id*/        IDFF_filterCrystality,
 /*is*/        IDFF_isCrystality,
 /*order*/     IDFF_orderCrystality,
 /*show*/      IDFF_showCrystality,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_CRYSTALITY,
};

TcrystalitySettings::TcrystalitySettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TcrystalitySettings> iopts[]=
  {
   IDFF_isCrystality              ,&TcrystalitySettings::is             ,0,0,_l(""),1,
     _l("isCrystality"),0,
   IDFF_showCrystality            ,&TcrystalitySettings::show           ,0,0,_l(""),1,
     _l("showCrystality"),1,
   IDFF_orderCrystality           ,&TcrystalitySettings::order          ,1,1,_l(""),1,
     _l("orderCrystality"),0,
   IDFF_crystality_bext_level     ,&TcrystalitySettings::bext_level     ,0,100,_l(""),1,
     _l("bext_level"),28,
   IDFF_crystality_echo_level     ,&TcrystalitySettings::echo_level     ,0,100,_l(""),1,
     _l("echo_level"),11,
   IDFF_crystality_stereo_level   ,&TcrystalitySettings::stereo_level   ,0,100,_l(""),1,
     _l("stereo_level"),11,
   IDFF_crystality_filter_level   ,&TcrystalitySettings::filter_level   ,1,  3,_l(""),1,
     _l("filter_level"),3,
   IDFF_crystality_feedback_level ,&TcrystalitySettings::feedback_level ,0,100,_l(""),1,
     _l("feedback_level"),30,
   IDFF_crystality_harmonics_level,&TcrystalitySettings::harmonics_level,0,100,_l(""),1,
     _l("harmonics_level"),43,
   0
  };
 addOptions(iopts);
}

void TcrystalitySettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TaudioFilterCrystality>(filtersorder,filters,queue);
}
void TcrystalitySettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TcrystalityPage>(&idffs);
}

const int* TcrystalitySettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_crystality_bext_level,IDFF_crystality_echo_level,IDFF_crystality_stereo_level,IDFF_crystality_filter_level,IDFF_crystality_feedback_level,IDFF_crystality_harmonics_level,0};
 return idResets;
}

bool TcrystalitySettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("Bandwidth extender: %i\nEcho level: %i\nExtra stereo: %i"), bext_level, echo_level, stereo_level);
 return true;
}
