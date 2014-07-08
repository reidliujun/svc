#ifndef _2XSAI_H_
#define _2XSAI_H_

#include "simd.h"
#include "ffImgfmt.h"

class T2xSaI
{
public:
 typedef uint8_t uint8;
 typedef uint32_t uint32;
 typedef uint16_t uint16;

private:
 template<uint32_t d,uint32_t r,uint32_t g,uint32_t b> struct makecol_depth {static const uint32_t value=r+(g<<8)+(b<<16);};
 static const int d=32;
 static const int minr=1,ming=256,minb=65536;
 static const uint32_t colorMask=(makecol_depth<d, 255, 0, 0>::value - minr) | (makecol_depth<d, 0, 255, 0>::value - ming) | (makecol_depth<d, 0, 0, 255>::value - minb);
 static const uint32_t lowPixelMask=minr | ming | minb;
 static const uint32_t qcolorMask=(makecol_depth<d, 255, 0, 0>::value - 3 * minr) | (makecol_depth<d, 0, 255, 0>::value - 3 * ming) | (makecol_depth<d, 0, 0, 255>::value - 3 * minb);
 static const uint32_t qlowpixelMask=(minr * 3) | (ming * 3) | (minb * 3);

 static int GET_RESULT(unsigned long A,unsigned long B,unsigned long C,unsigned long D) {return (A != C || A != D) - (B != C || B != D);}
 static unsigned long INTERPOLATE(unsigned long A,unsigned long B) {return ((A & colorMask) >> 1) + ((B & colorMask) >> 1) + (A & B & lowPixelMask);}
 static unsigned long Q_INTERPOLATE(unsigned long A,unsigned long B,unsigned long C,unsigned long D)
  {
   return ((A & qcolorMask) >> 2) + ((B & qcolorMask) >> 2) + ((C & qcolorMask) >> 2) + ((D & qcolorMask) >> 2) + ((((A & qlowpixelMask) + (B & qlowpixelMask) + (C & qlowpixelMask) + (D & qlowpixelMask)) >> 2) & qlowpixelMask);
  }

 static inline uint32 Q_INTERPOLATE16(uint32 A, uint32 B, uint32 C, uint32 D)
  {
   static const uint32_t qcolorMask = 0xE79CE79C;
   static const uint32_t qlowpixelMask = 0x18631863;

   register uint32 x = ((A & qcolorMask) >> 2) +
                       ((B & qcolorMask) >> 2) +
                       ((C & qcolorMask) >> 2) +
                       ((D & qcolorMask) >> 2);
   register uint32 y = (A & qlowpixelMask) +
                       (B & qlowpixelMask) +
                       (C & qlowpixelMask) +
                       (D & qlowpixelMask);
   y = (y>>2) & qlowpixelMask;
   return x+y;
  }
 static unsigned long INTERPOLATE16(unsigned long A,unsigned long B)
  {
   static const uint32_t colorMask = 0xF7DEF7DE;
   static const uint32_t lowPixelMask = 0x08210821;
   return ((A & colorMask) >> 1) + ((B & colorMask) >> 1) + (A & B & lowPixelMask);
  }
 static int GetResult1(uint32 A, uint32 B, uint32 C, uint32 D, uint32 E);
 static int GetResult2(uint32 A, uint32 B, uint32 C, uint32 D, uint32 E);
public:
 static void _2xSaI(const uint8 *srcPtr, stride_t srcPitch, uint8 *dstBitmap, int width, int height,stride_t dstPitch);
 static void super(const uint8_t *src, stride_t src_pitch, uint8_t *dst, stride_t dst_pitch, uint32_t width, uint32_t height);
};

class Thq2x
{
private:
 static void init(void);
 static int LUT16to32[65536];
 static __align8(int,RGBtoYUV[65536]);
 static __forceinline int Diff(unsigned int w5, unsigned int w1);
 static const __int64 reg_blank,const3,const5,const6,const14;
 static __forceinline void Interp1(unsigned char * pc, int c1, int c2);
 static __forceinline void Interp2(unsigned char * pc, int c1, int c2, int c3);
 static __forceinline void Interp5(unsigned char * pc, int c1, int c2);
 static __forceinline void Interp6(unsigned char * pc, int c1, int c2, int c3);
 static __forceinline void Interp7(unsigned char * pc, int c1, int c2, int c3);
 static __forceinline void Interp9(unsigned char * pc, int c1, int c2, int c3);
 static __forceinline void Interp10(unsigned char * pc, int c1, int c2, int c3);
public:
 static void hq2x_32( const unsigned char * pIn, unsigned char * pOut, int Xres, int Yres, stride_t srcBpL, stride_t dstBpL );
};

#endif
