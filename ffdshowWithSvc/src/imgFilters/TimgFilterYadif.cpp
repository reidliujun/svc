/*
 * Copyright (C) 2006 Michael Niedermayer <michaelni@gmx.at>
 * Ported to ffdshow by h.yamagata.
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

#include "stdafx.h"
#include "TdeinterlaceSettings.h"
#include "TimgFilterYadif.h"
#include "yadif/vf_yadif.h"
#include "IffdshowBase.h"
#include "Tconfig.h"
#include "ffdebug.h"

static inline void * memcpy_pic(unsigned char * dst, const unsigned char * src, int bytesPerLine, int height, stride_t dstStride, stride_t srcStride)
{
    int i;
    void *retval=dst;

    if(dstStride == srcStride)
    {
        if (srcStride < 0) {
            src += (height-1)*srcStride;
            dst += (height-1)*dstStride;
            srcStride = -srcStride;
        }

        memcpy(dst, src, srcStride*height);
    }
    else
    {
        for(i=0; i<height; i++)
        {
            memcpy(dst, src, bytesPerLine);
            src+= srcStride;
            dst+= dstStride;
        }
    }

    return retval;
}

void TimgFilterYadif::store_ref(const uint8_t *src[3], stride_t src_stride[3], int width, int height)
{
    int i;

    memcpy (yadctx->ref[3], yadctx->ref[0], sizeof(uint8_t *)*3);
    memmove(yadctx->ref[0], yadctx->ref[1], sizeof(uint8_t *)*3*3);


    // src[0] == NULL, at the end of stream, means it's the last picture
    if(src[0]){
        for(i=0; i<3; i++){
            memcpy_pic(yadctx->ref[2][i], src[i], width >> yadctx->shiftX[i],
                       height >> yadctx->shiftY[i], yadctx->stride[i], src_stride[i]);
        }
    }else{
        for(i=0; i<3; i++){
            memcpy_pic(yadctx->ref[2][i], yadctx->ref[1][i], width >> yadctx->shiftX[i],
                       height >> yadctx->shiftY[i], yadctx->stride[i], yadctx->stride[i]);
        }
    }
}

HRESULT TimgFilterYadif::put_image(TffPict &pict, const unsigned char *src[4], int full)
{
    int tff = 0;
    int frame_start_pos = 0;
    int double_frame_rate = yadctx->mode & 1;

    if(yadctx->field_order_mode < 0) {
        if (pict.fieldtype & FIELD_TYPE::INT_TFF)
            tff = 1;
    }
    else tff = (yadctx->field_order_mode&1)^1;

    int dx,dy;
    if (full)
     {
      dx = pict.rectFull.dx;
      dy = pict.rectFull.dy;
     }
    else
     {
      dx = pict.rectClip.dx;
      dy = pict.rectClip.dy;
     }

    store_ref(src, pict.stride, dx, dy);

    if(yadctx->do_deinterlace <= 2)
        yadctx->frame_duration = pict.rtStop - pict.rtStart;
    else
        yadctx->frame_duration = pict.rtStart - yadctx->buffered_rtStart;

    if (yadctx->frame_duration < 2)
        yadctx->frame_duration = 2;

    std::swap(pict.rtStart, yadctx->buffered_rtStart);
    std::swap(pict.rtStop,  yadctx->buffered_rtStop);

    if(yadctx->do_deinterlace == 0)
        return S_OK;
    if(yadctx->do_deinterlace == 1){
        yadctx->do_deinterlace = 2;
        return S_OK;
    }else if(yadctx->do_deinterlace == 2){
        // second call on the first image
        yadctx->do_deinterlace = 3;
        if (double_frame_rate){
            // half the frame_duration
            // output one image here, and one image on second input frame
            double_frame_rate = 0;
            pict.rtStop = pict.rtStart + (yadctx->frame_duration >> 1) - 1;
            if (pict.rtStop <= pict.rtStart)
                pict.rtStop = pict.rtStart + 1;
        }
    }else if(yadctx->do_deinterlace == 3){
        // second image inputed
        yadctx->do_deinterlace = 4;
        if (double_frame_rate){
            // output one image using the later half of the frame duration
            frame_start_pos = 1;
            pict.rtStart += yadctx->frame_duration >> 1;
            if (pict.rtStart > pict.rtStop)
                pict.rtStart = pict.rtStop;
        }else{
            // drop the second image
            return S_OK;
        }        
    }

    int hr = S_OK;

    for(int frame_pos = frame_start_pos ; frame_pos <= double_frame_rate ; frame_pos++){
        unsigned char *dst[4];
        bool cspChanged = getNext(csp1, pict, full, dst);

        libmplayer->yadif_filter(yadctx,
                                 dst,
                                 pict.stride,
                                 dx, dy,
                                 frame_pos ^ tff ^ 1, tff);

        pict.fieldtype = (pict.fieldtype & ~FIELD_TYPE::MASK_PROG) | FIELD_TYPE::PROGRESSIVE_FRAME;

        if (double_frame_rate){
            pict.rtStop = pict.rtStart + (yadctx->frame_duration >> 1) - 1;
            if (pict.rtStop <= pict.rtStart)
                pict.rtStop = pict.rtStart + 1;
        }

        TffPict inputPicture;
        if(double_frame_rate)
            inputPicture = pict;

        if (pict.rtStart < last_rtStop){
            DPRINTF(_l("yadif timestamp error, discarding a frame"));
            return S_OK;
        }

        // DPRINTF(_l("rtStart=%I64i rtStop=%I64i"), pict.rtStart, pict.rtStop);
        last_rtStop = pict.rtStop;
        HRESULT hr = parent->deliverSample(++it, pict);
        --it;

        if (frame_pos == double_frame_rate || hr != S_OK)
            break;

        // only if frame doubler is used and it has just delivered the first image.

        pict = inputPicture;
        pict.discontinuity = false;
        if (yadctx->frame_duration > 0){
            if (pict.rtStart != REFTIME_INVALID)
                pict.rtStart += yadctx->frame_duration >> 1;
            if (pict.rtStop != REFTIME_INVALID)
                pict.rtStop += yadctx->frame_duration >> 1;
        }
    }
    return hr;
}

int TimgFilterYadif::config(TffPict &pict)
{
        int i, j;

        for(i = 0 ; i < 3 ; i++){
            yadctx->shiftX[i] = pict.cspInfo.shiftX[i];
            yadctx->shiftY[i] = pict.cspInfo.shiftY[i];
            stride_t ffdshow_w = ((pict.rectFull.dx >> yadctx->shiftX[i])/16 + 2) * 16; // from void TffPict::convertCSP(int Icsp,Tbuffer &buf,int edge)
            stride_t w = std::max(pict.stride[i], ffdshow_w);
            int h = ((pict.rectFull.dy + (6 << yadctx->shiftY[i]) + 31) & (~31)) >> yadctx->shiftY[i];

            yadctx->stride[i]= w;
            for(j = 0 ; j < 3 ; j++){
                yadctx->ref[j][i]= (uint8_t*)malloc(w * h * sizeof(uint8_t)) + 3 * w;
                memset(yadctx->ref[j][i] - 3 * w, 128 * !!i, w * h * sizeof(uint8_t));
            }
        }

	return 1;
}

void TimgFilterYadif::done(void)
{
    if (dllok && yadctx)
    {
        int i;

        libmplayer->yadif_uninit(yadctx);
        for(i=0; i<3*3; i++){
            uint8_t **p= &yadctx->ref[i%3][i/3];
            if(*p) ::free(*p - 3 * yadctx->stride[i/3]);
            *p= NULL;
        }
        ::free(yadctx);
        yadctx = NULL;
        hasImageInBuffer = false;
    }
    last_rtStop = REFTIME_INVALID;
}

YadifContext* TimgFilterYadif::getContext(int mode, int field_order_mode){

    YadifContext *yadctx = (YadifContext*)malloc(sizeof(YadifContext));
    if (!yadctx) return yadctx;

    memset(yadctx, 0, sizeof(YadifContext));
    yadctx->do_deinterlace=1;

    yadctx->mode = mode;
    yadctx->field_order_mode = field_order_mode;
    libmplayer->yadif_init(yadctx);

    return yadctx;
}

//==========================================================================================================

/*
 * Copyright (c) 2008 h.yamagata
 */

