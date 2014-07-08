/*
 * Copyright (C) 2006 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

#include "config.h"
#include "cpudetect.h"

#include "mp_msg.h"

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#include "ffImgfmt.h"

#include "libvo/fastmemcpy.h"
#include "vf_yadif.h"
#include "../ffmpeg/libavutil/mem.h"

#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define ABS(a) ((a) > 0 ? (a) : (-(a)))

#define MIN3(a,b,c) MIN(MIN(a,b),c)
#define MAX3(a,b,c) MAX(MAX(a,b),c)

//===========================================================================//

static void yadif_default_execute(YadifContext *yadctx, int (*func)(YadifThreadContext *yadThreadCtx), int count){
    int i;

    for(i=0; i<count; i++){
        func(&yadctx->threadCtx[i]);
    }
}

static void (*filter_line)(YadifContext *yadctx, uint8_t *dst, uint8_t *prev, uint8_t *cur, uint8_t *next, int w, int refs, int parity);

#if defined(HAVE_FAST_64BIT) || (defined(HAVE_MMX) && defined(NAMED_ASM_ARGS))

#define LOAD4(mem,dst) \
            "movd      "mem", "#dst" \n\t"\
            "punpcklbw %%mm7, "#dst" \n\t"

#define PABS(tmp,dst) \
            "pxor     "#tmp", "#tmp" \n\t"\
            "psubw    "#dst", "#tmp" \n\t"\
            "pmaxsw   "#tmp", "#dst" \n\t"

#define CHECK(pj,mj) \
            "movq "#pj"(%[cur],%[mrefs]), %%mm2 \n\t" /* cur[x-refs-1+j] */\
            "movq "#mj"(%[cur],%[prefs]), %%mm3 \n\t" /* cur[x+refs-1-j] */\
            "movq      %%mm2, %%mm4 \n\t"\
            "movq      %%mm2, %%mm5 \n\t"\
            "pxor      %%mm3, %%mm4 \n\t"\
            "pavgb     %%mm3, %%mm5 \n\t"\
            "pand     %[pb1], %%mm4 \n\t"\
            "psubusb   %%mm4, %%mm5 \n\t"\
            "psrlq     $8,    %%mm5 \n\t"\
            "punpcklbw %%mm7, %%mm5 \n\t" /* (cur[x-refs+j] + cur[x+refs-j])>>1 */\
            "movq      %%mm2, %%mm4 \n\t"\
            "psubusb   %%mm3, %%mm2 \n\t"\
            "psubusb   %%mm4, %%mm3 \n\t"\
            "pmaxub    %%mm3, %%mm2 \n\t"\
            "movq      %%mm2, %%mm3 \n\t"\
            "movq      %%mm2, %%mm4 \n\t" /* ABS(cur[x-refs-1+j] - cur[x+refs-1-j]) */\
            "psrlq      $8,   %%mm3 \n\t" /* ABS(cur[x-refs  +j] - cur[x+refs  -j]) */\
            "psrlq     $16,   %%mm4 \n\t" /* ABS(cur[x-refs+1+j] - cur[x+refs+1-j]) */\
            "punpcklbw %%mm7, %%mm2 \n\t"\
            "punpcklbw %%mm7, %%mm3 \n\t"\
            "punpcklbw %%mm7, %%mm4 \n\t"\
            "paddw     %%mm3, %%mm2 \n\t"\
            "paddw     %%mm4, %%mm2 \n\t" /* score */

#define CHECK1 \
            "movq      %%mm0, %%mm3 \n\t"\
            "pcmpgtw   %%mm2, %%mm3 \n\t" /* if(score < spatial_score) */\
            "pminsw    %%mm2, %%mm0 \n\t" /* spatial_score= score; */\
            "movq      %%mm3, %%mm6 \n\t"\
            "pand      %%mm3, %%mm5 \n\t"\
            "pandn     %%mm1, %%mm3 \n\t"\
            "por       %%mm5, %%mm3 \n\t"\
            "movq      %%mm3, %%mm1 \n\t" /* spatial_pred= (cur[x-refs+j] + cur[x+refs-j])>>1; */

