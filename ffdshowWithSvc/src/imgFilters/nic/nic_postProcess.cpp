#include "stdafx.h"
#include "nic_postProcess.h"
#include "postproc/postprocFilters.h"
#include "simd.h"
#include "Tconfig.h"

#pragma warning(push)
#pragma warning(disable:4309)
#pragma warning(disable:4799)

/******************* general, useful macros ****************/

//#define DEBUGMODE
//#define SELFCHECK
//#define PREFETCH

#ifdef PREFETCH
#define PREFETCH_AHEAD_V 8
#define PREFETCH_AHEAD_H 8
#define PREFETCH_ENABLE
#endif

#ifdef SELFCHECK
#include "ffdebug.h"
#define PP_SELF_CHECK
#define SELF_CHECK
#define SIGN(a)    ( (a)<0 ? -1 : 1 )
#endif

#ifdef DEBUGMODE
#define SHOWDECISIONS_H
#define SHOW_DECISIONS
#define SHOWDECISIONS_V
#endif

/* decide DC mode or default mode for the horizontal filter */
static inline int deblock_horiz_useDC(uint8_t *v, stride_t stride, int DEBLOCK_HORIZ_USEDC_THR)
{
 __m64 mm64_mask = _mm_set_pi8(0,-2,-2,-2,-2,-2,-2,-2);// (0x00,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe);
 int32_t mm32_result;
 __m64 *pmm1;
 int eq_cnt, useDC;

 #ifdef PP_SELF_CHECK
 int eq_cnt2, j, k;
 #endif

 pmm1 = (__m64*)(&(v[1])); /* this is a 32-bit aligned pointer, not 64-aligned */

 //        mov eax, pmm1

         /* first load some constants into mm4, mm5, mm6, mm7 */
 __m64 mm6=mm64_mask;         //        movq mm6, mm64_mask          /*mm6 = 0x00fefefefefefefe       */
 __m64 mm4=_mm_setzero_si64();//        pxor mm4, mm4                /*mm4 = 0x0000000000000000       */

 __m64 mm1=*pmm1; //       movq mm1, qword ptr [eax]    /* mm1 = *pmm            0 1 2 3 4 5 6 7    */
 pmm1+=stride/8;  //        add eax, stride              /* eax += stride/8      0 1 2 3 4 5 6 7    */

 __m64 mm5=mm1;//        movq mm5, mm1                /* mm5 = mm1             0 1 2 3 4 5 6 7    */
 mm1=_mm_srli_si64(mm1,8);//        psrlq mm1, 8                 /* mm1 >>= 8             0 1 2 3 4 5 6 7    */

 __m64 mm2=mm5;//        movq mm2, mm5                /* mm2 = mm5             0 1 2 3 4 5 6 7    */
 mm5=_mm_subs_pu8(mm5,mm1);//        psubusb mm5, mm1             /* mm5 -= mm1            0 1 2 3 4 5 6 7    */

 __m64 mm3;
         movq(mm3, *pmm1);    /* mm3 = *pmm            0 1 2 3 4 5 6 7    */
         psubusb(mm1, mm2);             /* mm1 -= mm2            0 1 2 3 4 5 6 7    */

         pmm1+=stride/8;//add eax, stride              /* eax += stride/8      0 1 2 3 4 5 6 7    */
         por(mm5, mm1);               /* mm5 |= mm1            0 1 2 3 4 5 6 7    */
 __m64 mm0;
         movq( mm0, mm3);              /* mm0 = mm3             0 1 2 3 4 5 6 7    */
         pand( mm5, mm6);              /* mm5 &= 0xfefefefefefefefe     */
 __m64 mm7;
         pxor( mm7, mm7);                /*mm7 = 0x0000000000000000       */
         pcmpeqb( mm5, mm4);             /* are the bytes of mm5 == 0 ?   */

         movq( mm1, *pmm1);    /* mm3 = *pmm            0 1 2 3 4 5 6 7    */
         psubb( mm7, mm5);               /* mm7 has running total of eqcnts */

         psrlq (mm3, 8);                 /* mm3 >>= 8             0 1 2 3 4 5 6 7    */
         movq( mm5, mm0);                /* mm5 = mm0             0 1 2 3 4 5 6 7    */

         psubusb( mm0, mm3);             /* mm0 -= mm3            0 1 2 3 4 5 6 7    */

         pmm1+=stride/8;//add eax, stride              /* eax += stride/8      0 1 2 3 4 5 6 7    */
         psubusb (mm3, mm5);             /* mm3 -= mm5            0 1 2 3 4 5 6 7    */

         movq (mm5, *pmm1);//    /* mm5 = *pmm            0 1 2 3 4 5 6 7    */
         por (mm0, mm3);                 /* mm0 |= mm3            0 1 2 3 4 5 6 7    */

         movq( mm3, mm1);                /* mm3 = mm1             0 1 2 3 4 5 6 7    */
         pand( mm0, mm6);                /* mm0 &= 0xfefefefefefefefe     */

         psrlq(   mm1, 8);               /* mm1 >>= 8             0 1 2 3 4 5 6 7    */
         pcmpeqb( mm0, mm4);             /* are the bytes of mm0 == 0 ?   */

         movq( mm2, mm3);                /* mm2 = mm3             0 1 2 3 4 5 6 7    */
         psubb( mm7, mm0);               /* mm7 has running total of eqcnts */

         psubusb( mm3, mm1);             /* mm3 -= mm1            0 1 2 3 4 5 6 7    */

         psubusb( mm1, mm2);             /* mm1 -= mm2            0 1 2 3 4 5 6 7    */

         por( mm3, mm1);                 /* mm3 |= mm1            0 1 2 3 4 5 6 7    */
         movq( mm1, mm5);                /* mm1 = mm5             0 1 2 3 4 5 6 7    */

         pand(    mm3, mm6);             /* mm3 &= 0xfefefefefefefefe     */
         psrlq(   mm5, 8);               /* mm5 >>= 8             0 1 2 3 4 5 6 7    */

         pcmpeqb( mm3, mm4);             /* are the bytes of mm3 == 0 ?   */
         movq   ( mm0, mm1);             /* mm0 = mm1             0 1 2 3 4 5 6 7    */

         psubb  ( mm7, mm3);             /* mm7 has running total of eqcnts */
         psubusb( mm1, mm5);             /* mm1 -= mm5            0 1 2 3 4 5 6 7    */

         psubusb( mm5, mm0);             /* mm5 -= mm0            0 1 2 3 4 5 6 7    */
         por    ( mm1, mm5);             /* mm1 |= mm5            0 1 2 3 4 5 6 7    */

         pand   ( mm1, mm6);             /* mm1 &= 0xfefefefefefefefe     */

         pcmpeqb( mm1, mm4);             /* are the bytes of mm1 == 0 ?   */

         psubb  ( mm7, mm1);             /* mm7 has running total of eqcnts */

         movq    (mm1, mm7);             /* mm1 = mm7             0 1w2 3 4 5 6 7r   */
         psllq   (mm7, 8);               /* mm7 >>= 24            0 1 2 3 4 5 6 7m   */

         psrlq   (mm1, 24);              /* mm7 >>= 24            0 1 2 3 4 5 6 7m   */

         paddb   (mm7, mm1);             /* mm7 has running total of eqcnts */

         movq (mm1, mm7);                /* mm1 = mm7             0 1w2 3 4 5 6 7r   */
         psrlq (mm7, 16);                /* mm7 >>= 16            0 1 2 3 4 5 6 7m   */

         paddb   (mm7, mm1);             /* mm7 has running total of eqcnts */

         movq (mm1, mm7);                /* mm1 = mm7             0 1w2 3 4 5 6 7r   */
         psrlq (  mm7, 8);               /* mm7 >>= 8             0 1 2 3 4 5 6 7m   */

         paddb (mm7, mm1);               /* mm7 has running total of eqcnts */

         movd (mm32_result, mm7);

         //pop eax

 eq_cnt = mm32_result & 0xff;

 #ifdef PP_SELF_CHECK
 eq_cnt2 = 0;
 for (k=0; k<4; k++)
 {
         for (j=1; j<=7; j++)
         {
                 if (abs(v[j+k*stride]-v[1+j+k*stride]) <= 1) eq_cnt2++;
         }
 }
 if (eq_cnt2 != eq_cnt)
         DPRINTF(_l("ERROR: MMX version of useDC is incorrect"));
 #endif

 useDC = eq_cnt >= DEBLOCK_HORIZ_USEDC_THR;

 return useDC;
}

/* decide whether the DC filter should be turned on according to QP */
static inline int deblock_horiz_DC_on(uint8_t *v, stride_t stride, int QP)
{
 /* 99% of the time, this test turns out the same as the |max-min| strategy in the standard */
// return (abs(v[1]-v[8]) < 2*QP);
 for (int i=0; i<4; ++i)
  {
   if (abs(v[0]-v[5]) >= 2*QP) return false;
   if (abs(v[1]-v[8]) >= 2*QP) return false;
   if (abs(v[1]-v[4]) >= 2*QP) return false;
   if (abs(v[2]-v[7]) >= 2*QP) return false;
   if (abs(v[3]-v[6]) >= 2*QP) return false;
   v += stride;
  }
 return true;
}

