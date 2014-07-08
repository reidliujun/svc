#ifndef _CMESKALPAGE_H_
#define _CMESKALPAGE_H_

#include "TconfPageEnc.h"
#include "ffcodecs.h"

class TmeSkalPage :public TconfPageEnc
{
private:
 static const char_t *metrics[];
public:
 TmeSkalPage(TffdshowPageEnc *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual bool forCodec(int codecId) {return codecId==CODEC_ID_SKAL;}
};

#endif

