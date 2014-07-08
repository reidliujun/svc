#ifndef _WRITERSINK_H_
#define _WRITERSINK_H_

#include "combase.h"

class TmyWriterSink :public CUnknown, public IWMWriterSink
{
private:
 FILE *f;
public:
 DECLARE_IUNKNOWN
 TmyWriterSink(LPUNKNOWN punk,HRESULT *phr);
 STDMETHODIMP OnHeader(INSSBuffer *pHeader);
 STDMETHODIMP IsRealTime(BOOL *pfRealTime);
 STDMETHODIMP AllocateDataUnit(DWORD cbDataUnit,INSSBuffer* *ppDataUnit);
 STDMETHODIMP OnDataUnit(INSSBuffer *pDataUnit);
 STDMETHODIMP OnEndWriting(void);
};

#endif
