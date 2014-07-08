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
#include "TresampleSettings.h"
#include "TaudioFilterResample.h"
#include "Tconfig.h"
#include "IffdshowBase.h"
#include "Cresample.h"
#include "TffdshowPageDec.h"

const TresampleSettings::Tresampler TresampleSettings::resamplers[]=
{
 _l("libavcodec normal quality"),RESAMPLE_LAVC_NORMAL,
 _l("libavcodec high quality"),RESAMPLE_LAVC_HIGH,
 _l("libavcodec highest quality"),RESAMPLE_LAVC_HIGHEST,
 _l("libsamplerate sinc high quality"),RESAMPLE_SRC_SINC_BEST_QUALITY,
 _l("libsamplerate sinc medium quality"),RESAMPLE_SRC_SINC_MEDIUM_QUALITY,
 _l("libsamplerate sinc fastest"),RESAMPLE_SRC_SINC_FASTEST,
 NULL,0
};

const TfilterIDFF TresampleSettings::idffs=
{
 /*name*/      _l("Resample"),
 /*id*/        IDFF_filterResample,
 /*is*/        IDFF_isResample,
 /*order*/     IDFF_orderResample,
 /*show*/      IDFF_showResample,
 /*full*/      0,
 /*half*/      0,
 /*dlgId*/     IDD_RESAMPLE,
};

TresampleSettings::TresampleSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsAudio(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TresampleSettings> iopts[]=
  {
   IDFF_isResample    ,&TresampleSettings::is   ,0,0,_l(""),1,
     _l("isResample"),0,
   IDFF_showResample  ,&TresampleSettings::show ,0,0,_l(""),1,
     _l("showResample"),1,
   IDFF_orderResample ,&TresampleSettings::order,1,1,_l(""),1,
     _l("orderResample"),0,
   IDFF_resampleFreq  ,&TresampleSettings::freq ,1,192000,_l(""),1,
     _l("resampleFreq"),44100,
   IDFF_resampleMode  ,&TresampleSettings::mode ,0,5,_l(""),1,
     _l("resampleMode"),RESAMPLE_LAVC_NORMAL,
   IDFF_resampleIf    ,&TresampleSettings::_if   ,0,1,_l(""),1,
     _l("resampleIf"),0,
   IDFF_resampleIfCond,&TresampleSettings::cond ,-1,1,_l(""),1,
     _l("resampleIfCond"),1,
   IDFF_resampleIfFreq,&TresampleSettings::condFreq,1,192000,_l(""),1,
     _l("resampleIfFreq"),44100,
   0
  };
 addOptions(iopts);
 static const TcreateParamList2<TresampleSettings::Tresampler> listResampleMode(resamplers,&Tresampler::name);setParamList(IDFF_resampleMode,&listResampleMode);

}

void TresampleSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  {
   if (!queue.temporary) setOnChange(IDFF_resampleMode,filters,&Tfilters::onQueueChange);
   if (isIn(mode,(int)RESAMPLE_SRC_SINC_BEST_QUALITY,(int)RESAMPLE_SRC_SINC_FASTEST))
    {
     const Tconfig *config;filters->deci->getConfig(&config);
     if (config->check(TaudioFilterResampleSRC::dllname))
      queueFilter<TaudioFilterResampleSRC>(filtersorder,filters,queue);
    }
   else
    queueFilter<TaudioFilterResampleLavc>(filtersorder,filters,queue);
  }
}
void TresampleSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TresamplePage>(&idffs);
}

const int* TresampleSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={IDFF_resampleFreq,IDFF_resampleMode,IDFF_resampleIf,IDFF_resampleIfCond,IDFF_resampleIfFreq,0};
 return idResets;
}

const char_t* TresampleSettings::getResamplerName(int mode)
{
 for (int i=0;resamplers[i].name;i++)
  if (resamplers[i].id==mode)
   return resamplers[i].name;
 return _l("");
}

const char_t* TresampleSettings::getCondStr(int cond)
{
 return cond==-1?_l("lower than"):_l("higher than");
}

bool TresampleSettings::isResample(const TsampleFormat &fmt) const
{
 if (fmt.freq==(unsigned int)freq) return false;
 return _if==0 || (_if==1 && ((cond==-1 && fmt.freq<(unsigned int)condFreq) || (cond==1 && fmt.freq>(unsigned int)condFreq)));
}

bool TresampleSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("%s resampling to %i Hz"),getResamplerName(mode),freq);
 if (_if)
  strncatf(tipS,len,_l(" if sample rate is %s %i Hz"),getCondStr(cond),condFreq);
 return true;
}
