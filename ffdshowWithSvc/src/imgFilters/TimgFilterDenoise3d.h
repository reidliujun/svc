#ifndef _TIMGFILTERDENOISE3D_H_
#define _TIMGFILTERDENOISE3D_H_

#include "TimgFilter.h"

DECLARE_FILTER(TimgFilterDenoise3d,public,TimgFilter)
private:
 int oldLuma,oldChroma,oldTime;
protected:
 int FrameStride[3];
 TimgFilterDenoise3d(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void precalcCoefs(int nCt, double Dist25)=0;
 virtual void deNoise(TffPict &pict,unsigned int dx,unsigned int dy,const unsigned char *src[4],stride_t stride1[4],unsigned char *dst[4],stride_t stride2[4])=0;
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
public:
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual void onSizeChange(void);
 virtual void onSeek(void);
};

DECLARE_FILTER(TimgFilterDenoise3dNormal,public,TimgFilterDenoise3d)
private:
 int Coefs[4][512];
 unsigned char *Line;
 unsigned char *Frame[3];
 static inline uint8_t LowPass(uint8_t Prev,uint8_t Curr,const int *Coef)
  {
   return uint8_t(Curr+Coef[Prev-Curr]);
  }
 static void deNoise(const unsigned char *Frame,
                     const unsigned char *FramePrev,
                     unsigned char *FrameDest,
                     unsigned char *LineAnt,
                     int W, int H, stride_t sStride, stride_t pStride, stride_t dStride,
                     int *Horizontal, int *Vertical, int *Temporal);
protected:
 void precalcCoefs(int nCt, double Dist25);
 virtual void deNoise(TffPict &pict,unsigned int dx,unsigned int dy,const unsigned char *src[4],stride_t stride1[4],unsigned char *dst[4],stride_t stride2[4]);
public:
 TimgFilterDenoise3dNormal(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
};

DECLARE_FILTER(TimgFilterDenoise3dHQ,public,TimgFilterDenoise3d)
private:
 int Coefs[4][512*16];
 unsigned int *Line;
 unsigned short *Frame[3];
 static inline unsigned int LowPassMul(unsigned int PrevMul, unsigned int CurrMul,const int *Coef)
  {
   int dMul=PrevMul-CurrMul;
   int d=(dMul+0x10007FF)>>12;
   return CurrMul + Coef[d];
  }
 static void deNoiseTemporal(
                    const unsigned char *Frame,        // mpi->planes[x]
                    unsigned char *FrameDest,    // dmpi->planes[x]
                    unsigned short *FrameAnt,
                    int W, int H, stride_t sStride, stride_t dStride,
                    int *Temporal);
 static void deNoiseSpacial(
                     const unsigned char *Frame,        // mpi->planes[x]
                     unsigned char *FrameDest,    // dmpi->planes[x]
                     unsigned int *LineAnt,       // vf->priv->Line (width bytes)
                     int W, int H, stride_t sStride, stride_t dStride,
                     int *Horizontal, int *Vertical);
 static void deNoise(const unsigned char *Frame,  // mpi->planes[x]
                     unsigned char *FrameDest,    // dmpi->planes[x]
                     unsigned int *LineAnt,       // vf->priv->Line (width bytes)
		     unsigned short **FrameAntPtr, int *FrameAntStride,
                     int W, int H, stride_t sStride, stride_t dStride,
                     int *Horizontal, int *Vertical, int *Temporal);
protected:
 virtual void precalcCoefs(int nCt, double Dist25);
 virtual void deNoise(TffPict &pict,unsigned int dx,unsigned int dy,const unsigned char *src[4],stride_t stride1[4],unsigned char *dst[4],stride_t stride2[4]);
public:
 TimgFilterDenoise3dHQ(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
};

#endif
