#ifndef _CGENERICRAWPAGE_H_
#define _CGENERICRAWPAGE_H_

#include "TconfPageEnc.h"
#include "ffcodecs.h"

class TgenericRAWpage :public TconfPageEnc
{
public:
 TgenericRAWpage(TffdshowPageEnc *Iparent);
 virtual bool forCodec(int codecId) {return raw_codec(codecId);}
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

