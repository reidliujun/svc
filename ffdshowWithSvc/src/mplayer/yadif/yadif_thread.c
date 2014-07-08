/*
 * Copyright (c) 2004 Michael Niedermayer <michaelni@gmx.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

/*
 * Customized for yadif by Haruhiko Yamagata
 */


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <inttypes.h>
#include "ffImgfmt.h"
#include "../ffmpeg/libavutil/mem.h"
#include "vf_yadif.h"

typedef struct ThreadAPIContext{
    YadifThreadContext *yadThreadCtx;
    HANDLE thread;
    HANDLE work_sem;
    HANDLE done_sem;
    int (*func)(YadifThreadContext *yadThreadCtx);
}ThreadAPIContext;


static void WINAPI attribute_align_arg thread_func(void *v){
    ThreadAPIContext *threadAPIContext= v;

    for(;;){
        WaitForSingleObject(threadAPIContext->work_sem, INFINITE);
        if(threadAPIContext->func)
            threadAPIContext->func(threadAPIContext->yadThreadCtx);
        else
            return;
        ReleaseSemaphore(threadAPIContext->done_sem, 1, 0);
    }

    return;
}

/**
 * Free what has been allocated by yadif_thread_init().
 * Must be called after decoding has finished, especially do not call while yadif_thread_execute() is running.
 */
void yadif_thread_free(YadifContext *yadctx){
    ThreadAPIContext *threadAPIContext= yadctx->thread_opaque;
    int i;

    if (!threadAPIContext) return;
    for(i = 0 ; i < yadctx->thread_count ; i++){

        threadAPIContext[i].func= NULL;
        ReleaseSemaphore(threadAPIContext[i].work_sem, 1, 0);
        WaitForSingleObject(threadAPIContext[i].thread, INFINITE);
        if(threadAPIContext[i].work_sem) CloseHandle(threadAPIContext[i].work_sem);
        if(threadAPIContext[i].done_sem) CloseHandle(threadAPIContext[i].done_sem);
    }

    av_freep(&yadctx->thread_opaque);
}

void yadif_thread_execute(YadifContext *yadctx, int (*func)(YadifThreadContext *yadThreadCtx), int count){
    ThreadAPIContext *threadAPIContext= yadctx->thread_opaque;
    int i;

    for(i=0; i<count; i++){
        threadAPIContext[i].yadThreadCtx = &yadctx->threadCtx[i];
        threadAPIContext[i].func= func;

        ReleaseSemaphore(threadAPIContext[i].work_sem, 1, 0);
    }
    for(i=0; i<count; i++){
        WaitForSingleObject(threadAPIContext[i].done_sem, INFINITE);

        threadAPIContext[i].func= NULL;
    }
}

int yadif_thread_init(YadifContext *yadctx, int thread_count){
    int i;
    ThreadAPIContext *threadAPIContext;
    uint32_t threadid;

    yadctx->thread_count= thread_count;
    if (thread_count == 1) {
        yadctx->thread_opaque = NULL;
        return -1;
    }

    threadAPIContext= av_mallocz(sizeof(ThreadAPIContext)*thread_count);
    yadctx->thread_opaque= threadAPIContext;

    for(i=0; i<thread_count; i++){
        if(!(threadAPIContext[i].work_sem = CreateSemaphore(NULL, 0, 1, NULL)))
            goto fail;
        if(!(threadAPIContext[i].done_sem = CreateSemaphore(NULL, 0, 1, NULL)))
            goto fail;

        threadAPIContext[i].thread = (HANDLE)_beginthreadex(NULL, 0, thread_func, &threadAPIContext[i], 0, &threadid );
        if( !threadAPIContext[i].thread ) goto fail;
    }

    yadctx->execute= yadif_thread_execute;

    return 0;
fail:
    yadif_thread_free(yadctx);
    return -1;
}
