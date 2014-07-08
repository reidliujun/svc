#ifndef _FF_WMV9_H_
#define _FF_WMV9_H_

#include "char_t.h"
#include "imgFilters/ffImgfmt.h"

enum VIDEO_VBR_MODE
{
 VBR_OFF,          // VBR is not used (CBR)
 VBR_QUALITYBASED, // Quality based VBR, generally one pass encoding
 VBR_CONSTRAINED,  // Constrain the Peak Bitrate and also ( optionally ) the peak bufferwindow, 2 Pass
 VBR_UNCONSTRAINED // No constraint on the Peak Bitrate, generally 2 pass
};

struct Tff_wmv9cfg
{
 unsigned int codecIndex;
 int bitrate;
 int width,height;
 int csp;
 double fps;
 int crisp;
 int quality;
 int seckf;
 BOOL bUseVBR;
 VIDEO_VBR_MODE vbr_mode;
 int vbrquality;
 int maxbitrate;
 bool ivtc,deint;
 int cplx;
 const char_t *flnm;
 bool avioutput;
};

struct Tff_wmv9codecInfo
{
 unsigned int index;
 char_t name[256];
 FOURCC fcc;GUID mediatype;
 BOOL vbr;
 int cmplxMax;
};

struct Iff_wmv9
{
 virtual bool __stdcall getOk(void)=0;
 virtual size_t  __stdcall getCodecCount(void)=0;
 virtual bool __stdcall getCodecInfo(size_t i,const Tff_wmv9codecInfo* *info)=0;
 virtual bool __stdcall start(const Tff_wmv9cfg &cfg)=0;
 virtual void __stdcall getExtradata(const void* *ptr,size_t *len)=0;
 virtual int  __stdcall write(unsigned int framenum,int incsp,const unsigned char * const src[4],const stride_t srcStride[4],void *dst)=0;
 virtual void __stdcall end(void)=0;
 virtual const Tff_wmv9codecInfo* __stdcall findCodec(FOURCC fcc)=0;
 virtual bool __stdcall decStart(FOURCC fcc,double fps,unsigned int dx,unsigned int dy,const void *extradata,size_t extradata_len,int *csp)=0;
 virtual size_t __stdcall decompress(const unsigned char *src,size_t srcLen,unsigned char* *dst,stride_t *dstStride)=0;
};

extern "C" Iff_wmv9* __stdcall createWmv9(void);
extern "C" void __stdcall destroyWmv9(Iff_wmv9 *self);

#endif
