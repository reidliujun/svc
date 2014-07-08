// Avisynth v2.5.  Copyright 2002 Ben Rudiak-Gould et al.
// http://www.avisynth.org

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
// http://www.gnu.org/copyleft/gpl.html .
//
// Linking Avisynth statically or dynamically with other modules is making a
// combined work based on Avisynth.  Thus, the terms and conditions of the GNU
// General Public License cover the whole combination.
//
// As a special exception, the copyright holders of Avisynth give you
// permission to link Avisynth with independent modules that communicate with
// Avisynth solely through the interfaces defined in avisynth.h, regardless of the license
// terms of these independent modules, and to copy and distribute the
// resulting combined work under terms of your choice, provided that
// every copy of the combined work is accompanied by a complete copy of
// the source code of Avisynth (the version of Avisynth used to produce the
// combined work), being distributed under the terms of the GNU General
// Public License plus this exception.  An independent module is a module
// which is not derived from or based on Avisynth, such as 3rd-party filters,
// import and export plugins, or graphical user interfaces.

#ifndef _CONVERT_YV12_H_
#define _CONVERT_YV12_H_

#include "simd.h"

#pragma warning(push)
#pragma warning(disable: 4700 4701 4799)

/********************************
 * (c) Copyright 2003, Klaus Post
 *
 * Converts 8x2 (8 pixels, two lines) in parallel.
 * Requires mod8 pitch for output, and mod16 pitch for input.
 ********************************/
