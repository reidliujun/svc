#ifndef _CCREDITSPAGE_H_
#define _CCREDITSPAGE_H_

#include "TconfPageEnc.h"

class TcreditsPage :public TconfPageEnc
{
private:
 void startend2dlg(void),start2dlg(void),end2dlg(void),percent2dlg(void),quant2dlg(void),size2dlg(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TcreditsPage(TffdshowPageEnc *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual bool enabled(void);
};

#endif