#define CHECK2 /* pretend not to have checked dir=2 if dir=1 was bad.\
                  hurts both quality and speed, but matches the C version. */\
            "paddw    %[pw1], %%mm6 \n\t"\
            "psllw     $14,   %%mm6 \n\t"\
            "paddsw    %%mm6, %%mm2 \n\t"\
            "movq      %%mm0, %%mm3 \n\t"\
            "pcmpgtw   %%mm2, %%mm3 \n\t"\
            "pminsw    %%mm2, %%mm0 \n\t"\
            "pand      %%mm3, %%mm5 \n\t"\
            "pandn     %%mm1, %%mm3 \n\t"\
            "por       %%mm5, %%mm3 \n\t"\
            "movq      %%mm3, %%mm1 \n\t"

static void filter_line_mmx2(YadifContext *yadctx, uint8_t *dst, uint8_t *prev, uint8_t *cur, uint8_t *next, int w, int refs, int parity){
    static const uint64_t pw_1 = 0x0001000100010001ULL;
    static const uint64_t pb_1 = 0x0101010101010101ULL;
    const int mode = yadctx->mode;
    uint64_t tmp0, tmp1, tmp2, tmp3;
    int x;

#define FILTER\
    for(x=0; x<w; x+=4){\
        __asm__ volatile(\
            "pxor      %%mm7, %%mm7 \n\t"\
            LOAD4("(%[cur],%[mrefs])", %%mm0) /* c = cur[x-refs] */\
            LOAD4("(%[cur],%[prefs])", %%mm1) /* e = cur[x+refs] */\
            LOAD4("(%["prev2"])", %%mm2) /* prev2[x] */\
            LOAD4("(%["next2"])", %%mm3) /* next2[x] */\
            "movq      %%mm3, %%mm4 \n\t"\
            "paddw     %%mm2, %%mm3 \n\t"\
            "psraw     $1,    %%mm3 \n\t" /* d = (prev2[x] + next2[x])>>1 */\
            "movq      %%mm0, %[tmp0] \n\t" /* c */\
            "movq      %%mm3, %[tmp1] \n\t" /* d */\
            "movq      %%mm1, %[tmp2] \n\t" /* e */\
            "psubw     %%mm4, %%mm2 \n\t"\
            PABS(      %%mm4, %%mm2) /* temporal_diff0 */\
            LOAD4("(%[prev],%[mrefs])", %%mm3) /* prev[x-refs] */\
            LOAD4("(%[prev],%[prefs])", %%mm4) /* prev[x+refs] */\
            "psubw     %%mm0, %%mm3 \n\t"\
            "psubw     %%mm1, %%mm4 \n\t"\
            PABS(      %%mm5, %%mm3)\
            PABS(      %%mm5, %%mm4)\
            "paddw     %%mm4, %%mm3 \n\t" /* temporal_diff1 */\
            "psrlw     $1,    %%mm2 \n\t"\
            "psrlw     $1,    %%mm3 \n\t"\
            "pmaxsw    %%mm3, %%mm2 \n\t"\
            LOAD4("(%[next],%[mrefs])", %%mm3) /* next[x-refs] */\
            LOAD4("(%[next],%[prefs])", %%mm4) /* next[x+refs] */\
            "psubw     %%mm0, %%mm3 \n\t"\
            "psubw     %%mm1, %%mm4 \n\t"\
            PABS(      %%mm5, %%mm3)\
            PABS(      %%mm5, %%mm4)\
            "paddw     %%mm4, %%mm3 \n\t" /* temporal_diff2 */\
            "psrlw     $1,    %%mm3 \n\t"\
            "pmaxsw    %%mm3, %%mm2 \n\t"\
            "movq      %%mm2, %[tmp3] \n\t" /* diff */\
\
            "paddw     %%mm0, %%mm1 \n\t"\
            "paddw     %%mm0, %%mm0 \n\t"\
            "psubw     %%mm1, %%mm0 \n\t"\
            "psrlw     $1,    %%mm1 \n\t" /* spatial_pred */\
            PABS(      %%mm2, %%mm0)      /* ABS(c-e) */\
\
            "movq -1(%[cur],%[mrefs]), %%mm2 \n\t" /* cur[x-refs-1] */\
            "movq -1(%[cur],%[prefs]), %%mm3 \n\t" /* cur[x+refs-1] */\
            "movq      %%mm2, %%mm4 \n\t"\
            "psubusb   %%mm3, %%mm2 \n\t"\
            "psubusb   %%mm4, %%mm3 \n\t"\
            "pmaxub    %%mm3, %%mm2 \n\t"\
            "pshufw $9,%%mm2, %%mm3 \n\t"\
            "punpcklbw %%mm7, %%mm2 \n\t" /* ABS(cur[x-refs-1] - cur[x+refs-1]) */\
            "punpcklbw %%mm7, %%mm3 \n\t" /* ABS(cur[x-refs+1] - cur[x+refs+1]) */\
            "paddw     %%mm2, %%mm0 \n\t"\
            "paddw     %%mm3, %%mm0 \n\t"\
            "psubw    %[pw1], %%mm0 \n\t" /* spatial_score */\
\
            CHECK(-2,0)\
            CHECK1\
            CHECK(-3,1)\
            CHECK2\
            CHECK(0,-2)\
            CHECK1\
            CHECK(1,-3)\
            CHECK2\
\
            /* if(yadctx->mode<2) ... */\
            "movq    %[tmp3], %%mm6 \n\t" /* diff */\
            "cmp       $2, %[mode] \n\t"\
            "jge       1f \n\t"\
            LOAD4("(%["prev2"],%[mrefs],2)", %%mm2) /* prev2[x-2*refs] */\
            LOAD4("(%["next2"],%[mrefs],2)", %%mm4) /* next2[x-2*refs] */\
            LOAD4("(%["prev2"],%[prefs],2)", %%mm3) /* prev2[x+2*refs] */\
            LOAD4("(%["next2"],%[prefs],2)", %%mm5) /* next2[x+2*refs] */\
            "paddw     %%mm4, %%mm2 \n\t"\
            "paddw     %%mm5, %%mm3 \n\t"\
            "psrlw     $1,    %%mm2 \n\t" /* b */\
            "psrlw     $1,    %%mm3 \n\t" /* f */\
            "movq    %[tmp0], %%mm4 \n\t" /* c */\
            "movq    %[tmp1], %%mm5 \n\t" /* d */\
            "movq    %[tmp2], %%mm7 \n\t" /* e */\
            "psubw     %%mm4, %%mm2 \n\t" /* b-c */\
            "psubw     %%mm7, %%mm3 \n\t" /* f-e */\
            "movq      %%mm5, %%mm0 \n\t"\
            "psubw     %%mm4, %%mm5 \n\t" /* d-c */\
            "psubw     %%mm7, %%mm0 \n\t" /* d-e */\
            "movq      %%mm2, %%mm4 \n\t"\
            "pminsw    %%mm3, %%mm2 \n\t"\
            "pmaxsw    %%mm4, %%mm3 \n\t"\
            "pmaxsw    %%mm5, %%mm2 \n\t"\
            "pminsw    %%mm5, %%mm3 \n\t"\
            "pmaxsw    %%mm0, %%mm2 \n\t" /* max */\
            "pminsw    %%mm0, %%mm3 \n\t" /* min */\
            "pxor      %%mm4, %%mm4 \n\t"\
            "pmaxsw    %%mm3, %%mm6 \n\t"\
            "psubw     %%mm2, %%mm4 \n\t" /* -max */\
            "pmaxsw    %%mm4, %%mm6 \n\t" /* diff= MAX3(diff, min, -max); */\
            "1: \n\t"\
\
            "movq    %[tmp1], %%mm2 \n\t" /* d */\
            "movq      %%mm2, %%mm3 \n\t"\
            "psubw     %%mm6, %%mm2 \n\t" /* d-diff */\
            "paddw     %%mm6, %%mm3 \n\t" /* d+diff */\
            "pmaxsw    %%mm2, %%mm1 \n\t"\
            "pminsw    %%mm3, %%mm1 \n\t" /* d = clip(spatial_pred, d-diff, d+diff); */\
            "packuswb  %%mm1, %%mm1 \n\t"\
\
            :[tmp0]"=m"(tmp0),\
             [tmp1]"=m"(tmp1),\
             [tmp2]"=m"(tmp2),\
             [tmp3]"=m"(tmp3)\
            :[prev] "r"(prev),\
             [cur]  "r"(cur),\
             [next] "r"(next),\
             [prefs]"r"((stride_t)refs),\
             [mrefs]"r"((stride_t)-refs),\
             [pw1]  "m"(pw_1),\
             [pb1]  "m"(pb_1),\
             [mode] "g"(mode)\
        );\
        __asm__ volatile("movd %%mm1, %0" :"=m"(*dst));\
        dst += 4;\
        prev+= 4;\
        cur += 4;\
        next+= 4;\
    }

    if(parity){
#define prev2 "prev"
#define next2 "cur"
        FILTER
#undef prev2
#undef next2
    }else{
#define prev2 "cur"
#define next2 "next"
        FILTER
#undef prev2
#undef next2
    }
}
#undef LOAD4
#undef PABS
#undef CHECK
#undef CHECK1
#undef CHECK2
#undef FILTER

