#ifndef _CMASKINGTHEOPAGE_H_
#define _CMASKINGTHEOPAGE_H_

#include "TconfPageEnc.h"

class TmaskingPageTheo :public TconfPageEnc
{
private:
 static const char_t *sharpenesss[];
 void sensitivity2dlg(void);
public:
 TmaskingPageTheo(TffdshowPageEnc *Iparent);
 virtual bool enabled(void);
 virtual bool forCodec(int codecId) {return theora_codec(codecId);}
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

