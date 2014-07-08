/*=============================================================================
//
//  This software has been released under the terms of the GNU Public
//  license. See http://www.gnu.org/copyleft/gpl.html for details.
//
//  Copyright 2001 Anders Johansson ajh@atri.curtin.edu.au
//
//=============================================================================
*/

#ifndef _FIRFILTER_H_
#define _FIRFILTER_H_

#include "Tconfig.h"

// Design and implementation of different types of digital filters
class TfirFilter
{
public:
 typedef float _ftype_t;
private:
 static _ftype_t besselizero(_ftype_t x);
 static void boxcar(int n, _ftype_t* w);
 static void triang(int n, _ftype_t* w);
 static void hanning(int n, _ftype_t* w);
 static void hamming(int n,_ftype_t* w);
 static void blackman(int n,_ftype_t* w);
 static void flattop(int n,_ftype_t* w);
 static void kaiser(int n, _ftype_t* w,_ftype_t b);

 static float vec_inner_prod_sse(const float *a, const float *b, int len);

 template<class T,class _ftype_t> static _ftype_t dotproduct(int count,const T *buf,const _ftype_t *coefficients)
  {
   _ftype_t sum0=0,sum1=0,sum2=0,sum3=0;
   for (;count>=4;buf+=4,coefficients+=4,count-=4)
    {
     sum0+=buf[0]*coefficients[0];
     sum1+=buf[1]*coefficients[1];
     sum2+=buf[2]*coefficients[2];
     sum3+=buf[3]*coefficients[3];
    }
   while (count--) sum0+= *buf++ * *coefficients++;
   return sum0+sum1+sum2+sum3;
  }
public:
 // Exported functions
 //static _ftype_t fir(unsigned int n, _ftype_t* w, _ftype_t* x);
 static int updateq(unsigned int n, unsigned int xi, _ftype_t* xq, _ftype_t* in);
 static _ftype_t* design_fir(unsigned int *n, _ftype_t* fc, int type,int window, _ftype_t opt);
 template<class T> static T firfilter(const T *buf, int pos, int len, int count, const _ftype_t *coefficients)
  {
   int count1, count2;

   if (pos >= count)
    {
     pos -= count;
     count1 = count; count2 = 0;
    }
   else
    {
     count2 = pos;
     count1 = count - pos;
     pos = len - count1;
    }

   // high part of window
   const T *ptr = &buf[pos];
   #if 0
   _ftype_t result=0.0;
   while (count1--) result += *ptr++ * *coefficients++;
   // wrapped part of window
   while (count2--) result += *buf++ * *coefficients++;
   return T(result);
   #else
   _ftype_t r1=dotproduct(count1,ptr,coefficients);coefficients+=count1;
   _ftype_t r2=dotproduct(count2,buf,coefficients);
   return T(r1+r2);
   #endif
  }
};

extern "C" void vec_inner_prod_3dnow(const float *a, const float *b, int len,float *sum);
template<> inline TfirFilter::_ftype_t TfirFilter::dotproduct<float,float>(int count,const float *buf,const float *coefficients)
{
 if (Tconfig::cpu_flags&FF_CPU_SSE)
  return vec_inner_prod_sse(buf,coefficients,count);
 #ifndef WIN64
 else if (Tconfig::cpu_flags&FF_CPU_3DNOW)
  {
   float sum[2]={0,0};
   vec_inner_prod_3dnow(buf,coefficients,count,sum);
   return sum[0];
  }
 #endif
 else
  {
   float sum0=0,sum1=0,sum2=0,sum3=0;
   for (;count>=4;buf+=4,coefficients+=4,count-=4)
    {
     sum0+=buf[0]*coefficients[0];
     sum1+=buf[1]*coefficients[1];
     sum2+=buf[2]*coefficients[2];
     sum3+=buf[3]*coefficients[3];
    }
   while (count--) sum0+= *buf++ * *coefficients++;
   return sum0+sum1+sum2+sum3;
  }
}

#endif
