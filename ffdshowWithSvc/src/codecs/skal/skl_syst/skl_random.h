/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_random.h
 *
 * Rand utilities
 ***********************************************/

#ifndef _SKL_RANDOM_H_
#define _SKL_RANDOM_H_

//////////////////////////////////////////////////////////
// README ////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//
// 'Real' uniform random number generator.
//
// Adapted from the Mersenne Twister which is:
//
//    Coded by Takuji Nishimura, considering the suggestions by
//  Topher Cooper and Marc Rieffel in July-Aug. 1997.
//
// and
//
//  Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.
//  Any feedback is very welcome. For any question, comments,
//  see http://www.math.keio.ac.jp/matumoto/emt.html or email
//  matumoto@math.keio.ac.jp
//
// Note: Seed must *not* be zero
//////////////////////////////////////////////////////////

class SKL_RANDOM {

  private:

    enum { N=624, M=397 };
    SKL_UINT32 _V[N];
    int _n;
    void Freshen();
    int Is_Used_Up() const { return (_n<=0); }

  public:

    SKL_RANDOM(SKL_UINT32 seed=12554)         { Set_Seed(seed); }
    void Set_Seed(SKL_UINT32 s);
    SKL_UINT32 Get();
    float Get_Float(double Max=1.0)    { return (float)( (Max*2.3283064365386963e-10) * (double)Get() ); }
    float Get_SFloat(double Max=1.0)   { return (float)( (Max*4.6566128730773926e-10) * (double)Get() - Max); }
    SKL_UINT32 Get_Int(SKL_UINT32 Max) { return (Get()%Max); }
    SKL_INT32 Get_SInt(SKL_INT32 Min, SKL_INT32 Max) { return ((SKL_INT32)(Get()%(Max-Min+1)) + Min); }
    float Gauss(float Amp=1.0f);
};

//////////////////////////////////////////////////////////

#endif      /* _SKL_RANDOM_H_ */
