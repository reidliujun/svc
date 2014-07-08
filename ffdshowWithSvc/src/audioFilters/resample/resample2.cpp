/*
 * audio resampling
 * Copyright (c) 2004 Michael Niedermayer <michaelni@gmx.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/**
 * @file resample2.c
 * audio resampling
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#include "stdafx.h"
#include "resample2.h"

#define FFMIN(a,b) std::min(a,b)
#define FFMAX(a,b) std::max(a,b)
#define ABS ff_abs
/*
#if 0
#define FILTER_SHIFT 15

#define FELEM int16_t
#define FELEM2 int32_t
#define FELEM_MAX INT16_MAX
#define FELEM_MIN INT16_MIN
#else
#define FILTER_SHIFT 22

#define FELEM int32_t
#define FELEM2 int64_t
#define FELEM_MAX INT32_MAX
#define FELEM_MIN INT32_MIN
#endif
*/

template<class sample_t> struct AVResampleContextBase
{
protected:
 int filter_length;
 int ideal_dst_incr;
 int dst_incr;
 int index;
 int frac;
 int src_incr;
 int compensation_distance;
 int phase_shift;
 int phase_mask;
 int linear;
 void *filter_bank0;
public:
 AVResampleContextBase(void)
  {
   filter_length=
   ideal_dst_incr=
   dst_incr=
   index=
   frac=
   src_incr=
   compensation_distance=
   phase_shift=
   phase_mask=
   linear=0;
   filter_bank0=NULL;
  }
 virtual int av_resample(sample_t *dst, sample_t *src, int *consumed, int src_size, int dst_size, int update_ctx)=0;
 ~AVResampleContextBase()
  {
   aligned_free(filter_bank0);
  }
};

template<class sample_t,class FELEM,class FELEM2,class Tminmax,Tminmax FELEM_MAX,Tminmax FELEM_MIN,int FILTER_SHIFT>
struct TAVResampleContext :public AVResampleContextBase<sample_t>
{
private:
 // 0th order modified bessel function of the first kind.
 static double bessel(double x){
     double v=1;
     double t=1;
     int i;

     for(i=1; i<50; i++){
         t *= i;
         v += pow(x*x/4, i)/(t*t);
     }
     return v;
 }
 static Tminmax clip(int a, Tminmax amin, Tminmax amax)
 {
     if (a < amin)
         return amin;
     else if (a > amax)
         return amax;
     else
         return a;
 }

 #ifndef __GNUC__
  #ifdef WIN64
   static inline int lrintf(float flt)
   {
    return (int)(flt + (flt < 0 ? -0.5f : 0.5f)); //TODO: lrintf
   }
  #else
   static inline int lrintf(float f)
   {
    int i;
    __asm
     {
      fld   f
      fistp i
     }
    return i;
   }
  #endif
 #endif

 template<class T> static inline void storeSample(T &dst,FELEM2 val)
  {
   val = (val + (1<<(FILTER_SHIFT-1)))>>FILTER_SHIFT;
   dst = short((unsigned)(val + 32768) > 65535 ? (val>>31) ^ 32767 : val);
  }
 static inline void storeSample(float &dst,FELEM2 val)
  {
   dst=limit(val,-1.0f,1.0f);
  }
 template<class T> static inline void storeFilter(T &dst,double val)
  {
   dst = T(clip(lrintf(float(val)), FELEM_MIN, FELEM_MAX));
  }
 static inline void storeFilter(float &dst,double val)
  {
   dst = (float)limit(val,-1.0,1.0);
  }

