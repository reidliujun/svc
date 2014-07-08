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
#include "ThwOverlaySettings.h"
#include "TimgFilterHWoverlay.h"
#include "Coverlay.h"
#include "TffdshowPageDec.h"

const TfilterIDFF ThwOverlaySettings::idffs=
{
 /*name*/      _l("Overlay controls"),
 /*id*/        IDFF_overlayControl,
 /*is*/        IDFF_isOverlayControl,
 /*order*/     IDFF_overlayOrder,
 /*show*/      0,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_OVERLAY,
};

ThwOverlaySettings::ThwOverlaySettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs,false)
{
 full=-1;
 half=0;
 reset=0;
 order=TconfPageDec::maxOrder+1;
 static const TintOptionT<ThwOverlaySettings> iopts[]=
  {
   IDFF_isOverlayControl   ,&ThwOverlaySettings::is          ,   0,    0,_l(""),1,
     _l("isOverlayControl"),0,
   IDFF_overlayOrder       ,&ThwOverlaySettings::order       ,  -1,   -1,_l(""),0,
     NULL,0,
   IDFF_overlayBrightness  ,&ThwOverlaySettings::brightness  ,  -1,10000,_l(""),1,
     _l("overlayBrightness"),-1,
   IDFF_overlayContrast    ,&ThwOverlaySettings::contrast    ,  -1,20000,_l(""),1,
     _l("overlayContrast"),-1,
   IDFF_overlayHue         ,&ThwOverlaySettings::hue         ,-190,  180,_l(""),1,
     _l("overlayHue"),-190,
   IDFF_overlaySaturation  ,&ThwOverlaySettings::saturation  ,  -1,20000,_l(""),1,
     _l("overlaySaturation"),-1,
   IDFF_overlaySharpness   ,&ThwOverlaySettings::sharpness   ,  -1,   10,_l(""),1,
     _l("overlaySharpness"),-1,
   IDFF_overlayGamma       ,&ThwOverlaySettings::gamma       ,  -1,  500,_l(""),1,
     _l("overlayGamma"),-1,
   IDFF_overlayColorEnable ,&ThwOverlaySettings::colorEnable ,  -1,    0,_l(""),1,
     _l("overlayColorEnable"),-1,
   IDFF_overlayReset       ,&ThwOverlaySettings::reset       ,   0,    0,_l(""),1,
     NULL,0,
   0
  };
 addOptions(iopts);
}

void ThwOverlaySettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 queueFilter<TimgFilterHWoverlay>(filtersorder,filters,queue);
}
void ThwOverlaySettings::createPages(TffdshowPageDec *parent) const
{
 if ((parent->deci->getParam2(IDFF_filterMode)&IDFF_FILTERMODE_VFW)==0)
  parent->addFilterPage<ToverlayPage>(&idffs);
}

const int* ThwOverlaySettings::getResets(unsigned int pageId)
{
 return NULL;
}

bool ThwOverlaySettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tipS[0]='\0';
 return true;
}
