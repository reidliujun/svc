//#define IS_SSE2
// Copyright (c) 2002 Tom Barry.  All rights reserved.
//      trbarry@trbarry.com
// Requires Avisynth source code to compile for Avisynth
// Avisynth Copyright 2000 Ben Rudiak-Gould.
//      http://www.math.berkeley.edu/~benrg/avisynth.html
/////////////////////////////////////////////////////////////////////////////
//
// This file is subject to the terms of the GNU General Public License as
// published by the Free Software Foundation.  A copy of this license is
// included with this software distribution in the file COPYING.  If you
// do not have a copy, you may obtain a copy by writing to the Free
// Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details
//
//  Also, this program is "Philanthropy-Ware".  That is, if you like it and
//  feel the need to reward or inspire the author then please feel free (but
//  not obligated) to consider joining or donating to the Electronic Frontier
//  Foundation. This will help keep cyber space free of barbed wire and bullsh*t.
//
/////////////////////////////////////////////////////////////////////////////
/*

From the Readme_TomsMoComp.txt file:

        TomsMoComp - Motion Compensating Deinterlace

        TomsMoComp.dll is an Avisynth filter that uses motion compensation and adaptive processing to deinterlace video source.

        Just unzip the contents into your Avisynth directory, or somewhere. As the script shows,
        I made a subdirectory under Avisynth just to keep it separate.
        avs, one of the scripts I was testing:

        LoadPlugin("d:\AVISynth\TomsMoComp\Debug\TomsMoComp.dll")
        clip = AVISource("c:\vcr\bikes.avi")
        return clip.TomsMoComp(...)

        It specifies the file spec (change yours) and asks for TopFirst and SearchEffort=10
        to be turned on. I've so far tested it only
        with Avisynth/VirtualDub.

        TomsMoComp Parm list:

          return clip.TomsMoComp(TopFirst, SearchEffort, Vertical_Filter)

        All the values are integer, 0=no, 1=yes:

         TopFirst - assume the top field, lines 0,2,4,... should be displayed first.
            The default is the supposedly more common BottomFirst (not for me).

         SearchEffort - determines how much effort (cpu time) will be used to find
            moved pixels. Currently numbers from 0-45 with 0 being practially
            just a smarter bob.

        Known issues and limitation:
        1)  Assumes YUV (YUY2) Frame Based input. Use an AVIsynth function to convert first if
         needed.

        For now, see www.trbarry.com/TomsMoComp.zip

  THIS PROGRAM IS STILL NEW AND EXPERIMENTAL!

  Please send comments to Tom Barry (trbarry@trbarry.com)
*/

#include <malloc.h>
#include <string.h>
#include <unknwn.h>
#include "TomsMoComp_ff.h"
#include "../../inttypes.h"
#ifdef __GNUC__
#define __forceinline __attribute__((__always_inline__)) inline
#endif
#include "../../simd.h"
#include "../../compiler.h"

#pragma warning(disable: 4305 4309 4799 4127 4701)

static const __int64 ShiftMask=0xfefffefffefffeffULL;	// to avoid shifting chroma to luma
static const __int64 YMask =0x00ff00ff00ff00ffULL;
static const __int64 UVMask=0xff00ff00ff00ff00ULL;
static const __int64 Max_Mov=0x0404040404040404LL;
static const __int64 FOURS=0x0404040404040404LL;
static const __int64 TENS =0x0a0a0a0a0a0a0a0aLL;
static const __int64 ONES =0x0101010101010101LL;

typedef void (*TsearchLoopFc)(stride_t _src_pitch, stride_t _dst_pitch,
                     int _rowsize, const uint8_t* _pWeaveSrc, const uint8_t* _pWeaveSrcP,
                     uint8_t* _pWeaveDest, int _TopFirst, const uint8_t* _pCopySrc,
                     const uint8_t* _pCopySrcP, int _FldHeight);
