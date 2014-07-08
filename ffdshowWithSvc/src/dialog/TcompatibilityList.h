#ifndef _TCOMPATIBILITYLIST_H_
#define _TCOMPATIBILITYLIST_H_

#include "Twindow.h"
#include "interfaces.h"

class TcompatibilityList :public TdlgWindow
{
private:
 HWND parent;
 int x,y;
 void cfg2dlgI(const char_t *complist0,const char_t *newFileName,const char_t *delimit);
 const char_t* getDelimit(void);
 void onReset(void);
protected:
 int IDFF_target;
 comptrQ<IffdshowDec> deciD;
 virtual void init(void);
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 void onAddFile(void);
 void cfg2dlg(const char_t *newFileName);
 void dlg2dlg(const char_t *newFileName);
 void dlg2cfg(void);
public:
 TcompatibilityList(IffdshowBase *Ideci,HWND parent,int x,int y,int target);
 bool show(void);
};

#endif
