#ifndef _TIMGFILTERDEINTERLACE_H_
#define _TIMGFILTERDEINTERLACE_H_

#include "TimgFilter.h"
#include "postproc/postprocess.h"
#include "simd.h"

DECLARE_FILTER(TimgFilterDeinterlace,public,TimgFilter)
protected:
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK;}
public:
 TimgFilterDeinterlace(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

struct Tlibmplayer;
DECLARE_FILTER(TimgFilterMplayerDeinterlace,public,TimgFilter)
private:
 Tlibmplayer *libmplayer;
 void *pp_ctx;PPMode pp_mode;
protected:
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void) {done();}
public:
 TimgFilterMplayerDeinterlace(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterMplayerDeinterlace();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

DECLARE_FILTER(TimgFilterFramerateDoubler,public,TimgFilter)
private:
 TffPict *old;Tbuffer oldbuf;
 template<class Tsimd> struct Tinterpolate
  {
   static void pictInterpolate(unsigned char *dst,stride_t dstStride,const unsigned char *src1,stride_t src1stride,const unsigned char *src2,stride_t src2stride,unsigned int dx,unsigned int dy);
   static inline void bestInterp(__m64 &mm5,__m64 &mm6,const __m64 *paddr1,const __m64 *paddr2);
   static void interpolateMotion(stride_t src_pit, stride_t dst_pit, stride_t prev_pit, unsigned int rowsize, const BYTE* srcp, BYTE* dstp, const BYTE* _pPrev, int FldHeight,int Search_Effort,int thresh);
  };
 void (*interpolateMotion)(stride_t src_pit, stride_t dst_pit, stride_t prev_pit, unsigned int rowsize, const BYTE* srcp, BYTE* dstp, const BYTE* _pPrev, int FldHeight,int Search_Effort,int thresh);
 void (*pictInterpolate)(unsigned char *dst,stride_t dstStride,const unsigned char *src1,stride_t src1stride,const unsigned char *src2,stride_t src2stride,unsigned int dx,unsigned int dy);
protected:
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void) {done();}
public:
 TimgFilterFramerateDoubler(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual void onSeek(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
