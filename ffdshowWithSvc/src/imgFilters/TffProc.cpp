/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include "TffDecoderVideo.h"
#include "TffProc.h"
#include "TffPict.h"
#include "TpresetSettings.h"
#include "Tconvert.h"
#include "ffdshow_constants.h"
#include "ffImgfmt.h"
#include "TpresetSettingsVideo.h"
#include "TimgFilterOutput.h"
#include "ToutputVideoSettings.h"

class TimgFiltersProc :public TimgFilters
{
private:
 TimgFilterOutputConvert outputConvert;ToutputVideoConvertSettings outputConvertCfg;
protected:
 virtual void makeQueue(const Tpreset *cfg,TfilterQueue &queue)
  {
   TimgFilters::makeQueue(cfg,queue);
   queue.add(&outputConvert,&outputConvertCfg);
  }
public:
 TimgFiltersProc(IffdshowBase *Ideci,IprocVideoSink *Isink):TimgFilters(Ideci,Isink),outputConvert(Ideci,this)
  {
  }
 HRESULT processConvert(TffPict &pict,const TpresetVideo *cfg,int outcsp)
  {
   outputConvertCfg.csp=outcsp;
   return TimgFilters::process(pict,cfg);
  }
};

class TffdshowDecVideoProc :public TffdshowDecVideo
{
private:
 IprocVideoSink *procVideoSink;
protected:
 TimgFilters* createImgFilters(void)
  {
   return new TimgFiltersProc(this,procVideoSink);
  }
public:
 TffdshowDecVideoProc(HRESULT *phr,IprocVideoSink* iprocVideoSink):
  TffdshowDecVideo(CLSID_FFDSHOWVFW,NAME("TffdshowDecVideoProc"),CLSID_TFFDSHOWPAGEPROC,IDS_FFDSHOWVIDEOPROC,IDI_FFDSHOW,NULL,phr,IDFF_FILTERMODE_VIDEO|IDFF_FILTERMODE_PROC,MERIT_DO_NOT_USE,new TintStrColl),
  procVideoSink(iprocVideoSink)
  {
   AddRef();
  }
 void release(void)
  {
   if (imgFilters) delete imgFilters;
   Release();
  }

 void onStop()
  {
   if (imgFilters) imgFilters->onStop();
  }

 HRESULT processPict(TffPict &pict,int outcsp)
  {
   if (!imgFilters) imgFilters=createImgFilters();
   return ((TimgFiltersProc*)imgFilters)->processConvert(pict,presetSettings,outcsp);
  }
};

//==================================================================
CUnknown* WINAPI TffProcVideo::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffProcVideo *pNewObject=new TffProcVideo(punk,phr);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
STDMETHODIMP TffProcVideo::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 CheckPointer(ppv, E_POINTER);
 if (riid==IID_IffProc6 || riid==IID_IffProc7 || riid==IID_IffProc8)
  return GetInterface<IffProc>(this,ppv);
 else
  return CUnknown::NonDelegatingQueryInterface(riid,ppv);
}

TffProcVideo::TffProcVideo(LPUNKNOWN punk,HRESULT *phr):CUnknown(NAME("TffProcVideo"),punk),TffProcBase(IDFF_FILTERMODE_VIDEO)
{
 HRESULT hr=S_OK;
 proc=new TffdshowDecVideoProc(&hr,this);
 initDeci((IffdshowBase*)proc);
 convert=NULL;outcspInfo=NULL;
}
TffProcVideo::~TffProcVideo()
{
 end();
 proc->release();
}

STDMETHODIMP TffProcVideo::getNewSize(unsigned int srcDx,unsigned int srcDy,unsigned int *outDx,unsigned int *outDy)
{
 return proc->calcNewSize(srcDx,srcDy,outDx,outDy);
}

STDMETHODIMP TffProcVideo::begin(unsigned int srcDx,unsigned int srcDy,int IfpsNum,int IfpsDen)
{
 end();
 fpsNum=IfpsNum;fpsDen=IfpsDen;
 srcR=Trect(0,0,srcDx,srcDy);
 return S_OK;
}
STDMETHODIMP TffProcVideo::end(void)
{
 if (proc) proc->onStop();
 if (convert) delete convert;convert=NULL;
 dstbuf.clear();
 return S_OK;
}

STDMETHODIMP TffProcVideo::process(unsigned int framenum,int incsp,const unsigned char *src[4],stride_t srcStride[4],int ro,int outcsp,unsigned char *dst[4],stride_t dstStride[4])
{
 return processTime(framenum,REF_SECOND_MULT*fpsDen*framenum/fpsNum,REF_SECOND_MULT*fpsDen*(framenum+1)/fpsNum,incsp,src,srcStride,ro,outcsp,dst,dstStride);
}
STDMETHODIMP TffProcVideo::processTime(unsigned int framenum,int64_t ref_start,int64_t ref_stop,int incsp,const unsigned char *src[4],stride_t srcStride[4],int ro,int Ioutcsp,unsigned char *Idst[4],stride_t IdstStride[4])
{
 if (!Idst || !IdstStride) return S_FALSE;
 Trect r=srcR;
 TffPict pict(incsp,(unsigned char**)src,srcStride,r,!!ro,FRAME_TYPE::I,FIELD_TYPE::PROGRESSIVE_FRAME,0,NULL);
 pict.rtStart=ref_start;
 pict.rtStop=ref_stop;
 outcsp=Ioutcsp;dst=Idst;dstStride=IdstStride;
 return processPict(framenum,pict,outcsp);
}
HRESULT TffProcVideo::processPict(unsigned int framenum,TffPict &pict,int outcsp)
{
 deci->putParam(IDFF_currentFrame,framenum);
 return proc->processPict(pict,outcsp);
}

STDMETHODIMP TffProcVideo::deliverProcessedSample(TffPict &pict)
{
 if (!outcspInfo || outcspInfo->id!=(outcsp&FF_CSPS_MASK))
  outcspInfo=csp_getInfo(outcsp);
 if (pict.csp!=outcsp)
  {
   if (!convert) convert=new Tconvert(deci,pict.rectFull.dx,pict.rectFull.dy);
   if (dst[0])
    convert->convert(pict,outcsp,dst,dstStride);
   else
    {
     pict.convertCSP(outcsp,dstbuf,convert);
     for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
      {
       dst[i]=pict.data[i];
       dstStride[i]=pict.stride[i];
      }
    }
  }
 else
  if (dst[0])
   for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
    TffPict::copy(dst[i],dstStride[i],pict.data[i],pict.stride[i],(pict.rectFull.dx*outcspInfo->Bpp)>>outcspInfo->shiftX[i],pict.rectFull.dy>>outcspInfo->shiftY[i]);
  else
   for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
    {
     dst[i]=pict.data[i];
     dstStride[i]=pict.stride[i];
    }
 return S_OK;
}

STDMETHODIMP_(const TcspInfo*) TffProcVideo::getCspInfo(int csp)
{
 return csp_getInfo(csp);
}
