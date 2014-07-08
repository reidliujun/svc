/*=============================================================================
//
//  This software has been released under the terms of the GNU Public
//  license. See http://www.gnu.org/copyleft/gpl.html for details.
//
//  Copyright 2001 Anders Johansson ajh@atri.curtin.edu.au
//
//=============================================================================
*/

/* Design and implementation of different types of digital filters

*/
#include "stdafx.h"
#include "firfilter.h"
#include "TfirSettings.h"
#include "simd.h"

#if 0
/* Add new data to circular queue designed to be used with a FIR
   filter. xq is the circular queue, in pointing at the new sample, xi
   current index for xq and n the length of the filter. xq must be n*2
   long.
*/
#define updateq(n,xi,xq,in)\
  xq[xi]=(xq)[(xi)+(n)]=*(in);\
  xi=(++(xi))&((n)-1);
#endif


/******************************************************************************
*  FIR filter implementations
******************************************************************************/

/* C implementation of FIR filter y=w*x

   n number of filter taps, where mod(n,4)==0
   w filter taps
   x input signal must be a circular buffer which is indexed backwards
*/
/*
TfirFilter::_ftype_t TfirFilter::fir(register unsigned int n, _ftype_t* w, _ftype_t* x)
{
  register _ftype_t y; // Output
  y = 0.0;
  do{
    n--;
    y+=w[n]*x[n];
  }while(n != 0);
  return y;
}
*/
/*
// Boxcar
//
// n window length
// w buffer for the window parameters
*/
void TfirFilter::boxcar(int n, _ftype_t* w)
{
  int i;
  // Calculate window coefficients
  for (i=0 ; i<n ; i++)
    w[i] = 1.0;
}


/*
// Triang a.k.a Bartlett
//
//               |    (N-1)|
//           2 * |k - -----|
//               |      2  |
// w = 1.0 - ---------------
//                    N+1
// n window length
// w buffer for the window parameters
*/
void TfirFilter::triang(int n, _ftype_t* w)
{
  _ftype_t k1  = (_ftype_t)(n & 1);
  _ftype_t k2  = 1/((_ftype_t)n + k1);
  int      end = (n + 1) >> 1;
  int      i;

  // Calculate window coefficients
  for (i=0 ; i<end ; i++)
    w[i] = w[n-i-1] = _ftype_t((2.0*((_ftype_t)(i+1))-(1.0-k1))*k2);
}


/*
// Hanning
//                   2*pi*k
// w = 0.5 - 0.5*cos(------), where 0 < k <= N
//                    N+1
// n window length
// w buffer for the window parameters
*/
void TfirFilter::hanning(int n, _ftype_t* w)
{
  int      i;
  _ftype_t k = _ftype_t(2*M_PI/((_ftype_t)(n+1))); // 2*pi/(N+1)

  // Calculate window coefficients
  for (i=0; i<n; i++)
    *w++ = _ftype_t(0.5*(1.0 - cos(k*(_ftype_t)(i+1))));
}

/*
// Hamming
//                        2*pi*k
// w(k) = 0.54 - 0.46*cos(------), where 0 <= k < N
//                         N-1
//
// n window length
// w buffer for the window parameters
*/
void TfirFilter::hamming(int n,_ftype_t* w)
{
  int      i;
  _ftype_t k = _ftype_t(2*M_PI/((_ftype_t)(n-1))); // 2*pi/(N-1)

  // Calculate window coefficients
  for (i=0; i<n; i++)
    *w++ = _ftype_t(0.54 - 0.46*cos(k*(_ftype_t)i));
}

/*
// Blackman
//                       2*pi*k             4*pi*k
// w(k) = 0.42 - 0.5*cos(------) + 0.08*cos(------), where 0 <= k < N
//                        N-1                 N-1
//
// n window length
// w buffer for the window parameters
*/
void TfirFilter::blackman(int n,_ftype_t* w)
{
  int      i;
  _ftype_t k1 = _ftype_t(2*M_PI/((_ftype_t)(n-1))); // 2*pi/(N-1)
  _ftype_t k2 = 2*k1; // 4*pi/(N-1)

  // Calculate window coefficients
  for (i=0; i<n; i++)
    *w++ = _ftype_t(0.42 - 0.50*cos(k1*(_ftype_t)i) + 0.08*cos(k2*(_ftype_t)i));
}