/* The 9-tap low pass filter used in "DC" regions */
/* I'm not sure that I like this implementation any more...! */
static inline void deblock_horiz_lpf9(uint8_t *v, stride_t stride, int QP)
{
        int y, p1, p2;

        #ifdef PP_SELF_CHECK
        uint8_t selfcheck[9];
        int psum;
        uint8_t *vv;
        int i;
        #endif
        uint8_t *pmm1;
        uint32_t mm32_p1p2;
        static const uint64_t mm64_coefs[18] =
         {
          0x0001000200040006LL, /* p1 left */ 0x0000000000000001LL, /* v1 right */
          0x0001000200020004LL, /* v1 left */ 0x0000000000010001LL, /* v2 right */
          0x0002000200040002LL, /* v2 left */ 0x0000000100010002LL, /* v3 right */
          0x0002000400020002LL, /* v3 left */ 0x0001000100020002LL, /* v4 right */
          0x0004000200020001LL, /* v4 left */ 0x0001000200020004LL, /* v5 right */
          0x0002000200010001LL, /* v5 left */ 0x0002000200040002LL, /* v6 right */
          0x0002000100010000LL, /* v6 left */ 0x0002000400020002LL, /* v7 right */
          0x0001000100000000LL, /* v7 left */ 0x0004000200020001LL, /* v8 right */
          0x0001000000000000LL, /* v8 left */ 0x0006000400020001LL  /* p2 right */
         };
        __m64 mm64_0008 = _mm_set1_pi16(0x0008);
        __m64 mm0,mm2,mm7,mm6,mm5,mm1,mm3,mm4;
        __m64 mm64_temp;
        for (y=0; y<4; y++)
        {
                p1 = (abs(v[0+y*stride]-v[1+y*stride]) < QP ) ?  v[0+y*stride] : v[1+y*stride];
                p2 = (abs(v[8+y*stride]-v[9+y*stride]) < QP ) ?  v[9+y*stride] : v[8+y*stride];

                mm32_p1p2 = 0x0101 * ((p2 << 16) + p1);

                #ifdef PP_SELF_CHECK
                /* generate a self-check version of the filter result in selfcheck[9] */
                /* low pass filtering (LPF9: 1 1 2 2 4 2 2 1 1) */
                vv = &(v[y*stride]);
                psum = p1 + p1 + p1 + vv[1] + vv[2] + vv[3] + vv[4] + 4;
                selfcheck[1] = (((psum + vv[1]) << 1) - (vv[4] - vv[5])) >> 4;
                psum += vv[5] - p1;
                selfcheck[2] = (((psum + vv[2]) << 1) - (vv[5] - vv[6])) >> 4;
                psum += vv[6] - p1;
                selfcheck[3] = (((psum + vv[3]) << 1) - (vv[6] - vv[7])) >> 4;
                psum += vv[7] - p1;
                selfcheck[4] = (((psum + vv[4]) << 1) + p1 - vv[1] - (vv[7] - vv[8])) >> 4;
                psum += vv[8] - vv[1];
                selfcheck[5] = (((psum + vv[5]) << 1) + (vv[1] - vv[2]) - vv[8] + p2) >> 4;
                psum += p2 - vv[2];
                selfcheck[6] = (((psum + vv[6]) << 1) + (vv[2] - vv[3])) >> 4;
                psum += p2 - vv[3];
                selfcheck[7] = (((psum + vv[7]) << 1) + (vv[3] - vv[4])) >> 4;
                psum += p2 - vv[4];
                selfcheck[8] = (((psum + vv[8]) << 1) + (vv[4] - vv[5])) >> 4;
                #endif

                pmm1 = (&(v[y*stride-3])); /* this is 64-aligned */

                /* mm7 = 0, mm6 is left hand accumulator, mm5 is right hand acc */
                        unsigned char *eax=(unsigned char*)pmm1;
                        unsigned char *ebx=(unsigned char*)&mm64_coefs[0];

                        #ifdef PREFETCH_ENABLE
                        prefetcht0 32[ebx]
                        #endif

                        movd   (mm0,   mm32_p1p2);            /* mm0 = ________p2p2p1p1    0w1 2 3 4 5 6 7    */
                        punpcklbw (mm0, mm0);                 /* mm0 = p2p2p2p2p1p1p1p1    0m1 2 3 4 5 6 7    */

                        movq    (mm2, eax);                   /* mm2 = v4v3v2v1xxxxxxxx    0 1 2w3 4 5 6 7    */
                        pxor    (mm7, mm7);                   /* mm7 = 0000000000000000    0 1 2 3 4 5 6 7w   */

                        movq     (mm6, mm64_0008);            /* mm6 = 0008000800080008    0 1 2 3 4 5 6w7    */
                        punpckhbw (mm2, mm2);                 /* mm2 = v4__v3__v2__v1__    0 1 2m3 4 5 6 7    */

                        movq     (mm64_temp, mm0);            /*temp = p2p2p2p2p1p1p1p1    0r1 2 3 4 5 6 7    */

                        punpcklbw (mm0, mm7);                 /* mm0 = __p1__p1__p1__p1    0m1 2 3 4 5 6 7    */
                        movq      (mm5, mm6);                 /* mm5 = 0008000800080008    0 1 2 3 4 5w6r7    */

                        pmullw    (mm0, ebx);               /* mm0 *= mm64_coefs[0]      0m1 2 3 4 5 6 7    */

                        movq      (mm1, mm2);                 /* mm1 = v4v4v3v3v2v2v1v1    0 1w2r3 4 5 6 7    */
                        punpcklbw (mm2, mm2);                 /* mm2 = v2v2v2v2v1v1v1v1    0 1 2m3 4 5 6 7    */

                        punpckhbw (mm1, mm1);                 /* mm1 = v4v4v4v4v3v3v3v3    0 1m2 3 4 5 6 7    */

                        #ifdef PREFETCH_ENABLE
                        prefetcht0 32[ebx]
                        #endif

                        movq      (mm3, mm2);                /* mm3 = v2v2v2v2v1v1v1v1    0 1 2r3w4 5 6 7    */
                        punpcklbw (mm2, mm7);                /* mm2 = __v1__v1__v1__v1    0 1 2m3 4 5 6 7    */

                        punpckhbw (mm3, mm7);                /* mm3 = __v2__v2__v2__v2    0 1 2 3m4 5 6 7    */
                        paddw     (mm6, mm0);                /* mm6 += mm0                0r1 2 3 4 5 6m7    */

                        movq      (mm0, mm2);                /* mm0 = __v1__v1__v1__v1    0w1 2r3 4 5 6 7    */

                        pmullw    (mm0, 8+ebx);             /* mm2 *= mm64_coefs[1]      0m1 2 3 4 5 6 7    */
                        movq      (mm4, mm3);                /* mm4 = __v2__v2__v2__v2    0 1 2 3r4w5 6 7    */

                        pmullw    (mm2, 16+ebx);            /* mm2 *= mm64_coefs[2]      0 1 2m3 4 5 6 7    */

                        pmullw    (mm3, 32+ebx);            /* mm3 *= mm64_coefs[4]      0 1 2 3m4 5 6 7    */

                        pmullw    (mm4, 24+ebx);            /* mm3 *= mm64_coefs[3]      0 1 2 3 4m5 6 7    */
                        paddw     (mm5, mm0);                /* mm5 += mm0                0r1 2 3 4 5m6 7    */

                        paddw     (mm6, mm2);                /* mm6 += mm2                0 1 2r3 4 5 6m7    */
                        movq      (mm2, mm1);                /* mm2 = v4v4v4v4v3v3v3v3    0 1 2 3 4 5 6 7    */

                        punpckhbw (mm2, mm7);                /* mm2 = __v4__v4__v4__v4    0 1 2m3 4 5 6 7r   */
                        paddw     (mm5, mm4);                /* mm5 += mm4                0 1 2 3 4r5m6 7    */

                        punpcklbw (mm1, mm7);                /* mm1 = __v3__v3__v3__v3    0 1m2 3 4 5 6 7r   */
                        paddw     (mm6, mm3);                /* mm6 += mm3                0 1 2 3r4 5 6m7    */

                        #ifdef PREFETCH_ENABLE
                        prefetcht0 64[ebx]
                        #endif
                        movq      (mm0, mm1);                /* mm0 = __v3__v3__v3__v3    0w1 2 3 4 5 6 7    */

                        pmullw    (mm1, 48+ebx);            /* mm1 *= mm64_coefs[6]      0 1m2 3 4 5 6 7    */

                        pmullw    (mm0, 40+ebx);            /* mm0 *= mm64_coefs[5]      0m1 2 3 4 5 6 7    */
                        movq      (mm4, mm2);                /* mm4 = __v4__v4__v4__v4    0 1 2r3 4w5 6 7    */

                        pmullw    (mm2, 64+ebx);            /* mm2 *= mm64_coefs[8]      0 1 2 3 4 5 6 7    */
                        paddw     (mm6, mm1);                /* mm6 += mm1                0 1 2 3 4 5 6 7    */

                        pmullw    (mm4, 56+ebx);            /* mm4 *= mm64_coefs[7]      0 1 2 3 4m5 6 7    */
                        pxor      (mm3, mm3);                /* mm3 = 0000000000000000    0 1 2 3w4 5 6 7    */

                        movq      (mm1, 8+eax);             /* mm1 = xxxxxxxxv8v7v6v5    0 1w2 3 4 5 6 7    */
                        paddw     (mm5, mm0);                /* mm5 += mm0                0r1 2 3 4 5 6 7    */

                        punpcklbw (mm1, mm1);                /* mm1 = v8v8v7v7v6v6v5v5    0 1m2 3m4 5 6 7    */
                        paddw     (mm6, mm2);                /* mm6 += mm2                0 1 2r3 4 5 6 7    */

                        #ifdef PREFETCH_ENABLE
                        prefetcht0 96[ebx]
                        #endif

                        movq      (mm2, mm1);                /* mm2 = v8v8v7v7v6v6v5v5    0 1r2w3 4 5 6 7    */
                        paddw     (mm5, mm4);                /* mm5 += mm4                0 1 2 3 4r5 6 7    */

                        punpcklbw (mm2, mm2);                /* mm2 = v6v6v6v6v5v5v5v5    0 1 2m3 4 5 6 7    */
                        punpckhbw (mm1, mm1);                /* mm1 = v8v8v8v8v7v7v7v7    0 1m2 3 4 5 6 7    */

                        movq      (mm3, mm2);                /* mm3 = v6v6v6v6v5v5v5v5    0 1 2r3w4 5 6 7    */
                        punpcklbw (mm2, mm7);                /* mm2 = __v5__v5__v5__v5    0 1 2m3 4 5 6 7r   */

                        punpckhbw (mm3, mm7);                /* mm3 = __v6__v6__v6__v6    0 1 2 3m4 5 6 7r   */
                        movq      (mm0, mm2);                /* mm0 = __v5__v5__v5__v5    0w1 2b3 4 5 6 7    */

                        pmullw    (mm0, 72+ebx);            /* mm0 *= mm64_coefs[9]      0m1 2 3 4 5 6 7    */
                        movq      (mm4, mm3);                /* mm4 = __v6__v6__v6__v6    0 1 2 3 4w5 6 7    */

                        pmullw    (mm2, 80+ebx);            /* mm2 *= mm64_coefs[10]     0 1 2m3 4 5 6 7    */

                        pmullw    (mm3, 96+ebx);            /* mm3 *= mm64_coefs[12]     0 1 2 3m4 5 6 7    */

                        pmullw    (mm4, 88+ebx);            /* mm4 *= mm64_coefs[11]     0 1 2 3 4m5 6 7    */
                        paddw     (mm5, mm0);                /* mm5 += mm0                0r1 2 3 4 5 6 7    */

                        paddw     (mm6, mm2);                /* mm6 += mm2                0 1 2r3 4 5 6 7    */
                        movq      (mm2, mm1);                /* mm2 = v8v8v8v8v7v7v7v7    0 1r2w3 4 5 6 7    */

                        paddw     (mm6, mm3);                /* mm6 += mm3                0 1 2 3r4 5 6 7    */
                        punpcklbw (mm1, mm7);                /* mm1 = __v7__v7__v7__v7    0 1m2 3 4 5 6 7r   */

                        paddw     (mm5, mm4);                /* mm5 += mm4                0 1 2 3 4r5 6 7    */
                        punpckhbw (mm2, mm7);                /* mm2 = __v8__v8__v8__v8    0 1 2m3 4 5 6 7    */

                        #ifdef PREFETCH_ENABLE
                        prefetcht0 128[ebx]
                        #endif

                        movq      (mm3, mm64_temp);          /* mm0 = p2p2p2p2p1p1p1p1    0 1 2 3w4 5 6 7    */
                        movq      (mm0, mm1);                /* mm0 = __v7__v7__v7__v7    0w1r2 3 4 5 6 7    */

                        pmullw    (mm0, 104+ebx);           /* mm0 *= mm64_coefs[13]     0m1b2 3 4 5 6 7    */
                        movq      (mm4, mm2);                /* mm4 = __v8__v8__v8__v8    0 1 2r3 4w5 6 7    */

                        pmullw    (mm1, 112+ebx);           /* mm1 *= mm64_coefs[14]     0 1w2 3 4 5 6 7    */
                        punpckhbw (mm3, mm7);                /* mm0 = __p2__p2__p2__p2    0 1 2 3 4 5 6 7    */

                        pmullw    (mm2, 128+ebx);           /* mm2 *= mm64_coefs[16]     0 1b2m3 4 5 6 7    */

                        pmullw    (mm4, 120+ebx);           /* mm4 *= mm64_coefs[15]     0 1b2 3 4m5 6 7    */
                        paddw     (mm5, mm0);                /* mm5 += mm0                0r1 2 3 4 5m6 7    */

                        pmullw    (mm3, 136+ebx);           /* mm0 *= mm64_coefs[17]     0 1 2 3m4 5 6 7    */
                        paddw     (mm6, mm1);                /* mm6 += mm1                0 1w2 3 4 5 6m7    */

                        paddw     (mm6, mm2);                /* mm6 += mm2                0 1 2r3 4 5 6m7    */

                        paddw     (mm5, mm4);                /* mm5 += mm4                0 1 2 3 4r5m6 7    */
                        psrlw     (mm6, 4);                  /* mm6 /= 16                 0 1 2 3 4 5 6m7    */

                        paddw     (mm5, mm3);                /* mm6 += mm0                0 1 2 3r4 5m6 7    */

                        psrlw     (mm5, 4);                  /* mm5 /= 16                 0 1 2 3 4 5m6 7    */

                        packuswb  (mm6, mm5);                /* pack result into mm6      0 1 2 3 4 5r6m7    */

                        movq      (4+eax, mm6);             /* v[] = mm6                 0 1 2 3 4 5 6r7    */


                #ifdef PP_SELF_CHECK
                for (i=1; i<=8; i++)
                {
                        if (selfcheck[i] != v[i+y*stride])
                        {
                                DPRINTF(_l("ERROR: MMX version of horiz lpf9 is incorrect at %d"), i);
                        }
                }
                #endif
        }
}

/* horizontal deblocking filter used in default (non-DC) mode */
static inline void deblock_horiz_default_filter(uint8_t *v, stride_t stride, int QP)
{
 int a3_0, a3_1, a3_2, d;
 int q1, q;
 int y;

 for (y=0; y<4; y++)
  {
   q1 = v[4] - v[5];
   q = q1 / 2;
   if (q)
    {
     a3_0  = q1;
     a3_0 += a3_0 << 2;
     a3_0 = 2*(v[3]-v[6]) - a3_0;

     /* apply the 'delta' function first and check there is a difference to avoid wasting time */
     if (abs(a3_0) < 8*QP)
      {
       a3_1  = v[3]-v[2];
       a3_2  = v[7]-v[8];
       a3_1 += a3_1 << 2;
       a3_2 += a3_2 << 2;
       a3_1 += (v[1]-v[4]) << 1;
       a3_2 += (v[5]-v[8]) << 1;
       d = abs(a3_0) - std::min(abs(a3_1), abs(a3_2));

       if (d > 0) /* energy across boundary is greater than in one or both of the blocks */
        {
         d += d<<2;
         d = (d + 32) >> 6;

         if (d > 0)
          {
           //d *= SIGN(-a3_0);
           /* clip d in the range 0 ... q */
           if (q > 0)
            {
             if (a3_0 < 0)
              {
               //d = d<0 ? 0 : d;
               d = d>q ? q : d;
               v[4] = uint8_t(v[4]-d);
               v[5] = uint8_t(v[5]+d);
              }
            }
           else
            {
             if (a3_0 > 0)
              {
               //d = d>0 ? 0 : d;
               d = (-d)<q ? q : (-d);
               v[4] = uint8_t(v[4]-d);
               v[5] = uint8_t(v[5]+d);
              }
            }
          }
        }
      }
    }
   #ifdef PP_SELF_CHECK
   /* no selfcheck written for this yet */
   #endif
   v += stride;
  }
}

/* this is a horizontal deblocking filter - i.e. it will smooth _vertical_ block edges */
static void deblock_horiz(uint8_t *image, int width, stride_t stride, const int8_t *QP_store, int QP_stride, int chromaFlag, int DEBLOCK_HORIZ_USEDC_THR)
{
 int x, y=0;
 int QP;
 uint8_t *v;
 int useDC, DC_on;
 #ifdef PREFETCH_AHEAD_H
 void *prefetch_addr;
 #endif


 /* loop over every block boundary in that row */
 for (x=8; x<width; x+=8)
  {
   /* extract QP from the decoder's array of QP values */
   QP = chromaFlag ? QP_store[y/8*QP_stride+x/8]
                   : QP_store[y/16*QP_stride+x/16];

   /* v points to pixel v0, in the left-hand block */
   v = &(image[y*stride + x]) - 5;

   #ifdef PREFETCH_AHEAD_V
   /* try a prefetch PREFETCH_AHEAD_V bytes ahead on all eight rows... experimental */
   prefetch_addr = v + PREFETCH_AHEAD_V;
   _mm_prefetch(prefetch_addr+1*stride,_MM_HINT_T0);
   _mm_prefetch(prefetch_addr+2*stride,_MM_HINT_T0);
   _mm_prefetch(prefetch_addr+3*stride,_MM_HINT_T0);
   _mm_prefetch(prefetch_addr+4*stride,_MM_HINT_T0);
   #endif

   /* first decide whether to use default or DC offet mode */
   useDC = deblock_horiz_useDC(v, stride, DEBLOCK_HORIZ_USEDC_THR);

   if (useDC) /* use DC offset mode */
    {
     DC_on = deblock_horiz_DC_on(v, stride, QP);
     if (DC_on)
      {
       deblock_horiz_lpf9(v, stride, QP);
       #ifdef SHOWDECISIONS_H
       if (!chromaFlag)
        {
         v[0*stride + 4] =
         v[1*stride + 4] =
         v[2*stride + 4] =
         v[3*stride + 4] = 255;
        }
       #endif
      }
    }
   else    /* use default mode */
    {
     deblock_horiz_default_filter(v, stride, QP);
     #ifdef SHOWDECISIONS_H
     if (!chromaFlag)
      {
       v[0*stride + 4] =
       v[1*stride + 4] =
       v[2*stride + 4] =
       v[3*stride + 4] = 0;
      }
     #endif
    }
  }
}

