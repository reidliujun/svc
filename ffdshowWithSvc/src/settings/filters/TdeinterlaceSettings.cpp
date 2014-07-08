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
#include "TdeinterlaceSettings.h"
#include "postproc/postprocFilters.h"
#include "CdeinterlaceTomsMoComp.h"
#include "CdeinterlaceFramedoubler.h"
#include "CdeinterlaceKernel.h"
#include "CdeinterlaceDGBob.h"
#include "CdeinterlaceYadif.h"
#include "TimgFilterDeinterlace.h"
#include "TimgFilterDScalerDeinterlace.h"
#include "TimgFilterTomsMoComp.h"
#include "TimgFilterKernelDeint.h"
#include "TimgFilterDGbob.h"
#include "TimgFilterYadif.h"
#include "Cdeinterlace.h"
#include "TffdshowPageDec.h"

const TdeinterlaceSettings::TmethodProps TdeinterlaceSettings::methodProps[]=
{
 12,_l("Bypass")              ,BYPASS                   ,NULL,
  0,_l("Linear interpolation"),LINEAR_IPOL_DEINT_FILTER ,NULL,
  1,_l("Linear blending")     ,LINEAR_BLEND_DEINT_FILTER,NULL,
  2,_l("Cubic interpolation") ,CUBIC_IPOL_DEINT_FILTER  ,NULL,
  //3,_l("Cubic blending")      ,CUBIC_BLEND_DEINT_FILTER ,NULL,
  4,_l("Median")              ,MEDIAN_DEINT_FILTER      ,NULL,
  5,_l("TomsMoComp")          ,TOMSMOCOMP_DEINT_FILTER  ,TdeinterlacePageTomsMoComp::create,
  6,_l("DGBob")               ,DGBOB_DEINT_FILTER       ,TdeinterlacePageDGbob::create,
  //7,_l("Framerate doubler")   ,FRAMERATEDOUBLER         ,TdeinterlacePageFramedoubler::create,
  8,_l("ffmpeg deinterlacer") ,FFMPEG_DEINT_FILTER      ,NULL,
  9,_l("DScaler plugin")      ,DSCALER                  ,NULL,
 10,_l("5-tap lowpass")       ,LOWPASS5_DEINT_FILTER    ,NULL,
 11,_l("Kernel deinterlacer") ,KERNELDEINT              ,TdeinterlacePageKernel::create,
 13,_l("Kernel bob")          ,KERNELBOB                ,TdeinterlacePageKernel::create,
 14,_l("Yet Another DeInterlacing Filter (yadif)"),YADIF,TdeinterlacePageYadif::create,
  0,NULL                      ,0                        ,NULL
};
const char_t* TdeinterlaceSettings::dgbobModes[]=
{
 _l("equal number of frames, equal framerate"),
 _l("double number of frames, double framerate"),
 _l("double number of frames, equal framerate"),
 NULL
};
const TdeinterlaceSettings::TframerateDoublerSEs TdeinterlaceSettings::frameRateDoublerSEs[]=
{
 _l("No motion estimation - blend images"),0,
 _l("Current location only"),1,
 _l("1 pixel motion, left & right"),3,
 _l("3x3 square"),9,
 NULL,0
};

const TdeinterlaceSettings::TframerateDoublerSEs TdeinterlaceSettings::yadifParitySEs[]=
{
 _l("Auto"), -1,
 _l("Top field first"), 0,
 _l("Bottom field first"), 1,
 NULL
};

const TfilterIDFF TdeinterlaceSettings::idffs=
{
 /*name*/      _l("Deinterlacing"),
 /*id*/        IDFF_filterDeinterlace,
 /*is*/        IDFF_isDeinterlace,
 /*order*/     IDFF_orderDeinterlace,
 /*show*/      IDFF_showDeinterlace,
 /*full*/      IDFF_fullDeinterlace,
 /*half*/      0,
 /*dlgId*/     IDD_DEINTERLACE,
};

