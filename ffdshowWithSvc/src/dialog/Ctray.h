#ifndef _CDLGMISCPAGE_H_
#define _CDLGMISCPAGE_H_

#include "TconfPageBase.h"

class TdlgMiscPage :public TconfPageBase
{
private:
 void tray2dlg(void),paths2dlg(void);
 void onIconTypeChange(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TdlgMiscPage(TffdshowPageBase *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

