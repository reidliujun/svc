#ifndef _CMASKINGSKALPAGE_H_
#define _CMASKINGSKALPAGE_H_

#include "TconfPageEnc.h"
#include "ffcodecs.h"

class TmaskingPageSkal :public TconfPageEnc
{
public:
 TmaskingPageSkal(TffdshowPageEnc *Iparent);
 virtual bool forCodec(int codecId) {return codecId==CODEC_ID_SKAL;}
 virtual void cfg2dlg(void);
};

#endif

