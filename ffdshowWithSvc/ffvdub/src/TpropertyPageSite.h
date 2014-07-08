#ifndef _TPROPERTYPAGESITE_H_
#define _TPROPERTYPAGESITE_H_

#include "combase.h"
#include <ocidl.h>

class FilterActivation;
struct Tffvdub;
class TpropertyPageSite: public CUnknown,public IPropertyPageSite
{
private:
 FilterActivation *fa;
 Tffvdub *ffvdub;
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr);
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
 DECLARE_IUNKNOWN
 virtual HRESULT STDMETHODCALLTYPE OnStatusChange( /* [in] */ DWORD dwFlags);
 virtual HRESULT STDMETHODCALLTYPE GetLocaleID( /* [out] */ LCID *pLocaleID);
 virtual HRESULT STDMETHODCALLTYPE GetPageContainer( /* [out] */ IUnknown **ppUnk);
 virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator( /* [in] */ MSG *pMsg);
 TpropertyPageSite(LPUNKNOWN punk,HRESULT *phr);
 virtual ~TpropertyPageSite();
 void setFilterActivation(FilterActivation *Ifa);
};

#endif