/* decide DC mode or default mode in assembler */
static inline  int deblock_vert_useDC(uint8_t *v, stride_t stride, int DEBLOCK_VERT_USEDC_THR)
{
        __m64 mask = _mm_set1_pi8(/*0xfe*/-2);
        int32_t mm_data1;
        uint64_t *pmm1;
        int eq_cnt, useDC;
        #ifdef PP_SELF_CHECK
        int useDC2, i, j;
        #endif

        #ifdef PP_SELF_CHECK
        /* C-code version for testing */
        eq_cnt = 0;
        for (j=1; j<8; j++)
        {
                for (i=0; i<8; i++)
                {
                        if (abs(v[j*stride+i] - v[(j+1)*stride+i]) <= 1) eq_cnt++;
                }
        }
        useDC2 = (eq_cnt > DEBLOCK_VERT_USEDC_THR);
        #endif

        /* starting pointer is at v[stride] == v1 in mpeg4 notation */
        pmm1 = (uint64_t *)(&(v[stride]));

        /* first load some constants into mm4, mm6, mm7 */
                //push eax
                unsigned char *eax=(unsigned char*)pmm1;
                __m64 mm6,mm7,mm2,mm4,mm3;
                movq (mm6, mask);               /*mm6 = 0xfefefefefefefefe       */
                pxor (mm7, mm7);                /*mm7 = 0x0000000000000000       */

                movq (mm2, eax);              /* mm2 = *p_data                 */
                pxor (mm4, mm4);                /*mm4 = 0x0000000000000000       */

                eax+=stride;                 /* p_data += stride              */
                movq   (mm3, mm2);              /* mm3 = *p_data                 */
                __m64 mm0,mm1;
                movq   (mm2, eax);           /* mm2 = *p_data                 */
                movq   (mm0, mm3);             /* mm0 = mm3                     */

                movq   (mm3, mm2);             /* mm3 = *p_data                 */
                movq   (mm1, mm0);             /* mm1 = mm0                     */

                psubusb (mm0, mm2);            /* mm0 -= mm2                    */
                eax+=stride;                   /* p_data += stride              */

                psubusb (mm2, mm1);            /* mm2 -= mm1                    */
                por    (mm0, mm2);             /* mm0 |= mm2                    */

                pand   (mm0, mm6);             /* mm0 &= 0xfefefefefefefefe     */
                pcmpeqb (mm0, mm4);            /* is mm0 == 0 ?                 */

                movq   (mm2, eax);             /* mm2 = *p_data                 */
                psubb  (mm7, mm0);             /* mm7 has running total of eqcnts */
                __m64 mm5;
                movq   (mm5, mm3);             /* mm5 = mm3                     */
                movq   (mm3, mm2);             /* mm3 = *p_data                 */

                movq   (mm1, mm5);             /* mm1 = mm5                     */
                psubusb (mm5, mm2);            /* mm5 -= mm2                    */

                psubusb (mm2, mm1);            /* mm2 -= mm1                    */
                por    (mm5, mm2);             /* mm5 |= mm2                    */

                eax+=stride;           /* p_data += stride              */
                pand   (mm5, mm6);             /* mm5 &= 0xfefefefefefefefe     */

                pcmpeqb (mm5, mm4);            /* is mm0 == 0 ?                 */
                psubb  (mm7, mm5);             /* mm7 has running total of eqcnts */

                movq   (mm2, eax);           /* mm2 = *p_data                 */
                movq   (mm0, mm3);             /* mm0 = mm3                     */

                movq   (mm3, mm2);             /* mm3 = *p_data                 */
                movq   (mm1, mm0);             /* mm1 = mm0                     */

                psubusb (mm0, mm2);            /* mm0 -= mm2                    */
                eax+=stride;                   /* p_data += stride              */

                psubusb (mm2, mm1);            /* mm2 -= mm1                    */
                por    (mm0, mm2);             /* mm0 |= mm2                    */

                pand   (mm0, mm6);             /* mm0 &= 0xfefefefefefefefe     */
                pcmpeqb (mm0, mm4);            /* is mm0 == 0 ?                 */

                movq   (mm2, eax);           /* mm2 = *p_data                 */
                psubb  (mm7, mm0);             /* mm7 has running total of eqcnts */

                movq   (mm5, mm3);             /* mm5 = mm3                     */
                movq   (mm3, mm2);             /* mm3 = *p_data                 */

                movq   (mm1, mm5);             /* mm1 = mm5                     */
                psubusb (mm5, mm2);            /* mm5 -= mm2                    */

                psubusb (mm2, mm1);            /* mm2 -= mm1                    */
                por    (mm5, mm2);             /* mm5 |= mm2                    */

                eax+=stride;           /* p_data += stride              */
                pand   (mm5, mm6);             /* mm5 &= 0xfefefefefefefefe     */

                pcmpeqb (mm5, mm4);            /* is mm0 == 0 ?                 */
                psubb  (mm7, mm5);             /* mm7 has running total of eqcnts */

                movq   (mm2, eax);           /* mm2 = *p_data                 */
                movq   (mm0, mm3);             /* mm0 = mm3                     */

                movq   (mm3, mm2);             /* mm3 = *p_data                 */
                movq   (mm1, mm0);             /* mm1 = mm0                     */

                psubusb (mm0, mm2);            /* mm0 -= mm2                    */
                eax+=stride;           /* p_data += stride              */

                psubusb (mm2, mm1);            /* mm2 -= mm1                    */
                por    (mm0, mm2);             /* mm0 |= mm2                    */

                pand   (mm0, mm6);             /* mm0 &= 0xfefefefefefefefe     */
                pcmpeqb (mm0, mm4);            /* is mm0 == 0 ?                 */

                movq   (mm2, eax);           /* mm2 = *p_data                 */
                psubb  (mm7, mm0);             /* mm7 has running total of eqcnts */

                movq   (mm5, mm3);             /* mm5 = mm3                     */
                movq   (mm3, mm2);             /* mm3 = *p_data                 */

                movq   (mm1, mm5);             /* mm1 = mm5                     */
                psubusb (mm5, mm2);            /* mm5 -= mm2                    */

                psubusb (mm2, mm1);            /* mm2 -= mm1                    */
                por    (mm5, mm2);             /* mm5 |= mm2                    */

                eax+=stride;           /* p_data += stride              */
                pand   (mm5, mm6);             /* mm5 &= 0xfefefefefefefefe     */

                pcmpeqb (mm5, mm4);            /* is mm0 == 0 ?                 */
                psubb  (mm7, mm5);             /* mm7 has running total of eqcnts */

                movq   (mm2, eax);           /* mm2 = *p_data                 */
                movq   (mm0, mm3);             /* mm0 = mm3                     */

                movq   (mm3, mm2);             /* mm3 = *p_data                 */
                movq   (mm1, mm0);             /* mm1 = mm0                     */

                psubusb (mm0, mm2);            /* mm0 -= mm2                    */
                eax+=stride;           /* p_data += stride              */

                psubusb (mm2, mm1);            /* mm2 -= mm1                    */
                por    (mm0, mm2);             /* mm0 |= mm2                    */

                pand   (mm0, mm6);             /* mm0 &= 0xfefefefefefefefe     */
                pcmpeqb (mm0, mm4);            /* is mm0 == 0 ?                 */

                psubb  (mm7, mm0);             /* mm7 has running total of eqcnts */

                //pop eax

        /* now mm7 contains negative eq_cnt for all 8-columns */
        /* copy this to mm_data1                              */
        /* sum all 8 bytes in mm7 */
                movq    (mm1, mm7);            /* mm1 = mm7             0 1w2 3 4 5 6 7r   */
                psrlq   (mm7, 32);             /* mm7 >>= 32            0 1 2 3 4 5 6 7m   */

                paddb   (mm7, mm1);            /* mm7 has running total of eqcnts */

                movq (mm1, mm7);               /* mm1 = mm7             0 1w2 3 4 5 6 7r   */
                psrlq   (mm7, 16);             /* mm7 >>= 16            0 1 2 3 4 5 6 7m   */

                paddb   (mm1, mm7);            /* mm7 has running total of eqcnts */

                movq (mm7, mm1);               /* mm1 = mm7             0 1w2 3 4 5 6 7r   */
                psrlq   (mm7, 8);              /* mm7 >>= 8             0 1 2 3 4 5 6 7m   */

                paddb   (mm7, mm1);            /* mm7 has running total of eqcnts */

                movd (mm_data1, mm7);          /* mm_data1 = mm7       */

        eq_cnt = mm_data1 & 0xff;

        useDC = (eq_cnt  > DEBLOCK_VERT_USEDC_THR);

        #ifdef PP_SELF_CHECK
        if (useDC != useDC2) DPRINTF(_l("ERROR: MMX version of useDC is incorrect"));
        #endif

        return useDC;
}

/* decide whether the DC filter should be turned on accoding to QP */
static inline int deblock_vert_DC_on(uint8_t *v, stride_t stride, int QP)
{
 __m64 QP_x_2=_mm_set1_pi32(0x02020202*QP);
 for (int i=0; i<5; i++)
  {
   //if (abs(v[i+1*stride]-v[i+8*stride]) > 2 *QP) DC_on2 = 0;
   /*
   if (abs(v[i+0*stride]-v[i+5*stride]) >= 2*QP) return false;
   if (abs(v[i+1*stride]-v[i+4*stride]) >= 2*QP) return false;
   if (abs(v[i+1*stride]-v[i+8*stride]) >= 2*QP) return false;
   if (abs(v[i+2*stride]-v[i+7*stride]) >= 2*QP) return false;
   if (abs(v[i+3*stride]-v[i+6*stride]) >= 2*QP) return false;
   */
   static const int v1[]={0,1,1,2,3},v2[]={5,4,8,7,6};
   uint8_t *ptr1=&(v[v1[i]*stride]);
   uint8_t *ptr2=&(v[v2[i]*stride]);
   __m64 mm0=*(__m64*)ptr1,mm1=mm0;
   __m64 mm2=*(__m64*)ptr2;
   mm0=_mm_subs_pu8(mm0,mm2);
   mm2=_mm_subs_pu8(mm2,mm1);
   mm0=_mm_or_si64(mm0,mm2);
   mm0=_mm_subs_pu8(mm0,QP_x_2);
   mm1=mm0;
   mm0=_mm_srli_si64(mm0,32);
   mm0=_mm_or_si64(mm0,mm1);
   int DC_on=_mm_cvtsi64_si32(mm0);
   if (DC_on) return 0;
  }
 return 1;
}

/* function using MMX to copy an 8-pixel wide column and unpack to 16-bit values */
/* n is the number of rows to copy - this must be even */
static inline void deblock_vert_copy_and_unpack(stride_t stride, uint8_t *source, uint64_t *dest, int n)
{
        uint64_t *pmm1 = (uint64_t *)source;
        uint64_t *pmm2 = (uint64_t *)dest;
        int i = -n / 2;

        #ifdef PP_SELF_CHECK
        int j, k;
        #endif

        /* copy block to local store whilst unpacking to 16-bit values */
        unsigned char *eax=(unsigned char*)pmm1;
        unsigned char *ebx=(unsigned char*)pmm2;

        __m64 mm7=_mm_setzero_si64();                   /* set mm7 = 0                     */
        __m64 mm0,mm1,mm2,mm3;
        for (;i<0;i++)
         {
          movq   (mm0, eax);                     /* mm0 = v[0*stride]               */

          #ifdef PREFETCH_ENABLE
          prefetcht0 0[ebx]
          #endif

          eax+= stride;                    /* p_data += stride                */
          movq   (mm1, mm0);                        /* mm1 = v[0*stride]               */
          punpcklbw (mm0, mm7);                     /* unpack low bytes (left hand 4)  */

          movq   (mm2, eax);                     /* mm2 = v[0*stride]               */
          punpckhbw (mm1, mm7);                     /* unpack high bytes (right hand 4)*/

          movq   (mm3, mm2);                        /* mm3 = v[0*stride]               */
          punpcklbw (mm2, mm7);                     /* unpack low bytes (left hand 4)  */

          movq   (ebx, mm0);                     /* v_local[n] = mm0 (left)         */
          eax+= stride;                    /* p_data += stride                */

          movq   (8+ebx, mm1);                    /* v_local[n+8] = mm1 (right)      */
          punpckhbw (mm3, mm7);                     /* unpack high bytes (right hand 4)*/

          movq   (16+ebx, mm2);                   /* v_local[n+16] = mm2 (left)      */

          movq   (24+ebx, mm3);                   /* v_local[n+24] = mm3 (right)     */

          ebx+= 32;                        /* p_data2 += 8                    */
         }


        #ifdef PP_SELF_CHECK
        /* check that MMX copy has worked correctly */
        for (k=0; k<n; k++)
        {
                for (j=0; j<8; j++)
                {
                        if ( ((uint16_t *)dest)[k*8+j] != source[k*stride+j] )
                        {
                                DPRINTF(_l("ERROR: MMX copy block is flawed at (%d, %d)"), j, k);
                        }
                }
        }
        #endif

}

/* This function chooses the "endstops" for the vertial LPF9 filter: p1 and p2 */
/* We also convert these to 16-bit values here */
static inline void deblock_vert_choose_p1p2(uint8_t *v, stride_t stride, uint64_t *p1p2, int QP)
{
        uint64_t *pmm1, *pmm2;
        __m64 mm_b_qp = _mm_set1_pi32(0x01010101 * QP);

        #ifdef PP_SELF_CHECK
        int i;
        #endif

        pmm1 = (uint64_t *)(&(v[0*stride]));
        pmm2 = (uint64_t *)(&(v[8*stride]));


                unsigned char *eax= (unsigned char*)pmm1;
                unsigned char *ebx= (unsigned char*)pmm2;
                unsigned char *ecx= (unsigned char*)p1p2;

                /* p1 */
                __m64 mm7,mm0,mm2,mm1,mm3;
                pxor     (mm7, mm7);             /* mm7 = 0                       */
                movq     (mm0, eax);           /* mm0 = *pmm1 = v[l0]           */
                movq     (mm2, mm0);             /* mm2 = mm0 = v[l0]             */
                eax+=stride;          /* pmm1 += stride                */
                movq     (mm1, eax);           /* mm1 = *pmm1 = v[l1]           */
                movq     (mm3, mm1);             /* mm3 = mm1 = v[l1]             */
                psubusb  (mm0, mm1);             /* mm0 -= mm1                    */
                psubusb  (mm1, mm2);             /* mm1 -= mm2                    */
                por      (mm0, mm1);             /* mm0 |= mm1                    */
                psubusb  (mm0, mm_b_qp);         /* mm0 -= QP                     */

                /* now a zero byte in mm0 indicates use v0 else use v1         */
                pcmpeqb  (mm0, mm7);             /* zero bytes to ff others to 00 */
                movq     (mm1, mm0);             /* make a copy of mm0            */

                /* now ff byte in mm0 indicates use v0 else use v1             */
                pandn    (mm0, mm3);             /* mask v1 into 00 bytes in mm0  */
                pand     (mm1, mm2);             /* mask v0 into ff bytes in mm0  */
                por      (mm0, mm1);             /* mm0 |= mm1                    */
                movq     (mm1, mm0);             /* make a copy of mm0            */

                /* Now we have our result, p1, in mm0.  Next, unpack.          */
                punpcklbw (mm0, mm7);            /* low bytes to mm0              */
                punpckhbw (mm1, mm7);            /* high bytes to mm1             */

                /* Store p1 in memory                                          */
                movq     (ecx, mm0);           /* low words to p1p2[0]          */
                movq     (8+ecx, mm1);          /* high words to p1p2[1]         */

                /* p2 */
                movq     (mm1, ebx);           /* mm1 = *pmm2 = v[l8]           */
                movq     (mm3, mm1);             /* mm3 = mm1 = v[l8]             */
                ebx+=stride;                      /* pmm2 += stride                */
                movq     (mm0, ebx);           /* mm0 = *pmm2 = v[l9]           */
                movq     (mm2, mm0);            /* mm2 = mm0 = v[l9]             */
                psubusb  (mm0, mm1);            /* mm0 -= mm1                    */
                psubusb  (mm1, mm2);            /* mm1 -= mm2                    */
                por      (mm0, mm1);            /* mm0 |= mm1                    */
                psubusb  (mm0, mm_b_qp);        /* mm0 -= QP                     */

                /* now a zero byte in mm0 indicates use v0 else use v1              */
                pcmpeqb  (mm0, mm7);             /* zero bytes to ff others to 00 */
                movq     (mm1, mm0);             /* make a copy of mm0            */

                /* now ff byte in mm0 indicates use v0 else use v1                  */
                pandn    (mm0, mm3);             /* mask v1 into 00 bytes in mm0  */
                pand     (mm1, mm2);             /* mask v0 into ff bytes in mm0  */
                por      (mm0, mm1);             /* mm0 |= mm1                    */
                movq     (mm1, mm0);             /* make a copy of mm0            */

                /* Now we have our result, p2, in mm0.  Next, unpack.               */
                punpcklbw (mm0, mm7);            /* low bytes to mm0              */
                punpckhbw (mm1, mm7);            /* high bytes to mm1             */

                /* Store p2 in memory                                               */
                movq     (16+ecx, mm0);         /* low words to p1p2[2]          */
                movq     (24+ecx, mm1);         /* high words to p1p2[3]         */


        #ifdef PP_SELF_CHECK
        /* check p1 and p2 have been calculated correctly */
        /* p2 */
        for (i=0; i<8; i++)
        {
                if ( ((abs(v[9*stride+i] - v[8*stride+i]) - QP > 0) ? v[8*stride+i] : v[9*stride+i])
                     != ((uint16_t *)(&(p1p2[2])))[i] )
                {
                         DPRINTF(_l("ERROR: problem with P2"));
                }
        }

        /* p1 */
        for (i=0; i<8; i++)
        {
                if ( ((abs(v[0*stride+i] - v[1*stride+i]) - QP > 0) ? v[1*stride+i] : v[0*stride+i])
                     != ((uint16_t *)(&(p1p2[0])))[i] )
                {
                         DPRINTF(_l("ERROR: problem with P1"));
                }
        }
        #endif

}

