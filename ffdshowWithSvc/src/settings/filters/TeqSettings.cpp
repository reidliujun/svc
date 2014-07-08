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
#include "TeqSettings.h"
#include "TaudioFilterEQ.h"
#include "TaudioFilterEQsuper.h"
#include "Ceq.h"
#include "TffdshowPageDec.h"

const float TeqSettings::F[10]={31.25,62.5,125,250,500,1000,2000,4000,8000,16000};
const float TeqSettings::Fwinamp[10]={60,170,310,600,1000,3000,6000,12000,14000,16000};

const TfilterIDFF TeqSettings::idffs=
{
 /*name*/      _l("Equalizer"),
 /*id*/        IDFF_filterEQ,
 /*is*/        IDFF_isEQ,
 /*order*/     IDFF_orderEQ,
 /*show*/      IDFF_showEQ,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_EQ,
};

TeqSettings::TeqSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TeqSettings> iopts[]=
  {
   IDFF_isEQ      ,&TeqSettings::is    ,0,0,_l(""),1,
     _l("isEQ"),0,
   IDFF_showEQ    ,&TeqSettings::show  ,0,0,_l(""),1,
     _l("showEQ"),1,
   IDFF_orderEQ   ,&TeqSettings::order ,1,1,_l(""),1,
     _l("orderEQ"),0,
   IDFF_eqSuper   ,&TeqSettings::super ,0,0,_l(""),1,
     _l("eqSuper"),0,
   IDFF_eq0       ,&TeqSettings::eq0   ,0,200,_l(""),1,
     _l("eq0"),100,
   IDFF_eq1       ,&TeqSettings::eq1   ,0,200,_l(""),1,
     _l("eq1"),100,
   IDFF_eq2       ,&TeqSettings::eq2   ,0,200,_l(""),1,
     _l("eq2"),100,
   IDFF_eq3       ,&TeqSettings::eq3   ,0,200,_l(""),1,
     _l("eq3"),100,
   IDFF_eq4       ,&TeqSettings::eq4   ,0,200,_l(""),1,
     _l("eq4"),100,
   IDFF_eq5       ,&TeqSettings::eq5   ,0,200,_l(""),1,
     _l("eq5"),100,
   IDFF_eq6       ,&TeqSettings::eq6   ,0,200,_l(""),1,
     _l("eq6"),100,
   IDFF_eq7       ,&TeqSettings::eq7   ,0,200,_l(""),1,
     _l("eq7"),100,
   IDFF_eq8       ,&TeqSettings::eq8   ,0,200,_l(""),1,
     _l("eq8"),100,
   IDFF_eq9       ,&TeqSettings::eq9   ,0,200,_l(""),1,
     _l("eq9"),100,
   IDFF_eqLowdb   ,&TeqSettings::lowdb ,-10000,10000,_l(""),1,
     _l("eqLowdb"),-1200,
   IDFF_eqHighdb  ,&TeqSettings::highdb,-10000,10000,_l(""),1,
     _l("eqHighdb"),1200,
   IDFF_eq0freq   ,&TeqSettings::f0    ,100,192000*100,_l(""),1,
     _l("eq0freq"),int(F[0]*100),
   IDFF_eq1freq   ,&TeqSettings::f1    ,100,192000*100,_l(""),1,
     _l("eq1freq"),int(F[1]*100),
   IDFF_eq2freq   ,&TeqSettings::f2    ,100,192000*100,_l(""),1,
     _l("eq2freq"),int(F[2]*100),
   IDFF_eq3freq   ,&TeqSettings::f3    ,100,192000*100,_l(""),1,
     _l("eq3freq"),int(F[3]*100),
   IDFF_eq4freq   ,&TeqSettings::f4    ,100,192000*100,_l(""),1,
     _l("eq4freq"),int(F[4]*100),
   IDFF_eq5freq   ,&TeqSettings::f5    ,100,192000*100,_l(""),1,
     _l("eq5freq"),int(F[5]*100),
   IDFF_eq6freq   ,&TeqSettings::f6    ,100,192000*100,_l(""),1,
     _l("eq6freq"),int(F[6]*100),
   IDFF_eq7freq   ,&TeqSettings::f7    ,100,192000*100,_l(""),1,
     _l("eq7freq"),int(F[7]*100),
   IDFF_eq8freq   ,&TeqSettings::f8    ,100,192000*100,_l(""),1,
     _l("eq8freq"),int(F[8]*100),
   IDFF_eq9freq   ,&TeqSettings::f9    ,100,192000*100,_l(""),1,
     _l("eq9freq"),int(F[9]*100),
   0
  };
 addOptions(iopts);
}

void TeqSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  {
   if (!queue.temporary) setOnChange(IDFF_eqSuper,filters,&Tfilters::onQueueChange);
   if (super)
    queueFilter<TaudioFilterEQsuper>(filtersorder,filters,queue);
   else
    queueFilter<TaudioFilterEQ>(filtersorder,filters,queue);
  }
}
void TeqSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TeqPage>(&idffs);
}

const int* TeqSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={
  IDFF_eq0,IDFF_eq1,IDFF_eq2,IDFF_eq3,IDFF_eq4,IDFF_eq5,IDFF_eq6,IDFF_eq7,IDFF_eq8,IDFF_eq9,
  IDFF_eq0freq,IDFF_eq1freq,IDFF_eq2freq,IDFF_eq3freq,IDFF_eq4freq,IDFF_eq5freq,IDFF_eq6freq,IDFF_eq7freq,IDFF_eq8freq,IDFF_eq9freq,
  IDFF_eqLowdb,IDFF_eqHighdb,
  IDFF_eqSuper,
  0};
 return idResets;
}

bool TeqSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 char_t tip[200]=_l("");
 for (int i=0;i<10;i++)
  strncatf(tip, countof(tip), _l("%.2f, "),((highdb-lowdb)*(&eq0)[i]/200+lowdb)/100.0f);
 *strrchr(tip,',')='\0';
 tsnprintf_s(tipS, len, _TRUNCATE, _l("%s%s"), tip, super ? _l("\nSuperEQ") : _l(""));
 tipS[len-1]='\0';
 return true;
}
