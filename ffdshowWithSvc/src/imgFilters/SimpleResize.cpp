//  Simple (faster) resize for avisynth
//      Copyright (C) 2002 Tom Barry
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
//  Also, this program is "Philanthropy-Ware".  That is, if you like it and
//  feel the need to reward or inspire the author then please feel free (but
//  not obligated) to consider joining or donating to the Electronic Frontier
//  Foundation. This will help keep cyber space free of barbed wire and bullsh*t.
//
//  See their web page at www.eff.org

// Changes:
//
// Jan 2003 0.3.3.0 Avisynth 2.5 YV12 support, AvisynthPluginit2
// Feb 2002 0.3.0.0 Added InterlacedResize support
// Jan 2002 0.2.0.0 Some rather ineffectual P3 SSE optimizations
// Jan 2002 0.1.0.0 First release

#include "stdafx.h"
#include "SimpleResize.h"
#include "Tconfig.h"
#include "simd.h"

#pragma warning(push)
#pragma warning(disable: 4700 4701)

__align16(const int64_t,SimpleResize::YMask[2]   ) = {0x00ff00ff00ff00ffLL,0x00ff00ff00ff00ffLL}; // keeps only luma
__align16(const int64_t,SimpleResize::FPround1[2]) = {0x0080008000800080LL,0x0080008000800080LL}; // round words
__align16(const int64_t,SimpleResize::FPround2[2]) = {0x0000008000000080LL,0x0000008000000080LL}; // round dwords

SimpleResize::SimpleResize(const VideoInfo &vi,unsigned int _width, unsigned int _height, double _hWarp, double _vWarp, bool _Interlaced)
{
 ok=false;
                minslope = 0.30;                        // don't overstretch
                oldwidth = vi.width;
                oldheight = vi.height;
                newwidth = _width;
                newheight = _height;
                hWarp =  _hWarp;                // 1.15 remember I used hw=1.15, vw=.95 for sample
                vWarp=  _vWarp;                 // .95
                Interlaced = _Interlaced;
                SSE2enabled = Tconfig::cpu_flags&FF_CPU_SSE2?true:false;
                SSEMMXenabled = Tconfig::cpu_flags&FF_CPU_MMXEXT?true:false;

                vOffsetsUV=vWeightsUV=hControl=vWorkY=vWorkUV=vOffsets=vWeights=NULL;
                if (vi.IsYUY2)
                {
                        DoYV12 = false;
                }
                else if (vi.IsYV12)
                {
                        if (Interlaced)
                        {
                                ;//env->ThrowError("InterlacedResize: Interlace not supported for YV12 yet");
                        }
                        DoYV12 = true;
                        vOffsetsUV = (int*) aligned_malloc(newheight*4,128);
                        vWeightsUV = (int*) aligned_malloc(newheight*4,128);

                        if (!vOffsetsUV || !vWeightsUV)
                        {
                                return;
                        }
                }

                else
                {
                        return;
                }

                // 2 qwords, 2 offsets, and prefetch slack
            hControl = (int*) aligned_malloc(newwidth*12+128, 128);   // aligned for P4 cache line
            vWorkY   = (int*) aligned_malloc(2*oldwidth+128, 128);
            vWorkUV  = (int*) aligned_malloc(oldwidth+128, 128);
            vOffsets = (int*) aligned_malloc(newheight*4, 128);
            vWeights = (int*) aligned_malloc(newheight*4, 128);

                if (!hControl || !vWeights)
                {
                        return;
                }
                if (DoYV12)
                {
                        InitTables_YV12();
                }
                else
                {
                        InitTables();
                }
 ok=true;
}

SimpleResize::~SimpleResize()
{
 if (vOffsetsUV) aligned_free(vOffsetsUV);
 if (vWeightsUV) aligned_free(vWeightsUV);
 if (hControl  ) aligned_free(hControl);
 if (vWorkY    ) aligned_free(vWorkY);
 if (vWorkUV   ) aligned_free(vWorkUV);
 if (vOffsets  ) aligned_free(vOffsets);
 if (vWeights  ) aligned_free(vWeights);
}

void SimpleResize::GetFrame(const PVideoFrame *src,PVideoFrame *dst)
{

        if (DoYV12)
        {
                GetFrame_YV12( src, dst, PLANAR_Y);
                GetFrame_YV12( src, dst, PLANAR_U);
                GetFrame_YV12( src, dst, PLANAR_V);
        }
        else
        {
                GetFrame_YUY2( src, dst, PLANAR_Y);
        }
}