/* Vertical 9-tap low-pass filter for use in "DC" regions of the picture */
static inline void deblock_vert_lpf9(uint64_t *v_local, uint64_t *p1p2, uint8_t *v, stride_t stride)
{
__m64 mm_fours  = _mm_set1_pi16(0x0004);

        #ifdef PP_SELF_CHECK
        int j, k;
        uint8_t selfcheck[64], *vv;
        int p1, p2, psum;
        /* define semi-constants to enable us to move up and down the picture easily... */
        int l1 = 1 * stride;
        int l2 = 2 * stride;
        int l3 = 3 * stride;
        int l4 = 4 * stride;
        int l5 = 5 * stride;
        int l6 = 6 * stride;
        int l7 = 7 * stride;
        int l8 = 8 * stride;
        #endif


        #ifdef PP_SELF_CHECK
        /* generate a self-check version of the filter result in selfcheck[64] */
        /* loop left->right */
        for (j=0; j<8; j++)
        {
                vv = &(v[j]);
                p1 = ((uint16_t *)(&(p1p2[0+j/4])))[j%4]; /* yuck! */
                p2 = ((uint16_t *)(&(p1p2[2+j/4])))[j%4]; /* yuck! */
                /* the above may well be endian-fussy */
                psum = p1 + p1 + p1 + vv[l1] + vv[l2] + vv[l3] + vv[l4] + 4;
                selfcheck[j+8*0] = (((psum + vv[l1]) << 1) - (vv[l4] - vv[l5])) >> 4;
                psum += vv[l5] - p1;
                selfcheck[j+8*1] = (((psum + vv[l2]) << 1) - (vv[l5] - vv[l6])) >> 4;
                psum += vv[l6] - p1;
                selfcheck[j+8*2] = (((psum + vv[l3]) << 1) - (vv[l6] - vv[l7])) >> 4;
                psum += vv[l7] - p1;
                selfcheck[j+8*3] = (((psum + vv[l4]) << 1) + p1 - vv[l1] - (vv[l7] - vv[l8])) >> 4;
                psum += vv[l8] - vv[l1];
                selfcheck[j+8*4] = (((psum + vv[l5]) << 1) + (vv[l1] - vv[l2]) - vv[l8] + p2) >> 4;
                psum += p2 - vv[l2];
                selfcheck[j+8*5] = (((psum + vv[l6]) << 1) + (vv[l2] - vv[l3])) >> 4;
                psum += p2 - vv[l3];
                selfcheck[j+8*6] = (((psum + vv[l7]) << 1) + (vv[l3] - vv[l4])) >> 4;
                psum += p2 - vv[l4];
                selfcheck[j+8*7] = (((psum + vv[l8]) << 1) + (vv[l4] - vv[l5])) >> 4;
        }
        #endif

        /* vertical DC filter in MMX
                mm2 - p1/2 left
                mm3 - p1/2 right
                mm4 - psum left
                mm5 - psum right */
        /* alternate between using mm0/mm1 and mm6/mm7 to accumlate left/right */

                //push eax
                //push ebx
                //push ecx

                unsigned char *eax=(unsigned char*)p1p2;
                unsigned char *ebx=(unsigned char*)v_local;
                unsigned char *ecx=(unsigned char*)v;

                /* load p1 left into mm2 and p1 right into mm3 */
                __m64 mm2;
                movq  ( mm2, eax);                  /* mm2 = p1p2[0]               0 1 2w3 4 5 6 7    */
                ecx+= stride;//                  /* ecx points at v[1*stride]   0 1 2 3 4 5 6 7    */
                __m64 mm3;
                movq   (mm3, 8+eax);                 /* mm3 = p1p2[1]               0 1 2 3w4 5 6 7    */
                __m64 mm4;
                movq   (mm4, mm_fours);               /* mm4 = 0x0004000400040004    0 1 2 3 4w5 6 7    */

                /* psum = p1 + p1 + p1 + vv[1] + vv[2] + vv[3] + vv[4] + 4 */
                /* psum left will be in mm4, right in mm5          */
                __m64 mm5;
                movq   (mm5, mm4);                     /* mm5 = 0x0004000400040004    0 1 2 3 4 5w6 7    */

                paddsw (mm4, 16+ebx);                 /* mm4 += vv[1] left           0 1 2 3 4m5 6 7    */
                paddw  (mm5, mm3);                     /* mm5 += p2 left              0 1 2 3r4 5m6 7    */

                paddsw (mm4, 32+ebx);                 /* mm4 += vv[2] left           0 1 2 3 4m5 6 7    */
                paddw  (mm5, mm3);                     /* mm5 += p2 left              0 1 2 3r4 5m6 7    */

                paddsw (mm4, 48+ebx);                 /* mm4 += vv[3] left           0 1 2 3 4m5 6 7    */
                paddw  (mm5, mm3);                     /* mm5 += p2 left              0 1 2 3r4 5m6 7    */

                paddsw (mm5, 24+ebx);                 /* mm5 += vv[1] right          0 1 2 3 4 5m6 7    */
                paddw  (mm4, mm2);                     /* mm4 += p1 left              0 1 2r3 4m5 6 7    */

                paddsw (mm5, 40+ebx);                 /* mm5 += vv[2] right          0 1 2 3 4 5m6 7    */
                paddw  (mm4, mm2);                     /* mm4 += p1 left              0 1 2r3 4m5 6 7    */

                paddsw (mm5, 56+ebx);                 /* mm5 += vv[3] right          0 1 2 3 4 5m6 7    */
                paddw  (mm4, mm2);                     /* mm4 += p1 left              0 1 2r3 4m5 6 7    */

                paddsw (mm4, 64+ebx);                 /* mm4 += vv[4] left           0 1 2 3 4m5 6 7    */

                paddsw (mm5, 72+ebx);                 /* mm5 += vv[4] right          0 1 2 3 4 5m6 7    */

                /* v[1] = (((psum + vv[1]) << 1) - (vv[4] - vv[5])) >> 4 */
                /* compute this in mm0 (left) and mm1 (right)   */
                __m64 mm0,mm1;
                movq   (mm0, mm4);                     /* mm0 = psum left             0w1 2 3 4 5 6 7    */

                paddsw (mm0, 16+ebx);                 /* mm0 += vv[1] left           0m1 2 3 4 5 6 7    */
                movq   (mm1, mm5);                     /* mm1 = psum right            0 1w2 3 4 5r6 7    */

                paddsw (mm1, 24+ebx);                 /* mm1 += vv[1] right          0 1 2 3 4 5 6 7    */
                psllw  (mm0, 1);                       /* mm0 <<= 1                   0m1 2 3 4 5 6 7    */

                psubsw (mm0, 64+ebx);                 /* mm0 -= vv[4] left           0m1 2 3 4 5 6 7    */
                psllw  (mm1, 1);                       /* mm1 <<= 1                   0 1 2 3 4 5 6 7    */

                psubsw (mm1, 72+ebx);                 /* mm1 -= vv[4] right          0 1m2 3 4 5 6 7    */

                paddsw (mm0, 80+ebx);                 /* mm0 += vv[5] left           0m1 2 3 4 5 6 7    */

                paddsw (mm1, 88+ebx);                 /* mm1 += vv[5] right          0 1m2 3 4 5 6 7    */
                psrlw  (mm0, 4);                       /* mm0 >>= 4                   0m1 2 3 4 5 6 7    */

                /* psum += vv[5] - p1 */
                paddsw (mm4, 80+ebx);                 /* mm4 += vv[5] left           0 1 2 3 4m5 6 7    */
                psrlw  (mm1, 4);                       /* mm1 >>= 4                   0 1m2 3 4 5 6 7    */

                paddsw (mm5, 88+ebx);                 /* mm5 += vv[5] right          0 1 2 3 4 5 6 7    */
                psubsw (mm4, eax);                  /* mm4 -= p1 left              0 1 2 3 4 5 6 7    */

                packuswb (mm0, mm1);                   /* pack mm1, mm0 to mm0        0m1 2 3 4 5 6 7    */
                psubsw (mm5, 8+eax);                 /* mm5 -= p1 right             0 1 2 3 4 5 6 7    */

                /* v[2] = (((psum + vv[2]) << 1) - (vv[5] - vv[6])) >> 4 */
                /* compute this in mm6 (left) and mm7 (right)   */
                __m64 mm6,mm7;
                movq   (mm6, mm4);                     /* mm6 = psum left             0 1 2 3 4 5 6 7    */

                paddsw (mm6, 32+ebx);                 /* mm6 += vv[2] left           0 1 2 3 4 5 6 7    */
                movq   (mm7, mm5);                     /* mm7 = psum right            0 1 2 3 4 5 6 7    */

                paddsw (mm7, 40+ebx);                 /* mm7 += vv[2] right          0 1 2 3 4 5 6 7    */
                psllw  (mm6, 1);                       /* mm6 <<= 1                   0 1 2 3 4 5 6 7    */

                psubsw (mm6, 80+ebx);                 /* mm6 -= vv[5] left           0 1 2 3 4 5 6 7    */
                psllw  (mm7, 1);                       /* mm7 <<= 1                   0 1 2 3 4 5 6 7    */

                psubsw (mm7, 88+ebx);                 /* mm7 -= vv[5] right          0 1 2 3 4 5 6 7    */

                movq   (ecx, mm0);                     /* v[1*stride] = mm0           0 1 2 3 4 5 6 7    */

                paddsw (mm6, 96+ebx);                 /* mm6 += vv[6] left           0 1 2 3 4 5 6 7    */
                ecx+=stride;                          /* ecx points at v[2*stride]   0 1 2 3 4 5 6 7    */

                paddsw (mm7, 104+ebx);                /* mm7 += vv[6] right          0 1 2 3 4 5 6 7    */

                /* psum += vv[6] - p1 */

                paddsw (mm4, 96+ebx);                 /* mm4 += vv[6] left           0 1 2 3 4 5 6 7    */
                psrlw  (mm6, 4);                       /* mm6 >>= 4                   0 1 2 3 4 5 6 7    */

                paddsw (mm5, 104+ebx);                /* mm5 += vv[6] right          0 1 2 3 4 5 6 7    */
                psrlw  (mm7, 4);                       /* mm7 >>= 4                   0 1 2 3 4 5 6 7    */

                psubsw (mm4, eax);                   /* mm4 -= p1 left              0 1 2 3 4 5 6 7    */
                packuswb (mm6, mm7);                   /* pack mm7, mm6 to mm6        0 1 2 3 4 5 6 7    */

                psubsw (mm5, 8+eax);                  /* mm5 -= p1 right             0 1 2 3 4 5 6 7    */

                /* v[3] = (((psum + vv[3]) << 1) - (vv[6] - vv[7])) >> 4 */
                /* compute this in mm0 (left) and mm1 (right)    */

                movq   (mm0, mm4);                     /* mm0 = psum left             0 1 2 3 4 5 6 7    */

                paddsw (mm0, 48+ebx);                 /* mm0 += vv[3] left           0 1 2 3 4 5 6 7    */
                movq   (mm1, mm5);                     /* mm1 = psum right            0 1 2 3 4 5 6 7    */

                paddsw (mm1, 56+ebx);                 /* mm1 += vv[3] right          0 1 2 3 4 5 6 7    */
                psllw  (mm0, 1);                       /* mm0 <<= 1                   0 1 2 3 4 5 6 7    */

                psubsw (mm0, 96+ebx);                 /* mm0 -= vv[6] left           0 1 2 3 4 5 6 7    */
                psllw  (mm1, 1);                       /* mm1 <<= 1                   0 1 2 3 4 5 6 7    */

                psubsw (mm1, 104+ebx);                /* mm1 -= vv[6] right          0 1 2 3 4 5 6 7    */

                movq   (ecx, mm6);                   /* v[2*stride] = mm6           0 1 2 3 4 5 6 7    */
                paddsw (mm0, 112+ebx);                /* mm0 += vv[7] left           0 1 2 3 4 5 6 7    */

                paddsw (mm1, 120+ebx);                /* mm1 += vv[7] right          0 1 2 3 4 5 6 7    */
                ecx+=stride;                         /* ecx points at v[3*stride]   0 1 2 3 4 5 6 7    */

                /* psum += vv[7] - p1 */
                paddsw (mm4, 112+ebx);                /* mm4 += vv[5] left           0 1 2 3 4 5 6 7    */
                psrlw  (mm0, 4);                       /* mm0 >>= 4                   0 1 2 3 4 5 6 7    */

                paddsw (mm5, 120+ebx);                /* mm5 += vv[5] right          0 1 2 3 4 5 6 7    */
                psrlw  (mm1, 4);                       /* mm1 >>= 4                   0 1 2 3 4 5 6 7    */

                psubsw (mm4, eax);                   /* mm4 -= p1 left              0 1 2 3 4 5 6 7    */
                packuswb( mm0, mm1);                   /* pack mm1, mm0 to mm0        0 1 2 3 4 5 6 7    */

                psubsw (mm5, 8+eax);                  /* mm5 -= p1 right             0 1 2 3 4 5 6 7    */

                /* v[4] = (((psum + vv[4]) << 1) + p1 - vv[1] - (vv[7] - vv[8])) >> 4 */
                /* compute this in mm6 (left) and mm7 (right)    */
                movq   (ecx, mm0);                   /* v[3*stride] = mm0           0 1 2 3 4 5 6 7    */
                movq   (mm6, mm4);                     /* mm6 = psum left             0 1 2 3 4 5 6 7    */

                paddsw (mm6, 64+ebx);                 /* mm6 += vv[4] left           0 1 2 3 4 5 6 7    */
                movq   (mm7, mm5);                     /* mm7 = psum right            0 1 2 3 4 5 6 7    */

                paddsw (mm7, 72+ebx);                 /* mm7 += vv[4] right          0 1 2 3 4 5 6 7    */
                psllw  (mm6, 1);                       /* mm6 <<= 1                   0 1 2 3 4 5 6 7    */

                paddsw (mm6, eax);                   /* mm6 += p1 left              0 1 2 3 4 5 6 7    */
                psllw  (mm7, 1);                       /* mm7 <<= 1                   0 1 2 3 4 5 6 7    */

                paddsw (mm7, 8+eax);                  /* mm7 += p1 right             0 1 2 3 4 5 6 7    */

                psubsw (mm6, 16+ebx);                 /* mm6 -= vv[1] left           0 1 2 3 4 5 6 7    */

                psubsw (mm7, 24+ebx);                 /* mm7 -= vv[1] right          0 1 2 3 4 5 6 7    */

                psubsw (mm6, 112+ebx);                /* mm6 -= vv[7] left           0 1 2 3 4 5 6 7    */

                psubsw (mm7, 120+ebx);                /* mm7 -= vv[7] right          0 1 2 3 4 5 6 7    */

                paddsw (mm6, 128+ebx);                /* mm6 += vv[8] left           0 1 2 3 4 5 6 7    */
                ecx+=stride;                  /* ecx points at v[4*stride]   0 1 2 3 4 5 6 7    */

                paddsw (mm7, 136+ebx);                /* mm7 += vv[8] right          0 1 2 3 4 5 6 7    */
                /* psum += vv[8] - vv[1] */

                paddsw (mm4, 128+ebx);                /* mm4 += vv[5] left           0 1 2 3 4 5 6 7    */
                psrlw  (mm6, 4);                       /* mm6 >>= 4                   0 1 2 3 4 5 6 7    */

                paddsw (mm5, 136+ebx);                /* mm5 += vv[5] right          0 1 2 3 4 5 6 7    */
                psrlw  (mm7, 4);                       /* mm7 >>= 4                   0 1 2 3 4 5 6 7    */

                psubsw (mm4, 16+ebx);                 /* mm4 -= vv[1] left           0 1 2 3 4 5 6 7    */
                packuswb (mm6, mm7);                   /* pack mm7, mm6 to mm6        0 1 2 3 4 5 6 7    */

                psubsw (mm5, 24+ebx);                 /* mm5 -= vv[1] right          0 1 2 3 4 5 6 7    */

                /* v[5] = (((psum + vv[5]) << 1) + (vv[1] - vv[2]) - vv[8] + p2) >> 4 */
                /* compute this in mm0 (left) and mm1 (right)    */
                movq   (mm0, mm4);                     /* mm0 = psum left             0 1 2 3 4 5 6 7    */

                paddsw (mm0, 80+ebx);                 /* mm0 += vv[5] left           0 1 2 3 4 5 6 7    */
                movq   (mm1, mm5);                     /* mm1 = psum right            0 1 2 3 4 5 6 7    */

                paddsw (mm1, 88+ebx);                 /* mm1 += vv[5] right          0 1 2 3 4 5 6 7    */
                psllw  (mm0, 1);                       /* mm0 <<= 1                   0 1 2 3 4 5 6 7    */

                paddsw (mm0, 16+eax);                 /* mm0 += p2 left              0 1 2 3 4 5 6 7    */
                psllw  (mm1, 1);                       /* mm1 <<= 1                   0 1 2 3 4 5 6 7    */

                paddsw (mm1, 24+eax);                 /* mm1 += p2 right             0 1 2 3 4 5 6 7    */

                paddsw (mm0, 16+ebx);                 /* mm0 += vv[1] left           0 1 2 3 4 5 6 7    */
                movq   (ecx, mm6);                   /* v[4*stride] = mm6           0 1 2 3 4 5 6 7    */

                paddsw (mm1, 24+ebx);                 /* mm1 += vv[1] right          0 1 2 3 4 5 6 7    */

                psubsw (mm0, 32+ebx);                 /* mm0 -= vv[2] left           0 1 2 3 4 5 6 7    */

                psubsw (mm1, 40+ebx);                 /* mm1 -= vv[2] right          0 1 2 3 4 5 6 7    */

                psubsw (mm0, 128+ebx);                /* mm0 -= vv[8] left           0 1 2 3 4 5 6 7    */

                psubsw (mm1, 136+ebx);                /* mm1 -= vv[8] right          0 1 2 3 4 5 6 7    */

                /* psum += p2 - vv[2] */
                paddsw (mm4, 16+eax);                 /* mm4 += p2 left              0 1 2 3 4 5 6 7    */
                ecx+=stride;                  /* ecx points at v[5*stride]   0 1 2 3 4 5 6 7    */

                paddsw (mm5, 24+eax);                 /* mm5 += p2 right             0 1 2 3 4 5 6 7    */

                psubsw (mm4, 32+ebx);                 /* mm4 -= vv[2] left           0 1 2 3 4 5 6 7    */

                psubsw (mm5, 40+ebx);                 /* mm5 -= vv[2] right          0 1 2 3 4 5 6 7    */

                /* v[6] = (((psum + vv[6]) << 1) + (vv[2] - vv[3])) >> 4 */
                /* compute this in mm6 (left) and mm7 (right)    */

                movq   (mm6, mm4);                     /* mm6 = psum left             0 1 2 3 4 5 6 7    */

                paddsw (mm6, 96+ebx);                 /* mm6 += vv[6] left           0 1 2 3 4 5 6 7    */
                movq   (mm7, mm5);                     /* mm7 = psum right            0 1 2 3 4 5 6 7    */

                paddsw (mm7, 104+ebx);                /* mm7 += vv[6] right          0 1 2 3 4 5 6 7    */
                psllw  (mm6, 1);                       /* mm6 <<= 1                   0 1 2 3 4 5 6 7    */

                paddsw (mm6, 32+ebx);                 /* mm6 += vv[2] left           0 1 2 3 4 5 6 7    */
                psllw  (mm7, 1);                       /* mm7 <<= 1                   0 1 2 3 4 5 6 7    */

                paddsw (mm7, 40+ebx);                 /* mm7 += vv[2] right          0 1 2 3 4 5 6 7    */
                psrlw  (mm0, 4);                       /* mm0 >>= 4                   0 1 2 3 4 5 6 7    */

                psubsw (mm6, 48+ebx);                 /* mm6 -= vv[3] left           0 1 2 3 4 5 6 7    */
                psrlw  (mm1, 4);                       /* mm1 >>= 4                   0 1 2 3 4 5 6 7    */

                psubsw (mm7, 56+ebx);                 /* mm7 -= vv[3] right          0 1 2 3 4 5 6 7    */
                packuswb (mm0, mm1);                   /* pack mm1, mm0 to mm0        0 1 2 3 4 5 6 7    */

                movq   (ecx, mm0);                   /* v[5*stride] = mm0           0 1 2 3 4 5 6 7    */

                /* psum += p2 - vv[3] */

                paddsw (mm4, 16+eax);                 /* mm4 += p2 left              0 1 2 3 4 5 6 7    */
                psrlw  (mm6, 4);                       /* mm6 >>= 4                   0 1 2 3 4 5 6 7    */

                paddsw (mm5, 24+eax);                 /* mm5 += p2 right             0 1 2 3 4 5 6 7    */
                psrlw  (mm7, 4);                       /* mm7 >>= 4                   0 1 2 3 4 5 6 7    */

                psubsw (mm4, 48+ebx);                 /* mm4 -= vv[3] left           0 1 2 3 4 5 6 7    */
                ecx+=stride;                   /* ecx points at v[6*stride]   0 1 2 3 4 5 6 7    */

                psubsw (mm5, 56+ebx);                 /* mm5 -= vv[3] right           0 1 2 3 4 5 6 7    */

                /* v[7] = (((psum + vv[7]) << 1) + (vv[3] - vv[4])) >> 4 */
                /* compute this in mm0 (left) and mm1 (right)     */

                movq   (mm0, mm4);                     /* mm0 = psum left             0 1 2 3 4 5 6 7    */

                paddsw (mm0, 112+ebx);                /* mm0 += vv[7] left           0 1 2 3 4 5 6 7    */
                movq   (mm1, mm5);                     /* mm1 = psum right            0 1 2 3 4 5 6 7    */

                paddsw (mm1, 120+ebx);                /* mm1 += vv[7] right          0 1 2 3 4 5 6 7    */
                psllw  (mm0, 1);                       /* mm0 <<= 1                   0 1 2 3 4 5 6 7    */

                paddsw (mm0, 48+ebx);                 /* mm0 += vv[3] left           0 1 2 3 4 5 6 7    */
                psllw  (mm1, 1);                       /* mm1 <<= 1                   0 1 2 3 4 5 6 7    */

                paddsw (mm1, 56+ebx);                 /* mm1 += vv[3] right          0 1 2 3 4 5 6 7    */
                packuswb( mm6, mm7);                   /* pack mm7, mm6 to mm6        0 1 2 3 4 5 6 7    */

                psubsw (mm0, 64+ebx);                 /* mm0 -= vv[4] left           0 1 2 3 4 5 6 7    */

                psubsw (mm1, 72+ebx);                 /* mm1 -= vv[4] right          0 1 2 3 4 5 6 7    */
                psrlw  (mm0, 4);                       /* mm0 >>= 4                   0 1 2 3 4 5 6 7    */

                movq   (ecx, mm6);                   /* v[6*stride] = mm6           0 1 2 3 4 5 6 7    */

                /* psum += p2 - vv[4] */

                paddsw (mm4, 16+eax);                 /* mm4 += p2 left               0 1 2 3 4 5 6 7    */

                paddsw (mm5, 24+eax);                 /* mm5 += p2 right              0 1 2 3 4 5 6 7    */
                ecx+=stride;                  /* ecx points at v[7*stride]   0 1 2 3 4 5 6 7    */

                psubsw (mm4, 64+ebx);                 /* mm4 -= vv[4] left            0 1 2 3 4 5 6 7    */
                psrlw  (mm1, 4);                       /* mm1 >>= 4                   0 1 2 3 4 5 6 7    */

                psubsw (mm5, 72+ebx);                 /* mm5 -= vv[4] right 0 1 2 3 4 5 6 7    */

                /* v[8] = (((psum + vv[8]) << 1) + (vv[4] - vv[5])) >> 4 */
                /* compute this in mm6 (left) and mm7 (right)     */

                movq   (mm6, mm4);                     /* mm6 = psum left             0 1 2 3 4 5 6 7    */

                paddsw (mm6, 128+ebx);                /* mm6 += vv[8] left           0 1 2 3 4 5 6 7    */
                movq   (mm7, mm5);                     /* mm7 = psum right            0 1 2 3 4 5 6 7    */

                paddsw (mm7, 136+ebx);                /* mm7 += vv[8] right          0 1 2 3 4 5 6 7    */
                psllw  (mm6, 1);                       /* mm6 <<= 1                   0 1 2 3 4 5 6 7    */

                paddsw (mm6, 64+ebx);                 /* mm6 += vv[4] left           0 1 2 3 4 5 6 7    */
                psllw  (mm7, 1);                       /* mm7 <<= 1                   0 1 2 3 4 5 6 7    */

                paddsw (mm7, 72+ebx);                 /* mm7 += vv[4] right          0 1 2 3 4 5 6 7    */
                packuswb (mm0, mm1);                  /* pack mm1, mm0 to mm0        0 1 2 3 4 5 6 7    */

                psubsw (mm6, 80+ebx);                 /* mm6 -= vv[5] left           0 1 2 3 4 5 6 7    */

                psubsw (mm7, 88+ebx);                 /* mm7 -= vv[5] right          0 1 2 3 4 5 6 7    */
                psrlw  (mm6, 4);                       /* mm6 >>= 4                   0 1 2 3 4 5 6 7    */

                movq   (ecx, mm0);                   /* v[7*stride] = mm0           0 1 2 3 4 5 6 7    */
                psrlw  (mm7, 4);                       /* mm7 >>= 4                   0 1 2 3 4 5 6 7    */

                packuswb( mm6, mm7);                   /* pack mm7, mm6 to mm6        0 1 2 3 4 5 6 7    */

                ecx+=stride;                   /* ecx points at v[8*stride]   0 1 2 3 4 5 6 7    */

                //nop                                 /*                             0 1 2 3 4 5 6 7    */

                movq   (ecx, mm6);                   /* v[8*stride] = mm6           0 1 2 3 4 5 6 7    */


        #ifdef PP_SELF_CHECK
        /* use the self-check version of the filter result in selfcheck[64] to verify the filter output */
        /* loop top->bottom */
        for (k=0; k<8; k++)
        {   /* loop left->right */
                for (j=0; j<8; j++)
                {
                        vv = &(v[(k+1)*stride + j]);
                        if (*vv != selfcheck[j+8*k])
                        {
                                DPRINTF(_l("ERROR: problem with vertical LPF9 filter in row %d"), k+1);
                        }
                }
        }
        #endif

}

