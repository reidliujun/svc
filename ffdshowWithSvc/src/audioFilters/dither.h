#ifndef _DITHER_H_
#define _DITHER_H_

#include "TsampleFormat.h"

class Tdither
{
private:
 struct Trandom
  {
  private:
   static unsigned int __r1,__r2;
   static const unsigned char Parity [256];
   static unsigned int random_int(void);
  public:
   static double Random_Equi(double mult);
   static double Random_Triangular(double mult);
  };

 int shapingtype;
 const float*  FilterCoeff;
 uint64_t Mask;
 double Add;
 float Dither;
 float ErrorHistory[6][16]; // max. 2 channels, 16th order Noise shaping
 float DitherHistory[6][16];
 int LastRandomNumber[6];
 static const float F44_0[16 + 32], F44_1[16 + 32],F44_2[16 + 32],F44_3[16 + 32],F44_4[16 + 32];
 static double scalar16 ( const float* x, const float* y );
 int64_t ROUND64(double x);
 int64_t dither_output(int shapingtype, long i, double Sum, int k);
 template<class Tout,int conv> Tout* ditherSamples(const float *bufIn,Tout * const bufOut,unsigned int nchannels,size_t numsamples,int dithering);
public:
 Tdither(int bits,int Ishapingtype);
 int16_t* process(const float *bufIn,int16_t * const bufOut,unsigned int nchannels,size_t numsamples,int dithering);
 int24_t* process(const float *bufIn,int24_t * const bufOut,unsigned int nchannels,size_t numsamples,int dithering);
 int32_t* process(const float *bufIn,int32_t * const bufOut,unsigned int nchannels,size_t numsamples,int dithering);
};

#endif
