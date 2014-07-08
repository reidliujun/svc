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
#include "TnoiseSettings.h"
#include "TimgFilterNoise.h"
#include "Cnoise.h"
#include "TffdshowPageDec.h"

const char_t* TnoiseSettings::methodsNames[]=
{
 _l("old"),
 _l("avih"),
 _l("mplayer")
};

const TfilterIDFF TnoiseSettings::idffs=
{
 /*name*/      _l("Noise"),
 /*id*/        IDFF_filterNoise,
 /*is*/        IDFF_isNoise,
 /*order*/     IDFF_orderNoise,
 /*show*/      IDFF_showNoise,
 /*full*/      IDFF_fullNoise,
 /*half*/      IDFF_halfNoise,
 /*dlgId*/     IDD_NOISE,
};

TnoiseSettings::TnoiseSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 mplayerTemporal=1;
 mplayerQuality=1;

 static const TintOptionT<TnoiseSettings> iopts[]=
  {
   IDFF_isNoise                   ,&TnoiseSettings::is                   ,0,0,_l(""),1,
     _l("isNoise"),0,
   IDFF_showNoise                 ,&TnoiseSettings::show                 ,0,0,_l(""),1,
     _l("showNoise"),1,
   IDFF_orderNoise                ,&TnoiseSettings::order                ,1,1,_l(""),1,
     _l("orderNoise"),0,
   IDFF_fullNoise                 ,&TnoiseSettings::full                 ,0,0,_l(""),1,
     _l("fullNoise"),0,
   IDFF_halfNoise                 ,&TnoiseSettings::half                 ,0,0,_l(""),1,
     _l("halfNoise"),0,
   IDFF_noiseMethod               ,&TnoiseSettings::method               ,0,2,_l(""),1,
     _l("noiseMethod"),2,
   IDFF_uniformNoise              ,&TnoiseSettings::uniform              ,0,0,_l(""),1,
     _l("uniformNoise"),1,
   IDFF_noisePattern              ,&TnoiseSettings::pattern              ,0,0,_l(""),1,
     _l("noisePattern"),1,
   IDFF_noiseAveraged             ,&TnoiseSettings::averaged             ,0,0,_l(""),1,
     _l("noiseAveraged"),0,
   IDFF_noiseStrength             ,&TnoiseSettings::strength             ,0,255,_l(""),1,
     _l("noiseStrength"),30,
   IDFF_noiseStrengthChroma       ,&TnoiseSettings::strengthChroma       ,0,127,_l(""),1,
     _l("noiseStrengthChroma"),10,
   IDFF_noiseFlickerA             ,&TnoiseSettings::flickerA             ,0,100,_l(""),1,
     _l("noiseFlickerA"),0,
   IDFF_noiseFlickerF             ,&TnoiseSettings::flickerF             ,1,100,_l(""),1,
     _l("noiseFlickerF"),50,
   IDFF_noiseShakeA               ,&TnoiseSettings::shakeA               ,0,15,_l(""),1,
     _l("noiseShakeA"),0,
   IDFF_noiseShakeF               ,&TnoiseSettings::shakeF               ,1,100,_l(""),1,
     _l("noiseShakeF"),8,
   IDFF_noiseLinesA               ,&TnoiseSettings::linesA               ,1,100,_l(""),1,
     _l("noiseLinesA"),10,
   IDFF_noiseLinesF               ,&TnoiseSettings::linesF               ,0,100,_l(""),1,
     _l("noiseLinesF"),0,
   IDFF_noiseLinesC               ,&TnoiseSettings::linesC               ,0,255,_l(""),1,
     _l("noiseLinesC"),127,
   IDFF_noiseLinesTransparency    ,&TnoiseSettings::linesTransparency    ,1,255,_l(""),1,
     _l("noiseLinesTransparency"),127,
   IDFF_noiseScratchesA           ,&TnoiseSettings::scratchesA           ,1,100,_l(""),1,
     _l("noiseScratchesA"),50,
   IDFF_noiseScratchesF           ,&TnoiseSettings::scratchesF           ,0,100,_l(""),1,
     _l("noiseScratchesF"),0,
   IDFF_noiseScratchesC           ,&TnoiseSettings::scratchesC           ,0,255,_l(""),1,
     _l("noiseScratchesC"),127,
   IDFF_noiseScratchesTransparency,&TnoiseSettings::scratchesTransparency,1,255,_l(""),1,
     _l("noiseScratchesTransparency"),127,
   0
  };
 addOptions(iopts);
}

void TnoiseSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (is && show)
  {
   if (!queue.temporary) setOnChange(IDFF_noiseMethod,filters,&Tfilters::onQueueChange);
   switch (method)
    {
     case NOISE_FF:
     case NOISE_AVIH:
      queueFilter<TimgFilterNoise>(filtersorder,filters,queue);
      break;
     case NOISE_MPLAYER:
      queueFilter<TimgFilterNoiseMplayer>(filtersorder,filters,queue);
      break;
    }
   queueFilter<TimgFilterFlicker>(filtersorder,filters,queue);
   queueFilter<TimgFilterShake>(filtersorder,filters,queue);
   queueFilter<TimgFilterFilmLines>(filtersorder,filters,queue);
   queueFilter<TimgFilterScratches>(filtersorder,filters,queue);
  }
}
void TnoiseSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TnoisePage>(&idffs);
}

const int* TnoiseSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={
  IDFF_noiseStrength,IDFF_noiseStrengthChroma,
  IDFF_uniformNoise,IDFF_noisePattern,IDFF_noiseAveraged,
  IDFF_noiseFlickerA,IDFF_noiseFlickerF,
  IDFF_noiseShakeA,IDFF_noiseShakeF,
  IDFF_noiseLinesA,IDFF_noiseLinesF,IDFF_noiseLinesC,IDFF_noiseLinesTransparency,
  IDFF_noiseScratchesA,IDFF_noiseScratchesF,IDFF_noiseScratchesC,IDFF_noiseLinesTransparency,
  0};
 return idResets;
}

bool TnoiseSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 char_t flags[256]=_l("");
 if (uniform || pattern || averaged)
  {
   tsnprintf_s(flags, countof(flags), _TRUNCATE, _l(" (%s%s%s"), uniform ? _l("uniform, ") : _l(""), pattern ? _l("pattern, ") : _l(""), averaged ? _l("averaged, ") : _l(""));
   flags[strlen(flags)-2]='\0';
   strncat_s(flags, countof(flags), _l(")"), _TRUNCATE);
  }
 tsnprintf_s(tipS, len, _TRUNCATE, _l("method:%s%s\nluma strength:%i, chroma strength:%i%s%s%s%s"),methodsNames[method],flags,strength,strengthChroma,flickerA?_l("\nflickering"):_l(""),shakeA?_l("\nshaking"):_l(""),linesF?_l("\nlines"):_l(""),scratchesF?_l("\nscratches"):_l(""));
 tipS[len-1]='\0';
 return true;
}
