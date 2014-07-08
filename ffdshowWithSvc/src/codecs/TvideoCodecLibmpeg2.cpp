/*
 * Copyright (c) 2002-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "IffdshowDecVideo.h"
#include "TvideoCodecLibmpeg2.h"
#include "Tdll.h"
#include "Tconfig.h"
#include "libmpeg2/include/attributes.h"
#include "libmpeg2/libmpeg2/mpeg2_internal.h"
#include "libavcodec/avcodec.h"
#include "ffdebug.h"
#include "dsutil.h"
#include "cc_decoder.h"

const char_t* TvideoCodecLibmpeg2::dllname=_l("libmpeg2_ff.dll");

TvideoCodecLibmpeg2::TvideoCodecLibmpeg2(IffdshowBase *Ideci,IdecVideoSink *Isink):
 Tcodec(Ideci),TcodecDec(Ideci,Isink),
 TvideoCodec(Ideci),
 TvideoCodecDec(Ideci,Isink),
 ccDecoder(NULL)
{
 dll=new Tdll(dllname,config);
 dll->loadFunction(mpeg2_set_accel,"mpeg2_set_accel");
 dll->loadFunction(mpeg2_accel,"mpeg2_accel");
 dll->loadFunction(mpeg2_init,"mpeg2_init");
 dll->loadFunction(mpeg2_info,"mpeg2_info");
 dll->loadFunction(mpeg2_parse,"mpeg2_parse");
 dll->loadFunction(mpeg2_buffer,"mpeg2_buffer");
 dll->loadFunction(mpeg2_close,"mpeg2_close");
 dll->loadFunction(mpeg2_reset,"mpeg2_reset");
 dll->loadFunction(mpeg2_set_rtStart,"mpeg2_set_rtStart");
 ok=dll->ok;
 if (ok)
  {
   int accel=0;
   if (Tconfig::cpu_flags&FF_CPU_MMX   ) accel|=MPEG2_ACCEL_X86_MMX;
   if (Tconfig::cpu_flags&FF_CPU_3DNOW ) accel|=MPEG2_ACCEL_X86_3DNOW;
   if (Tconfig::cpu_flags&FF_CPU_MMXEXT) accel|=MPEG2_ACCEL_X86_MMXEXT;
   if (Tconfig::cpu_flags&FF_CPU_SSE2  ) accel|=MPEG2_ACCEL_X86_SSE2;
   if (Tconfig::cpu_flags&FF_CPU_SSE3  ) accel|=MPEG2_ACCEL_X86_SSE3;
   mpeg2_set_accel(accel);
  }
 mpeg2dec=NULL;info=NULL;quants=NULL;quantBytes=1;extradata=NULL;buffer=NULL;
 buffer=new Tbuffer();
 oldflags = 0;
 m_fFilm = false;
}
void TvideoCodecLibmpeg2::init(void)
{
 mpeg2dec=mpeg2_init();
 info=mpeg2_info(mpeg2dec);
 wait4Iframe=true;
 sequenceFlag=FIELD_TYPE::SEQ_START;
}
bool TvideoCodecLibmpeg2::beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags)
{
 if (extradata) delete extradata;
 extradata=new Textradata(mt,16);
 init();
 oldpict.rtStop=0;
 pict.csp=FF_CSP_420P;
 return true;
}
void TvideoCodecLibmpeg2::end(void)
{
 if (mpeg2dec) mpeg2_close(mpeg2dec);mpeg2dec=NULL;
 if (quants) free(quants);quants=NULL;
}
TvideoCodecLibmpeg2::~TvideoCodecLibmpeg2()
{
 end();
 if (dll) delete dll;
 if (extradata) delete extradata;
 if (ccDecoder) delete ccDecoder;
 if (buffer) delete buffer;
}

HRESULT __declspec(align(16))(TvideoCodecLibmpeg2::decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn))
{
 HRESULT hr=decompressI(src,srcLen,pIn);
 int len=mpeg2dec->buf_end - mpeg2dec->buf_start;
 if (len>0)
  {
   unsigned char *b=(unsigned char *)buffer->alloc(len);
   memcpy(b, mpeg2dec->buf_start, len);
   mpeg2_buffer(mpeg2dec, b, b+len);
  }
 return hr;
}

HRESULT __declspec(align(16))(TvideoCodecLibmpeg2::decompressI(const unsigned char *src,size_t srcLen,IMediaSample *pIn))
{
 //if (pIn->IsDiscontinuity() == S_OK)
 // onSeek(0);
 REFERENCE_TIME rtStart=REFTIME_INVALID,rtStop=_I64_MIN;
 HRESULT hr=pIn->GetTime(&rtStart,&rtStop);
 if (FAILED(hr))
  rtStart=rtStop=REFTIME_INVALID;

 int len=(int)srcLen;
 while (len>=0)
  {
   mpeg2_state_t state=mpeg2_parse(mpeg2dec);
   switch (state)
    {
     case STATE_BUFFER:
      if (len==0)
       len=-1;
      else
       {
        mpeg2_buffer(mpeg2dec,src,src+len);
        len=0;
       }
      break;
     case STATE_INVALID:
      break;
     case STATE_GOP:
      if(mpeg2dec->info.user_data_len > 4 && *(DWORD*)mpeg2dec->info.user_data == 0xf8014343)
       {
        if (!ccDecoder) ccDecoder=new TccDecoder(deciV);
        ccDecoder->decode(mpeg2dec->info.user_data+2,mpeg2dec->info.user_data_len-2);
       }
      break;
     case STATE_SEQUENCE:
      {
       sequenceFlag=FIELD_TYPE::SEQ_START;

       avgTimePerFrame=10LL*info->sequence->frame_period/27;
       deciV->setAverageTimePerFrame(&avgTimePerFrame,true);
       break;
      }
     case STATE_PICTURE:
      mpeg2dec->decoder.quant_stride=quantsStride=quantsDx=(info->sequence->picture_width+15)>>4;
      quantsDy=(info->sequence->picture_height+15)>>4;
      quants=mpeg2dec->decoder.quant_store=(char*)realloc(quants, quantsStride*quantsDy*2);
      quantType=1;
      mpeg2_set_rtStart(mpeg2dec,rtStart);
      rtStart=REFTIME_INVALID;
      break;
     case STATE_END:
      sequenceFlag |= FIELD_TYPE::SEQ_END;

     case STATE_SLICE:
      //if (info->display_fbuf && (!wait4Iframe || (info->display_picture->flags&PIC_MASK_CODING_TYPE)==PIC_FLAG_CODING_TYPE_I))
      if (info->display_picture && info->discard_fbuf && !(info->display_picture->flags&PIC_FLAG_SKIP))
       {
        int frametype;
        if (info->sequence->flags&SEQ_FLAG_MPEG2) quantType=FF_QSCALE_TYPE_MPEG2;
        switch (info->display_picture->flags&PIC_MASK_CODING_TYPE)
         {
          case PIC_FLAG_CODING_TYPE_I:frametype=FRAME_TYPE::I;break;
          case PIC_FLAG_CODING_TYPE_B:frametype=FRAME_TYPE::B;break;
          default:
          case PIC_FLAG_CODING_TYPE_P:frametype=FRAME_TYPE::P;break;
         }
        if (pIn->IsPreroll()==S_OK)
         return sinkD->deliverPreroll(frametype);

        int fieldtype = SetDeinterlaceMethod();

        if (sequenceFlag != FIELD_TYPE::SEQ_START || frametype == FRAME_TYPE::I)
         {
          fieldtype|=sequenceFlag;

          sequenceFlag=0;
         }

        unsigned char *data[4]={info->display_fbuf->buf[0],info->display_fbuf->buf[1],info->display_fbuf->buf[2],NULL};
        stride_t stride[4]={info->sequence->width,info->sequence->chroma_width,info->sequence->chroma_width,0};
        int csp;
        switch ((info->sequence->chroma_width==info->sequence->width)+(info->sequence->chroma_height==info->sequence->height))
         {
          case 1:csp=FF_CSP_422P;break;
          case 2:csp=FF_CSP_444P;break;
          default:
          case 0:csp=FF_CSP_420P;break;
         }
        TffPict pict(csp,data,stride,Trect(0,0,info->sequence->picture_width,info->sequence->picture_height,info->sequence->pixel_width,info->sequence->pixel_height),true,frametype,fieldtype,srcLen,NULL); //TODO: src frame size
        pict.rtStart=info->display_picture->rtStart;
        if (pict.rtStart==REFTIME_INVALID) pict.rtStart=oldpict.rtStop;
        pict.rtStop=pict.rtStart+avgTimePerFrame*info->display_picture->nb_fields/(info->display_picture_2nd?1:2);
        oldpict=pict;
        if (frametype==FRAME_TYPE::I)
         wait4Iframe=false;
        if (!wait4Iframe)
         {
          hr=sinkD->deliverDecodedSample(pict);
          if (FAILED(hr))
           return hr;
         }
        else
         DPRINTF(_l("libmpeg2: waiting for keyframe"));
       }
      break;
    }
  }
 return S_OK;
}

bool TvideoCodecLibmpeg2::onSeek(REFERENCE_TIME segmentStart)
{
 end();
 init();
 if (extradata && extradata->data)
  mpeg2_buffer(mpeg2dec,extradata->data,extradata->data+extradata->size);
 oldpict.rtStop=0;
 if (ccDecoder)
  ccDecoder->onSeek();
 return true;
}

/* 
 *	Copyright (C) 2003-2006 Gabest
 *	http://www.gabest.org
 *
 *      copied and modified from Mpeg2DecFilter.cpp
 */
