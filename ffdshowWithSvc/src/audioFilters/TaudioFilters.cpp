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
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"
#include "TaudioFilter.h"
#include "TaudioFilterVolume.h"
#include "TaudioFilterOutput.h"
#include "TaudioFilterOSD.h"
#include "TpresetSettingsAudio.h"
#include "TglobalSettings.h"
#include "ToutputAudioSettings.h"

//========================================== TaudioFilters ================================================
TaudioFilters::TaudioFilters(IffdshowBase *Ideci,IprocAudioSink *Isink):Tfilters(Ideci),sink(Isink)
{
 deciA=deci;
 output=getFilter<TaudioFilterOutput>();
 postgainCfg=NULL;postgainVol=NULL;
}
TaudioFilters::~TaudioFilters()
{
 if (postgainCfg) delete postgainCfg;
 if (postgainVol)
  {
   postgainVol->done();
   delete postgainVol;
  }
}

HRESULT TaudioFilters::process(const TsampleFormat &Ifmt,void *samples,size_t numsamples,const TpresetAudio *cfg,float postgain)
{
 if (postgain!=1.0)
  {
   if (!postgainVol)
    {
     postgainVol=new TaudioFilterVolume(deci,this);
     postgainCfg=new TvolumeSettings(new TintStrColl);
     postgainCfg->reset();
     postgainCfg->normalize=0;
     postgainCfg->is=1;
    }
   postgainCfg->vol=int(postgain*100);
  }

 deci->lock(IDFF_lockPresetPtr);
 if (queueChanged)
  {
   makeQueue(cfg,queue);
   if (postgainVol) queue.add(postgainVol,postgainCfg);
   InterlockedDecrement((LONG*)&queueChanged);
   deciD->sendOnNewFiltersMsg();
  }
 queue.copyCfg(cfg);
 deci->unlock(IDFF_lockPresetPtr);

 preferredsfs=cfg->preferredsfs;

 TsampleFormat fmt=Ifmt;
 return deliverSamples(queue.begin(),fmt,samples,numsamples);
}

HRESULT TaudioFilters::deliverSamples(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples)
{
 if (it==queue.end())
  return sink->deliverProcessedSample(samples,numsamples,fmt);
 else
  {
   TaudioFilter *filter=(TaudioFilter*)it->filter;
   const TfilterSettingsAudio *cfg=(const TfilterSettingsAudio*)it->getCfg();
   if (samples && numsamples)
    return filter->process(it,fmt,samples,numsamples,cfg);
   else
    return filter->flush(it,fmt,cfg);
  }
}

void TaudioFilters::getOutputFmt(TsampleFormat &fmt,const TpresetAudio *cfg)
{
 TfilterQueue queue(true);
 makeQueue(cfg,queue);
 queue.copyCfg(cfg);
 preferredsfs=cfg->preferredsfs;
 for (TfilterQueue::iterator f=queue.begin();f!=queue.end();f++)
  ((TaudioFilter*)f->filter)->getOutputFmt(fmt,(const TfilterSettingsAudio*)f->getCfg());
}

//========================================== TaudioFiltersPlayer ==========================================
TaudioFiltersPlayer::TaudioFiltersPlayer(IffdshowBase *Ideci,IprocAudioSink *Isink,TpresetAudio *presetSettings):TaudioFilters(Ideci,Isink)
{
 deci->getGlobalSettings((TglobalSettingsBase**)&globalCfg);
 outsfs=presetSettings->output->outsfs;
 TaudioFilterOSD* f=getFilter<TaudioFilterOSD>();
 osd=f;
 f->setOutsfs(outsfs);
}

void TaudioFiltersPlayer::makeQueue(const Tpreset *cfg,TfilterQueue &queue)
{
 TaudioFilters::makeQueue(cfg,queue);
 queue.add(osd,&globalCfg->osd);
}
