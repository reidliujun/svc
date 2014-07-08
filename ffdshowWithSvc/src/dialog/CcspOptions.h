#ifndef _CRGBCONV_H_
#define _CRGBCONV_H_

#include "TconfPageDecVideo.h"

class TcspOptionsPage :public TconfPageDecVideo
{
public:
 TcspOptionsPage(TffdshowPageDec *Iparent);
 ~TcspOptionsPage(){};
 virtual bool reset(bool testonly=false);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void getTip(char_t *tipS,size_t len);
 virtual void translate(void);
};

#endif