// ================= SSE2 =================
#define PABS(tmp,dst) \
            "pxor     "#tmp", "#tmp" \n\t"\
            "psubw    "#dst", "#tmp" \n\t"\
            "pmaxsw   "#tmp", "#dst" \n\t"

#define FILTER_LINE_FUNC_NAME filter_line_sse2
#include "vf_yadif_template.c"

// ================ SSSE3 =================
#define PABS(tmp,dst) \
            "pabsw     "#dst", "#dst" \n\t"

#define FILTER_LINE_FUNC_NAME filter_line_ssse3
#include "vf_yadif_template.c"

#endif /* defined(HAVE_MMX) && defined(NAMED_ASM_ARGS) */

static void filter_line_c(YadifContext *yadctx, uint8_t *dst, uint8_t *prev, uint8_t *cur, uint8_t *next, int w, int refs, int parity){
    int x;
    uint8_t *prev2= parity ? prev : cur ;
    uint8_t *next2= parity ? cur  : next;
    for(x=0; x<w; x++){
        int c= cur[-refs];
        int d= (prev2[0] + next2[0])>>1;
        int e= cur[+refs];
        int temporal_diff0= ABS(prev2[0] - next2[0]);
        int temporal_diff1=( ABS(prev[-refs] - c) + ABS(prev[+refs] - e) )>>1;
        int temporal_diff2=( ABS(next[-refs] - c) + ABS(next[+refs] - e) )>>1;
        int diff= MAX3(temporal_diff0>>1, temporal_diff1, temporal_diff2);
        int spatial_pred= (c+e)>>1;
        int spatial_score= ABS(cur[-refs-1] - cur[+refs-1]) + ABS(c-e)
                         + ABS(cur[-refs+1] - cur[+refs+1]) - 1;

#define CHECK(j)\
    {   int score= ABS(cur[-refs - 1 + j] - cur[+refs -1- j])\
                 + ABS(cur[-refs     + j] - cur[+refs  -  j])\
                 + ABS(cur[-refs + 1 + j] - cur[+refs +1- j]);\
        if(score < spatial_score){\
            spatial_score= score;\
            spatial_pred= (cur[-refs  + j] + cur[+refs  - j])>>1;\

        CHECK(-1) CHECK(-2) }} }}
        CHECK( 1) CHECK( 2) }} }}

        if(yadctx->mode<2){
            int b= (prev2[-2*refs] + next2[-2*refs])>>1;
            int f= (prev2[+2*refs] + next2[+2*refs])>>1;
#if 0
            int a= cur[-3*refs];
            int g= cur[+3*refs];
            int max= MAX3(d-e, d-c, MIN3(MAX(b-c,f-e),MAX(b-c,b-a),MAX(f-g,f-e)) );
            int min= MIN3(d-e, d-c, MAX3(MIN(b-c,f-e),MIN(b-c,b-a),MIN(f-g,f-e)) );
#else
            int max= MAX3(d-e, d-c, MIN(b-c, f-e));
            int min= MIN3(d-e, d-c, MAX(b-c, f-e));
#endif

            diff= MAX3(diff, min, -max);
        }

        if(spatial_pred > d + diff)
           spatial_pred = d + diff;
        else if(spatial_pred < d - diff)
           spatial_pred = d - diff;

        dst[0] = spatial_pred;

        dst++;
        cur++;
        prev++;
        next++;
        prev2++;
        next2++;
    }
}

