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
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"
#include "TffProcAudio.h"
#include "TaudioFilters.h"
#include "TpresetSettings.h"
#include "ffdshow_constants.h"
#include "reg.h"
#include "TsampleFormat.h"

CUnknown* WINAPI TffProcAudio::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffProcAudio *pNewObject=new TffProcAudio(punk,phr);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
STDMETHODIMP TffProcAudio::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 CheckPointer(ppv, E_POINTER);
 if (riid==IID_IffProcAudio)
  return GetInterface<IffProcAudio>(this,ppv);
 else
  return CUnknown::NonDelegatingQueryInterface(riid,ppv);
}

TffProcAudio::TffProcAudio(LPUNKNOWN punk,HRESULT *phr):CUnknown(NAME("TffProcAudio"),punk),TffProcBase(IDFF_FILTERMODE_AUDIO)
{
 if (SUCCEEDED(CoCreateInstance(CLSID_FFDSHOWAUDIO,NULL,CLSCTX_INPROC_SERVER,getGUID<IffdshowDecAudio>(),(void**)&deciA)))
  initDeci(deciA);
 audioFilters=NULL;
}
TffProcAudio::~TffProcAudio()
{
 end();
 if (audioFilters) delete audioFilters;
}

void TffProcAudio::initAudioFilters(void)
{
 if (!audioFilters)
  {
   audioFilters=new TaudioFilters(deci,this);
   deciA->setAudioFilters(audioFilters);
  }
}

STDMETHODIMP TffProcAudio::begin(const WAVEFORMATEX *wfIn,WAVEFORMATEX *wfOut)
{
 end();
 deciD->getPresetPtr((Tpreset**)&presetSettings);
 initAudioFilters();
 TsampleFormat sf(*wfIn);
 audioFilters->getOutputFmt(sf,presetSettings);
 WAVEFORMATEXTENSIBLE wfext=sf.toWAVEFORMATEXTENSIBLE();
 *wfOut=wfext.Format;
 return S_OK;
}
STDMETHODIMP TffProcAudio::end(void)
{
 return S_OK;
}
STDMETHODIMP TffProcAudio::process(const WAVEFORMATEX *wfIn,size_t innumsamples,WAVEFORMATEX *wfOut,size_t *outnumsamples,void* *samples)
{
 WAVEFORMATEXTENSIBLE wfexIn;wfexIn.Format=*wfIn;wfexIn.SubFormat=GUID_NULL;wfexIn.dwChannelMask=0;
 WAVEFORMATEXTENSIBLE wfexOut;
 HRESULT hr=processEx(&wfexIn,innumsamples,&wfexOut,outnumsamples,samples);
 if (hr==S_OK)
  *wfOut=wfexOut.Format;
 return hr;
}
STDMETHODIMP TffProcAudio::processEx(const WAVEFORMATEXTENSIBLE *wfIn,size_t innumsamples,WAVEFORMATEXTENSIBLE *wfOut,size_t *outnumsamples,void* *samples0)
{
 if (!wfIn || !wfOut || !outnumsamples || !samples0) return E_POINTER;
 if (innumsamples==0) return S_FALSE;
 HRESULT hr=audioFilters->process(*wfIn,*samples0,innumsamples,presetSettings,1);
 if (hr==S_OK)
  {
   *wfOut=sfOut.toWAVEFORMATEXTENSIBLE();
   *outnumsamples=this->outnumsamples;
   *samples0=(void*)outsamples;
  }
 return hr;
}
STDMETHODIMP TffProcAudio::deliverProcessedSample(const void *buf,size_t numsamples,const TsampleFormat &outsf)
{
 sfOut=outsf;
 outnumsamples=(int)numsamples;
 outsamples=buf;
 return S_OK;
}
