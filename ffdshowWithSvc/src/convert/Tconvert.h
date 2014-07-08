#ifndef _TCONVERT_H_
#define _TCONVERT_H_

#include "interfaces.h"
#include "ffImgfmt.h"
#include "IffColorspaceConvert.h"
#include "TrgbPrimaries.h"

//#define AVISYNTH_BITBLT //use avisynth bitblt function to just copy frame when no colorspace conversion is needed
#define XVID_BITBLT //use xvid's YV12 -> YV12 copy function - seems to be fastest

struct Tswscale;
struct Tconfig;
struct Tlibmplayer;
struct TcspInfo;
struct Tpalette;
struct TffPict;
struct ToutputVideoSettings;

class Tconvert : public TrgbPrimaries
{
private:
 void init(Tlibmplayer *Ilibmplayer,bool IavisynthYV12_RGB,unsigned int Idx,unsigned int Idy, int rgbInterlaceMode);
 bool avisynthYV12_RGB;
 Tlibmplayer *libmplayer;
 Tswscale *swscale;bool initsws;
 int oldincsp,oldoutcsp;
 int incsp1,outcsp1;
 const TcspInfo *incspInfo,*outcspInfo;
 int rgbInterlaceMode;

 enum
  {
   MODE_none,
   MODE_avisynth_yv12_to_yuy2,
   MODE_xvidImage_output,
   MODE_avisynth_yuy2_to_yv12,
   MODE_mmx_ConvertRGB32toYUY2,
   MODE_mmx_ConvertRGB24toYUY2,
   MODE_mmx_ConvertYUY2toRGB32,
   MODE_mmx_ConvertYUY2toRGB24,
   MODE_mmx_ConvertUYVYtoRGB32,
   MODE_mmx_ConvertUYVYtoRGB24,
   MODE_palette8torgb,
   MODE_CLJR,
   MODE_xvidImage_input,
   MODE_swscale,
   MODE_avisynth_bitblt,
   MODE_fallback
  } mode;
 static const char_t* getModeName(int mode);

 void (*avisynth_yv12_to_yuy2)(const BYTE* srcY, const BYTE* srcU, const BYTE* srcV, int src_rowsize, stride_t src_pitch, stride_t src_pitch_uv,
                               BYTE* dst, stride_t dst_pitch,
                               int height);
 void (*avisynth_yuy2_to_yv12)(const BYTE* src, int src_rowsize, stride_t src_pitch,
                               BYTE* dstY, BYTE* dstU, BYTE* dstV, stride_t dst_pitch, stride_t dst_pitchUV,
                               int height);
 void (*palette8torgb)(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
 int tmpcsp;
 unsigned char *tmp[3];stride_t tmpStride[3];
 Tconvert *tmpConvert1,*tmpConvert2;
 #ifdef AVISYNTH_BITBLT
 unsigned int rowsize;
 #endif
 void freeTmpConvert(void);
public:
 Tconvert(IffdshowBase *deci,unsigned int Idx,unsigned int Idy);
 Tconvert(Tlibmplayer *Ilibmplayer,bool IavisynthYV12_RGB,unsigned int Idx,unsigned int Idy,const TrgbPrimaries &IrgbPrimaries, int rgbInterlaceMode);
 ~Tconvert();
 unsigned int dx,dy,outdy;
 int convert(int incsp,const uint8_t*const src[],const stride_t srcStride[],int outcsp,uint8_t* dst[],stride_t dstStride[],const Tpalette *srcpal);
 int convert(const TffPict &pict,int outcsp,uint8_t* dst[],stride_t dstStride[]);
};

class TffColorspaceConvert :public CUnknown,
                            public IffColorspaceConvert
{
private:
 Tconfig *config;
 Tlibmplayer *libmplayer;
 Tconvert *c;
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr);
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
 DECLARE_IUNKNOWN

 TffColorspaceConvert(LPUNKNOWN punk,HRESULT *phr);
 virtual ~TffColorspaceConvert();

 STDMETHODIMP allocPicture(int csp,unsigned int dx,unsigned int dy,uint8_t *data[],stride_t stride[]);
 STDMETHODIMP freePicture(uint8_t *data[]);
 STDMETHODIMP convert(unsigned int dx,unsigned int dy,int incsp,uint8_t *src[],const stride_t srcStride[],int outcsp,uint8_t *dst[],stride_t dstStride[]);
 STDMETHODIMP convertPalette(unsigned int dx,unsigned int dy,int incsp,uint8_t *src[],const stride_t srcStride[],int outcsp,uint8_t *dst[],stride_t dstStride[],const unsigned char *pal,unsigned int numcolors);
};

#endif
