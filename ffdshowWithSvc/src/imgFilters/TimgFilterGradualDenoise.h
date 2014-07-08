#ifndef _TIMGFILTERGRADUALDENOISE_H_
#define _TIMGFILTERGRADUALDENOISE_H_

#include "TimgFilter.h"

DECLARE_FILTER(TimgFilterGradualDenoise,public,TimgFilter)
private:
 unsigned char *prevImg;unsigned int prevStride;
 void (TimgFilterGradualDenoise::*denoiseFc)(int gradualStrength,unsigned int dx1,unsigned int dy1,const unsigned char *srcY,stride_t stride1,unsigned char *dstY,stride_t stride2);
 template<class _mm> void denoise(int gradualStrength,unsigned int dx1,unsigned int dy1,const unsigned char *srcY,stride_t stride1,unsigned char *dstY,stride_t stride2);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void);
public:
 TimgFilterGradualDenoise(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual void onSeek(void);
};

#endif
