#ifndef _TFFDSHOWPAGEBASE_H_
#define _TFFDSHOWPAGEBASE_H_

#include "Twindow.h"
#include "IPropertyPageFF.h"
#include "ffdshow_constants.h"

#pragma warning (disable:4211)

class TconfPageBase;
typedef vectorEx<TconfPageBase*> TconfPages;

struct ThtiPage :TconfPages
{
private:
 int *id;
public:
 ThtiPage(int *Iid):id(Iid) {}
 ThtiPage(int *Iid,const TconfPages &Ipages):id(Iid),TconfPages(Ipages) {}
 ~ThtiPage();
 TconfPageBase* getPage(void);
 HTREEITEM hti;
};
struct ThtiPages :public std::vector<ThtiPage*>
{
};

struct TfilterIDFF;
class TffdshowPageBase :public Twindow,public IPropertyPage,public IPropertyPageFF,public CUnknown
{
private:
 int resstr;
 IPropertyPageSite *pageSite;
 bool dirty,wasobjectset;

 void saveDialogSettings(void);
 HWND findParentDlg(void);HWND dlg;
 HBITMAP bmp1[5],bmp2[5];
 ThtiPage* hti2htiPage(HTREEITEM hti);
 ThtiPages htiPages;
 void onHelp(void);
protected:
 enum
  {
   WM_FFONCHANGE=WM_APP+1,
   WM_FFONFRAME =WM_APP+4
  };

 const wchar_t *titleW;
 HIMAGELIST hil;
 int ilClear,ilChecked,ilArrowUD,ilArrowU,ilArrowD;
 HWND htv;
 void treeSelectItem(int uniqueId,int prevVisId=0);
 bool isShift;
 bool resorePos;
 bool translateTV(void);

 ThtiPage* addTI(TVINSERTSTRUCT *tvis,const TconfPages &pages,int *Iid);
 ThtiPage* addTI(TVINSERTSTRUCT *tvis,TconfPageBase *page);
 virtual void onActivate(void) {}
 virtual void onApplyChanges(void) {}
 virtual INT_PTR msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam),defMsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);

 TconfPageBase *page;bool pageIs;
 void selectPage(int i);
 virtual void selectPage(TconfPageBase *Ipage);
 void deletePage(ThtiPage *htiPage);
public:
 TffdshowPageBase(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr);
 virtual ~TffdshowPageBase() {}
 DECLARE_IUNKNOWN
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
 STDMETHODIMP_(ULONG) NonDelegatingRelease(void);
 STDMETHODIMP_(ULONG) NonDelegatingAddRef(void);

 virtual HRESULT STDMETHODCALLTYPE SetPageSite(IPropertyPageSite *pPageSite);
 virtual HRESULT STDMETHODCALLTYPE SetObjects(ULONG cObjects,IUnknown **ppUnk);
 virtual HRESULT STDMETHODCALLTYPE Show(UINT nCmdShow);
 virtual HRESULT STDMETHODCALLTYPE Move(LPCRECT pRect);
 virtual HRESULT STDMETHODCALLTYPE IsPageDirty(void);
 virtual HRESULT STDMETHODCALLTYPE Apply(void);
 virtual HRESULT STDMETHODCALLTYPE Help(LPCOLESTR pszHelpDir);
 virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(MSG *pMsg);
 STDMETHODIMP GetPageInfo(LPPROPPAGEINFO pPageInfo);
 virtual HRESULT OnConnect(IUnknown *pUnk);
 virtual HRESULT OnDisconnect(void);
 STDMETHODIMP Activate(HWND hwndParent,LPCRECT prect,BOOL fModal);
 STDMETHODIMP Deactivate(void);
 virtual HRESULT OnApplyChanges(void);

 STDMETHODIMP toTop(void);
 STDMETHODIMP resize(const CRect &newrect);

 HFONT arrowsFont;
 TconfPageBase* hti2page(HTREEITEM hti);
 virtual void translate(void);
 void enableHints(bool is);
 void setChange(void);
 virtual void showShowHide(int previd=0,int prevVisId=0) {}

 template<class Iff> static void configure(const CLSID &clsid,int filtermode,LPTSTR lpCmdLine)
  {
   if (CoInitialize(NULL)!=S_OK) return;
   comptr<Iff> deciI;
   if (CoCreateInstance(clsid,NULL,CLSCTX_INPROC_SERVER,getGUID<Iff>(),(void**)&deciI)==S_OK)
    {
     comptrQ<IffdshowBase> deci=deciI;
     deci->putParam(IDFF_filterMode,filtermode|IDFF_FILTERMODE_CONFIG);
     if (strstr(lpCmdLine,_l("top"))) deci->putParam(IDFF_alwaysOnTop,1);
     deci->showCfgDlg(NULL);
    }
   CoUninitialize();
  }
};

#endif
