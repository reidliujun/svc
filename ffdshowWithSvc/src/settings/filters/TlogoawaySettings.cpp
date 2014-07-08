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
#include "TlogoawaySettings.h"
#include "TimgFilterLogoaway.h"
#include "Clogoaway.h"
#include "TffdshowPageDec.h"

const TlogoawaySettings::Tmode TlogoawaySettings::modes[]=
{
 MODE_XY,_l("XY"),
 MODE_UGLARM,_l("UGLARM"),
 MODE_SOLIDFILL,_l("Solid fill"),
 MODE_SHAPEXY,_l("Shape XY"),
 //MODE_SHAPEUGLARM,"Shape UGLARM",
 0,NULL
};
const char_t* TlogoawaySettings::borderModes[]=
{
 _l("Unknown"),
 _l("Direct"),
 _l("Opposite"),
 _l("Interpolation"),
 NULL
};
const char_t* TlogoawaySettings::navigators[]=
{
 _l("UNKNOWN"),
 _l("NORTH"),
 _l("EAST"),
 _l("SOUTH"),
 _l("WEST"),
 _l("NW"),
 _l("NE"),
 _l("SW"),
 _l("SE"),
 NULL
};

const TlogoawaySettings::TborderPreset TlogoawaySettings::borderPresets[]=
{
 _l("Over video")                   ,BM_DIRECT     ,BM_DIRECT     ,BM_DIRECT     ,BM_DIRECT     ,
 _l("Over top letterbox")           ,BM_INTERPOLATE,BM_DIRECT     ,BM_DIRECT     ,BM_DIRECT     ,
 _l("Over top & left letterbox")    ,BM_INTERPOLATE,BM_DIRECT     ,BM_DIRECT     ,BM_INTERPOLATE,
 _l("Over top & right letterbox")   ,BM_INTERPOLATE,BM_INTERPOLATE,BM_DIRECT     ,BM_DIRECT     ,
 _l("Over bottom letterbox")        ,BM_DIRECT     ,BM_DIRECT     ,BM_INTERPOLATE,BM_DIRECT     ,
 _l("Over bottom & left letterbox") ,BM_DIRECT     ,BM_DIRECT     ,BM_INTERPOLATE,BM_INTERPOLATE,
 _l("Over bottom & right letterbox"),BM_DIRECT     ,BM_INTERPOLATE,BM_INTERPOLATE,BM_DIRECT     ,
 _l("Over left letterbox")          ,BM_DIRECT     ,BM_DIRECT     ,BM_DIRECT     ,BM_INTERPOLATE,
 _l("Over right letterbox")         ,BM_DIRECT     ,BM_INTERPOLATE,BM_DIRECT     ,BM_DIRECT     ,
 NULL
};

const TfilterIDFF TlogoawaySettings::idffs=
{
 /*name*/      _l("Logoaway"),
 /*id*/        IDFF_filterLogoaway,
 /*is*/        IDFF_isLogoaway,
 /*order*/     IDFF_orderLogoaway,
 /*show*/      IDFF_showLogoaway,
 /*full*/      IDFF_fullLogoaway,
 /*half*/      0,
 /*dlgId*/     IDD_LOGOAWAY,
};

