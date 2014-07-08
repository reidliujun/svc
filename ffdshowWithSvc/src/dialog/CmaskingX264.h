#ifndef _CMASKINGX264PAGE_H_
#define _CMASKINGX264PAGE_H_

#include "TconfPageEnc.h"

class TmaskingPageX264 :public TconfPageEnc
{
public:
 TmaskingPageX264(TffdshowPageEnc *Iparent);
 virtual bool enabled(void);
 virtual bool forCodec(int codecId) {return codecId==CODEC_ID_X264;}
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

