#ifndef _CLFECROSSOVERPAGE_H_
#define _CLFECROSSOVERPAGE_H_

#include "TconfPageDecAudio.h"

class TlfeCrossoverPage :public TconfPageDecAudio
{
public:
 TlfeCrossoverPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

