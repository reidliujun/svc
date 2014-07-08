#ifndef _COFFSETPAGE_H_
#define _COFFSETPAGE_H_

#include "TconfPageDecVideo.h"

class ToffsetPage :public TconfPageDecVideo
{
private:
 void offset2dlg(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 ToffsetPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

