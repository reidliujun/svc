#ifndef _TFFPROC_H_
#define _TFFPROC_H_

#include "IffProc.h"
#include "TffRect.h"
#include "TffProcBase.h"
#include "TimgFilters.h"

struct TpresetVideo;
struct TglobalSettingsDecVideo;
class Tconvert;
struct TcspInfo;
class TffdshowDecVideoProc;
class TffProcVideo :public CUnknown,
                    public TffProcBase,
                    public IffProc,
                    public IprocVideoSink
{
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr);
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
 DECLARE_IUNKNOWN

 TffProcVideo(LPUNKNOWN punk,HRESULT *phr);
 virtual ~TffProcVideo();

 STDMETHODIMP setTempPreset(const char *tempPresetName) {return TffProcBase::setTempPreset(tempPresetName);}
 STDMETHODIMP setActivePreset(const char *presetName) {return TffProcBase::setActivePreset(presetName);}
 STDMETHODIMP loadPreset(const char *presetName) {return TffProcBase::loadPreset(presetName);}
 STDMETHODIMP saveBytestreamConfig(void *buf,size_t len) {return TffProcBase::saveBytestreamConfig(buf,len);}
 STDMETHODIMP loadBytestreamConfig(const void *buf,size_t len) {return TffProcBase::loadBytestreamConfig(buf,len);}
 STDMETHODIMP config(HWND owner) {return TffProcBase::config(owner);}
 STDMETHODIMP getIffDecoder_(/*IffdshowBase*/void* *deciPtr) {return TffProcBase::getIffDecoder_(deciPtr);}
 STDMETHODIMP putStringParam(const char *param,char sep) {return TffProcBase::putStringParam(param,sep);}
 STDMETHODIMP getParamsEnum(IffdshowParamsEnum* *enumPtr) {return TffProcBase::getParamsEnum(enumPtr);}
 STDMETHODIMP notifyParamsChanged(void) {return TffProcBase::notifyParamsChanged();}
 STDMETHODIMP saveActivePreset(const char *name) {return TffProcBase::saveActivePreset(name);}
 STDMETHODIMP putParam(unsigned int paramID,int val) {return TffProcBase::putParam(paramID,val);}
 STDMETHODIMP setBasePageSite(IPropertyPage *page) {return TffProcBase::setBasePageSite(page);}

 STDMETHODIMP getNewSize(unsigned int srcDx,unsigned int srcDy,unsigned int *outDx,unsigned int *outDy);
 STDMETHODIMP begin(unsigned int srcDx,unsigned int srcDy,int FpsNum,int FpsDen);
 STDMETHODIMP process(unsigned int framenum,int incsp,const unsigned char *src[4],stride_t srcStride[4],int ro,int outcsp,unsigned char *dst[4],stride_t dstStride[4]) ;
 STDMETHODIMP processTime(unsigned int framenum,int64_t ref_start,int64_t ref_stop,int incsp,const unsigned char *src[4],stride_t srcStride[4],int ro,int outcsp,unsigned char *dst[4],stride_t dstStride[4]);
 STDMETHODIMP end(void);
 STDMETHODIMP_(const TcspInfo*) getCspInfo(int csp);
 // IprocVideoSink
 STDMETHODIMP deliverProcessedSample(TffPict &pict);

 HRESULT processPict(unsigned int framenum,TffPict &pict,int outcsp);
private:
 Trect srcR;
 unsigned int srcDx,srcDy;
 int fpsNum,fpsDen;
 TffdshowDecVideoProc *proc;
 const TcspInfo *outcspInfo;
 Tconvert *convert;
 Tbuffer dstbuf;
 int outcsp;unsigned char **dst;stride_t *dstStride;
};

#endif
