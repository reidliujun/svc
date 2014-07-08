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
#include "TdolbyDecoderSettings.h"
#include "TaudioFilterDolbyDecoder.h"
#include "CdolbyDecoder.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TdolbyDecoderSettings::idffs=
{
 /*name*/      _l("Dolby decoder"),
 /*id*/        IDFF_filterDolbyDecoder,
 /*is*/        IDFF_isDolbyDecoder,
 /*order*/     IDFF_orderDolbyDecoder,
 /*show*/      IDFF_showDolbyDecoder,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_DOLBYDECODER,
};

TdolbyDecoderSettings::TdolbyDecoderSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TdolbyDecoderSettings> iopts[]=
  {
   IDFF_isDolbyDecoder    ,&TdolbyDecoderSettings::is   ,0,0,_l(""),1,
     _l("isDolbyDecoder"),0,
   IDFF_showDolbyDecoder  ,&TdolbyDecoderSettings::show ,0,0,_l(""),1,
     _l("showDolbyDecoder"),1,
   IDFF_orderDolbyDecoder ,&TdolbyDecoderSettings::order,1,1,_l(""),1,
     _l("orderDolbyDecoder"),0,
   IDFF_dolbyDecoderDelay ,&TdolbyDecoderSettings::delay,0,1000,_l(""),1,
     _l("dolbyDecoderDelay"),20,
   0
  };
 addOptions(iopts);
}

void TdolbyDecoderSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TaudioFilterDolbyDecoder2>(filtersorder,filters,queue);
}
void TdolbyDecoderSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TdolbyDecoderPage>(&idffs);
}

const int* TdolbyDecoderSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_dolbyDecoderDelay,0};
 return idResets;
}

bool TdolbyDecoderSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("delay: %i ms"), delay);
 return true;
}
