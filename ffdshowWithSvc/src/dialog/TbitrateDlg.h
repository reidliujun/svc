#ifndef _TBITRATEDLG_H_
#define _TBITRATEDLG_H_

#include "Twindow.h"
#include "interfaces.h"

class TbitrateDlg :public TdlgWindow
{
private:
 IffdshowBase *deci;
 void init(void);
 int numframes;
 uint32_t getLen(void);
 double getFps(void);
 void time2dlg(void),bps2dlg(void);
 unsigned int getbps(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 static const int mpeg12_frame_rate_tab[][2];
 TbitrateDlg(HWND hParent,IffdshowBase *Ideci);
};

#endif
