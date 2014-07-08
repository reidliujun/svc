#ifndef _TFFDSHOWVIDEOINPUTPIN_H_
#define _TFFDSHOWVIDEOINPUTPIN_H_

#include "TinputPin.h"
#include "TffdshowDecVideoAllocator.h"
#include "TffPict.h"
#include "TvideoCodec.h"

class TffdshowDecVideo;
class TffdshowVideo;
struct IcheckSubtitle;
class TffdshowVideoInputPin :public TinputPin
{
private:
 TffdshowVideo *fv;
 int supdvddec;
 bool usingOwnAllocator,rawDecode;
 TvideoCodecDec *video;
 char_t autosubflnm[MAX_PATH];bool oldSubHeuristic;char_t oldSubSearchDir[MAX_PATH];
 int connectedSplitter;
 bool wasVC1;
protected:
 virtual bool init(const CMediaType &mt);
 virtual void done(void);
public:
 TffdshowVideoInputPin(TCHAR *objectName,TffdshowVideo *pFilter,HRESULT *phr);
 virtual ~TffdshowVideoInputPin();

 STDMETHODIMP ReceiveConnection(IPin* pConnector, const AM_MEDIA_TYPE* pmt);
 HRESULT CheckMediaType(const CMediaType* mtIn);
 STDMETHODIMP Receive(IMediaSample* pSample);
 STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
 STDMETHODIMP BeginFlush(void);

 STDMETHODIMP GetAllocator(IMemAllocator** ppAllocator);
 STDMETHODIMP NotifyAllocator(IMemAllocator *pAllocator,BOOL bReadOnly);

 HRESULT getAVIfps(unsigned int *fps1000);
 int getAVIfps1000_2(void) {return avgTimePerFrame==0?25000:int(REF_SECOND_MULT*1000/avgTimePerFrame);}
 HRESULT getAVIdimensions(unsigned int *x,unsigned int *y);
 HRESULT getInputSAR(unsigned int *a1,unsigned int *a2);
 HRESULT getInputDAR(unsigned int *a1,unsigned int *a2);
 FOURCC getMovieFOURCC(void);
 HRESULT getMovieSource(const TvideoCodecDec* *moviePtr);
 HRESULT getFrameTime(unsigned int framenum,unsigned int *sec);
 HRESULT getFrameTimeMS(unsigned int framenum,unsigned int *msec);
 HRESULT calcMeanQuant(float *quant);
 HRESULT quantsAvailable(void);
 HRESULT getQuantMatrices(uint8_t intra8[64],uint8_t inter8[64]);
 virtual HRESULT getInCodecString(char_t *buf,size_t buflen);
 HRESULT getAverageTimePerFrame(int64_t *avg);
 const char_t *findAutoSubflnm(IcheckSubtitle *checkSubtitle,const char_t *searchDir,const char_t *serchExt,bool heuristic);

 HRESULT decompress(IMediaSample *pSample,long *srcLen);
 STDMETHODIMP EndOfStream(void);

 TffdshowDecVideoAllocator allocator;
 BITMAPINFO biIn;TffPictBase pictIn;
 REFERENCE_TIME avgTimePerFrame;
 int sourceFlags;
 bool waitForKeyframes(void);
 void setSampleSkipped(void);
 const char* get_current_idct(void){return video->get_current_idct();}
 bool isInterlacedRawVideo;
 IBaseFilter *pCompatibleFilter;
 enum
  {
   MPC_matroska_splitter,
   Haali_Media_splitter,
   MPC_mpegSplitters,
   DVBSourceFilter,
   Unknown_Splitter
  };
};

class TffdshowVideoEncInputPin :public IMixerPinConfig,public TffdshowVideoInputPin
{
private:
 bool isOverlay;
public:
 DECLARE_IUNKNOWN
 TffdshowVideoEncInputPin(TffdshowVideo* pFilter, HRESULT* phr):TffdshowVideoInputPin(NAME("TffdshowVideoEncInputPin"),pFilter,phr),isOverlay(false) {}
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

 STDMETHODIMP SetRelativePosition(THIS_ IN DWORD dwLeft, IN DWORD dwTop, IN DWORD dwRight, IN DWORD dwBottom);
 STDMETHODIMP GetRelativePosition(THIS_ OUT DWORD *pdwLeft,OUT DWORD *pdwTop,OUT DWORD *pdwRight,OUT DWORD *pdwBottom);
 STDMETHODIMP SetZOrder(THIS_ IN DWORD dwZOrder);
 STDMETHODIMP GetZOrder(THIS_ OUT DWORD *pdwZOrder);
 STDMETHODIMP SetColorKey(THIS_ IN COLORKEY *pColorKey);
 STDMETHODIMP GetColorKey(THIS_ OUT COLORKEY *pColorKey,OUT DWORD *pColor);
 STDMETHODIMP SetBlendingParameter(THIS_ IN DWORD dwBlendingParameter);
 STDMETHODIMP GetBlendingParameter(THIS_ OUT DWORD *pdwBlendingParameter);
 STDMETHODIMP SetAspectRatioMode(THIS_ IN AM_ASPECT_RATIO_MODE amAspectRatioMode);
 STDMETHODIMP GetAspectRatioMode(THIS_ OUT AM_ASPECT_RATIO_MODE* pamAspectRatioMode);
 STDMETHODIMP SetStreamTransparent(THIS_ IN BOOL bStreamTransparent);
 STDMETHODIMP GetStreamTransparent(THIS_ OUT BOOL *pbStreamTransparent);
};

#endif
