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
#include "TavisynthSettings.h"
#include "avisynth/TimgFilterAvisynth.h"
#include "Cavisynth.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TavisynthSettings::idffs=
{
 /*name*/      _l("Avisynth"),
 /*id*/        IDFF_filterAvisynth,
 /*is*/        IDFF_isAvisynth,
 /*order*/     IDFF_orderAvisynth,
 /*show*/      IDFF_showAvisynth,
 /*full*/      IDFF_fullAvisynth,
 /*half*/      0,
 /*dlgId*/     IDD_AVISYNTH,
};

TavisynthSettings::TavisynthSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 half=0;
 memset(script, 0, sizeof(script));
 memset(scriptMULTI_SZ, 0, sizeof(scriptMULTI_SZ));
 static const TintOptionT<TavisynthSettings> iopts[]=
  {
   IDFF_isAvisynth               ,&TavisynthSettings::is              ,0,0,_l(""),1,
     _l("isAvisynth"),0,
   IDFF_showAvisynth             ,&TavisynthSettings::show            ,0,0,_l(""),1,
     _l("showAvisynth"),1,
   IDFF_orderAvisynth            ,&TavisynthSettings::order           ,1,1,_l(""),1,
     _l("orderAvisynth"),0,
   IDFF_fullAvisynth             ,&TavisynthSettings::full            ,0,0,_l(""),1,
     _l("fullAvisynth"),0,
   IDFF_avisynthFfdshowSource    ,&TavisynthSettings::ffdshowSource   ,0,0,_l(""),1,
     _l("avisynthFfdshowSource"),1,
   IDFF_avisynthApplyPulldown    ,&TavisynthSettings::applyPulldown   ,0,2,_l(""),1,
     _l("avisynthApplyPulldown"),0,
   IDFF_avisynthInYV12           ,&TavisynthSettings::inYV12          ,0,0,_l(""),1,
     _l("avisynthInYV12"),1,
   IDFF_avisynthInYUY2           ,&TavisynthSettings::inYUY2          ,0,0,_l(""),1,
     _l("avisynthInYUY2"),1,
   IDFF_avisynthInRGB24          ,&TavisynthSettings::inRGB24         ,0,0,_l(""),1,
     _l("avisynthInRGB24"),1,
   IDFF_avisynthInRGB32          ,&TavisynthSettings::inRGB32         ,0,0,_l(""),1,
     _l("avisynthInRGB32"),1,
   IDFF_avisynthEnableBuffering  ,&TavisynthSettings::enableBuffering ,0,0,_l(""),1,
     _l("avisynthEnableBuffering"),0,
   IDFF_avisynthBufferAhead      ,&TavisynthSettings::bufferAhead     ,0,99,_l(""),1,
     _l("avisynthBufferAhead"),10,
   IDFF_avisynthBufferBack       ,&TavisynthSettings::bufferBack      ,0,99,_l(""),1,
     _l("avisynthBufferBack"),10,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_avisynthScript           ,(TstrVal)&TavisynthSettings::script         ,MAX_AVISYNTH_SCRIPT_LENGTH ,0,_l(""),1,
     _l("avisynthScript"),_l(""),
   IDFF_avisynthScriptMULTI_SZ   ,(TstrVal)&TavisynthSettings::scriptMULTI_SZ ,MAX_AVISYNTH_SCRIPT_LENGTH, 1,_l(""),1,
     _l("avisynthScriptMULTI_SZ"),NULL,
   0
  };
 addOptions(sopts);
}

void TavisynthSettings::reg_op(TregOp &t)
{
 ff_strncpy(scriptMULTI_SZ, script, MAX_AVISYNTH_SCRIPT_LENGTH); // before save
 TfilterSettingsVideo::reg_op(t);
 ff_strncpy(script, scriptMULTI_SZ, MAX_AVISYNTH_SCRIPT_LENGTH); // after load
}

void TavisynthSettings::getDefaultStr(int id,char_t *buf,size_t buflen)
{
 if (id == IDFF_avisynthScriptMULTI_SZ)
  ff_strncpy(buf, script, buflen);
 return TfilterSettingsVideo::getDefaultStr(id, buf, buflen);
}

void TavisynthSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);

 TimgFilterAvisynth* avisynth=filters->getFilter<TimgFilterAvisynth>();

 if (!avisynth)
  return;

 if (!is || !show || avisynth->deciV->getMovieFOURCC() == 0x53495641 /* "AVIS" */)
  {
   avisynth->onStop();
   avisynth->reset();
  }
 else
  queueFilter<TimgFilterAvisynth>(filtersorder,filters,queue);
}

void TavisynthSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TavisynthPage>(&idffs);
}

const int* TavisynthSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_avisynthInYV12,IDFF_avisynthInYUY2,IDFF_avisynthInRGB24,IDFF_avisynthInRGB32,IDFF_avisynthFfdshowSource,IDFF_avisynthBufferAhead,IDFF_avisynthBufferBack,IDFF_avisynthApplyPulldown,IDFF_avisynthEnableBuffering,0};
 return idResets;
}
