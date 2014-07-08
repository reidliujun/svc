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
#include "TfirSettings.h"
#include "TaudioFilterFIR.h"
#include "Cfir.h"
#include "TffdshowPageDec.h"

const char_t* TfirSettings::types[]=
{
 _l("lowpass"),
 _l("highpass"),
 _l("bandpass"),
 _l("bandstop"),
 NULL
};
const char_t* TfirSettings::windows[]=
{
 _l("box"),
 _l("triangle"),
 _l("hamming"),
 _l("hanning"),
 _l("blackman"),
 _l("flattop"),
 _l("kaiser"),
 NULL
};

const TfilterIDFF TfirSettings::idffs=
{
 /*name*/      _l("FIR filter"),
 /*id*/        IDFF_filterFir,
 /*is*/        IDFF_isFir,
 /*order*/     IDFF_orderFir,
 /*show*/      IDFF_showFir,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_FIR,
};

TfirSettings::TfirSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TfirSettings> iopts[]=
  {
   IDFF_isFir         ,&TfirSettings::is        ,0,0,_l(""),1,
     _l("isFIR"),0,
   IDFF_showFir       ,&TfirSettings::show      ,0,0,_l(""),1,
     _l("showFIR"),1,
   IDFF_orderFir      ,&TfirSettings::order     ,1,1,_l(""),1,
     _l("orderFIR"),0,
   IDFF_firTaps       ,&TfirSettings::taps      ,8,1024,_l(""),1,
     _l("firTaps"),32,
   IDFF_firType       ,&TfirSettings::type      ,0,3,_l(""),1,
     _l("firType"),0,
   IDFF_firFreq       ,&TfirSettings::freq      ,1,192000,_l(""),1,
     _l("firFreq"),6000,
   IDFF_firWidth      ,&TfirSettings::width     ,1,192000/2,_l(""),1,
     _l("firWidth"),1000,
   IDFF_firWindow     ,&TfirSettings::window    ,0,6,_l(""),1,
     _l("firWindow"),4,
   IDFF_firKaiserBeta ,&TfirSettings::kaiserbeta,1000,20000,_l(""),1,
     _l("firKaiserBeta"),1000,
   0
  };
 addOptions(iopts);
 static const TcreateParamList1 listFirType(types);setParamList(IDFF_firType,&listFirType);
 static const TcreateParamList1 listFirWindow(windows);setParamList(IDFF_firWindow,&listFirWindow);

}

void TfirSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (show)
  queueFilter<TaudioFilterFir>(filtersorder,filters,queue);
}
void TfirSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TfirPage>(&idffs);
}

const int* TfirSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_firTaps,IDFF_firType,IDFF_firFreq,IDFF_firWidth,IDFF_firWindow,IDFF_firKaiserBeta,0};
 return idResets;
}

bool TfirSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 char_t widths[50]=_l("");
 if (type==BANDPASS || type==BANDSTOP)
  tsprintf(widths,_l(", width %i Hz"),width);
 tsnprintf_s(tipS, len, _TRUNCATE, _l("%s at %iHz%s\n%i taps\n%s window"), types[type], freq, widths, taps, windows[window]);
 return true;
}