/*
// Flattop
//                                        2*pi*k                     4*pi*k
// w(k) = 0.2810638602 - 0.5208971735*cos(------) + 0.1980389663*cos(------), where 0 <= k < N
//                                          N-1                        N-1
//
// n window length
// w buffer for the window parameters
*/
void TfirFilter::flattop(int n,_ftype_t* w)
{
  int      i;
  _ftype_t k1 = _ftype_t(2*M_PI/((_ftype_t)(n-1))); // 2*pi/(N-1)
  _ftype_t k2 = 2*k1;                   // 4*pi/(N-1)

  // Calculate window coefficients
  for (i=0; i<n; i++)
    *w++ = _ftype_t(0.2810638602 - 0.5208971735*cos(k1*(_ftype_t)i) + 0.1980389663*cos(k2*(_ftype_t)i));
}

/* Computes the 0th order modified Bessel function of the first kind.
// (Needed to compute Kaiser window)
//
// y = sum( (x/(2*n))^2 )
//      n
*/

TfirFilter::_ftype_t TfirFilter::besselizero(_ftype_t x)
{
  static const _ftype_t BIZ_EPSILON=_ftype_t(1E-21); // Max error acceptable
  _ftype_t temp;
  _ftype_t sum   = 1.0f;
  _ftype_t u     = 1.0f;
  _ftype_t halfx = x/2.0f;
  int      n     = 1;

  do {
    temp = halfx/(_ftype_t)n;
    u *=temp * temp;
    sum += u;
    n++;
  } while (u >= BIZ_EPSILON * sum);
  return(sum);
}

/*
// Kaiser
//
// n window length
// w buffer for the window parameters
// b beta parameter of Kaiser window, Beta >= 1
//
// Beta trades the rejection of the low pass filter against the
// transition width from passband to stop band.  Larger Beta means a
// slower transition and greater stop band rejection.  See Rabiner and
// Gold (Theory and Application of DSP) under Kaiser windows for more
// about Beta.  The following table from Rabiner and Gold gives some
// feel for the effect of Beta:
//
// All ripples in dB, width of transition band = D*N where N = window
// length
//
// BETA    D       PB RIP   SB RIP
// 2.120   1.50  +-0.27      -30
// 3.384   2.23    0.0864    -40
// 4.538   2.93    0.0274    -50
// 5.658   3.62    0.00868   -60
// 6.764   4.32    0.00275   -70
// 7.865   5.0     0.000868  -80
// 8.960   5.7     0.000275  -90
// 10.056  6.4     0.000087  -100
*/
void TfirFilter::kaiser(int n, _ftype_t* w, _ftype_t b)
{
  _ftype_t tmp;
  _ftype_t k1  = 1.0f/besselizero(b);
  int      k2  = 1 - (n & 1);
  int      end = (n + 1) >> 1;
  int      i;

  // Calculate window coefficients
  for (i=0 ; i<end ; i++){
    tmp = (_ftype_t)(2*i + k2) / ((_ftype_t)n - 1.0f);
    w[end-(1&(!k2))+i] = w[end-1-i] = k1 * besselizero(_ftype_t(b*sqrt(1.0 - tmp*tmp)));
  }
}


/******************************************************************************
*  FIR filter design
******************************************************************************/