template<class _mm> struct TconvertYV12
{
 static void yuy2_to_yv12(const BYTE* src, int src_rowsize, stride_t src_pitch,
                          BYTE* dstY, BYTE* dstU, BYTE* dstV, stride_t dst_pitchY, stride_t dst_pitchUV,
                          int height)
  {
   if (_mm::align && (intptr_t(src)&15 || intptr_t(dstY)&15 || intptr_t(dstU)&15 || intptr_t(dstV)&15 || src_pitch&15 || dst_pitchY &15 ||dst_pitchUV&15))
    {
     TconvertYV12<typename _mm::T64>::yuy2_to_yv12(src,src_rowsize,src_pitch,dstY,dstU,dstV,dst_pitchY,dst_pitchUV,height);
     return;
    }
   typename _mm::__m mask1 = _mm::set_pi32(0x00ff00ff,0x00ff00ff);
   typename _mm::__m mask2 = _mm::set_pi32(0xff00ff00,0xff00ff00);

   stride_t src_pitch2 = src_pitch*2;
   stride_t dst_pitch2 = dst_pitchY*2;

   src_rowsize = (src_rowsize+3)/4;
   typename _mm::__m mm7,mm4,mm0,mm1,mm3,mm6,mm2,mm5;
   movq (mm7,mask2);
   movq (mm4,mask1);
   for (int y=0;y<height;y+=2,dstY+=dst_pitch2,dstU+=dst_pitchUV,dstV+=dst_pitchUV,src+=src_pitch2)
    {
     const BYTE *esi=src;
     for (int edx=0;edx<src_rowsize;edx+=_mm::size/2,esi+=_mm::size*2)
      {
       movq (mm0,esi);        // YUY2 upper line  (4 pixels luma, 2 chroma)
        movq (mm1,esi+src_pitch);   // YUY2 lower line
       movq (mm6,mm0);
        movq (mm2, esi+_mm::size);    // Load second pair
       movq (mm3, esi+src_pitch+_mm::size);
        movq (mm5,mm2);
       _mm::pavgb (mm6,mm1);         // Average (chroma)
        _mm::pavgb (mm5,mm3);        // Average Chroma (second pair)
       pand (mm0,mm4);          // Mask luma
        psrlq (mm5, 8);
       pand (mm1,mm4);          // Mask luma
        psrlq (mm6, 8);
       pand (mm2,mm4);          // Mask luma
        pand (mm3,mm4);
       pand (mm5,mm4);          // Mask chroma
        pand (mm6,mm4);         // Mask chroma
       packuswb (mm0, mm2);     // Pack luma (upper)
        packuswb (mm6, mm5);    // Pack chroma
       packuswb (mm1, mm3);     // Pack luma (lower)
        movq (mm5, mm6);        // Chroma copy
       pand (mm5, mm7);         // Mask V
        pand (mm6, mm4);        // Mask U
       psrlq (mm5,8);           // shift down V
        packuswb (mm5, mm7);    // Pack U
       packuswb (mm6, mm7);     // Pack V
       movq (dstY+edx*2,mm0);
        movq (dstY+dst_pitchY+edx*2,mm1);
       movd (dstV+edx, mm5);   // Store V
        movd (dstU+edx, mm6);  // Store U
      }
    }
   _mm::sfence();
   _mm::empty();
  }
 static void yuy2_i_to_yv12(const BYTE* src, int src_rowsize, stride_t src_pitch,
                            BYTE* dstY, BYTE* dstU, BYTE* dstV, stride_t dst_pitchY, stride_t dst_pitchUV,
                            int height)
  {
   if (_mm::align && (intptr_t(src)&15 || intptr_t(dstY)&15 || intptr_t(dstU)&15 || intptr_t(dstV)&15 || src_pitch&15 || dst_pitchY &15 ||dst_pitchUV&15))
    {
     TconvertYV12<typename _mm::T64>::yuy2_i_to_yv12(src,src_rowsize,src_pitch,dstY,dstU,dstV,dst_pitchY,dst_pitchUV,height);
     return;
    }
   typename _mm::__m mask1   =_mm::set_pi32(0x00ff00ff,0x00ff00ff);
   typename _mm::__m mask2   =_mm::set_pi32(0xff00ff00,0xff00ff00);
   typename _mm::__m add_ones=_mm::set1_pi8(0x01);

   stride_t src_pitch2 = src_pitch*2;
   stride_t dst_pitch2 = dst_pitchY*2;
   stride_t src_pitch4 = src_pitch*4;
   stride_t dst_pitch3 = dst_pitchY*3;

   src_rowsize = (src_rowsize+3)/4;
   typename _mm::__m mm7,mm4,mm0,mm1,mm3,mm6,mm2,mm5;
   movq (mm7,mask2);
   movq (mm4,mask1);
   for (int y=0;y<height;y+=4)
    {
     const BYTE *esi=src;
     int edx;
     for (edx=0;edx<src_rowsize;edx+=_mm::size/2,esi+=_mm::size*2)
      {
       movq (mm0,esi);        // YUY2 upper line  (4 pixels luma, 2 chroma)
        movq (mm1,esi+src_pitch2);   // YUY2 lower line
       movq (mm6,mm0);
        movq (mm2, esi+_mm::size);    // Load second pair
       movq (mm3, esi+src_pitch2+_mm::size);
        movq (mm5,mm2);

       _mm::pavgb (mm6,mm1);         // Average (chroma)
        _mm::pavgb (mm5,mm3);        // Average Chroma (second pair)
       psubusb (mm5, add_ones);         // Better rounding (thanks trbarry!)
        psubusb (mm6, add_ones);
       _mm::pavgb (mm6,mm0);         // Average (chroma) (upper = 75% lower = 25%)
        _mm::pavgb (mm5,mm2);        // Average Chroma (second pair) (upper = 75% lower = 25%)

       pand (mm0,mm4 );         // Mask luma
        psrlq (mm5, 8);
       pand (mm1,mm4 );         // Mask luma
        psrlq (mm6, 8);
       pand (mm2,mm4 );         // Mask luma
        pand (mm3,mm4);
       pand (mm5,mm4 );          // Mask chroma
        pand (mm6,mm4);          // Mask chroma
       packuswb (mm0, mm2 );    // Pack luma (upper)
        packuswb (mm6, mm5);    // Pack chroma
       packuswb (mm1, mm3 );    // Pack luma (lower)
        movq (mm5, mm6);        // Chroma copy
       pand (mm5, mm7 );        // Mask V
        pand (mm6, mm4);        // Mask U
       psrlq (mm5,8);            // shift down V
        packuswb (mm5, mm7);     // Pack U
       packuswb (mm6, mm7 );    // Pack V
       movq (dstY+edx*2,mm0);
       movq (dstY+dst_pitchY*2+edx*2,mm1);
       movd (dstV+edx, mm5);   // Store V
       movd (dstU+edx, mm6);  // Store U
      }
     dstY+=dst_pitchY;
     dstU+=dst_pitchUV;
     dstV+=dst_pitchUV;
     esi=src+src_pitch;
     for (edx=0;edx<src_rowsize;edx+=_mm::size/2,esi+=_mm::size*2)
      {
       movq (mm0,esi);        // YUY2 upper line  (4 pixels luma, 2 chroma)
        movq (mm1,esi+src_pitch2);   // YUY2 lower line
       movq (mm6,mm0);
        movq (mm2, esi+_mm::size);    // Load second pair
       movq (mm3, esi+src_pitch2+_mm::size);
        movq (mm5,mm2);

       _mm::pavgb (mm6,mm1);         // Average (chroma)
        _mm::pavgb (mm5,mm3);        // Average Chroma (second pair)
       psubusb (mm5, add_ones);         // Better rounding (thanks trbarry!)
        psubusb (mm6, add_ones);
       _mm::pavgb (mm6,mm1);         // Average (chroma) (upper = 25% lower = 75%)
        _mm::pavgb (mm5,mm3);        // Average Chroma (second pair) (upper = 25% lower = 75%)

       pand (mm0,mm4);          // Mask luma
        psrlq (mm5, 8);
       pand (mm1,mm4);          // Mask luma
        psrlq (mm6, 8);
       pand (mm2,mm4);          // Mask luma
        pand (mm3,mm4);
       pand (mm5,mm4);           // Mask chroma
        pand (mm6,mm4);          // Mask chroma
       packuswb (mm0, mm2);     // Pack luma (upper)
        packuswb (mm6, mm5);    // Pack chroma
       packuswb (mm1, mm3);     // Pack luma (lower)
        movq (mm5, mm6);        // Chroma copy
       pand (mm5, mm7);         // Mask V
        pand (mm6, mm4);        // Mask U
       psrlq (mm5,8);            // shift down V
        packuswb (mm5, mm7);     // Pack U
       packuswb (mm6, mm7);     // Pack V

       movq (dstY+edx*2,mm0);
       movq (dstY+(dst_pitchY*2)+edx*2,mm1);
       movd (dstV+edx, mm5);   // Store V
       movd (dstU+edx, mm6);  // Store U
      }
     dstY+=dst_pitch3;
     dstU+=dst_pitchUV;
     dstV+=dst_pitchUV;
     src+=src_pitch4;
    }
   _mm::sfence();
   _mm::empty();
  }

 static void yv12_to_yuy2(const BYTE* srcY, const BYTE* srcU, const BYTE* srcV, int src_rowsize, stride_t src_pitch, stride_t src_pitch_uv,
                          BYTE* dst, stride_t dst_pitch,
                          int height)
  {
   if (_mm::align && (intptr_t(srcY)&15 || intptr_t(srcU)&15 || intptr_t(srcV)&15 || intptr_t(dst)&15 || src_pitch&15 || src_pitch_uv&15 || dst_pitch&15))
    {
     TconvertYV12<typename _mm::T64>::yv12_to_yuy2(srcY,srcU,srcV,src_rowsize,src_pitch,src_pitch_uv,dst,dst_pitch,height);
     return;
    }
   stride_t src_pitch_uv2 = src_pitch_uv*2;
   int skipnext = 0;

   stride_t dst_pitch2=dst_pitch*2;
   stride_t src_pitch2 = src_pitch*2;

   /**** Do first and last lines - NO interpolation:   *****/
   // MMX loop relies on C-code to adjust the lines for it.

   const BYTE* _srcY=srcY;
   const BYTE* _srcU=srcU;
   const BYTE* _srcV=srcV;
   BYTE* _dst=dst;
   for (int i=0;i<4;i++)
    {
     switch (i)
      {
       case 1:
        _srcY+=src_pitch;  // Same chroma as in 0
        _dst+=dst_pitch;
        break;
       case 2:
        _srcY=srcY+(src_pitch*(height-2));
        _srcU=srcU+(src_pitch_uv*((height>>1)-1));
        _srcV=srcV+(src_pitch_uv*((height>>1)-1));
        _dst = dst+(dst_pitch*(height-2));
        break;
       case 3: // Same chroma as in 4
        _srcY += src_pitch;
        _dst += dst_pitch;
        break;
       default:  // Nothing, case 0
        break;
      }
     typename _mm::__m mm7=_mm::setzero_si64();
     unsigned char *edi=_dst;
     const unsigned char *eax=_srcY;
     const unsigned char *ebx=_srcU;
     const unsigned char *ecx=_srcV;
     for (int edx=0;edx<src_rowsize;edx+=_mm::size,eax+=_mm::size,ebx+=_mm::size/2,ecx+=_mm::size/2,edi+=_mm::size*2)
      {
       typename _mm::__m mm0,mm1,mm3,mm2,mm4,mm5;
       movq (mm0,eax);    //Y
        movd (mm1,ebx);  //U
       movq (mm3,mm0);
        movd (mm2,ecx);  //V
       punpcklbw (mm0,mm7);  // Y low
        punpckhbw (mm3,mm7);   // Y high
       punpcklbw (mm1,mm7);   // 00uu 00uu
        punpcklbw (mm2,mm7);   // 00vv 00vv
       movq (mm4,mm1);
        movq (mm5,mm2);
       punpcklbw (mm1,mm7);   // 0000 00uu low
        punpcklbw (mm2,mm7);   // 0000 00vv low
       punpckhbw (mm4,mm7);   // 0000 00uu high
        punpckhbw (mm5,mm7);   // 0000 00vv high
       pslld (mm1,8);
        pslld (mm4,8);
       pslld (mm2,24);
        pslld (mm5,24);
       por (mm0, mm1);
        por (mm3, mm4);
       por (mm0, mm2);
        por (mm3, mm5);
       movq (edi,mm0);
        movq (edi+_mm::size,mm3);
      }
    }

  /****************************************
   * Conversion main loop.
   * The code properly interpolates UV from
   * interlaced material.
   * We process two lines in the same field
   * in the same loop, to avoid reloading
   * chroma each time.
   *****************************************/

   height-=4;

   dst+=dst_pitch2;
   srcY+=src_pitch2;
   srcU+=src_pitch_uv;
   srcV+=src_pitch_uv;

   const BYTE *srcp[3];
   srcp[0] = srcY;
   srcp[1] = srcU-src_pitch_uv;
   srcp[2] = srcV-src_pitch_uv;

   typename _mm::__m add_ones=_mm::set1_pi8(1);
   for (int y=0;y<height;y+=2,dst+=dst_pitch2,srcp[0]+=src_pitch2,srcp[1]+=src_pitch_uv,srcp[2]+=src_pitch_uv)
    {
     unsigned char *edi=dst;
     const unsigned char *eax=srcp[0];
     const unsigned char *ebx=srcp[1];
     const unsigned char *ecx=srcp[2];
     for (int x=0;x<src_rowsize;x+=_mm::size,edi+=_mm::size*2,eax+=_mm::size,ebx+=_mm::size/2,ecx+=_mm::size/2)
      {
       typename _mm::__m mm6=add_ones;
       //int edx= src_pitch_uv;
       typename _mm::__m mm0,mm7=_mm::setzero_si64(),mm2,mm3,mm4,mm1,mm5;
       movq (mm0,eax);          // mm0 = Y current line
       movd (mm2,ebx+src_pitch_uv);            // mm2 = U top field
        movd (mm3, ecx+src_pitch_uv);          // mm3 = V top field
       movd (mm4,ebx);        // U prev top field
        movq (mm1,mm0);             // mm1 = Y current line
       movd (mm5,ecx);        // V prev top field
        _mm::pavgb (mm4,mm2);            // interpolate chroma U  (25/75)
       _mm::pavgb (mm5,mm3 );            // interpolate chroma V  (25/75)
        psubusb (mm4, mm6);         // Better rounding (thanks trbarry!)
       psubusb (mm5, mm6 );
        _mm::pavgb (mm4,mm2   );         // interpolate chroma U
       _mm::pavgb (mm5,mm3    );         // interpolate chroma V
        punpcklbw (mm0,mm7);        // Y low
       punpckhbw (mm1,mm7 );        // Y high*
        punpcklbw (mm4,mm7);        // U 00uu 00uu 00uu 00uu
       punpcklbw (mm5,mm7 );        // V 00vv 00vv 00vv 00vv
        pxor (mm6,mm6     );
       punpcklbw (mm6,mm4 );        // U 0000 uu00 0000 uu00 (low)
        punpckhbw (mm7,mm4);         // V 0000 uu00 0000 uu00 (high
       por (mm0,mm6 );
        por (mm1,mm7);
       movq (mm6,mm5);
        punpcklbw (mm5,mm5);          // V 0000 vvvv 0000 vvvv (low)
       punpckhbw (mm6,mm6);           // V 0000 vvvv 0000 vvvv (high)
        pslld (mm5,24);
       pslld (mm6,24);
        por (mm0,mm5);
       por (mm1,mm6);
       //mov edx, src_pitch_uv2
        movq (edi,mm0);
       movq (edi+_mm::size,mm1);

       //Next line

        movq (mm6,add_ones);
       movd (mm4,ebx+src_pitch_uv2);        // U next top field
        movd (mm5,ecx+src_pitch_uv2);       // V prev top field
       //mov edx, [src_pitch]
        pxor (mm7,mm7);
       movq (mm0,eax+src_pitch);        // Next U-line
        _mm::pavgb (mm4,mm2);            // interpolate chroma U
       movq (mm1,mm0);             // mm1 = Y current line
       _mm::pavgb (mm5,mm3);             // interpolate chroma V
        psubusb (mm4, mm6);         // Better rounding (thanks trbarry!)
       psubusb (mm5, mm6 );
        _mm::pavgb (mm4,mm2   );         // interpolate chroma U
       _mm::pavgb (mm5,mm3    );         // interpolate chroma V
        punpcklbw (mm0,mm7);        // Y low
       punpckhbw (mm1,mm7 );        // Y high*
        punpcklbw (mm4,mm7);        // U 00uu 00uu 00uu 00uu
       punpcklbw (mm5,mm7 );        // V 00vv 00vv 00vv 00vv
        pxor (mm6,mm6     );
       punpcklbw (mm6,mm4 );        // U 0000 uu00 0000 uu00 (low)
        punpckhbw (mm7,mm4);         // V 0000 uu00 0000 uu00 (high
       por (mm0,mm6 );
        por (mm1,mm7);
       movq (mm6,mm5);
        punpcklbw (mm5,mm5);          // V 0000 vvvv 0000 vvvv (low)
       punpckhbw (mm6,mm6);           // V 0000 vvvv 0000 vvvv (high)
        pslld (mm5,24);
       //mov edx,[dst_pitch]
       pslld (mm6,24);
        por (mm0,mm5);
       por (mm1,mm6);
        movq (edi+dst_pitch,mm0);
       movq (edi+dst_pitch+_mm::size,mm1);
      }
    }
   _mm::sfence();
   _mm::empty();
  }
 static void yv12_i_to_yuy2(const BYTE* srcY, const BYTE* srcU, const BYTE* srcV, int src_rowsize, stride_t src_pitch, stride_t src_pitch_uv,
                            BYTE* dst, stride_t dst_pitch,
                            int height)
  {
   if (_mm::align && (intptr_t(srcY)&15 || intptr_t(srcU)&15 || intptr_t(srcV)&15 || intptr_t(dst)&15 || src_pitch&15 || src_pitch_uv&15 || dst_pitch&15))
    {
     TconvertYV12<typename _mm::T64>::yv12_i_to_yuy2(srcY,srcU,srcV,src_rowsize,src_pitch,src_pitch_uv,dst,dst_pitch,height);
     return;
    }
   stride_t src_pitch_uv2 = src_pitch_uv*2;
   stride_t src_pitch_uv4 = src_pitch_uv*4;
   int skipnext = 0;

   stride_t dst_pitch2=dst_pitch*2;
   stride_t src_pitch2 = src_pitch*2;

   stride_t dst_pitch4 = dst_pitch*4;
   stride_t src_pitch4 = src_pitch*4;


   /**** Do first and last lines - NO interpolation:   *****/
   // MMX loop relies on C-code to adjust the lines for it.
   const BYTE* _srcY=srcY;
   const BYTE* _srcU=srcU;
   const BYTE* _srcV=srcV;
   BYTE* _dst=dst;
  //
   for (int i=0;i<8;i++)
    {
     switch (i)
      {
       case 1:
        _srcY+=src_pitch2;  // Same chroma as in 0
        _dst+=dst_pitch2;
        break;
       case 2:
        _srcY-=src_pitch;  // Next field
        _dst-=dst_pitch;
        _srcU+=src_pitch_uv;
        _srcV+=src_pitch_uv;
        break;
       case 3:
        _srcY+=src_pitch2;  // Same  chroma as in 2
        _dst+=dst_pitch2;
        break;
       case 4: // Now we process the bottom four lines of the picture.
        _srcY=srcY+(src_pitch*(height-4));
        _srcU=srcU+(src_pitch_uv*((height>>1)-2));
        _srcV=srcV+(src_pitch_uv*((height>>1)-2));
        _dst = dst+(dst_pitch*(height-4));
        break;
       case 5: // Same chroma as in 4
        _srcY += src_pitch2;
        _dst += dst_pitch2;
        break;
       case 6:  // Next field
        _srcY -= src_pitch;
        _dst -= dst_pitch;
        _srcU+=src_pitch_uv;
        _srcV+=src_pitch_uv;
        break;
       case 7:  // Same chroma as in 6
        _srcY += src_pitch2;
        _dst += dst_pitch2;
       default:  // Nothing, case 0
        break;
      }

     unsigned char *edi=_dst;
     const unsigned char *eax=_srcY;
     const unsigned char *ebx=_srcU;
     const unsigned char *ecx=_srcV;
     typename _mm::__m mm7=_mm::setzero_si64();
     for (int edx=0;edx<src_rowsize;edx+=_mm::size,eax+=_mm::size,ebx+=_mm::size/2,ecx+=_mm::size/2,edi+=_mm::size*2)
      {
       typename _mm::__m mm0,mm1,mm3,mm2,mm4,mm5;
       movq (mm0,eax);    //Y
        movd (mm1,ebx);  //U
       movq (mm3,mm0);
        movd (mm2,ecx);   //V
       punpcklbw (mm0,mm7);  // Y low
        punpckhbw (mm3,mm7);   // Y high
       punpcklbw (mm1,mm7 );  // 00uu 00uu
        punpcklbw (mm2,mm7);   // 00vv 00vv
       movq (mm4,mm1 );
        movq (mm5,mm2);
       punpcklbw (mm1,mm7 );  // 0000 00uu low
        punpcklbw (mm2,mm7);   // 0000 00vv low
       punpckhbw (mm4,mm7 );  // 0000 00uu high
        punpckhbw (mm5,mm7);   // 0000 00vv high
       pslld (mm1,8);
        pslld (mm4,8);
       pslld (mm2,24);
        pslld (mm5,24);
       por (mm0, mm1);
        por (mm3, mm4);
       por (mm0, mm2);
        por (mm3, mm5);
       movq (edi,mm0);
        movq (edi+_mm::size,mm3);
      }
    }

  /****************************************
   * Conversion main loop.
   * The code properly interpolates UV from
   * interlaced material.
   * We process two lines in the same field
   * in the same loop, to avoid reloading
   * chroma each time.
   *****************************************/

   height-=8;

   dst+=dst_pitch4;
   srcY+=src_pitch4;
   srcU+=src_pitch_uv2;
   srcV+=src_pitch_uv2;

   const BYTE *srcp[3];
   srcp[0] = srcY;
   srcp[1] = srcU-src_pitch_uv2;
   srcp[2] = srcV-src_pitch_uv2;

   typename _mm::__m add_ones=_mm::set1_pi8(1);
   for (int y=0;y<height;)
    {
     unsigned char *edi=dst;
     const unsigned char *eax=srcp[0];
     const unsigned char *ebx=srcp[1];
     const unsigned char *ecx=srcp[2];
    yloop:
     for (int x=0;x<src_rowsize;x+=_mm::size,edi+=_mm::size*2,eax+=_mm::size,ebx+=_mm::size/2,ecx+=_mm::size/2)
      {
       //mov edx, src_pitch_uv2
       typename _mm::__m mm6=add_ones,mm0,mm7,mm2,mm3,mm4,mm1,mm5;
       movq (mm0,eax);          // mm0 = Y current line
        pxor (mm7,mm7);
       movd (mm2,ebx+src_pitch_uv2);            // mm2 = U top field
        movd (mm3, ecx+src_pitch_uv2);          // mm3 = V top field
       movd (mm4,ebx);            // U prev top field
        movq (mm1,mm0);             // mm1 = Y current line
       movd (mm5,ecx);            // V prev top field
        _mm::pavgb (mm4,mm2);            // interpolate chroma U
       _mm::pavgb (mm5,mm3);             // interpolate chroma V
        psubusb (mm4, mm6);         // Better rounding (thanks trbarry!)
       psubusb (mm5, mm6);
        _mm::pavgb (mm4,mm2);            // interpolate chroma U
       _mm::pavgb (mm5,mm3);             // interpolate chroma V
       punpcklbw (mm0,mm7);        // Y low
       punpckhbw (mm1,mm7);         // Y high*
        punpcklbw (mm4,mm7);        // U 00uu 00uu 00uu 00uu
       punpcklbw (mm5,mm7);         // V 00vv 00vv 00vv 00vv
        pxor (mm6,mm6);
       punpcklbw (mm6,mm4);         // U 0000 uu00 0000 uu00 (low)
        punpckhbw (mm7,mm4);         // V 0000 uu00 0000 uu00 (high
       por (mm0,mm6);
        por (mm1,mm7);
       movq (mm6,mm5);
        punpcklbw (mm5,mm5);          // V 0000 vvvv 0000 vvvv (low)
       punpckhbw (mm6,mm6);           // V 0000 vvvv 0000 vvvv (high)
        pslld (mm5,24);
       pslld (mm6,24);
        por (mm0,mm5);
       por (mm1,mm6);
       //mov edx, src_pitch_uv4
        movq (edi,mm0);
       movq (edi+_mm::size,mm1);

       //Next line in same field
        movq (mm6, add_ones);
       movd (mm4,ebx+src_pitch_uv4);        // U next top field
        movd (mm5,ecx+src_pitch_uv4);       // V prev top field
       //mov edx, [src_pitch2]
        movq( mm0,eax+src_pitch2);        // Next Y-line
       _mm::pavgb (mm4,mm2);            // interpolate chroma U
        _mm::pavgb (mm5,mm3);             // interpolate chroma V
       psubusb (mm4, mm6);         // Better rounding (thanks trbarry!)
        psubusb (mm5, mm6);
       _mm::pavgb (mm4,mm2);            // interpolate chroma U
        _mm::pavgb (mm5,mm3);             // interpolate chroma V
       pxor (mm7,mm7);
       movq (mm1,mm0);             // mm1 = Y current line
        punpcklbw (mm0,mm7);        // Y low
       punpckhbw (mm1,mm7 );        // Y high*
        punpcklbw (mm4,mm7);        // U 00uu 00uu 00uu 00uu
       punpcklbw (mm5,mm7 );        // V 00vv 00vv 00vv 00vv
        pxor (mm6,mm6);
       punpcklbw (mm6,mm4);         // U 0000 uu00 0000 uu00 (low)
        punpckhbw (mm7,mm4);         // V 0000 uu00 0000 uu00 (high
       por (mm0,mm6 );
        por( mm1,mm7);
       movq (mm6,mm5);
        punpcklbw (mm5,mm5);          // V 0000 vvvv 0000 vvvv (low)
       punpckhbw (mm6,mm6);           // V 0000 vvvv 0000 vvvv (high)
        pslld (mm5,24);
       //mov edx,[dst_pitch2]
       pslld (mm6,24);
        por (mm0,mm5);
       por (mm1,mm6);
        movq (edi+dst_pitch2,mm0);
       movq (edi+dst_pitch2+_mm::size,mm1);
      }
     if (skipnext)
      {
       dst+=dst_pitch4;
       srcp[0]+=src_pitch4;
       srcp[1]+=src_pitch_uv2;
       srcp[2]+=src_pitch_uv2;
       skipnext=0;
       y+=4;
      }
     else
      {
       edi=dst;
       eax=srcp[0];
       ebx=srcp[1];
       ecx=srcp[2];
       edi+=dst_pitch;
       eax+=src_pitch;
       ebx+=src_pitch_uv;
       ecx+=src_pitch_uv;
       skipnext=1;
       if(y+4<=height)
        goto yloop;
      }
    }
   _mm::sfence();
   _mm::empty();
  }
};

#pragma warning(pop)

#endif
