#ifndef _CGENERICLJPEGPAGE_H_
#define _CGENERICLJPEGPAGE_H_

#include "TconfPageEnc.h"
#include "ffcodecs.h"

class TgenericLJPEGpage :public TconfPageEnc
{
private:
 static const TcspFcc cspFccs[];
public:
 TgenericLJPEGpage(TffdshowPageEnc *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual bool forCodec(int codecId) {return codecId==CODEC_ID_LJPEG;}
 virtual bool enabled(void);
};

#endif

