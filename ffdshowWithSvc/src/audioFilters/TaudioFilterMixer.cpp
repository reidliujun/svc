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
#include "TaudioFilterMixer.h"
#include "TmixerSettings.h"
#include "mixer.h"

TaudioFilterMixer::TaudioFilterMixer(IffdshowBase *Ideci,Tfilters *Iparent):TaudioFilter(Ideci,Iparent)
{
 mixerPCM16.deciA=mixerPCM32.deciA=mixerFloat.deciA=deciA;
 matrixPtr=NULL;
 oldnchannels=0;
 oldchannelmask=0xffffffff;
 oldsf=TsampleFormat::SF_NULL;
 oldcfg.out=-1;
}

bool TaudioFilterMixer::getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg0)
{
 if (super::getOutputFmt(fmt,cfg0))
  {
   ((const TmixerSettings*)cfg0)->setFormatOut(fmt);
   return true;
  }
 else
  return false;
}

HRESULT TaudioFilterMixer::process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0)
{
 const TmixerSettings *cfg=(const TmixerSettings*)cfg0;

 samples=init(cfg,fmt,samples,numsamples);
 bool wasChanged = oldnchannels!=fmt.nchannels || oldchannelmask!=fmt.channelmask || oldsf!=fmt.sf || !cfg->equal(oldcfg);
 if (wasChanged)
  {
   oldnchannels=fmt.nchannels;
   oldchannelmask=fmt.channelmask;
   oldsf=fmt.sf;
   oldcfg=*cfg;
  }
 
 switch (fmt.sf)
  {
   case TsampleFormat::SF_PCM16  :mixerPCM16.process(fmt,(int16_t*&)samples,numsamples,cfg,&matrixPtr,&inmask,&outmask,wasChanged);break;
   case TsampleFormat::SF_PCM32  :mixerPCM32.process(fmt,(int32_t*&)samples,numsamples,cfg,&matrixPtr,&inmask,&outmask,wasChanged);break;
   case TsampleFormat::SF_FLOAT32:mixerFloat.process(fmt,(float*&)  samples,numsamples,cfg,&matrixPtr,&inmask,&outmask,wasChanged);break;
  }
 return parent->deliverSamples(++it,fmt,samples,numsamples);
}

HRESULT TaudioFilterMixer::queryInterface(const IID &iid,void **ptr) const
{
 if (iid==IID_IaudioFilterMixer) {*ptr=(IaudioFilterMixer*)this;return S_OK;}
 else return E_NOINTERFACE;
}

STDMETHODIMP TaudioFilterMixer::getMixerMatrixData(double Imatrix[6][6])
{
 if (!matrixPtr) return E_UNEXPECTED;
 if (!Imatrix) return E_POINTER;
 CAutoLock lock(&csMatrix);
 for (int i=0 ; i<6 ; i++)
  for (int j=0 ; j<6 ; j++)
   Imatrix[i][j]=(*matrixPtr)[i][j];
 return S_OK;
}

STDMETHODIMP TaudioFilterMixer::getMixerMatrixData2(double Imatrix[9][9],int *inmaskPtr,int *outmaskPtr)
{
 if (!matrixPtr) return E_UNEXPECTED;
 if (!Imatrix) return E_POINTER;
 CAutoLock lock(&csMatrix);
 memcpy(Imatrix,*matrixPtr,sizeof(double)*9*9);
 *inmaskPtr  = inmask;
 *outmaskPtr = outmask;
 return S_OK;
}
