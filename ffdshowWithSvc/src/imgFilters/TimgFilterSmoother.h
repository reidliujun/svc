#ifndef _TIMGFILTERSMOOTHER_H_
#define _TIMGFILTERSMOOTHER_H_

#include "TimgFilter.h"

struct Tblur;

DECLARE_FILTER(TimgFilterSmootherLuma,public,TimgFilter)
protected:
 struct Tblur
  {
  private:
   static const int smoother_blur_square_table[765*2+1];
   static const unsigned char smoother_blur_codes[];
   unsigned int dx,dy;
   unsigned char *sum_row[5],*avg_row[5];
   static void avgrow(const unsigned char *src,unsigned char *dst,unsigned int width);
   static void filtrow_1(unsigned char *sum,const unsigned char *src,unsigned int dx,stride_t stride,int thresh);
  public:
   Tblur(unsigned int Idx,unsigned int Idy);
   ~Tblur();
   void run(const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,int g_thresh);
  };

 Tblur *blur1;
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void);
public:
 TimgFilterSmootherLuma(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

DECLARE_FILTER(TimgFilterSmootherChroma,public,TimgFilterSmootherLuma)
private:
 Tblur *blur2;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
public:
 TimgFilterSmootherChroma(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
