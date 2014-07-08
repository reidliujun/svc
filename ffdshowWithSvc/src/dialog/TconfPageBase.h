#ifndef _TCONFPAGEBASE_H_
#define _TCONFPAGEBASE_H_

#include "Twindow.h"
#include "ffdshow_constants.h"
#include "IffdshowBase.h"
#include "Twidget.h"

class TffdshowPageBase;
class TconfPageBase :public Twindow
{
private:
 char_t dialogNameRes[256];
 char_t *helpStr;
 //static LRESULT CALLBACK chbWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
 //static LRESULT CALLBACK rbtWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
 //static LRESULT CALLBACK btWndProc (HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
 //static LRESULT CALLBACK tbrWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
protected:
 TffdshowPageBase *parent;
 void loadHelpStr(int id);
 char_t* loadText(int resId);

 virtual void onEditChange(void);
public:
 TconfPageBase(TffdshowPageBase *Iparent,int IfilterPageId=0);
 virtual ~TconfPageBase();
 void createWindow(void),destroyWindow(void);
 HWND hwndParent;
 HTREEITEM hti;
 virtual const char_t* dialogName(void);
 virtual bool forCodec(int codecId) {return 0;}
 virtual void translate(void);
 int inPreset;
 int filterID,filterPageID;
 int uniqueID(void) const {return dialogId|(filterID?filterID<<16:0);}
 const int *propsIDs;
 virtual int  getInter(void) {return -1;}
 virtual bool invInter(void) {return false;}
 virtual int  getShow(void) {return -1;}
 virtual void setShow(int s) {}
 virtual void interDlg(void) {}
 virtual void setOrder(int o) {}
 virtual int  getOrder(void) {return INT_MIN;}
 virtual int  getProcessFull(void) {return -1;}
 virtual void setProcessFull(int full) {}
 virtual int  getProcessHalf(void) {return -1;}
 virtual void setProcessHalf(int half) {}
 const char_t *helpURL;
 virtual void applySettings(void) {}
 virtual void onFrame(void) {}
 virtual bool hasReset(void) {return false;}
 virtual bool reset(bool testonly=false) {return false;}
 virtual void getTip(char_t *tipS,size_t len) {tipS[0]='\0';}
 void resize(const CRect &newrect);
};

#endif
