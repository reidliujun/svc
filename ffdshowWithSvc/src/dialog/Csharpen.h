#ifndef _CSHARPENPAGE_H_
#define _CSHARPENPAGE_H_

#include "TconfPageDecVideo.h"

class TsharpenPage :public TconfPageDecVideo
{
private:
 void xsharpen2dlg(int sharpenMethod),unsharp2dlg(int sharpenMethod),msharpen2dlg(int sharpenMethod),asharp2dlg(int sharpenMethod),mplayer2dlg(int sharpenMethod);
 int idff1,idff2,idff3;
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual Twidget* createDlgItem(int id,HWND h);
 virtual int getTbrIdff(int id,const TbindTrackbars bind);
public:
 TsharpenPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

