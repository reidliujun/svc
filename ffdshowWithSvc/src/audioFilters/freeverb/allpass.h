// Allpass filter
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#ifndef _allpass_
#define _allpass_

#include "denormals.h"

class allpass
{
public:
 allpass()
  {
   bufidx = 0;
  }
 void setbuffer(float *buf, int size)
  {
   buffer = buf;
   bufsize = size;
  }
 __forceinline  float process(float input)
  {
   float output;
   float bufout;

   bufout = buffer[bufidx];
   undenormalise(bufout);

   output = -input + bufout;
   buffer[bufidx] = input + (bufout*feedback);

   if(++bufidx>=bufsize) bufidx = 0;

   return output;
  }
 void mute()
  {
   for (int i=0; i<bufsize; i++)
    buffer[i]=0;
  }
 void setfeedback(float val)
  {
   feedback = val;
  }
 float getfeedback()
  {
   return feedback;
  }
private:
 float   feedback;
 float   *buffer;
 int     bufsize;
 int     bufidx;
};

#endif
