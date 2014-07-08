#ifndef _T3X3BLUR_H_
#define _T3X3BLUR_H_

#include "interfaces.h"
#include "ffImgfmt.h"

struct Tlibmplayer;
struct SwsContext;
class T3x3blurSWS
{
private:
 Tlibmplayer *libmplayer;
 SwsContext *swsc;
 unsigned int dx,dy;
public:
 T3x3blurSWS(IffdshowBase *deci,unsigned int Idx,unsigned int Idy);
 ~T3x3blurSWS();
 void process(const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride);
};

#endif
