#ifndef _TFFVFW_H_
#define _TFFVFW_H_

#include "Iffvfw.h"
#include "TvideoCodec.h"
#include "TfakeImediaSample.h"
#include "interfaces.h"
#include "TimgFilters.h"

struct IffdshowEnc;
struct IffdshowEncVFW;
class Tconvert;
struct TcspInfo;
class TffdshowDecVideoVFW;
class Tffvfw :public Iffvfw,
              public CUnknown,
              public IdecVideoSink,
              public TfakeMediaSample,
              public IprocVideoSink
{
private:
 DWORD previous_out_biSizeImage;
 LPVOID previouts_lpOutput;
 bool graph;
 bool initCo(void),initDec(void);
 comptr<IffdshowEncVFW> deciE_VFW;comptrQ<IffdshowEnc> deciE;comptrQ<IffdshowBase> deciEB;
 TffdshowDecVideoVFW *decVFW;
 Tconvert *convert;

 TvideoCodecDec* initDecoder(const BITMAPINFO *lpbiInput,CodecID *codecId);
 struct TautoForcedCsps :TcspInfos
  {
   TcspInfos decGetForcedCsp(IffdshowDec *deciD);
  } autoforcedcolorspaces;
 int autoforcedilace;
 TvideoCodecDec *dec;

 int colorspace;
 ICDECOMPRESS *icd;
 // IdecVideoSink
 STDMETHODIMP deliverDecodedSample(TffPict &pict);
 STDMETHODIMP deliverPreroll(int frametype);
 STDMETHODIMP acceptsManyFrames(void) {return S_FALSE;}
 STDMETHODIMP flushDecodedSamples(void) {return S_OK;}
 // IprocVideoSink
 STDMETHODIMP deliverProcessedSample(TffPict &pict);
public:
 static void* operator new(size_t bytes)
  {
   return LocalAlloc(LPTR,bytes);
  }
 static void operator delete(void *ptr)
  {
   if (ptr) LocalFree(ptr);
  }

 static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr);
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
 DECLARE_IUNKNOWN

 Tffvfw(LPUNKNOWN punk,HRESULT *phr);
 virtual ~Tffvfw();

 STDMETHODIMP_(void) aboutDlg(HWND hParent);
 STDMETHODIMP_(void) configDlg(HWND hParent);
 STDMETHODIMP_(int) saveMem(void *buf,int len);
 STDMETHODIMP_(int) loadMem(const void *buf,int len);
 STDMETHODIMP_(void) loadReg(void);

 STDMETHODIMP_(LRESULT) coQuery(BITMAPINFO*,BITMAPINFO*);
 STDMETHODIMP_(LRESULT) coGetFormat(BITMAPINFO*,BITMAPINFO*);
 STDMETHODIMP_(LRESULT) coGetSize(BITMAPINFO*,BITMAPINFO*);
 STDMETHODIMP_(LRESULT) coFramesInfo(ICCOMPRESSFRAMES*);
 STDMETHODIMP_(LRESULT) coBegin(BITMAPINFO*,BITMAPINFO*);
 STDMETHODIMP_(LRESULT) coEnd(void);
 STDMETHODIMP_(LRESULT) coRun(void*);

 STDMETHODIMP_(LRESULT) decQuery(BITMAPINFO*,BITMAPINFO*);
 STDMETHODIMP_(LRESULT) decGetFormat(BITMAPINFO*,BITMAPINFO*);
 STDMETHODIMP_(LRESULT) decBegin(BITMAPINFO*,BITMAPINFO*);
 STDMETHODIMP_(LRESULT) decEnd(void);
 STDMETHODIMP_(LRESULT) decRun(void*);

 //void dbgError(const char *msg);
};

#endif