/* Design FIR filter using the Window method

   n     filter length must be odd for HP and BS filters
   w     buffer for the filter taps (must be n long)
   fc    cutoff frequencies (1 for LP and HP, 2 for BP and BS)
         0 < fc < 1 where 1 <=> Fs/2
   flags window and filter type as defined in filter.h
         variables are ored together: i.e. LP|HAMMING will give a
         low pass filter designed using a hamming window
   opt   beta constant used only when designing using kaiser windows

   returns 0 if OK, -1 if fail
*/
TfirFilter::_ftype_t* TfirFilter::design_fir(unsigned int *n, _ftype_t* fc, int type, int window, _ftype_t opt)
{
  unsigned int  o   = *n & 1;            // Indicator for odd filter length
  unsigned int  end = ((*n + 1) >> 1) - o;       // Loop end
  unsigned int  i;                      // Loop index

  _ftype_t k1 = 2 * _ftype_t(M_PI);             // 2*pi*fc1
  _ftype_t k2 = 0.5f * (_ftype_t)(1 - o);// Constant used if the filter has even length
  _ftype_t k3;                           // 2*pi*fc2 Constant used in BP and BS design
  _ftype_t g  = 0.0f;                    // Gain
  _ftype_t t1,t2,t3;                     // Temporary variables
  _ftype_t fc1,fc2;                      // Cutoff frequencies

  // Sanity check
  if(*n==0) return NULL;
  fc[0]=limit(fc[0],_ftype_t(0.001),_ftype_t(1));

  if (!o && (type==TfirSettings::BANDSTOP || type==TfirSettings::HIGHPASS))
   (*n)++;
  _ftype_t *w=(_ftype_t*)aligned_calloc(sizeof(_ftype_t),*n);

  // Get window coefficients
  switch(window){
  case(TfirSettings::WINDOW_BOX):
    boxcar(*n,w); break;
  case(TfirSettings::WINDOW_TRIANGLE):
    triang(*n,w); break;
  case(TfirSettings::WINDOW_HAMMING):
    hamming(*n,w); break;
  case(TfirSettings::WINDOW_HANNING):
    hanning(*n,w); break;
  case(TfirSettings::WINDOW_BLACKMAN):
    blackman(*n,w); break;
  case(TfirSettings::WINDOW_FLATTOP):
    flattop(*n,w); break;
  case(TfirSettings::WINDOW_KAISER):
    kaiser(*n,w,opt); break;
  default:
   {
    delete []w;
    return NULL;
   }
  }

  if(type==TfirSettings::LOWPASS || type==TfirSettings::HIGHPASS){
    fc1=*fc;
    // Cutoff frequency must be < 0.5 where 0.5 <=> Fs/2
    fc1 = ((fc1 <= 1.0) && (fc1 > 0.0)) ? fc1/2 : 0.25f;
    k1 *= fc1;

    if(type==TfirSettings::LOWPASS){ // Low pass filter

      // If the filter length is odd, there is one point which is exactly
      // in the middle. The value at this point is 2*fCutoff*sin(x)/x,
      // where x is zero. To make sure nothing strange happens, we set this
      // value separately.
      if (o){
        w[end] = fc1 * w[end] * 2.0f;
        g=w[end];
      }

      // Create filter
      for (i=0 ; i<end ; i++){
        t1 = (_ftype_t)(i+1) - k2;
        w[end-i-1] = w[*n-end+i] = _ftype_t(w[end-i-1] * sin(k1 * t1)/(M_PI * t1)); // Sinc
        g += 2*w[end-i-1]; // Total gain in filter
      }
    }
    else{ // High pass filter
      //if (!o) // High pass filters must have odd length
      // return -1;
      w[end] = _ftype_t(1.0 - (fc1 * w[end] * 2.0));
      g= w[end];

      // Create filter
      for (i=0 ; i<end ; i++){
        t1 = (_ftype_t)(i+1);
        w[end-i-1] = w[*n-end+i] = _ftype_t(-1 * w[end-i-1] * sin(k1 * t1)/(M_PI * t1)); // Sinc
        g += ((i&1) ? (2*w[end-i-1]) : (-2*w[end-i-1])); // Total gain in filter
      }
    }
  }

  if(type==TfirSettings::BANDPASS || type==TfirSettings::BANDSTOP){
    fc1=fc[0];
    fc2=limit(fc[1],_ftype_t(0.001),_ftype_t(1));
    // Cutoff frequencies must be < 1.0 where 1.0 <=> Fs/2
    fc1 = ((fc1 <= 1.0) && (fc1 > 0.0)) ? fc1/2 : 0.25f;
    fc2 = ((fc2 <= 1.0) && (fc2 > 0.0)) ? fc2/2 : 0.25f;
    k3  = k1 * fc2; // 2*pi*fc2
    k1 *= fc1;      // 2*pi*fc1

    if(type==TfirSettings::BANDPASS){ // Band pass
      // Calculate center tap
      if (o){
        g=w[end]*(fc1+fc2);
        w[end] = (fc2 - fc1) * w[end] * 2.0f;
      }

      // Create filter
      for (i=0 ; i<end ; i++){
        t1 = (_ftype_t)(i+1) - k2;
        t2 = _ftype_t(sin(k3 * t1)/(M_PI * t1)); // Sinc fc2
        t3 = _ftype_t(sin(k1 * t1)/(M_PI * t1)); // Sinc fc1
        g += w[end-i-1] * (t3 + t2);   // Total gain in filter
        w[end-i-1] = w[*n-end+i] = w[end-i-1] * (t2 - t3);
      }
    }
    else{ // Band stop
      //if (!o) // Band stop filters must have odd length
      //  return -1;
      w[end] = _ftype_t(1.0 - (fc2 - fc1) * w[end] * 2.0);
      g= w[end];

      // Create filter
      for (i=0 ; i<end ; i++){
        t1 = (_ftype_t)(i+1);
        t2 = _ftype_t(sin(k1 * t1)/(M_PI * t1)); // Sinc fc1
        t3 = _ftype_t(sin(k3 * t1)/(M_PI * t1)); // Sinc fc2
        w[end-i-1] = w[*n-end+i] = w[end-i-1] * (t2 - t3);
        g += 2*w[end-i-1]; // Total gain in filter
      }
    }
  }

  // Normalize gain
  g=1/g;
  for (i=0; i<*n; i++)
    w[i] *= g;

  return w;
}