TdeinterlaceSettings::TdeinterlaceSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 memset(dscalerFlnm,0,sizeof(dscalerFlnm));
 memset(dscalerCfg,0,sizeof(dscalerCfg));
 half=0;
 static const TintOptionT<TdeinterlaceSettings> iopts[]=
  {
   IDFF_isDeinterlace            ,&TdeinterlaceSettings::is                       ,0,0,_l(""),1,
     _l("isDeinterlace"),0,
   IDFF_showDeinterlace          ,&TdeinterlaceSettings::show                     ,0,0,_l(""),1,
     _l("showDeinterlace"),1,
   IDFF_orderDeinterlace         ,&TdeinterlaceSettings::order                    ,1,1,_l(""),1,
     _l("orderDeinterlace"),0,
   IDFF_fullDeinterlace          ,&TdeinterlaceSettings::full                     ,0,0,_l(""),1,
     _l("fullDeinterlace"),0,
   IDFF_deinterlaceAlways        ,&TdeinterlaceSettings::deinterlaceAlways        ,0,0,_l(""),1,
     _l("deinterlaceAlways"),0,
   IDFF_swapFields               ,&TdeinterlaceSettings::swapfields               ,0,0,_l(""),1,
     _l("swapFields"),0,
   IDFF_deinterlaceMethod        ,&TdeinterlaceSettings::cfgId                    ,0,14,_l(""),1,
     _l("deinterlaceMethod"),cfgIdDef,
   IDFF_tomocompSE               ,&TdeinterlaceSettings::tomsmocompSE             ,0,30,_l(""),1,
     _l("tomocompSE"),3,
   IDFF_tomocompVF               ,&TdeinterlaceSettings::tomsmocompVF             ,0,0,_l(""),1,
     _l("tomocompVF"),0,
   IDFF_frameRateDoublerThreshold,&TdeinterlaceSettings::frameRateDoublerThreshold,0,255,_l(""),1,
     _l("frameRateDoublerThreshold"),255,
   IDFF_frameRateDoublerSE       ,&TdeinterlaceSettings::frameRateDoublerSE       ,0,10,_l(""),1,
     _l("frameRateDoublerSE"),3,
   IDFF_kernelDeintThreshold     ,&TdeinterlaceSettings::kernelThreshold          ,0,255,_l(""),1,
     _l("kernelDeintThreshold"),10,
   IDFF_kernelDeintSharp         ,&TdeinterlaceSettings::kernelSharp              ,0,0,_l(""),1,
     _l("kernelDeintSharp"),0,
   IDFF_kernelDeintTwoway        ,&TdeinterlaceSettings::kernelTwoway             ,0,0,_l(""),1,
     _l("kernelDeintTwoway"),0,
   IDFF_kernelDeintMap           ,&TdeinterlaceSettings::kernelMap                ,0,0,_l(""),1,
     _l("kernelDeintMap"),0,
   IDFF_kernelDeintLinked        ,&TdeinterlaceSettings::kernelLinked             ,0,0,_l(""),1,
     _l("kernelDeintLinked"),0,
   IDFF_dgbobMode                ,&TdeinterlaceSettings::dgbobMode                ,0,2,_l(""),1,
     _l("dgbobMode"),1,
   IDFF_dgbobThreshold           ,&TdeinterlaceSettings::dgbobThreshold           ,0,60,_l(""),1,
     _l("dgbobThreshold"),12,
   IDFF_dgbobAP                  ,&TdeinterlaceSettings::dgbobAP                  ,0,0,_l(""),1,
     _l("dgbobAP"),0,
   IDFF_yadifMode                ,&TdeinterlaceSettings::yadifMode                ,0,3,_l(""),1,
     _l("yadifMode"),0,
   IDFF_yadifFieldOrder          ,&TdeinterlaceSettings::yadifFieldOrder          ,-1,1,_l(""),1,
     _l("yadifParity"),-1,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_dscalerDIflnm,(TstrVal)&TdeinterlaceSettings::dscalerFlnm,MAX_PATH,0,_l(""),1,
     _l("dscalerDIflnm"),_l(""),
   IDFF_dscalerDIcfg ,(TstrVal)&TdeinterlaceSettings::dscalerCfg ,512     ,0,_l(""),1,
     _l("dscalerDIcfg"),_l(""),
   0
  };
 addOptions(sopts);

 static const TcreateParamList2<TmethodProps> listMethod(methodProps,&TmethodProps::name);setParamList(IDFF_deinterlaceMethod,&listMethod);
 static const TcreateParamList2<TframerateDoublerSEs> listSE(frameRateDoublerSEs,&TframerateDoublerSEs::name);setParamList(IDFF_frameRateDoublerSE,&listSE);
 static const TcreateParamList1 listDGbobMode(dgbobModes);setParamList(IDFF_dgbobMode,&listDGbobMode);
 static const TcreateParamList2<TframerateDoublerSEs> listYadifParity(yadifParitySEs,&TframerateDoublerSEs::name);setParamList(IDFF_yadifFieldOrder,&listYadifParity);
}

void TdeinterlaceSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  {
   if (!queue.temporary) setOnChange(IDFF_deinterlaceMethod,filters,&Tfilters::onQueueChange);
   queueFilter<TimgFilterDeinterlace>(filtersorder,filters,queue);
   switch (getMethod(cfgId).id)
    {
     case BYPASS:break;
     case TOMSMOCOMP_DEINT_FILTER:queueFilter<TimgFilterTomsMoComp>(filtersorder,filters,queue); return;
     case DGBOB_DEINT_FILTER:queueFilter<TimgFilterDGbob>(filtersorder,filters,queue); return;
     case FRAMERATEDOUBLER:queueFilter<TimgFilterFramerateDoubler>(filtersorder,filters,queue); return;
     case DSCALER:queueFilter<TimgFilterDScalerDI>(filtersorder,filters,queue); return;
     case KERNELDEINT:queueFilter<TimgFilterKernelDeint2>(filtersorder,filters,queue); return;
     case KERNELBOB:queueFilter<TimgFilterKernelBob>(filtersorder,filters,queue); return;
     case YADIF:queueFilter<TimgFilterYadif>(filtersorder,filters,queue); return;
     default:queueFilter<TimgFilterMplayerDeinterlace>(filtersorder,filters,queue); return; //mplayer deinterlacers
    }
  }
 if (getMethod(cfgId).id != YADIF || !is || !show)
  {
   // Inform yadif filter that it is no longer in the filter queue.
   // FIXME create more generic function that call Tfilter::onRemoveFromFilterQueue.
   TimgFilterYadif *imgFilterYadif = filters->getFilter<TimgFilterYadif>();
   imgFilterYadif->done();
  }
}
void TdeinterlaceSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TdeinterlacePage>(&idffs);
}

const TdeinterlaceSettings::TmethodProps& TdeinterlaceSettings::getMethod(int cfgId)
{
 for (const TmethodProps *m=methodProps;m->name;m++)
  if (m->cfgId==cfgId)
   return *m;
 return methodProps[0];
}

bool TdeinterlaceSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 if (getMethod(cfgId).id!=DSCALER)
  {
   tsnprintf_s(tipS, len, _TRUNCATE, _l("Method: %s"), getMethod(cfgId).name);
   return true;
  }
 else
  return false;
}
