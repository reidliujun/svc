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
#include "TvolumeSettings.h"
#include "TaudioFilterVolume.h"
#include "Cvolume.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TvolumeSettings::idffs=
{
 /*name*/      _l("Volume"),
 /*id*/        IDFF_filterVolume,
 /*is*/        IDFF_isVolume,
 /*order*/     IDFF_orderVolume,
 /*show*/      IDFF_showVolume,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_VOLUME,
};

TvolumeSettings::TvolumeSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TvolumeSettings> iopts[]=
  {
   IDFF_isVolume            ,&TvolumeSettings::is          ,0,0,_l(""),1,
     _l("isVolume"),0,
   IDFF_showVolume          ,&TvolumeSettings::show        ,0,0,_l(""),1,
     _l("showVolume"),1,
   IDFF_orderVolume         ,&TvolumeSettings::order       ,1,1,_l(""),1,
     _l("orderVolume"),0,
   IDFF_volume              ,&TvolumeSettings::vol         ,0,3200,_l(""),1,
     _l("volume"),100,
   IDFF_volumeL             ,&TvolumeSettings::volL        ,0,3200,_l(""),1,
     _l("volumeL"),100,
   IDFF_volumeC             ,&TvolumeSettings::volC        ,0,3200,_l(""),1,
     _l("volumeC"),100,
   IDFF_volumeR             ,&TvolumeSettings::volR        ,0,3200,_l(""),1,
     _l("volumeR"),100,
   IDFF_volumeSL            ,&TvolumeSettings::volSL       ,0,3200,_l(""),1,
     _l("volumeSL"),100,
   IDFF_volumeSR            ,&TvolumeSettings::volSR       ,0,3200,_l(""),1,
     _l("volumeSR"),100,
   IDFF_volumeAL            ,&TvolumeSettings::volAL       ,0,3200,_l(""),1,
     _l("volumeAL"),100,
   IDFF_volumeAR            ,&TvolumeSettings::volAR       ,0,3200,_l(""),1,
     _l("volumeAR"),100,
   IDFF_volumeLFE           ,&TvolumeSettings::volLFE      ,0,3200,_l(""),1,
     _l("volumeLFE"),100,

   IDFF_volumeLmute         ,&TvolumeSettings::volLmute    ,0,2,_l(""),1,
     _l("volumeLmute"),0,
   IDFF_volumeCmute         ,&TvolumeSettings::volCmute    ,0,2,_l(""),1,
     _l("volumeCmute"),0,
   IDFF_volumeRmute         ,&TvolumeSettings::volRmute    ,0,2,_l(""),1,
     _l("volumeRmute"),0,
   IDFF_volumeSLmute        ,&TvolumeSettings::volSLmute   ,0,2,_l(""),1,
     _l("volumeSLmute"),0,
   IDFF_volumeSRmute        ,&TvolumeSettings::volSRmute   ,0,2,_l(""),1,
     _l("volumeSRmute"),0,
   IDFF_volumeALmute        ,&TvolumeSettings::volALmute   ,0,2,_l(""),1,
     _l("volumeALmute"),0,
   IDFF_volumeARmute        ,&TvolumeSettings::volARmute   ,0,2,_l(""),1,
     _l("volumeARmute"),0,
   IDFF_volumeLFEmute       ,&TvolumeSettings::volLFEmute  ,0,2,_l(""),1,
     _l("volumeLFEmute"),0,

   IDFF_volumeNormalize     ,&TvolumeSettings::normalize   ,0,0,_l(""),1,
     _l("volNormalize"),0,
   IDFF_maxNormalization    ,&TvolumeSettings::normalizeMax,0,3200,_l(""),1,
     _l("volNormalizeMax"),400,
   IDFF_volumeNormalizeResetOnSeek     ,&TvolumeSettings::normalizeResetOnSeek,0,0,_l(""),1,
     _l("volNormalizeResetOnSeek"),0,
   IDFF_volumeNormalizeRegainVolume     ,&TvolumeSettings::normalizeRegainVolume,0,0,_l(""),1,
     _l("volNormalizeRegainVolume"),0,
   0
  };
 addOptions(iopts);
}

void TvolumeSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (show)
  queueFilter<TaudioFilterVolume>(filtersorder,filters,queue);
}
void TvolumeSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TvolumePage>(&idffs);
}

const int* TvolumeSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={
  IDFF_volume,
  IDFF_volumeL,IDFF_volumeC,IDFF_volumeR,IDFF_volumeSL,IDFF_volumeSR,IDFF_volumeAL,IDFF_volumeAR,IDFF_volumeLFE,
  IDFF_volumeNormalize,IDFF_maxNormalization,IDFF_volumeNormalizeResetOnSeek,
  0};
 return idResets;
}

bool TvolumeSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 static const char_t *ms[]={_l(""),_l("(muted) "),_l("(solo) ")};
 tsnprintf_s(tipS, len, _TRUNCATE, _l("volume: %i%% (L:%i%%%s, C:%i%%%s, R:%i%%%s, SL:%i%%%s, SR:%i%%%s, LFE:%i%%%s)%s"),vol,volL,ms[volLmute],volC,ms[volCmute],volR,ms[volRmute],volSL,ms[volSLmute],volSR,ms[volSRmute],volLFE,ms[volLFEmute],normalize?_l(", normalizing"):_l(""));
 return true;
}
