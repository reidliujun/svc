#ifndef _CMASKINGXVIDPAGE_H_
#define _CMASKINGXVIDPAGE_H_

#include "TconfPageEnc.h"

class TmaskingPageXvid :public TconfPageEnc
{
public:
 TmaskingPageXvid(TffdshowPageEnc *Iparent);
 virtual bool enabled(void);
 virtual bool forCodec(int codecId) {return xvid_codec(codecId);}
 virtual void cfg2dlg(void);
};

#endif

