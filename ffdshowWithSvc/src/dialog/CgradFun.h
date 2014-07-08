#ifndef _CGRADFUNPAGE_H_
#define _CGRADFUNPAGE_H_

#include "TconfPageDecVideo.h"

class TgradFunPage :public TconfPageDecVideo
{
public:
 TgradFunPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

