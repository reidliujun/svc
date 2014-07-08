#ifndef _CFREEVERBPAGE_H_
#define _CFREEVERBPAGE_H_

#include "TconfPageDecAudio.h"

class TfreeverbPage :public TconfPageDecAudio
{
public:
 TfreeverbPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

