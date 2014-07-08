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
#include "TaudioDenoiseSettings.h"
#include "TaudioFilterDenoise.h"
#include "CaudioDenoise.h"
#include "TffdshowPageDec.h"

const TfilterIDFF TaudioDenoiseSettings::idffs=
{
 /*name*/      _l("Noise reduction"),
 /*id*/        IDFF_filterAudioDenoise,
 /*is*/        IDFF_isAudioDenoise,
 /*order*/     IDFF_orderAudioDenoise,
 /*show*/      IDFF_showAudioDenoise,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_AUDIODENOISE,
};

TaudioDenoiseSettings::TaudioDenoiseSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TaudioDenoiseSettings> iopts[]=
  {
   IDFF_isAudioDenoise       ,&TaudioDenoiseSettings::is       ,0,0,_l(""),1,
     _l("isAudioDenoise"), 0,
   IDFF_showAudioDenoise     ,&TaudioDenoiseSettings::show     ,0,0,_l(""),1,
     _l("showAudioDenoise"), 1,
   IDFF_orderAudioDenoise    ,&TaudioDenoiseSettings::order    ,1,1,_l(""),1,
     _l("orderAudioDenoise"), 0,
   IDFF_audioDenoiseThreshold,&TaudioDenoiseSettings::threshold,1,5000,_l(""),1,
     _l("audioDenoiseThreshold"), 100,
   0
  };
 addOptions(iopts);
}

void TaudioDenoiseSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  queueFilter<TaudioFilterDenoise>(filtersorder,filters,queue);
}
void TaudioDenoiseSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TaudioDenoisePage>(&idffs);
}

const int* TaudioDenoiseSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_audioDenoiseThreshold,0};
 return idResets;
}
bool TaudioDenoiseSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("threshold: %i"), threshold);
 return true;
}
