/*
 * Copyright (c) 2005,2006 Milan Cutka
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
#include "TconvolverSettings.h"
#include "TaudioFilterConvolver.h"
#include "Cconvolver.h"
#include "TffdshowPageDec.h"
#include "TsampleFormat.h"

const TfilterIDFF TconvolverSettings::idffs=
{
 /*name*/      _l("Convolver"),
 /*id*/        IDFF_filterConvolver,
 /*is*/        IDFF_isConvolver,
 /*order*/     IDFF_orderConvolver,
 /*show*/      IDFF_showConvolver,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_CONVOLVER,
};

TconvolverSettings::TconvolverSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 memset(file,0,sizeof(file));memset(fileL,0,sizeof(file));memset(fileR,0,sizeof(file));memset(fileC,0,sizeof(file));memset(fileLFE,0,sizeof(file));memset(fileSL,0,sizeof(file));memset(fileSR,0,sizeof(file));

 static const TintOptionT<TconvolverSettings> iopts[]=
  {
   IDFF_isConvolver             ,&TconvolverSettings::is             ,0,0,_l(""),1,
     _l("isConvolver"),0,
   IDFF_showConvolver           ,&TconvolverSettings::show           ,0,0,_l(""),1,
     _l("showConvolver"),1,
   IDFF_orderConvolver          ,&TconvolverSettings::order          ,1,1,_l(""),1,
     _l("orderConvolver"),0,
   IDFF_convolverMappingMode    ,&TconvolverSettings::mappingMode    ,0,1,_l(""),1,
     _l("convolverMappingMode"),0,
   IDFF_convolverMixingStrength ,&TconvolverSettings::mixingStrength ,0,100,_l(""),1,
     _l("convolverMixingStrength"),100,
   IDFF_convolverLevelAdjustDB  ,&TconvolverSettings::levelAdjustDB  ,-300,300,_l(""),1,
     _l("convolverLevelAdjustDB"),0,
   IDFF_convolverLevelAdjustAuto,&TconvolverSettings::levelAdjustAuto,0,0,_l(""),1,
     _l("convolverLevelAdjustAuto"),1,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_convolverFile    ,(TstrVal)&TconvolverSettings::file   ,MAX_PATH,0,_l(""),1,
     _l("convolverFile"),_l(""),
   IDFF_convolverFileL   ,(TstrVal)&TconvolverSettings::fileL  ,MAX_PATH,0,_l(""),1,
     _l("convolverFileL"),_l(""),
   IDFF_convolverFileR   ,(TstrVal)&TconvolverSettings::fileR  ,MAX_PATH,0,_l(""),1,
     _l("convolverFileR"),_l(""),
   IDFF_convolverFileC   ,(TstrVal)&TconvolverSettings::fileC  ,MAX_PATH,0,_l(""),1,
     _l("convolverFileC"),_l(""),
   IDFF_convolverFileLFE ,(TstrVal)&TconvolverSettings::fileLFE,MAX_PATH,0,_l(""),1,
     _l("convolverFileLFE"),_l(""),
   IDFF_convolverFileSL  ,(TstrVal)&TconvolverSettings::fileSL ,MAX_PATH,0,_l(""),1,
     _l("convolverFileSL"),_l(""),
   IDFF_convolverFileSR  ,(TstrVal)&TconvolverSettings::fileSR ,MAX_PATH,0,_l(""),1,
     _l("convolverFileSR"),_l(""),
   0
  };
 addOptions(sopts);
}

void TconvolverSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TaudioFilterConvolver>(filtersorder,filters,queue);
}
void TconvolverSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TconvolverPage>(&idffs);
}

const int* TconvolverSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={/*IDFF_convolverFile,*/IDFF_convolverMixingStrength,IDFF_convolverLevelAdjustDB,IDFF_convolverLevelAdjustAuto,IDFF_convolverMappingMode,0};
 return idResets;
}
bool TconvolverSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tipS[0]='\0';
 if (mappingMode==0)
  if (file[0]) tsnprintf_s(tipS, len, _TRUNCATE, _l("Impulse file: %s"), file);
 else
  {
   if (fileL[0]) tsnprintf_s(tipS, len, _TRUNCATE, _l("%s channel impulse file: %s"),TsampleFormat::getSpeakerName(SPEAKER_FRONT_LEFT),fileL);
   if (fileR[0]) strncatf(tipS,len,_l("\n%s channel impulse file: %s"),TsampleFormat::getSpeakerName(SPEAKER_FRONT_RIGHT),fileR);
   if (fileC[0]) strncatf(tipS,len,_l("\n%s channel impulse file: %s"),TsampleFormat::getSpeakerName(SPEAKER_FRONT_CENTER),fileC);
   if (fileLFE[0]) strncatf(tipS,len,_l("\n%s channel impulse file: %s"),TsampleFormat::getSpeakerName(SPEAKER_LOW_FREQUENCY),fileLFE);
   if (fileSL[0]) strncatf(tipS,len,_l("\n%s channel impulse file: %s"),TsampleFormat::getSpeakerName(SPEAKER_BACK_LEFT),fileSL);
   if (fileSR[0]) strncatf(tipS,len,_l("\n%s channel impulse file: %s"),TsampleFormat::getSpeakerName(SPEAKER_BACK_RIGHT),fileSR);
  }
 return true;
}
