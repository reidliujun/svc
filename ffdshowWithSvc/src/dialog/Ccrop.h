#ifndef _CCROPPAGE_H_
#define _CCROPPAGE_H_

#include "TconfPageDecVideo.h"

class TcropPage :public TconfPageDecVideo
{
private:
 void zoom2dlg(void),crop2dlg(void),panscan2dlg(void), autocrop2dlg(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TcropPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

