#ifndef _CWARPSHARPAGE_H_
#define _CWARPSHARPAGE_H_

#include "TconfPageDecVideo.h"

class TwarpsharpPage :public TconfPageDecVideo
{
private:
 void warpsharp2dlg(int sharpenMethod),awarpsharp2dlg(int sharpenMethod),awarpsharp2dlg1(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual Twidget* createDlgItem(int id,HWND h);
 virtual int getTbrIdff(int id,const TbindTrackbars bind);
public:
 TwarpsharpPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

