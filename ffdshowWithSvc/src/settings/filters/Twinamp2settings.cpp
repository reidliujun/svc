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
#include "Twinamp2settings.h"
#include "TaudioFilterWinamp2.h"
#include "Cwinamp2.h"
#include "TffdshowPageDec.h"

const TfilterIDFF Twinamp2settings::idffs=
{
 /*name*/      _l("Winamp 2"),
 /*id*/        IDFF_filterWinamp2,
 /*is*/        IDFF_isWinamp2,
 /*order*/     IDFF_orderWinamp2,
 /*show*/      IDFF_showWinamp2,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_WINAMP2,
};

Twinamp2settings::Twinamp2settings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 memset(flnm,0,sizeof(flnm));
 memset(modulename,0,sizeof(modulename));
 static const TintOptionT<Twinamp2settings> iopts[]=
  {
   IDFF_isWinamp2    ,&Twinamp2settings::is    ,0,0,_l(""),1,
     _l("isWinamp2"),0,
   IDFF_showWinamp2  ,&Twinamp2settings::show  ,0,0,_l(""),1,
     _l("showWinamp2"),1,
   IDFF_orderWinamp2 ,&Twinamp2settings::order ,1,1,_l(""),1,
     _l("orderWinamp2"),0,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_winamp2flnm                     ,(TstrVal)&Twinamp2settings::flnm                     ,MAX_PATH                     ,0 ,_l(""),1,
     _l("winamp2flnm"),_l(""),
   IDFF_winamp2filtername               ,(TstrVal)&Twinamp2settings::modulename               ,256                          ,0 ,_l(""),1,
     _l("winamp2filtername"),_l(""),
   IDFF_winamp2allowMultichannelOnlyIn  ,(TstrVal)&Twinamp2settings::allowMultichannelOnlyIn  ,MAX_COMPATIBILITYLIST_LENGTH ,0 ,_l(""),1,
     _l("allowMultichannelOnlyIn"),_l("dsp_dfx.dll"),
   0
  };
 addOptions(sopts);
}

void Twinamp2settings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 queueFilter<TaudioFilterWinamp2>(filtersorder,filters,queue);
}
void Twinamp2settings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<Twinamp2page>(&idffs);
}

bool Twinamp2settings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 if (flnm[0] && modulename[0])
  {
   tsnprintf_s(tipS, len, _TRUNCATE,_l("%s: %s"),flnm,modulename);
   return true;
  }
 else
  return false;
}
