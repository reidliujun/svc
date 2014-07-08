#ifndef _CRESIZEBORDERSPAGE_H_
#define _CRESIZEBORDERSPAGE_H_

#include "TconfPageDecVideo.h"

class TresizeBordersPage :public TconfPageDecVideo
{
private:
 void onLock(void);
 void div2dlg(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual Twidget* createDlgItem(int id,HWND h);
 virtual int getTbrIdff(int id,const TbindTrackbars bind);
public:
 TresizeBordersPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