attribute_align_arg void yadif_threaded_filter(YadifThreadContext *yadThreadCtx){
    int i,y;
    YadifContext *yadctx = yadThreadCtx->yadctx;
    uint8_t **dst = yadctx->dst;
    stride_t *dst_stride = yadctx->dst_stride;
    int width = yadctx->width;
    int height = yadctx->height;
    int parity = yadctx->parity;
    int tff = yadctx->tff;

    for(i = yadThreadCtx->plane_start ; i <= yadThreadCtx->plane_end ; i++){
        int w= width >> yadctx->shiftX[i];
        int h= yadThreadCtx->y_end[i];
        int refs= yadctx->stride[i];

        for(y=yadThreadCtx->y_start[i] ; y < h ; y++){
            if((y ^ parity) & 1){
                uint8_t *prev= &yadctx->ref[0][i][y*refs];
                uint8_t *cur = &yadctx->ref[1][i][y*refs];
                uint8_t *next= &yadctx->ref[2][i][y*refs];
                uint8_t *dst2= &dst[i][y*dst_stride[i]];
                filter_line(yadctx, dst2, prev, cur, next, w, refs, parity ^ tff);
            }else{
                memcpy(&dst[i][y*dst_stride[i]], &yadctx->ref[1][i][y*refs], w);
            }
        }
    }
}

