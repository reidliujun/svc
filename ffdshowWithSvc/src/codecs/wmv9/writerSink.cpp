#include "stdafx.h"
#include "writerSink.h"
#include "nssBuffer.h"

TmyWriterSink::TmyWriterSink(LPUNKNOWN punk,HRESULT *phr):CUnknown(_l("TmyWriterSink"),punk)
{
 AddRef();
 f=NULL;
 *phr=S_OK;
}
STDMETHODIMP TmyWriterSink::OnHeader(INSSBuffer *pHeader)
{
 if (!f) f=fopen("c:\\2.wmv","wb");
 BYTE *buf;DWORD len;
 pHeader->GetBufferAndLength(&buf,&len);
 fwrite(buf,1,len,f);
 return S_OK;
}
STDMETHODIMP TmyWriterSink::IsRealTime(BOOL *pfRealTime)
{
 *pfRealTime=FALSE;
 return S_OK;
}
STDMETHODIMP TmyWriterSink::AllocateDataUnit(DWORD cbDataUnit,INSSBuffer* *ppDataUnit)
{
 if (!ppDataUnit) return E_POINTER;
 HRESULT hr;
 TmyNssBuffer *buf=new TmyNssBuffer(NULL,&hr);
 buf->alloc(cbDataUnit);
 *ppDataUnit=(INSSBuffer*)buf;
 return S_OK;
}
STDMETHODIMP TmyWriterSink::OnDataUnit(INSSBuffer *pDataUnit)
{
 BYTE *buf;DWORD len;
 pDataUnit->GetBufferAndLength(&buf,&len);
 char_t pomS[256];
 OutputDebugString(_itoa(len,pomS,10));
 fwrite(buf,1,len,f);
 return S_OK;
}
STDMETHODIMP TmyWriterSink::OnEndWriting(void)
{
 if (f) fclose(f);
 return S_OK;
}
