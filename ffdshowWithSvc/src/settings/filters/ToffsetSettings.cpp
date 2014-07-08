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
#include "ToffsetSettings.h"
#include "TimgFilterOffset.h"
#include "Coffset.h"
#include "TffdshowPageDec.h"

const TfilterIDFF ToffsetSettings::idffs=
{
 /*name*/      _l("Offset"),
 /*id*/        IDFF_filterOffset,
 /*is*/        IDFF_isOffset,
 /*order*/     IDFF_orderOffset,
 /*show*/      IDFF_showOffset,
 /*full*/      IDFF_fullOffset,
 /*half*/      IDFF_halfOffset,
 /*dlgId*/     IDD_OFFSETFLIP,
};

ToffsetSettings::ToffsetSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<ToffsetSettings> iopts[]=
  {
   IDFF_isOffset          ,&ToffsetSettings::is    ,0,0,_l(""),1,
     _l("isOffset"),0,
   IDFF_showOffset        ,&ToffsetSettings::show  ,0,0,_l(""),1,
     _l("showOffset"),1,
   IDFF_orderOffset       ,&ToffsetSettings::order ,1,1,_l(""),1,
     _l("orderOffset"),0,
   IDFF_fullOffset        ,&ToffsetSettings::full  ,0,0,_l(""),1,
     _l("fullOffset"),0,
   IDFF_halfOffset        ,&ToffsetSettings::half  ,0,0,_l(""),1,
     _l("halfOffset"),0,
   IDFF_offsetY_X         ,&ToffsetSettings::Y_X   ,-32,32,_l(""),1,
     _l("offsetY_X"),0,
   IDFF_offsetY_Y         ,&ToffsetSettings::Y_Y   ,-32,32,_l(""),1,
     _l("offsetY_Y"),0,
   IDFF_offsetU_X         ,&ToffsetSettings::U_X   ,-32,32,_l(""),1,
     _l("offsetU_X"),0,
   IDFF_offsetU_Y         ,&ToffsetSettings::U_Y   ,-32,32,_l(""),1,
     _l("offsetU_Y"),0,
   IDFF_offsetV_X         ,&ToffsetSettings::V_X   ,-32,32,_l(""),1,
     _l("offsetV_X"),0,
   IDFF_offsetV_Y         ,&ToffsetSettings::V_Y   ,-32,32,_l(""),1,
     _l("offsetV_Y"),0,

   IDFF_transfFlip        ,&ToffsetSettings::flip  ,0,0,_l(""),1,
     _l("transfFlip"),0,
   IDFF_transfMirror      ,&ToffsetSettings::mirror,0,0,_l(""),1,
     _l("transMirror"),0,

   0
  };
 addOptions(iopts);
}

void ToffsetSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);

 if (is && show)
  {
   if (!queue.temporary)
    {
     setOnChange(IDFF_transfFlip,filters,&Tfilters::onQueueChange);
     setOnChange(IDFF_transfMirror,filters,&Tfilters::onQueueChange);
    }
   queueFilter<TimgFilterOffset>(filtersorder,filters,queue);
   if (flip) queueFilter<TimgFilterFlip>(filtersorder,filters,queue);
   if (mirror) queueFilter<TimgFilterMirror>(filtersorder,filters,queue);
  }
}
void ToffsetSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<ToffsetPage>(&idffs);
}

const int* ToffsetSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_offsetY_X,IDFF_offsetY_Y,IDFF_offsetU_X,IDFF_offsetU_Y,IDFF_offsetV_X,IDFF_offsetV_Y,IDFF_transfFlip,IDFF_transfMirror,0};
 return idResets;
}

bool ToffsetSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE,_l("luma offset:[%i,%i] chroma offset:[%i,%i]"),Y_X,Y_Y,U_X,U_Y);
 if (flip) strncat_s(tipS, len, _l("\nflip"), _TRUNCATE);
 if (mirror) strncat_s(tipS, len, _l("\nmirror"), _TRUNCATE);
 tipS[len-1]='\0';
 return true;
}
