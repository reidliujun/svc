#ifndef _TSWSCALE_H_
#define _TSWSCALE_H_

#include "ffImgfmt.h"

struct Tlibmplayer;
struct SwsContext;
struct Tswscale
{
private:
 Tlibmplayer *libmplayer;
 unsigned int dx,dy;
public:
 Tswscale(Tlibmplayer *Ilibmplayer);
 ~Tswscale();
 SwsContext *swsc;
 static bool getVersion(char *ver);
 bool init(unsigned int dx, unsigned int dy, int incsp, int outcsp, const int32_t yuv2rgbTable[6]);
 bool convert(const uint8_t* src[], const stride_t srcStride[], uint8_t* dst[], stride_t dstStride[]);
 void done(void);
};

#endif
