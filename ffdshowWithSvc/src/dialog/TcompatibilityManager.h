#ifndef _TCOMPATIBILITYMANAGER_H_
#define _TCOMPATIBILITYMANAGER_H_

#include "Twindow.h"
#include "interfaces.h"

#define TCOMPATIBILITYMANAGER_TIMEOUT 250

class TcompatibilityManager :public TdlgWindow
{
private:
 HWND parent;
 int mode;
 bool chbsend;
 void onModeChange(void),dlg2cfg(void);
 void onDontAsk(void);
 void onChbSend(void);
 void setcaption(void);
 int time;
protected:
 comptrQ<IffdshowDec> deciD;
 virtual void init(void);
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 void cfg2dlg(void);
 ffstring filename;
public:
 TcompatibilityManager(IffdshowBase *Ideci,HWND parent,const char_t *exe);
 int show(void);
 static int s_mode;
};

#endif
