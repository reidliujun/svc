#ifndef _NSSBUFFER_H_
#define _NSSBUFFER_H_

#include "combase.h"

class TmyNssBuffer :public CUnknown, public INSSBuffer
{
private:
 BYTE *buf;
 DWORD buflen,len;
public:
 TmyNssBuffer(LPUNKNOWN punk,HRESULT *phr);
 virtual ~TmyNssBuffer();
 virtual void alloc(DWORD len);
 DECLARE_IUNKNOWN
 STDMETHODIMP GetLength(DWORD *pdwLength);
 STDMETHODIMP SetLength(DWORD dwLength);
 STDMETHODIMP GetMaxLength(DWORD *pdwLength);
 STDMETHODIMP GetBuffer(BYTE **ppdwBuffer);
 STDMETHODIMP GetBufferAndLength(BYTE **ppdwBuffer,DWORD *pdwLength);
};

#endif
