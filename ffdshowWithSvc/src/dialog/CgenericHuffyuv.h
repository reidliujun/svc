#ifndef _CGENERICHUFFYUVPAGE_H_
#define _CGENERICHUFFYUVPAGE_H_

#include "TconfPageEnc.h"
#include "ffcodecs.h"

class TgenericHuffyuvPage :public TconfPageEnc
{
public:
 TgenericHuffyuvPage(TffdshowPageEnc *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual bool forCodec(int codecId) {return codecId==CODEC_ID_HUFFYUV;}
 virtual bool enabled(void);
 virtual void translate(void);
};

#endif

