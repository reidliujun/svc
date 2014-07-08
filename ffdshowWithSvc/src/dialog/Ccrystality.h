#ifndef _CCRYSTALITYPAGE_H_
#define _CCRYSTALITYPAGE_H_

#include "TconfPageDecAudio.h"

class TcrystalityPage :public TconfPageDecAudio
{
public:
 TcrystalityPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