float TfirFilter::vec_inner_prod_sse(const float *eax, const float *edi, int ecx)
{
  __m128 xmm3,xmm4,xmm0,xmm1,xmm5,xmm6;
  xorps (xmm3, xmm3);
  xorps (xmm4, xmm4);

  ecx-=8;// sub $8, %%ecx
  if (ecx<0) goto //jb
   mul8_skip;

mul8_loop:
  movups (xmm0,eax);
  movups (xmm1,edi);
  movups (xmm5,16/sizeof(float)+eax);
  movups (xmm6,16/sizeof(float)+edi);
  eax+=32/sizeof(float);
  edi+=32/sizeof(float);
  mulps (xmm1,xmm0);
  mulps (xmm6,xmm5);
  addps (xmm3,xmm1);
  addps (xmm4,xmm6);

  ecx-=8;
  if (ecx>=0) //jae
   goto mul8_loop;

mul8_skip:

  addps (xmm3,xmm4);

  ecx+=4;
  if (ecx<0) //jl
   goto mul4_skip;

  movups (xmm0,eax);
  movups (xmm1,edi);
  eax+=16/sizeof(float);
  edi+=16/sizeof(float);
  mulps (xmm1, xmm0);
  addps (xmm3, xmm1);

  ecx-=4;

mul4_skip:

  ecx+=4;

  goto cond1;

mul1_loop:
  movss (xmm0,eax);
  movss (xmm1,edi);
  eax+=4/sizeof(float);
  edi+=4/sizeof(float);
  mulss (xmm1,xmm0);
  addss (xmm3,xmm1);

cond1:
  ecx-=1;
  if (ecx>=0) // jae
   goto mul1_loop;

  movhlps  (xmm4,xmm3);
  addps    (xmm3,xmm4);
  movaps   (xmm4,xmm3);
  //FIXME: which one?
  xmm4=_mm_shuffle_ps(xmm4,xmm4,0x55);// shufps $0x55, xmm4, xmm4
                                      // shufps $33, xmm4, xmm4
  addss    (xmm3, xmm4);
  float sum;
  movss    (&sum , xmm3);
  return sum;
}
