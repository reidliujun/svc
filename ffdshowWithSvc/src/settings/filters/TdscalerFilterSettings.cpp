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
#include "TdscalerFilterSettings.h"
#include "TimgFilterDScaler.h"
#include "CDScaler.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TDScalerFilterSettings::idffs=
{
 /*name*/      _l("DScaler filter"),
 /*id*/        IDFF_filterDScaler,
 /*is*/        IDFF_isDScaler,
 /*order*/     IDFF_orderDScaler,
 /*show*/      IDFF_showDScaler,
 /*full*/      IDFF_fullDScaler,
 /*half*/      IDFF_halfDScaler,
 /*dlgId*/     IDD_DSCALER,
};

TDScalerFilterSettings::TDScalerFilterSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 memset(fltflnm,0,sizeof(fltflnm));
 memset(cfg,0,sizeof(cfg));
 static const TintOptionT<TDScalerFilterSettings> iopts[]=
  {
   IDFF_isDScaler            ,&TDScalerFilterSettings::is    ,0,0,_l(""),1,
     _l("isDScaler"),0,
   IDFF_showDScaler          ,&TDScalerFilterSettings::show  ,0,0,_l(""),1,
     _l("showDScaler"),1,
   IDFF_orderDScaler         ,&TDScalerFilterSettings::order ,1,1,_l(""),1,
     _l("orderDScaler"),0,
   IDFF_fullDScaler          ,&TDScalerFilterSettings::full  ,0,0,_l(""),1,
     _l("fullDScaler"),0,
   IDFF_halfDScaler          ,&TDScalerFilterSettings::half  ,0,0,_l(""),1,
     _l("halfDScaler"),0,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_dscalerFltflnm,(TstrVal)&TDScalerFilterSettings::fltflnm,MAX_PATH,0,_l(""),1,
     _l("dscalerFltflnm"),_l(""),
   IDFF_dscalerCfg    ,(TstrVal)&TDScalerFilterSettings::cfg    ,512     ,0,_l(""),1,
     _l("dscalerCfg"),_l(""),
   0
  };
 addOptions(sopts);
}

void TDScalerFilterSettings ::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TimgFilterDScalerFLT>(filtersorder,filters,queue);
}
void TDScalerFilterSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TDScalerPageFlt>(&idffs);
}
