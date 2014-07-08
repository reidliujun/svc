#ifndef _TAUDIOFILTERCONVOLVER_H_
#define _TAUDIOFILTERCONVOLVER_H_

#include "TaudioFilter.h"
#include "TconvolverSettings.h"

struct TconvolverSettings;
class TaudioFilterResampleLavc;
DECLARE_FILTER(TaudioFilterConvolver,public,TaudioFilter)
private:
 TconvolverSettings oldcfg;
 Tbuffer buf;

 template<class T> struct complexT
  {
  private:
   T v[2];
  public:
   complexT(void) {}
   complexT(T r,T i=0) {v[0]=r;v[1]=i;}
   T& real() {return v[0];}
   const T& real() const {return v[0];}
   T& imag() {return v[1];}
   const T& imag() const {return v[1];}
  };
 typedef complexT<float> complex;
 static void inline complex_mul(const complex *in1, const complex *in2, complex *result, unsigned int count)
  {
   for (unsigned int index=0;index<count;++index,in1++,in2++,result++)
    {
     result->real() += in1->real() * in2->real() - in1->imag() * in2->imag();
     result->imag() += in1->real() * in2->imag() + in1->imag() * in2->real();
    }
  }

 class Tconvolver
  {
  private:
   unsigned int number_of_responses;
   unsigned int chunk_length;
   float normalization_factor;
   std::vector<complex> fft_complex;
   std::vector<float> fft_real;
   template<class _Tin,class _Tout> struct Tfft
    {
    protected:
     typedef _Tin Tin;
     typedef _Tout Tout;
     int length;
     ints ip;
     std::vector<float> w;
    public:
     Tfft(int Ilength):
      length(Ilength),
      ip(int(2+sqrt((double)Ilength))),
      w(Ilength/2)
     {
      ip[0]=0;
     }
    };
   struct Tfftforward : Tfft<float,complex>
    {
     Tfftforward(int Ilength):Tfft<float,complex>(Ilength) {}
     void execute(const Tin *in,Tout *out);
    } fft_plan_forward;
   struct Tfftbackward : Tfft<complex,float>
    {
     Tfftbackward(int Ilength):Tfft<complex,float>(Ilength) {}
     void execute(const Tin *in,Tout *out);
    } fft_plan_backward;
   struct fft_response_t
    {
     int channels;
     size_t length;
     std::vector< std::vector<complex> > channel_data; // individual response channels.
     unsigned int number_of_chunks;
     void init(const TwavReader<float> &response,const TconvolverSettings *cfg,unsigned int chunk_length,float normalization_factor,Tfftforward &fft_plan_forward,unsigned int procchannel=INT32_MAX);
    };
   std::vector<fft_response_t> fft_responses;

   std::vector< std::vector<complex> > input_chunk_ringbuffers; // array of ringbuffers for the fft'ed input
   ints input_chunk_ringbuffer_indexes; // array of indexes into the input_chunk_ringbuffers
   std::vector< std::vector<float> > overlap_buffers; // array of overlap buffers

   unsigned int in_channels,in_channel[8];
   unsigned int out_channels,out_channel[8];
   static inline float mix(float a,float b,float strength,float invstrength)
    {
     return a*invstrength+b*strength;
    }
  public:
   Tconvolver(const TsampleFormat &infmt,const TwavReader<float> &response,const TconvolverSettings *cfg,unsigned int procchannel=INT32_MAX);
   unsigned int number_of_response_channels;
   int process(const float * const in_data,float *out_data,size_t numsamples,const TconvolverSettings *cfg);
  };

 typedef std::vector<Tconvolver> Tconvolvers;
 static void resampleImpulse(TwavReader<float> &impulse,int dstfreq);
 Tconvolvers convolvers;
 unsigned int outchannels;
 TbyteBuffer buffer;
protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_FLOAT32;}
 virtual bool is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
public:
 TaudioFilterConvolver(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual bool getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void done(void);
 virtual void onSeek(void);
};

#endif
