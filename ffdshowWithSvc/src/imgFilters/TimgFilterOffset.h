#ifndef _TIMGFILTEROFFSET_H_
#define _TIMGFILTEROFFSET_H_

#include "TimgFilter.h"

DECLARE_FILTER(TimgFilterOffset,public,TimgFilter)
private:
 int Y_X,Y_Y,U_X,U_Y,V_X,V_Y;
 static void offset(int c,const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,unsigned int dx,unsigned int dy,int offsetX,int offsetY);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void);
public:
 TimgFilterOffset(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

DECLARE_FILTER(TimgFilterFlip,public,TimgFilter)
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK&~(FF_CSP_NV12|FF_CSP_CLJR);}
public:
 TimgFilterFlip(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

DECLARE_FILTER(TimgFilterMirror,public,TimgFilter)
private:
 template<class pixel_t> void mirror(int i,unsigned int dx,unsigned char *line);
 void (TimgFilterMirror::*mirrorFc)(int i,unsigned int dx,unsigned char *line);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK&~(FF_CSP_NV12|FF_CSP_CLJR);}
public:
 TimgFilterMirror(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
