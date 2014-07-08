#ifndef _CNOISEPAGE_H_
#define _CNOISEPAGE_H_

#include "TconfPageDecVideo.h"

class TnoisePage :public TconfPageDecVideo
{
private:
 void noise2dlg(void),flicker2dlg(void),shake2dlg(void),lines2dlg(void),scratches2dlg(void);
 char_t grayS[256];
 const char_t *nameGray(int id,int c);
public:
 TnoisePage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

