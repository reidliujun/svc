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
#include "TvisSettings.h"
#include "TimgFilterShowMV.h"
#include "CshowMV.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TvisSettings::idffs=
{
 /*name*/      _l("Visualizations"),
 /*id*/        IDFF_filterVis,
 /*is*/        IDFF_isVis,
 /*order*/     IDFF_orderVis,
 /*show*/      IDFF_showVis,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_SHOWMV,
};

TvisSettings::TvisSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 full=0;half=0;
 static const TintOptionT<TvisSettings> iopts[]=
  {
   IDFF_isVis     ,&TvisSettings::is     ,0,0,_l(""),1,
     _l("isVis"),0,
   IDFF_showVis   ,&TvisSettings::show   ,0,0,_l(""),1,
     _l("showVis"),1,
   IDFF_orderVis  ,&TvisSettings::order  ,1,1,_l(""),1,
     _l("orderShowMV"),0,
   IDFF_visMV     ,&TvisSettings::mv     ,0,0,_l(""),1,
     _l("visMV"),1,
   IDFF_visQuants ,&TvisSettings::quants ,0,0,_l(""),1,
     _l("visQuants"),0,
   IDFF_visGraph  ,&TvisSettings::graph  ,0,0,_l(""),1,
     _l("visGraph"),0,
   0
  };
 addOptions(iopts);
}

void TvisSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  if (mv || quants || graph)
   {
    queueFilter<TimgFilterShowMV>(filtersorder,filters,queue);
    if (!queue.temporary)
     {
      setOnChange(IDFF_visMV,filters,&Tfilters::onQueueChange);
      setOnChange(IDFF_visQuants,filters,&Tfilters::onQueueChange);
      setOnChange(IDFF_visGraph,filters,&Tfilters::onQueueChange);
     }
   }
}
void TvisSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TshowMVpage>(&idffs);
}

const int* TvisSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_visMV,IDFF_visQuants,IDFF_visGraph,0};
 return idResets;
}

bool TvisSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("%sshow motion vectors\n%sshow quantizers\n%sshow graph"),mv?_l(""):_l("do not "),quants?_l(""):_l("do not "),graph?_l(""):_l("do not "));
 return true;
}

void TvisSettings::reg_op2(TregOp &t)
{
 t._REG_OP_N(IDFF_visMV,_l("visMV"),mv,1);
 t._REG_OP_N(IDFF_visQuants,_l("visQuants"),quants,0);
 t._REG_OP_N(IDFF_visGraph,_l("visGraph"),graph,0);
}
