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

#ifndef _VF_YADIF_H_
#define _VF_YADIF_H_

typedef struct YadifThreadContext{
    struct YadifContext *yadctx;
    int plane_start;
    int plane_end;
    int y_start[3];
    int y_end[3];
} YadifThreadContext;

/**
 * YadifContext
 */
typedef struct YadifContext {
    int mode;
    int field_order_mode;
    int64_t buffered_rtStart;
    int64_t buffered_rtStop;
    int64_t frame_duration;
    stride_t stride[3];
    uint8_t *ref[4][3];
    unsigned int shiftX[4],shiftY[4];

    /**
     * do_deinterlace
     * 0:not initialized
     * 1:before buffuring the first frame
     * 2:first frame buffered, input the first frame again
     * 3:input the second frame
     * 4:normal (running)
     */
    int do_deinterlace;

    /*
     * Thread stuffs
     */
    int thread_count;
    void *thread_opaque;
    int (*execute)(struct YadifContext *yadctx, int (*func)(YadifThreadContext *yadThreadCtx), int count);
    YadifThreadContext *threadCtx;

    /*
     * temporary & private stuffs
     * parameters to be handed to threads
     */
    uint8_t **dst;
    stride_t *dst_stride;
    int width;
    int height;
    int parity;
    int tff;
} YadifContext;

void yadif_filter(YadifContext *p, uint8_t *dst[3], stride_t dst_stride[3], int width, int height, int parity, int tff);
void yadif_init(YadifContext *yadctx);
void yadif_uninit(YadifContext *yadctx);

#endif // _VF_YADIF_H_
