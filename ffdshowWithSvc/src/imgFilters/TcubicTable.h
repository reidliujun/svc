#ifndef _TCUBICTABLE_H_
#define _TCUBICTABLE_H_

#include "simd.h"
#include "ffImgfmt.h"

#pragma warning(disable:4799)

struct TcubicTable
{
private:
 int table[1024];
public:
 TcubicTable(double A=-0.6);
 unsigned char interpolate(const unsigned char *src, stride_t stride, int x, int y)
  {
   __m64 mm0=_mm_cvtsi32_si64(*(int*)src);
   __m64 mm7=_mm_setzero_si64();
   __m64 mm1=_mm_cvtsi32_si64(*(int*)(src+stride));
   mm0=_mm_unpacklo_pi8(mm0,mm7);
   __m64 mm2=_mm_cvtsi32_si64(*(int*)(src+stride*2));
   mm1=_mm_unpacklo_pi8(mm1,mm7);
   __m64 mm3=_mm_cvtsi32_si64(*(int*)(src+stride*3));
   mm2=_mm_unpacklo_pi8(mm2,mm7);
   mm3=_mm_unpacklo_pi8(mm3,mm7);

   __m64 mm5=*(__m64*)(table+x*4);

   mm0=_mm_madd_pi16(mm0,mm5);
   mm1=_mm_madd_pi16(mm1,mm5);
   mm2=_mm_madd_pi16(mm2,mm5);
   mm3=_mm_madd_pi16(mm3,mm5);

   __m64 mm4=_mm_setzero_si64(),mm6=_mm_setzero_si64();
   mm4=_mm_unpacklo_pi32(mm4,mm0);
   mm5=_mm_unpacklo_pi32(mm5,mm1);
   mm6=_mm_unpacklo_pi32(mm6,mm2);
   mm7=_mm_unpacklo_pi32(mm7,mm3);

   mm0=_mm_add_pi32(mm0,mm4);
   mm1=_mm_add_pi32(mm1,mm5);
   mm2=_mm_add_pi32(mm2,mm6);
   mm3=_mm_add_pi32(mm3,mm7);

   mm0=_mm_srli_si64(mm0,32);
   mm1=_mm_srli_si64(mm1,32);
   mm2=_mm_srli_si64(mm2,32);
   mm3=_mm_srli_si64(mm3,32);

   mm4=_mm_set1_pi32(0x00000080);

   mm0=_mm_unpacklo_pi32(mm0,mm2);
   mm1=_mm_unpacklo_pi32(mm1,mm3);

   mm0=_mm_add_pi32(mm0,mm4);
   mm1=_mm_add_pi32(mm1,mm4);

   mm4=_mm_set1_pi32(0x0000ffff);

   mm0=_mm_srli_si64(mm0,8);
   mm1=_mm_slli_si64(mm1,8);

   mm0=_mm_and_si64(mm0,mm4);
   mm4=_mm_andnot_si64(mm4,mm1);

   mm0=_mm_or_si64(mm0,mm4);

   mm0=_mm_madd_pi16(mm0,*(__m64*)(table+y*4));
   mm4=_mm_unpacklo_pi32(mm4,mm0);
   mm0=_mm_add_pi32(mm0,mm4);

   mm4=_mm_setzero_si64();
   mm0=_mm_add_pi32(mm0,_mm_set_pi8(0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00));
   mm0=_mm_srai_pi32(mm0,4);

   mm4=_mm_cmpgt_pi16(mm4,mm0);
   mm4=_mm_andnot_si64(mm4,mm0);
   mm4=_mm_packs_pu16(mm4,mm4);
   mm4=_mm_srli_si64(mm4,56);
   return (unsigned char)_mm_cvtsi64_si32(mm4);
  }
};

#endif
