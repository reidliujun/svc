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
#include "TgrabSettings.h"
#include "TimgFilterGrab.h"
#include "Cgrab.h"
#include "TffdshowPageDec.h"

const TgrabSettings::Tformat TgrabSettings::formats[]=
{
 _l("JPEG"),_l("jpg"),IDFF_MOVIE_LAVC,
 _l("BMP"),_l("bmp"),0,
 _l("PNG"),_l("png"),IDFF_MOVIE_LAVC,
 NULL
};

const TfilterIDFF TgrabSettings::idffs=
{
 /*name*/      _l("Grab"),
 /*id*/        IDFF_filterGrab,
 /*is*/        IDFF_isGrab,
 /*order*/     IDFF_orderGrab,
 /*show*/      IDFF_showGrab,
 /*full*/      IDFF_fullGrab,
 /*half*/      0,
 /*dlgId*/     IDD_GRAB,
};

TgrabSettings::TgrabSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 half=0;
 memset(path,0,sizeof(path));
 memset(prefix,0,sizeof(prefix));
 static const TintOptionT<TgrabSettings> iopts[]=
  {
   IDFF_isGrab           ,&TgrabSettings::is        ,0,0,_l(""),1,
     _l("isGrab"),0,
   IDFF_showGrab         ,&TgrabSettings::show      ,0,0,_l(""),1,
    _l("showGrab"),1,
   IDFF_orderGrab        ,&TgrabSettings::order     ,1,1,_l(""),1,
     _l("orderGrab"),0,
   IDFF_fullGrab         ,&TgrabSettings::full      ,0,0,_l(""),1,
     _l("fullGrab"),0,
   IDFF_grabDigits       ,&TgrabSettings::digits    ,0,6,_l(""),1,
     _l("grabDigits"),5,
   IDFF_grabFormat       ,&TgrabSettings::format    ,0,3,_l(""),1,
     _l("grabFormat"),0,
   IDFF_grabMode         ,&TgrabSettings::mode      ,0,2,_l(""),1,
     _l("grabMode"),1,
   IDFF_grabFrameNum     ,&TgrabSettings::frameNum  ,0,1000000,_l(""),1,
     _l("grabFrameNum"),0,
   IDFF_grabFrameNum1    ,&TgrabSettings::frameNum1 ,0,1000000,_l(""),1,
     _l("grabFrameNum1"),100,
   IDFF_grabFrameNum2    ,&TgrabSettings::frameNum2 ,0,1000000,_l(""),1,
     _l("grabFrameNum2"),110,
   IDFF_grabQual         ,&TgrabSettings::qual      ,0,100,_l(""),1,
     _l("grabQual"),80,
   IDFF_grabStep         ,&TgrabSettings::step      ,1,1000,_l(""),1,
     _l("grabStep"),1,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_grabPath         ,(TstrVal)&TgrabSettings::path   ,MAX_PATH ,0, _l(""),1,
     _l("grabPath"),_l("c:\\"),
   IDFF_grabPrefix       ,(TstrVal)&TgrabSettings::prefix ,30       ,0, _l(""),1,
     _l("grabPrefix"),_l("grab"),
   0
  };
 addOptions(sopts);

 static const TcreateParamList2<Tformat> listFormat(formats,&Tformat::name);setParamList(IDFF_grabFormat,&listFormat);
}

void TgrabSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (show)
  queueFilter<TimgFilterGrab>(filtersorder,filters,queue);
}
void TgrabSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TgrabPage>(&idffs);
}

const int* TgrabSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={
  IDFF_grabPath,IDFF_grabPrefix,
  IDFF_grabFormat,IDFF_grabDigits,
  IDFF_grabMode,IDFF_grabFrameNum,IDFF_grabFrameNum1,IDFF_grabFrameNum2,IDFF_grabStep,
  IDFF_grabQual,
  0};
 return idResets;
}

void TgrabSettings::reg_op2(TregOp &t)
{
 t._REG_OP_S(IDFF_grabPath,_l("grabPath"),path,MAX_PATH,_l("c:\\"));
 t._REG_OP_S(IDFF_grabPrefix,_l("grabPrefix"),prefix,30,_l("grab"));
 t._REG_OP_N(IDFF_grabFormat,_l("grabFormat"),format,0);
 t._REG_OP_N(IDFF_grabDigits,_l("grabDigits"),digits,5);
 t._REG_OP_N(IDFF_grabMode,_l("grabMode"),mode,1);
 t._REG_OP_N(IDFF_grabFrameNum,_l("grabFrameNum"),frameNum,0);
 t._REG_OP_N(IDFF_grabFrameNum1,_l("grabFrameNum1"),frameNum1,100);
 t._REG_OP_N(IDFF_grabFrameNum2,_l("grabFrameNum2"),frameNum2,110);
 t._REG_OP_N(IDFF_grabQual,_l("grabQual"),qual,80);
 t._REG_OP_N(IDFF_grabStep,_l("grabStep"),step,1);
}
