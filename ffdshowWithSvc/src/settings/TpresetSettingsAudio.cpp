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
#include "TpresetSettingsAudio.h"
#include "TvolumeSettings.h"
#include "TeqSettings.h"
#include "Twinamp2settings.h"
#include "TmixerSettings.h"
#include "TfreeverbSettings.h"
#include "TresampleSettings.h"
#include "TdolbyDecoderSettings.h"
#include "TdelaySettings.h"
#include "TfirSettings.h"
#include "TaudioDenoiseSettings.h"
#include "TcrystalitySettings.h"
#include "TlfeCrossoverSettings.h"
#include "TchannelSwapSettings.h"
#include "TconvolverSettings.h"
#include "ToutputAudioSettings.h"
#include "ffdshow_constants.h"
#include "TsampleFormat.h"
#include "IffdshowDecAudio.h"
#include "Teval.h"

//========================= TaudioAutoPresetProps =========================
TaudioAutoPresetProps::TaudioAutoPresetProps(IffdshowBase *Ideci):
 TautoPresetProps(Ideci),
 deciA(Ideci),
 freq(0)
{
 nchannels[0]='\0';
}
const char_t* TaudioAutoPresetProps::getNchannels(void)
{
 if (nchannels[0]=='\0')
  {
   unsigned int nch;
   if (deciA->inputSampleFormat(&nch,NULL)==S_OK)
    _itoa(nch,nchannels,10);
  }
 return nchannels;
}

const char_t* TaudioAutoPresetProps::getFreq(void)
{
 if (freq==0)
  {
   unsigned int freqI;
   if (deciA->inputSampleFormat(NULL,&freqI)==S_OK)
    freq=freqI;
  }
 return _l("freq");
}
bool TaudioAutoPresetProps::freqMatch(const char_t *expr,const char_t *)
{
 Teval::Tvariable vars[]={{_l("freq"),&freq},NULL};
 const char_t *err;
 double res=Teval(expr,vars)(&err);
 return !err && !!res;
}

//============================== TpresetAudio =============================
TpresetAudio::TpresetAudio(const char_t *IpresetName,const char_t *Ireg_child,int filtermode):
 Tpreset(Ireg_child,IpresetName,0,filtermode)
{
 static const TintOptionT<TpresetAudio> iopts[]=
  {
   IDFF_preferredsfs ,&TpresetAudio::preferredsfs,1,1,_l(""),1,
     _l("preferredsfs"),TsampleFormat::SF_ALL,

   IDFF_dithering    ,&TpresetAudio::dithering    ,0,0,_l(""),1,
     _l("dithering"),0,
   IDFF_noiseShaping ,&TpresetAudio::noiseShaping ,0,3,_l(""),1,
     _l("noiseShaping"),0,
   IDFF_audio_decoder_DRC,&TpresetAudio::decoderDRC   ,0,0,_l(""),1,
     _l("decoderDRC")  ,0,
   IDFF_audio_decoder_DRC_Level,&TpresetAudio::decoderDRCLevel   ,0,0,_l(""),1,
     _l("decoderDRCLevel"),100,
   0
  };
 addOptions(iopts);

 static const char_t *freqHelp=_l("Enter logical expression with 'freq' variable and comparison and arithmetic operators,\nfor example \"44100<=freq AND 48000<=freq\".");
 static const TautoPresetItemDef autoPresetItems[]=
  {
   {
    _l("on number of channels match"),NULL,
    _l("autoloadNchannel"),0,
    _l("autoloadNchannels"),_l(""),
    &TautoPresetProps::stricoll,
    (TautoPresetItemDef::TgetValFc)&TaudioAutoPresetProps::getNchannels,
    NULL
   },
   {
    _l("on sampling frequency match"),NULL,
    _l("autoloadFreq"),0,
    _l("autoloadFreqs"),_l(""),
    (TautoPresetItemDef::TcompareFc)&TaudioAutoPresetProps::freqMatch,
    (TautoPresetItemDef::TgetValFc)&TaudioAutoPresetProps::getFreq,
    NULL,freqHelp
   },
   0
  };
 addAutoPresetItems(autoPresetItems);

 new TdolbyDecoderSettings(options,filters);
 new TvolumeSettings(options,filters);
 new TeqSettings(options,filters);
 new TfirSettings(options,filters);
 new TconvolverSettings(options,filters);
 new TaudioDenoiseSettings(options,filters);
 new Twinamp2settings(options,filters);
 new TfreeverbSettings(options,filters);
 new TcrystalitySettings(options,filters);
 new TresampleSettings(options,filters);
 new TdelaySettings(options,filters);
 new TlfeCrossoverSettings(options,filters);
 new TchannelSwapSettings(options,filters);
 new TmixerSettings(options,filters);
 output=new ToutputAudioSettings(options,filters);
 output->order=filters->maxOrder()+1;
}
