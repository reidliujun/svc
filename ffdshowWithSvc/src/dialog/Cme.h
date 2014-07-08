#ifndef _CMEPAGE_H_
#define _CMEPAGE_H_

#include "TconfPageEnc.h"

class TmePage :public TconfPageEnc
{
private:
 void qpel2dlg(void),prepass2dlg(void);
 static const char_t *mb_decisions[],*me_cmps[],*me_prepasses[];
public:
 TmePage(TffdshowPageEnc *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual bool enabled(void);
 virtual void translate(void);
};

#endif

