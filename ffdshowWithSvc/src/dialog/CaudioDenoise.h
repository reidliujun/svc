#ifndef _CAUDIODENOISEPAGE_H_
#define _CAUDIODENOISEPAGE_H_

#include "TconfPageDecAudio.h"

class TaudioDenoisePage :public TconfPageDecAudio
{
public:
 TaudioDenoisePage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

