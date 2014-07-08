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
#include "TpictPropSettings.h"
#include "TimgFilterLuma.h"
#include "TimgFilterChroma.h"
#include "TimgFilterColorize.h"
#include "CpictProp.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TpictPropSettings::idffs=
{
 /*name*/      _l("Picture properties"),
 /*id*/        IDFF_filterPictProp,
 /*is*/        IDFF_isPictProp,
 /*order*/     IDFF_orderPictProp,
 /*show*/      IDFF_showPictProp,
 /*full*/      IDFF_fullPictProp,
 /*half*/      IDFF_halfPictProp,
 /*dlgId*/     IDD_PICTPROP,
};

TpictPropSettings::TpictPropSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TpictPropSettings> iopts[]=
  {
   IDFF_isPictProp          ,&TpictPropSettings::is                ,0,0,_l(""),1,
     _l("isPictProp"),0,
   IDFF_showPictProp        ,&TpictPropSettings::show              ,0,0,_l(""),1,
     _l("showPictProp"),1,
   IDFF_orderPictProp       ,&TpictPropSettings::order             ,1,1,_l(""),1,
     _l("orderPictProp"),0,
   IDFF_fullPictProp        ,&TpictPropSettings::full              ,0,0,_l(""),1,
     _l("fullPictProp"),0,
   IDFF_halfPictProp        ,&TpictPropSettings::half              ,0,0,_l(""),1,
     _l("halfPictProp"),0,
   IDFF_lumGain             ,&TpictPropSettings::lumGain           ,0,256,_l(""),1,
     _l("lumGain"),lumGainDef,
   IDFF_lumOffset           ,&TpictPropSettings::lumOffset         ,-256,256,_l(""),1,
     _l("lumOffset"),lumOffsetDef,
   IDFF_gammaCorrection     ,&TpictPropSettings::gammaCorrection   ,1,400,_l(""),1,
     _l("gammaCorrection"),gammaCorrectionDef,
   IDFF_gammaCorrectionR    ,&TpictPropSettings::gammaCorrectionR  ,1,400,_l(""),1,
     _l("gammaCorrectionR"),gammaCorrectionDef,
   IDFF_gammaCorrectionG    ,&TpictPropSettings::gammaCorrectionG  ,1,400,_l(""),1,
     _l("gammaCorrectionG"),gammaCorrectionDef,
   IDFF_gammaCorrectionB    ,&TpictPropSettings::gammaCorrectionB  ,1,400,_l(""),1,
     _l("gammaCorrectionB"),gammaCorrectionDef,
   IDFF_hue                 ,&TpictPropSettings::hue               ,-180,180,_l(""),1,
     _l("hue"),hueDef,
   IDFF_saturation          ,&TpictPropSettings::saturation        ,0,128,_l(""),1,
     _l("saturation"),saturationDef,
   IDFF_colorizeStrength    ,&TpictPropSettings::colorizeStrength  ,0,255,_l(""),1,
     _l("colorizeStrength"),0,
   IDFF_colorizeColor       ,&TpictPropSettings::colorizeColor     ,1,1,_l(""),1,
     _l("colorizeColor"),RGB(255,255,255),
   IDFF_colorizeChromaonly  ,&TpictPropSettings::colorizeChromaonly,0,0,_l(""),1,
     _l("colorizeChromaonly"),1,
   IDFF_pictPropLevelFix    ,&TpictPropSettings::levelfix          ,0,0,_l(""),1,
     _l("pictProcLevelFix"),0,
   IDFF_pictPropLevelFixFull,&TpictPropSettings::levelfixFull      ,0,0,_l(""),1,
     _l("pictProcLevelFixFull"),0,
   IDFF_scanlineEffect      ,&TpictPropSettings::scanlineEffect    ,0,4,_l(""),1,
     _l("scanlineEffect"),0,
   0
  };
 addOptions(iopts);
}

void TpictPropSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  {
   if (!queue.temporary) setOnChange(IDFF_pictPropLevelFix,filters,&Tfilters::onQueueChange);
   queueFilter<TimgFilterLuma>(filtersorder,filters,queue);
   queueFilter<TimgFilterGammaRGB>(filtersorder,filters,queue);
   queueFilter<TimgFilterChroma>(filtersorder,filters,queue);
   queueFilter<TimgFilterColorize>(filtersorder,filters,queue);
   if (levelfix) queueFilter<TimgFilterLevelFix>(filtersorder,filters,queue);
  }
}
void TpictPropSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TpictPropPage>(&idffs);
}

const int* TpictPropSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={
  IDFF_lumGain,IDFF_lumOffset,IDFF_scanlineEffect,
  IDFF_gammaCorrection,IDFF_gammaCorrectionR,IDFF_gammaCorrectionG,IDFF_gammaCorrectionB,
  IDFF_hue,IDFF_saturation,
  IDFF_colorizeStrength,IDFF_colorizeColor,IDFF_colorizeChromaonly,
  IDFF_pictPropLevelFix,IDFF_pictPropLevelFixFull,
  0};
 return idResets;
}

bool TpictPropSettings::getTip(unsigned int pageId,char_t *buf,size_t buflen)
{
 tsnprintf_s(buf, buflen, _TRUNCATE, _l("luma gain:%i, luma offset:%i, gamma:%5.2f, RGB gamma:%.2f,%.2f,%.2f, hue:%i, saturation:%i, colorize strength:%i%s"),lumGain,lumOffset,gammaCorrection/100.0f,gammaCorrectionR/100.0f,gammaCorrectionG/100.0f,gammaCorrectionB/100.0f,hue,saturation,colorizeStrength,levelfix?_l(", luma level fix"):_l(""));
 return true;
}
