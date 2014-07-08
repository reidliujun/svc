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
#include "TinputPin.h"
#include "Tcodec.h"
#include "ffdebug.h"
#include "dsutil.h"

TinputPin::TinputPin(const char_t* objectName,CTransformFilter *filter,HRESULT* phr,LPWSTR pinname):
 CDeCSSInputPin(objectName,filter,phr,pinname),
 codec(NULL)
{
 strippacket=false;
 wasGetSourceName=false;
}

HRESULT TinputPin::SetMediaType(const CMediaType* mt)
{
 DPRINTF(_l("TinputPin::SetMediaType"));
 HRESULT hr=CDeCSSInputPin::SetMediaType(mt);
 return hr!=S_OK?hr:(init(*mt)?S_OK:E_FAIL);
}

HRESULT TinputPin::Disconnect(void)
{
 HRESULT hr=CDeCSSInputPin::Disconnect();
 if (hr==S_OK)
  {
   done();
   filesourceFlnm = _l("");
   wasGetSourceName=false;
  }
 return hr;
}

STDMETHODIMP TinputPin::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
 HRESULT hr=CDeCSSInputPin::NewSegment(tStart,tStop,dRate);
 if (hr==S_OK && codec)
  codec->onSeek(tStart);
 return hr;
}

STDMETHODIMP TinputPin::EndOfStream(void)
{
 if (codec)
  {
   HRESULT hr=codec->flush();
   if (FAILED(hr))
    return hr;
  }
 return CDeCSSInputPin::EndOfStream();
}

const char_t* TinputPin::getDecoderName(void)
{
 return codec?codec->getName():_l("unknown");
}

HRESULT TinputPin::getEncoderInfo(char_t *buf,size_t buflen)
{
 if (!buf) return E_POINTER;
 if (codec)
  {
   codec->getEncoderInfo(buf,buflen);
   return S_OK;
  }
 else
  {
   buf[0]='\0';
   return S_FALSE;
  }
}

struct TpinFileSourceComp
{
 bool operator ()(IBaseFilter *bff,IPin*) const
  {
   comptr<IFileSourceFilter> ifsf=NULL;
   return SUCCEEDED(bff->QueryInterface(IID_IFileSourceFilter,(void**)&ifsf)) && ifsf!=NULL;
  }
};

const char_t* TinputPin::getFileSourceName(void)
{
 IFilterGraph *graph=m_pFilter->GetFilterGraph();
 if (!graph || wasGetSourceName || !filesourceFlnm.empty())
  return filesourceFlnm.c_str();
 wasGetSourceName=true;
 comptr<IBaseFilter> filter;
 if (searchPrevNextFilter(PINDIR_INPUT,this,this,&filter,TpinFileSourceComp()) && filter)
  {
   comptr<IFileSourceFilter> ifsf;filter->QueryInterface(IID_IFileSourceFilter,(void**)&ifsf);
   LPOLESTR aviNameL=NULL;
   ifsf->GetCurFile(&aviNameL,NULL);
   if (aviNameL)
    {
     filesourceFlnm = text<char_t>(aviNameL);
     CoTaskMemFree(aviNameL);
    }
   else
    filesourceFlnm=_l("");
  }
 return filesourceFlnm.c_str();
}

HRESULT TinputPin::getStreamName(char_t *buf,size_t buflen)
{
 if (!buf || buflen==0) return E_POINTER;
 *buf=0;
 HRESULT hr=getInCodecString(buf,buflen);
 if (FAILED(hr)) return hr;
 char_t pname[256];
 hr=connectedTo(pname,(int)countof(pname));
 if (FAILED(hr)) return hr;
 if (pname[0]=='\0') return S_OK;
 char_t buf2[256];
 tsnprintf_s(buf2, 256, _TRUNCATE,_l("%s - %s"),pname,buf);buf2[255]='\0';
 ff_strncpy(buf,buf2,buflen);
 buf[buflen-1]='\0';
 return S_OK;
}

HRESULT TinputPin::connectedTo(char_t *buf,size_t buflen)
{
 if (!buf || buflen==0) return E_POINTER;
 *buf='\0';
 if (!m_Connected) return E_UNEXPECTED;
 PIN_INFO pi;
 m_Connected->QueryPinInfo(&pi);
 text<char_t>(pi.achName, -1, buf, buflen);
 if (pi.pFilter) pi.pFilter->Release();
 return S_OK;
}

bool TinputPin::onSeek(REFERENCE_TIME segmentStart)
{
 if (codec)
  return codec->onSeek(segmentStart);
 else
  return false;
} 