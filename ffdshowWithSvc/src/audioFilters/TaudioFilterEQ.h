#ifndef _TAUDIOFILTEREQ_H_
#define _TAUDIOFILTEREQ_H_

#include "TaudioFilter.h"
#include "TeqSettings.h"

class TaudioFilterEQsuper;
DECLARE_FILTER(TaudioFilterEQ,public,TaudioFilter)
private:
 static const double Q;
 class Tequalizer
  {
  private:
   static const int AF_NCH=8;
   static const int KM=10;
   float wq[AF_NCH][KM][2];      // Circular buffer for W data
   float g[AF_NCH][KM];
   float a[KM][2];               // A weights
   float b[KM][2];               // B weights
   int K;
   static void bp2(float* a, float* b, double fc, double q);
   template<class sample_t> float equalize(const float *g,sample_t in,unsigned int ci);
  public:
   void init(const TeqSettings *cfg,double freq);
   void reset(void)
    {
     memset(wq,0,sizeof(wq));
    }
   template<class sample_t> void process(sample_t *samples,size_t numsamples,unsigned int nchannels);
  } eq;
 TeqSettings old;
 int oldsf;
protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_ALL_24;}
public:
 TaudioFilterEQ(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void onSeek(void);
};

#endif