// YV12 Luma
void SimpleResize::GetFrame_YV12(const PVideoFrame *src, PVideoFrame *dst, int Planar_Type)
{
        int vWeight1[4];
        int vWeight2[4];

        const BYTE* srcp = src->ptr[Planar_Type];
        const BYTE* srcp2W = srcp;
        BYTE* dstp=dst->ptr[Planar_Type];
        BYTE* dstp2 = dst->ptr[Planar_Type];

        //      BYTE* dstp = dst->GetWritePtr(Planar_Type);
        const stride_t src_pitch = src->pitch[Planar_Type];
        const stride_t dst_pitch = dst->pitch[Planar_Type];
        const int src_row_size = src->rowSize[Planar_Type];
        const int row_size = dst->rowSize[Planar_Type];
        const int height = dst->height[Planar_Type];

        const int* pControl = &hControl[0];
        const unsigned char* srcp1;
        const unsigned char* srcp2;
        int* vWorkYW = vWorkY;

        int* vOffsetsW = (Planar_Type == PLANAR_Y)
                ? vOffsets
                : vOffsetsUV;

        int* vWeightsW = (Planar_Type == PLANAR_Y)
                ? vWeights
                : vWeightsUV;

        // Just in case things are not aligned right, maybe turn off sse2
        #ifdef __SSE2__
        __m128i xmm0,xmm5,xmm6,xmm7,xmm1,xmm2,xmm3,xmm4;
        #endif
        __m64 mm5,mm6,mm0,mm7,mm1,mm2,mm3,mm4;
        for (int y = 0; y < height; y++)
        {

                vWeight1[0] = vWeight1[1] = vWeight1[2] = vWeight1[3] =
                        (256-vWeightsW[y]) << 16 | (256-vWeightsW[y]);
                vWeight2[0] = vWeight2[1] = vWeight2[2] = vWeight2[3] =
                        vWeightsW[y] << 16 | vWeightsW[y];

                srcp1 = srcp + vOffsetsW[y] * src_pitch;

                if (Interlaced)
                {
                        srcp2 = (y < height-2)
                                ? srcp1 + 2 * src_pitch
                                : srcp1;
                }
                else
                {
                        srcp2 = (y < height-1)
                                ? srcp1 + src_pitch
                                : srcp1;
                }

                        int             ecx= src_row_size,ebx;
                        ecx>>=3;                                  // 8 bytes a time
                        const unsigned char *esi= srcp1;                              // top of 2 src lines to get
                        const unsigned char *edx= srcp2;                              // next "
                        unsigned char *edi= (unsigned char*)vWorkYW;                    // luma work destination line
                        int eax=0;

// Let's check here to see if we are on a P4 or higher and can use SSE2 instructions.
// This first loop is not the performance bottleneck anyway but it is trivial to tune
// using SSE2 if we have proper alignment.
#ifdef __SSE2__
                        if (SSE2enabled==0)                  // is SSE2 supported?
                         goto vMaybeSSEMMX;                            // n, can't do anyway

                        if (ecx< 2)                                  // we have at least 16 byts, 2 qwords?
                         goto vMaybeSSEMMX;                            // n, don't bother

                        if ((intptr_t(esi)|intptr_t(edx))&0xf)                               // both src rows 16 byte aligned?
                         goto vMaybeSSEMMX;                    // n, don't use sse2

                        ecx>>=1;                                // do 16 bytes at a time instead
                        ecx--;                                  // jigger loop ct
                        //align       16
                        movdqu  (xmm0, FPround1);
                        movdqu  (xmm5, vWeight1);
                        movdqu  (xmm6, vWeight2);
                        pxor    (xmm7, xmm7);

                        //align   16
        vLoopSSE2_Fetch:
                        prefetcht0 (esi+eax*2+16);
                        prefetcht0 (edx+eax*2+16);

        vLoopSSE2:
                        movdqu  (xmm1, esi+eax); // top of 2 lines to interpolate
                        movdqu  (xmm3, edx+eax); // 2nd of 2 lines
                        movdqa  (xmm2, xmm1);
                        movdqa  (xmm4, xmm3);

                        punpcklbw (xmm1, xmm7);                    // make words
                        punpckhbw (xmm2, xmm7);                    // "
                        punpcklbw (xmm3, xmm7);                    // "
                        punpckhbw (xmm4, xmm7);                    // "

                        pmullw  (xmm1, xmm5);                              // mult by top weighting factor
                        pmullw  (xmm2, xmm5);              // "
                        pmullw  (xmm3, xmm6);                              // mult by bot weighting factor
                        pmullw  (xmm4, xmm6);              // "

                        paddw   (xmm1, xmm3);                              // combine lumas low
                        paddw   (xmm2, xmm4);                              // combine lumas high

                        paddusw (xmm1, xmm0);                              // round
                        paddusw (xmm2, xmm0);                              // round

                        psrlw   (xmm1, 8);                                 // right adjust luma
                        psrlw   (xmm2, 8);                                 // right adjust luma

                        packuswb (xmm1, xmm2);                             // pack words to our 16 byte answer
                        movntdq (edi+eax, xmm1);      // save lumas in our work area

                        eax= eax+16;
                        ecx--;     // don
                        if (ecx>0) goto vLoopSSE2_Fetch;                 // if not on last one loop, prefetch
                        if (ecx==0) goto vLoopSSE2;                               // or just loop, or not

// done with our SSE2 fortified loop but we may need to pick up the spare change
                        _mm_sfence();
                        ecx= src_row_size;               // get count again
                        ecx&=0x0000000f;                 // just need mod 16
                        movq    (mm5, vWeight1);
                        movq    (mm6, vWeight2);
                        movq    (mm0, FPround1);                   // useful rounding constant
                        ecx>>=3;                                  // 8 bytes at a time, any?
                        if (ecx==0) goto MoreSpareChange;                 // n, did them all
#endif
// Let's check here to see if we are on a P2 or Athlon and can use SSEMMX instructions.
// This first loop is not the performance bottleneck anyway but it is trivial to tune
// using SSE if we have proper alignment.
        vMaybeSSEMMX:
                        movq    (mm5, vWeight1);
                        movq    (mm6, vWeight2);
                        movq    (mm0, FPround1);                   // useful rounding constant
                        pxor    (mm7, mm7);
                        if (SSEMMXenabled==0)                // is SSE supported?
                         goto vLoopMMX;                                // n, can't do anyway
                        ecx--;// jigger loop ctr

                    //align       16
        vLoopSSEMMX_Fetch:
                        prefetcht0 (esi+eax+8);
                        prefetcht0 (edx+eax+8);

        vLoopSSEMMX:
                        movq    (mm1, esi+eax); // top of 2 lines to interpolate
                        movq    (mm3, edx+eax); // 2nd of 2 lines
                        movq    (mm2, mm1);                                // copy top bytes
                        movq    (mm4, mm3);                                // copy 2nd bytes

                        punpcklbw (mm1, mm7);                              // make words
                        punpckhbw (mm2, mm7);                              // "
                        punpcklbw (mm3, mm7);                              // "
                        punpckhbw (mm4, mm7);                              // "

                        pmullw  (mm1, mm5);                                // mult by weighting factor
                        pmullw  (mm2, mm5);                                // mult by weighting factor
                        pmullw  (mm3, mm6);                                // mult by weighting factor
                        pmullw  (mm4, mm6);                                // mult by weighting factor

                        paddw   (mm1, mm3);                                // combine lumas
                        paddw   (mm2, mm4);                                // combine lumas

                        paddusw (mm1, mm0);                                // round
                        paddusw (mm2, mm0);                                // round

                        psrlw   (mm1, 8  );                                // right adjust luma
                        psrlw   (mm2, 8  );                                // right adjust luma

                        packuswb (mm1,mm2);                                // pack UV's into low dword

                        movntq  (edi+eax, mm1); // save in our work area

                        eax= eax+8;
                        ecx--;
                        if (ecx>0) goto vLoopSSEMMX_Fetch;                       // if not on last one loop, prefetch
                        if (ecx==0) goto vLoopSSEMMX;                             // or just loop, or not
                        _mm_sfence();
                        goto             MoreSpareChange;                 // all done with vertical

                    //align       16
        vLoopMMX:
                        movq    (mm1, esi+eax); // top of 2 lines to interpolate
                        movq    (mm3, edx+eax); // 2nd of 2 lines
                        movq    (mm2, mm1);                                // copy top bytes
                        movq    (mm4, mm3);                                // copy 2nd bytes

                        punpcklbw (mm1, mm7);                              // make words
                        punpckhbw (mm2, mm7);                              // "
                        punpcklbw (mm3, mm7);                              // "
                        punpckhbw (mm4, mm7);                              // "

                        pmullw  (mm1, mm5);                                // mult by weighting factor
                        pmullw  (mm2, mm5);                                // mult by weighting factor
                        pmullw  (mm3, mm6);                                // mult by weighting factor
                        pmullw  (mm4, mm6);                                // mult by weighting factor

                        paddw   (mm1, mm3);                                // combine lumas
                        paddw   (mm2, mm4);                                // combine lumas

                        paddusw (mm1, mm0);                                // round
                        paddusw (mm2, mm0);                                // round

                        psrlw   (mm1, 8);                                  // right just
                        psrlw   (mm2, 8);                                  // right just

                        packuswb (mm1,mm2);                                // pack UV's into low dword

                        movq    (edi+eax, mm1); // save lumas in our work area

                        eax= eax+8;
                        ecx--;if (ecx>0) goto vLoopMMX;

// Add a little code here to check if we have more pixels to do and, if so, make one
// more pass thru vLoopMMX. We were processing in multiples of 8 pixels and alway have
// an even number so there will never be more than 7 left.
        MoreSpareChange:
                        if (!(eax< src_row_size))               // did we get them all
                         goto DoHorizontal;// jnl             DoHorizontal                    // yes, else have 2 left
                        ecx= 1;                                  // jigger loop ct
                        eax= src_row_size;
                        eax-=8;                                  // back up to last 8 pixels
                        goto vLoopMMX;


// We've taken care of the vertical scaling, now do horizontal
        DoHorizontal:
                        pxor    (mm7, mm7);
                        movq    (mm6, FPround2);           // useful rounding constant, dwords
                        esi= (const unsigned char*)pControl;           // @ horiz control bytes
                        ecx= row_size;
                        ecx>>=2;                          // 4 bytes a time, 4 pixels
                        edx= (const unsigned char*)vWorkYW;            // our luma data
                        edi= dstp;                       // the destination line
                        if (SSEMMXenabled==0)                // is SSE2 supported?
                         goto hLoopMMX;                                // n

// With SSE support we will make 8 pixels (from 8 pairs) at a time
                        ecx >>=1;                          // 8 bytes a time instead of 4
                        if (ecx==0) goto LessThan8;
                        //align 16
        hLoopMMXSSE:
                        // handle first 2 pixels
                        eax= *(int*)(esi+16);           // get data offset in pixels, 1st pixel pair
                        ebx= *(int*)(esi+20);           // get data offset in pixels, 2nd pixel pair
                        movd    (mm0, edx+eax);          // copy luma pair 0000xxYY
                        punpcklwd (mm0, edx+ebx);    // 2nd luma pair, now xxxxYYYY
                        punpcklbw (mm0, mm7);                  // make words out of bytes, 0Y0Y0Y0Y
                        eax= *(int*)(esi+16+24);        // get data offset in pixels, 3rd pixel pair
                        ebx= *(int*)(esi+20+24);        // get data offset in pixels, 4th pixel pair
                        pmaddwd (mm0, esi);                      // mult and sum lumas by ctl weights
                        paddusw (mm0, mm6);                        // round
                        psrlw   (mm0, 8);                          // right just 4 luma pixel value 0Y0Y0Y0Y

                        // handle 3rd and 4th pixel pairs
                        movd    (mm1, edx+eax);          // copy luma pair 0000xxYY
                        punpcklwd (mm1, edx+ebx);    // 2nd luma pair, now xxxxYYYY
                        punpcklbw (mm1, mm7);                  // make words out of bytes, 0Y0Y0Y0Y
                        eax= *(int*)(esi+16+48);        // get data offset in pixels, 5th pixel pair
                        ebx= *(int*)(esi+20+48);        // get data offset in pixels, 6th pixel pair
                        pmaddwd (mm1, esi+24);           // mult and sum lumas by ctl weights
                        paddusw (mm1, mm6);                        // round
                        psrlw   (mm1, 8);                          // right just 4 luma pixel value 0Y0Y0Y0Y

                        // handle 5th and 6th pixel pairs
                        movd    (mm2, edx+eax);          // copy luma pair 0000xxYY
                        punpcklwd (mm2, edx+ebx);    // 2nd luma pair, now xxxxYYYY
                        punpcklbw (mm2, mm7);                  // make words out of bytes, 0Y0Y0Y0Y
                        eax=*(int*)(esi+16+72);        // get data offset in pixels, 7th pixel pair
                        ebx=*(int*)(esi+20+72);        // get data offset in pixels, 8th pixel pair
                        pmaddwd (mm2, esi+48);                   // mult and sum lumas by ctl weights
                        paddusw (mm2, mm6);                        // round
                        psrlw   (mm2, 8);                          // right just 4 luma pixel value 0Y0Y0Y0Y

                        // handle 7th and 8th pixel pairs
                        movd    (mm3, edx+eax);          // copy luma pair
                        punpcklwd (mm3, edx+ebx);    // 2nd luma pair
                        punpcklbw (mm3, mm7);                  // make words out of bytes
                        pmaddwd (mm3, esi+72);           // mult and sum lumas by ctl weights
                        paddusw (mm3, mm6);                        // round
                        psrlw   (mm3, 8);                          // right just 4 luma pixel value 0Y0Y0Y0Y

                        // combine, store, and loop
                        packuswb (mm0,mm1);                        // pack into qword, 0Y0Y0Y0Y
                        packuswb (mm2,mm3);                        // pack into qword, 0Y0Y0Y0Y
                        packuswb (mm0,mm2);                        // and again into  YYYYYYYY
                        movntq   (edi,mm0);     // done with 4 pixels

                        esi=esi+96;            // bump to next control bytest
                        edi=edi+8;                     // bump to next output pixel addr
                        ecx--;
                        if (ecx>0) goto hLoopMMXSSE;                          // loop for more
                        _mm_sfence();

        LessThan8:
                        ecx= row_size;
                        ecx&=7;                          // we have done all but maybe this
                        ecx>>=2;                          // now do only 4 bytes at a time
                        if (ecx==0) goto LessThan4;

                        //align 16
        hLoopMMX:
                        // handle first 2 pixels
                        eax=*(int*)(esi+16);           // get data offset in pixels, 1st pixel pair
                        ebx=*(int*)(esi+20);           // get data offset in pixels, 2nd pixel pair
                        movd    (mm0, edx+eax);          // copy luma pair 0000xxYY
                        punpcklwd (mm0, edx+ebx);    // 2nd luma pair, now xxxxYYYY
                        punpcklbw (mm0, mm7);                  // make words out of bytes, 0Y0Y0Y0Y
                        eax=*(int*)(esi+16+24);        // get data offset in pixels, 3rd pixel pair
                        ebx=*(int*)(esi+20+24);        // get data offset in pixels, 4th pixel pair
                        pmaddwd (mm0, esi);                      // mult and sum lumas by ctl weights
                        paddusw (mm0, mm6);                        // round
                        psrlw   (mm0, 8);                          // right just 4 luma pixel value 0Y0Y0Y0Y

                        // handle 3rd and 4th pixel pairs
                        movd    (mm1, edx+eax);          // copy luma pair
                        punpcklwd (mm1, edx+ebx);    // 2nd luma pair
                        punpcklbw (mm1, mm7);                  // make words out of bytes
                        pmaddwd (mm1, esi+24);                   // mult and sum lumas by ctl weights
                        paddusw (mm1, mm6);                        // round
                        psrlw   (mm1, 8);                          // right just 4 luma pixel value 0Y0Y0Y0Y

                        // combine, store, and loop
                        packuswb (mm0,mm1);                        // pack all into qword, 0Y0Y0Y0Y
                        packuswb (mm0,mm7);                        // and again into  0000YYYY
                        movd    (edi, mm0);     // done with 4 pixels
                        esi=esi+48;            // bump to next control bytest
                        edi=edi+4;                     // bump to next output pixel addr
                        ecx--;if (ecx>0) goto hLoopMMX;                             // loop for more

                        // test to see if we have a mod 4 size row, if not then more spare change
        LessThan4:
                        ecx= row_size;
                        ecx&=3;                          // remainder size mod 4
                        if (ecx< 2)
                         goto LastOne;                         // none, done

                        // handle 2 more pixels
                        eax=*(int*)(esi+16);           // get data offset in pixels, 1st pixel pair
                        ebx=*(int*)(esi+20);           // get data offset in pixels, 2nd pixel pair
                        movd    (mm0, edx+eax);          // copy luma pair 0000xxYY
                        punpcklwd (mm0, edx+ebx);    // 2nd luma pair, now xxxxYYYY
                        punpcklbw (mm0, mm7);                  // make words out of bytes, 0Y0Y0Y0Y

                        pmaddwd (mm0, esi);                      // mult and sum lumas by ctl weights
                        paddusw (mm0, mm6);                        // round
                        psrlw   (mm0, 8);                          // right just 2 luma pixel value 000Y,000Y
                        packuswb (mm0,mm7);                        // pack all into qword, 00000Y0Y
                        packuswb (mm0,mm7);                        // and again into  000000YY
                        movd    (edi, mm0);     // store, we are guarrenteed room in buffer (8 byte mult)
                        ecx-= 2;
                        esi=esi+24;           // bump to next control bytest
                        edi=edi+2;                    // bump to next output pixel addr

                        // maybe one last pixel
        LastOne:
                        if (ecx==0)                          // still more?
                         goto AllDone;                         // n, done

                        eax=*(int*)(esi+16);           // get data offset in pixels, 1st pixel pair
                        movd    (mm0, edx+eax);          // copy luma pair 0000xxYY
                        punpcklbw (mm0, mm7);                  // make words out of bytes, xxxx0Y0Y

                        pmaddwd (mm0, esi);                      // mult and sum lumas by ctl weights
                        paddusw (mm0, mm6);                        // round
                        psrlw   (mm0, 8);                          // right just 2 luma pixel value xxxx000Y
                        movd    (eax, mm0);
                        *edi=(unsigned char)eax;       // store last one

        AllDone:
//                        pop             ecx
        dstp += dst_pitch;
    }
 _mm_empty();
}