attribute_align_arg void yadif_filter(YadifContext *yadctx, uint8_t *dst[3], stride_t dst_stride[3], int width, int height, int parity, int tff){
    int y, i, thread_num, pl;
    int pixels_all,pixels_per_thread, residual_pixels;
    int w[3],h[3];
    int y_start,plane_start;
    int thread_count = yadctx->thread_count;
#if __STDC_VERSION__ >= 199901L
    YadifThreadContext threads[thread_count];
#else
    YadifThreadContext *threads=_alloca(sizeof(YadifThreadContext) * thread_count);
#endif

#ifdef HAVE_FAST_64BIT
    DECLARE_ALIGNED(16, uint8_t, regbuf[16*16]);
    __asm__ volatile(
        "movq    %[regbuf], %%rax \n\t"
        "movdqa  %%xmm6,   6*16(%%rax) \n\t"
        "movdqa  %%xmm7,   7*16(%%rax) \n\t"
        "movdqa  %%xmm8,   8*16(%%rax) \n\t"
        "movdqa  %%xmm9,   9*16(%%rax) \n\t"
        "movdqa  %%xmm10, 10*16(%%rax) \n\t"
        "movdqa  %%xmm11, 11*16(%%rax) \n\t"
        "movdqa  %%xmm12, 12*16(%%rax) \n\t"
        "movdqa  %%xmm13, 13*16(%%rax) \n\t"
        "movdqa  %%xmm14, 14*16(%%rax) \n\t"
        "movdqa  %%xmm15, 15*16(%%rax) \n\t"
        ::[regbuf] "r"(regbuf)
    );
#endif

    memset(threads, 0, sizeof(YadifThreadContext) * thread_count);

    yadctx->threadCtx = threads;
    yadctx->dst = dst;
    yadctx->dst_stride = dst_stride;
    yadctx->width = width;
    yadctx->height = height;
    yadctx->parity = parity;
    yadctx->tff = tff;

    for (i = 0 ; i < 3 ; i++){
        w[i] = width  >> yadctx->shiftX[i];
        h[i] = height >> yadctx->shiftY[i];
    }

    pixels_all = w[0] * h[0] + w[1] * h[1] + w[2] * h[2];
    pixels_per_thread = pixels_all / thread_count;
    y_start = 0;
    plane_start = 0;
    for (thread_num = 0 ; thread_num < thread_count ; thread_num++){
        threads[thread_num].yadctx = yadctx;
        threads[thread_num].plane_start = plane_start;
        threads[thread_num].y_start[plane_start] = y_start;
        residual_pixels = pixels_per_thread;
        for (pl = plane_start ; pl < 3 ; pl++){
            int lines = residual_pixels / w[pl];
            if (threads[thread_num].y_start[pl] + lines <= h[pl]){
                threads[thread_num].plane_end = pl;
                y_start = threads[thread_num].y_end[pl] = threads[thread_num].y_start[pl] + lines;
                break;
            }
            plane_start++;
            y_start = 0;
            threads[thread_num].y_end[pl] = h[pl];
            residual_pixels -= (h[pl] - threads[thread_num].y_start[pl]) * w[pl];
        }
    }

    threads[thread_count - 1].plane_end = 2;
    threads[thread_count - 1].y_end[2] = h[2];

    yadctx->execute(yadctx, yadif_threaded_filter, yadctx->thread_count);
#ifdef HAVE_FAST_64BIT
    __asm__ volatile(
        "movq    %[regbuf], %%rax \n\t"
        "movdqa   6*16(%%rax),%%xmm6  \n\t"
        "movdqa   7*16(%%rax),%%xmm7  \n\t"
        "movdqa   8*16(%%rax),%%xmm8  \n\t"
        "movdqa   9*16(%%rax),%%xmm9  \n\t"
        "movdqa  10*16(%%rax),%%xmm10 \n\t"
        "movdqa  11*16(%%rax),%%xmm11 \n\t"
        "movdqa  12*16(%%rax),%%xmm12 \n\t"
        "movdqa  13*16(%%rax),%%xmm13 \n\t"
        "movdqa  14*16(%%rax),%%xmm14 \n\t"
        "movdqa  15*16(%%rax),%%xmm15 \n\t"
        ::[regbuf] "r"(regbuf)
    );
#endif
#if defined(HAVE_MMX) && defined(NAMED_ASM_ARGS)
    if(filter_line == filter_line_mmx2) __asm__ volatile("emms \n\t" : : : "memory");
#endif
}

void yadif_init(YadifContext *yadctx){

    filter_line = filter_line_c;
#if defined(HAVE_FAST_64BIT) || (defined(HAVE_MMX) && defined(NAMED_ASM_ARGS))
    if(gCpuCaps.hasSSSE3)
        filter_line = filter_line_ssse3;
    else if(gCpuCaps.hasSSE2)
        filter_line = filter_line_sse2;
    else if(gCpuCaps.hasMMX2)
        filter_line = filter_line_mmx2;
#endif
    yadctx->execute = yadif_default_execute;
    if (yadif_thread_init(yadctx, GetCPUCount()) < 0)
        yadctx->thread_count = 1;
}

void yadif_uninit(YadifContext *yadctx){
    yadif_thread_free(yadctx);
}
