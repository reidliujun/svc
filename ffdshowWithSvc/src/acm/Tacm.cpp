/*
 * Copyright (c) 2003-2006 Milan Cutka
 * based on gsm610 sample from Microsoft Windows 2000 DDK
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Tacm.h"
#include "ffdshow_mediaguids.h"
#include "avisynth/Tavisynth.h"
#include "IffdshowDecAudio.h"
#include "TsampleFormat.h"

//========================= TacmCreator ==========================
extern "C" Iffacm2creator* __stdcall ffacm2creator(void)
{
 HRESULT hr;
 TacmCreator *creator=new TacmCreator(NULL,&hr);
 creator->AddRef();
 return creator;
}
CUnknown* WINAPI TacmCreator::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TacmCreator *pNewObject=new TacmCreator(punk,phr);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
STDMETHODIMP TacmCreator::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 CheckPointer(ppv, E_POINTER);
 if (riid==IID_Iffacm2creator)
  return GetInterface<TacmCreator>(this,ppv);
 else
  return CUnknown::NonDelegatingQueryInterface(riid,ppv);
}
TacmCreator::TacmCreator(LPUNKNOWN punk,HRESULT *phr):CUnknown(NAME("TacmCreator"),punk,phr)
{
}
TacmCreator::~TacmCreator()
{
}
STDMETHODIMP_(int) TacmCreator::getSize(void)
{
 return sizeof(Tacm);
}
STDMETHODIMP TacmCreator::create(void *ptr)
{
 if (!ptr) return E_POINTER;
 new(ptr)Tacm;
 return S_OK;
}


//============================= Tacm =============================
const UINT Tacm::gauFormatTagIndexToTag[]=
{
 WAVE_FORMAT_PCM,
 WAVE_FORMAT_AVIS
};
const int Tacm::ACM_DRIVER_MAX_FORMAT_TAGS=countof(Tacm::gauFormatTagIndexToTag);

const int Tacm::ACM_DRIVER_MAX_CHANNELS=2;
const int Tacm::ACM_DRIVER_MAX_FORMATS_PCM=1;

Tacm::Tacm(void)
{
 randomize();
 avisynth=NULL;
}

LRESULT Tacm::driverDetails(LPACMDRIVERDETAILSW details)
{
 DWORD cbStruct=std::min(details->cbStruct,(DWORD)sizeof(ACMDRIVERDETAILSW));
 ACMDRIVERDETAILSW add;
 add.cbStruct=cbStruct;
 add.fccType=ACMDRIVERDETAILS_FCCTYPE_AUDIOCODEC;
 add.fccComp=ACMDRIVERDETAILS_FCCCOMP_UNDEFINED;
 add.wMid=0;
 add.wPid=0;
 add.vdwACM=VERSION_MSACM;
 add.vdwDriver=VERSION_ACM_DRIVER;
 add.fdwSupport=ACMDRIVERDETAILS_SUPPORTF_CODEC;
 add.cFormatTags=ACM_DRIVER_MAX_FORMAT_TAGS;
 add.cFilterTags=0;
 if (FIELD_OFFSET(ACMDRIVERDETAILS,hicon)<cbStruct)
  {
   add.hicon=NULL;
   wcscpy(add.szShortName,L"ffdshow");
   wcscpy(add.szLongName,FFDSHOWAUDIO_NAME_L);
   if (FIELD_OFFSET(ACMDRIVERDETAILS, szCopyright)<cbStruct)
    {
     wcscpy(add.szCopyright,L"2003-2005 Milan Cutka");
     wcscpy(add.szLicensing,L"GNU GPL");
     wcscpy(add.szFeatures,L"audio decoding and processing");
    }
  }
 memcpy(details,&add,(UINT)add.cbStruct);
 return MMSYSERR_NOERROR;
}
LRESULT Tacm::driverAbout(HWND parent)
{
 if (parent==(HWND)-1)
  {
   // this ACM driver does NOT support a custom about box, so
   // return MMSYSERR_NOTSUPPORTED...
   return MMSYSERR_NOTSUPPORTED;
  }
 // this driver does not support a custom dialog, so tell the ACM or
 // calling application to display one for us. note that this is the
 // _recommended_ method for consistency and simplicity of ACM drivers.
 // why write code when you don't have to?
 return MMSYSERR_NOTSUPPORTED;
}

bool Tacm::avisIsValidFormat(LPWAVEFORMATEX pwfx)
{
 if (!pwfx) return false;
 if (pwfx->wFormatTag!=WAVE_FORMAT_AVIS) return false;
 if (pwfx->nChannels<1 || pwfx->nChannels>ACM_DRIVER_MAX_CHANNELS) return false;
 return true;
}
bool Tacm::pcmIsValidFormat(LPWAVEFORMATEX pwfx)
{
 if (!pwfx) return false;
 if (pwfx->wFormatTag!=WAVE_FORMAT_PCM) return false;
 if (pwfx->nChannels<1 || pwfx->nChannels>ACM_DRIVER_MAX_CHANNELS) return false;
 return true;
}

LRESULT Tacm::formatSuggest(LPACMDRVFORMATSUGGEST padfs)
{
 static const DWORD ACMD_FORMAT_SUGGEST_SUPPORT=ACM_FORMATSUGGESTF_WFORMATTAG|ACM_FORMATSUGGESTF_NCHANNELS|ACM_FORMATSUGGESTF_NSAMPLESPERSEC|ACM_FORMATSUGGESTF_WBITSPERSAMPLE;

 DWORD fdwSuggest=padfs->fdwSuggest&ACMD_FORMAT_SUGGEST_SUPPORT;
 if (fdwSuggest&~ACMD_FORMAT_SUGGEST_SUPPORT) return MMSYSERR_NOTSUPPORTED;
 LPWAVEFORMATEX pwfxSrc=padfs->pwfxSrc;
 LPWAVEFORMATEX pwfxDst=padfs->pwfxDst;
 switch (pwfxSrc->wFormatTag)
  {
   case WAVE_FORMAT_PCM:
    break;
   case WAVE_FORMAT_AVIS:
    {
     if (!avisIsValidFormat(pwfxSrc)) return ACMERR_NOTPOSSIBLE;

     if (fdwSuggest&ACM_FORMATSUGGESTF_WFORMATTAG)
      {
       if (pwfxDst->wFormatTag!=WAVE_FORMAT_PCM)
        return ACMERR_NOTPOSSIBLE;
      }
     else
      pwfxDst->wFormatTag=WAVE_FORMAT_PCM;

     if (fdwSuggest&ACM_FORMATSUGGESTF_NCHANNELS)
      {
       if (pwfxSrc->nChannels!=pwfxDst->nChannels)
        return ACMERR_NOTPOSSIBLE;
      }
     else
      pwfxDst->nChannels=pwfxSrc->nChannels;

     if (fdwSuggest&ACM_FORMATSUGGESTF_NSAMPLESPERSEC)
      {
       if (pwfxSrc->nSamplesPerSec!=pwfxDst->nSamplesPerSec)
        return ACMERR_NOTPOSSIBLE;
      }
     else
      pwfxDst->nSamplesPerSec=pwfxSrc->nSamplesPerSec;

     if (fdwSuggest&ACM_FORMATSUGGESTF_WBITSPERSAMPLE)
      {
       if (pwfxDst->wBitsPerSample!=16)
        return ACMERR_NOTPOSSIBLE;
      }
     else
      pwfxDst->wBitsPerSample=16;

     TsampleFormat sf(*pwfxDst);
     pwfxDst->nBlockAlign    =(WORD)sf.blockAlign();
     pwfxDst->nAvgBytesPerSec=sf.avgBytesPerSec();
     pwfxDst->cbSize         =0;
     return MMSYSERR_NOERROR;
    }
  }
 return ACMERR_NOTPOSSIBLE;
}

LRESULT Tacm::formatTagDetails(LPACMFORMATTAGDETAILSW padft,DWORD fdwDetails)
{
 UINT uFormatTag;
 switch (fdwDetails&ACM_FORMATTAGDETAILSF_QUERYMASK)
  {
   case ACM_FORMATTAGDETAILSF_INDEX:
    if (ACM_DRIVER_MAX_FORMAT_TAGS<=padft->dwFormatTagIndex) return ACMERR_NOTPOSSIBLE;
    uFormatTag=gauFormatTagIndexToTag[padft->dwFormatTagIndex];
    break;
   case ACM_FORMATTAGDETAILSF_LARGESTSIZE:
    switch (padft->dwFormatTag)
     {
      case WAVE_FORMAT_UNKNOWN:
      case WAVE_FORMAT_AVIS:
       uFormatTag=WAVE_FORMAT_AVIS;
       break;
      case WAVE_FORMAT_PCM:
       uFormatTag=WAVE_FORMAT_PCM;
       break;
      default:
       return ACMERR_NOTPOSSIBLE;
     }
    break;
   case ACM_FORMATTAGDETAILSF_FORMATTAG:
    switch (padft->dwFormatTag)
     {
      case WAVE_FORMAT_AVIS:
       uFormatTag=WAVE_FORMAT_AVIS;
       break;
      case WAVE_FORMAT_PCM:
       uFormatTag=WAVE_FORMAT_PCM;
       break;
      default:
       return ACMERR_NOTPOSSIBLE;
     }
    break;
   default:
    return MMSYSERR_NOTSUPPORTED;
  }

 switch (uFormatTag)
  {
   case WAVE_FORMAT_PCM:
    padft->dwFormatTagIndex=0;
    padft->dwFormatTag     =WAVE_FORMAT_PCM;
    padft->cbFormatSize    =sizeof(PCMWAVEFORMAT);
    padft->fdwSupport      =ACMDRIVERDETAILS_SUPPORTF_CODEC;
    padft->cStandardFormats=ACM_DRIVER_MAX_FORMATS_PCM;
    //padft->szFormatTag[0]  =L'\0';
    break;

   case WAVE_FORMAT_AVIS:
    padft->dwFormatTagIndex=1;
    padft->dwFormatTag     =WAVE_FORMAT_AVIS;
    padft->cbFormatSize    =sizeof(WAVEFORMATEX);
    padft->fdwSupport      =ACMDRIVERDETAILS_SUPPORTF_CODEC;
    padft->cStandardFormats=1;
    wcscpy(padft->szFormatTag,FFDSHOWAUDIO_NAME_L);
    break;

   default:
    return ACMERR_NOTPOSSIBLE;
  }
 padft->cbStruct=std::min(padft->cbStruct,(DWORD)sizeof(*padft));
 return MMSYSERR_NOERROR;
}

LRESULT Tacm::streamOpen(LPACMDRVSTREAMINSTANCE padsi)
{
 LPWAVEFORMATEX pwfxSrc=padsi->pwfxSrc;
 if (pwfxSrc->wFormatTag!=WAVE_FORMAT_AVIS) return ACMERR_NOTPOSSIBLE;
 LPWAVEFORMATEX pwfxDst=padsi->pwfxDst;
 if (!avisIsValidFormat(pwfxSrc) || !pcmIsValidFormat(pwfxDst)) return ACMERR_NOTPOSSIBLE;
 padsi->fdwDriver=0L;
 padsi->dwDriver=(DWORD_PTR)this;
 if (ACM_STREAMOPENF_QUERY&padsi->fdwOpen) return MMSYSERR_NOERROR; //only querying
 if (!avisynth)
  {
   CMediaType mt;
   mt.formattype=FORMAT_WaveFormatEx;
   mt.SetFormat((BYTE*)pwfxSrc,sizeof(*pwfxSrc)+pwfxSrc->cbSize);
   TsampleFormat fmt;
   avisynth=new TavisynthAudio(mt,fmt,NULL,"ffdshow_acm_avisynth_script");
  }
 if (avisynth->ok)
  {
   bytesPerSample=avisynth->vi->BytesPerAudioSample();
   fps_denominator=avisynth->vi->fps_denominator;
   fps_numerator=avisynth->vi->fps_numerator;
   return MMSYSERR_NOERROR;
  }
 else
  {
   OutputDebugString(_l("ffacm error"));//ffvfw->dbgError(err.msg);
   return ACMERR_NOTPOSSIBLE;
  }
}
LRESULT Tacm::streamClose(LPACMDRVSTREAMINSTANCE padsi)
{
 if (avisynth) delete avisynth;avisynth=NULL;
 return MMSYSERR_NOERROR;
}
LRESULT Tacm::streamSize(LPACMDRVSTREAMINSTANCE padsi,LPACMDRVSTREAMSIZE padss)
{
 LPWAVEFORMATEX pwfxSrc=padsi->pwfxSrc;
 LPWAVEFORMATEX pwfxDst=padsi->pwfxDst;
 switch (ACM_STREAMSIZEF_QUERYMASK&padss->fdwSize)
  {
   case ACM_STREAMSIZEF_SOURCE:
    padss->cbDstLength=10*DWORD(2*bytesPerSample*pwfxDst->nSamplesPerSec*int64_t(fps_denominator)/fps_numerator);
    return MMSYSERR_NOERROR;
   case ACM_STREAMSIZEF_DESTINATION:
    padss->cbSrcLength=16;
    return MMSYSERR_NOERROR;
   default:
    return MMSYSERR_NOTSUPPORTED;
  }
}
LRESULT Tacm::convert(LPACMDRVSTREAMINSTANCE padsi,LPACMDRVSTREAMHEADER padsh)
{
 padsh->cbSrcLengthUsed=16;
 uint64_t s1=*(uint64_t*)padsh->pbSrc,s2=*(uint64_t*)(padsh->pbSrc+8);
 (*avisynth->clip)->GetAudio(padsh->pbDst,s1,s2-s1+1);
 padsh->cbDstLengthUsed=DWORD((s2-s1+1)*bytesPerSample);
 return MMSYSERR_NOERROR;
}
