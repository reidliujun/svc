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
#include "TcropSettings.h"
#include "TimgFilterCrop.h"
#include "Ccrop.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TcropSettings::idffs=
{
 /*name*/      _l("Crop"),
 /*id*/        IDFF_filterCropNzoom,
 /*is*/        IDFF_isCropNzoom,
 /*order*/     IDFF_orderCropNzoom,
 /*show*/      IDFF_showCropNzoom,
 /*full*/      IDFF_fullCropNzoom,
 /*half*/      0,
 /*dlgId*/     IDD_CROP,
};

TcropSettings::TcropSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 half=0;
 static const TintOptionT<TcropSettings> iopts[]=
  {
   IDFF_isCropNzoom        ,&TcropSettings::is                 ,0,0,_l(""),1,
     _l("isCropNzoom"),0,
   IDFF_showCropNzoom      ,&TcropSettings::show               ,0,0,_l(""),1,
     _l("showCropNzoom"),1,
   IDFF_orderCropNzoom     ,&TcropSettings::order              ,1,1,_l(""),1,
     _l("orderCropNzoom"),0,
   IDFF_fullCropNzoom      ,&TcropSettings::full               ,1,1,_l(""),1,
     _l("fullCropNzoom"),1,
   IDFF_cropNzoomMode      ,&TcropSettings::mode               ,0,5,_l(""),1,
     _l("isZoom"),0,
   IDFF_magnificationX     ,&TcropSettings::magnificationX     ,0,100,_l(""),1,
     _l("magnificationX"),0,
   IDFF_magnificationY     ,&TcropSettings::magnificationY     ,0,100,_l(""),1,
     _l("magnificationY"),0,
   IDFF_magnificationLocked,&TcropSettings::magnificationLocked,0,0,_l(""),1,
     _l("magnificationLocked"),1,
   IDFF_cropLeft           ,&TcropSettings::cropLeft           ,0,2048,_l(""),1,
     _l("cropLeft"),0,
   IDFF_cropRight          ,&TcropSettings::cropRight          ,0,2048,_l(""),1,
     _l("cropRight"),0,
   IDFF_cropTop            ,&TcropSettings::cropTop            ,0,2048,_l(""),1,
     _l("cropTop"),0,
   IDFF_cropBottom         ,&TcropSettings::cropBottom         ,0,2048,_l(""),1,
     _l("cropBottom"),0,
   IDFF_panscanZoom        ,&TcropSettings::panscanZoom        ,0,100,_l(""),1,
     _l("panscanZoom"),0,
   IDFF_panscanX           ,&TcropSettings::panscanX           ,-100,100,_l(""),1,
     _l("panscanX"),0,
   IDFF_panscanY           ,&TcropSettings::panscanY           ,-100,100,_l(""),1,
     _l("panscanY"),0,
   IDFF_cropTolerance      ,&TcropSettings::cropTolerance      ,0,2048,_l(""),1,
     _l("cropTolerance"),40,
   IDFF_cropRefreshDelay   ,&TcropSettings::cropRefreshDelay   ,0,3600000,_l(""),1,
     _l("cropRefreshDelay"),5000,
   IDFF_cropStopScan       ,&TcropSettings::cropStopScan       ,0,3600000,_l(""),1,
     _l("cropStopScan"),100000,
   0
  };
 addOptions(iopts);
}

void TcropSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  {
   if (!queue.temporary) setOnChange(IDFF_fullCropNzoom,filters,&Tfilters::onQueueChange);
   if (full && (mode < 3 || mode > 5)) // Do expand only if not in AutoCrop (autocrop code is located in TimgFilterCrop class)
    queueFilter<TimgFilterCropExpand>(filtersorder,filters,queue);
   else
    queueFilter<TimgFilterCrop>(filtersorder,filters,queue);
  }
}
void TcropSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TcropPage>(&idffs);
}

const int* TcropSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={
  IDFF_cropNzoomMode,
  IDFF_magnificationX,IDFF_magnificationY,IDFF_magnificationLocked,
  IDFF_cropLeft,IDFF_cropRight,IDFF_cropTop,IDFF_cropBottom,
  IDFF_panscanZoom,IDFF_panscanX,IDFF_panscanY,
  0};
 return idResets;
}

bool TcropSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 switch (mode)
  {
   case 0:if (magnificationLocked)
           tsnprintf_s(tipS, len, _TRUNCATE, _l("zoom: %i%%"), magnificationX);
          else
           tsnprintf_s(tipS, len, _TRUNCATE, _l("horizontal magnification: %i%%, vertical magnification: %i%%"), magnificationX,magnificationY);
          break;
   case 1:tsnprintf_s(tipS, len, _TRUNCATE, _l("crop: left:%i, top:%i, right:%i, bottom:%i"), cropLeft, cropTop, cropRight,cropBottom);
          break;
   case 2:tsnprintf_s(tipS, len, _TRUNCATE, _l("Pan & scan"));
          break;
   default:tipS[0]='\0';
  }
 tipS[len-1]='\0';
 return true;
}