// YUY2
void SimpleResize::GetFrame_YUY2(const PVideoFrame *src, PVideoFrame *dst, int Planar_Type)
{
        int vWeight1[4];
        int vWeight2[4];
//    unsigned char* dstp = dst->GetWritePtr(PLANAR_Y);

        const unsigned char* srcp = src->ptr[Planar_Type];
        BYTE* dstp=dst->ptr[0];

        const stride_t src_pitch = src->pitch[0];
        const stride_t dst_pitch = dst->pitch[0];
        const int src_row_size = src->rowSize[0];
        const int row_size = dst->rowSize[0];
        const int height = dst->height[0];
        const int* pControl = &hControl[0];
        const unsigned char* srcp1;
        const unsigned char* srcp2;
        int* vWorkYW = vWorkY;
        int* vWorkUVW = vWorkUV;
        int EndOffset = src_row_size / 2;
        #ifdef __SSE2__
         __m128i xmm0,xmm5,xmm6,xmm7,xmm1,xmm2,xmm3,xmm4;
        #endif
        __m64 mm5,mm6,mm0,mm7,mm1,mm2,mm3,mm4;
        for (int y = 0; y < height; y++)
        {

                vWeight1[0] = vWeight1[1] = vWeight1[2] = vWeight1[3] =
                        (256-vWeights[y]) << 16 | (256-vWeights[y]);
                vWeight2[0] = vWeight2[1] = vWeight2[2] = vWeight2[3] =
                        vWeights[y] << 16 | vWeights[y];

                srcp1 = srcp + vOffsets[y] * src_pitch;

                if (Interlaced)
                {
                        srcp2 = (y < height-2)
                                ? srcp1 + 2 * src_pitch
                                : srcp1;
                }
                else
                {
                        srcp2 = (y < height-1)
                                ? srcp1 + src_pitch
                                : srcp1;
                }

                        int             ecx= src_row_size;
                        ecx>>= 3;                                  // 8 bytes a time
                        const unsigned char *esi= srcp1;                              // top of 2 src lines to get
                        const unsigned char *edx= srcp2;                              // next "
                        unsigned char *edi= (unsigned char*)vWorkYW;                    // luma work destination line
                        unsigned char *ebx= (unsigned char*)vWorkUVW;                   // luma work destination line
                        int eax=0;

// Let's check here to see if we are on a P4 or higher and can use SSE2 instructions.
// This first loop is not the performance bottleneck anyway but it is trivial to tune
// using SSE2 if we have proper alignment.
#ifdef __SSE2__
                        if (SSE2enabled==0)                  // is SSE2 supported?
                         goto vMaybeSSEMMX;                            // n, can't do anyway
                        if (ecx< 2)                                  // we have at least 16 byts, 2 qwords?
                         goto vMaybeSSEMMX;                            // n, don't bother
                        ecx>>= 1;                                  // do 16 bytes at a time instead
                        ecx--;                                             // jigger loop ct
                        //align       16
                        movdqu  (xmm0, FPround1);
                        movdqu  (xmm5, vWeight1);
                        movdqu  (xmm6, vWeight2);
                        movdqu  (xmm7, YMask   );

        vLoopSSE2_Fetch:
                        prefetcht0 (esi+eax*2+16);
                        prefetcht0 (edx+eax*2+16);

        vLoopSSE2:
                        movdqu  (xmm1,esi+eax*2); // top of 2 lines to interpolate
                        movdqu  (xmm2,edx+eax*2); // 2nd of 2 lines

                        movdqa  (xmm3, xmm1);                              // get chroma  bytes
                        pand    (xmm1, xmm7);                              // keep only luma
                        psrlw   (xmm3, 8   );                              // right just chroma
                        pmullw  (xmm1, xmm5);                              // mult by weighting factor
                        pmullw  (xmm3, xmm5);              // mult by weighting factor

                        movdqa  (xmm4, xmm2);                              // get chroma bytes
                        pand    (xmm2, xmm7);                              // keep only luma
                        psrlw   (xmm4, 8   );                              // right just chroma
                        pmullw  (xmm2, xmm6);                              // mult by weighting factor
                        pmullw  (xmm4, xmm6);              // mult by weighting factor

                        paddw   (xmm1, xmm2     );                         // combine lumas
                        paddusw (xmm1, xmm0     );                         // round
                        psrlw   (xmm1, 8        );                         // right adjust luma
                        movntdq (edi+eax*2, xmm1);    // save lumas in our work area

                        paddw   (xmm3, xmm4);                              // combine chromas
                        paddusw (xmm3, xmm0);                              // round
                        psrlw   (xmm3, 8   );                              // right adjust chroma
                        packuswb(xmm3,xmm3 );                             // pack UV's into low dword
                        movdq2q (mm1, xmm3   );                            // save in our work area
                        movntq  (ebx+eax, mm1); // save in our work area

                        eax= eax+8;
                        ecx--;                                             // don
                        if (ecx>0) goto vLoopSSE2_Fetch;                 // if not on last one loop, prefetch
                        if (ecx==0) goto vLoopSSE2;                               // or just loop, or not

// done with our SSE2 fortified loop but we may need to pick up the spare change
                        _mm_sfence();
                        ecx= src_row_size;               // get count again
                        ecx&= 0x0000000f;                 // just need mod 16
                        movq    (mm7, YMask   );                           // useful luma mask constant - lazy dupl init
                        movq    (mm5, vWeight1);
                        movq    (mm6, vWeight2);
                        movq    (mm0, FPround1);                   // useful rounding constant
                        ecx>>= 3;                                  // 8 bytes at a time, any?
                        if (ecx==0) goto MoreSpareChange;                 // n, did them all
#endif
// Let's check here to see if we are on a P2 or Athlon and can use SSEMMX instructions.
// This first loop is not the performance bottleneck anyway but it is trivial to tune
// using SSE if we have proper alignment.
        vMaybeSSEMMX:
                        movq    (mm7, YMask   );                           // useful luma mask constant
                        movq    (mm5, vWeight1);
                        movq    (mm6, vWeight2);
                        movq    (mm0, FPround1);                   // useful rounding constant
                        if (SSEMMXenabled==0)                        // is SSE2 supported?
                         goto vLoopMMX;                                // n, can't do anyway
                        ecx--;                                             // jigger loop ctr

                    //align       16
        vLoopSSEMMX_Fetch:
                        prefetcht0 (esi+eax*2+8);
                        prefetcht0 (edx+eax*2+8);

        vLoopSSEMMX:
                        movq    (mm1, esi+eax*2); // top of 2 lines to interpolate
                        movq    (mm2, edx+eax*2); // 2nd of 2 lines

                        movq    (mm3, mm1);                                // copy top bytes
                        pand    (mm1, mm7);                                // keep only luma
                        pxor    (mm3, mm1);                                // keep only chroma
                        psrlw   (mm3, 8  );                                // right just chroma
                        pmullw  (mm1, mm5);                                // mult by weighting factor
                        pmullw  (mm3, mm5);                                // mult by weighting factor

                        movq    (mm4, mm2);                                // copy 2nd bytes
                        pand    (mm2, mm7);                                // keep only luma
                        pxor    (mm4, mm2);                                // keep only chroma
                        psrlw   (mm4, 8  );                                // right just chroma
                        pmullw  (mm2, mm6);                                // mult by weighting factor
                        pmullw  (mm4, mm6);                                // mult by weighting factor

                        paddw   (mm1, mm2);                                // combine lumas
                        paddusw (mm1, mm0);                                // round
                        psrlw   (mm1, 8        );                          // right adjust luma
                        movntq  (edi+eax*2, mm1);       // save lumas in our work area

                        paddw   (mm3, mm4      );                          // combine chromas
                        paddusw (mm3, mm0      );                          // round
                        psrlw   (mm3, 8        );                          // right adjust chroma
                        packuswb(mm3,mm3       );                         // pack UV's into low dword
                        movd    (ebx+eax, mm3  ); // save in our work area

                        eax=eax+4;
                        ecx--;
                        if (ecx>0) goto vLoopSSEMMX_Fetch;                       // if not on last one loop, prefetch
                        if (ecx==0) goto vLoopSSEMMX;                             // or just loop, or not
                        _mm_sfence();
                        goto MoreSpareChange;                 // all done with vertical

                    //align       16
        vLoopMMX:
                        movq    (mm1, esi+eax*2); // top of 2 lines to interpolate
                        movq    (mm2, edx+eax*2); // 2nd of 2 lines

                        movq    (mm3, mm1      );                          // copy top bytes
                        pand    (mm1, mm7      );                          // keep only luma
                        pxor    (mm3, mm1      );                          // keep only chroma
                        psrlw   (mm3, 8        );                          // right just chroma
                        pmullw  (mm1, mm5      );                          // mult by weighting factor
                        pmullw  (mm3, mm5      );                          // mult by weighting factor

                        movq    (mm4, mm2      );                          // copy 2nd bytes
                        pand    (mm2, mm7      );                          // keep only luma
                        pxor    (mm4, mm2      );                          // keep only chroma
                        psrlw   (mm4, 8        );                          // right just chroma
                        pmullw  (mm2, mm6      );                          // mult by weighting factor
                        pmullw  (mm4, mm6      );                          // mult by weighting factor

                        paddw   (mm1, mm2      );                          // combine lumas
                        paddusw (mm1, mm0      );                          // round
                        psrlw   (mm1, 8        );                          // right adjust luma
                        movq    (edi+eax*2, mm1);       // save lumas in our work area

                        paddw   (mm3, mm4    );                          // combine chromas
                        paddusw (mm3, mm0    );                          // round
                        psrlw   (mm3, 8      );                          // right adjust chroma
                        packuswb(mm3,mm3     );                          // pack UV's into low dword
                        movd    (ebx+eax, mm3); // save in our work area

                        eax=eax+4;
                        ecx--;if (ecx>0) goto vLoopMMX;

// Add a little code here to check if we have 2 more pixels to do and, if so, make one
// more pass thru vLoopMMX. We were processing in multiples of 4 pixels and alway have
// an even number so there will never be more than 2 left. trbarry 7/29/2002
        MoreSpareChange:
                        if (!(eax<EndOffset))                  // did we get them all
                         goto DoHorizontal;                    // yes, else have 2 left
                        ecx= 1;                                  // jigger loop ct
                        eax-= 2;                                  // back up 2 pixels (4 bytes, but eax carried as 1/2)
                        goto vLoopMMX;


// We've taken care of the vertical scaling, now do horizontal
        DoHorizontal:
                        movq    (mm7, YMask   );                   // useful 0U0U..  mask constant
                        movq    (mm6, FPround2);                   // useful rounding constant, dwords
                        esi= (const unsigned char*)pControl;           // @ horiz control bytes
                        ecx= row_size;
                        ecx>>= 2;                          // 4 bytes a time, 2 pixels
                        edx= (const unsigned char*)vWorkYW;            // our luma data, as 0Y0Y 0Y0Y..
                        edi= dstp;                       // the destination line
                        ebx= (unsigned char*)vWorkUVW;           // chroma data, as UVUV UVUV...
                        //align 16
        hLoopMMX:
                        eax=*(int*)(esi+16);           // get data offset in pixels, 1st pixel pair
                        movd (mm0, edx+eax*2);           // copy luma pair
                        eax>>= 1;                          // div offset by 2
                        movd    (mm1, ebx+eax*2);       // copy UV pair VUVU
                        psllw   (mm1, 8);                          // shift out V, keep 0000U0U0

// we need to use both even and odd chroma from same location - trb 9/2002
                        punpckldq (mm1, ebx+eax*2);      // copy UV pair VUVU
                        psrlw   (mm1, 8);                          // shift out U0, keep 0V0V 0U0U
//

                        eax=*(int*)(esi+20);           // get data offset in pixels, 2nd pixel pair
                        punpckldq (mm0, edx+eax*2);              // copy luma pair
/*
                        shr             eax, 1                          // div offset by 2
                        punpckldq mm1, [ebx+eax*2]      // copy UV pair VUVU
                        psrlw   mm1, 8                          // shift out U0, keep 0V0V 0U0U
*/
                        pmaddwd (mm0, esi);                      // mult and sum lumas by ctl weights
                        paddusw (mm0, mm6);                        // round
                        psrlw   (mm0, 8  );                        // right just 2 luma pixel value 000Y,000Y

                        pmaddwd (mm1, esi+8);            // mult and sum chromas by ctl weights
                        paddusw (mm1, mm6  );                      // round
                        pslld   (mm1, 8    );                      // shift into low bytes of different words
                        pand    (mm1, mm7  );                      // keep only 2 chroma values 0V00,0U00
                        por     (mm0, mm1  );                      // combine luma and chroma, 0V0Y,0U0Y
                        packuswb(mm0,mm0   );                     // pack all into low dword, xxxxVYUY
                        movd    (edi, mm0  );   // done with 2 pixels

                        esi=esi+24;            // bump to next control bytest
                        edi=edi+4;                     // bump to next output pixel addr
                        ecx--;if (ecx>0) goto hLoopMMX;                             // loop for more

        dstp += dst_pitch;
        }
 _mm_empty();
}



