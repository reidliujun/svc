#ifndef _CSECONDPASSPAGE_H_
#define _CSECONDPASSPAGE_H_

#include "TconfPageEnc.h"

class TperfectDlg;
class TsecondPassPage :public TconfPageEnc
{
private:
 TperfectDlg *perfectDlg;
 friend class TperfectDlg;
 void onPerfectDestroy(void),onStats1(void),onStats2(void),onSimulate(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TsecondPassPage(TffdshowPageEnc *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual bool enabled(void);
};

#endif

