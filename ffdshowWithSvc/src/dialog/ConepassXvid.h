#ifndef _CONEPASSXVIDPAGE_H_
#define _CONEPASSXVIDPAGE_H_

#include "TconfPageEnc.h"
#include "ffcodecs.h"

class TonePassXvidPage :public TconfPageEnc
{
public:
 TonePassXvidPage(TffdshowPageEnc *Iparent);
 virtual void cfg2dlg(void);
 virtual bool forCodec(int codecId) {return xvid_codec(codecId);}
 virtual bool enabled(void);
};

#endif