/* Vertical deblocking filter for use in non-flat picture regions */
static inline void deblock_vert_default_filter(uint8_t *v, stride_t stride, int QP)
{
        uint64_t *pmm1;
        int i;

        #ifdef PP_SELF_CHECK
        /* define semi-constants to enable us to move up and down the picture easily... */
        int l1 = 1 * stride;
        int l2 = 2 * stride;
        int l3 = 3 * stride;
        int l4 = 4 * stride;
        int l5 = 5 * stride;
        int l6 = 6 * stride;
        int l7 = 7 * stride;
        int l8 = 8 * stride;
        int x, y, a3_0_SC, a3_1_SC, a3_2_SC, d_SC, q_SC;
        uint8_t selfcheck[8][2];
        #endif

        #ifdef PP_SELF_CHECK
        /* compute selfcheck matrix for later comparison */
        for (x=0; x<8; x++)
        {
                a3_0_SC = 2*v[l3+x] - 5*v[l4+x] + 5*v[l5+x] - 2*v[l6+x];
                a3_1_SC = 2*v[l1+x] - 5*v[l2+x] + 5*v[l3+x] - 2*v[l4+x];
                a3_2_SC = 2*v[l5+x] - 5*v[l6+x] + 5*v[l7+x] - 2*v[l8+x];
                q_SC    = (v[l4+x] - v[l5+x]) / 2;

                if (abs(a3_0_SC) < 8*QP)
                {

                        d_SC = abs(a3_0_SC) - std::min(abs(a3_1_SC), abs(a3_2_SC));
                        if (d_SC < 0) d_SC=0;

                        d_SC = (5*d_SC + 32) >> 6;
                        d_SC *= SIGN(-a3_0_SC);

                        //printf("d_SC[%d] preclip=%d\n", x, d_SC);
                        /* clip d in the range 0 ... q */
                        if (q_SC > 0)
                        {
                                d_SC = d_SC<0    ? 0    : d_SC;
                                d_SC = d_SC>q_SC ? q_SC : d_SC;
                        }
                        else
                        {
                                d_SC = d_SC>0    ? 0    : d_SC;
                                d_SC = d_SC<q_SC ? q_SC : d_SC;
                        }
                }
                else
                {
                        d_SC = 0;
                }
                selfcheck[x][0] = v[l4+x] - d_SC;
                selfcheck[x][1] = v[l5+x] + d_SC;
        }
        #endif

        __m64 mm_8_x_QP=_mm_set1_pi32(0x00080008 * QP);
        __m64 mm_0020 = _mm_set1_pi16(0x0020);

        /* working in 4-pixel wide columns, left to right */
        /*i=0 in left, i=1 in right */
        __m64 mm7,mm0,mm1,mm2,mm3,mm4,mm5,mm6;
        for (i=0; i<2; i++)
        {
                /* v should be 64-bit aligned here */
                pmm1 = (uint64_t *)(&(v[4*i]));
                /* pmm1 will be 32-bit aligned but this doesn't matter as we'll use movd not movq */

                        unsigned char *ecx=(unsigned char*)pmm1;

                        pxor      (mm7, mm7);               /* mm7 = 0000000000000000    0 1 2 3 4 5 6 7w   */
                        ecx+=stride;                        /* %0 += stride              0 1 2 3 4 5 6 7    */

                        movd      (mm0, ecx);            /* mm0 = v1v1v1v1v1v1v1v1    0w1 2 3 4 5 6 7    */
                        punpcklbw (mm0, mm7);               /* mm0 = __v1__v1__v1__v1 L  0m1 2 3 4 5 6 7r   */

                        ecx+= stride;                   /* ecx += stride              0 1 2 3 4 5 6 7    */
                        movd      (mm1, ecx);            /* mm1 = v2v2v2v2v2v2v2v2    0 1w2 3 4 5 6 7    */

                        ecx+= stride;           /* ecx += stride              0 1 2 3 4 5 6 7    */
                        punpcklbw (mm1, mm7);               /* mm1 = __v2__v2__v2__v2 L  0 1m2 3 4 5 6 7r   */

                        movd      (mm2, ecx);            /* mm2 = v3v3v3v3v3v3v3v3    0 1 2w3 4 5 6 7    */
                        ecx+= stride;           /* ecx += stride              0 1 2 3 4 5 6 7    */

                        punpcklbw (mm2, mm7);               /* mm2 = __v3__v3__v3__v3 L  0 1 2m3 4 5 6 7r   */

                        movd      (mm3, ecx);            /* mm3 = v4v4v4v4v4v4v4v4    0 1 2 3w4 5 6 7    */

                        punpcklbw (mm3, mm7);               /* mm3 = __v4__v4__v4__v4 L  0 1 2 3m4 5 6 7r   */

                        psubw     (mm1, mm2);               /* mm1 = v2 - v3          L  0 1m2r3 4 5 6 7    */

                        movq      (mm4, mm1);               /* mm4 = v2 - v3          L  0 1r2 3 4w5 6 7    */
                        psllw     (mm1, 2);                 /* mm1 = 4 * (v2 - v3)    L  0 1m2 3 4 5 6 7    */

                        paddw     (mm1, mm4);               /* mm1 = 5 * (v2 - v3)    L  0 1m2 3 4r5 6 7    */
                        psubw     (mm0, mm3);               /* mm0 = v1 - v4          L  0m1 2 3r4 5 6 7    */

                        psllw     (mm0, 1);                 /* mm0 = 2 * (v1 - v4)    L  0m1 2 3 4 5 6 7    */

                        psubw     (mm0, mm1);               /* mm0 = a3_1             L  0m1r2 3 4 5 6 7    */

                        pxor      (mm1, mm1);               /* mm1 = 0000000000000000    0 1w2 3 4 5 6 7    */

                        pcmpgtw   (mm1, mm0);               /* is 0 > a3_1 ?          L  0r1m2 3 4 5 6 7    */

                        ecx+= stride;           /* ecx += stride              0 1 2 3 4 5 6 7    */
                        pxor      (mm0, mm1);               /* mm0 = abs(a3_1) step 1 L  0m1r2 3 4 5 6 7    */

                        psubw     (mm0, mm1);               /* mm0 = abs(a3_1) step 2 L  0m1r2 3 4 5 6 7    */

                        movd      (mm1, ecx);            /* mm1 = v5v5v5v5v5v5v5v5    0 1w2 3 4 5 6 7    */

                        punpcklbw (mm1, mm7);               /* mm1 = __v5__v5__v5__v5 L  0 1m2 3 4 5 6 7r   */


                        ecx+= stride;           /* ecx += stride              0 1 2 3 4 5 6 7    */
                        psubw     (mm3, mm1);               /* mm3 = v4 - v5          L  0 1r2 3m4 5 6 7    */

                        movd      (mm4, ecx);            /* mm4 = v6v6v6v6v6v6v6v6    0 1 2 3 4w5 6 7    */

                        punpcklbw (mm4, mm7);               /* mm4 = __v6__v6__v6__v6 L  0 1 2 3 4m5 6 7r   */

                        ecx+= stride;           /* ecx += stride              0 1 2 3 4 5 6 7    */

                        movd      (mm5, ecx);            /* mm5 = v7v7v7v7v7v7v7v7    0 1 2 3 4 5w6 7    */
                        psubw     (mm2, mm4);               /* mm2 = v3 - v6          L  0 1 2m3 4r5 6 7    */

                        punpcklbw (mm5, mm7);               /* mm5 = __v7__v7__v7__v7 L  0 1 2 3 4 5m6 7r   */

                        ecx+= stride;           /* ecx += stride              0 1 2 3 4 5 6 7    */
                        psubw     (mm5, mm4);               /* mm5 = v7 - v6          L  0 1 2 3 4r5m6 7    */

                        movq      (mm4, mm5);               /* mm4 = v7 - v6          L  0 1 2 3 4w5r6 7    */

                        psllw     (mm4, 2);                 /* mm4 = 4 * (v7 - v6)    L  0 1 2 3 4 5m6 7    */

                        paddw     (mm5, mm4);               /* mm5 = 5 * (v7 - v6)    L  0 1 2 3 4r5m6 7    */

                        movd      (mm4, ecx);            /* mm4 = v8v8v8v8v8v8v8v8    0 1 2 3 4w5 6 7    */

                        punpcklbw (mm4, mm7);               /* mm4 = __v8__v8__v8__v8 L  0 1 2 3 4m5 6 7r   */

                        psubw     (mm1, mm4);               /* mm1 = v5 - v8          L  0 1m2 3 4r5 6 7    */

                        pxor      (mm4, mm4);               /* mm4 = 0000000000000000    0 1 2 3 4w5 6 7    */
                        psllw     (mm1, 1);                 /* mm1 = 2 * (v5 - v8)    L  0 1m2 3 4 5 6 7    */

                        paddw     (mm1, mm5);               /* mm1 = a3_2             L  0 1m2 3 4 5r6 7    */

                        pcmpgtw   (mm4, mm1);               /* is 0 > a3_2 ?          L  0 1r2 3 4m5 6 7    */

                        pxor      (mm1, mm4);               /* mm1 = abs(a3_2) step 1 L  0 1m2 3 4r5 6 7    */

                        psubw     (mm1, mm4);               /* mm1 = abs(a3_2) step 2 L  0 1m2 3 4r5 6 7    */

                        /* at this point, mm0 = abs(a3_1), mm1 = abs(a3_2), mm2 = v3 - v6, mm3 = v4 - v5 */
                        movq      (mm4, mm1);               /* mm4 = abs(a3_2)        L  0 1r2 3 4w5 6 7    */

                        pcmpgtw   (mm1, mm0);               /* is abs(a3_2) > abs(a3_1)  0r1m2 3 4 5 6 7    */

                        pand      (mm0, mm1);               /* min() step 1           L  0m1r2 3 4 5 6 7    */

                        pandn     (mm1, mm4);               /* min() step 2           L  0 1m2 3 4r5 6 7    */

                        por       (mm0, mm1);               /* min() step 3           L  0m1r2 3 4 5 6 7    */

                        /* at this point, mm0 = MIN( abs(a3_1), abs(a3_2), mm2 = v3 - v6, mm3 = v4 - v5 */
                        movq      (mm1, mm3);               /* mm1 = v4 - v5          L  0 1w2 3r4 5 6 7    */
                        psllw     (mm3, 2);                 /* mm3 = 4 * (v4 - v5)    L  0 1 2 3m4 5 6 7    */

                        paddw     (mm3, mm1);               /* mm3 = 5 * (v4 - v5)    L  0 1r2 3m4 5 6 7    */
                        psllw     (mm2, 1);                 /* mm2 = 2 * (v3 - v6)    L  0 1 2m3 4 5 6 7    */

                        psubw     (mm2, mm3);               /* mm2 = a3_0             L  0 1 2m3r4 5 6 7    */

                        /* at this point, mm0 = MIN( abs(a3_1), abs(a3_2), mm1 = v4 - v5, mm2 = a3_0 */
                        movq      (mm4, mm2);               /* mm4 = a3_0             L  0 1 2r3 4w5 6 7    */
                        pxor      (mm3, mm3);               /* mm3 = 0000000000000000    0 1 2 3w4 5 6 7    */

                        pcmpgtw   (mm3, mm2);               /* is 0 > a3_0 ?          L  0 1 2r3m4 5 6 7    */

                        movq      (mm2, mm_8_x_QP);         /* mm4 = 8*QP                0 1 2w3 4 5 6 7    */
                        pxor      (mm4, mm3);               /* mm4 = abs(a3_0) step 1 L  0 1 2 3r4m5 6 7    */

                        psubw     (mm4, mm3);               /* mm4 = abs(a3_0) step 2 L  0 1 2 3r4m5 6 7    */

                        /* compare a3_0 against 8*QP */
                        pcmpgtw   (mm2, mm4);               /* is 8*QP > abs(d) ?     L  0 1 2m3 4r5 6 7    */

                        pand      (mm2, mm4);               /* if no, d = 0           L  0 1 2m3 4r5 6 7    */

                        movq      (mm4, mm2);               /* mm2 = a3_0             L  0 1 2r3 4w5 6 7    */

                        /* at this point, mm0 = MIN( abs(a3_1), abs(a3_2), mm1 = v4 - v5, mm2 = a3_0 , mm3 = SGN(a3_0), mm4 = abs(a3_0) */
                        psubusw   (mm4, mm0);               /* mm0 = (A3_0 - a3_0)    L  0r1 2 3 4m5 6 7    */

                        movq      (mm0, mm4);               /* mm0=abs(d)             L  0w1 2 3 4r5 6 7    */

                        psllw     (mm0, 2);                 /* mm0 = 4 * (A3_0-a3_0)  L  0m1 2 3 4 5 6 7    */

                        paddw     (mm0, mm4);               /* mm0 = 5 * (A3_0-a3_0)  L  0m1 2 3 4r5 6 7    */

                        paddw     (mm0, mm_0020);           /* mm0 += 32              L  0m1 2 3 4 5 6 7    */

                        psraw     (mm0, 6);                 /* mm0 >>= 6              L  0m1 2 3 4 5 6 7    */

                        /* at this point, mm0 = abs(d), mm1 = v4 - v5, mm3 = SGN(a3_0) */
                        pxor      (mm2, mm2);               /* mm2 = 0000000000000000    0 1 2w3 4 5 6 7    */

                        pcmpgtw   (mm2, mm1);               /* is 0 > (v4 - v5) ?     L  0 1r2m3 4 5 6 7    */

                        pxor      (mm1, mm2);               /* mm1 = abs(mm1) step 1  L  0 1m2r3 4 5 6 7    */

                        psubw     (mm1, mm2);               /* mm1 = abs(mm1) step 2  L  0 1m2r3 4 5 6 7    */

                        psraw     (mm1, 1);                 /* mm1 >>= 2              L  0 1m2 3 4 5 6 7    */

                        /* OK at this point, mm0 = abs(d), mm1 = abs(q), mm2 = SGN(q), mm3 = SGN(-d) */
                        movq      (mm4, mm2);               /* mm4 = SGN(q)           L  0 1 2r3 4w5 6 7    */

                        pxor      (mm4, mm3);               /* mm4 = SGN(q) ^ SGN(-d) L  0 1 2 3r4m5 6 7    */

                        movq      (mm5, mm0);               /* mm5 = abs(d)           L  0r1 2 3 4 5w6 7    */

                        pcmpgtw   (mm5, mm1);               /* is abs(d) > abs(q) ?   L  0 1r2 3 4 5m6 7    */

                        pand      (mm1, mm5);               /* min() step 1           L  0m1 2 3 4 5r6 7    */

                        pandn     (mm5, mm0);               /* min() step 2           L  0 1r2 3 4 5m6 7    */

                        por       (mm1, mm5);               /* min() step 3           L  0m1 2 3 4 5r6 7    */

                        pand      (mm1, mm4);               /* if signs differ, set 0 L  0m1 2 3 4r5 6 7    */

                        pxor      (mm1, mm2);               /* Apply sign step 1      L  0m1 2r3 4 5 6 7    */

                        psubw     (mm1, mm2);               /* Apply sign step 2      L  0m1 2r3 4 5 6 7    */

                        /* at this point we have d in mm1 */

                if (i==0)
                {
                        movq (mm6, mm1);
                }

        }

        /* add d to rows l4 and l5 in memory... */
        pmm1 = (uint64_t *)(&(v[4*stride]));
        unsigned char *ecx=(unsigned char*) pmm1;
                packsswb  (mm6, mm1);
                movq      (mm0, ecx);
                psubb     (mm0, mm6);
                movq      (ecx, mm0);
                ecx+=stride;                    /* %0 += stride              0 1 2 3 4 5 6 7    */
                paddb     (mm6, ecx);
                movq      (ecx, mm6);

        #ifdef PP_SELF_CHECK
        /* do selfcheck */
        for (x=0; x<8; x++)
        {
                for (y=0; y<2; y++)
                {
                        if (selfcheck[x][y] != v[l4+x+y*stride])
                        {
                                DPRINTF(_l("ERROR: problem with vertical default filter in col %d, row %d"), x, y);
                                DPRINTF(_l("%d should be %d"), v[l4+x+y*stride], selfcheck[x][y]);

                        }
                }
        }
        #endif
}

