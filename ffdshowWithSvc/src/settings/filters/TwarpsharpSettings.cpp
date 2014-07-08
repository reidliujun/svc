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
#include "TwarpsharpSettings.h"
#include "TimgFilterWarpsharp.h"
#include "Cwarpsharp.h"
#include "TffdshowPageDec.h"

const char_t* TwarpsharpSettings::cms[]=
{
 _l("none"),
 _l("downsampled"),
 _l("independent"),
 NULL
};
const char_t* TwarpsharpSettings::bms[]=
{
 _l("high quality 3-pass"),
 _l("fast 3-pass"),
 _l("fast 1-pass"),
 NULL
};

const TfilterIDFF TwarpsharpSettings::idffs=
{
 /*name*/      _l("Warpsharp"),
 /*id*/        IDFF_filterWarpsharp,
 /*is*/        IDFF_isWarpsharp,
 /*order*/     IDFF_orderWarpsharp,
 /*show*/      IDFF_showWarpsharp,
 /*full*/      IDFF_fullWarpsharp,
 /*half*/      IDFF_halfWarpsharp,
 /*dlgId*/     IDD_WARPSHARP,
};

TwarpsharpSettings::TwarpsharpSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TwarpsharpSettings> iopts[]=
  {
   IDFF_isWarpsharp          ,&TwarpsharpSettings::is                ,0,0,_l(""),1,
     _l("isWarpsharp"),0,
   IDFF_showWarpsharp        ,&TwarpsharpSettings::show              ,0,0,_l(""),1,
     _l("showWarpsharp"),1,
   IDFF_orderWarpsharp       ,&TwarpsharpSettings::order             ,1,1,_l(""),1,
     _l("orderWarpsharp"),0,
   IDFF_fullWarpsharp        ,&TwarpsharpSettings::full              ,0,0,_l(""),1,
     _l("fullWarpsharp"),0,
   IDFF_halfWarpsharp        ,&TwarpsharpSettings::half              ,0,0,_l(""),1,
     _l("halfWarpsharp"),0,
   IDFF_warpsharpMethod      ,&TwarpsharpSettings::method            ,0,1,_l(""),1,
     _l("warpsharpMethod"),0,
   IDFF_warpsharpDepth       ,&TwarpsharpSettings::warpsharpDepth    ,0,255,_l(""),1,
     _l("warpsharpDepth"),warpsharpDepthDef,
   IDFF_warpsharpThreshold   ,&TwarpsharpSettings::warpsharpThreshold,0,255,_l(""),1,
     _l("warpsharpThreshold"),warpsharpThresholdDef,
   IDFF_awarpsharpDepth      ,&TwarpsharpSettings::awarpsharpDepth   ,0,6400,_l(""),1,
     _l("awarpsharpDepth"),1600,
   IDFF_awarpsharpThresh     ,&TwarpsharpSettings::awarpsharpThresh  ,0,99,_l(""),1,
     _l("awarpsharpThresh"),50,
   IDFF_awarpsharpBlur       ,&TwarpsharpSettings::awarpsharpBlur    ,0,4,_l(""),1,
     _l("awarpsharpBlur"),2,
   IDFF_awarpsharpCM         ,&TwarpsharpSettings::awarpsharpCM      ,0,2,_l(""),1,
     _l("awarpsharpCM"),0,
   IDFF_awarpsharpBM         ,&TwarpsharpSettings::awarpsharpBM      ,0,2,_l(""),1,
     _l("awarpsharpBM"),2,
   0
  };
 addOptions(iopts);
}

void TwarpsharpSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  {
   if (!queue.temporary) setOnChange(IDFF_warpsharpMethod,filters,&Tfilters::onQueueChange);
   switch (method)
    {
     case WARPSHARP_FF:queueFilter<TimgFilterWarpsharp>(filtersorder,filters,queue); return;
     case WARPSHARP_A:queueFilter<TimgFilterAwarpsharp>(filtersorder,filters,queue); return;
    }
  }
}
void TwarpsharpSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TwarpsharpPage>(&idffs);
}

const int* TwarpsharpSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={
  IDFF_warpsharpMethod,
  IDFF_warpsharpDepth,IDFF_warpsharpThreshold,
  IDFF_awarpsharpDepth,IDFF_awarpsharpThresh,IDFF_awarpsharpBlur,IDFF_awarpsharpCM,IDFF_awarpsharpBM,
  0};
 return idResets;
}

bool TwarpsharpSettings::getTip(unsigned int pageId,char_t *buf,size_t buflen)
{
 switch (method)
  {
   case 0:tsnprintf_s(buf, buflen, _TRUNCATE, _l("original warpsharp - depth:%i, threshold:%i"),warpsharpDepth,warpsharpThreshold);
          break;
   case 1:tsnprintf_s(buf, buflen, _TRUNCATE, _l("aWarpSharp - depth:%g, threshold:%g, %i %s blur, chroma mode:%s"),awarpsharpDepth/100.0f,awarpsharpThresh/100.0f,awarpsharpBlur,bms[awarpsharpBM],cms[awarpsharpCM]);
          break;
  }
 buf[buflen-1]='\0';
 return true;
}
