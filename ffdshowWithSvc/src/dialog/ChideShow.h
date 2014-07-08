#ifndef _CHIDESHOWPAGE_H_
#define _CHIDESHOWPAGE_H_

#include "TconfPageDecVideo.h"

struct ThtiPages;
class ThideShowPage :public TconfPageDec
{
private:
 ThtiPages &filterPages;
 void hideSelected(void),showSelected(void),hideAll(void),showAll(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 ThideShowPage(TffdshowPageDec *Iparent,ThtiPages &IfilterPages);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

