#ifndef _CSUBTITLESTEXTPAGE_H_
#define _CSUBTITLESTEXTPAGE_H_

#include "TconfPageDecVideo.h"

class TsubtitlesTextPage :public TconfPageDecVideo
{
private:
 void linespacing2dlg(void),min2dlg(void),fix2dlg(void),split2dlg(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TsubtitlesTextPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