/* this is a vertical deblocking filter - i.e. it will smooth _horizontal_ block edges */
static void deblock_vert( uint8_t *image, int width, stride_t stride, const int8_t *QP_store, int QP_stride, int chromaFlag, int DEBLOCK_VERT_USEDC_THR)
{
 uint64_t v_local[20];
 uint64_t p1p2[4];
 int Bx, x, y;
 int QP;
 uint8_t *v;
 int useDC, DC_on;

 #ifdef PREFETCH_AHEAD_V
 void *prefetch_addr;
 #endif

 y = 0;

 /* loop over all blocks, left to right */
 for (Bx=0; Bx<width; Bx+=8)
  {
   QP = chromaFlag ? QP_store[y/8*QP_stride+Bx/8]
                   : QP_store[y/16*QP_stride+Bx/16];
          v = &(image[y*stride + Bx]) - 5*stride;

   #ifdef PREFETCH_AHEAD_V
   /* try a prefetch PREFETCH_AHEAD_V bytes ahead on all eight rows... experimental */
   prefetch_addr = v + PREFETCH_AHEAD_V;
   _mm_prefetch(prefetch_addr+1*stride,_MM_HINT_T0);
   _mm_prefetch(prefetch_addr+2*stride,_MM_HINT_T0);
   _mm_prefetch(prefetch_addr+3*stride,_MM_HINT_T0);
   _mm_prefetch(prefetch_addr+4*stride,_MM_HINT_T0);
   _mm_prefetch(prefetch_addr+5*stride,_MM_HINT_T0);
   _mm_prefetch(prefetch_addr+6*stride,_MM_HINT_T0);
   _mm_prefetch(prefetch_addr+7*stride,_MM_HINT_T0);
   _mm_prefetch(prefetch_addr+8*stride,_MM_HINT_T0);
   #endif

   /* decide whether to use DC mode on a block-by-block basis */
   useDC = deblock_vert_useDC(v, stride,DEBLOCK_VERT_USEDC_THR);
   if (useDC)
    {
     /* we are in DC mode for this block.  But we only want to filter low-energy areas */

     /* decide whether the filter should be on or off for this block */
     DC_on = deblock_vert_DC_on(v, stride, QP);

     /* use DC offset mode */
     if (DC_on)
      {
       v = &(image[y*stride + Bx])- 5*stride;

       /* copy the block we're working on and unpack to 16-bit values */
       deblock_vert_copy_and_unpack(stride, &(v[stride]), &(v_local[2]), 8);

       deblock_vert_choose_p1p2(v, stride, p1p2, QP);

       deblock_vert_lpf9(v_local, p1p2, v, stride);

       #ifdef SHOWDECISIONS_V
       if (!chromaFlag)
        {
         v[4*stride  ] =
         v[4*stride+1] =
         v[4*stride+2] =
         v[4*stride+3] =
         v[4*stride+4] =
         v[4*stride+5] =
         v[4*stride+6] =
         v[4*stride+7] = 255;
        }
       #endif
      }
    }
   else /* use the default filter */
    {
     ///* loop over every column of pixels crossing that horizontal boundary */
     //for (dx=0; dx<8; dx++) {
     x = Bx;// + dx;
     v = &(image[y*stride + x])- 5*stride;
     deblock_vert_default_filter(v, stride, QP);
     //}
     #ifdef SHOWDECISIONS_V
     if (!chromaFlag)
      {
       v[4*stride  ] =
       v[4*stride+1] =
       v[4*stride+2] =
       v[4*stride+3] =
       v[4*stride+4] =
       v[4*stride+5] =
       v[4*stride+6] =
       v[4*stride+7] = 0;
      }
     #endif
    }
  }
}

