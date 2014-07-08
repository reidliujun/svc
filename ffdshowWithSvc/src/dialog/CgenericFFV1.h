#ifndef _CGENERICFFV1PAGE_H_
#define _CGENERICFFV1PAGE_H_

#include "TconfPageEnc.h"
#include "ffcodecs.h"

class TgenericFFV1page :public TconfPageEnc
{
public:
 TgenericFFV1page(TffdshowPageEnc *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual bool forCodec(int codecId) {return codecId==CODEC_ID_FFV1;}
 virtual bool enabled(void);
};

#endif

