// Reverb model declaration
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#ifndef _revmodel_
#define _revmodel_

#include "comb.h"
#include "allpass.h"

class revmodel
{
public:
        revmodel();
        void    mute();
        void    processreplace(float *l, float *r, int numsamples, int skip);
        void    setroomsize(float value);
        float   getroomsize();
        void    setdamp(float value);
        float   getdamp();
        void    setwet(float value);
        float   getwet();
        void    setdry(float value);
        float   getdry();
        void    setwidth(float value);
        float   getwidth();
        void    setmode(float value);
        float   getmode();
private:
        void    update();
private:
        float   gain;
        float   roomsize,roomsize1;
        float   damp,damp1;
        float   wet,wet1,wet2;
        float   dry;
        float   width;
        float   mode;

// These values assume 44.1KHz sample rate
// they will probably be OK for 48KHz sample rate
// but would need scaling for 96KHz (or other) sample rates.
// The values were obtained by listening tests.
        enum
         {
          numcombs       =8,
          numallpasses   =4,
          stereospread   =23,
          combtuningL1   =1116,
          combtuningR1   =1116+stereospread,
          combtuningL2   =1188,
          combtuningR2   =1188+stereospread,
          combtuningL3   =1277,
          combtuningR3   =1277+stereospread,
          combtuningL4   =1356,
          combtuningR4   =1356+stereospread,
          combtuningL5   =1422,
          combtuningR5   =1422+stereospread,
          combtuningL6   =1491,
          combtuningR6   =1491+stereospread,
          combtuningL7   =1557,
          combtuningR7   =1557+stereospread,
          combtuningL8   =1617,
          combtuningR8   =1617+stereospread,
          allpasstuningL1=556,
          allpasstuningR1=556+stereospread,
          allpasstuningL2=441,
          allpasstuningR2=441+stereospread,
          allpasstuningL3=341,
          allpasstuningR3=341+stereospread,
          allpasstuningL4=225,
          allpasstuningR4=225+stereospread
         };

        // The following are all declared inline
        // to remove the need for dynamic allocation
        // with its subsequent error-checking messiness

        // Comb filters
        comb    combL[numcombs];
        comb    combR[numcombs];

        // Allpass filters
        allpass allpassL[numallpasses];
        allpass allpassR[numallpasses];

        // Buffers for the combs
        float   bufcombL1[combtuningL1];
        float   bufcombR1[combtuningR1];
        float   bufcombL2[combtuningL2];
        float   bufcombR2[combtuningR2];
        float   bufcombL3[combtuningL3];
        float   bufcombR3[combtuningR3];
        float   bufcombL4[combtuningL4];
        float   bufcombR4[combtuningR4];
        float   bufcombL5[combtuningL5];
        float   bufcombR5[combtuningR5];
        float   bufcombL6[combtuningL6];
        float   bufcombR6[combtuningR6];
        float   bufcombL7[combtuningL7];
        float   bufcombR7[combtuningR7];
        float   bufcombL8[combtuningL8];
        float   bufcombR8[combtuningR8];

        // Buffers for the allpasses
        float   bufallpassL1[allpasstuningL1];
        float   bufallpassR1[allpasstuningR1];
        float   bufallpassL2[allpasstuningL2];
        float   bufallpassR2[allpasstuningR2];
        float   bufallpassL3[allpasstuningL3];
        float   bufallpassR3[allpasstuningR3];
        float   bufallpassL4[allpasstuningL4];
        float   bufallpassR4[allpasstuningR4];
};

#endif//_revmodel_

//ends