TimgFilterYadif::TimgFilterYadif(IffdshowBase *Ideci,Tfilters *Iparent,bool Ibob):TimgFilter(Ideci,Iparent)
{
    hasImageInBuffer = false;
    deci->getPostproc(&libmplayer);
    dllok = libmplayer && libmplayer->yadif_filter && libmplayer->yadif_init;
    yadctx = NULL;
    last_rtStop = REFTIME_INVALID;
}

TimgFilterYadif::~TimgFilterYadif()
{
    done();
    if (libmplayer) libmplayer->Release();
}

bool TimgFilterYadif::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg)
{
    return dllok && super::is(pict,cfg);
}

void TimgFilterYadif::onSeek(void)
{
    done();
}

void TimgFilterYadif::onSizeChange(void)
{
    done();
}

bool TimgFilterYadif::onPullImageFromSubtitlesFilter(void)
{
    if (!yadctx || !dllok || yadctx->do_deinterlace < 2 || !hasImageInBuffer)
        return false;

     TffPict pict1 = oldpict;
     for (int i=0 ; i<3 ; i++){
        pict1.data[i] = yadctx->ref[2][i];
        pict1.stride[i] = yadctx->stride[i];
    }
    hasImageInBuffer = false;
    parent->setStopAtSubtitles(true);
    parent->deliverSample(++it,pict1);
    return true;
}

