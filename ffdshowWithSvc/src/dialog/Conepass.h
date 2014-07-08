#ifndef _CONEPASSPAGE_H_
#define _CONEPASSPAGE_H_

#include "TconfPageEnc.h"

class TonePassPage :public TconfPageEnc
{
private:
 void stats2dlg(void);
public:
 TonePassPage(TffdshowPageEnc *Iparent);
 virtual void cfg2dlg(void);
 virtual bool enabled(void);
};

#endif

