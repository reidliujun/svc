#ifndef _COVERLAYPAGE_H_
#define _COVERLAYPAGE_H_

#include "TconfPageDecVideo.h"

class ToverlayPage :public TconfPageDecVideo
{
 bool repaint;
public:
 ToverlayPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual void onFrame(void);
 virtual bool reset(bool testonly=false);
};

#endif

