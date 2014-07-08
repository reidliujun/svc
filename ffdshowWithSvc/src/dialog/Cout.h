#ifndef _CINOUTPAGE_H_
#define _CINOUTPAGE_H_

#include "TconfPageEnc.h"

class ToutPage :public TconfPageEnc
{
private:
 TffdshowPageEnc *parentE;
 void out2dlg(void),stats2dlg(void),debug2dlg(void),fps2dlg(void),aspect2dlg(void);
 void onStoreExternal(void),onLavcStats(void),onBitrate(void),onAspect(void),onFps(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 ToutPage(TffdshowPageEnc *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