 FELEM *filter_bank;
 static void av_build_filter(FELEM *filter, double factor, int tap_count, int phase_count, int scale, int type){
     int ph, i;
     double x, y, w, *tab=(double*)_alloca(sizeof(double)*tap_count);
     const int center= (tap_count-1)/2;

     /* if upsampling, only need to interpolate, no filter */
     if (factor > 1.0)
         factor = 1.0;

     for(ph=0;ph<phase_count;ph++) {
         double norm = 0;
         double e= 0;
         for(i=0;i<tap_count;i++) {
             x = M_PI * ((double)(i - center) - (double)ph / phase_count) * factor;
             if (x == 0) y = 1.0;
             else        y = sin(x) / x;
             switch(type){
             case 0:{
                 const float d= -0.5; //first order derivative = -0.5
                 x = fabs(((double)(i - center) - (double)ph / phase_count) * factor);
                 if(x<1.0) y= 1 - 3*x*x + 2*x*x*x + d*(            -x*x + x*x*x);
                 else      y=                       d*(-4 + 8*x - 5*x*x + x*x*x);
                 break;}
             case 1:
                 w = 2.0*x / (factor*tap_count) + M_PI;
                 y *= 0.3635819 - 0.4891775 * cos(w) + 0.1365995 * cos(2*w) - 0.0106411 * cos(3*w);
                 break;
             case 2:
                 w = 2.0*x / (factor*tap_count*M_PI);
                 y *= bessel(16*sqrt(FFMAX(1-w*w, 0.0)));
                 break;
             }

             tab[i] = y;
             norm += y;
         }

         /* normalize so that an uniform color remains the same */
         for(i=0;i<tap_count;i++) {
             FELEM v;storeFilter(v,tab[i] * scale / norm + e);
             filter[ph * tap_count + i] = v;
             e += tab[i] * scale / norm - v;
         }
     }
 }

public:
 TAVResampleContext(int out_rate, int in_rate, int filter_size, int phase_shift, int linear, double cutoff)
  {
   TAVResampleContext *c=this;

   double factor= FFMIN(out_rate *cutoff / in_rate, 1.0);
   int phase_count= 1<<phase_shift;

   //memset(c, 0, sizeof(TAVResampleContext));

   c->phase_shift= phase_shift;
   c->phase_mask= phase_count-1;
   c->linear= linear;

   c->filter_length= std::max((int)ceil(filter_size/factor),1);
   c->filter_bank0=c->filter_bank= (FELEM*)aligned_calloc(c->filter_length*(phase_count+1),sizeof(FELEM));
   av_build_filter(c->filter_bank, factor, c->filter_length, phase_count, 1<<FILTER_SHIFT, 1);
   memcpy(&c->filter_bank[c->filter_length*phase_count+1], c->filter_bank, (c->filter_length-1)*sizeof(FELEM));
   c->filter_bank[c->filter_length*phase_count]= c->filter_bank[c->filter_length - 1];

   c->src_incr= out_rate;
   c->ideal_dst_incr= c->dst_incr= in_rate * phase_count;
   c->index= -phase_count*((c->filter_length-1)/2);
  }
 int av_resample(sample_t *dst, sample_t *src, int *consumed, int src_size, int dst_size, int update_ctx)
  {
   TAVResampleContext *c=this;
   int dst_index, i;
   int index= c->index;
   int frac= c->frac;
   int dst_incr_frac= c->dst_incr % c->src_incr;
   int dst_incr=      c->dst_incr / c->src_incr;
   int compensation_distance= c->compensation_distance;

  if(compensation_distance == 0 && c->filter_length == 1 && c->phase_shift==0){
        int64_t index2= ((int64_t)index)<<32;
        int64_t incr= (1LL<<32) * c->dst_incr / c->src_incr;
        dst_size= FFMIN(dst_size, int((src_size-1-index) * (int64_t)c->src_incr / c->dst_incr));

        for(dst_index=0; dst_index < dst_size; dst_index++){
            dst[dst_index] = src[index2>>32];
            index2 += incr;
        }
        frac += dst_index * dst_incr_frac;
        index += dst_index * dst_incr;
        index += frac / c->src_incr;
        frac %= c->src_incr;
  }else{
   for(dst_index=0; dst_index < dst_size; dst_index++){
       FELEM *filter= c->filter_bank + c->filter_length*(index & c->phase_mask);
       int sample_index= index >> c->phase_shift;
       FELEM2 val=0;

       if(sample_index < 0){
           for(i=0; i<c->filter_length; i++)
               val += src[ABS(sample_index + i) % src_size] * filter[i];
       }else if(sample_index + c->filter_length > src_size){
           break;
       }else if(c->linear){
           FELEM2 v=0;
           int sub_phase= (frac<<8) / c->src_incr;
           for(i=0; i<c->filter_length; i++){
               FELEM2 coeff= filter[i]*(256 - sub_phase) + filter[i + c->filter_length]*sub_phase;
               v += src[sample_index + i] * coeff;
           }
           val= FELEM2(/*v>>8*/v/256);
       }else{
           for(i=0; i<c->filter_length; i++){
               val += src[sample_index + i] * (FELEM2)filter[i];
           }
       }

       storeSample(dst[dst_index],val);

       frac += dst_incr_frac;
       index += dst_incr;
       if(frac >= c->src_incr){
           frac -= c->src_incr;
           index++;
       }

       if(dst_index + 1 == compensation_distance){
           compensation_distance= 0;
           dst_incr_frac= c->ideal_dst_incr % c->src_incr;
           dst_incr=      c->ideal_dst_incr / c->src_incr;
       }
   }
  }
   *consumed= FFMAX(index, 0) >> c->phase_shift;
   if(index>=0) index &= c->phase_mask;

   if(compensation_distance){
       compensation_distance -= dst_index;
       assert(compensation_distance > 0);
   }
   if(update_ctx){
       c->frac= frac;
       c->index= index;
       c->dst_incr= dst_incr_frac + c->src_incr*dst_incr;
       c->compensation_distance= compensation_distance;
   }
#if 0
   if(update_ctx && !c->compensation_distance){
#undef rand
       av_resample_compensate(c, rand() % (8000*2) - 8000, 8000*2);
av_log(NULL, AV_LOG_DEBUG, "%d %d %d\n", c->dst_incr, c->ideal_dst_incr, c->compensation_distance);
   }
#endif

   return dst_index;
  }
};

