/*
 * Copyright (c) 2004-2006 Milan Cutka
 * derived from XMMS plugin for dymanic noise filtering, Copyright (C) 2001 by Rowland
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
#include "TaudioFilterDenoise.h"

const int TaudioFilterDenoise::filtcount[BANDS-1]={10, 8, 6, 4, 3, 2, 1};

TaudioFilterDenoise::TaudioFilterDenoise(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 oldthreshold=0;ethreshold=0;
}

HRESULT TaudioFilterDenoise::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples0,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TaudioDenoiseSettings *cfg=(const TaudioDenoiseSettings*)cfg0;

 if (oldthreshold!=cfg->threshold)
  {
   done();
   oldthreshold=cfg->threshold;
   // take the exponent, more or less..
   ethreshold= 1;  ethreshold <<= (cfg->threshold/2);
   if (cfg->threshold& 1)  ethreshold= (ethreshold*3)/2;
   onSeek();
  }

 int16_t *samples=(int16_t*)init(cfg,fmt,samples0,numsamples);
 // do the work...
 for (unsigned int i=0;i<numsamples;i++)
  for (unsigned int channel=0;channel<fmt.nchannels;channel++)
   {
    int32_t inf[BANDS];memset(inf,0,sizeof(inf));
    // ...input voxels are 16 bits, but we'll use 32 to avoid rounding errors
    int32_t in = samples[fmt.nchannels*i+channel]*PRECISION;   /* for better precision */

    int32_t data_= 0;
    register int j;

    // filter each frequency band...
    for (int band=0;band<BANDS;band++)
     {
      FilterABand* pB= &Channel[channel].B[band];
      register int32_t inf_;
      int32_t threshold_= threshold[band]* PRECISION;
      int32_t* pprevs= pB->prevs;
      unsigned long squelchgain= pB->squelchgain;
      unsigned long safesquelch;
      if (band<(BANDS-1))
       {
        register int32_t in_;
        register int32_t * pprev_;  int j0;
        inf_= inf[band];
        in_ = band? inf_ : in; // previous filter's output
        // ...voxel input to the filter
        inf_= in_; // the lowpass part
        j0= MAXPREVS-filtcount[band];  pprev_= &pprevs[j0];
        for (j= j0; j<MAXPREVS; j++)
          { inf_+= *pprev_;  pprev_++; }
        inf_ /= filtcount[band]+1;
        inf[band]= inf_;
        inf[band+1]= in_ - inf_;  // the highpass part
      }

      inf_ = inf[band];
      int32_t out_= inf_;
      if (!pB->squelching)
       {
        if (squelchgain< SQUELCHMULT)
         squelchgain+= SQUELCHMULT/256; // come out of squelch quickly so we don't blunt attacks
        if ((inf_< threshold_) && (inf_> -threshold_))
         {
          // squelchable voxel
          pB->squelching= 1;
          if (squelchgain) squelchgain--;
         }
       }
      if (pB->squelching)
       {
        if (squelchgain) squelchgain--;
        if ((inf_> threshold_) || (inf_< -threshold_))
         {
          pB->squelching= 0;  // kick out of squelch mode
         }
       }

      safesquelch= squelchgain*HEADROOM;  safesquelch/= SQUELCHMULT;
      out_ *= safesquelch;  out_/= HEADROOM;

      pB->squelchgain= squelchgain;
      data_ += out_;  // reassemble the bands here, after filtering

      if (band< (BANDS-1))
       {
        // shift the prevs for next time...
        register int32_t * pprev_, * pprev_1;  int j0;
        j0= MAXPREVS-filtcount[band];
        pprev_= &pprevs[j0]; pprev_1= pprev_+1;
        for (j= j0; j< MAXPREVS-1; j++)
         {
          *pprev_= *pprev_1;  pprev_++; pprev_1++;
         }
        pprevs[MAXPREVS-1]= band? inf[band-1] : in;
       }
     }
    samples[fmt.nchannels*i+channel] = int16_t(data_ / PRECISION);  // reverse *PRECISION above
   }
 return parent->deliverSamples(++it,fmt,samples,numsamples);
}

void TaudioFilterDenoise::onSeek(void)
{
 for (int i=0;i<8;i++) memset(&Channel[i],0,sizeof(Channel[0]));
 threshold[0]= 0;
 threshold[1]= ethreshold / 3;
 threshold[2]= ethreshold / 2;
 for (int band=3;band<BANDS; band++)
  threshold[band]= ethreshold;
}