// This function accepts a position from 0 to 1 and warps it, to 0 through 1 based
// upon the wFact var. The warp equations are designed to:
//
// * Always be rising but yield results from 0 to 1
//
// * Have a first derivative that doesn't go to 0 or infinity, at least close
//   to the center of the screen
//
// * Have a curvature (absolute val of 2nd derivative) that is small in the
//   center and smoothly rises towards the edges. We would like the curvature
//   to be everywhere = 0 when the warp factor = 1
//
double SimpleResize::WarpFactor(double position, double wFact)
{
        double x;
        double z;
        double w;
        x = 2 * (position - .5);
        if (true) //(wFact < 1.0)
        // For warp factor < 1 the warp is calculated as (1-w) * x^3 + w *x, centered

        // The warp is calculated as z = (1 - w) * x^3 + w * x, centered
        // around .5 and ranging from 0 to 1. After some tinkering this seems
        // to give decent values and derivatives at the right places.
                w = 2.0 - wFact;        // reverse parm for compat with initial release

                if (x < 0.0)
                {
                        z = -(1 - w) * x*x*x - w * x;      // -1 < x < 0, wFact < 1
                        return .5 - .5 * z;
                }
                else
                {
                        z = (1 - w) * x*x*x + w * x;      // -1 < x < 0, wFact < 1
                        return .5 + .5 * z;                             // amts to same formula as above for now
                }
}

