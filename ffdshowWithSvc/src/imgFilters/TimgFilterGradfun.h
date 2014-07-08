#ifndef _TIMGFILTERGRADFUN_H_
#define _TIMGFILTERGRADFUN_H_

#include "TimgFilter.h"

DECLARE_FILTER(TimgFilterGradfun,public,TimgFilter)
private:
 static const int edgeSize=16;
 int oldthresh;
 struct TgradFun
  {
  private:
   int32_t *pBuffer;
   stride_t nBufferPitch;

   int nThreshold;

   static void gf_prepare_mmx(int32_t *pDst, stride_t nDstPitch, const uint8_t *pSrc, stride_t nSrcPitch, unsigned int nWidth, unsigned int nHeight);
   static void gf_render_mmx(uint8_t *pDst, stride_t nDstPitch, const int32_t *pSrc, stride_t nSrcPitch, unsigned int nWidth, unsigned int nHeight, int nThr);
  public:
   bool ok;
   TgradFun(unsigned int dx,unsigned int dy,float dThreshold);
   ~TgradFun();
   int GF_filter(uint8_t *pPlane, stride_t nPitch, unsigned int nWidth, unsigned int nHeight);
  } *gradFun;
 Tbuffer edgebuf;
protected:
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
public:
 TimgFilterGradfun(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual void onSizeChange(void);
 virtual void done(void);
};

#endif
