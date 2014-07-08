#ifndef _TLIBMPLAYER_H_
#define _TLIBMPLAYER_H_

#include "postproc/postprocess.h"
#include "libaf/reorder_ch.h"
#include "TpostprocSettings.h"
#include "yadif/vf_yadif.h"

class Tdll;
struct Tconfig;
struct SwsContext;
struct SwsFilter;
struct SwsVector;
struct mp3lib_ctx;
struct SwsParams;
struct Tlibmplayer
{
private:
 Tdll *dll;
 Tlibmplayer(const Tconfig *config);
 ~Tlibmplayer();
 friend class TffdshowBase;
 friend class TffColorspaceConvert;
 int refcount;
public:
 void AddRef(void)
  {
   refcount++;
  }
 void Release(void)
  {
   if (--refcount<0)
    delete this;
  }
 static const char_t *dllname;

 void (*init_mplayer)(int mmx,int mmx2,int _3dnow,int _3dnowExt,int sse,int sse2,int ssse3);

 pp_context_t* (*pp_get_context)(int width, int height, int flags);
 void (*pp_postprocess)(const uint8_t * src[3], stride_t srcStride[3], uint8_t * dst[3], stride_t dstStride[3], int horizontalSize, int verticalSize, QP_STORE_T *QP_store,  int QP_stride, pp_mode_t *mode, pp_context_t *ppContext, int pict_type);
 void (*pp_free_context)(pp_context_t *ppContext);

 SwsVector* (*sws_getGaussianVec)(double variance, double quality);
 SwsVector* (*sws_getConstVec)(double c,int length);
 void (*sws_freeVec)(SwsVector *a);
 void (*sws_normalizeVec)(SwsVector *a, double height);
 SwsFilter* (*sws_getDefaultFilter)(float lumaGBlur, float chromaGBlur, float lumaSarpen, float chromaSharpen, float chromaHShift, float chromaVShift, int verbose);
 void (*sws_freeFilter)(SwsFilter *filter);
 SwsContext* (*sws_getContext)(int srcW, int srcH, int srcFormat, int dstW, int dstH, int dstFormat, SwsParams *params, SwsFilter *srcFilter, SwsFilter *dstFilter,const int32_t yuv2rgbTable[6]);
 int (*sws_scale)(struct SwsContext *context, const uint8_t* src[], const stride_t srcStride[], int srcSliceY, int srcSliceH, uint8_t* dst[], stride_t dstStride[]);
 int (*sws_scale_ordered)(SwsContext *c, const uint8_t* src[], const stride_t srcStride[], int srcSliceY,int srcSliceH, uint8_t* dst[], stride_t dstStride[]);
 void (*sws_freeContext)(struct SwsContext *swsContext);
 void (*reorder_channel_nch) (void *buf, int src_layout,int dest_layout,int chnum,int samples,int samplesize);

 mp3lib_ctx* (*MP3_Init)(int mono);
 int (*MP3_DecodeFrame)(mp3lib_ctx* ctx,const unsigned char *Isrc,unsigned int Isrclen,unsigned char *hova,short single,unsigned int *srcUsed);
 void (*MP3_Done)(mp3lib_ctx *ctx);

 void (*palette8torgb32)(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
 void (*palette8tobgr32)(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
 void (*palette8torgb24)(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
 void (*palette8tobgr24)(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
 void (*palette8torgb16)(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
 void (*palette8tobgr16)(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
 void (*palette8torgb15)(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
 void (*palette8tobgr15)(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
 void (*decCPUCount)(void);
 void (*incCPUCount)(void);

 void (*yadif_init)(YadifContext *yadctx);
 void (*yadif_uninit)(YadifContext *yadctx);
 void (*yadif_filter)(YadifContext *yadctx, uint8_t *dst[3], stride_t dst_stride[3], int width, int height, int parity, int tff);


 static int swsCpuCaps(void);
 static void swsInitParams(SwsParams *params,int resizeMethod);
 static int ppCpuCaps(int csp=0);
 static void pp_mode_defaults(PPMode &ppMode);
 static int getPPmode(const TpostprocSettings *cfg,int currentq);
};

#endif
