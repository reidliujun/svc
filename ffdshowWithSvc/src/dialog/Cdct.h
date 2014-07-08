#ifndef _CDCTPAGE_H_
#define _CDCTPAGE_H_

#include "TconfPageDecVideo.h"

class TdctPage :public TconfPageDecVideo
{
private:
 static const int eds[],idffs[];
 void coeffs2dlg(),quant2dlg(void);
 void onMatrix(void);
public:
 TdctPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

