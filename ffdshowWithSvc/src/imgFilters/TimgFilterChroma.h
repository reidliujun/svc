#ifndef _TIMGFILTERCHROMA_H_
#define _TIMGFILTERCHROMA_H_

#include "TimgFilter.h"

struct TpictPropSettings;
DECLARE_FILTER(TimgFilterChroma,public,TimgFilter)
private:
 short hueSin[360+1],hueCos[360+1];
 void (TimgFilterChroma::*chromaFc)(const TpictPropSettings *cfg,const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstU,unsigned char *dstV);
 template<class _mm> void chroma(const TpictPropSettings *cfg,const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstU,unsigned char *dstV);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
public:
 TimgFilterChroma(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
