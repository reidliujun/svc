#include "stdafx.h"
#include "nssBuffer.h"

TmyNssBuffer::TmyNssBuffer(LPUNKNOWN punk,HRESULT *phr):CUnknown(_l("TmyNssBuffer"),punk)
{
 buf=NULL;
 buflen=NULL;
 AddRef();
 *phr=S_OK;
}
TmyNssBuffer::~TmyNssBuffer()
{
 if (buf) free(buf);
}
void TmyNssBuffer::alloc(DWORD Ilen)
{
 if (buf) free(buf);
 buf=(BYTE*)malloc(len=buflen=Ilen);
}
STDMETHODIMP TmyNssBuffer::GetLength(DWORD *pdwLength)
{
 if (!pdwLength) return E_POINTER;
 *pdwLength=len;
 return S_OK;
}
STDMETHODIMP TmyNssBuffer::SetLength(DWORD dwLength)
{
 len=dwLength;
 return S_OK;
}
STDMETHODIMP TmyNssBuffer::GetMaxLength(DWORD *pdwLength)
{
 if (!pdwLength) return E_POINTER;
 *pdwLength=buflen;
 return S_OK;
}
STDMETHODIMP TmyNssBuffer::GetBuffer(BYTE **ppdwBuffer)
{
 if (!ppdwBuffer) return E_POINTER;
 *ppdwBuffer=buf;
 return S_OK;
}
STDMETHODIMP TmyNssBuffer::GetBufferAndLength(BYTE **ppdwBuffer,DWORD *pdwLength)
{
 if (!ppdwBuffer || !pdwLength) return E_POINTER;
 *ppdwBuffer=buf;
 *pdwLength=len;
 return S_OK;
}
