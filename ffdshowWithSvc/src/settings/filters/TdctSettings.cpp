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
#include "TdctSettings.h"
#include "TimgFilterDCT.h"
#include "Cdct.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TdctSettings::idffs=
{
 /*name*/      _l("DCT"),
 /*id*/        IDFF_filterDCT,
 /*is*/        IDFF_isDCT,
 /*order*/     IDFF_orderDCT,
 /*show*/      IDFF_showDCT,
 /*full*/      IDFF_fullDCT,
 /*half*/      IDFF_halfDCT,
 /*dlgId*/     IDD_DCT,
};

TdctSettings::TdctSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TdctSettings> iopts[]=
  {
   IDFF_isDCT    ,&TdctSettings::is    ,0,0,_l(""),1,
     _l("isDCT"),0,
   IDFF_showDCT  ,&TdctSettings::show  ,0,0,_l(""),1,
     _l("showDCT"),1,
   IDFF_orderDCT ,&TdctSettings::order ,1,1,_l(""),1,
     _l("orderDCT"),0,
   IDFF_fullDCT  ,&TdctSettings::full  ,0,0,_l(""),1,
     _l("fullDCT"),0,
   IDFF_halfDCT  ,&TdctSettings::half  ,0,0,_l(""),1,
     _l("halfDCT"),0,
   IDFF_dctMode  ,&TdctSettings::mode  ,0,2,_l(""),1,
     _l("dctMode"),0,
   IDFF_dct0     ,&TdctSettings::fac0  ,0,3000,_l(""),1,
     _l("dct0"),1000,
   IDFF_dct1     ,&TdctSettings::fac1  ,0,3000,_l(""),1,
     _l("dct1"),1000,
   IDFF_dct2     ,&TdctSettings::fac2  ,0,3000,_l(""),1,
     _l("dct2"),1000,
   IDFF_dct3     ,&TdctSettings::fac3  ,0,3000,_l(""),1,
     _l("dct3"),1000,
   IDFF_dct4     ,&TdctSettings::fac4  ,0,3000,_l(""),1,
     _l("dct4"),1000,
   IDFF_dct5     ,&TdctSettings::fac5  ,0,3000,_l(""),1,
     _l("dct5"),1000,
   IDFF_dct6     ,&TdctSettings::fac6  ,0,3000,_l(""),1,
     _l("dct6"),500,
   IDFF_dct7     ,&TdctSettings::fac7  ,0,3000,_l(""),1,
     _l("dct7"),0,
   IDFF_dctQuant ,&TdctSettings::quant ,1,31,_l(""),1,
     _l("dctQuant"),5,
   IDFF_dctMatrix0 ,&TdctSettings::matrix0 ,1,1,_l(""),1,
     _l("dctMatrix0") ,0x13121110,
   IDFF_dctMatrix1 ,&TdctSettings::matrix4 ,1,1,_l(""),1,
     _l("dctMatrix1") ,0x17161514,
   IDFF_dctMatrix2 ,&TdctSettings::matrix8 ,1,1,_l(""),1,
     _l("dctMatrix2") ,0x14131211,
   IDFF_dctMatrix3 ,&TdctSettings::matrix12,1,1,_l(""),1,
     _l("dctMatrix3") ,0x18171615,
   IDFF_dctMatrix4 ,&TdctSettings::matrix16,1,1,_l(""),1,
     _l("dctMatrix4") ,0x15141312,
   IDFF_dctMatrix5 ,&TdctSettings::matrix20,1,1,_l(""),1,
     _l("dctMatrix5") ,0x19181716,
   IDFF_dctMatrix6 ,&TdctSettings::matrix24,1,1,_l(""),1,
     _l("dctMatrix6") ,0x16151413,
   IDFF_dctMatrix7 ,&TdctSettings::matrix28,1,1,_l(""),1,
     _l("dctMatrix7") ,0x1b1a1817,
   IDFF_dctMatrix8 ,&TdctSettings::matrix32,1,1,_l(""),1,
     _l("dctMatrix8") ,0x17161514,
   IDFF_dctMatrix9 ,&TdctSettings::matrix36,1,1,_l(""),1,
     _l("dctMatrix9") ,0x1c1b1a19,
   IDFF_dctMatrix10,&TdctSettings::matrix40,1,1,_l(""),1,
     _l("dctMatrix10"),0x18171615,
   IDFF_dctMatrix11,&TdctSettings::matrix44,1,1,_l(""),1,
     _l("dctMatrix11"),0x1e1c1b1a,
   IDFF_dctMatrix12,&TdctSettings::matrix48,1,1,_l(""),1,
     _l("dctMatrix12"),0x1a181716,
   IDFF_dctMatrix13,&TdctSettings::matrix52,1,1,_l(""),1,
     _l("dctMatrix13"),0x1f1e1c1b,
   IDFF_dctMatrix14,&TdctSettings::matrix56,1,1,_l(""),1,
     _l("dctMatrix14"),0x1b191817,
   IDFF_dctMatrix15,&TdctSettings::matrix60,1,1,_l(""),1,
     _l("dctMatrix15"),0x211f1e1c,
   0
  };
 addOptions(iopts);
}

void TdctSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TimgFilterDCT>(filtersorder,filters,queue);
}
void TdctSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TdctPage>(&idffs);
}

const int* TdctSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_dctMode,IDFF_dct0,IDFF_dct1,IDFF_dct2,IDFF_dct3,IDFF_dct4,IDFF_dct5,IDFF_dct6,IDFF_dct7,IDFF_dctQuant,IDFF_dctMatrix0,IDFF_dctMatrix1,IDFF_dctMatrix2,IDFF_dctMatrix3,IDFF_dctMatrix4,IDFF_dctMatrix5,IDFF_dctMatrix6,IDFF_dctMatrix7,IDFF_dctMatrix8,IDFF_dctMatrix9,IDFF_dctMatrix10,IDFF_dctMatrix11,IDFF_dctMatrix12,IDFF_dctMatrix13,IDFF_dctMatrix14,IDFF_dctMatrix15,0};
 return idResets;
}

bool TdctSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 switch (mode)
  {
   case 0:
    tsnprintf_s(tipS, len, _TRUNCATE, _l("coefficients: %g %g %g %g %g %g %g %g"), fac0/1000.0f, fac1/1000.0f, fac2/1000.0f, fac3/1000.0f, fac4/1000.0f, fac5/1000.0f, fac6/1000.0f, fac7/1000.0f);
    break;
   case 1:
   case 2:
    tsnprintf_s(tipS, len, _TRUNCATE, _l("%s quantization by quantizer %i"), mode == 1 ? _l("H.263") : _l("MPEG"), quant);
    break;
  }
 tipS[len-1]='\0';
 return true;
}
