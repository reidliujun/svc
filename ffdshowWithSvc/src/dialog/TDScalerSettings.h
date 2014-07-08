#ifndef _TDSCALERSETTINGS_H_
#define _TDSCALERSETTINGS_H_

#include "interfaces.h"

struct TregOp;
struct SETTING;
class Twindow;
struct TDScalerSettings
{
private:
 comptr<IffdshowBase> deci;comptrQ<IffdshowDecVideo> deciV;
 HWND parent;
 LONG onChangeMsg;
 int cdy_top;
 LONG rdx,rdy,cdy;
 int sbrWidth;
 typedef std::hash_map<int,SETTING*> Tsets;
 Tsets sets;
 long *bActive;
 static BOOL CALLBACK dscalerDlgEnum(HWND hwnd,LPARAM lParam);
 static INT_PTR CALLBACK dscalerDlgProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
 void cfg2dlg(HWND m_hwnd);
 void writeops(TregOp &t);
 int nSettings;
 SETTING *pSettings;
public:
 TDScalerSettings(IffdshowBase *Ideci,int InSettings,SETTING *IpSettings);
 ~TDScalerSettings();
 void cfg2str(ffstring &s),str2cfg(const ffstring &s);
 void reset(void);
 HWND createWindow(HINSTANCE hi,const Twindow *Iparent,const RECT &r,LONG IonChangeMsg,long *IbActive);
 static int dscalerCpu(void);

 HWND wnd;
};

#endif