template<class Tsimd,bool IS_SSE2,int BPP,bool USE_YV12,bool USE_VERTICAL_FILTER> struct TtomsMoComp
{
private:
 template<bool DBL_RESIZE,bool SKIP_SEARCH,int SearchRange> struct TsearchLoop
  {
   static __forceinline void merge4PIXavg(const unsigned char *PADDR1,const unsigned char *PADDR2,__m64 &mm0,__m64 &mm1,__m64 &mm2,__m64 &mm3,__m64 &mm5,__m64 &mm7)
    {
     movq (mm0, PADDR1); /* our 4 pixels */
     movq (mm1, PADDR2); /* our pixel2 value */
     movq (mm2, mm0);                        /* another copy of our pixel1 value */
     movq (mm3, mm1);                        /* another copy of our pixel1 value */
     psubusb (mm2, mm1 );
     psubusb (mm3, mm0 );
     por (mm2,mm3   );
     Tsimd::v_pavgb(mm0, mm1, mm3, ShiftMask); /* avg of 2 pixels */
     movq (mm3, mm2 );                       /* another copy of our our weights */
     pxor  (mm1, mm1);
     psubusb (mm3, mm7);                     /* nonzero where old weights lower, else 0 */
     pcmpeqb (mm3, mm1);                     /* now ff where new better, else 00     */
     pcmpeqb (mm1, mm3);             /* here ff where old better, else 00 */
     pand (mm0, mm3   );                     /* keep only better new pixels */
     pand ( mm2, mm3  );                     /* and weights */
     pand (mm5, mm1   );                     /* keep only better old pixels */
     pand ( mm7, mm1  );
     por  (mm5, mm0   );                     /* and merge new & old vals */
     por  (mm7, mm2   );
    }
   static __forceinline void merge4PIXavgH(const unsigned char *PADDR1A, const unsigned char *PADDR1B, const unsigned char *PADDR2A, const unsigned char *PADDR2B,__m64 &mm0,__m64 &mm1,__m64 &mm2,__m64 &mm3,__m64 &mm5,__m64 &mm7)
    {
     movq (mm0, PADDR1A); /* our 4 pixels */
     movq (mm1, PADDR2A); /* our pixel2 value */
     movq (mm2, PADDR1B); /* our 4 pixels */
     movq (mm3, PADDR2B); /* our pixel2 value */
     Tsimd::v_pavgb(mm0, mm2, mm2, ShiftMask);
     Tsimd::v_pavgb(mm1, mm3, mm3, ShiftMask);
     movq (mm2, mm0);                        /* another copy of our pixel1 value */
     movq (mm3, mm1);                        /* another copy of our pixel1 value */
     psubusb (mm2, mm1);
     psubusb (mm3, mm0);
     por (mm2,mm3);
     Tsimd::v_pavgb(mm0, mm1, mm3, ShiftMask);    /* avg of 2 pixels */
     movq (mm3, mm2  );                      /* another copy of our our weights */
     pxor  (mm1, mm1 );
     psubusb (mm3, mm7);                     /* nonzero where old weights lower, else 0 */
     pcmpeqb (mm3, mm1);                     /* now ff where new better, else 00     */
     pcmpeqb (mm1, mm3);             /* here ff where old better, else 00 */
     pand (mm0, mm3   );                     /* keep only better new pixels */
     pand  (mm2, mm3  );                     /* and weights */
     pand (mm5, mm1   );                     /* keep only better old pixels */
     pand ( mm7, mm1  );
     por  (mm5, mm0   );                     /* and merge new & old vals */
     por  (mm7, mm2   );
    }
   template<int r> static __forceinline void search(stride_t &ecx,const unsigned char* const esi,const unsigned char* const edi,__m64 &mm0,__m64 &mm1,__m64 &mm2,__m64 &mm3,__m64 &mm5,__m64 &mm7)
    {
     if (r==3 || r==5)
      {
       merge4PIXavg(edi+ecx-BPP, esi+ecx+BPP,mm0,mm1,mm2,mm3,mm5,mm7); // left, right
       merge4PIXavg(edi+ecx+BPP, esi+ecx-BPP,mm0,mm1,mm2,mm3,mm5,mm7); // right, left
      }
     if (r==5)
      {
       merge4PIXavgH(edi+ecx-BPP, edi+ecx, esi+ecx, esi+ecx+BPP,mm0,mm1,mm2,mm3,mm5,mm7); // left, right
       merge4PIXavgH(edi+ecx+BPP, edi+ecx, esi+ecx, esi+ecx-BPP,mm0,mm1,mm2,mm3,mm5,mm7); // right, left
      }
     if (r==21 || r==99)
      {
       merge4PIXavg(edi-6, esi+2*ecx+6,mm0,mm1,mm2,mm3,mm5,mm7);  // up left, down right
       merge4PIXavg(edi+6, esi+2*ecx-6,mm0,mm1,mm2,mm3,mm5,mm7);  // up right, down left
       merge4PIXavg(edi+ecx-6, esi+ecx+6,mm0,mm1,mm2,mm3,mm5,mm7); // left, right
       merge4PIXavg(edi+ecx+6, esi+ecx-6,mm0,mm1,mm2,mm3,mm5,mm7); // right, left
       merge4PIXavg(edi+2*ecx-6, esi+6,mm0,mm1,mm2,mm3,mm5,mm7);   // down left, up right
       merge4PIXavg(edi+2*ecx+6, esi-6,mm0,mm1,mm2,mm3,mm5,mm7);   // down right, up left
      }
     if (r==9 || r==11 || r==13 || r==15 || r==19 || r==21 || r==99)
      {
       merge4PIXavg(edi-BPP, esi+2*ecx+BPP,mm0,mm1,mm2,mm3,mm5,mm7);  // up left, down right
       merge4PIXavg(edi+BPP, esi+2*ecx-BPP,mm0,mm1,mm2,mm3,mm5,mm7);  // up right, down left
       merge4PIXavg(edi+2*ecx-BPP, esi+BPP,mm0,mm1,mm2,mm3,mm5,mm7);   // down left, up right
       merge4PIXavg(edi+2*ecx+BPP, esi-BPP,mm0,mm1,mm2,mm3,mm5,mm7);   // down right, up left
       merge4PIXavg(edi+ecx-BPP, esi+ecx+BPP,mm0,mm1,mm2,mm3,mm5,mm7); // left, right
       merge4PIXavg(edi+ecx+BPP, esi+ecx-BPP,mm0,mm1,mm2,mm3,mm5,mm7); // right, left
      }
     if (r==11 || r==13 || r==19)
      {
       merge4PIXavgH(edi+ecx-BPP, edi+ecx, esi+ecx, esi+ecx+BPP,mm0,mm1,mm2,mm3,mm5,mm7); // left, right
       merge4PIXavgH(edi+ecx+BPP, edi+ecx, esi+ecx, esi+ecx-BPP,mm0,mm1,mm2,mm3,mm5,mm7); // right, left
      }

     if (r!=0)
      if (!USE_YV12)
       por (mm7,UVMask);

     if (r==13)
      {
       merge4PIXavgH(edi+2*ecx, edi+ecx, esi+ecx, esi,mm0,mm1,mm2,mm3,mm5,mm7);	// down, up
       merge4PIXavgH(edi, edi+ecx, esi+ecx, esi+2*ecx,mm0,mm1,mm2,mm3,mm5,mm7);	// up, down
      }
     if (r==99)
      {
       merge4PIXavg(edi-8, esi+2*ecx+8,mm0,mm1,mm2,mm3,mm5,mm7);  // up left, down right
       merge4PIXavg(edi+8, esi+2*ecx-8,mm0,mm1,mm2,mm3,mm5,mm7);  // up right, down left
       merge4PIXavg(edi+ecx-8, esi+ecx+8,mm0,mm1,mm2,mm3,mm5,mm7); // left, right
       merge4PIXavg(edi+ecx+8, esi+ecx-8,mm0,mm1,mm2,mm3,mm5,mm7); // right, left
       merge4PIXavg(edi+2*ecx-8, esi+8,mm0,mm1,mm2,mm3,mm5,mm7);   // down left, up right
       merge4PIXavg(edi+2*ecx+8, esi-8,mm0,mm1,mm2,mm3,mm5,mm7);   // down right, up left
      }
     if (r==15 || r==19 || r==21 || r==99)
      {
       merge4PIXavg(edi-2*BPP, esi+2*ecx+2*BPP,mm0,mm1,mm2,mm3,mm5,mm7);  // up left, down right
       merge4PIXavg(edi+2*BPP, esi+2*ecx-2*BPP,mm0,mm1,mm2,mm3,mm5,mm7);  // up right, down left
       merge4PIXavg(edi+ecx-2*BPP, esi+ecx+2*BPP,mm0,mm1,mm2,mm3,mm5,mm7); // left, right
       merge4PIXavg(edi+ecx+2*BPP, esi+ecx-2*BPP,mm0,mm1,mm2,mm3,mm5,mm7); // right, left
       merge4PIXavg(edi+2*ecx-2*BPP, esi+2*BPP,mm0,mm1,mm2,mm3,mm5,mm7);   // down left, up right
       merge4PIXavg(edi+2*ecx+2*BPP, esi-2*BPP,mm0,mm1,mm2,mm3,mm5,mm7);   // down right, up left
      }
     if (r==19)
      {
       merge4PIXavgH(edi+2*ecx, edi+ecx, esi+ecx, esi,mm0,mm1,mm2,mm3,mm5,mm7);	// down, up
       merge4PIXavgH(edi, edi+ecx, esi+ecx, esi+2*ecx,mm0,mm1,mm2,mm3,mm5,mm7);	// up, down
      }
     if (r==9 || r==11 || r==13 || r==15 || r==19 || r==21 || r==99)
      {
       merge4PIXavg(edi+2*ecx, esi,mm0,mm1,mm2,mm3,mm5,mm7);	// down, up
       merge4PIXavg(edi, esi+2*ecx,mm0,mm1,mm2,mm3,mm5,mm7);	// up, down
      }

     if (r!=0)
      {
       paddusb (mm7, ONES);   // bias toward no motion
       merge4PIXavg(edi+ecx,esi+ecx,mm0,mm1,mm2,mm3,mm5,mm7);  // center, in old and new
      }
    }
   static void SearchLoop( stride_t src_pitch, stride_t dst_pitch,
                     int rowsize, const uint8_t* pWeaveSrc, const uint8_t* pWeaveSrcP,
                     uint8_t* pWeaveDest, int TopFirst, const uint8_t* pCopySrc,
                     const uint8_t* pCopySrcP, int FldHeight)
    {
     uint8_t *pDest;
     const uint8_t *pSrcP;
     const uint8_t *pSrc;
     const uint8_t *pBob;
     const uint8_t *pBobP;
     int LineCt;

     stride_t src_pitch2;
     if (DBL_RESIZE)
      src_pitch2 = src_pitch;                     // even & odd lines are not interleaved in DScaler
     else
      src_pitch2 = 2 * src_pitch;         // even & odd lines are interleaved in Avisynth

     stride_t dst_pitch2 = 2 * dst_pitch;
     int y;
     int Last8 = (rowsize-8);                    // ofs to last 8 bytes in row

     stride_t dst_pitchw = dst_pitch; // local stor so asm can ref
     pSrc  = pWeaveSrc;                      // points 1 weave line above
     pSrcP = pWeaveSrcP;                     // "

     if (DBL_RESIZE)
      {
       if (USE_VERTICAL_FILTER)
        pDest = pWeaveDest;                     //  but will appear 1 line lower than with not VF
       else
        pDest = pWeaveDest;
       pBob =  pCopySrc;
       pBobP =  pCopySrcP;                     // not used
       LineCt = FldHeight-1;
      }
     else
      {
       // Not DBL_RESIZE here
       if (USE_VERTICAL_FILTER)
        pDest = pWeaveDest + dst_pitch;
       else
        pDest = pWeaveDest + dst_pitch2;
       if (TopFirst)
        {
         pBob = pCopySrc + src_pitch2;      // remember one weave line just copied previously
         pBobP = pCopySrcP + src_pitch2;
        }
       else
        {
         pBob =  pCopySrc;
         pBobP =  pCopySrcP;
        }
       LineCt = FldHeight - 2;
      }
     __m64 Min_Vals,Max_Vals;
     for (y=1; y <= LineCt; y++)
      {
       __m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7;
       // Loop general reg usage
       //
       // eax - pBobP, then pDest
       // ebx - pBob
       // ecx - src_pitch2
       // edx - current offset
       // edi - prev weave pixels, 1 line up
       // esi - next weave pixels, 1 line up
       const unsigned char *ebx= pBob;
       stride_t ecx= src_pitch2,edx;
       const unsigned char *esi;
       unsigned char *edi,*eax;
       if (IS_SSE2)
        ;// sse2 code deleted for now
       else
        {
         ebx= pBob;
         ecx= src_pitch2;
         // simple bob first 8 bytes
         if (USE_VERTICAL_FILTER)
          {
           movq    (mm0, ebx);
           movq    (mm1, ebx+ecx);
           movq    (mm2, mm0);
           Tsimd::v_pavgb(mm2, mm1, mm3, ShiftMask);             // halfway between
           Tsimd::v_pavgb(mm0, mm2, mm3, ShiftMask);              // 1/4 way
           Tsimd::v_pavgb(mm1, mm2, mm3, ShiftMask);              // 3/4 way
           edi= pDest;
           stride_t eaxx= dst_pitchw;
           Tsimd::movntq (edi, mm0);
           Tsimd::movntq (edi+eaxx, mm1);

           // simple bob last 8 bytes
           edx= Last8;
           esi= ebx+edx;
           movq    (mm0, esi);
           movq    (mm1, esi+ecx);
           movq    (mm2, mm0);
           Tsimd::v_pavgb (mm2, mm1, mm3, ShiftMask);              // halfway between
           Tsimd::v_pavgb (mm0, mm2, mm3, ShiftMask);              // 1/4 way
           Tsimd::v_pavgb (mm1, mm2, mm3, ShiftMask);              // 3/4 way
           edi+= edx;                                                // last 8 bytes of dest
           Tsimd::movntq(edi, mm0);
           Tsimd::movntq(edi+eaxx, mm1);
          }
         else
          {
           movq    (mm0, ebx);
           //pavgb   mm0, qword ptr[ebx+ecx]
           Tsimd::v_pavgb(mm0, ebx+ecx, mm2, ShiftMask);
           edi= pDest;
           Tsimd::movntq(edi, mm0);

           // simple bob last 8 bytes
           edx= Last8;
           esi=ebx+edx;
           movq    (mm0, esi);
           //pavgb   mm0, qword ptr[esi+ecx]
           Tsimd::v_pavgb(mm0, esi+ecx, mm2, ShiftMask);
           Tsimd::movntq(edi+edx, mm0);
          }
         // now loop and get the middle qwords
         esi= pSrc;
         edi= (unsigned char*)pSrcP;
         edx= 8;                          // curr offset into all lines
        LoopQ:
         eax= (unsigned char*)pBobP;
         edi+= 8;
         esi+= 8;
         ebx+= 8;
         eax+= edx;

         //weirdbob.inc
         // First, get and save our possible Bob values
         // Assume our pixels are layed out as follows with x the calc'd bob value
         // and the other pixels are from the current field
         //
         //                j a b c k             current field
         //            x                 calculated line
         //        m d e f n             current field
         //
         // we calc the bob value as:
         //              x2 = either avg(a,f), avg(c,d), avg(b,e), avg(j,n), or avg(k,m)

         // selected for the     smallest of abs(a,f), abs(c,d), or abs(b,e), etc.

         // a,f
         // 2003/06/17 Use BPP (bits per pixel) here
         movq    (mm0, ebx-BPP);         // value a from top left
         movq    (mm1, ebx+ecx+BPP);     // value f from bottom right
         movq    (mm6, mm0);
         //pavgb   mm6, mm1                                        // avg(a,f), also best so far
         Tsimd::v_pavgb(mm6, mm1, mm7, ShiftMask);      // avg(a,f), also best so far
         movq    (mm7, mm0);
         psubusb (mm7, mm1);
         psubusb (mm1, mm0);
         por     (mm7, mm1);                                        // abs diff, also best so far

         // c,d
         movq    (mm0, ebx+BPP);         // value a from top left
         movq    (mm1, ebx+ecx-BPP);     // value f from bottom right
         movq    (mm2, mm0);
         //pavgb   mm2, mm1                                        // avg(c,d)
         Tsimd::v_pavgb(mm2, mm1, mm3, ShiftMask);      // avg(c,d)
         movq    (mm3, mm0);
         psubusb (mm3, mm1);
         psubusb (mm1, mm0);
         por     (mm3, mm1);                                        // abs(c,d)
         movq    (mm1, mm3);                                        // keep copy

         psubusb (mm3, mm7);                        // nonzero where new weights bigger, else 0
         pxor    (mm4, mm4);
         pcmpeqb (mm3, mm4);                        // now ff where new better, else 00
         pcmpeqb (mm4, mm3);                        // here ff where old better, else 00

         pand    (mm1, mm3);                        // keep only better new avg and abs
         pand    (mm2, mm3);

         pand    (mm6, mm4);
         pand    (mm7, mm4);

         por     (mm6, mm2);                        // and merge new & old vals keeping best
         por     (mm7, mm1);

         if (!USE_YV12)
          {
           por     (mm7, UVMask);                     // but we know YUY2 chroma is worthless so far
           pand    (mm5, YMask);                      // mask out chroma from here also
          }

         // j,n
         movq    (mm0, ebx-2*BPP);               // value j from top left
         movq    (mm1, ebx+ecx+2*BPP);   // value n from bottom right
         movq    (mm2, mm0);
         // pavgb   mm2, mm1                                        // avg(j,n)
         Tsimd::v_pavgb(mm2, mm1, mm3, ShiftMask);      // avg(j,n)
         movq    (mm3, mm0);
         psubusb (mm3, mm1);
         psubusb (mm1, mm0);
         por     (mm3, mm1);                                        // abs(j-n)
         movq    (mm1, mm3);                                        // keep copy

         psubusb (mm3, mm7);                        // nonzero where new weights bigger, else 0
         pxor    (mm4, mm4);
         pcmpeqb (mm3, mm4);                        // now ff where new better, else 00
         pcmpeqb (mm4, mm3);                        // here ff where old better, else 00

         pand    (mm1, mm3);                        // keep only better new avg and abs
         pand    (mm3, mm2);

         pand    (mm6, mm4);
         pand    (mm7, mm4);

         por     (mm6, mm3);                        // and merge new & old vals keeping best
         por     (mm7, mm1);                        // "

         // k, m
         movq    (mm0, ebx+2*BPP);               // value k from top right
         movq    (mm1, ebx+ecx-2*BPP);   // value n from bottom left
         movq    (mm4, mm0);
         //pavgb   mm4, mm1                                        // avg(k,m)
         Tsimd::v_pavgb(mm4, mm1, mm3, ShiftMask);      // avg(k,m)
         movq    (mm3, mm0);
         psubusb (mm3, mm1);
         psubusb (mm1, mm0);
         por     (mm3, mm1);                                        // abs(k,m)
         movq    (mm1, mm3);                                        // keep copy

         movq    (mm2, mm4);                        // avg(k,m)

         psubusb (mm3, mm7);                        // nonzero where new weights bigger, else 0
         pxor    (mm4, mm4);
         pcmpeqb (mm3, mm4);                        // now ff where new better, else 00
         pcmpeqb (mm4, mm3);                        // here ff where old better, else 00

         pand    (mm1, mm3);                        // keep only better new avg and abs
         pand    (mm3, mm2);

         pand    (mm6, mm4);
         pand    (mm7, mm4);

         por     (mm6, mm3);                        // and merge new & old vals keeping best
         por     (mm7, mm1);                        // "

         // b,e
         movq    (mm0,ebx);             // value b from top
         movq    (mm1,ebx+ecx); // value e from bottom

         // We will also calc here the max/min values to later limit comb
         // so the max excursion will not exceed the Max_Comb constant
         if (SKIP_SEARCH)
          {
           movq    (mm2, mm0);
           //pminub  mm2, mm1
           Tsimd::v_pminub(mm2, mm1, mm4);
           //pmaxub  mm6, mm2                        // clip our current results so far to be above this
           Tsimd::pmaxub(mm6, mm2);
           movq    (mm2, mm0);
           Tsimd::pmaxub (mm2, mm1);
           // pminub  mm6, mm2                        // clip our current results so far to be below this
           Tsimd::v_pminub(mm6, mm2, mm4);
          }
         else
          {
           movq    (mm2, mm0);
           movq    (mm4, eax);
           psubusb (mm2, mm4);
           psubusb (mm4, mm0);
           por     (mm4, mm2);                        // abs diff

           movq    (mm2, mm1);
           movq    (mm3, eax+ecx);
           psubusb (mm2, mm3);
           psubusb (mm3, mm1);
           por     (mm3, mm2);                        // abs diff
           //pmaxub  mm3, mm4                        // top or bottom pixel moved most
           Tsimd::pmaxub(mm3, mm4);                     // top or bottom pixel moved most
           psubusb (mm3, Max_Mov);            // moved more than allowed? or goes to 0?
           pxor    (mm4, mm4);
           pcmpeqb (mm3, mm4);                        // now ff where low motion, else high motion
           movq    (mm2, mm0);
           // pminub  mm2, mm1
           Tsimd::v_pminub(mm2, mm1, mm4);
           // pmaxub  mm6, mm2                        // clip our current results so far to be above this
           Tsimd::pmaxub(mm6, mm2);

           psubusb (mm2, mm3);                        // maybe decrease it to 0000.. if no surround motion
           movq    (Min_Vals, mm2);

           movq    (mm2, mm0);
           Tsimd::pmaxub (mm2, mm1);
           //  pminub  mm6, mm2                        // clip our current results so far to be below this
           Tsimd::v_pminub (mm6, mm2, mm4);
           paddusb (mm2, mm3);                        // maybe increase it to ffffff if no surround motion
           movq    (Max_Vals, mm2);
          }

         movq    (mm2, mm0);
         //pavgb   mm2, mm1                                        // avg(b,e)
         Tsimd::v_pavgb(mm2, mm1, mm3, ShiftMask);      // avg(b,e)

         movq    (mm3, mm0);
         psubusb (mm3, mm1);
         psubusb (mm1, mm0);
         por     (mm3, mm1);                        // abs(c,d)
         movq    (mm1, mm3);                        // keep copy of diffs

         pxor    (mm4, mm4);
         psubusb (mm3, mm7);                        // nonzero where new weights bigger, else 0
         pcmpeqb (mm3, mm4);                        // now ff where new better, else 00
         pcmpeqb (mm4, mm3);                        // here ff where old better, else 00

         pand    (mm1, mm3);
         pand    (mm2, mm3);

         pand    (mm6, mm4);
         pand    (mm7, mm4);

         por     (mm6, mm2);                        // our x2 value
         por     (mm7, mm1);                        // our x2 diffs
         movq    (mm4, mm7);                        // save as bob uncertainty indicator

         // For non-SSE2:
         // through out most of the rest of this loop we will maintain
         //      mm4             our min bob value
         //      mm5             best weave pixels so far
         //  mm6         our max Bob value
         //      mm7             best weighted pixel ratings so far

         // We will keep a slight bias to using the weave pixels
         // from the current location, by rating them by the min distance
         // from the Bob value instead of the avg distance from that value.
         // our best and only rating so far
         pcmpeqb(mm7, mm7);                        // ffff, say we didn't find anything good yet
        }
       // end of top
       search<SearchRange>(ecx,esi,edi,mm0,mm1,mm2,mm3,mm5,mm7);
       // bottom
       if (IS_SSE2)
        ;//sse2 code deleted for now
       else
        {
         // Version for non-SSE2
         if (SKIP_SEARCH)
          movq    (mm0, mm6);                        // just use the results of our wierd bob
         else
          {
           // Use the better of bob or weave
           //pminub  mm4, TENS                       // the most we care about
           Tsimd::v_pminub (mm4, TENS, mm0);       // the most we care about

           psubusb (mm7, mm4  );                      // foregive that much from weave est?
           psubusb (mm7, FOURS);                      // bias it a bit toward weave
           pxor    (mm0, mm0);
           pcmpeqb (mm7, mm0);                        // all ff where weave better, else 00
           pcmpeqb (mm0, mm7);                        // all ff where bob better, else 00
           pand    (mm0, mm6);                        // use bob for these pixel values
           pand    (mm7, mm5);                        // use weave for these
           por     (mm0, mm7);                        // combine both
           //pminub  mm0, Max_Vals           // but clip to catch the stray error
           Tsimd::v_pminub(mm0, Max_Vals, mm1); // but clip to catch the stray error
           //pmaxub  mm0, Min_Vals
           Tsimd::pmaxub (mm0, Min_Vals);
          }
         eax= pDest;
         if (USE_VERTICAL_FILTER)
          {
           movq    (mm1, mm0);
           //pavgb   mm0, qword ptr[ebx]
           Tsimd::v_pavgb (mm0, ebx, mm2, ShiftMask);
           //movntq  qword ptr[eax+edx], mm0
           Tsimd::movntq(eax+edx, mm0);
           //pavgb   mm1, qword ptr[ebx+ecx]
           Tsimd::v_pavgb (mm1, ebx+ecx, mm2, ShiftMask);
           eax+= dst_pitchw;
           //movntq  qword ptr[eax+edx], mm1
           Tsimd::movntq(eax+edx, mm1);
          }
         else
          {
           // movntq  qword ptr[eax+edx], mm0
           movq    (mm1, esi+ecx);
           Tsimd::movntq(eax+edx, mm0);
          }
         edx=edx+8;            // bump offset pointer
         if (edx<Last8)                     // done with line?
          goto LoopQ;                           // y
        }
       // adjust for next line
       pSrc  += src_pitch2;
       pSrcP += src_pitch2;
       pDest += dst_pitch2;
       pBob += src_pitch2;
       pBobP += src_pitch2;
      }
     _mm_empty();
    }
  };
public:
 static TsearchLoopFc getProcess(int SearchEffort)
  {
   // Go fill in the hard part, being variously lazy depending upon SearchEffort
   if (SearchEffort == -1)
    return TsearchLoop< true, true, 0>::SearchLoop;
   else if (SearchEffort == 0)
    return TsearchLoop<false, true, 0>::SearchLoop;
   else if (SearchEffort <= 1)
    return TsearchLoop<false,false, 1>::SearchLoop;
   // else if (SearchEffort <= 2)
    //Search_Effort_2();
   else if (SearchEffort <= 3)
    return TsearchLoop<false,false, 3>::SearchLoop;
   else if (SearchEffort <= 5)
    return TsearchLoop<false,false, 5>::SearchLoop;
   else if (SearchEffort <= 9)
    return TsearchLoop<false,false, 9>::SearchLoop;
   else if (SearchEffort <= 11)
    return TsearchLoop<false,false,11>::SearchLoop;
   else if (SearchEffort <= 13)
    return TsearchLoop<false,false,13>::SearchLoop;
   else if (SearchEffort <= 15)
    return TsearchLoop<false,false,15>::SearchLoop;
   else if (SearchEffort <= 19)
    return TsearchLoop<false,false,19>::SearchLoop;
   else if (SearchEffort <= 21)
    return TsearchLoop<false,false,21>::SearchLoop;
   else
    return TsearchLoop<false,false,99>::SearchLoop;
  }
};