// YUY2
// For each horizontal output pair of pixels there is are 2 qword masks followed by 2 int
// offsets. The 2 masks are the weights to be used for the luma and chroma, respectively.
// Each mask contains LeftWeight1, RightWeight1, LeftWeight2, RightWeight2. So a pair of pixels
// will later be processed each pass through the horizontal resize loop.

// The weights are scaled 0-256 and the left and right weights will sum to 256 for each pixel.


int SimpleResize::InitTables(void)
{
        int i;
        int j;
        int k;
        int wY1;
        int wY2;
        int wUV1;
        int wUV2;

        // First set up horizontal table
        for(i=0; i < newwidth; i+=2)
        {
                // first make even pixel control
                if (hWarp==1)                   // if no warp factor
                {
                        j = i * 256 * (oldwidth-1) / (newwidth-1);
                }
                else                                    // stretch and warp somehow
                {
                        j = (int) (256 * WarpFactor(i / (newwidth-1.0), hWarp) * (oldwidth-1));
                }

                k = j>>8;
                wY2 = j - (k << 8);                             // luma weight of right pixel
                wY1 = 256 - wY2;                                // luma weight of left pixel
                wUV2 = (k%2)
                        ? 128 + (wY2 >> 1)
                        : wY2 >> 1;
                wUV1 = 256 - wUV2;

                if (k > oldwidth - 2)
                {
                        hControl[i*3+4] = oldwidth - 1;  //     point to last byte
                        hControl[i*3] =   0x00000100;    // use 100% of rightmost Y
                        hControl[i*3+2] = 0x00000100;    // use 100% of rightmost U
                }
                else
                {
                        hControl[i*3+4] = k;                    // pixel offset
                        hControl[i*3] = wY2 << 16 | wY1; // luma weights
                        hControl[i*3+2] = wUV2 << 16 | wUV1; // chroma weights
                }

                // now make odd pixel control
                if (hWarp==1)                   // if no warp factor
                {
                        j = (i+1) * 256 * (oldwidth-1) / (newwidth-1);
                }
                else                                    // stretch and warp somehow
                {
                        j = (int) (256 * WarpFactor((i+1) / (newwidth-1.0), hWarp) * (oldwidth-1));
                }

                k = j>>8;
                wY2 = j - (k << 8);                             // luma weight of right pixel
                wY1 = 256 - wY2;                                // luma weight of left pixel
                wUV2 = (k%2)
                        ? 128 + (wY2 >> 1)
                        : wY2 >> 1;
                wUV1 = 256 - wUV2;

                if (k > oldwidth - 2)
                {
                        hControl[i*3+5] = oldwidth - 1;  //     point to last byte
                        hControl[i*3+1] = 0x00000100;    // use 100% of rightmost Y
                        hControl[i*3+3] = 0x00000100;    // use 100% of rightmost V
                }
                else
                {
                        hControl[i*3+5] = k;                    // pixel offset
                        hControl[i*3+1] = wY2 << 16 | wY1; // luma weights

//                      hControl[i*3+3] = wUV2 << 16 | wUV1; // chroma weights
// horiz chroma weights should be same as for even pixel - trbarry 09/16/2002
                        hControl[i*3+3] = hControl[i*3+2]; // chroma weights

                }
        }

        hControl[newwidth*3+4] =  2 * (oldwidth-1);             // give it something to prefetch at end
        hControl[newwidth*3+5] =  2 * (oldwidth-1);             // "

        // Next set up vertical table. The offsets are measured in lines and will be mult
        // by the source pitch later
        for(i=0; i< newheight; ++i)
        {
                if (vWarp==1)                   // if no warp factor
                {
                        j = i * 256 * (oldheight-1) / (newheight-1);
                }
                else                                    // stretch and warp somehow
                {
                        j = (int) (256 * WarpFactor(i / (newheight-1.0), vWarp) * (oldheight-1));
                }
                if (Interlaced)                                         // do hard way?
                {
                        if (i%2)                                                // is odd output line?
                        {
                                if (j < 256)                            // before 1st odd input line
                                {
                                        vOffsets[i] = 1;                // all from line 1
                                        vWeights[i] = 0;                // weight to give to 2nd line
                                }
                                else
                                {
                                        k = (((j-256) >> 9) << 1) + 1; // next lowest odd line
                                        vOffsets[i] = k;
                                        wY2 = j - (k << 8);
                                        vWeights[i] = wY2 >> 1; // weight to give to 2nd line
                                }
                        }
                        else                                                    // is even output line
                        {
                                k = (j >> 9) << 1;                      // next lower even line
                                vOffsets[i] = k;
                                wY2 = j - (k << 8);
                                vWeights[i] = wY2 >> 1;         // weight to give to 2nd line
                        }
                }
                else                                                            // simple way, do as progressive
                {
                        k = j >> 8;
                        vOffsets[i] = k;
                        wY2 = j - (k << 8);
                        vWeights[i] = wY2;                              // weight to give to 2nd line
                }
        }

        return 0;
}

