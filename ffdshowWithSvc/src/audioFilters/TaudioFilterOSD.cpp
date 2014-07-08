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
#include "TaudioFilterOSD.h"
#include "ToutputAudioSettings.h"
#include "TOSDsettings.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecVideo.h"
#include "dsutil.h"

TaudioFilterOSD::TaudioFilterOSD(IffdshowBase *Ideci,Tfilters *Iparent):
 TaudioFilter(Ideci,Iparent),
 lasttime(-CLOCKS_PER_SEC*10),registered(NULL),
 oldIs(-1),
 TOSDprovider(deci,deciD),
 sfi(NULL)
{
}

TaudioFilterOSD::~TaudioFilterOSD()
{
 unregister();
 if (sfi)
  delete sfi;
}

bool TaudioFilterOSD::is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg0)
{
 const TOSDsettingsAudio *cfg=(const TOSDsettingsAudio*)cfg0;
 return cfg->is || cfg->getFormat(0);
}

HRESULT TaudioFilterOSD::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TOSDsettingsAudio *cfg=(const TOSDsettingsAudio*)cfg0;tempcfg=cfg;

 clock_t curtime=-1;
 if (!registered && (cfg->is && (oldIs!=cfg->is || (curtime=clock())-lasttime>CLOCKS_PER_SEC*10))) //try every 10 seconds
  {
   if (curtime==-1) curtime=clock();
   lasttime=curtime;
   IFilterGraph *graph=NULL;
   if (deci->getGraph(&graph)==S_OK && graph)
    {
     comptr<IffdshowDecVideo> deciV;
     if (!sfi)
      sfi=new TsearchInterfaceInGraph(graph, getGUID<IffdshowDecVideo>(), searchFilterInterface);
     if (sfi && sfi->getResult((IUnknown**)&deciV) && deciV && SUCCEEDED(deciV->registerOSDprovider(this,"Audio track:")))
      registered=deciV;
     if (sfi->waitSucceeded())
      oldIs=cfg->is;
    }
  }
 return parent->deliverSamples(++it,fmt,samples,numsamples);
}

void TaudioFilterOSD::unregister(void)
{
 if (registered)
  {
   IFilterGraph *graph=NULL;
   if (deci->getGraph(&graph)==S_OK && graph)
    {
     comptr<IffdshowDecVideo> deciV;
     if (sfi && sfi->getResult((IUnknown**)&deciV))
      if (deciV==registered)
       deciV->unregisterOSDprovider(this);
    }
   registered=NULL;
  }
}

void TaudioFilterOSD::onDisconnect(PIN_DIRECTION dir)
{
 unregister();
}

bool TaudioFilterOSD::getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg)
{
 return true;
}
