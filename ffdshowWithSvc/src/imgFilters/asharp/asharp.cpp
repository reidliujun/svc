//
//      asharp (version 0.95) - adaptive sharpenning filter.
//
//      asharp engine implementation (C/MMX/ISSE)
//
//      Copyright (C) 2002 Marc Fauconneau
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
//  Please contact me for any bugs or questions.
//  marc.fd@libertysurf.fr

//  Change log :
//         27 nov 2002 - ver 0.95  - First GPL release.

#include "stdafx.h"
#include "asharp.h"
#include "Tconfig.h"
#include "simd.h"

#pragma warning (push)
#pragma warning (disable:4799)

static void asharp_run_c(unsigned char* planeptr, int pitch,
                         int height, int width,
                         int T,int D, int B, int B2, int bf,unsigned char* lineptr)
{
 unsigned char *cfp=planeptr+pitch;
 unsigned char *lp=lineptr;
 memcpy(lp,planeptr,width);
 for (int y=1;y<height-1;y++)
  {
   int last = cfp[0];
   int x;
   for (x=1;x<width-1;x++)
    {
     int avg = 0;
     int dev = 0;
     int T2;
     int diff;
     int tmp;

     avg += lp[x-1];
     avg += lp[x  ];
     avg += lp[x+1];
     avg += last;
     avg += cfp[x    ];
     avg += cfp[x  +1];
     avg += cfp[x+pitch-1];
     avg += cfp[x+pitch  ];
     avg += cfp[x+pitch+1];

     avg *= (65535/9);
     avg >>= 16;

     #define CHECK(A) \
     if (abs(A-cfp[x])>dev) dev = abs(A-cfp[x]);

     if (bf)
      {
       if (y%8>0)
        {
         if (x%8>0) CHECK(lp[x-1])
         CHECK(lp[x  ])
         if (x%8<7) CHECK(lp[x+1])
        }
       if (x%8>0) CHECK(last)
       if (x%8<7) CHECK(cfp[x  +1])
       if (y%8<7)
        {
         if (x%8>0) CHECK(cfp[x+pitch-1])
         CHECK(cfp[x+pitch  ])
         if (x%8<7) CHECK(cfp[x+pitch+1])
        }
      }
     else
      {
       CHECK(lp[x-pitch-1])
       CHECK(lp[x-pitch  ])
       CHECK(lp[x-pitch+1])
       CHECK(last)
       CHECK(cfp[x  +1])
       CHECK(cfp[x+pitch-1])
       CHECK(cfp[x+pitch  ])
       CHECK(cfp[x+pitch+1])
      }
     #undef CHECK
     T2 = T;
     diff = cfp[x]-avg;
     int D2 = D;

     if (x%8==6) D2=(D2*B2)>>8;
     if (x%8==7) D2=(D2*B)>>8;
     if (x%8==0) D2=(D2*B)>>8;
     if (x%8==1) D2=(D2*B2)>>8;
     if (y%8==6) D2=(D2*B2)>>8;
     if (y%8==7) D2=(D2*B)>>8;
     if (y%8==0) D2=(D2*B)>>8;
     if (y%8==1) D2=(D2*B2)>>8;

     int Da = -32+(D>>7);
     if (D>0) T2 = ((((dev<<7)*D2)>>16)+Da)<<4;

     if (T2>T) T2=T;
     if (T2<-32) T2=-32;

     tmp = (((diff<<7)*T2)>>16)+cfp[x];

     if (tmp < 0) tmp = 0;
     if (tmp > 255) tmp = 255;
     lp[x-1] = (unsigned char)last;
     last = cfp[x];
     cfp[x] = (unsigned char)tmp;
    }
   lp[x] = cfp[x];
   cfp += pitch;
  }
}

static __forceinline void apply(__m64 &srch,__m64 &srcl,__m64 &difl,__m64 &difh,const __m64 &thrh,const __m64 &thrl)
{
 difh=_mm_slli_pi16(difh,7);     //  psllw   difh,7
 difl=_mm_slli_pi16(difl,7);     //  psllw   difl,7
 difh=_mm_mulhi_pi16(difh,thrh); //  pmulhw  difh,thrh
 difl=_mm_mulhi_pi16(difl,thrl); //  pmulhw  difl,thrl
 srch=_mm_add_pi16(srch,difh);   //  paddw   srch,difh
 srcl=_mm_add_pi16(srcl,difl);   //  paddw   srcl,difl
}

