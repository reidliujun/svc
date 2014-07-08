#ifndef _CMASKINGPAGE_H_
#define _CMASKINGPAGE_H_

#include "TconfPageEnc.h"

class TmaskingPage :public TconfPageEnc
{
private:
 void masking2dlg(void),elim2dlg(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TmaskingPage(TffdshowPageEnc *Iparent);
 virtual bool enabled(void);
 virtual void cfg2dlg(void);
};

#endif

