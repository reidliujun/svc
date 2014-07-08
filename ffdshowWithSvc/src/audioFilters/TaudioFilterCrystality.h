#ifndef _TAUDIOFILTERCRYSTALITY_H_
#define _TAUDIOFILTERCRYSTALITY_H_

#include "TaudioFilter.h"
#include "TcrystalitySettings.h"

DECLARE_FILTER(TaudioFilterCrystality,public,TaudioFilter)
private:
 TcrystalitySettings old;unsigned int oldnchannels;

 struct Tbandext
  {
  private:
   int bext_sfactor;
   int filter_level;

   int prev0, prev1, prev2;
   int amplUp, amplDown;

   void pitchShifter(const int &in, int *out);
   static const int SH_BUF_SIZE=10*3;
   int shBuf[SH_BUF_SIZE];
   int shBufPos;
   int shBufPos1;
   int cond;

   struct Interpolation
    {
     int acount;// counter
     int lval;	// value
     int sal;	// sum
     int al;
     int a1l;
    } bandext_energy,bandext_amplitude;
   static void interpolate(Interpolation *s, int l);
   static int calc_scalefactor(int a, int e);
  public:
   Tbandext(void) {onSeek();}
   virtual void init(const TcrystalitySettings *cfg);
   virtual void process(int16_t *data, const size_t datasize,unsigned int N);
   virtual void onSeek(void);
  };
 Tbandext bandext[8];

 struct Techo3d
  {
  private:
   int stereo_sfactor;
   int echo_sfactor;
   int feedback_sfactor;
   int harmonics_sfactor;

   int left0p,right0p;
   int lsine[65536];
   int rsine[65536];
   int ll0, ll1, ll2, rr0, rr1, rr2;
   int l0, l1, l2, r0, r1, r2, ls, rs, ls1, rs1;
   int lharmb, rharmb, lhfb, rhfb;
   static const int DELAY2=21000;
   static const int DELAY1=35000;
   static const int DELAY3=14000;
   static const int DELAY4=5;
   static const int BUF_SIZE=DELAY1+DELAY2+DELAY3+DELAY4;
   int16_t buf[BUF_SIZE];
   int bufPos;
   int bufPos1;
   int bufPos2;
   int bufPos3;
   int bufPos4;
  public:
   Techo3d(void) {onSeek();}
   void init(const TcrystalitySettings *cfg);
   void process(int16_t *data, size_t datasize);
   void onSeek(void);
  } echo3d;

protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_PCM16;}
public:
 TaudioFilterCrystality(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TaudioFilterCrystality();
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void onSeek(void);
};

#endif