int TvideoCodecLibmpeg2::SetDeinterlaceMethod(void)
{
    int di_method = FIELD_TYPE::PROGRESSIVE_FRAME; // weave

    DWORD seqflags = info->sequence->flags;
    DWORD newflags = info->display_picture->flags;

    if(!(seqflags & SEQ_FLAG_PROGRESSIVE_SEQUENCE) 
    && !(oldflags & PIC_FLAG_REPEAT_FIRST_FIELD))
    {
        if(!m_fFilm && (newflags & PIC_FLAG_REPEAT_FIRST_FIELD) && (newflags & PIC_FLAG_PROGRESSIVE_FRAME))
        {
            m_fFilm = true;
        }
        else if(m_fFilm && !(newflags & PIC_FLAG_REPEAT_FIRST_FIELD))
        {
            m_fFilm = false;
        }
    }

    if(seqflags & SEQ_FLAG_PROGRESSIVE_SEQUENCE)
        di_method = FIELD_TYPE::PROGRESSIVE_FRAME; // hurray!
    else if(m_fFilm){
        di_method = FIELD_TYPE::PROGRESSIVE_FRAME; // we are lucky

        if(newflags & PIC_FLAG_TOP_FIELD_FIRST)
            di_method |= FIELD_TYPE::INT_TFF;
        else
            di_method |= FIELD_TYPE::INT_BFF;

    }
#if 1 // ffdshow custom code
    else if(newflags & PIC_FLAG_TOP_FIELD_FIRST)
        di_method = FIELD_TYPE::INT_TFF;
    else
        di_method = FIELD_TYPE::INT_BFF;

#else // the (nearly) original code & comment
    else if(!(oldflags & PIC_FLAG_PROGRESSIVE_FRAME))
        di_method = DIBlend; // ok, clear thing
    else
        // big trouble here, the progressive_frame bit is not reliable :'(
        // frames without temporal field diffs can be only detected when ntsc 
        // uses the repeat field flag (signaled with m_fFilm), if it's not set 
        // or we have pal then we might end up blending the fields unnecessarily...
        di_method = DIBlend;
#endif
    oldflags = newflags;
    return di_method;
}