template<class sample_t> void TreSampleContext<sample_t>::init(int io_channels,int output_rate, int input_rate)
{
 memset(this,0,sizeof(*this));
 this->ratio = (float)output_rate / (float)input_rate;

 this->io_channels = io_channels;
 this->filter_channels = this->io_channels;
}

template<> TreSampleContext<int16_t>::TreSampleContext(int io_channels,int output_rate, int input_rate, int filter_size, int phase_shift, int linear, double cutoff,int bits)
{
 init(io_channels,output_rate,input_rate);
 switch (bits)
  {
   case 15:resample_context=new TAVResampleContext<int16_t,int16_t,int32_t,int32_t,INT16_MAX,INT16_MIN,15>(output_rate, input_rate, filter_size, phase_shift,linear, cutoff);break;
   case 22:resample_context=new TAVResampleContext<int16_t,int32_t,int64_t,int64_t,INT32_MAX,INT32_MIN,22>(output_rate, input_rate, filter_size, phase_shift,linear, cutoff);break;
  }
}

template<> TreSampleContext<float>::TreSampleContext(int io_channels,int output_rate, int input_rate, int filter_size, int phase_shift, int linear, double cutoff,int bits)
{
 init(io_channels,output_rate,input_rate);
 resample_context=new TAVResampleContext<float,float,float,int,1,-1,0>(output_rate, input_rate, filter_size, phase_shift,linear, cutoff);
}

/* resample audio. 'nb_samples' is the number of input samples */
/* XXX: optimize it ! */
template<class sample_t> int TreSampleContext<sample_t>::audio_resample(sample_t *output, const sample_t *input, size_t nb_samples)
{
    int nb_samples1;
    sample_t *buftmp2, *buftmp3;
    int lenout;

    /* XXX: move those malloc to resample init code */
    size_t bufinsizewanted=(nb_samples + this->temp_len) * sizeof(sample_t);
    bufin.alloc(bufinsizewanted);
    memcpy(bufin, this->temp, this->temp_len * sizeof(sample_t));
    buftmp2 = (sample_t*)bufin + this->temp_len;

    /* make some zoom to avoid round pb */
    lenout= (int)(nb_samples * this->ratio) + 16;

    buftmp3 = output;
    memcpy(buftmp2, input, nb_samples*sizeof(sample_t));

    nb_samples += this->temp_len;

    /* resample each channel */
    nb_samples1 = 0; /* avoid warning */
    int consumed;
    nb_samples1 = resample_context->av_resample( buftmp3, bufin, &consumed, (int)nb_samples, (int)lenout, 1);
    this->temp_len= nb_samples - consumed;
    this->temp= (sample_t*)realloc(this->temp, this->temp_len*sizeof(sample_t));
    memcpy(this->temp, (sample_t*)bufin + consumed, this->temp_len*sizeof(sample_t));

    return nb_samples1;
}

template<class sample_t> TreSampleContext<sample_t>::~TreSampleContext()
{
    delete this->resample_context;
    free(this->temp);
}

#undef FFMIN
#undef FFMAX
#undef ABS

template struct TreSampleContext<int16_t>;
template struct TreSampleContext<float>;
