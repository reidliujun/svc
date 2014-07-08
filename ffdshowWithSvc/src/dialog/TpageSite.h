#ifndef _TPAGESITE_H_
#define _TPAGESITE_H_

#include "Twindow.h"
#include "Crect.h"
#include "IPropertyPageFF.h"

//struct IffdshowBase;
struct IPropertyPageFF;
class TpageSite :public IPropertyPageSite,public CUnknown,public TdlgWindow, public IPropertyPageSiteFF
{
private:
 int idcaption;char_t caption[256];
 int idff_multiplePages;
 typedef std::vector<IPropertyPage*> TpropertyPages;
 TpropertyPages propertypages;
 IPropertyPage *activepage;comptrQ<IPropertyPageFF> activepageFF;
 CRect pagerect;
 void setActivePage(IPropertyPage *page);
 void setDirty(int d);
 void applyChanges(void);
 char_t regflnm[MAX_PATH];
 CRect prevrect;
 void resize(CRect &newrect);
 HWND htab;
 HICON hicon;
protected:
 virtual INT_PTR msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
public:
 TpageSite(int Iidff_multiplePages=0);
 virtual ~TpageSite();
 DECLARE_IUNKNOWN
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
 virtual HRESULT STDMETHODCALLTYPE OnStatusChange(DWORD dwFlags);
 virtual HRESULT STDMETHODCALLTYPE GetLocaleID(LCID *pLocaleID);
 virtual HRESULT STDMETHODCALLTYPE GetPageContainer(IUnknown **ppUnk);
 virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(MSG *pMsg);

 STDMETHODIMP onTranslate(void);

 INT_PTR show(IffdshowBase *deci,HWND parent,int Iidcaption=0,int Iicon=0);
 INT_PTR show(IffdshowBase *deci,HWND parent,int Iidcaption,int Iicon,CAUUID &pages,IUnknown *unk);
};

#endif
