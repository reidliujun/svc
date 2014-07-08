#ifndef _CDELAYPAGE_H_
#define _CDELAYPAGE_H_

#include "TconfPageDecAudio.h"

class TdelayPage :public TconfPageDecAudio
{
public:
 TdelayPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void cfg2dlg(void);
};

#endif