HRESULT TimgFilterYadif::onEndOfStream(void)
{
    if (!yadctx || !dllok)
        return S_OK;

    // here, it is guaranteed that we have copied last pict to oldpict.
    TffPict pict1 = oldpict;
    pict1.rtStart += yadctx->frame_duration;
    pict1.rtStop += yadctx->frame_duration;
    const unsigned char *src[4];
    src[0] = src[1] = src[2] = src[3] = NULL;
    return put_image(pict1, src, oldcfg.full);
}

HRESULT TimgFilterYadif::onDiscontinuity(const TffPict &pict)
{
    if (!yadctx || !dllok)
        return S_OK;

    TffPict pict1 = oldpict;
    pict1.rtStart = pict.rtStart;
    pict1.rtStop = pict.rtStop;
    const unsigned char *src[4];
    src[0] = src[1] = src[2] = src[3] = NULL;
    return put_image(pict1, src, oldcfg.full);
}

HRESULT TimgFilterYadif::process(TfilterQueue::iterator it0,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
    it = it0;
    cfg = (const TdeinterlaceSettings*)cfg0;
    hasImageInBuffer = false;
    int fieldtype = pict.fieldtype;
    bool start_and_end_frame = ((fieldtype & FIELD_TYPE::SEQ_START) && (fieldtype & FIELD_TYPE::SEQ_END));

    if (!dllok){
        return parent->deliverSample(++it,pict);
    }

    if ((pict.fieldtype & FIELD_TYPE::PROGRESSIVE_FRAME) && !cfg->deinterlaceAlways){
        onDiscontinuity(pict);
        done();
        return parent->deliverSample(++it,pict);
    }

    if (pict.rectClip != pict.rectFull && !cfg->full)
        parent->dirtyBorder=1;

    init(pict,cfg->full,cfg->half);
    if (  pict.discontinuity
       || (fieldtype & FIELD_TYPE::SEQ_START)){
        // flush the last picture, if any.
        onDiscontinuity(pict);
        done();
        if (start_and_end_frame){
            // Only one frame in the sequence, means it is best to skip deinterlacing.
            return parent->deliverSample(++it,pict);
        }
    }

    const unsigned char *src[4];
    bool cspChanged = getCur(FF_CSP_420P | FF_CSP_422P | FF_CSP_FLAGS_INTERLACED, pict, cfg->full,src);

    if (!yadctx){
        yadctx = getContext(cfg->yadifMode, cfg->yadifFieldOrder);
        config(pict);
    }

    oldpict = pict;
    oldcfg = *cfg;

    yadctx->field_order_mode = cfg->yadifFieldOrder;
    yadctx->mode = cfg->yadifMode;

    int old_do_deinterlace = yadctx->do_deinterlace;
    if (yadctx->do_deinterlace == 1){
        // duplicate the first image to avoid black screen in case second image does not exist.
        REFERENCE_TIME rtStart = pict.rtStart,rtStop = pict.rtStop;
        put_image(pict, src, cfg->full);
        pict.rtStart = rtStart;
        pict.rtStop = rtStop;
    }

    hasImageInBuffer = true;
    HRESULT hr = put_image(pict, src, cfg->full);

    if (old_do_deinterlace >= 2 && (fieldtype & FIELD_TYPE::SEQ_END)){
        onEndOfStream();
        done();
    }

    return hr;
}
