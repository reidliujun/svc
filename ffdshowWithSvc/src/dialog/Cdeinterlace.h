#ifndef _CDEINTERLACEPAGE_H_
#define _CDEINTERLACEPAGE_H_

#include "TconfPageDecVideo.h"

class TdeinterlacePanel;
class TdeinterlacePage :public TconfPageDecVideo
{
private:
 TdeinterlacePanel *panel;
 HWND place;RECT placer;
 void fillDeinterlacers(void),deint2dlg(void);
 strings diflnms;int dscalerstart;
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TdeinterlacePage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
 virtual void getTip(char_t *tipS,size_t len);
 virtual bool reset(bool testonly=false);
};

#endif