// YV12

// For each horizontal output pair of pixels there is are 2 qword masks followed by 2 int
// offsets. The 2 masks are the weights to be used for the luma and chroma, respectively.
// Each mask contains LeftWeight1, RightWeight1, LeftWeight2, RightWeight2. So a pair of pixels
// will later be processed each pass through the horizontal resize loop.  I think with my
// current math the Horizontal Luma and Chroma contains the same values but since I may have screwed it
// up I'll leave it this way for now. Vertical chroma is different.

// Note - try just using the luma calcs for both, seem to be the same.

// The weights are scaled 0-256 and the left and right weights will sum to 256 for each pixel.

int SimpleResize::InitTables_YV12(void)
{
        int i;
        int j;
        int k;
        int wY1;
        int wY2;

        // First set up horizontal table, use for both luma & chroma since
        // it seems to have the same equation.
        // We will generate these values in pairs, mostly because that's the way
        // I wrote it for YUY2 above.

        for(i=0; i < newwidth; i+=2)
        {
                // first make even pixel control
                if (hWarp==1)                   // if no warp factor
                {
                        j = i * 256 * (oldwidth-1) / (newwidth-1);
                }
                else                                    // stretch and warp somehow
                {
                        j = (int) (256 * WarpFactor(i / (newwidth-1.0), hWarp) * (oldwidth-1));
                }

                k = j>>8;
                wY2 = j - (k << 8);                             // luma weight of right pixel
                wY1 = 256 - wY2;                                // luma weight of left pixel

                if (k > oldwidth - 2)
                {
                        hControl[i*3+4] = oldwidth - 1;  //     point to last byte
                        hControl[i*3] =   0x00000100;    // use 100% of rightmost Y
                }
                else
                {
                        hControl[i*3+4] = k;                    // pixel offset
                        hControl[i*3] = wY2 << 16 | wY1; // luma weights
                }

                // now make odd pixel control
                if (hWarp==1)                   // if no warp factor
                {
                        j = (i+1) * 256 * (oldwidth-1) / (newwidth-1);
                }
                else                                    // stretch and warp somehow
                {
                        j = (int) (256 * WarpFactor((i+1) / (newwidth-1.0), hWarp) * (oldwidth-1));
                }

                k = j>>8;
                wY2 = j - (k << 8);                             // luma weight of right pixel
                wY1 = 256 - wY2;                                // luma weight of left pixel

                if (k > oldwidth - 2)
                {
                        hControl[i*3+5] = oldwidth - 1;  //     point to last byte
                        hControl[i*3+1] = 0x00000100;    // use 100% of rightmost Y
                }
                else
                {
                        hControl[i*3+5] = k;                    // pixel offset
                        hControl[i*3+1] = wY2 << 16 | wY1; // luma weights
                }
        }

        hControl[newwidth*3+4] =  2 * (oldwidth-1);             // give it something to prefetch at end
        hControl[newwidth*3+5] =  2 * (oldwidth-1);             // "
        hControl[newwidth*3+4] =  2 * (oldwidth-1);             // give it something to prefetch at end
        hControl[newwidth*3+5] =  2 * (oldwidth-1);             // "

        // Next set up vertical tables. The offsets are measured in lines and will be mult
        // by the source pitch later .

        // For YV12 we need separate Luma and chroma tables
        // First Luma Table

        for(i=0; i< newheight; ++i)
        {
                if (vWarp==1)                   // if no warp factor
                {
                        j = i * 256 * (oldheight-1) / (newheight-1);
                }
                else                                    // stretch and warp somehow
                {
                        j = (int) (256 * WarpFactor(i / (newheight-1.0), vWarp) * (oldheight-1));
                }
                if (Interlaced)                                         // do hard way?
                {
                        if (i%2)                                                // is odd output line?
                        {
                                if (j < 256)                            // before 1st odd input line
                                {
                                        vOffsets[i] = 1;                // all from line 1
                                        vWeights[i] = 0;                // weight to give to 2nd line
                                }
                                else
                                {
                                        k = (((j-256) >> 9) << 1) + 1; // next lowest odd line
                                        vOffsets[i] = k;
                                        wY2 = j - (k << 8);
                                        vWeights[i] = wY2 >> 1; // weight to give to 2nd line
                                }
                        }
                        else                                                    // is even output line
                        {
                                k = (j >> 9) << 1;                      // next lower even line
                                vOffsets[i] = k;
                                wY2 = j - (k << 8);
                                vWeights[i] = wY2 >> 1;         // weight to give to 2nd line
                        }
                }
                else                                                            // simple way, do as progressive
                {
                        k = j >> 8;
                        vOffsets[i] = k;
                        wY2 = j - (k << 8);
                        vWeights[i] = wY2;                              // weight to give to 2nd line
                }
        }

        // Vertical table for chroma
        for(i=0; i< newheight/2; ++i)
        {
                if (vWarp==1)                   // if no warp factor
                {
                        j = (int) ( (i+.25) * 256 * (oldheight-1) / (newheight-1.0) - 64 );
                }
                else                                    // stretch and warp somehow
                {
                        j = (int) (256 * WarpFactor(
                                (i+.25) / (newheight-1.0), vWarp) * (oldheight-1.0)   );
                }
                if (Interlaced)                                         // do hard way?
                {
                        if (i%2)                                                // is odd output line?
                        {
                                if (j < 256)                            // before 1st odd input line
                                {
                                        vOffsetsUV[i] = 1;              // all from line 1
                                        vWeightsUV[i] = 0;              // weight to give to 2nd line
                                }
                                else
                                {
                                        k = (((j-256) >> 9) << 1) + 1; // next lowest odd line
                                        vOffsetsUV[i] = k;
                                        wY2 = j - (k << 8);
                                        vWeightsUV[i] = wY2 >> 1;       // weight to give to 2nd line
                                }
                        }
                        else                                                    // is even output line
                        {
                                k = (j >> 9) << 1;                      // next lower even line
                                vOffsetsUV[i] = k;
                                wY2 = j - (k << 8);
                                vWeightsUV[i] = wY2 >> 1;               // weight to give to 2nd line
                        }
                }
                else                                                            // simple way, do as progressive
                {
                        k = j >> 8;
                        vOffsetsUV[i] = k;
                        wY2 = j - (k << 8);
                        vWeightsUV[i] = wY2;                            // weight to give to 2nd line
                }
        }

        return 0;
}
/*
AVSValue __cdecl Create_InterlacedResize(AVSValue args, void* user_data, IScriptEnvironment* env)
{
        return new SimpleResize(args[0].AsClip(), args[1].AsInt(), args[2].AsInt(),
                1.0, 1.0, true, env);
}

AVSValue __cdecl Create_InterlacedWarpedResize(AVSValue args, void* user_data, IScriptEnvironment* env)
{
        return new SimpleResize(args[0].AsClip(), args[1].AsInt(), args[2].AsInt(),
                args[3].AsFloat(), args[4].AsFloat(), true, env);
}


AVSValue __cdecl Create_SimpleResize(AVSValue args, void* user_data, IScriptEnvironment* env)
{
        return new SimpleResize(args[0].AsClip(), args[1].AsInt(), args[2].AsInt(),
                1.0, 1.0, false, env);
}

AVSValue __cdecl Create_WarpedResize(AVSValue args, void* user_data, IScriptEnvironment* env)
{
        return new SimpleResize(args[0].AsClip(), args[1].AsInt(), args[2].AsInt(),
                args[3].AsFloat(), args[4].AsFloat(), false, env);
}
*/

#pragma warning(pop)