template<class Tsimd> struct TdblResize
{
 static __forceinline void DblResizeH_1(int rowsize, const BYTE* srcp, BYTE* dstp)
  {
   int ct = (rowsize-8) >> 3;
   // Loop general reg usage
   //
   // ecx loop ctr
   // edi - dest
   // esi - src pixels

   // now loop and get the middle qwords
   unsigned char *edi= dstp;
   const unsigned char *esi= srcp;
   __m64 mm0,mm2,mm6,mm1,mm7;
   for (int ecx=ct;ecx>0;ecx--,esi+=8,edi+=16)
    {
     // just use the current line, first luma
     movq (mm0, esi);  // orig luma, in register as VYUYVYUY
     movq (mm2, mm0);    // keep a copy while we got it
     movq (mm6, esi+2); // luma 2 bytes to right
     Tsimd::pavgb   (mm6, mm0);    // 3 pixels of valid new luma averag, plus garbage
     pand (mm0, YMask);    // keep only old luma, 0Y0Y0Y0Y
     psllw (mm6, 8);     // now only new luma as y0y0y0y0
     por  (mm6, mm0);    // all luma yYyYyYyY

     // now  chroma bytes
     movq (mm1, mm2);
     Tsimd::pavgb (mm1, esi+4); // chroma 4 bytes to right
     psrlw (mm2, 8);     // only orig chroma as 0V0U0V0U
     packuswb (mm2, mm2);    // now xxxxVUVU
     psrlw (mm1, 8);     // only new chroma as 0?0?0v0u
     packuswb (mm1, mm1);    // now new chroma bytes xxxx??vu
     punpcklwd (mm2,mm1);    // ??VUvuVU, merged old and new chroma

     // merge final luma & chroma bytes

     movq (mm7, mm6);    // copy of luma, ?YyYyYyY
     punpcklbw (mm6, mm2);    // low bytes vyuYVyUY
     punpckhbw (mm7, mm2);    // hi bytes ???YVyUY

     Tsimd::movntq(edi, mm6); // store low qword
     Tsimd::movntq(edi+8, mm7); // store high qword
    }

   // now expand the last 8 into 16 bytes
   movq (mm0, esi);  // orig luma, in register as VYUYVYUY
   movq (mm2, mm0);    // keep a copy while we got it
   movq (mm6, mm0);    // and another to work on now

   psrlq (mm6, 16);     // pretend most of it came from +2 offs (remem revers order)
   Tsimd::pavgb (  mm6, mm0);    // 3 pixels of valid new luma averag, plus garbage
   pand (mm0, YMask);    // keep only old luma, 0Y0Y0Y0Y
   psllw (mm6, 8);     // now only new luma as ?0y0y0y0
   por  (mm6, mm0);    // all luma ?YyYyYyY

   // now final chroma bytes
   movq (mm1, mm2);
   psrlq (mm1, 32);     // pretend most of it came from +4 offs (remem revers order)
   Tsimd::pavgb   (mm1, mm2);    // 2 pixels of valid new chroma averag, plus garbage
   psrlw (mm2, 8);     // only orig chroma as 0V0U0V0U
   packuswb (mm2, mm2);    // now xxxxVUVU
   psrlw (mm1, 8);     // only new chroma as 0?0?0v0u
   packuswb (mm1, mm1);    // now new chroma bytes xxxx??vu
   punpcklwd (mm2,mm1);    // ??VUvuVU, merged old and new chroma

   // merge final luma & chroma bytes

   movq( mm7, mm6);    // copy of luma, ?YyYyYyY
   punpcklbw (mm6, mm2);    // low bytes vyuYVyUY
   punpckhbw (mm7, mm2);    // hi bytes ???YVyUY

   movq (edi, mm6);    // store low qword
   movq (edi+8, mm7);     // store high qword

   *(edi+13)=*(esi+5);
   *(short*)(edi+14)=*(short*)(esi+6);
  }
 static __forceinline void isBobBetter(__m64 &mm0,__m64 &mm1,__m64 &mm2,__m64 &mm3,__m64 &mm4,__m64 &mm6,__m64 &mm7)
  {
   movq   (mm2, mm0 );
   Tsimd::v_pavgb(mm2, mm1, mm3, ShiftMask) ;
   movq   (mm3, mm0 );
   psubusb(mm3, mm1 );
   psubusb(mm1, mm0 );
   por    (mm3, mm1 );       /* abs diff */
   movq   (mm1, mm3 );       /* keep copy */
   psubusb(mm3, mm7 );       /* nonzero where new weights bigger, else 0 */
   pxor   (mm4, mm4 );
   pcmpeqb(mm3, mm4 );       /* now ff where new better, else 00     */
   pcmpeqb(mm4, mm3 );       /* here ff where old better, else 00 */
   pand   (mm1, mm3 );       /* keep only better new avg and abs */
   pand   (mm2, mm3 );
   pand   (mm6, mm4 );
   pand   (mm7, mm4 );
   por    (mm6, mm2 );       /* and merge new & old vals keeping best */
   por    (mm7, mm1 );
  }

 static void Avisynth_DblResizeH(stride_t src_pit, stride_t dst_pit, int rowsize, const BYTE* srcp, BYTE* dstp, int FldHeight)     // go H expand lines into even output lines
  {
   const BYTE* pSrc = srcp;
   BYTE* pDest = dstp + 2*dst_pit;
   int y;
   int ct = (rowsize-8) >> 3;

   DblResizeH_1(rowsize, srcp, dstp);  // expand top line
   DblResizeH_1(rowsize, srcp + src_pit, dstp + dst_pit);  // expand 2nd line
   DblResizeH_1(rowsize, srcp+(FldHeight-2) * src_pit,dstp + (FldHeight-2) * dst_pit);  // expand next to last line
   DblResizeH_1(rowsize, srcp+(FldHeight-1) * src_pit, dstp + (FldHeight-1) * dst_pit);  // expand last line

   for (y=1; y <= FldHeight-4; y++, pSrc+=src_pit,pDest+=dst_pit)
    {
     // Loop general reg usage
     //
     // eax -
     // ebx - src 2 lines above curr
     // ecx loop ctr
     // edx - dst_pit
     // edi - dest
     // esi - src pixels, 1 line up

     // now loop and get the middle qwords
     unsigned char *edi= pDest;
     const unsigned char *ebx= pSrc;
     stride_t edx= src_pit;
     __m64 mm0,mm1,mm6,mm7,mm2,mm4,mm3;
     for (int ecx=ct;ecx>0;ebx+=8,edi+=16,ecx--)
      {
       // Assume our pixels are layed out as follows with x the calc'd value
       // and the other pixels are from the current field, top, curr, and lower lines.
       //
       //    i   j   2 lines above
       //    a   b   above line
       //        c x d   curr line
       //        e   f   next line
       //        m   n   2 lines below
       //
       // we calc the middle x value as:
       //  x = either avg(a,f), avg(c,d), avg(b,e)

       // selected for the smallest of SAD(a,f), SAD(c,d), or SAD(b,e), etc.
       // where SAD() = sum of absolute diff
       //
       // we do this first for luma. For now we will just horizontally average chroma

       const unsigned char *esi=ebx+edx;

       // i,n
       movq (mm0, ebx);   // value i from top left
       movq (mm1, ebx+4*edx+2); // value n from bottom right
       movq (mm6, mm0);
       //  pavgb mm6, mm1     // avg(i,n), also best so far
       Tsimd::v_pavgb(mm6, mm1, mm7, ShiftMask); // avg(i,n), also best so far
       movq (mm7, mm0);
       psubusb (mm7, mm1);
       psubusb (mm1, mm0);
       por  (mm7, mm1);     // abs diff (SAD), also best so far

       // m,j
       movq    (mm0, ebx+2);  // value j from top right
       movq    (mm1, ebx+4*edx); // value m from bottom left
       isBobBetter(mm0,mm1,mm2,mm3,mm4,mm6,mm7);

       // a,f
       movq    (mm0, esi);   // value a from top left
       movq    (mm1, esi+2*edx+2); // value f from bottom right
       isBobBetter(mm0,mm1,mm2,mm3,mm4,mm6,mm7);


       // e,b
       movq    (mm0, esi+2);  // value b from top right
       movq    (mm1, esi+2*edx); // value e from bottom left
       isBobBetter(mm0,mm1,mm2,mm3,mm4,mm6,mm7);

       // c,d
       movq    (mm0, esi+edx); // value c from left
       movq    (mm1, esi+edx+2); // value d from right

       // We will also clip what we've got so far to avoid artifacts

       movq (mm2, mm0);
       //  pminub mm2, mm1
       Tsimd::v_pminub(mm2, mm1, mm4);
       //  pmaxub mm6, mm2   // clip our current results so far to be above this
       Tsimd::pmaxub (mm6, mm2);
       movq (mm2, mm0);
       Tsimd::pmaxub (mm2, mm1);
       //  pminub mm6, mm2   // clip our current results so far to be below this
       Tsimd::v_pminub (mm6, mm2, mm4);

       isBobBetter(mm0,mm1,mm2,mm3,mm4,mm6,mm7);

       // we have now create 4 new luma values for pixel offsets 1,3,5,7. But we need to merge these
       // with luma values for offsets 0,2,4,6 and some chroma info. First the other luma.
       movq (mm0, esi+edx); // orig luma, in register as VYUYVYUY
       movq (mm2, mm0);    // keep a copy while we got it
       pand (mm0, YMask);    // keep only old luma, 0Y0Y0Y0Y
       psllw( mm6, 8);     // now only new luma as y0y0y0y0
       por  (mm6, mm0);    // all luma yYyYyYyY
       __m64 SaveLuma;
       movq (SaveLuma, mm6);   // and save it for later

       // get chroma, but we only use 3 lines for chroma instead of 5


       // a,f - Chroma
       movq (mm0, esi);   // value a from top left
       movq (mm1, esi+2*edx+4); // value f from bottom right
       movq (mm6, mm0 );
       //  pavgb mm6, mm1     // avg(a,f), also best so far
       Tsimd::v_pavgb(mm6, mm1, mm7, ShiftMask);
       movq (mm7, mm0);
       psubusb (mm7, mm1);
       psubusb (mm1, mm0);
       por  (mm7, mm1);     // abs diff (SAD), also best so far

       // e,b - Chroma
       movq    (mm0, esi+4);  // value b from top right
       movq    (mm1, esi+2*edx); // value e from bottom left
       isBobBetter(mm0,mm1,mm2,mm3,mm4,mm6,mm7);

       // c,d - Chroma
       movq    (mm0, esi+edx); // value c from left
       movq    (mm1, esi+edx+4); // value d from right

       // We will also clip what we've got so far to avoid artifacts
       movq (mm2, mm0);
       //  pminub mm2, mm1
       Tsimd::v_pminub (mm2, mm1, mm4);
       //  pmaxub mm6, mm2   // clip our current results so far to be above this
       Tsimd::pmaxub (mm6, mm2);
       movq (mm2, mm0);
       Tsimd::pmaxub (mm2, mm1);
       //  pminub mm6, mm2   // clip our current results so far to be below this
       Tsimd::v_pminub (mm6, mm2, mm4);

       isBobBetter(mm0,mm1,mm2,mm3,mm4,mm6,mm7);

       movq  (mm2, esi+edx); // another copy of orig Chroma from c
       psrlw (mm2, 8);     // only orig chroma as 0V0U0V0U
       packuswb (mm2, mm2);    // now xxxxVUVU
       psrlw (mm6, 8);     // only new chroma as 0v0u0v0u
       packuswb (mm6, mm6);    // now new chroma bytes xxxxvuvu
       punpcklwd (mm2,mm6);    // vuVUvuVU, merged old and new chroma

       // merge luma & chroma
       movq (mm6, SaveLuma);   // get luma again
       movq (mm7, mm6);    // copy of luma
       punpcklbw (mm6, mm2);    // low bytes vyuYVyUY
       punpckhbw (mm7, mm2);    // hi bytes vyuYVyUY

       //  movntq qword ptr[eax+edx], mm0
       Tsimd::movntq (edi, mm6); // store low qword
       Tsimd::movntq (edi+8, mm7); // store high qword
      }

     // done with our horizontal loop but we have 8 more bytes of input to process
     // we will be fairly lazy with this and for now just use the current line, first luma
     const unsigned char *esi= ebx+edx;
     movq (mm0, esi+edx); // orig luma, in register as VYUYVYUY
     movq (mm2, mm0);    // keep a copy while we got it
     movq (mm6, mm0);    // and another to work on now

     psrlq( mm6, 16  );   // pretend most of it came from +2 offs (remem revers order)
     Tsimd::pavgb(   mm6, mm0);    // 3 pixels of valid new luma averag, plus garbage
     pand (mm0, YMask );  // keep only old luma, 0Y0Y0Y0Y
     psllw( mm6, 8    ); // now only new luma as ?0y0y0y0
     por  (mm6, mm0   ); // all luma ?YyYyYyY

   // now final chroma bytes
     movq (mm1, mm2);
     psrlq (mm1, 32);     // pretend most of it came from +4 offs (remem revers order)
     Tsimd::pavgb   (mm1, mm2);    // 2 pixels of valid new chroma averag, plus garbage
     psrlw (mm2, 8);     // only orig chroma as 0V0U0V0U
     packuswb (mm2, mm2);    // now xxxxVUVU
     psrlw (mm1, 8);     // only new chroma as 0?0?0v0u
     packuswb (mm1, mm1);    // now new chroma bytes xxxx??vu
     punpcklwd (mm2,mm1);    // ??VUvuVU, merged old and new chroma

   // merge final luma & chroma bytes

     movq (mm7, mm6);    // copy of luma, ?YyYyYyY
     punpcklbw (mm6, mm2);    // low bytes vyuYVyUY
     punpckhbw (mm7, mm2);    // hi bytes ???YVyUY

     movq (edi, mm6);    // store low qword
     movq (edi+8, mm7);     // store high qword

     *(edi+13)=*(esi+edx+5);    // last U source byte
     *(short*)(edi+14)=*(short*)(esi+edx+6);    // last 2 YV source bytes
    }
  }
};

