#ifndef _CGRABPAGE_H_
#define _CGRABPAGE_H_

#include "TconfPageDecVideo.h"

class TgrabPage :public TconfPageDecVideo
{
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TgrabPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

