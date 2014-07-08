#ifndef _CCURVENORMALPAGE_H_
#define _CCURVENORMALPAGE_H_

#include "TconfPageEnc.h"

class TcurveNormalPage :public TconfPageEnc
{
public:
 TcurveNormalPage(TffdshowPageEnc *Iparent);
 virtual void cfg2dlg(void);
 virtual bool enabled(void);
};

#endif