struct TomsMoComp :public ItomsMoComp
{
private:
 enum
  {
   PLANAR_Y=0,
   PLANAR_U=1,
   PLANAR_V=2
  };
 struct TVideoFrame
  {
   unsigned char* ptr[3];
   unsigned int dx[3],dy[3];stride_t stride[3];
   const unsigned char* GetReadPtr(int plane) {return ptr[plane];}
   unsigned char* GetWritePtr(int plane) {return ptr[plane];}
   stride_t GetPitch(int plane) {return stride[plane];}
   unsigned int GetRowSize(int plane) {return dx[plane];}
   unsigned int GetHeight(int plane) {return dy[plane];}
  };
 typedef TVideoFrame *PVideoFrame;
 PVideoFrame src;
 PVideoFrame dst;
 PVideoFrame prevSrc;
 uint8_t* pWorkArea;
 const uint8_t* pWeaveSrcP;
 const uint8_t* pCopySrcP;

 int TopFirst;
 int searchEffort,oldSearchEffort;
 bool IsYUY2,Use_Vertical_Filter;
 bool SSE2enabled,SSEMMXenabled,_3DNOWenabled;
 TsearchLoopFc searchLoopFc;

 int PrevFrame;
 int PrevInFrame;
 int dropFrame;

 stride_t src_pitch;

