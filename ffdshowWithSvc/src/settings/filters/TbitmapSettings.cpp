/*
 * Copyright (c) 2004-2006 Milan Cutka
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
#include "TbitmapSettings.h"
#include "TimgFilterBitmap.h"
#include "Cbitmap.h"
#include "TffdshowPageDec.h"
#include "TsubtitlesSettings.h"

const char_t* TbitmapSettings::modes[]=
{
 _l("blend"),
 _l("darken"),
 _l("lighten"),
 _l("add"),
 _l("softlight"),
 _l("exclusion"),
 NULL
};

const TfilterIDFF TbitmapSettings::idffs=
{
 /*name*/      _l("Bitmap overlay"),
 /*id*/        IDFF_filterBitmap,
 /*is*/        IDFF_isBitmap,
 /*order*/     IDFF_orderBitmap,
 /*show*/      IDFF_showBitmap,
 /*full*/      IDFF_fullBitmap,
 /*half*/      0,
 /*dlgId*/     IDD_BITMAP,
};

TbitmapSettings::TbitmapSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 half=0;
 memset(flnm,0,sizeof(flnm));
 static const TintOptionT<TbitmapSettings> iopts[]=
  {
   IDFF_isBitmap       ,&TbitmapSettings::is        ,0,0,_l(""),1,
     _l("isBitmap"),0,
   IDFF_showBitmap     ,&TbitmapSettings::show      ,0,0,_l(""),1,
     _l("showBitmap"),1,
   IDFF_orderBitmap    ,&TbitmapSettings::order     ,1,1,_l(""),1,
     _l("orderBitmap"),0,
   IDFF_fullBitmap     ,&TbitmapSettings::full      ,0,0,_l(""),1,
     _l("fullBitmap"),0,
   IDFF_bitmapPosx     ,&TbitmapSettings::posx      ,-4096,4096,_l(""),1,
     _l("bitmapPosX"),50,
   IDFF_bitmapPosy     ,&TbitmapSettings::posy      ,-4096,4096,_l(""),1,
     _l("bitmapPosY"),50,
   IDFF_bitmapPosmode  ,&TbitmapSettings::posmode   ,0,1,_l(""),1,
     _l("bitmapPosMode"),0,
   IDFF_bitmapAlign    ,&TbitmapSettings::align     ,0,3,_l(""),1,
     _l("bitmapAlign"),ALIGN_CENTER,
   IDFF_bitmapMode     ,&TbitmapSettings::mode      ,0,5,_l(""),1,
     _l("bitmapMode"),0,
   IDFF_bitmapStrength ,&TbitmapSettings::strength  ,0,256,_l(""),1,
     _l("bitmapStrength"),128,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_bitmapFlnm     ,(TstrVal)&TbitmapSettings::flnm  ,MAX_PATH,0, _l(""),1,
     _l("bitmapFlnm"),_l(""),
   0
  };
 addOptions(sopts);

 static const TcreateParamList1 listMode(modes);setParamList(IDFF_bitmapMode,&listMode);
 static const TcreateParamList1 listAlign(TsubtitlesSettings::alignments);setParamList(IDFF_bitmapAlign,&listAlign);
}

void TbitmapSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TimgFilterBitmap>(filtersorder,filters,queue);
}
void TbitmapSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TbitmapPage>(&idffs);
}

const int* TbitmapSettings::getResets(unsigned int pageId)
{
 static const int idResets[]=
  {
   IDFF_bitmapPosx,IDFF_bitmapPosy,IDFF_bitmapPosmode,IDFF_bitmapAlign,IDFF_bitmapMode,IDFF_bitmapStrength,
   0
  };
 return idResets;
}

bool TbitmapSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 if (flnm[0])
  {
   tsnprintf_s(tipS, len, _TRUNCATE, _l("%s %s"), modes[mode], flnm);
   tipS[len-1]='\0';
  }
 else
  tipS[0]='\0';
 return true;
}
