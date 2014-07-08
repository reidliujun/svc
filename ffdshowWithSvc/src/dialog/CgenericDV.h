#ifndef _CGENERICDVPAGE_H_
#define _CGENERICDVPAGE_H_

#include "TconfPageEnc.h"
#include "ffcodecs.h"

class TgenericDVpage :public TconfPageEnc
{
private:
public:
 TgenericDVpage(TffdshowPageEnc *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual bool forCodec(int codecId) {return codecId==CODEC_ID_DVVIDEO;}
 virtual bool enabled(void);
 virtual void translate(void);
};

#endif

