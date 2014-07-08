#ifndef _CKEYSPAGE_H_
#define _CKEYSPAGE_H_

#include "TconfPageDec.h"

class TkeysPage :public TconfPageDec
{
private:
 void keys2dlg(void),remote2dlg(void);
 HWND hlv;
 int keyChanging;
 void beginKeyChange(void),endKeyChange(void);
 void keyChange(int newkey);
 bool ignoreNextKey;
 void onGirder(void);
 struct TwidgetKeys : TwindowWidget
  {
  private:
   TkeysPage *keysPage;
  protected:
   virtual LRESULT onSysKeyDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onKeyDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onGetDlgCode(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onKeyUp(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onChar(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
  public:
   TwidgetKeys(HWND h,TkeysPage *Iself):TwindowWidget(h,Iself),keysPage(Iself) {allowOwnProc();}
  };
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual Twidget* createDlgItem(int id,HWND h);
public:
 TkeysPage(TffdshowPageDec *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual bool reset(bool testonly=false);
 virtual void translate(void);
};

#endif