 static void Fieldcopy(void *dest, const void *src, size_t count, int rows, stride_t dst_pitch, stride_t src_pitch)
  {
  BYTE* pDest = (BYTE*) dest;
  const BYTE* pSrc = (const BYTE*) src;
  for (int i=0; i < rows; i++)
   {
    memcpy(pDest, pSrc, count);
    pSrc += src_pitch;
    pDest += dst_pitch;
   }
 }

 void ProcessFrame(int SearchEffort,const unsigned char *srcp,unsigned char *dstp,stride_t dst_pitch,int rowsize,int FldHeight,int height)
  {
   unsigned char *pWeaveDest;const unsigned char *pWeaveSrc;
   unsigned char *pCopyDest;const unsigned char *pCopySrc;
   if (SearchEffort==-2)    // note - DBL_Resize carried ast TopFirst == 0 hrtrtr
    {
     SearchEffort = -1;    // pretend it is -1 after this
     pWeaveDest = dstp+dst_pitch; // odd dest lines
     pCopyDest = dstp;    // even dest lines
     if (Use_Vertical_Filter)
      {
       if (SSEMMXenabled)
        TdblResize<Tmmxext>::Avisynth_DblResizeH(src_pitch, rowsize,rowsize/2, srcp, pWorkArea,FldHeight);// go H expand lines into even output lines
       //else if (_3DNOWenabled)
       // TdblResize<T3dnow>::Avisynth_DblResizeH(src_pitch, rowsize,rowsize/2, srcp, pWorkArea,FldHeight);// go H expand lines into even output lines
       else
        TdblResize<Tmmx>::Avisynth_DblResizeH(src_pitch, rowsize,rowsize/2, srcp, pWorkArea,FldHeight);// go H expand lines into even output lines
       src_pitch = rowsize;   // pretend it is wider now, and in out buffer
       pWeaveSrc = pWorkArea;   // pretend source is here now
       pCopySrc = pWorkArea;   // "
       Fieldcopy(pCopyDest, pCopySrc, rowsize, 1, dst_pitch, src_pitch); // copy top EVEN line
      }
     else
      {
       if (SSEMMXenabled)
        TdblResize<Tmmxext>::Avisynth_DblResizeH(src_pitch, 2*dst_pitch, rowsize/2, srcp, dstp, FldHeight);     // go H expand lines into even output lines
       //else if (_3DNOWenabled)
       // TdblResize<T3dnow>::Avisynth_DblResizeH(src_pitch, 2*dst_pitch, rowsize/2, srcp, dstp, FldHeight);     // go H expand lines into even output lines
       else
        TdblResize<Tmmx>::Avisynth_DblResizeH(src_pitch, 2*dst_pitch, rowsize/2, srcp, dstp, FldHeight);     // go H expand lines into even output lines
       src_pitch = 2*dst_pitch;  // pretend it is wider now, and in out buffer
       pWeaveSrc = dstp;    // pretend source is here now
       pCopySrc = dstp;    // "
       //   Fieldcopy(pWeaveDest, pCopySrc, rowsize, 1, dst_pitch, src_pitch);   // copy top ODD line
      }

     Fieldcopy(pWeaveDest + (height-2)*dst_pitch, pCopySrc+(FldHeight-1)*src_pitch, rowsize, 1, dst_pitch, src_pitch);   // copy bottom Odd line
    }
   else if (SearchEffort == -1)    // note - DBL_Resize carried ast TopFirst == 0 hrtrtr
    {
     pCopySrc = srcp;      // even src lines, 0,2,4..
     pWeaveSrc = srcp;      // odd src lines, 1,3,5..
     pCopyDest = dstp;      // even dest
     pWeaveDest = dstp+dst_pitch;   // odd dest
     if (Use_Vertical_Filter)
      Fieldcopy(pCopyDest, pCopySrc, rowsize, 1, dst_pitch, src_pitch); // copy top EVEN line
     else
      Fieldcopy(pCopyDest, pCopySrc, rowsize,  FldHeight, dst_pitch*2, src_pitch);  // copy all EVEN lines (base 0) I don't remember why???
     Fieldcopy(pWeaveDest + (height-2)*dst_pitch, pCopySrc+(FldHeight-1)*src_pitch, rowsize, 1, dst_pitch, src_pitch); // copy bottom Odd line
    }
   else if (TopFirst)
    {
     pWeaveSrc = srcp+src_pitch;
     pCopySrc = srcp;
     pWeaveDest = dstp+dst_pitch;
     pCopyDest = dstp;
     Fieldcopy(pWeaveDest, pCopySrc, rowsize, 1, dst_pitch*2, src_pitch*2); // bob this later?
     Fieldcopy(pWeaveDest+(FldHeight-1)*dst_pitch*2, pCopySrc+(FldHeight-1)*src_pitch*2, rowsize, 1, dst_pitch*2, src_pitch*2);
     if (Use_Vertical_Filter)
      {
       Fieldcopy(pCopyDest, pCopySrc, rowsize, 1, dst_pitch*2, src_pitch*2);
       Fieldcopy(pCopyDest+(FldHeight-1)*dst_pitch*2,pCopySrc+(FldHeight-1)*src_pitch*2, rowsize, 1, dst_pitch*2, src_pitch*2); // copy last bob line
      }
     else
      Fieldcopy(pCopyDest, pCopySrc, rowsize, FldHeight, dst_pitch*2, src_pitch*2);
    }
   else // not Top First
    {
     pWeaveSrc = srcp;
     pCopySrc = srcp + src_pitch;
     pWeaveDest = dstp;
     pCopyDest = dstp + dst_pitch;
     Fieldcopy(pWeaveDest, pCopySrc, rowsize, 1, dst_pitch*2, src_pitch*2);
     Fieldcopy(pWeaveDest+(FldHeight-1)*dst_pitch*2, pCopySrc+(FldHeight-1)*src_pitch*2, rowsize,  1, dst_pitch*2, src_pitch*2); // bob this later
     if (Use_Vertical_Filter)
      {
       Fieldcopy(pCopyDest, pCopySrc, rowsize, 1, dst_pitch*2, src_pitch*2); // copy first bob line
       Fieldcopy(pCopyDest+(FldHeight-1)*dst_pitch*2, pCopySrc+(FldHeight-1)*src_pitch*2, rowsize, 1, dst_pitch*2, src_pitch*2); // copy last bob line
       pWeaveDest += dst_pitch;                // bug, adj for poor planning
      }
     else
      Fieldcopy(pCopyDest, pCopySrc, rowsize, FldHeight, dst_pitch*2, src_pitch*2);
    }

   if (oldSearchEffort!=SearchEffort)
    searchLoopFc=getSearchFc(SearchEffort);
   searchLoopFc(src_pitch, dst_pitch, rowsize, pWeaveSrc, pWeaveSrcP,pWeaveDest, TopFirst, pCopySrc,pCopySrcP, FldHeight);
  }
 void ProcessFrame_YV12(int SearchEffort,const unsigned char *srcp,unsigned char *dstp,stride_t dst_pitch,int rowsize,int FldHeight,int height)
  {
   unsigned char *pWeaveDest;const unsigned char *pWeaveSrc;
   unsigned char *pCopyDest;const unsigned char *pCopySrc;
   if (SearchEffort < -1) SearchEffort = -1;  //>>>>> temp
   if (SearchEffort == -1)                         // note - DBL_Resize carried ast TopFirst == 0 hrtrtr
    {
     pCopySrc = srcp;                                                // even src lines, 0,2,4..
     pWeaveSrc = srcp;                                               // odd src lines, 1,3,5..
     pCopyDest = dstp;                                               // even dest
     pWeaveDest = dstp+dst_pitch;                    // odd dest
     if (Use_Vertical_Filter)
      Fieldcopy(pCopyDest, pCopySrc, rowsize, 1, dst_pitch, src_pitch);// copy top EVEN line
     else
      Fieldcopy(pCopyDest, pCopySrc, rowsize, FldHeight, dst_pitch*2, src_pitch); // copy all EVEN lines (base 0) I don't remember why???
     Fieldcopy(pWeaveDest + (height-2)*dst_pitch, pCopySrc+(FldHeight-1)*src_pitch, rowsize, 1, dst_pitch, src_pitch); // copy bottom Odd line
    }
   else
   if (TopFirst)
    {
     pWeaveSrc = srcp+src_pitch;
     pCopySrc = srcp;
     pWeaveDest = dstp+dst_pitch;
     pCopyDest = dstp;
     Fieldcopy(pWeaveDest, pCopySrc, rowsize,1, dst_pitch*2, src_pitch*2);// bob this later?
     Fieldcopy(pWeaveDest+(FldHeight-1)*dst_pitch*2,pCopySrc+(FldHeight-1)*src_pitch*2, rowsize, 1, dst_pitch*2, src_pitch*2);
     if (Use_Vertical_Filter)
      {
       Fieldcopy(pCopyDest, pCopySrc, rowsize, 1, dst_pitch*2, src_pitch*2);
       Fieldcopy(pCopyDest+(FldHeight-1)*dst_pitch*2,pCopySrc+(FldHeight-1)*src_pitch*2, rowsize, 1, dst_pitch*2, src_pitch*2);  // copy last bob line
      }
     else
      Fieldcopy(pCopyDest, pCopySrc, rowsize, FldHeight, dst_pitch*2, src_pitch*2);
    }
   else             // not Top First
    {
     pWeaveSrc = srcp;
     pCopySrc = srcp + src_pitch;
     pWeaveDest = dstp;
     pCopyDest = dstp + dst_pitch;
     Fieldcopy(pWeaveDest, pCopySrc, rowsize, 1, dst_pitch*2, src_pitch*2);
     Fieldcopy(pWeaveDest+(FldHeight-1)*dst_pitch*2,pCopySrc+(FldHeight-1)*src_pitch*2, rowsize, 1, dst_pitch*2, src_pitch*2);   // bob this later
     if (Use_Vertical_Filter)
      {
       Fieldcopy(pCopyDest, pCopySrc, rowsize, 1, dst_pitch*2, src_pitch*2);   // copy first bob line
       Fieldcopy(pCopyDest+(FldHeight-1)*dst_pitch*2,pCopySrc+(FldHeight-1)*src_pitch*2, rowsize, 1, dst_pitch*2, src_pitch*2); // copy last bob line
       pWeaveDest += dst_pitch;  // bug, adj for poor planning
      }
     else
      Fieldcopy(pCopyDest, pCopySrc, rowsize, FldHeight, dst_pitch*2, src_pitch*2);
    }

   if (oldSearchEffort!=SearchEffort)
    searchLoopFc=getSearchFc(SearchEffort);
   searchLoopFc(src_pitch, dst_pitch, rowsize, pWeaveSrc, pWeaveSrcP,pWeaveDest, TopFirst, pCopySrc,pCopySrcP, FldHeight);
  }
 TsearchLoopFc getSearchFc(int searchEffort)
  {
   oldSearchEffort=searchEffort;
   if (IsYUY2)
    if (Use_Vertical_Filter)
     if (SSEMMXenabled)
      return TtomsMoComp<Tmmxext,false,2,false,true>::getProcess(searchEffort);
     //else if (_3DNOWenabled)
     // return TtomsMoComp<T3dnow,false,2,false,true>::getProcess(searchEffort);
     else
      return TtomsMoComp<Tmmx,false,2,false,true>::getProcess(searchEffort);
    else
     if (SSEMMXenabled)
      return TtomsMoComp<Tmmxext,false,2,false,false>::getProcess(searchEffort);
     //else if (_3DNOWenabled)
     // return TtomsMoComp<T3dnow,false,2,false,false>::getProcess(searchEffort);
     else
      return TtomsMoComp<Tmmx,false,2,false,false>::getProcess(searchEffort);
   else
    if (Use_Vertical_Filter)
     if (SSEMMXenabled)
      return TtomsMoComp<Tmmxext,false,1,true,true>::getProcess(searchEffort);
     //else if (_3DNOWenabled)
     // return TtomsMoComp<T3dnow,false,1,true,true>::getProcess(searchEffort);
     else
      return TtomsMoComp<Tmmx,false,1,true,true>::getProcess(searchEffort);
    else
     if (SSEMMXenabled)
      return TtomsMoComp<Tmmxext,false,1,true,false>::getProcess(searchEffort);
     //else if (_3DNOWenabled)
     // return TtomsMoComp<T3dnow,false,1,true,true>::getProcess(searchEffort);
     else
      return TtomsMoComp<Tmmx,false,1,true,false>::getProcess(searchEffort);
  }
public:
 TomsMoComp(void)
  {
   src=NULL;
   dst=NULL;
   prevSrc=NULL;
   pWorkArea=NULL;
  }
 virtual ~TomsMoComp()
  {
   if (src) delete src;
   if (dst) delete dst;
   if (prevSrc) delete prevSrc;
   if (pWorkArea) free(pWorkArea);
  }
 virtual void STDMETHODCALLTYPE destroy(void)
  {
   delete this;
  }
 virtual bool STDMETHODCALLTYPE create(bool isSSE2,bool isMMX2,bool is3dnow,bool _IsYUY2, int _TopFirst, int _Search_Effort, bool _Use_Vertical_Filter,int &width,int &height,stride_t srcStride[4],stride_t dstStride[4])
  {
   IsYUY2=_IsYUY2;
   src=new TVideoFrame;
   src->dx[0]=width;src->dx[1]=src->dx[2]=width/2;
   src->stride[0]=srcStride[0];src->stride[1]=srcStride[1];src->stride[2]=srcStride[2];
   src->dy[0]=height;src->dy[1]=src->dy[2]=height/2;
   dst=new TVideoFrame;
   memcpy(dst->dx,src->dx,sizeof(src->dx));
   memcpy(dst->dy,src->dy,sizeof(src->dy));
   dst->stride[0]=dstStride[0];dst->stride[1]=dstStride[1];dst->stride[2]=dstStride[2];
   prevSrc=new TVideoFrame;
   memcpy(prevSrc->dx,src->dx,sizeof(src->dx));
   memcpy(prevSrc->dy,src->dy,sizeof(src->dy));
   prevSrc->stride[0]=srcStride[0];prevSrc->stride[1]=srcStride[1];prevSrc->stride[2]=srcStride[2];

   TopFirst = _TopFirst;
   Use_Vertical_Filter = _Use_Vertical_Filter;
   searchEffort = _Search_Effort;
   PrevFrame = -2;                 // last frame processed, if any
   PrevInFrame = -2;               // last frame requested, if any
   dropFrame = 1;                  // next frame to drop
   SSE2enabled = isSSE2;
   SSEMMXenabled =isMMX2;
   _3DNOWenabled =is3dnow;
   pWeaveSrcP=pCopySrcP=NULL;

   if (width % 4)
    return false;

   if (searchEffort == -1)
    {
     height = 2 * height;
     TopFirst = 0;
    }

   if (searchEffort == -2)
    {
     width = 2 * width;
     height = 2 * height;
     TopFirst = 0;
     if (Use_Vertical_Filter)
      {
       pWorkArea = (BYTE*) malloc(width*height*2+128);
      }
    }

   oldSearchEffort=-1;
   return true;
  }
 virtual void STDMETHODCALLTYPE GetFrame(int inFrame,int tff,const uint8_t *srcs[3],uint8_t *dsts[3],const uint8_t *psrcs[3])
  {
   int useFrame=inFrame;
   for (int i=0;i<3;i++)
    {
     src->ptr[i]=(unsigned char*)srcs[i];
     dst->ptr[i]=dsts[i];
     prevSrc->ptr[i]=(unsigned char*)psrcs[i];
    }

   if (TopFirst==-1)
    TopFirst = tff?1: 0;

   if (useFrame<0)
    return;

   int SearchEffortW=0;

   if (useFrame)
    {
     SearchEffortW = searchEffort;
     // prevSrc = child->GetFrame(useFrame-1, env);
     pWeaveSrcP = TopFirst ? prevSrc->ptr[0] + src_pitch : prevSrc->ptr[0];
     pCopySrcP  = TopFirst ? prevSrc->ptr[0] + src_pitch : prevSrc->ptr[0];
    }

   if (IsYUY2)
    {
     const unsigned char *srcp = src->ptr[0];
     unsigned char *dstp = dst->ptr[0];
     src_pitch = src->stride[0];
     stride_t dst_pitch = dst->stride[0];
     int rowsize = dst->dx[0]*2;
     int height = dst->dy[0];
     int FldHeight = height / 2;
     if (useFrame)
      {
       pWeaveSrcP = TopFirst ? prevSrc->ptr[0] + src_pitch : prevSrc->ptr[0];
       pCopySrcP  = TopFirst ? prevSrc->ptr[0] + src_pitch : prevSrc->ptr[0];
      }
     ProcessFrame(SearchEffortW,srcp,dstp,dst_pitch,rowsize,FldHeight,height);       // table and then write the frame
    }
   else
    {
     // For YV12 we are going to cheat and deinterlace each plane separately as if it was
     // the whole frame.  This is a quick hack, but probably will be okay.
     // First, the Y plane, pretending it is the whole frame
     const unsigned char *srcp=src->GetReadPtr(PLANAR_Y);
     unsigned char *dstp = dst->GetWritePtr(PLANAR_Y);
     src_pitch = src->GetPitch(PLANAR_Y);
     stride_t dst_pitch = dst->GetPitch(PLANAR_Y);
     int rowsize = dst->GetRowSize(PLANAR_Y);   // Could also be PLANAR_Y_ALIGNED which would return a mod16 row_size
     int height = dst->GetHeight(PLANAR_Y);
     int FldHeight = height / 2;
     if (useFrame)
      {
       pWeaveSrcP = TopFirst ? prevSrc->GetReadPtr(PLANAR_Y) + src_pitch : prevSrc->GetReadPtr(PLANAR_Y);
       pCopySrcP  = TopFirst ? prevSrc->GetReadPtr(PLANAR_Y) + src_pitch : prevSrc->GetReadPtr(PLANAR_Y);
      }
     ProcessFrame_YV12(SearchEffortW,srcp,dstp,dst_pitch,rowsize,FldHeight,height);       // table and then write the frame

     // Next, the U plane
     srcp = src->GetReadPtr(PLANAR_U);
     dstp = dst->GetWritePtr(PLANAR_U);
     src_pitch = src->GetPitch(PLANAR_U);
     dst_pitch = dst->GetPitch(PLANAR_U);
     rowsize = dst->GetRowSize(PLANAR_U);  // Could also be PLANAR_U_ALIGNED which would return a mod8 row_size
     height = dst->GetHeight(PLANAR_U);
     FldHeight = height / 2;
     if (useFrame)
      {
       pWeaveSrcP = TopFirst ? prevSrc->GetReadPtr(PLANAR_U) + src_pitch : prevSrc->GetReadPtr(PLANAR_U);
       pCopySrcP  = TopFirst ? prevSrc->GetReadPtr(PLANAR_U) + src_pitch : prevSrc->GetReadPtr(PLANAR_U);
      }
     ProcessFrame_YV12(SearchEffortW,srcp,dstp,dst_pitch,rowsize,FldHeight,height);       // table and then write the frame

     // And the V plane, same sizes as U, different locations
     srcp = src->GetReadPtr(PLANAR_V);
     dstp = dst->GetWritePtr(PLANAR_V);
     if (useFrame)
      {
       pWeaveSrcP = TopFirst ? prevSrc->GetReadPtr(PLANAR_V) + src_pitch : prevSrc->GetReadPtr(PLANAR_V);
       pCopySrcP  = TopFirst ? prevSrc->GetReadPtr(PLANAR_V) + src_pitch : prevSrc->GetReadPtr(PLANAR_V);
      }
     ProcessFrame_YV12(SearchEffortW,srcp,dstp,dst_pitch,rowsize,FldHeight,height);       // table and then write the frame
    }
   PrevFrame = useFrame;
   PrevInFrame = inFrame;

   _mm_empty();
  }
};

extern "C" ItomsMoComp* createI(void)
{
 return new TomsMoComp;
}

extern "C" void __stdcall getVersion(char *ver,const char* *license)
{
 strcpy(ver,"1.0.1.8, build date "__DATE__" "__TIME__" ("COMPILER COMPILER_X64")");
 *license="(c) 2002 Tom Barry";
}
