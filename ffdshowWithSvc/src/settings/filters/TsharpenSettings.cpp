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
#include "TsharpenSettings.h"
#include "TimgFilterSharpen.h"
#include "Csharpen.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TsharpenSettings::idffs=
{
 /*name*/      _l("Sharpen"),
 /*id*/        IDFF_filterSharpen,
 /*is*/        IDFF_isSharpen,
 /*order*/     IDFF_orderSharpen,
 /*show*/      IDFF_showSharpen,
 /*full*/      IDFF_fullSharpen,
 /*half*/      IDFF_halfSharpen,
 /*dlgId*/     IDD_SHARPEN,
};

TsharpenSettings::TsharpenSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TsharpenSettings> iopts[]=
  {
   IDFF_isSharpen            ,&TsharpenSettings::is              ,0,0,_l(""),1,
     _l("xsharpen"),0,
   IDFF_showSharpen          ,&TsharpenSettings::show            ,0,0,_l(""),1,
     _l("showSharpen"),1,
   IDFF_orderSharpen         ,&TsharpenSettings::order           ,1,1,_l(""),1,
     _l("orderSharpen"),0,
   IDFF_fullSharpen          ,&TsharpenSettings::full            ,0,0,_l(""),1,
     _l("fullSharpen"),0,
   IDFF_halfSharpen          ,&TsharpenSettings::half            ,0,0,_l(""),1,
     _l("halfSharpen"),0,
   IDFF_sharpenMethod        ,&TsharpenSettings::method          ,0,5,_l(""),1,
     _l("sharpenMethod"),0,
   IDFF_xsharp_strength      ,&TsharpenSettings::xsharpStrength  ,1,127,_l(""),1,
     _l("xsharp_strenght"),xsharpStrengthDef,
   IDFF_xsharp_threshold     ,&TsharpenSettings::xsharpThreshold ,0,255,_l(""),1,
     _l("xsharp_threshold"),xsharpThresholdDef,
   IDFF_unsharp_strength     ,&TsharpenSettings::unsharpStrength ,1,127,_l(""),1,
     _l("unsharp_strength"),unsharpStrengthDef,
   IDFF_unsharp_threshold    ,&TsharpenSettings::unsharpThreshold,0,255,_l(""),1,
     _l("unsharp_threshold"),unsharpThresholdDef,
   IDFF_msharp_strength      ,&TsharpenSettings::msharpStrength  ,0,255,_l(""),1,
     _l("msharpStrength"),msharpStrengthDef,
   IDFF_msharp_threshold     ,&TsharpenSettings::msharpThreshold ,0,255,_l(""),1,
     _l("msharpThreshold"),msharpThresholdDef,
   IDFF_msharpHQ             ,&TsharpenSettings::msharpHQ        ,0,0,_l(""),1,
     _l("msharpHQ"),0,
   IDFF_msharpMask           ,&TsharpenSettings::msharpMask      ,0,0,_l(""),1,
     _l("msharpMask"),0,
   IDFF_asharpT              ,&TsharpenSettings::asharpT         ,-100,3200,_l(""),1,
     _l("asharpT"),200,
   IDFF_asharpD              ,&TsharpenSettings::asharpD         ,0,1600,_l(""),1,
     _l("asharpD"),400,
   IDFF_asharpB              ,&TsharpenSettings::asharpB         ,0,400,_l(""),1,
     _l("asharpB"),0,
   IDFF_asharpHQBF           ,&TsharpenSettings::asharpHQBF      ,0,0,_l(""),1,
     _l("asharpHQBF"),0,
   IDFF_mplayerSharpLuma     ,&TsharpenSettings::mplayerLuma     ,0,400,_l(""),1,
     _l("mplayerSharpLuma"),50,
   IDFF_mplayerSharpChroma   ,&TsharpenSettings::mplayerChroma   ,0,400,_l(""),1,
     _l("mplayerSharpChroma"),50,
   0
  };
 addOptions(iopts);
}

void TsharpenSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  {
   if (!queue.temporary) setOnChange(IDFF_sharpenMethod,filters,&Tfilters::onQueueChange);
   switch (method)
    {
     case SHARPEN_XSHARP:queueFilter<TimgFilterXsharp>(filtersorder,filters,queue); return;
     case SHARPEN_UNSHARP:queueFilter<TimgFilterUnsharp>(filtersorder,filters,queue); return;
     case SHARPEN_MSHARP:queueFilter<TimgFilterMsharp>(filtersorder,filters,queue); return;
     case SHARPEN_ASHARP:queueFilter<TimgFilterAsharp>(filtersorder,filters,queue); return;
     case SHARPEN_MPLAYER:queueFilter<TimgFilterMplayerSharp>(filtersorder,filters,queue); return;
    }
  }
}
void TsharpenSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TsharpenPage>(&idffs);
}

const int* TsharpenSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={
  IDFF_sharpenMethod,
  IDFF_xsharp_strength,IDFF_xsharp_threshold,
  IDFF_unsharp_strength,IDFF_unsharp_threshold,
  IDFF_msharp_strength,IDFF_msharp_threshold,IDFF_msharpHQ,IDFF_msharpMask,
  IDFF_asharpT,IDFF_asharpD,IDFF_asharpB,IDFF_asharpHQBF,
  IDFF_mplayerSharpLuma,IDFF_mplayerSharpChroma,
  0};
 return idResets;
}

bool TsharpenSettings::getTip(unsigned int pageId,char_t *buf,size_t len)
{
 char_t tipS[256];
 const char_t *name=_l("");
 const char_t *capts[3]={NULL,NULL,NULL};
 int vals[3];
 int strength=0,threshold=0;
 switch (method)
  {
   case 0:name=_l("xsharpen");capts[0]=_l("strength");capts[1]=_l("threshold");
          vals[0]=xsharpStrength;
          vals[1]=xsharpThreshold;
          break;
   case 1:name=_l("unsharp mask");capts[0]=_l("strength");
          vals[0]=unsharpStrength;
          break;
   case 2:name=_l("msharpen");capts[0]=_l("strength");capts[1]=_l("threshold");
          vals[0]=msharpStrength;
          vals[1]=msharpThreshold;
          break;
   case 4:name=_l("asharp");capts[0]=_l("Unsharp masking threshold");capts[1]=_l("Adaptive sharpenning strength");capts[2]=_l("Block adaptive sharpenning");
          vals[0]=asharpT;
          vals[1]=asharpD;
          vals[2]=asharpB;
          break;
   case 5:name=_l("swscaler");capts[0]=_l("Luma sharpening");capts[1]=_l("Chroma sharpening");
          vals[0]=mplayerLuma;
          vals[1]=mplayerChroma;
          break;
  }
 tsnprintf_s(tipS, countof(tipS), _TRUNCATE, _l("method:%s\n"), name);
 if (capts[0]) strncatf(tipS, countof(tipS), _l("%s: %i"),capts[0],vals[0]);
 if (capts[1]) strncatf(tipS, countof(tipS), _l(", %s: %i"),capts[1],vals[1]);
 if (capts[2]) strncatf(tipS, countof(tipS), _l(", %s: %i"),capts[2],vals[2]);

 ff_strncpy(buf,tipS,len);
 return true;
}

void TsharpenSettings::reg_op(TregOp &t)
{
 TfilterSettingsVideo::reg_op(t);
 if (method==3) method=4;
}