static __forceinline void diff(__m64 &srch,__m64 &srcl,__m64 &difl,__m64 &difh,__m64 &acch,__m64 &accl,const __m64 &c4w_inv9,const __m64 &zero)
{
 acch=_mm_mulhi_pi16(acch,c4w_inv9);//  pmulhw    acch,c4w_inv9
 accl=_mm_mulhi_pi16(accl,c4w_inv9);//  pmulhw    accl,c4w_inv9
 srcl=srch;                         //  movq      srcl,srch
 srch=_mm_unpackhi_pi8(srch,zero);  //  punpckhbw srch,zero
 srcl=_mm_unpacklo_pi8(srcl,zero);  //  punpcklbw srcl,zero
 difh=srch;                         //  movq      difh,srch
 difl=srcl;                         //  movq      difl,srcl
 difh=_mm_subs_pi16(difh,acch);     //  psubsw    difh,acch
 difl=_mm_subs_pi16(difl,accl);     //  psubsw    difl,accl
}

static __forceinline void acc8b(__m64 &srch,__m64 &srcl,__m64 &acch,__m64 &accl,const __m64 &zero)
{
 srcl=srch;                        //  movq      srcl,srch
 srch=_mm_unpackhi_pi8(srch,zero); //  punpckhbw srch,zero
 srcl=_mm_unpacklo_pi8(srcl,zero); //  punpcklbw srcl,zero
 accl=_mm_adds_pu16(accl,srcl);    //  paddusw   accl,srcl
 acch=_mm_adds_pu16(acch,srch);    //  paddusw   acch,srch
}

static __forceinline void loadAcc(const __m64 &offset,__m64 &srch,__m64 &srcl,__m64 &acch,__m64 &accl,const __m64 &zero)
{
 srch=offset;
 acc8b(srch,srcl,acch,accl,zero);
}

static __forceinline void run_a(unsigned char *srcp,unsigned char *tmpp,__m64 &last,int stride,__m64 &srch,__m64 &srcl,__m64 &acch,__m64 &accl,const __m64 &zero)
{
 acch=zero; //  pxor acch,acch
 accl=zero; //  pxor accl,accl
 loadAcc(*(__m64*)(tmpp-1),srch,srcl,acch,accl,zero);
 loadAcc(*(__m64*)(tmpp  ),srch,srcl,acch,accl,zero);
 loadAcc(*(__m64*)(tmpp+1),srch,srcl,acch,accl,zero);
 loadAcc(last             ,srch,srcl,acch,accl,zero);
 loadAcc(*(__m64*)(srcp  ),srch,srcl,acch,accl,zero);
 loadAcc(*(__m64*)(srcp+1),srch,srcl,acch,accl,zero);
 loadAcc(*(__m64*)(srcp+stride-1),srch,srcl,acch,accl,zero);
 loadAcc(*(__m64*)(srcp+stride  ),srch,srcl,acch,accl,zero);
 loadAcc(*(__m64*)(srcp+stride+1),srch,srcl,acch,accl,zero);
}

static __forceinline void run1(unsigned char *cfp,int x,unsigned char *lineptr,__m64 &mm7,int stride,__m64 &mm2,__m64 &mm3,__m64 &mm4,__m64 &mm5,__m64 &mm6,const __m64 &c4w_inv9,const __m64 &c4w_T,__m64 &mm1)
{
 unsigned char *ptr=cfp+x;
 unsigned char *tmp=lineptr+x;
 __m64 zero=_mm_setzero_si64();
 run_a(ptr,tmp,mm7,stride,mm2,mm3,mm4,mm5,zero);
 *(__m64*)(tmp-1)=mm7;
 mm1=*(__m64*)ptr;
 diff(mm1,mm2,mm6,mm7,mm4,mm5,c4w_inv9,zero);
 mm3=c4w_T;
 mm4=mm3;
 apply(mm1,mm2,mm6,mm7,mm3,mm4);
 mm2=_mm_packs_pu16(mm2,mm1);
 mm7=*(__m64*)(ptr+7);
 *(__m64*)ptr=mm2;
}

