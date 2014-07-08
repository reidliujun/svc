#ifndef _TIMGFILTERSHARPEN_H_
#define _TIMGFILTERSHARPEN_H_

#include "TimgFilter.h"
#include "asharp/asharp.h"
#include "TsharpenSettings.h"

DECLARE_FILTER(TimgFilterAsharp,public,TimgFilter)
private:
 int asharpCfgT,asharpCfgD,asharpCfgB,asharpCfgB2;
 int oldAsharpT,oldAsharpD,oldAsharpB,oldAsharpHQBF;
 unsigned char *aline;
 asharp_run_fct *asharp_run;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void) {done();}
public:
 TimgFilterAsharp(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

struct SwsContext;
struct SwsFilter;
struct Tlibmplayer;
DECLARE_FILTER(TimgFilterMplayerSharp,public,TimgFilter)
private:
 Tlibmplayer *libmplayer;
 SwsContext *swsc;
 SwsFilter *swsf;
 int oldmplayersharpenluma,oldmplayersharpenchroma;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return SWS_IN_CSPS;}
 virtual int getSupportedOutputColorspaces(const TfilterSettingsVideo *cfg) const {return SWS_OUT_CSPS;}
 virtual void onSizeChange(void) {done();}
public:
 TimgFilterMplayerSharp(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterMplayerSharp();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

struct TsharpenSettings;
DECLARE_FILTER(TimgFilterUnsharp,public,TimgFilter)
private:
 unsigned short *Ysum;stride_t minStride;
 void (TimgFilterUnsharp::*unsharpFc)(const TsharpenSettings *cfg,const unsigned char *src,unsigned char *dst,unsigned short *sum);
 template<class _mm> void unsharp(const TsharpenSettings *cfg,const unsigned char *src,unsigned char *dst,unsigned short *sum);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void) {done();}
public:
 TimgFilterUnsharp(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

DECLARE_FILTER(TimgFilterXsharp,public,TimgFilter)
private:
 unsigned char *Ymin,*Ymax;stride_t minStride;
 void (TimgFilterXsharp::*xsharpenFc)(unsigned int dx1,unsigned int dy1,const TsharpenSettings *cfg,const unsigned char *srcY,stride_t stride1,unsigned char *dstY,stride_t stride2);
 template<class _mm> void xsharpen(unsigned int dx1,unsigned int dy1,const TsharpenSettings *cfg,const unsigned char *srcY,stride_t stride1,unsigned char *dstY,stride_t stride2);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void) {done();}
public:
 TimgFilterXsharp(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

class T3x3blurSWS;
DECLARE_FILTER(TimgFilterMsharp,public,TimgFilter)
private:
 stride_t stride;
 unsigned char *blur,*work;
 T3x3blurSWS *blur3x3;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void) {done();}
public:
 TimgFilterMsharp(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
