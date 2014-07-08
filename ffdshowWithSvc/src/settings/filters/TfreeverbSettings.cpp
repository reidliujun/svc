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
#include "TfreeverbSettings.h"
#include "TaudioFilterFreeverb.h"
#include "Cfreeverb.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TfreeverbSettings::idffs=
{
 /*name*/      _l("Freeverb"),
 /*id*/        IDFF_filterFreeverb,
 /*is*/        IDFF_isFreeverb,
 /*order*/     IDFF_orderFreeverb,
 /*show*/      IDFF_showFreeverb,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_FREEVERB,
};

TfreeverbSettings::TfreeverbSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TfreeverbSettings> iopts[]=
  {
   IDFF_isFreeverb      ,&TfreeverbSettings::is      ,0,0,_l(""),1,
     _l("isFreeverb"),0,
   IDFF_showFreeverb    ,&TfreeverbSettings::show    ,0,0,_l(""),1,
     _l("showFreeverb"),1,
   IDFF_orderFreeverb   ,&TfreeverbSettings::order   ,1,1,_l(""),1,
     _l("orderFreeverb"),0,
   IDFF_freeverbRoomsize,&TfreeverbSettings::roomsize,0,1000,_l(""),1,
     _l("freeverbRoomsize"),initialroom,
   IDFF_freeverbDamp    ,&TfreeverbSettings::damp    ,0,1000,_l(""),1,
     _l("freeverbDamp"),initialdamp,
   IDFF_freeverbWet     ,&TfreeverbSettings::wet     ,0,1000,_l(""),1,
     _l("freeverbWet"),initialwet,
   IDFF_freeverbDry     ,&TfreeverbSettings::dry     ,0,1000,_l(""),1,
     _l("freeverbDry"),initialdry,
   IDFF_freeverbWidth   ,&TfreeverbSettings::width   ,0,1000,_l(""),1,
     _l("freeverbWidth"),initialwidth,
   IDFF_freeverbMode    ,&TfreeverbSettings::mode    ,0,1000,_l(""),1,
     _l("freeverbMode"),initialmode,
   0
  };
 addOptions(iopts);
}

void TfreeverbSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TaudioFilterFreeverb>(filtersorder,filters,queue);
}
void TfreeverbSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TfreeverbPage>(&idffs);
}

const int* TfreeverbSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_freeverbRoomsize,IDFF_freeverbDamp,IDFF_freeverbWet,IDFF_freeverbDry,IDFF_freeverbWidth,IDFF_freeverbMode,0};
 return idResets;
}
