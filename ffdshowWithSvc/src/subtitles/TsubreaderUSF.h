#ifndef _TSUBREADERUSF_H_
#define _TSUBREADERUSF_H_

#include "Tsubreader.h"
#include "interfaces.h"

class Tdll;
class Tconvert;
struct TsubreaderUSF2 : Tsubreader
{
private:
 int (*init)(void *pConfig,void **pxf,void **pcfg);
 void (*deinit)(void *pxf);
 void (*setDimension)(void *pxf,WORD pWidth,WORD pHeight);
 void (*loadInitText)(void *pxf,const wchar_t *pText);
 void (*loadUSFText)(void *pxf,const wchar_t *pText);
 int (*addSubtitleText)(void *pxf,const wchar_t *pText);
 int (*countVSubs)(void *pxf);
 DWORD (*getVSubStart)(void *pxf,int idx);
 DWORD (*getVSubStop)(void *pxf,int idx);
 void* (*getVSub)(void *pxf,int idx,int ms,int *width,int *height,int32_t** *lines);
 void (*freeVSub)(void *bmp,int32_t **lines);
 int (*changed)(void *pxf,int ms);
 void (*FlushSubtitles)(void *pxf);

 Tdll *dll;
 IffdshowBase *deci;
 void destroy(void);
 void *pixi,*pcfg;
 static wchar_t* stream2text(const Tstream &fs);
 static ffwstring referenceTimeToStr(const REFERENCE_TIME &time);
 int32_t **sublines;void *subbmp;
public:
 static const char_t *dllname;
 TsubreaderUSF2(const Tstream &f,IffdshowBase *Ideci,bool initonly);
 virtual ~TsubreaderUSF2();
 unsigned int dx,dy;
 Tconvert *convert;
 unsigned char *img[3],*alpha[3];int stride[3];
 void create(unsigned int dx,unsigned int dy);
 virtual void clear(void);
 void addSub(REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen);
 void* getSub(int idx,int ms,int *width,int *height,int32_t** *lines,bool *wasChange);
 void freeSub(void *bmp,int32_t **lines);
 bool wasChange(int ms);
};

#endif
