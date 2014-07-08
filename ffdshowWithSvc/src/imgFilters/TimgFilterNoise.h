#ifndef _TIMGFILTERNOISE_H_
#define _TIMGFILTERNOISE_H_

#include "TimgFilter.h"
#include "TnoiseSettings.h"
#include "simd.h"

DECLARE_FILTER(TimgFilterNoise,public,TimgFilter)
public:
 static const int patt[4];
private:
 int noiseAvihStrength,noiseAvihStrengthChroma;
 int oldPattern;
 int oldcsp;void doneChroma(void);
 short *noiseMask[3];stride_t noiseMaskStride[3];int noiseCount[3];
 void genAVIHnoise(int plane,int strength);
 TnoiseSettings oldnoise;
 static __m64 noiseConst64,noisenext64;
 #ifdef __SSE2__
  static __m128i noiseConst128,noisenext128;
 #endif
 template<class _mm> struct Tnoise
  {
   static typename _mm::__m &noiseConst(),&noisenext();
   static void init(void);
   static void reset(void);
  };
 template<class _mm,bool chroma,bool avih,bool uniform> static void addNoise(const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,unsigned int dx,unsigned int dy,short *noiseMask,stride_t noiseMaskStride,int noiseStrength);
 template<class _mm,bool chroma,bool avih> static void generateAddNoise(const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,unsigned int dx,unsigned int dy,int noiseStrength,int uniformNoise,short *noiseMask,stride_t noiseMaskStride,int noiseCount);
 void (TimgFilterNoise::*processLumaFc)(const TnoiseSettings *cfg,const unsigned char *srcY,unsigned char *dstY);
 template<class _mm> void processLuma(const TnoiseSettings *cfg,const unsigned char *srcY,unsigned char *dstY);
 void (TimgFilterNoise::*processChromaFc)(const TnoiseSettings *cfg,const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstU,unsigned char *dstV);
 template<class _mm> void processChroma(const TnoiseSettings *cfg,const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstU,unsigned char *dstV);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void);
public:
 TimgFilterNoise(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

DECLARE_FILTER(TimgFilterNoiseMplayer,public,TimgFilter)
private:
 struct Tprocess : safe_bool<Tprocess>
  {
  private:
   enum
    {
     MAX_NOISE=4096,
     MAX_SHIFT=1024,
     MAX_RES=MAX_NOISE-MAX_SHIFT
    };
   static int RAND_N(int range)
    {
     return int((double)range*rand()/(RAND_MAX+1.0));
    }
   int8_t *noise;
   int8_t *prev_shift[MAX_RES][3];
   int nonTempRandShift[MAX_RES];
   int shiftptr;
   TnoiseSettings cfg;
   void (*lineNoise)(uint8_t *dst,const uint8_t *src, int8_t *noise, int len, int shift);
   void (*lineNoiseAvg)(uint8_t *dst,const uint8_t *src, int len, int8_t **shift);
   static void lineNoise_C(uint8_t *dst,const uint8_t *src, int8_t *noise, int len, int shift);
   template<class _mm> static void lineNoise_simd(uint8_t *dst,const uint8_t *src, int8_t *noise, int len, int shift);
   static void lineNoiseAvg_C(uint8_t *dst,const uint8_t *src, int len, int8_t **shift);
   template<class _mm> static void lineNoiseAvg_simd(uint8_t *dst,const uint8_t *src, int len, int8_t **shift);
  public:
   Tprocess(void);
   ~Tprocess()
    {
     done();
    };
   void init(int strength,const TnoiseSettings *cfg);
   void process(const uint8_t *src,uint8_t *dst,stride_t dstStride,stride_t srcStride,unsigned int width,unsigned int height);
   void done(void);
   bool boolean_test(void) const {return !!noise;}
  } y,uv;
 TnoiseSettings oldnoise;
 int oldcsp;void doneChroma(void);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void) {done();}
public:
 TimgFilterNoiseMplayer(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#include "TpictPropSettings.h"
class TimgFilterLuma;
DECLARE_FILTER(TimgFilterFlicker,public,TimgFilter)
private:
 TimgFilterLuma *luma;TpictPropSettings lumaSettings;int lumaT;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
public:
 TimgFilterFlicker(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterFlicker();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#include "ToffsetSettings.h"
class TimgFilterOffset;
DECLARE_FILTER(TimgFilterShake,public,TimgFilter)
private:
 TimgFilterOffset *offset;ToffsetSettings offsetSettings;int offsetTX,offsetTY;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
public:
 TimgFilterShake(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterShake();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

DECLARE_FILTER(TimgFilterFilmLines,public,TimgFilter)
private:
 struct Tline
  {
  private:
   unsigned int dx;
   int life;
  public:
   bool firsttime;
   Tline(void):firsttime(true) {}
   Tline(const TnoiseSettings *cfg);
   void setDx(unsigned int Idx);
   bool update(void);
   unsigned char color;
   int strength;
   int x,diffx;
  };
 typedef std::list<Tline> Tlines;
 Tlines lines;int linesT;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void);
public:
 TimgFilterFilmLines(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual void onSeek(void);
};

DECLARE_FILTER(TimgFilterScratches,public,TimgFilter)
private:
 int scratchesT;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
public:
 TimgFilterScratches(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
