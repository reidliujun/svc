#ifndef _TFFDSHOWPAGEDEC_H_
#define _TFFDSHOWPAGEDEC_H_

#include "TffdshowPageBase.h"
#include "Twidget.h"

class TconfPageDec;
class Tpresets;
struct TfilterSettings;
class TffdshowPageDec :public TffdshowPageBase
{
private:
 void setFullHalf(void);
 bool hasFull;
 int tvx,tvy;
 TconfPageBase *dragpage;
 void swap(int direction);
 void swap(TconfPageBase *page1,TconfPageBase *page2);
 struct TwidgetTv :TwindowWidget
  {
  private:
   TffdshowPageDec *self;
  protected:
   virtual LRESULT onKeyDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onKeyUp(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
  public:
   TwidgetTv(HWND h,TffdshowPageDec *Iself):TwindowWidget(h,Iself),self(Iself) {allowOwnProc();}
  };
 void onReset(void),onResetOrder(void);
 enum {WM_FFONNEWFILTERS=WM_APP+5};
protected:
 comptrQ<IffdshowDec> deciD;

 virtual void onActivate(void);
 virtual void onApplyChanges(void);

 HTREEITEM htiPresets;
 char_t oldActivePresetName[260];
 Tpresets *localPresets;
 friend class TpresetsPage;
 void fillPresetsCbx(void);
 void selectPreset(const char_t *presetName);

 HTREEITEM htiBeforeShowHide;
 ThtiPages filterPages;
 static int CALLBACK orderCompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort);
 void sortOrder(void);

 virtual Twidget* createDlgItem(int id,HWND h);
 virtual INT_PTR msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);

 bool invInter(TconfPageBase *page=NULL,RECT *r=NULL);

 virtual void selectPage(TconfPageBase *Ipage);
public:
 TffdshowPageDec(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr,bool IhasFull);
 virtual ~TffdshowPageDec();

 template<class Tpage> void addFilterPage(const TfilterIDFF *idff)
  {
   filterPages.push_back(addTI(NULL,new Tpage(this,idff)));
  }

 virtual HRESULT OnConnect(IUnknown *pUnk);
 virtual HRESULT OnDisconnect(void);
 STDMETHODIMP Deactivate(void);

 void presetChanged(const char_t *presetName);
 void drawInter(void);
 virtual void showShowHide(int previd=0,int prevVisId=0);
};

#endif