/* entry point for MMX postprocessing */
void nic_postprocess(unsigned char * dst[],    stride_t dst_stride[4],
                     unsigned int chromaShiftX,unsigned int chromaShiftY,
                     int horizontal_size,      int vertical_size,
                     const int8_t *QP_store,   int QP_stride,
                     int mode,
                     int DEBLOCK_HORIZ_USEDC_THR, int DEBLOCK_VERT_USEDC_THR)
{
 uint8_t *puc_flt;
 const int8_t *QP_ptr;
 int y, i;

 /* this loop is (hopefully) going to improve performance */
 /* loop down the picture, copying and processing in vertical stripes, each four pixels high */
 for (y=0; y<vertical_size; y+= 4)
  {
   if (mode & LUM_H_DEBLOCK)
    {
     puc_flt = &((dst[0])[y*dst_stride[0]]);
     QP_ptr  = &(QP_store[(y>>4)*QP_stride]);
     deblock_horiz(puc_flt, horizontal_size,      dst_stride[0], QP_ptr, QP_stride, 0, DEBLOCK_HORIZ_USEDC_THR);
    }

   if (mode & LUM_V_DEBLOCK)
    {
     if ( (y%8) && (y-4)>5 )
      {
       puc_flt = &((dst[0])[(y-4)*dst_stride[0]]);
       QP_ptr  = &(QP_store[(y>>4)*QP_stride]);
       deblock_vert( puc_flt, horizontal_size,   dst_stride[0], QP_ptr, QP_stride, 0, DEBLOCK_VERT_USEDC_THR);
      }
    }
  } /* for loop */

 /* now we're going to do U and V */
 horizontal_size >>= chromaShiftX;
 vertical_size   >>= chromaShiftY;

 /* loop U then V */
 for (i=1; i<=2; i++)
  {
   for (y=0; y<vertical_size-3; y+= 4)
    {
     if (mode & CHROM_H_DEBLOCK)
      {
       puc_flt = &((dst[i])[y*dst_stride[i]]);
       QP_ptr  = &(QP_store[(y>>3)*QP_stride]);
       deblock_horiz(puc_flt, horizontal_size,      dst_stride[i], QP_ptr, QP_stride, 1, DEBLOCK_HORIZ_USEDC_THR);
      }

     if (mode & CHROM_V_DEBLOCK)
      {
       if ( (y%8) && (y-4)>5 )
        {
         puc_flt = &((dst[i])[(y-4)*dst_stride[i]]);
         QP_ptr  = &(QP_store[(y>>4)*QP_stride]);
         deblock_vert( puc_flt, horizontal_size,   dst_stride[i], QP_ptr, QP_stride, 1,DEBLOCK_VERT_USEDC_THR);
        }
      }
    } /* stripe loop */
  } /* U,V loop */

 _mm_empty();
}

/* this is the deringing filter */

