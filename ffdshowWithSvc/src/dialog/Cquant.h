#ifndef _CQUANTPAGE_H_
#define _CQUANTPAGE_H_

#include "TconfPageEnc.h"

class TquantPage :public TconfPageEnc
{
private:
 static const char_t *dct_algos[],*qnss[],*dcPrecisions[];
 void type2dlg(void),bias2dlg(void),qns2dlg(void),precision2dlg(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TquantPage(TffdshowPageEnc *Iparent);
 virtual bool enabled(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

