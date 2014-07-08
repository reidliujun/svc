#ifndef _TFAKEMEDIASAMPLE_H_
#define _TFAKEMEDIASAMPLE_H_

class TfakeMediaSample :public IMediaSample
{
protected:
 bool isDiscontinuity,isSyncPoint;
public:
 virtual HRESULT STDMETHODCALLTYPE GetPointer(BYTE **ppBuffer) {return E_NOTIMPL;}
 virtual long STDMETHODCALLTYPE GetSize( void) {return E_NOTIMPL;}
 virtual HRESULT STDMETHODCALLTYPE GetTime( REFERENCE_TIME *pTimeStart,REFERENCE_TIME *pTimeEnd) {return E_NOTIMPL;}
 virtual HRESULT STDMETHODCALLTYPE SetTime(REFERENCE_TIME *pTimeStart,REFERENCE_TIME *pTimeEnd) {return E_NOTIMPL;}
 virtual HRESULT STDMETHODCALLTYPE IsSyncPoint( void) {return isSyncPoint?S_OK:S_FALSE;}
 virtual HRESULT STDMETHODCALLTYPE SetSyncPoint( BOOL bIsSyncPoint) {return E_NOTIMPL;}
 virtual HRESULT STDMETHODCALLTYPE IsPreroll( void) {return S_FALSE;}
 virtual HRESULT STDMETHODCALLTYPE SetPreroll( BOOL bIsPreroll) {return E_NOTIMPL;}
 virtual long STDMETHODCALLTYPE GetActualDataLength( void) {return E_NOTIMPL;}
 virtual HRESULT STDMETHODCALLTYPE SetActualDataLength( long __MIDL_0010) {return E_NOTIMPL;}
 virtual HRESULT STDMETHODCALLTYPE GetMediaType( AM_MEDIA_TYPE **ppMediaType) {return E_NOTIMPL;}
 virtual HRESULT STDMETHODCALLTYPE SetMediaType( AM_MEDIA_TYPE *pMediaType) {return E_NOTIMPL;}
 virtual HRESULT STDMETHODCALLTYPE IsDiscontinuity( void) {return isDiscontinuity?S_OK:S_FALSE;}
 virtual HRESULT STDMETHODCALLTYPE SetDiscontinuity( BOOL bDiscontinuity) {return E_NOTIMPL;}
 virtual HRESULT STDMETHODCALLTYPE GetMediaTime( LONGLONG *pTimeStart,LONGLONG *pTimeEnd) {return E_NOTIMPL;}
 virtual HRESULT STDMETHODCALLTYPE SetMediaTime( LONGLONG *pTimeStart,LONGLONG *pTimeEnd) {return E_NOTIMPL;}
};

#endif
