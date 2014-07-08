#ifndef _TFFDSHOWDECVIDEOALLOCATOR_H_
#define _TFFDSHOWDECVIDEOALLOCATOR_H_

class TffdshowDecVideoAllocator :public CMemAllocator
{
protected:
 CBaseFilter *filter;
 CMediaType mt;
public:
 TffdshowDecVideoAllocator(CBaseFilter* Ifilter,HRESULT* phr);
 STDMETHODIMP_(ULONG) NonDelegatingAddRef() {return filter->AddRef();}
 STDMETHODIMP_(ULONG) NonDelegatingRelease() {return filter->Release();}

 void NotifyMediaType(const CMediaType &Imt);
 bool mtChanged;

 STDMETHODIMP GetBuffer(IMediaSample** ppBuffer,REFERENCE_TIME* pStartTime,REFERENCE_TIME* pEndTime,DWORD dwFlags);
};

#endif
