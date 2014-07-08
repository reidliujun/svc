#ifndef _CVOBSUBPAGE_H_
#define _CVOBSUBPAGE_H_

#include "TconfPageDecVideo.h"

class TvobsubPage :public TconfPageDecVideo
{
private:
 void scale2dlg(void),gauss2dlg(void);
public:
 TvobsubPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

