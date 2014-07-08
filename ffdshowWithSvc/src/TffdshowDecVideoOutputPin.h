#ifndef _TFFDSHOWDECVIDEOOUTPUTPIN_H_
#define _TFFDSHOWDECVIDEOOUTPUTPIN_H_

#include "transfrm.h"
#include "TffdecoderVideo.h"
#include "TffOutputQ.h"

class TffdshowDecVideo;
struct IMediaSample;

class TffdshowDecVideoOutputPin : public CTransformOutputPin
{
protected:
 HANDLE hEvent;
 TffdshowDecVideo* fdv;
 int oldSettingOfMultiThread;     // -1: first run, 0: false, 1: true
 void waitUntillQueueCleanedUp(void);
 void freeQueue(void);
 TffOutputQueue* queue;
 bool isFirstFrame;
public:
 TffdshowDecVideoOutputPin(
        TCHAR *pObjectName,
        TffdshowDecVideo *Ifdv,
        HRESULT * phr,
        LPCWSTR pName);
 virtual ~TffdshowDecVideoOutputPin();
 virtual HRESULT Deliver(IMediaSample *pSample);
 virtual HRESULT BreakConnect();
 virtual HRESULT CompleteConnect(IPin *pReceivePin);
 virtual STDMETHODIMP Disconnect(void);
 virtual STDMETHODIMP Connect(IPin * pReceivePin, const AM_MEDIA_TYPE *pmt);
 virtual HRESULT DeliverBeginFlush(void);
 virtual HRESULT DeliverEndFlush(void);
 virtual HRESULT DeliverEndOfStream(void);
 virtual HRESULT Inactive(void);
 virtual HRESULT DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
 void waitForPopEvent(void);
 void resetPopEvent(void);
 IMediaSample* GetBuffer(void);
 IMemAllocator* GetAllocator(void) {return m_pAllocator;}
 void SendAnyway(void);
 HRESULT GetDeliveryBuffer(IMediaSample ** ppSample,
  REFERENCE_TIME * pStartTime,
  REFERENCE_TIME * pEndTime,
  DWORD dwFlags);
 friend class TffdshowDecVideo;
};

#endif
