#ifndef _CSHOWMVPAGE_H_
#define _CSHOWMVPAGE_H_

#include "TconfPageDecVideo.h"

class TshowMVpage :public TconfPageDecVideo
{
public:
 TshowMVpage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void cfg2dlg(void);
};

#endif