template<class Tsimd> struct Tnic_dering
{
 static void nic_dering( uint8_t *image, int width, int height, stride_t stride, const int8_t *QP_store, int QP_stride, unsigned int chromaShiftX,unsigned int chromaShiftY) {
        int x, y;
        uint8_t *b8x8, *b10x10;
        uint8_t b8x8filtered[64];
        int QP, max_diff;
        uint8_t min, max, thr;
        uint8_t max_diffq[8];   // a qword value of max_diff

        /* loop over all the 8x8 blocks in the image... */
        /* don't process outer row of blocks for the time being. */
        __m64 mm0,mm1,mm2,mm7,mm3,mm4,mm5,mm6;
        for (y=8; y<height-8; y+=8)
        {
                for (x=8; x< width-8; x+=8)
                {
                        /* QP for this block.. */
                        /*
                        QP = chroma ? QP_store[(y>>3)*QP_stride+(x>>3)] // Nick: QP_store[y/8*QP_stride+x/8]
                                    : QP_store[(y>>4)*QP_stride+(x>>4)]; //Nick: QP_store[y/16*QP_stride+x/16];
                        */
                        QP = QP_store[(y>>(4-chromaShiftY))*QP_stride+(x>>(4-chromaShiftX))];
                        /* pointer to the top left pixel in 8x8   block */
                        b8x8   = &(image[stride*y + x]);

                        /* pointer to the top left pixel in 10x10 block */
                        b10x10 = &(image[stride*(y-1) + (x-1)]);

                        // Threshold determination - find min and max grey levels in the block
                        // but remove loop thru 64 bytes.  trbarry 03/13/2002

                        unsigned char *esi=b8x8;               // the block addr
                        stride_t            eax=stride;             // offset to next qword in block

                        movq    (mm0, esi); // row 0, 1st qword is our min
                        movq    (mm1, mm0);                        // .. and our max

                        Tsimd::pminub  (mm0, esi+eax);         // row 1
                        Tsimd::pmaxub  (mm1, esi+eax);

                        esi= esi+2*eax;                        // bump for next 2
                        Tsimd::pminub  (mm0, esi);                     // row 2
                        Tsimd::pmaxub  (mm1, esi);

                        Tsimd::pminub  (mm0, esi+eax);         // row 3
                        Tsimd::pmaxub  (mm1, esi+eax);

                        esi= esi+2*eax;                        // bump for next 2
                        Tsimd::pminub  (mm0, esi);                     // row 4
                        Tsimd::pmaxub  (mm1, esi);

                        Tsimd::pminub  (mm0, esi+eax);         // row 5
                        Tsimd::pmaxub  (mm1, esi+eax);

                        esi= esi+2*eax;                        // bump for next 2
                        Tsimd::pminub  (mm0, esi);                     // row 6
                        Tsimd::pmaxub  (mm1, esi);

                        Tsimd::pminub  (mm0, esi+eax);         // row 7
                        Tsimd::pmaxub  (mm1, esi+eax);

                        // get min of 8 bytes in mm0
                        mm2=Tsimd::_mm_shuffle_pi16_14(mm0);                  // words 3,2 into low words of mm2
                        Tsimd::pminub  (mm0, mm2);                                                // now 4 min bytes in low half of mm0
                        mm2=Tsimd::_mm_shuffle_pi16_1(mm0);                                 // get word 1 of mm0 in low word of mm2
                        Tsimd::pminub  (mm0, mm2);                                                // got it down to 2 bytes
                        movq    (mm2, mm0);
                        psrlq   (mm2, 8  );                                                // byte 1 to low byte
                        Tsimd::pminub  (mm0, mm2);                                                // our answer in low order byte
                        min= (unsigned char)_mm_cvtsi64_si32(mm0);                                                 // save answer

                        // get max of 8 bytes in mm1
                        mm2=Tsimd::_mm_shuffle_pi16_14(mm1);                  // words 3,2 into low words of mm2
                        Tsimd::pmaxub  (mm1, mm2);                                                // now 4 max bytes in low half of mm1
                        mm2=Tsimd::_mm_shuffle_pi16_1(mm1);                                 // get word 1 of mm1 in low word of mm2
                        Tsimd::pmaxub  (mm1, mm2);                                                // got it down to 2 bytes
                        movq    (mm2, mm1);
                        psrlq   (mm2, 8  );                                                // byte 1 to low byte
                        Tsimd::pmaxub  (mm1, mm2);                                                // our answer in low order byte
                        max= (unsigned char)_mm_cvtsi64_si32(mm1);                                                 // save answer

                        /* Threshold determination - compute threshold and dynamic range */
                        thr = uint8_t((max + min + 1) >> 1); // Nick / 2 changed to >> 1

                        max_diff = QP>>1;
                        max_diffq[0] = max_diffq[1] = max_diffq[2] = max_diffq[3]
                                = max_diffq[4] = max_diffq[5] = max_diffq[6] = max_diffq[7]
                                = uint8_t(max_diff);

                        /* To opimize in MMX it's better to always fill in the b8x8filtered[] array

                        b8x8filtered[8*v + h] = ( 8
                                + 1 * b10x10[stride*(v+0) + (h+0)] + 2 * b10x10[stride*(v+0) + (h+1)]
                                                                                                                + 1 * b10x10[stride*(v+0) + (h+2)]
                                + 2 * b10x10[stride*(v+1) + (h+0)] + 4 * b10x10[stride*(v+1) + (h+1)]
                                                                                                                + 2 * b10x10[stride*(v+1) + (h+2)]
                                + 1 * b10x10[stride*(v+2) + (h+0)] + 2 * b10x10[stride*(v+2) + (h+1)]
                                                                                                                + 1 * b10x10[stride*(v+2) + (h+2)]
                                         >> 4;  // Nick / 16 changed to >> 4

                        Note - As near as I can see, (a + 2*b + c)/4 = avg( avg(a,c), b) and likewise vertical. So since
                        there is a nice Tsimd::pavgb MMX instruction that gives a rounded vector average we may as well use it.
                        */
                        // Fill in b10x10 array completely with  2 dim center weighted average
                        // This section now also includes the clipping step.
                        // trbarry 03/14/2002

                        esi= b10x10;                                     // ptr to 10x10 source array
                        unsigned char *edi= b8x8filtered;                       // ptr to 8x8 output array
                        eax= stride;                                     // amt to bump source ptr for next row

                        movq    (mm7,esi);                     // this is really line -1
                        Tsimd::pavgb   (mm7,esi+2);           // avg( b[v,h], b[v,h+2] }
                        Tsimd::pavgb   (mm7,esi+1);       // center weighted avg of 3 pixels  w=1,2,1

                        esi= esi+eax;                          // bump src ptr to point at line 0
                        movq    (mm0, esi);                     // really line 0
                        Tsimd::pavgb   (mm0, esi+2);
                        movq    (mm2, esi+1);           // save orig line 0 vals for later clip
                        Tsimd::pavgb   (mm0, mm2);

                        movq    (mm1, esi+eax);         // get line 1
                        Tsimd::pavgb   (mm1, esi+eax+2);
                        movq    (mm3, esi+eax+1);       // save orig line 1 vals for later clip
                        Tsimd::pavgb   (mm1, mm3);

                        // 0
                        Tsimd::pavgb   (mm7, mm1);                                        // avg lines surrounding line 0
                        Tsimd::pavgb   (mm7, mm0);                                        // center weighted avg of lines -1,0,1

                        movq    (mm4, mm2);                                        // value for clip min
                        psubusb (mm4, max_diffq);                          // min
                        Tsimd::pmaxub  (mm7, mm4);                                        // must be at least min
                        paddusb (mm2, max_diffq);                          // max
                        Tsimd::pminub  (mm7, mm2);                                        // but no greater than max

                        movq    (edi, mm7);

                        esi= esi+2*eax;                        // bump source ptr 2 lines
                        movq    (mm2, esi);                     // get line 2
                        Tsimd::pavgb   (mm2, esi+2);
                        movq    (mm4, esi+1);           // save orig line 2 vals for later clip
                        Tsimd::pavgb   (mm2, mm4);

                        // 1
                        Tsimd::pavgb   (mm0, mm2);                                        // avg lines surrounding line 1
                        Tsimd::pavgb   (mm0, mm1);                                        // center weighted avg of lines 0,1,2

                        movq    (mm5, mm3);                                        // value for clip min
                        psubusb (mm5, max_diffq);                          // min
                        Tsimd::pmaxub  (mm0, mm5);                                        // must be at least min
                        paddusb (mm3, max_diffq);                          // max
                        Tsimd::pminub  (mm0, mm3);                                        // but no greater than max

                        movq    (edi+8, mm0);

                        movq    (mm3, esi+eax);         // get line 3
                        Tsimd::pavgb   (mm3, esi+eax+2);
                        movq    (mm5, esi+eax+1);       // save orig line 3 vals for later clip
                        Tsimd::pavgb   (mm3, mm5);

                        // 2
                        Tsimd::pavgb   (mm1, mm3);                                        // avg lines surrounding line 2
                        Tsimd::pavgb   (mm1, mm2);                                        // center weighted avg of lines 1,2,3

                        movq    (mm6, mm4);                                        // value for clip min
                        psubusb (mm6, max_diffq);                          // min
                        Tsimd::pmaxub  (mm1, mm6);                                        // must be at least min
                        paddusb (mm4, max_diffq);                          // max
                        Tsimd::pminub  (mm1, mm4);                                        // but no greater than max

                        movq    (edi+16, mm1);

                        esi= esi+2*eax;                        // bump source ptr 2 lines
                        movq    (mm4, esi);                     // get line 4
                        Tsimd::pavgb   (mm4, esi+2);
                        movq    (mm6, esi+1);           // save orig line 4 vals for later clip
                        Tsimd::pavgb   (mm4, mm6);

                        // 3
                        Tsimd::pavgb   (mm2, mm4);                                        // avg lines surrounding line 3
                        Tsimd::pavgb   (mm2, mm3);                                        // center weighted avg of lines 2,3,4

                        movq    (mm7, mm5);                                        // save value for clip min
                        psubusb (mm7, max_diffq);                          // min
                        Tsimd::pmaxub  (mm2, mm7);                                        // must be at least min
                        paddusb (mm5, max_diffq);                          // max
                        Tsimd::pminub  (mm2, mm5);                                        // but no greater than max

                        movq    (edi+24, mm2);

                        movq    (mm5, esi+eax);         // get line 5
                        Tsimd::pavgb   (mm5, esi+eax+2);
                        movq    (mm7, esi+eax+1);       // save orig line 5 vals for later clip
                        Tsimd::pavgb   (mm5, mm7);

                        // 4
                        Tsimd::pavgb   (mm3, mm5);                                        // avg lines surrounding line 4
                        Tsimd::pavgb   (mm3, mm4);                                        // center weighted avg of lines 3,4,5

                        movq    (mm0, mm6);                                        // save value for clip min
                        psubusb (mm0, max_diffq);                          // min
                        Tsimd::pmaxub  (mm3, mm0);                                        // must be at least min
                        paddusb (mm6, max_diffq);                          // max
                        Tsimd::pminub  (mm3, mm6);                                        // but no greater than max

                        movq    (edi+32, mm3);

                        esi= esi+2*eax;                        // bump source ptr 2 lines
                        movq    (mm6, esi);                     // get line 6
                        Tsimd::pavgb   (mm6, esi+2);
                        movq    (mm0, esi+1);           // save orig line 6 vals for later clip
                        Tsimd::pavgb   (mm6, mm0);

                        // 5
                        Tsimd::pavgb   (mm4, mm6);                                        // avg lines surrounding line 5
                        Tsimd::pavgb   (mm4, mm5);                                        // center weighted avg of lines 4,5,6

                        movq    (mm1, mm7);                                        // save value for clip min
                        psubusb (mm1, max_diffq);                          // min
                        Tsimd::pmaxub  (mm4, mm1);                                        // must be at least min
                        paddusb (mm7, max_diffq);                          // max
                        Tsimd::pminub  (mm4, mm7);                                        // but no greater than max

                        movq    (edi+40, mm4);

                        movq    (mm7, esi+eax);         // get line 7
                        Tsimd::pavgb   (mm7, esi+eax+2);
                        movq    (mm1, esi+eax+1);       // save orig line 7 vals for later clip
                        Tsimd::pavgb   (mm7, mm1);

                        // 6
                        Tsimd::pavgb   (mm5, mm7);                                        // avg lines surrounding line 6
                        Tsimd::pavgb   (mm5, mm6);                                        // center weighted avg of lines 5,6,7

                        movq    (mm2, mm0);                                        // save value for clip min
                        psubusb (mm2, max_diffq);                          // min
                        Tsimd::pmaxub  (mm5, mm2);                                        // must be at least min
                        paddusb (mm0, max_diffq);                          // max
                        Tsimd::pminub  (mm5, mm0);                                        // but no greater than max

                        movq    (edi+48, mm5);

                        movq    (mm0, esi+2*eax);       // get line 8
                        Tsimd::pavgb   (mm0, esi+2*eax+2);
                        Tsimd::pavgb   (mm0, esi+2*eax+1);

                        // 7
                        Tsimd::pavgb   (mm6, mm0);                                        // avg lines surrounding line 7
                        Tsimd::pavgb   (mm6, mm7);                                        // center weighted avg of lines 6,7,8

                        movq    (mm3, mm1);                                        // save value for clip min
                        psubusb (mm3, max_diffq);                          // min
                        Tsimd::pmaxub  (mm6, mm3);                                        // must be at least min
                        paddusb (mm1, max_diffq);                          // max
                        Tsimd::pminub  (mm6, mm1);                                        // but no greater than max

                        movq    (edi+56, mm6);

                        // trbarry 03-15-2002
                        // If I (hopefully) understand all this correctly then we are supposed to only use the filtered
                        // pixels created above in the case where the orig pixel and all its 8 surrounding neighbors
                        // are either all above or all below the threshold.  Since a mmx compare sets the mmx reg
                        // bytes to either 00 or ff we will just check a topside average of all 9 of these to see
                        // if it is still 00 or ff.
                        // register notes:
                        // esi  b10x10
                        // edi  b8x8filtered
                        // eax  stride
                        // mm0  line 0, 3, or 6   compare average
                        // mm1  line 1, 4, or 7                 "
                        // mm2  line 2, 5, 8, or -1             "
                        // mm3  odd line original pixel value
                        // mm4  even line original pixel value
                        // mm5
                        // mm6  8 00's
                        // mm7  8 copies of threshold

                        esi= b10x10;                                     // ptr to 10x10 source array
                        edi= b8x8filtered;                       // ptr to 8x8 output array
                        eax= stride;                                     // amt to bump source ptr for next row
                        //mov             bh, thr
                        //mov     bl, thr
                        psubusb (mm6, mm6);                                        // all 00
                        //movd    (mm7, eax); // ebx?
                        //mm7=Tsimd::_mm_shuffle_pi16_0(mm7);// low word to all words
                        mm7=_mm_set1_pi8(thr);

                        // get compare avg of row -1
                        movq    (mm3, esi+1);           // save center pixels of line -1

                        movq    (mm2, mm7);
                        psubusb (mm2, esi);                     // left pixels >= threshold?
                        pcmpeqb (mm2, mm6);                                        // ff if >= else 00

                        movq    (mm5, mm7);
                        psubusb (mm5, mm3);                                        // center pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm2, mm5);                                        // combine answers

                        movq    (mm5, mm7);
                        psubusb (mm5, esi+2);       // right pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm2, mm5);                                        // combine answers

                        esi= esi+eax;                          // &row 0

                        // get compare avg of row 0
                        movq    (mm4, esi+1);           // save center pixels

                        movq    (mm0, mm7);
                        psubusb (mm0, esi);                     // left pixels >= threshold?
                        pcmpeqb (mm0, mm6);                                        // ff if >= else 00

                        movq    (mm5, mm7);
                        psubusb (mm5, mm4);                                        // center pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm0, mm5);                                        // combine answers

                        movq    (mm5, mm7);
                        psubusb (mm5, esi+2);       // right pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm0, mm5);                                        // combine answers

                        // get compare avg of row 1

                        movq    (mm3, esi+eax+1);       // save center pixels

                        movq    (mm1, mm7);
                        psubusb (mm1, esi+eax);         // left pixels >= threshold?
                        pcmpeqb (mm1, mm6);                                        // ff if >= else 00

                        movq    (mm5, mm7);
                        psubusb (mm5, mm3);                                        // center pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm1, mm5);                                        // combine answers

                        movq    (mm5, mm7);
                        psubusb (mm5, esi+eax+2);   // right pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm1, mm5);                                        // combine answers

                        // decide whether to move filtered or orig pixels to row 0, move them

                        Tsimd::pavgb   (mm2, mm0);
                        Tsimd::pavgb   (mm2, mm1);
                        pcmpeqb (mm5, mm5);                                        // all ff
                        pcmpeqb (mm5, mm2);                                        // 00 or ff
                        pcmpeqb (mm5, mm2);                                        // mm2 = ff or 00 ? ff : 00
                        psubusb (mm2, mm2);                                        // 00
                        pcmpeqb (mm2, mm5);                                        // opposite of mm5
                        pand    (mm2, mm4);                                        // use orig vales if thresh's diff, else 00
                        pand    (mm5, edi);         // use filterd vales if thresh's same, else 00
                        por     (mm5, mm2);                                        // one of them hsa a value
                        movq    (esi+1, mm5);       // and store 8 pixels

                        esi=esi+eax;                          // &row 1

                        // get compare avg of row 2

                        movq    (mm4, esi+eax+1);       // save center pixels

                        movq    (mm2, mm7);
                        psubusb (mm2, esi+eax);         // left pixels >= threshold?
                        pcmpeqb (mm2, mm6);                                        // ff if >= else 00

                        movq    (mm5, mm7);
                        psubusb (mm5, mm4);                                        // center pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm2, mm5);                                        // combine answers

                        movq    (mm5, mm7);
                        psubusb (mm5, esi+eax+2);   // right pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm2, mm5);                                        // combine answers

                        // decide whether to move filtered or orig pixels to row 1, move them

                        Tsimd::pavgb   (mm0, mm1);
                        Tsimd::pavgb   (mm0, mm2);
                        pcmpeqb (mm5, mm5);                                        // all ff
                        pcmpeqb (mm5, mm0);                                        // 00 or ff
                        pcmpeqb (mm5, mm0);                                        // mm2 = ff or 00 ? ff : 00
                        psubusb (mm0, mm0);                                        // 00
                        pcmpeqb (mm0, mm5);                                        // opposite of mm5
                        pand    (mm0, mm3);                                        // use orig vales if thresh's diff, else 00
                        pand    (mm5, edi+1*8);     // use filterd vales if thresh's same, else 00
                        por     (mm5, mm0);                                        // one of them hsa a value
                        movq    (esi+1, mm5);           // and store 8 pixels

                        esi= esi+eax;                          // &row 2

                        // get compare avg of row 3

                        movq    (mm3, esi+eax+1);       // save center pixels

                        movq    (mm0, mm7);
                        psubusb (mm0, esi+eax);         // left pixels >= threshold?
                        pcmpeqb (mm0, mm6);                                        // ff if >= else 00

                        movq    (mm5, mm7);
                        psubusb (mm5, mm3);                                        // center pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm0, mm5);                                        // combine answers

                        movq    (mm5, mm7);
                        psubusb (mm5, esi+eax+2);   // right pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm0, mm5);                                        // combine answers

                        // decide whether to move filtered or orig pixels to row 2, move them

                        Tsimd::pavgb   (mm1, mm0);
                        Tsimd::pavgb   (mm1, mm2);
                        pcmpeqb (mm5, mm5);                                        // all ff
                        pcmpeqb (mm5, mm1);                                        // 00 or ff
                        pcmpeqb (mm5, mm1);                                        // mm2 = ff or 00 ? ff : 00
                        psubusb (mm1, mm1);                                        // 00
                        pcmpeqb (mm1, mm5);                                        // opposite of mm5
                        pand    (mm1, mm4);                                        // use orig vales if thresh's diff, else 00
                        pand    (mm5, edi+2*8);     // use filterd vales if thresh's same, else 00
                        por     (mm5, mm1);                                        // one of them hsa a value
                        movq    (esi+1, mm5);           // and store 8 pixels

                        esi= esi+eax;                          // &row 3

                        // get compare avg of row 4

                        movq    (mm4, esi+eax+1);       // save center pixels

                        movq    (mm1, mm7);
                        psubusb (mm1, esi+eax);         // left pixels >= threshold?
                        pcmpeqb (mm1, mm6);                                        // ff if >= else 00

                        movq    (mm5, mm7);
                        psubusb (mm5, mm4);                                        // center pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm1, mm5);                                        // combine answers

                        movq    (mm5, mm7);
                        psubusb (mm5, esi+eax+2);   // right pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm1, mm5);                                        // combine answers

                        // decide whether to move filtered or orig pixels to row 3, move them

                        Tsimd::pavgb   (mm2, mm0);
                        Tsimd::pavgb   (mm2, mm1);
                        pcmpeqb (mm5, mm5);                                        // all ff
                        pcmpeqb (mm5, mm2);                                        // 00 or ff
                        pcmpeqb (mm5, mm2);                                        // mm2 = ff or 00 ? ff : 00
                        psubusb (mm2, mm2);                                        // 00
                        pcmpeqb (mm2, mm5);                                        // opposite of mm5
                        pand    (mm2, mm3);                                        // use orig vales if thresh's diff, else 00
                        pand    (mm5, edi+3*8);     // use filterd vales if thresh's same, else 00
                        por     (mm5, mm2);                                        // one of them hsa a value
                        movq    (esi+1, mm5);           // and store 8 pixels

                        esi= esi+eax;                          // &row 4

                        // get compare avg of row 5

                        movq    (mm3, esi+eax+1);       // save center pixels

                        movq    (mm2, mm7);
                        psubusb (mm2, esi+eax);         // left pixels >= threshold?
                        pcmpeqb (mm2, mm6);                                        // ff if >= else 00

                        movq    (mm5, mm7);
                        psubusb (mm5, mm3);                                        // center pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm2, mm5);                                        // combine answers

                        movq    (mm5, mm7);
                        psubusb (mm5, esi+eax+2);   // right pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm2, mm5);                                        // combine answers

                        // decide whether to move filtered or orig pixels to row 4, move them

                        Tsimd::pavgb   (mm0, mm2);
                        Tsimd::pavgb   (mm0, mm1);
                        pcmpeqb (mm5, mm5);                                        // all ff
                        pcmpeqb (mm5, mm0);                                        // 00 or ff
                        pcmpeqb (mm5, mm0);                                        // mm2 = ff or 00 ? ff : 00
                        psubusb (mm0, mm0);                                        // 00
                        pcmpeqb (mm0, mm5);                                        // opposite of mm5
                        pand    (mm0, mm4);                                        // use orig vales if thresh's diff, else 00
                        pand    (mm5, edi+4*8);     // use filterd vales if thresh's same, else 00
                        por     (mm5, mm0);                                        // one of them hsa a value
                        movq    (esi+1, mm5);           // and store 8 pixels

                        esi= esi+eax;                          // &row 5

                        // get compare avg of row 6

                        movq    (mm4, esi+eax+1);       // save center pixels

                        movq    (mm0, mm7);
                        psubusb (mm0, esi+eax);         // left pixels >= threshold?
                        pcmpeqb (mm0, mm6);                                        // ff if >= else 00

                        movq    (mm5, mm7);
                        psubusb (mm5, mm4);                                        // center pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm0, mm5);                                        // combine answers

                        movq    (mm5, mm7);
                        psubusb (mm5, esi+eax+2);   // right pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm0, mm5);                                        // combine answers

                        // decide whether to move filtered or orig pixels to row 5, move them

                        Tsimd::pavgb   (mm1, mm0);
                        Tsimd::pavgb   (mm1, mm2);
                        pcmpeqb (mm5, mm5);                                        // all ff
                        pcmpeqb (mm5, mm1);                                        // 00 or ff
                        pcmpeqb (mm5, mm1);                                        // mm2 = ff or 00 ? ff : 00
                        psubusb (mm1, mm1);                                        // 00
                        pcmpeqb (mm1, mm5);                                        // opposite of mm5
                        pand    (mm1, mm3);                                        // use orig vales if thresh's diff, else 00
                        pand    (mm5, edi+5*8);     // use filterd vales if thresh's same, else 00
                        por     (mm5, mm1);                                        // one of them hsa a value
                        movq    (esi+1, mm5);           // and store 8 pixels

                        esi= esi+eax;                          // &row 6

                        // get compare avg of row 7

                        movq    (mm3, esi+eax+1);       // save center pixels

                        movq    (mm1, mm7);
                        psubusb (mm1, esi+eax);         // left pixels >= threshold?
                        pcmpeqb (mm1, mm6);                                        // ff if >= else 00

                        movq    (mm5, mm7);
                        psubusb (mm5, mm3);                                        // center pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm1, mm5);                                        // combine answers

                        movq    (mm5, mm7);
                        psubusb (mm5, esi+eax+2);   // right pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm1, mm5);                                        // combine answers

                        // decide whether to move filtered or orig pixels to row 6, move them

                        Tsimd::pavgb   (mm2, mm0);
                        Tsimd::pavgb   (mm2, mm1);
                        pcmpeqb (mm5, mm5);                                        // all ff
                        pcmpeqb (mm5, mm2);                                        // 00 or ff
                        pcmpeqb (mm5, mm2);                                        // mm2 = ff or 00 ? ff : 00
                        psubusb (mm2, mm2);                                        // 00
                        pcmpeqb (mm2, mm5);                                        // opposite of mm5
                        pand    (mm2, mm4);                                        // use orig vales if thresh's diff, else 00
                        pand    (mm5, edi+6*8);     // use filterd vales if thresh's same, else 00
                        por     (mm5, mm2);                                        // one of them hsa a value
                        movq    (esi+1, mm5);           // and store 8 pixels

                        esi=esi+eax;                          // &row 7

                        // get compare avg of row 8

                        movq    (mm4, esi+eax+1);       // save center pixels

                        movq    (mm2, mm7);
                        psubusb (mm2, esi+eax);         // left pixels >= threshold?
                        pcmpeqb (mm2, mm6);                                        // ff if >= else 00

                        movq    (mm5, mm7);
                        psubusb (mm5, mm4);                                        // center pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm2, mm5);                                        // combine answers

                        movq    (mm5, mm7);
                        psubusb (mm5, esi+eax+2);   // right pixels >= threshold?
                        pcmpeqb (mm5, mm6);
                        Tsimd::pavgb   (mm2, mm5);                                        // combine answers

                        // decide whether to move filtered or orig pixels to row 7, move them

                        Tsimd::pavgb   (mm0, mm1);
                        Tsimd::pavgb   (mm0, mm2);
                        pcmpeqb (mm5, mm5);                                        // all ff
                        pcmpeqb (mm5, mm0);                                        // 00 or ff
                        pcmpeqb (mm5, mm0);                                        // mm2 = ff or 00 ? ff : 00
                        psubusb (mm0, mm0);                                        // 00
                        pcmpeqb (mm0, mm5);                                        // opposite of mm5
                        pand    (mm0, mm3);                                        // use orig vales if thresh's diff, else 00
                        pand    (mm5, edi+7*8);     // use filterd vales if thresh's same, else 00
                        por     (mm5, mm0);                                        // one of them hsa a value
                        movq    (esi+1, mm5);           // and store 8 pixels
                }
        }
        _mm_empty();
  }
};

#pragma warning(pop)

Tnic_deringFc getNic_dering(void)
{
 return Tconfig::cpu_flags&FF_CPU_MMXEXT?Tnic_dering<Tmmxext>::nic_dering:Tnic_dering<Tmmx>::nic_dering;
}
