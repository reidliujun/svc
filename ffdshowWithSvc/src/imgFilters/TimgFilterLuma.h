#ifndef _TIMGFILTERLUMA_H_
#define _TIMGFILTERLUMA_H_

#include "TimgFilter.h"
#include "postproc/postprocess.h"

struct TpictPropSettings;
DECLARE_FILTER(TimgFilterLuma,public,TimgFilter)
private:
 unsigned int gammaTab[256],gammaTab8[256],gammaTab16[256],gammaTab24[256];int oldGamma;
 void processGamma(const unsigned char *srcY,unsigned char *dstY,const TpictPropSettings *cfg);
 void (TimgFilterLuma::*processLumaFc)(const TpictPropSettings *cfg,const unsigned char *srcY,unsigned char *dstY,int ystep);
 template<class _mm> void processLuma(const TpictPropSettings *cfg,const unsigned char *srcY,unsigned char *dstY,int ystep);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
public:
 TimgFilterLuma(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

DECLARE_FILTER(TimgFilterGammaRGB,public,TimgFilter)
private:
 int oldGammaR,oldGammaG,oldGammaB;
 unsigned int gammaTabR[256],gammaTabG[256],gammaTabB[256];
 unsigned int gammaTabYUV[3][256];
 void processGammaRGB32(const unsigned char *src,unsigned char *dst,const TpictPropSettings *cfg);
 void processGammaYUV(TffPict &pict,const unsigned char *src[4],unsigned char *dst[4],const TpictPropSettings *cfg);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR|FF_CSP_RGB32;}
public:
 TimgFilterGammaRGB(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

struct Tlibmplayer;
DECLARE_FILTER(TimgFilterLevelFix,public,TimgFilter)
private:
 Tlibmplayer *libmplayer;
 void *pp_ctx;PPMode pp_mode;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void);
public:
 TimgFilterLevelFix(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterLevelFix();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