template<class Tsimd> struct Tasharp
{
 static __forceinline void check8bm(__m64 &cur,__m64 &tmp,__m64 &ref,__m64 &max,const __m64 &msk)
  {
   tmp=ref;                   //  movq    tmp,ref
   tmp=_mm_subs_pu8(tmp,cur); //  psubusb tmp,cur
   cur=_mm_subs_pu8(cur,ref); //  psubusb cur,ref
   tmp=_mm_or_si64(tmp,cur);  //  por     tmp,cur
   tmp=_mm_and_si64(tmp,msk); //  pand    tmp,msk
   Tsimd::pmaxub(max,tmp);
  }

 static __forceinline void loadCheck2(const __m64 &offset,__m64 &cur,__m64 &tmp,__m64 &ref,__m64 &max,const __m64 &msk)
  {
   cur=offset;
   check8bm(cur,tmp,ref,max,msk);
  }

 static __forceinline void check8b(__m64 &cur,__m64 &tmp,__m64 &ref,__m64 &max)
  {
   tmp=ref;                   //  movq    tmp,ref
   tmp=_mm_subs_pu8(tmp,cur); //  psubusb tmp,cur
   cur=_mm_subs_pu8(cur,ref); //  psubusb cur,ref
   tmp=_mm_or_si64(tmp,cur);  //  por     tmp,cur
   Tsimd::pmaxub(max,tmp);
  }

 static __forceinline void loadCheck(const __m64 &offset,__m64 &cur,__m64 &tmp,__m64 &ref,__m64 &max)
  {
   cur=offset;
   check8b(cur,tmp,ref,max);
  }

 static __forceinline void atresh(__m64 &thrh,__m64 &thrl,const __m64 &c4w_D1,const __m64 &c4w_D2,const __m64 &c4w_Td4,const __m64 &c4w_Da,__m64 &zero)
  {
   thrl=thrh;                        //  movq      thrl,thrh
   thrh=_mm_unpackhi_pi8(thrh,zero); //  punpckhbw thrh,zero
   thrl=_mm_unpacklo_pi8(thrl,zero); //  punpcklbw thrl,zero
   thrh=_mm_slli_pi16(thrh,7);       //  psllw     thrh,7
   thrl=_mm_slli_pi16(thrl,7);       //  psllw     thrl,7
   thrh=_mm_mulhi_pi16(thrh,c4w_D1); //  pmulhw    thrh,c4w_D1
   thrl=_mm_mulhi_pi16(thrl,c4w_D2); //  pmulhw    thrl,c4w_D2
   Tsimd::pminsw(thrh,c4w_Td4);
   Tsimd::pminsw(thrl,c4w_Td4);
   thrh=_mm_adds_pi16(thrh,c4w_Da);  //  paddsw    thrh,c4w_Da
   thrl=_mm_adds_pi16(thrl,c4w_Da);  //  paddsw    thrl,c4w_Da
   thrh=_mm_slli_pi16(thrh,4);       //  psllw             thrh,4
   thrl=_mm_slli_pi16(thrl,4);       //  psllw             thrl,4
  }

 struct Trun_c2
  {
   static __forceinline void run(unsigned char *srcp,unsigned char *tmpp,__m64 &last,int stride,__m64 &cur,__m64 &tmp,__m64 &ref,__m64 &max,const __m64 &c8b_00r,const __m64 &c8b_00l)
    {
     max=_mm_setzero_si64(); //  pxor      max,max
     ref=*(__m64*)srcp;      //  movq      ref,[srcp]
     loadCheck2(*(__m64*)(tmpp-1)       ,cur,tmp,ref,max,c8b_00l);
     loadCheck (*(__m64*)(tmpp  )       ,cur,tmp,ref,max);
     loadCheck2(*(__m64*)(tmpp+1)       ,cur,tmp,ref,max,c8b_00r);
     loadCheck2(last                    ,cur,tmp,ref,max,c8b_00l);
     loadCheck2(*(__m64*)(srcp+1)       ,cur,tmp,ref,max,c8b_00r);
     loadCheck2(*(__m64*)(srcp+stride-1),cur,tmp,ref,max,c8b_00l);
     loadCheck (*(__m64*)(srcp+stride  ),cur,tmp,ref,max);
     loadCheck2(*(__m64*)(srcp+stride+1),cur,tmp,ref,max,c8b_00r);
    }
  };

 struct Trun_c2u
  {
   static __forceinline void run(unsigned char *srcp,unsigned char *tmpp,__m64 &last,int stride,__m64 &cur,__m64 &tmp,__m64 &ref,__m64 &max,const __m64 &c8b_00r,const __m64 &c8b_00l)
    {
     max=_mm_setzero_si64();
     ref=*(__m64*)srcp;
     loadCheck2(*(__m64*)(tmpp-1),cur,tmp,ref,max,c8b_00l);
     loadCheck (*(__m64*)(tmpp  ),cur,tmp,ref,max);
     loadCheck2(*(__m64*)(tmpp+1),cur,tmp,ref,max,c8b_00r);
     loadCheck2(last             ,cur,tmp,ref,max,c8b_00l);
     loadCheck2(*(__m64*)(srcp+1),cur,tmp,ref,max,c8b_00r);
    }
  };

 struct Trun_c2d
  {
   static __forceinline void run(unsigned char *srcp,unsigned char *tmpp,__m64 &last,int stride,__m64 &cur,__m64 &tmp,__m64 &ref,__m64 &max,const __m64 &c8b_00r,const __m64 &c8b_00l)
    {
     max=_mm_setzero_si64();
     ref=*(__m64*)srcp;
     loadCheck2(last                    ,cur,tmp,ref,max,c8b_00l);
     loadCheck2(*(__m64*)(srcp+1)       ,cur,tmp,ref,max,c8b_00r);
     loadCheck2(*(__m64*)(srcp+stride-1),cur,tmp,ref,max,c8b_00l);
     loadCheck (*(__m64*)(srcp+stride  ),cur,tmp,ref,max);
     loadCheck2(*(__m64*)(srcp+stride+1),cur,tmp,ref,max,c8b_00r);
    }
  };

 struct Trun_c
  {
   static __forceinline void run(unsigned char *srcp,unsigned char *tmpp,__m64 &last,int stride,__m64 &cur,__m64 &tmp,__m64 &ref,__m64 &max,const __m64 &c8b_00r,const __m64 &c8b_00l)
    {
     max=_mm_setzero_si64();
     ref=*(__m64*)srcp;
     loadCheck(*(__m64*)(tmpp-1)       ,cur,tmp,ref,max);
     loadCheck(*(__m64*)(tmpp  )       ,cur,tmp,ref,max);
     loadCheck(*(__m64*)(tmpp+1)       ,cur,tmp,ref,max);
     loadCheck(last                    ,cur,tmp,ref,max);
     loadCheck(*(__m64*)(srcp+1)       ,cur,tmp,ref,max);
     loadCheck(*(__m64*)(srcp+stride-1),cur,tmp,ref,max);
     loadCheck(*(__m64*)(srcp+stride  ),cur,tmp,ref,max);
     loadCheck(*(__m64*)(srcp+stride+1),cur,tmp,ref,max);
    }
  };

 template<class Trun> static __forceinline void run2(unsigned char *cfp,int x,unsigned char *lineptr,__m64 &mm7,int stride,__m64 &mm2,__m64 &mm3,__m64 &mm4,__m64 &mm5,__m64 &mm6,const __m64 &c4w_inv9,__m64 &mm1,const __m64 &c8b_00r,const __m64 &c8b_00l,const __m64 &c4w_D1,const __m64 &c4w_D2,const __m64 &c4w_Td4,const __m64 &c4w_Da)
  {
   unsigned char *ptr=cfp+x;
   unsigned char *tmp=lineptr+x;
   __m64 mm0=_mm_setzero_si64();
   run_a(ptr,tmp,mm7,stride,mm2,mm3,mm4,mm5,mm0);
   Trun::run(ptr,tmp,mm7,stride,mm2,mm0,mm6,mm3,c8b_00r,c8b_00l);
   *(__m64*)(tmp-1)=mm7;
   mm0=_mm_setzero_si64();
   mm1=*(__m64*)ptr;
   diff(mm1,mm2,mm6,mm7,mm4,mm5,c4w_inv9,mm0);
   atresh(mm3,mm4,c4w_D1,c4w_D2,c4w_Td4,c4w_Da,mm0);
   apply(mm1,mm2,mm6,mm7,mm3,mm4);
   mm2=_mm_packs_pu16(mm2,mm1);
   mm7=*(__m64*)(ptr+7);
   *(__m64*)ptr=mm2;
  }

 static void asharp_run(unsigned char* planeptr, int pitch,
                        int height, int width,
                        int T,int D, int B, int B2, int bf, unsigned char* lineptrna)
  {
   unsigned char* lineptr = (unsigned char*)(((intptr_t)lineptrna)+8-(((intptr_t)lineptrna)&7));

   memcpy(lineptr,planeptr,width);
   unsigned char* cfp = planeptr+pitch;

   __align8(uint16_t,c4w_Dtab[3*8]);
   __align8(uint16_t,*c4w_Dtabp) = c4w_Dtab;

   for (int i=0;i<3;i++)
    {
     int D2=D;
     int D3=D;
     int D4=D;
     if (B<=128)
      {
       if (i==0) D2=(D2*B)>>8;
       if (i==1) D2=(D2*B2)>>8;
       D3=D2;
       D4=D2;
       D3=(D2*B2)>>8;
       D4=(D2*B)>>8;
      }
     c4w_Dtab[i*8+0] = (uint16_t)D4;
     c4w_Dtab[i*8+1] = (uint16_t)D3;
     c4w_Dtab[i*8+2] = (uint16_t)D2;
     c4w_Dtab[i*8+3] = (uint16_t)D2;
     c4w_Dtab[i*8+4] = (uint16_t)D2;
     c4w_Dtab[i*8+5] = (uint16_t)D2;
     c4w_Dtab[i*8+6] = (uint16_t)D3;
     c4w_Dtab[i*8+7] = (uint16_t)D4;
    }

   __m64 c4w_inv9=_mm_set1_pi16(65535/9);
   __m64 c4w_Da=_mm_set1_pi16(short(-32+(D>>7)));
   __m64 c8b_00r=_mm_set_pi8(-1,-1,-1,-1,-1,-1,-1,0);
   __m64 c8b_00l=_mm_set_pi8(0,-1,-1,-1,-1,-1,-1,-1);
   __m64 c4w_T=_mm_set1_pi16(short(T));
   __m64 c4w_Td4=_mm_set1_pi16(short(T>>4));
   __align8(uint8_t,v8b_first[8]);

   __m64 mm2,c4w_D1,c4w_D2;
   for (int y=1;y<height-1;y++)
    {
     if ((y&7)==0 || (y&7)==7)
      {
       c4w_D1=*(__m64*)(c4w_Dtabp+0);
       c4w_D2=mm2=*(__m64*)(c4w_Dtabp+4);
      }
     else if ((y&7)==1 || (y&7)==6)
      {
       c4w_D1=*(__m64*)(c4w_Dtabp+8);
       c4w_D2=mm2=*(__m64*)(c4w_Dtabp+12);
      }
     else
      {
       c4w_D1=*(__m64*)(c4w_Dtabp+16);
       c4w_D2=mm2=*(__m64*)(c4w_Dtabp+20);
      }

     *(__m64*)v8b_first=_mm_slli_si64(*(__m64*)cfp,8);
     v8b_first[0]=v8b_first[1];
     __m64 mm7=*(__m64*)v8b_first;

     __m64 mm1,mm3,mm4,mm5,mm6;
     if (bf && D>0)
      if ((y&7)==0)
       for (int x=0;x<width;x+=8)
        run2<Trun_c2>(cfp,x,lineptr,mm7,pitch,mm2,mm3,mm4,mm5,mm6,c4w_inv9,mm1,c8b_00r,c8b_00l,c4w_D1,c4w_D2,c4w_Td4,c4w_Da);
      else if ((y&7)==7)
       for (int x=0;x<width;x+=8)
        run2<Trun_c2u>(cfp,x,lineptr,mm7,pitch,mm2,mm3,mm4,mm5,mm6,c4w_inv9,mm1,c8b_00r,c8b_00l,c4w_D1,c4w_D2,c4w_Td4,c4w_Da);
      else
       for (int x=0;x<width;x+=8)
        run2<Trun_c2d>(cfp,x,lineptr,mm7,pitch,mm2,mm3,mm4,mm5,mm6,c4w_inv9,mm1,c8b_00r,c8b_00l,c4w_D1,c4w_D2,c4w_Td4,c4w_Da);
     else if (D>0)
      for (int x=0;x<width;x+=8)
       run2<Trun_c>(cfp,x,lineptr,mm7,pitch,mm2,mm3,mm4,mm5,mm6,c4w_inv9,mm1,c8b_00r,c8b_00l,c4w_D1,c4w_D2,c4w_Td4,c4w_Da);
     else
      for (int x=0;x<width;x+=8)
       run1(cfp,x,lineptr,mm7,pitch,mm2,mm3,mm4,mm5,mm6,c4w_inv9,c4w_T,mm1);
     cfp+=pitch;
    }
  }
};

asharp_run_fct* getAsharp(void)
{
 if (Tconfig::cpu_flags&FF_CPU_MMXEXT)
  return Tasharp<Tmmxext>::asharp_run;
 else if (Tconfig::cpu_flags&FF_CPU_MMX)
  return Tasharp<Tmmx>::asharp_run;
 else
  return asharp_run_c;
}

#pragma warning(pop)
