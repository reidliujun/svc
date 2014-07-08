#ifndef _CCURVEALTPAGE_H_
#define _CCURVEALTPAGE_H_

#include "TconfPageEnc.h"

class TcurveAltPage :public TconfPageEnc
{
private:
 static const char_t *aggressions[];
public:
 TcurveAltPage(TffdshowPageEnc *Iparent);
 virtual void cfg2dlg(void);
 virtual bool enabled(void);
 virtual void translate(void);
};

#endif

