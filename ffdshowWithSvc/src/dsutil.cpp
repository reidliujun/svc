/*
 * Copyright (c) 2002-2006 Milan Cutka
 * uses many routines developed by Gabest
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
#include "dsutil.h"
#include "TffRect.h"
#include "theora/theora.h"
#include "xiph/vorbis/vorbisformat.h"
#include "ffdshow_mediaguids.h"

// registry key info from RadLight Filter Manager v1.3 by RadScorpion
static void getFilterReg(const CLSID &clsid,char_t *reg, size_t len)
{
 LPOLESTR sclsidW;
 StringFromIID(clsid,&sclsidW);
 //char sclsid[MAX_PATH];
 //WideCharToMultiByte(CP_ACP,0,sclsidW,-1,sclsid,MAX_PATH,NULL,NULL);
 tsnprintf_s(reg, len, _TRUNCATE, _l("\\CLSID\\{083863F1-70DE-11d0-BD40-00A0C911CE86}\\Instance\\%s"),(const char_t*)text<char_t>(sclsidW));
 CoTaskMemFree(sclsidW);
}
struct TfilterReg
{
 DWORD version;
 DWORD merit;
 BYTE b[16384];
};

HRESULT getFilterMerit(HKEY hive,const char_t *reg,DWORD *merit)
{
 HKEY hKey;
 HRESULT res=E_FAIL;
 if (RegOpenKeyEx(hive,reg,0,KEY_READ,&hKey)==ERROR_SUCCESS)
  {
   TfilterReg buf;
   DWORD buflen=sizeof(buf);
   if (RegQueryValueEx(hKey,_l("FilterData"),NULL,NULL,LPBYTE(&buf),&buflen)==ERROR_SUCCESS)
    {
     *merit=buf.merit;
     res=S_OK;
    }
   RegCloseKey(hKey);
  }
 return res;
}
HRESULT getFilterMerit(const CLSID &clsid,DWORD *merit)
{
 if (!merit) return E_POINTER;
 char_t reg[MAX_PATH];
 getFilterReg(clsid, reg, countof(reg));
 return getFilterMerit(HKEY_CLASSES_ROOT,reg,merit);
}

HRESULT setFilterMerit(HKEY hive,const char_t *reg,DWORD merit)
{
 HKEY hKey;
 HRESULT res=E_FAIL;
 if (RegOpenKeyEx(hive,reg,0,KEY_READ|KEY_WRITE,&hKey)==ERROR_SUCCESS)
  {
   TfilterReg buf;
   DWORD buflen=sizeof(buf);
   if (RegQueryValueEx(hKey,_l("FilterData"),NULL,NULL,LPBYTE(&buf),&buflen)==ERROR_SUCCESS)
    {
     buf.merit=merit;
     if (RegSetValueEx(hKey,_l("FilterData"),NULL,REG_BINARY,LPBYTE(&buf),buflen)==ERROR_SUCCESS)
      res=S_OK;
    }
   RegCloseKey(hKey);
  }
 return res;
}
HRESULT setFilterMerit(const CLSID &clsid,DWORD merit)
{
 char_t reg[MAX_PATH];
 getFilterReg(clsid, reg, countof(reg));
 return setFilterMerit(HKEY_CLASSES_ROOT,reg,merit);
}

bool ExtractBIH(const AM_MEDIA_TYPE &mt, BITMAPINFOHEADER* bih)
{
 if (mt.formattype==FORMAT_VideoInfo)
  {
   VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)mt.pbFormat;
   memcpy(bih, &vih->bmiHeader, sizeof(BITMAPINFOHEADER));
   return true;
  }
 else if(mt.formattype == FORMAT_VideoInfo2)
  {
   VIDEOINFOHEADER2* vih = (VIDEOINFOHEADER2*)mt.pbFormat;
   memcpy(bih, &vih->bmiHeader, sizeof(BITMAPINFOHEADER));
   return true;
  }
 if(mt.formattype == FORMAT_MPEGVideo)
  {
   VIDEOINFOHEADER* vih = &((MPEG1VIDEOINFO*)mt.pbFormat)->hdr;
   memcpy(bih, &vih->bmiHeader, sizeof(BITMAPINFOHEADER));
   return true;
  }
 else if(mt.formattype == FORMAT_MPEG2_VIDEO)
  {
   VIDEOINFOHEADER2* vih = &((MPEG2VIDEOINFO*)mt.pbFormat)->hdr;
   memcpy(bih, &vih->bmiHeader, sizeof(BITMAPINFOHEADER));
   return true;
  }
 else if(mt.formattype == FORMAT_TheoraIll)
  {
   sTheoraFormatBlock *oggFormat=(sTheoraFormatBlock*)mt.pbFormat;
   memset(bih,0,sizeof(*bih));
   bih->biCompression=FOURCC_THEO;
   bih->biWidth=oggFormat->width;
   bih->biHeight=oggFormat->height;
   bih->biBitCount=12;
   return true;
  }
 else
  return false;
}

void getFilterName(IBaseFilter *filter,char_t *name,char_t *filtername,size_t bufsize)
{
 name[0]='\0';filtername[0]='\0';
 CLSID iffclsid;
 filter->GetClassID(&iffclsid);
 getCLSIDname(iffclsid,name,256);
 FILTER_INFO iff;
 if (filter->QueryFilterInfo(&iff)==S_OK)
  {
   //WideCharToMultiByte(CP_ACP,0,iff.achName,-1,filtername,MAX_PATH,NULL,NULL);
   text<char_t>(iff.achName, -1, filtername, bufsize);
   if (iff.pGraph) iff.pGraph->Release();
  }
 if (name[0]=='\0') strcpy(name,filtername);
}

pins_vector::~pins_vector()
{
 for (iterator p=begin();p!=end();p++)
  (*p)->Release();
}
pins_vector::pins_vector(IBaseFilter *filter,PIN_DIRECTION dir)
{
 clear();
 comptr<IEnumPins> ep;
 if (SUCCEEDED(filter->EnumPins(&ep)))
  {
   ep->Reset();
   for (IPin *p=NULL;ep->Next(1,&p,NULL)==S_OK;p=NULL)
    {
     PIN_INFO pi;
     if (SUCCEEDED(p->QueryPinInfo(&pi)))
      {
       if (pi.dir==dir)
        {
         p->AddRef();
         push_back(p);
        }
       pi.pFilter->Release();
      }
     p->Release();
    }
  }
}

struct TpinClsidCompare
{
private:
 const CLSID &clsid;
public:
 TpinClsidCompare(const CLSID &Iclsid):clsid(Iclsid) {}
 bool operator()(const CLSID &c,IPin*) const
  {
   return !!(c==clsid);
  }
};

bool searchPrevNextFilter(PIN_DIRECTION direction,IPin *pin,const CLSID &clsid)
{
 return searchPrevNextFilter(direction,pin,pin,NULL,TpinClsidCompare(clsid));
}

template<class Tcompare> bool searchFilterInterface(IFilterGraph *graph,IUnknown* *dest,Tcompare compFc)
{
 if (!graph) return false;
 comptr<IEnumFilters> eff;
 *dest=NULL;
 if (graph->EnumFilters(&eff)==S_OK)
  {
   eff->Reset();
   for (comptr<IBaseFilter> bff;eff->Next(1,&bff,NULL)==S_OK;bff=NULL)
    if (compFc(bff,dest))
     return true;
  }
 return false;
}

struct TfilterInterfaceCmp
{
private:
 const IID &iid;
public:
 TfilterInterfaceCmp(const IID &Iiid):iid(Iiid) {}
 bool operator()(IBaseFilter *f,IUnknown* *dest)
  {
   return f->QueryInterface(iid,(void**)dest)==S_OK && *dest;
  }
};
bool searchFilterInterface(IFilterGraph *graph,const IID &iid,IUnknown* *dest)
{
 return searchFilterInterface(graph,dest,TfilterInterfaceCmp(iid));
}

struct TpinInterfaceCmp
{
private:
 const IID &iid;
public:
 TpinInterfaceCmp(const IID &Iiid):iid(Iiid) {}
 bool operator()(IBaseFilter *f,IUnknown* *dest)
  {
   comptr<IEnumPins> epi;
   if (f->EnumPins(&epi)==S_OK)
    {
     epi->Reset();
     for (comptr<IPin> bpi;epi->Next(1,&bpi,NULL)==S_OK;bpi=NULL)
      {
       if (bpi->QueryInterface(iid,(void**)dest)==S_OK && *dest)
        return true;
      }
    }
   return false;
  }
};
bool searchPinInterface(IFilterGraph *graph,const IID &iid,IUnknown **dest)
{
 return searchFilterInterface(graph,dest,TpinInterfaceCmp(iid));
}

IBaseFilter* searchFilter(IFilterGraph *graph,const CLSID &clsid,IBaseFilter *exclude)
{
 comptr<IEnumFilters> eff;
 if (graph->EnumFilters(&eff)==S_OK)
  {
   eff->Reset();
   for (comptr<IBaseFilter> bff;eff->Next(1,&bff,NULL)==S_OK;bff=NULL)
    if (!exclude || bff!=exclude)
     {
      CLSID bffclsid;
      if (SUCCEEDED(bff->GetClassID(&bffclsid)) && bffclsid==clsid)
       return bff;
     }
  }
 return NULL;
}

CLSID GetCLSID(IBaseFilter* pBF)
{
 CLSID clsid=GUID_NULL;
 if (pBF) pBF->GetClassID(&clsid);
 return clsid;
}

class CPinInfo : public PIN_INFO
{
public:
 CPinInfo() {pFilter=NULL;}
 ~CPinInfo() {if (pFilter) pFilter->Release();}
};

static IBaseFilter* GetFilterFromPin(IPin* pPin)
{
 if (!pPin) return NULL;
 IBaseFilter *pBF=NULL;
 CPinInfo pi;
 if (pPin && SUCCEEDED(pPin->QueryPinInfo(&pi)))
  pBF=pi.pFilter;
 return pBF;
}

CLSID GetCLSID(IPin* pPin)
{
 return GetCLSID(GetFilterFromPin(pPin));
}

void setVIH2aspect(VIDEOINFOHEADER2 *vih2,const Trect &r,int userDAR256)
{
 if (userDAR256)
  {
   vih2->dwPictAspectRatioX=userDAR256>>8;
   vih2->dwPictAspectRatioY=256;
  }
 else
  {
   vih2->dwPictAspectRatioX=r.dar().num;
   vih2->dwPictAspectRatioY=r.dar().den;
   vih2->dwControlFlags=0;
  }
 //DPRINTF("setVIH2aspect: aspectX:%i, aspectY:%i",vih2->dwPictAspectRatioX,vih2->dwPictAspectRatioY);
}

void bih2mediatype(const BITMAPINFOHEADER &bih,CMediaType *mt)
{
 mt->SetType(&MEDIATYPE_Video);
 mt->SetFormatType(&FORMAT_VideoInfo);
 mt->SetTemporalCompression(FALSE);
 mt->SetSampleSize(bih.biSizeImage);
 unsigned int elen = bih.biSize > sizeof(BITMAPINFOHEADER) ? bih.biSize - sizeof(BITMAPINFOHEADER) : 0;
 if (elen==0 && bih.biClrUsed>0)
  elen=sizeof(RGBQUAD)*bih.biClrUsed;
 VIDEOINFOHEADER *vih=(VIDEOINFOHEADER*)mt->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER)+elen);
 if (vih)
  {
   ZeroMemory(vih,sizeof(VIDEOINFOHEADER));
   vih->rcSource.left=0;vih->rcSource.right=bih.biWidth;vih->rcSource.top=0;vih->rcSource.bottom=bih.biHeight;
   vih->rcTarget=vih->rcSource;
   vih->AvgTimePerFrame=0;
   vih->bmiHeader=bih;
   if (elen)
    memcpy(&vih->bmiHeader+1,&bih+1,elen);
  }
}

REFERENCE_TIME getAvgTimePerFrame(const CMediaType &mt)
{
 if (mt.formattype==FORMAT_VideoInfo)
  return ((VIDEOINFOHEADER*)mt.pbFormat)->AvgTimePerFrame;
 else if (mt.formattype==FORMAT_VideoInfo2)
  return ((VIDEOINFOHEADER2*)mt.pbFormat)->AvgTimePerFrame;
 else if (mt.formattype==FORMAT_MPEGVideo)
  return ((MPEG1VIDEOINFO*)mt.pbFormat)->hdr.AvgTimePerFrame;
 else if (mt.formattype==FORMAT_MPEG2Video)
  return ((MPEG2VIDEOINFO*)mt.pbFormat)->hdr.AvgTimePerFrame;
 else if (mt.formattype==FORMAT_TheoraIll)
  {
   const sTheoraFormatBlock *oggFormat=(const sTheoraFormatBlock*)mt.pbFormat;
   return oggFormat->frameRateDenominator*REF_SECOND_MULT/oggFormat->frameRateNumerator;
  }
 else
  return 0;
}

Textradata::Textradata(const CMediaType &mt,int padding):data(NULL),size(0)
{
 const unsigned char *data=NULL;unsigned int size=0;
 if (mt.formattype==FORMAT_WaveFormatEx)
  {
   set(parse((const WAVEFORMATEX*)mt.pbFormat),padding);
   return;
  }
 else if (mt.formattype==FORMAT_VideoInfo)
  {
   size=mt.cbFormat-sizeof(VIDEOINFOHEADER);
   data=size?mt.pbFormat+sizeof(VIDEOINFOHEADER):NULL;
  }
 else if (mt.formattype==FORMAT_VideoInfo2)
  {
   size=mt.cbFormat-sizeof(VIDEOINFOHEADER2);
   data=size?mt.pbFormat+sizeof(VIDEOINFOHEADER2):NULL;
  }
 else if (mt.formattype==FORMAT_MPEGVideo)
  {
   set(parse((const MPEG1VIDEOINFO*)mt.pbFormat),padding);
   return;
  }
 else if (mt.formattype==FORMAT_MPEG2Video)
  {
   set(parse((const MPEG2VIDEOINFO*)mt.pbFormat),padding);
   return;
  }
 else if (mt.formattype==FORMAT_VorbisFormat2)
  {
   const VORBISFORMAT2 *vf2=(const VORBISFORMAT2*)mt.pbFormat;
   size=mt.cbFormat-sizeof(VORBISFORMAT2);
   data=size?mt.pbFormat+sizeof(VORBISFORMAT2):NULL;
  }
 else if (mt.formattype==FORMAT_RLTheora)
  {
   size=mt.cbFormat-sizeof(VIDEOINFOHEADER);
   data=size?mt.pbFormat+sizeof(VIDEOINFOHEADER):NULL;
  }
 set(data,size,padding);
}
Textradata::Textradata(const WAVEFORMATEX &wfex,int padding):data(NULL),size(0)
{
 set(parse(&wfex),0);
}
Textradata::Textradata(const MPEG1VIDEOINFO &mpeg1info,int padding)
{
 set(parse(&mpeg1info),0);
}
Textradata::Textradata(const MPEG2VIDEOINFO &mpeg2info,int padding)
{
 set(parse(&mpeg2info),0);
}

Textradata::TdataSize Textradata::parse(const WAVEFORMATEX *wfex)
{
 return wfex->cbSize?std::make_pair((const unsigned char*)(wfex+1),(size_t)wfex->cbSize):TdataSize();
}
Textradata::TdataSize Textradata::parse(const MPEG1VIDEOINFO *mpeg1info)
{
 return std::make_pair(mpeg1info->cbSequenceHeader?mpeg1info->bSequenceHeader:NULL,mpeg1info->cbSequenceHeader);
}
Textradata::TdataSize Textradata::parse(const MPEG2VIDEOINFO *mpeg2info)
{
 return std::make_pair(mpeg2info->cbSequenceHeader?(const uint8_t*)mpeg2info->dwSequenceHeader:NULL,mpeg2info->cbSequenceHeader);
}

Textradata::~Textradata()
{
 done();
}
void Textradata::done(void)
{
 if (own && data)
  free((void*)data);
}
void Textradata::clear(void)
{
 set(NULL,0);
}
void Textradata::set(const void *Idata,size_t Isize,unsigned int padding,bool Iown)
{
 set(TdataSize((const unsigned char*)Idata,Isize),padding,Iown);
}
void Textradata::set(const TdataSize &dataSize,unsigned int padding,bool Iown)
{
 done();
 if (dataSize.second>0 && (padding>0 || Iown))
  {
   own=true;
   size=dataSize.second;
   data=(const unsigned char*)calloc(1,size+padding);
   memcpy((void*)data,dataSize.first,size);
  }
 else
  {
   own=false;
   data=(const unsigned char*)dataSize.first;
   size=dataSize.second;
  }
}

//==================================== TsearchInterfaceInGraph ========================================
unsigned int __stdcall TsearchInterfaceInGraph::searchFilterInterfaceThreadEntry(void *self0)
{
 TsearchInterfaceInGraph *self = (TsearchInterfaceInGraph*)self0;
 return (unsigned int)self->searchInterfaceFunc(self->graph,self->iid,&(self->dest));
}

bool TsearchInterfaceInGraph::getResult(IUnknown* *Idest)
{
 if (!hThread)
  {
   hThread = (HANDLE)_beginthreadex(NULL,0,searchFilterInterfaceThreadEntry,this,0,NULL);
  }
 if (hThread)
  {
   waitResult = WaitForSingleObject(hThread, 100);
   if (waitResult == WAIT_OBJECT_0)
    {
     DWORD retval;
     if (GetExitCodeThread(hThread, &retval) && retval)
      {
       *Idest=dest;
       CloseHandle(hThread);
       hThread = NULL;
       return true;
      }
    }
  }
 return false;
}

TsearchInterfaceInGraph::~TsearchInterfaceInGraph()
{
 if (hThread)
  {
   WaitForSingleObject(hThread, INFINITE);
   CloseHandle(hThread);
   hThread = NULL;
  }
}