TlogoawaySettings::TlogoawaySettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 half=0;
 memset(parambitmap,0,sizeof(parambitmap));
 static const TintOptionT<TlogoawaySettings> iopts[]=
  {
   IDFF_isLogoaway           ,&TlogoawaySettings::is            ,0,0,_l(""),1,
     _l("isLogoaway"),0,
   IDFF_showLogoaway         ,&TlogoawaySettings::show          ,0,0,_l(""),1,
     _l("showLogoaway"),1,
   IDFF_orderLogoaway        ,&TlogoawaySettings::order         ,1,1,_l(""),1,
     _l("orderLogoaway"),0,
   IDFF_fullLogoaway         ,&TlogoawaySettings::full          ,0,0,_l(""),1,
     _l("fullLogoaway"),0,
   IDFF_logoawayX            ,&TlogoawaySettings::x             ,0,16384,_l(""),1,
     _l("logoawayX"),100,
   IDFF_logoawayY            ,&TlogoawaySettings::y             ,0,16384,_l(""),1,
     _l("logoawayY"),10,
   IDFF_logoawayDx           ,&TlogoawaySettings::dx            ,1,16384,_l(""),1,
     _l("logoawayDx"),16,
   IDFF_logoawayDy           ,&TlogoawaySettings::dy            ,1,16384,_l(""),1,
     _l("logoawayDy"),16,
   IDFF_logoawayMode         ,&TlogoawaySettings::mode          ,0,MODE_LAST-1,_l(""),1,
     _l("logoawayMode"),MODE_XY,
   IDFF_logoawayBlur         ,&TlogoawaySettings::blur          ,0,10,_l(""),1,
     _l("logoawayBlur"),1,
 //borderpaint = NO;
   IDFF_logoawayPointnw      ,&TlogoawaySettings::pointnw       ,5,8,_l(""),1,
     _l("logoawayPointnw"),NW,
   IDFF_logoawayPointne      ,&TlogoawaySettings::pointne       ,5,8,_l(""),1,
     _l("logoawayPointne"),NE,
   IDFF_logoawayPointsw      ,&TlogoawaySettings::pointsw       ,5,8,_l(""),1,
     _l("logoawayPointsw"),SW,
   IDFF_logoawayPointse      ,&TlogoawaySettings::pointse       ,5,8,_l(""),1,
     _l("logoawayPointse"),SE,
   IDFF_logoawayBordn_mode   ,&TlogoawaySettings::bordn_mode    ,1,3,_l(""),1,
     _l("logoawayBordn_mode"),BM_DIRECT,
   IDFF_logoawayBorde_mode   ,&TlogoawaySettings::borde_mode    ,1,3,_l(""),1,
     _l("logoawayBorde_mode"),BM_DIRECT,
   IDFF_logoawayBords_mode   ,&TlogoawaySettings::bords_mode    ,1,3,_l(""),1,
     _l("logoawayBords_mode"),BM_DIRECT,
   IDFF_logoawayBordw_mode   ,&TlogoawaySettings::bordw_mode    ,1,3,_l(""),1,
     _l("logoawayBordw_mode"),BM_DIRECT,
   IDFF_logoawayVhweight     ,&TlogoawaySettings::vhweight      ,0,10,_l(""),1,
     _l("logoawayVhweight"),5,
   IDFF_logoawaySolidcolor   ,&TlogoawaySettings::solidcolor    ,1,1,_l(""),1,
     _l("logoawaySolidcolor"),RGB(255,255,255),
   IDFF_logoawayLumaOnly     ,&TlogoawaySettings::lumaonly      ,0,0,_l(""),1,
     _l("logoawayLumaOnly"),0,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_logoawayParamBitmapFlnm ,(TstrVal)&TlogoawaySettings::parambitmap  ,MAX_PATH,0,_l(""),1,
     _l("logoawayParambitmap"),_l(""),
   0
  };
 addOptions(sopts);

 static const TcreateParamList2<Tmode> listMode(modes,&Tmode::name);setParamList(IDFF_logoawayMode,&listMode);
 static const TcreateParamList1 listBord(borderModes);setParamList(IDFF_logoawayBordn_mode,&listBord);setParamList(IDFF_logoawayBords_mode,&listBord);setParamList(IDFF_logoawayBordw_mode,&listBord);setParamList(IDFF_logoawayBorde_mode,&listBord);
}

void TlogoawaySettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TimgFilterLogoaway>(filtersorder,filters,queue);
}
void TlogoawaySettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TlogoawayPage>(&idffs);
}

const int* TlogoawaySettings::getResets(unsigned int pageId)
{
 static const int idResets[]={
  IDFF_logoawayX,IDFF_logoawayY,IDFF_logoawayDx,IDFF_logoawayDy,
  IDFF_logoawayMode,
  IDFF_logoawayBlur,
  IDFF_logoawayPointnw,IDFF_logoawayPointne,IDFF_logoawayPointsw,IDFF_logoawayPointse,
  IDFF_logoawayBordn_mode,IDFF_logoawayBorde_mode,IDFF_logoawayBords_mode,IDFF_logoawayBordw_mode,
  IDFF_logoawayVhweight,
  IDFF_logoawaySolidcolor,
  IDFF_logoawayLumaOnly,
  IDFF_logoawayParamBitmapFlnm,
  0};
 return idResets;
}

const char_t* TlogoawaySettings::getModeDescr(int mode)
{
 for (int i=0;modes[i].name;i++)
  if (modes[i].id==mode)
   return modes[i].name;
 return _l("");
}

bool TlogoawaySettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("[%i,%i]-[%i,%i]\n%s"),x,y,x+dx-1,y+dy-1,getModeDescr(mode));
 return true;
}
