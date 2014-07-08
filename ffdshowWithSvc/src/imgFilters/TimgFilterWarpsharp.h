#ifndef _TIMGFILTERWARPSHARP_H_
#define _TIMGFILTERWARPSHARP_H_

#include "TimgFilter.h"
#include "TcubicTable.h"
#include "awarpsharp/aws_api.h"

class T3x3blurSWS;
struct TwarpsharpSettings;
struct SwsContext;
struct Tlibmplayer;
DECLARE_FILTER(TimgFilterWarpsharp,public,TimgFilter)
private:
 Tlibmplayer *libmplayer;
 unsigned char *blur;
 SwsContext *swsblur;
 stride_t blurworkStride;
 TcubicTable warpcubic;
 void (*warpsharpbumpFc)(const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,unsigned int dxY,unsigned int dyY,int wsthresh);
 template<class _mm> static void warpsharpbump(const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,unsigned int dxY,unsigned int dyY,int wsthresh);
 void warpsharpblur(const unsigned char *src,stride_t srcstride,unsigned char *dst,stride_t dststride,unsigned int dxY,unsigned int dyY);
 void warpsharp(const unsigned char *src,unsigned char *dst,const TwarpsharpSettings *cfg);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void) {done();}
public:
 TimgFilterWarpsharp(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterWarpsharp();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

DECLARE_FILTER(TimgFilterAwarpsharp,public,TimgFilter)
private:
 awsapi_settings aws;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSP_420P|FF_CSP_FLAGS_YUV_ADJ;}
 virtual void onSizeChange(void) {done();}
public:
 TimgFilterAwarpsharp(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
