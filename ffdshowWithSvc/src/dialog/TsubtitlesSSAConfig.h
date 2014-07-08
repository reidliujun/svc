#ifndef _TSUBTITLESSSACONFIG_H_
#define _TSUBTITLESSSACONFIG_H_

#include "Twindow.h"
#include "interfaces.h"

class TsubtitlesSSAConfig :public TdlgWindow
{
private:
 HWND parent;
 int x,y;
 bool closing;
 
protected:
 comptrQ<IffdshowDec> deciD;
 virtual void init(void);
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TsubtitlesSSAConfig(IffdshowBase *Ideci,HWND parent,int x,int y);
 bool show(void);
};

#endif
