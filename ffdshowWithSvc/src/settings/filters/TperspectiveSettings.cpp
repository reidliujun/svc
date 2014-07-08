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
#include "TperspectiveSettings.h"
#include "TimgFilterPerspective.h"
#include "Cperspective.h"
#include "TffdshowPageDec.h"

const char_t* TperspectiveSettings::interpolationNames[]=
{
 _l("None"),
 _l("Linear"),
 _l("Cubic"),
 NULL
};

const TfilterIDFF TperspectiveSettings::idffs=
{
 /*name*/      _l("Perspective"),
 /*id*/        IDFF_filterPerspective,
 /*is*/        IDFF_isPerspective,
 /*order*/     IDFF_orderPerspective,
 /*show*/      IDFF_showPerspective,
 /*full*/      IDFF_fullPerspective,
 /*half*/      IDFF_halfPerspective,
 /*dlgId*/     IDD_PERSPECTIVE,
};

TperspectiveSettings::TperspectiveSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TperspectiveSettings> iopts[]=
  {
   IDFF_isPerspective            ,&TperspectiveSettings::is              ,0,0,_l(""),1,
     _l("isPerspective"),0,
   IDFF_showPerspective          ,&TperspectiveSettings::show            ,0,0,_l(""),1,
     _l("showPerspective"),1,
   IDFF_orderPerspective         ,&TperspectiveSettings::order           ,1,1,_l(""),1,
     _l("orderPerspective"),0,
   IDFF_fullPerspective          ,&TperspectiveSettings::full            ,0,0,_l(""),1,
     _l("fullPerspective"),1,
   IDFF_halfPerspective          ,&TperspectiveSettings::half            ,0,0,_l(""),1,
     _l("halfPerspective"),0,
   IDFF_perspectiveIsSrc         ,&TperspectiveSettings::isSrc           ,0,0,_l(""),1,
     _l("perspectiveIsSrc"),0,
   IDFF_perspectiveX1            ,&TperspectiveSettings::x1              ,0,100,_l(""),1,
     _l("perspectiveX1"),0,
   IDFF_perspectiveY1            ,&TperspectiveSettings::y1              ,0,100,_l(""),1,
     _l("perspectiveY1"),0,
   IDFF_perspectiveX2            ,&TperspectiveSettings::x2              ,0,100,_l(""),1,
     _l("perspectiveX2"),100,
   IDFF_perspectiveY2            ,&TperspectiveSettings::y2              ,0,100,_l(""),1,
     _l("perspectiveY2"),0,
   IDFF_perspectiveX3            ,&TperspectiveSettings::x3              ,0,100,_l(""),1,
     _l("perspectiveX3"),0,
   IDFF_perspectiveY3            ,&TperspectiveSettings::y3              ,0,100,_l(""),1,
     _l("perspectiveY3"),100,
   IDFF_perspectiveX4            ,&TperspectiveSettings::x4              ,0,100,_l(""),1,
     _l("perspectiveX4"),100,
   IDFF_perspectiveY4            ,&TperspectiveSettings::y4              ,0,100,_l(""),1,
     _l("perspectiveY4"),100,
   IDFF_perspectiveInterpolation ,&TperspectiveSettings::interpolation   ,0,2,_l(""),1,
     _l("perspectiveInterpolation"),0,
   0
  };
 addOptions(iopts);

 static const TcreateParamList1 listInterpolation(interpolationNames);setParamList(IDFF_perspectiveInterpolation,&listInterpolation);
}

void TperspectiveSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TimgFilterPerspective>(filtersorder,filters,queue);
}
void TperspectiveSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TperspectivePage>(&idffs);
}

const int* TperspectiveSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={
   IDFF_perspectiveIsSrc,IDFF_perspectiveInterpolation,
   IDFF_perspectiveX1,IDFF_perspectiveY1,
   IDFF_perspectiveX2,IDFF_perspectiveY2,
   IDFF_perspectiveX3,IDFF_perspectiveY3,
   IDFF_perspectiveX4,IDFF_perspectiveY4,
   0};
 return idResets;
}

bool TperspectiveSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("%s rectangle: [%i,%i] [%i,%i] [%i,%i] [%i,%i]\n%s interpolation"),isSrc?_l("Source"):_l("Destination"),x1,y1,x2,y2,x3,y3,x4,y4,interpolationNames[interpolation]);
 return true;
}
