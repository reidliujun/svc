#ifndef _CFIRSTPASSPAGE_H_
#define _CFIRSTPASSPAGE_H_

#include "TconfPageEnc.h"

class TfirstPassPage :public TconfPageEnc
{
private:
 void onStats1(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TfirstPassPage(TffdshowPageEnc *Iparent);
 virtual void cfg2dlg(void);
 virtual bool enabled(void);
};

#endif

