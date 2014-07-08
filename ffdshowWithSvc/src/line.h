#ifndef _LINE_H_
#define _LINE_H_

#include "TimgFilter.h"

template<class Tpixop> static void drawline(int x1,int y1,int x2,int y2,unsigned char color,unsigned char *bits,stride_t stride)
{
 const int dx=x2-x1;
 const int dy=y2-y1;
 const int ax=abs(dx)<<1;
 const int ay=abs(dy)<<1;
 unsigned char *dst=bits+x1+y1*stride,*dstEnd=bits+x2+y2*stride;
 const int sx=(dx>=0)?1:-1;
 const stride_t sy=(dy>=0)?stride:-stride;
 if (ax>ay)
  {
   int d=ay-(ax>>1);
   while (dst!=dstEnd)
    {
     Tpixop::pixop(dst,color);
     if (d>0 || (d==0 && dx>=0))
      {
       dst+=sy;
       d-=ax;
      }
     dst+=sx;
     d+=ay;
    }
  }
 else
  {
   int d=ax-(ay>>1);
   while (dst!=dstEnd)
    {
     Tpixop::pixop(dst,color);
     if (d>0 || (d==0 && dy>=0))
      {
       dst+=sx;
       d-=ay;
      }
     dst+=sy;
     d+=ax;
    }
  }
 Tpixop::pixop(dst,color);
}

struct TputColor
{
 static inline void pixop(unsigned char *dst,unsigned char color)
  {
   *dst=color;
  }
};

template<int i> struct TaddColor
{
 static inline void pixop(unsigned char *dst,unsigned char color)
  {
   *dst+=i;
  }
};

#endif
