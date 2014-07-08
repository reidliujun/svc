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
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "IffdshowEnc.h"
#include "TvideoCodecLibavcodec.h"
#include "TglobalSettings.h"
#include "ffdshow_mediaguids.h"
#include "TcodecSettings.h"
#include "ffmpeg/libavcodec/dvdata.h"
#include "rational.h"
#include "qtpalette.h"
#include "line.h"
#include "simd.h"
#include "dsutil.h"
#include "cc_decoder.h"
#include "TffdshowVideoInputPin.h"
#include "ffdebug.h"

TvideoCodecLibavcodec::TvideoCodecLibavcodec(IffdshowBase *Ideci,IdecVideoSink *IsinkD, int SVC):
 Tcodec(Ideci),TcodecDec(Ideci,IsinkD),
 TvideoCodec(Ideci),
 TvideoCodecDec(Ideci,IsinkD),
 TvideoCodecEnc(Ideci,NULL),
 codedPictureBuffer(this),
 h264RandomAccess(this)
{
	if(!SVC){
		create();
	}else{
		createSVC();
	}
}
TvideoCodecLibavcodec::TvideoCodecLibavcodec(IffdshowBase *Ideci,IencVideoSink *IsinkE):
 Tcodec(Ideci),TcodecDec(Ideci,NULL),
 TvideoCodec(Ideci),
 TvideoCodecDec(Ideci,NULL),
 TvideoCodecEnc(Ideci,IsinkE),
 codedPictureBuffer(this),
 h264RandomAccess(this)
{
 create();
 if (ok && !libavcodec->dec_only)
  {
   encoders.push_back(new Tencoder(_l("MPEG-4"),CODEC_ID_MPEG4));
   encoders.push_back(new Tencoder(_l("DivX 3"),CODEC_ID_MSMPEG4V3));
   encoders.push_back(new Tencoder(_l("MS MPEG4 v2"),CODEC_ID_MSMPEG4V2));
   encoders.push_back(new Tencoder(_l("MPEG-1"),CODEC_ID_MPEG1VIDEO,Tfourccs(FOURCC_MPEG,FOURCC_MPG1,0)));
   encoders.push_back(new Tencoder(_l("MPEG-2"),CODEC_ID_MPEG2VIDEO,Tfourccs(FOURCC_MPEG,FOURCC_MPG2,0)));
   encoders.push_back(new Tencoder(_l("H.263"),CODEC_ID_H263));
   encoders.push_back(new Tencoder(_l("H.263+"),CODEC_ID_H263P));
   encoders.push_back(new Tencoder(_l("H.261"),CODEC_ID_H261));
   encoders.push_back(new Tencoder(_l("WMV 7"),CODEC_ID_WMV1));
   encoders.push_back(new Tencoder(_l("WMV 8"),CODEC_ID_WMV2));
   encoders.push_back(new Tencoder(_l("MJPEG"),CODEC_ID_MJPEG));
   encoders.push_back(new Tencoder(_l("Lossless JPEG"),CODEC_ID_LJPEG));
   encoders.push_back(new Tencoder(_l("HuffYUV"),CODEC_ID_HUFFYUV));
   encoders.push_back(new Tencoder(_l("FFV1"),CODEC_ID_FFV1));
   encoders.push_back(new Tencoder(_l("DV"),CODEC_ID_DVVIDEO));
   //encoders.push_back(new Tencoder(_l("SNOW (experimental)"),CODEC_ID_SNOW));
   encoders.push_back(new Tencoder(_l("FLV1"),CODEC_ID_FLV1));
  }
}
void TvideoCodecLibavcodec::create(void)
{
 ownmatrices=false;
 deci->getLibavcodec(&libavcodec);
 ok=libavcodec?libavcodec->ok:false;
 avctx=NULL;avcodec=NULL;frame=NULL;quantBytes=1;statsfile=NULL;threadcount=0;codecinited=false;extradata=NULL;theorart=false;
 ffbuf=NULL;ffbuflen=0;
 codecName[0]='\0';
 ccDecoder=NULL;
 autoSkipingLoopFilter= false;
 h264_has_start_code = false;
 svc = 0;
}

void TvideoCodecLibavcodec::createSVC(void)
{
 ownmatrices=false;
 deci->getLibavcodecSVC(&libavcodec);
 ok=libavcodec?libavcodec->ok:false;
 avctx=NULL;avcodec=NULL;frame=NULL;quantBytes=1;statsfile=NULL;threadcount=0;codecinited=false;extradata=NULL;theorart=false;
 ffbuf=NULL;ffbuflen=0;
 codecName[0]='\0';
 ccDecoder=NULL;
 autoSkipingLoopFilter= false;
 h264_has_start_code = false;
 svc = 1;
}

TvideoCodecLibavcodec::~TvideoCodecLibavcodec()
{
 end();
 if (libavcodec) libavcodec->Release();
 if (extradata) delete extradata;
 if (ffbuf) free(ffbuf);
 if (ccDecoder) delete ccDecoder;
}
void TvideoCodecLibavcodec::end(void)
{
 if (statsfile)
  {
   fflush(statsfile);
   fclose(statsfile);
   statsfile=NULL;
  }
 if (avctx)
  {
   if (ownmatrices)
    {
     if (avctx->intra_matrix) free(avctx->intra_matrix);
     if (avctx->inter_matrix) free(avctx->inter_matrix);
     ownmatrices=false;
    }
   if (avctx->slice_offset) free(avctx->slice_offset);
   if (codecinited) libavcodec->avcodec_close(avctx);codecinited=false;
   if (threadcount) libavcodec->avcodec_thread_free(avctx);
   libavcodec->av_free(avctx);avctx=NULL;
   libavcodec->av_free(frame);frame=NULL;
  }
 avcodec=NULL;
}

//----------------------------- decompression -----------------------------
bool TvideoCodecLibavcodec::beginDecompress(TffPictBase &pict,FOURCC fcc,const CMediaType &mt,int sourceFlags)
{
 oldpict.rtStop = 0;
 h264_has_start_code = false;
 avcodec=libavcodec->avcodec_find_decoder(codecId);
 if (!avcodec) return false;

 avctx=libavcodec->avcodec_alloc_context(this);

 int numthreads=deci->getParam2(IDFF_numLAVCdecThreads);
 if (numthreads>1 && sup_threads_dec(codecId))
  libavcodec->avcodec_thread_init(avctx,threadcount=numthreads);
 else
  threadcount=0;

 frame=libavcodec->avcodec_alloc_frame();
 avctx->width =pict.rectFull.dx;
 avctx->height=pict.rectFull.dy;
 intra_matrix=avctx->intra_matrix=(uint16_t*)calloc(sizeof(uint16_t),64);
 inter_matrix=avctx->inter_matrix=(uint16_t*)calloc(sizeof(uint16_t),64);
 ownmatrices=true;

 grayscale=deci->getParam2(IDFF_grayscale);
 if (grayscale && codecId!=CODEC_ID_THEORA) avctx->flags|=CODEC_FLAG_GRAY;

 avctx->codec_tag=fcc;
 avctx->workaround_bugs=deci->getParam2(IDFF_workaroundBugs);
 avctx->error_concealment=deci->getParam2(IDFF_errorConcealment);
 avctx->error_recognition=deci->getParam2(IDFF_errorRecognition);
 if (sourceFlags&SOURCE_TRUNCATED || mpeg12_codec(codecId) || codecId==CODEC_ID_MJPEG)
  avctx->flags|=CODEC_FLAG_TRUNCATED;
 if (mpeg12_codec(codecId) && deci->getParam2(IDFF_fastMpeg2))
  avctx->flags2=CODEC_FLAG2_FAST;
 if (codecId==CODEC_ID_H264)
  if (int skip=deci->getParam2(IDFF_fastH264))
   avctx->skip_loop_filter=skip&2?AVDISCARD_ALL:AVDISCARD_NONREF;
 initialSkipLoopFilter= avctx->skip_loop_filter;

 avctx->debug_mv=1;//(deci->getParam2(IDFF_isVis) & deci->getParam2(IDFF_visMV));

 avctx->idct_algo=limit(deci->getParam2(IDFF_idct),0,6);
 if (extradata) delete extradata;
 extradata=new Textradata(mt,FF_INPUT_BUFFER_PADDING_SIZE);
 if (extradata->size>0 && (codecId!=CODEC_ID_H264 || fcc==FOURCC_AVC1))
  {
   avctx->extradata_size=(int)extradata->size;
   avctx->extradata=extradata->data;
   sendextradata=mpeg1_codec(codecId);
   if (fcc==FOURCC_AVC1 && mt.formattype==FORMAT_MPEG2Video)
    {
     const MPEG2VIDEOINFO *mpeg2info=(const MPEG2VIDEOINFO*)mt.pbFormat;
     avctx->nal_length_size=mpeg2info->dwFlags;
    }
   else if (fcc==FOURCC_THEO)
    {
     if (mt.formattype==FORMAT_RLTheora)
      {
       theorart=true;
       const uint8_t *src=(const uint8_t*)avctx->extradata;
       size_t dstsize=extradata->size;
       uint8_t *dst=(uint8_t*)malloc(dstsize),*dst0=dst;
       dst[1]=src[0+0*sizeof(DWORD)];
       dst[0]=src[1+0*sizeof(DWORD)];
       DWORD len0=*(DWORD*)&src[0*sizeof(DWORD)];
       memcpy(dst+2,src+16,len0);
       dst+=2+len0;

       dst[1]=src[0+1*sizeof(DWORD)];
       dst[0]=src[1+1*sizeof(DWORD)];
       DWORD len1=*(DWORD*)&src[1*sizeof(DWORD)];
       memcpy(dst+2,src+16+len0,len1);
       dst+=2+len1;

       dst[1]=src[0+2*sizeof(DWORD)];
       dst[0]=src[1+2*sizeof(DWORD)];
       DWORD len2=*(DWORD*)&src[2*sizeof(DWORD)];
       memcpy(dst+2,src+16+len0+len1,len2);

       extradata->clear();
       extradata->set(dst0,dstsize,0,true);
       free(dst0);
       avctx->extradata=extradata->data;
       avctx->extradata_size=(int)extradata->size;
      }
     if (extradata->size>2 && extradata->data[2]==0)
      {
       const uint8_t *src=(const uint8_t*)avctx->extradata;
       size_t dstsize=extradata->size;
       uint8_t *dst=(uint8_t*)malloc(dstsize);
       long len=*(long*)src;
       dst[1]=((uint8_t*)&len)[0];
       dst[0]=((uint8_t*)&len)[1];
       memcpy(dst+2,src+4,len);
       *((int16_t*)(dst+2+len))=int16_t(extradata->size-4-len);
       memcpy(dst+2+len+2,src+4+len,extradata->size-4-len);
       extradata->clear();
       extradata->set(dst,dstsize,0,true);
       free(dst);
       avctx->extradata=extradata->data;
       avctx->extradata_size=(int)extradata->size;
      }
    }
  }
 else
  sendextradata=false;

 if (fcc==FOURCC_RLE4 || fcc==FOURCC_RLE8 || fcc==FOURCC_CSCD || sup_palette(codecId))
  {
   BITMAPINFOHEADER bih;ExtractBIH(mt,&bih);
   avctx->bits_per_coded_sample=bih.biBitCount;
   if (avctx->bits_per_coded_sample<=8 || codecId==CODEC_ID_PNG)
    {
     avctx->palctrl=&pal;
     pal.palette_changed=1;
     const void *palette;int palettesize;
     if (!extradata->data)
      switch (avctx->bits_per_coded_sample)
       {
        case 2:
         palette=qt_default_palette_4;
         palettesize=4*4;
         break;
        case 4:
         palette=qt_default_palette_16;
         palettesize=16*4;
         break;
        default:
        case 8:
         palette=qt_default_palette_256;
         palettesize=256*4;
         break;
       }
     else
      {
       palette=extradata->data;
       palettesize=AVPALETTE_SIZE;
      }
     memcpy(pal.palette,palette,std::min(palettesize,AVPALETTE_SIZE));
    }
  }
 else if (extradata->data && (codecId==CODEC_ID_RV10 || codecId==CODEC_ID_RV20))
  {
   #pragma pack(push, 1)
   struct rvinfo
    {
/*     DWORD dwSize, fcc1, fcc2;
     WORD w, h, bpp;
     DWORD unk1, fps, type1, type2;*/
     DWORD type1, type2;
     BYTE w2, h2, w3, h3;
    } __attribute__((packed));
   #pragma pack(pop)

   const uint8_t *src=(const uint8_t*)avctx->extradata;
   size_t dstsize=extradata->size - 26;
   uint8_t *dst=(uint8_t*)malloc(dstsize);
   memcpy(dst,src+26,dstsize);
   extradata->clear();
   extradata->set(dst,dstsize,0,true);
   free(dst);
   avctx->extradata=extradata->data;
   avctx->extradata_size=(int)extradata->size;

   rvinfo *info=(rvinfo*)extradata->data;
   avctx->sub_id=info->type2;bswap(avctx->sub_id);
  }
  
 if (libavcodec->avcodec_open(avctx,avcodec)<0) {
 	return false;
 }
 if (codecId == CODEC_ID_H264
    && !libavcodec->avcodec_h264_decode_init_is_avc(avctx)
    && isTSfile())
  h264_has_start_code = true;

 if (h264_has_start_code)
  codedPictureBuffer.init();

 pict.csp=avctx->pix_fmt!=PIX_FMT_NONE?csp_lavc2ffdshow(avctx->pix_fmt):FF_CSP_420P;
 if (avctx->sample_aspect_ratio.num && avctx->sample_aspect_ratio.den) {
  pict.setSar(avctx->sample_aspect_ratio);
 }
 containerSar=pict.rectFull.sar;
 dont_use_rtStop_from_upper_stream = (sourceFlags&SOURCE_NEROAVC || avctx->codec_tag==FOURCC_MPG1 || avctx->codec_tag==FOURCC_MPG2) && avctx->codec_tag!=FOURCC_THEO;
 avgTimePerFrame=-1;
 codecinited=true;
 wasKey=false;
 segmentTimeStart=0;
 posB=1;
 return true;
}

// return true for TS, PS files and no extension (DVBSource etc).
bool TvideoCodecLibavcodec::isTSfile(void)
{   
 const char_t *sourceFullFlnm;
 ffstring sourceExt;
 sourceFullFlnm = deci->getSourceName();
 extractfileext(sourceFullFlnm,sourceExt);
 sourceExt.ConvertToLowerCase();
 return (sourceExt == _l("ts") || sourceExt == _l("m2ts") || sourceExt == _l("m2t") || sourceExt == _l("mts") || sourceExt == _l("mpg")  || sourceExt == _l("mpeg") || sourceExt == _l(""));
}

void TvideoCodecLibavcodec::onGetBuffer(AVFrame *pic)
{
}

void TvideoCodecLibavcodec::handle_user_data(const uint8_t *buf,int buf_len)
{
 if(buf_len > 4 && *(DWORD*)buf == 0xf8014343)
  {
   if (!ccDecoder) ccDecoder=new TccDecoder(deciV);
   ccDecoder->decode(buf+2,buf_len-2);
  }
}

HRESULT TvideoCodecLibavcodec::flushDec(void)
{
 return decompress(NULL,0,NULL);
}
HRESULT TvideoCodecLibavcodec::decompress(const unsigned char *src,size_t srcLen0,IMediaSample *pIn)
{
 bool isSyncPoint = pIn && pIn->IsSyncPoint() == S_OK;
 if (codecId==CODEC_ID_FFV1) // libavcodec can crash or loop infinitely when first frame after seeking is not keyframe
  {
   if (!wasKey)
    if (isSyncPoint)
     wasKey=true;
    else
     return S_OK;
  }

 unsigned int skip=0;

 if (src && (codecId==CODEC_ID_RV10 || codecId==CODEC_ID_RV20) && avctx->sub_id)
  {
   avctx->slice_count=src[0]+1;
   if (!avctx->slice_offset) avctx->slice_offset=(int*)malloc(sizeof(int)*1000);
   for (int i=0;i<avctx->slice_count;i++)
    avctx->slice_offset[i]=((DWORD*)(src+1))[2*i+1];
   skip=1+2*sizeof(DWORD)*avctx->slice_count;
  }
 else if (codecId==CODEC_ID_COREPNG)
  avctx->corepng_frame_type=pIn && pIn->IsSyncPoint()==S_OK?SAMPLE_I:SAMPLE_P;
 else if (src && theorart)
  {
   struct _TheoraPacket
    {
     long  bytes;
     long  b_o_s;
     long  e_o_s;
     int64_t  granulepos;
     int64_t  packetno;
    };
   _TheoraPacket *packet=(_TheoraPacket*)src;
   if (packet->bytes==0 || src[sizeof(_TheoraPacket)]&0x80)
    return S_OK;
   skip+=sizeof(_TheoraPacket);
   avctx->granulepos=packet->granulepos;
  }

 src+=skip;int size=int(srcLen0-skip);

 if (pIn)
  pIn->GetTime(&rtStart,&rtStop);

 b[posB].rtStart=rtStart;
 b[posB].rtStop=rtStop;
 b[posB].srcSize=size;
 posB=1-posB; /* posB++; if(posB==2) posB=0;*/

 if (codecId==CODEC_ID_H264)
  {
   if(autoSkipingLoopFilter)
    {
     if(deciV->getLate()<=0)
      {
       avctx->skip_loop_filter= initialSkipLoopFilter;
       //avctx->skip_frame = AVDISCARD_NONE;
       autoSkipingLoopFilter= false;
      }
    }
   else
    {
     if(deciV->shouldSkipH264loopFilter())
      {
       avctx->skip_loop_filter=AVDISCARD_ALL;
       //avctx->skip_frame = AVDISCARD_NONREF;
       autoSkipingLoopFilter= true;
      }
    }
  }

 if (src && h264_has_start_code)
  size = codedPictureBuffer.append(src, size);

 while (!src || size>0)
  {
   int got_picture,used_bytes;

   avctx->parserRtStart=&rtStart; // needed for mpeg1/2
   avctx->reordered_opaque = rtStart;
   avctx->reordered_opaque2 = rtStop;
   avctx->reordered_opaque3 = size;

   if (sendextradata)
    {
     used_bytes=libavcodec->avcodec_decode_video(avctx,frame,&got_picture,extradata->data,(int)extradata->size);
     sendextradata=false;
     if (used_bytes>0) used_bytes=0;
    }
   else
    {
     unsigned int neededsize=size+FF_INPUT_BUFFER_PADDING_SIZE;

     if (ffbuflen<neededsize)
      ffbuf=(unsigned char*)realloc(ffbuf,ffbuflen=neededsize);

     if (src)
      {
       if (codecId == CODEC_ID_H264)
        {
         if (h264_has_start_code)
          used_bytes = codedPictureBuffer.send(&got_picture);
         else
          {
           memcpy(ffbuf,src,size);memset(ffbuf+size,0,FF_INPUT_BUFFER_PADDING_SIZE);
           if (h264RandomAccess.search(ffbuf, size))
            {
             memcpy(ffbuf,src,size);memset(ffbuf+size,0,FF_INPUT_BUFFER_PADDING_SIZE);
			//mbl call decoder
             used_bytes=libavcodec->avcodec_decode_video(avctx,frame,&got_picture,ffbuf,size);
             if (used_bytes < 0)
              return S_OK;
             h264RandomAccess.judgeUsability(&got_picture);
            }
           else
            return S_OK;
          }
        }
       else
        {
         memcpy(ffbuf,src,size);memset(ffbuf+size,0,FF_INPUT_BUFFER_PADDING_SIZE);
         used_bytes=libavcodec->avcodec_decode_video(avctx,frame,&got_picture,ffbuf,size);
        }
      }
     else
      used_bytes=libavcodec->avcodec_decode_video(avctx,frame,&got_picture,NULL,0);
    }

   if (used_bytes<0)
    return S_OK;

   if (got_picture && frame->data[0])
    {
     int frametype;
     if (avctx->codec_id==CODEC_ID_H261)
      {
       frametype=FRAME_TYPE::I;
      }
     else
      {
       switch (frame->pict_type)
        {
         case FF_P_TYPE:frametype=FRAME_TYPE::P;break;
         case FF_B_TYPE:frametype=FRAME_TYPE::B;break;
         case FF_I_TYPE:frametype=FRAME_TYPE::I;break;
         case FF_S_TYPE:frametype=FRAME_TYPE::GMC;break;
         case FF_SI_TYPE:frametype=FRAME_TYPE::SI;break;
         case FF_SP_TYPE:frametype=FRAME_TYPE::SP;break;
         case 0:frametype=pIn && pIn->IsSyncPoint()==S_OK?FRAME_TYPE::I:FRAME_TYPE::P;break;
         default:frametype=FRAME_TYPE::UNKNOWN;break;
        }
       }

     if (pIn && pIn->IsPreroll()==S_OK)
      return sinkD->deliverPreroll(frametype);

     int fieldtype = frame->interlaced_frame ?
                         (frame->top_field_first ?
                             FIELD_TYPE::INT_TFF :
                             FIELD_TYPE::INT_BFF):
                         FIELD_TYPE::PROGRESSIVE_FRAME;

     if (frame->play_flags&CODEC_FLAG_QPEL)
      frametype|=FRAME_TYPE::QPEL;

     int csp=csp_lavc2ffdshow(avctx->pix_fmt);

     if (grayscale) // workaround for green picture when decoding mpeg with CODEC_FLAG_GRAY, the problem is probably somewhere else
      {
       const TcspInfo* cspinfo=csp_getInfo(csp);
       for (unsigned int i=1;i<cspinfo->numPlanes;i++)
        {
         if (frame->data[i][0]!=cspinfo->black[i]
             || (codecId!=CODEC_ID_MPEG4 && codecId!=CODEC_ID_MPEG2VIDEO && codecId!=CODEC_ID_MPEG1VIDEO && codecId!=CODEC_ID_VC1 && codecId!=CODEC_ID_WMV3 && codecId!=CODEC_ID_SVQ3 && codecId!=CODEC_ID_HUFFYUV)
            )
          memset(frame->data[i],cspinfo->black[i],frame->linesize[i]*avctx->height>>cspinfo->shiftY[i]);
        }
      }

     Trect r(0,0,avctx->width,avctx->height);

     if (avctx->sample_aspect_ratio.num &&
         !(connectedSplitter == TffdshowVideoInputPin::MPC_matroska_splitter && avctx->sample_aspect_ratio.num==1 && avctx->sample_aspect_ratio.den==1)
        )  // With MPC's internal matroska splitter, AR is not reliable.
      r.sar=avctx->sample_aspect_ratio;
     else
      r.sar=containerSar;

     quants=frame->qscale_table;
     quantsStride=frame->qstride;
     quantType=frame->qscale_type;
     quantsDx=(r.dx+15)>>4;quantsDy=(r.dy+15)>>4;

     const stride_t linesize[4]={frame->linesize[0],frame->linesize[1],frame->linesize[2],frame->linesize[3]};

     TffPict pict(csp,frame->data,linesize,r,true,frametype,fieldtype,srcLen0,pIn,avctx->palctrl); //TODO: src frame size
     pict.gmcWarpingPoints=frame->num_sprite_warping_points;pict.gmcWarpingPointsReal=frame->real_sprite_warping_points;

     if (h264_has_start_code)
      {
       pict.rtStart = frame->reordered_opaque;
       pict.rtStop = frame->reordered_opaque2;
       pict.srcSize = (size_t)frame->reordered_opaque3;
      }
     else if (dont_use_rtStop_from_upper_stream)
      {
       pict.rtStart = frame->reordered_opaque;
       pict.srcSize = (size_t)frame->reordered_opaque3; // FIXME this is not correct for MPEG-1/2 that use SOURCE_TRUNCATED. (Just for OSD, not that important bug)

       if (pict.rtStart==REFTIME_INVALID)
        pict.rtStart=oldpict.rtStop;

       if (avgTimePerFrame==-1)
        deciV->getAverageTimePerFrame(&avgTimePerFrame);

       if (avgTimePerFrame)
        {
         pict.rtStop=pict.rtStart+avgTimePerFrame+frame->repeat_pict*avgTimePerFrame/2;
        }
       else if (avctx->time_base.num && avctx->time_base.den)
        {
         REFERENCE_TIME duration = REF_SECOND_MULT * avctx->time_base.num / avctx->time_base.den;
         pict.rtStop = pict.rtStart + duration;
         if (frame->repeat_pict)
          pict.rtStop += (duration >> 1) * frame->repeat_pict;
        }
       else
        {
         pict.rtStop=pict.rtStart+1;
        }

       if (avctx->codec_tag==FOURCC_MPG1 || avctx->codec_tag==FOURCC_MPG2)
        pict.mediatimeStart=pict.mediatimeStop=REFTIME_INVALID;

       oldpict=pict;
      }
     else if (theorart)
      {
       pict.rtStart = frame->reordered_opaque - segmentTimeStart;
       pict.rtStop  = pict.rtStart + 1;
      }
     else if (avctx->has_b_frames)
      {
       // do not reorder timestamps in this case.
       // Timestamps simply increase. 
       // ex: AVI files

       pict.rtStart=b[posB].rtStart; 
       pict.rtStop=b[posB].rtStop;
       pict.srcSize=b[posB].srcSize;
      }

     HRESULT hr=sinkD->deliverDecodedSample(pict);
     if (FAILED(hr) || (used_bytes && sinkD->acceptsManyFrames()!=S_OK) || avctx->codec_id==CODEC_ID_LOCO)
      return hr;
    }
   else
    {
     if (!src)
      break;
    }

   if(!used_bytes && codecId==CODEC_ID_SVQ3)
    return S_OK;

   src+=used_bytes;
   size-=used_bytes;
  }
 return S_OK;
}

bool TvideoCodecLibavcodec::onSeek(REFERENCE_TIME segmentStart)
{
oldpict.rtStop = 0;
 wasKey=false;
 segmentTimeStart=segmentStart;
 posB=1;
 b[0].rtStart=b[1].rtStart=b[0].rtStop=b[0].rtStop=0;b[0].srcSize=b[1].srcSize=0;
 if (ccDecoder) ccDecoder->onSeek();
 codedPictureBuffer.onSeek();
 h264RandomAccess.onSeek();
 return avctx?(libavcodec->avcodec_flush_buffers(avctx),true):false;
}
bool TvideoCodecLibavcodec::onDiscontinuity(void)
{
 wasKey=false;
 if (ccDecoder) ccDecoder->onSeek();
 codedPictureBuffer.onSeek();
 h264RandomAccess.onSeek();
 return avctx?(libavcodec->avcodec_flush_buffers(avctx),true):false;
}

const char_t* TvideoCodecLibavcodec::getName(void) const
{
 if (avcodec)
  {
   tsnprintf_s(codecName, countof(codecName), _TRUNCATE, _l("libavcodec %s"), (const char_t*)text<char_t>(avcodec->name));
   return codecName;
  }
 else return _l("libavcodec");
}
void TvideoCodecLibavcodec::getEncoderInfo(char_t *buf,size_t buflen) const
{
 int xvid_build,divx_version,divx_build,lavc_build;
 if (avctx && (mpeg12_codec(codecId) || mpeg4_codec(codecId) || x264_codec(codecId) || codecId==CODEC_ID_FLV1))
  {
   libavcodec->avcodec_get_encoder_info(avctx,&xvid_build,&divx_version,&divx_build,&lavc_build);
   if (xvid_build)
    tsnprintf_s(buf, buflen, _TRUNCATE, _l("XviD build %i"), xvid_build);
   else if (lavc_build)
    tsnprintf_s(buf, buflen, _TRUNCATE, _l("libavcodec build %i"), lavc_build);
   else if (divx_version || divx_build)
    tsnprintf_s(buf, buflen, _TRUNCATE, _l("DivX version %i.%02i, build %i"), divx_version/100, divx_version%100, divx_build);
   else
    ff_strncpy(buf,_l("unknown"),buflen);
  }
 else
  ff_strncpy(buf,_l("unknown"),buflen);
 buf[buflen-1]='\0';
}

void TvideoCodecLibavcodec::line(unsigned char *dst,unsigned int _x0,unsigned int _y0,unsigned int _x1,unsigned int _y1,stride_t strideY)
{
 drawline< TaddColor<100> >(_x0,_y0,_x1,_y1,100,dst,strideY);
}

void TvideoCodecLibavcodec::draw_arrow(uint8_t *buf, int sx, int sy, int ex, int ey, stride_t stride,int mulx,int muly,int dstdx,int dstdy)
{
 sx=limit(mulx*sx>>12,0,dstdx-1);sy=limit(muly*sy>>12,0,dstdy-1);
 ex=limit(mulx*ex>>12,0,dstdx-1);ey=limit(muly*ey>>12,0,dstdy-1);
    int dx,dy;

    dx= ex - sx;
    dy= ey - sy;

    if(dx*dx + dy*dy > 3*3){
        int rx=  dx + dy;
        int ry= -dx + dy;
        int length= ff_sqrt((rx*rx + ry*ry)<<8);

        //FIXME subpixel accuracy
        rx= roundDiv(rx*3<<4, length);
        ry= roundDiv(ry*3<<4, length);

        line(buf, sx, sy, sx + rx, sy + ry, stride);
        line(buf, sx, sy, sx - ry, sy + rx, stride);
    }
    line(buf, sx, sy, ex, ey, stride);
}

bool TvideoCodecLibavcodec::drawMV(unsigned char *dst,unsigned int dstdx,stride_t stride,unsigned int dstdy) const
{
 if (!frame->motion_val || !frame->mb_type || !frame->motion_val[0]) return false;

 #define IS_8X8(a)  ((a)&MB_TYPE_8x8)
 #define IS_16X8(a) ((a)&MB_TYPE_16x8)
 #define IS_8X16(a) ((a)&MB_TYPE_8x16)
 #define IS_INTERLACED(a) ((a)&MB_TYPE_INTERLACED)
 #define USES_LIST(a, list) ((a) & ((MB_TYPE_P0L0|MB_TYPE_P1L0)<<(2*(list))))

 const int shift= 1 + ((frame->play_flags&CODEC_FLAG_QPEL)?1:0);
 const int mv_sample_log2= 4 - frame->motion_subsample_log2;
 const int mv_stride= (frame->mb_width << mv_sample_log2) + (avctx->codec_id == CODEC_ID_H264 ? 0 : 1);
 int direction=0;

 int mulx=(dstdx<<12)/avctx->width;
 int muly=(dstdy<<12)/avctx->height;

 for(int mb_y=0; mb_y<frame->mb_height; mb_y++)
  for(int mb_x=0; mb_x<frame->mb_width; mb_x++)
   {
    const int mb_index= mb_x + mb_y*frame->mb_stride;
    if (!USES_LIST(frame->mb_type[mb_index], direction))
     continue;
    if(IS_8X8(frame->mb_type[mb_index]))
     for(int i=0; i<4; i++)
      {
       int sx= mb_x*16 + 4 + 8*(i&1) ;
       int sy= mb_y*16 + 4 + 8*(i>>1);
       int xy= (mb_x*2 + (i&1) + (mb_y*2 + (i>>1))*mv_stride)<<(mv_sample_log2-1);
       int mx= (frame->motion_val[direction][xy][0]>>shift) + sx;
       int my= (frame->motion_val[direction][xy][1]>>shift) + sy;
       draw_arrow(dst,sx,sy,mx,my,stride,mulx,muly,dstdx,dstdy);
      }
    else if(IS_16X8(frame->mb_type[mb_index]))
     for(int i=0; i<2; i++)
      {
       int sx=mb_x*16 + 8;
       int sy=mb_y*16 + 4 + 8*i;
       int xy=(mb_x*2 + (mb_y*2 + i)*mv_stride) << (mv_sample_log2-1);
       int mx=frame->motion_val[direction][xy][0]>>shift;
       int my=frame->motion_val[direction][xy][1]>>shift;
       if (IS_INTERLACED(frame->mb_type[mb_index]))
        my*=2;
       draw_arrow(dst,sx,sy,mx+sx,my+sy,stride,mulx,muly,dstdx,dstdy);
      }
    else if(IS_8X16(frame->mb_type[mb_index]))
     for(int i=0; i<2; i++)
      {
       int sx=mb_x*16 + 4 + 8*i;
       int sy=mb_y*16 + 8;
       int xy= (mb_x*2 + i + mb_y*2*mv_stride) << (mv_sample_log2-1);
       int mx=(frame->motion_val[direction][xy][0]>>shift);
       int my=(frame->motion_val[direction][xy][1]>>shift);
       if(IS_INTERLACED(frame->mb_type[mb_index]))
        my*=2;
       draw_arrow(dst,sx,sy,mx+sx,my+sy,stride,mulx,muly,dstdx,dstdy);
      }
    else
     {
      int sx= mb_x*16 + 8;
      int sy= mb_y*16 + 8;
      int xy= (mb_x + mb_y*mv_stride) << mv_sample_log2;
      int mx= (frame->motion_val[direction][xy][0]>>shift) + sx;
      int my= (frame->motion_val[direction][xy][1]>>shift) + sy;
      draw_arrow(dst,sx,sy,mx,my,stride,mulx,muly,dstdx,dstdy);
     }
   }
 #undef IS_8X8
 #undef IS_16X8
 #undef IS_8X16
 #undef IS_INTERLACED
 #undef USES_LIST
 return true;
}

//------------------------------ compression ------------------------------
void TvideoCodecLibavcodec::getCompressColorspaces(Tcsps &csps,unsigned int outDx,unsigned int outDy)
{
 switch (coCfg->codecId)
  {
   case CODEC_ID_HUFFYUV:
    if (coCfg->huffyuv_csp==0)
     csps.add(FF_CSP_422P);
    else
     csps.add(FF_CSP_420P);
    break;
   case CODEC_ID_FFV1:
    switch (coCfg->ffv1_csp)
     {
      case FOURCC_YV12:csps.add(FF_CSP_420P);break;
      case FOURCC_444P:csps.add(FF_CSP_444P);break;
      case FOURCC_422P:csps.add(FF_CSP_422P);break;
      case FOURCC_411P:csps.add(FF_CSP_411P);break;
      case FOURCC_410P:csps.add(FF_CSP_410P);break;
      case FOURCC_RGB3:csps.add(FF_CSP_RGB32);break;
     }
    break;
   case CODEC_ID_MJPEG:
    csps.add(FF_CSP_420P|FF_CSP_FLAGS_YUV_JPEG);
    break;
   case CODEC_ID_LJPEG:
    if (coCfg->ljpeg_csp==FOURCC_RGB3)
     csps.add(FF_CSP_RGB32);
    else
     csps.add(FF_CSP_420P);
    break;
   case CODEC_ID_DVVIDEO:
    {
     std::vector<const DVprofile*> profiles=coCfg->getDVprofile(outDx,outDy);
     if (!profiles.empty())
      {
       for (std::vector<const DVprofile*>::const_iterator p=profiles.begin();p!=profiles.end();p++)
        csps.add(csp_lavc2ffdshow((*p)->pix_fmt));
      }
     else
      csps.add(FF_CSP_420P); //won't be used anyway
     break;
    }
   case CODEC_ID_SVQ1:
    csps.add(FF_CSP_410P);
    break;
   default:
    csps.add(FF_CSP_420P);
    break;
  }
}

bool TvideoCodecLibavcodec::supExtradata(void)
{
 return coCfg->codecId==CODEC_ID_HUFFYUV || (sup_globalheader(coCfg->codecId) && coCfg->globalHeader);
}
bool TvideoCodecLibavcodec::getExtradata(const void* *ptr,size_t *len)
{
 if (!avctx || !len) return false;
 *len=avctx->extradata_size;
 if (ptr) *ptr=avctx->extradata;
 return true;
}

LRESULT TvideoCodecLibavcodec::beginCompress(int cfgcomode,int csp,const Trect &r)
{
 _mm_empty();
 avctx=libavcodec->avcodec_alloc_context();
 frame=libavcodec->avcodec_alloc_frame();

 avcodec=libavcodec->avcodec_find_encoder((CodecID)coCfg->codecId);
 if (!avcodec) return ICERR_ERROR;

 this->cfgcomode=cfgcomode;

 if (coCfg->numthreads>1 && sup_threads(coCfg->codecId))
  libavcodec->avcodec_thread_init(avctx,threadcount=coCfg->numthreads);
 else
  threadcount=0;

 avctx->width=r.dx;avctx->height=r.dy;
 mb_width =(avctx->width+15)/16;
 mb_height=(avctx->height+15)/16;
 mb_count=mb_width*mb_height;
 avctx->time_base.den=deci->getParam2(IDFF_enc_fpsRate);
 avctx->time_base.num=deci->getParam2(IDFF_enc_fpsScale);
 if (avctx->time_base.den>(1<<16)-1)
  {
   avctx->time_base.num=(int)(0.5+(double)avctx->time_base.num/avctx->time_base.den*((1<<16)-1));
   avctx->time_base.den=(1<<16)-1;
  }
 if (coCfg->codecId==CODEC_ID_FFV1)
  avctx->gop_size=coCfg->ffv1_key_interval;
 else
  avctx->gop_size=coCfg->max_key_interval;

 avctx->codec_tag=coCfg->fourcc;
 if (sup_interlace(coCfg->codecId) && coCfg->interlacing) avctx->flags|=CODEC_FLAG_INTERLACED_DCT|CODEC_FLAG_INTERLACED_ME;
 if (sup_globalheader(coCfg->codecId) && coCfg->globalHeader) avctx->flags|=CODEC_FLAG_GLOBAL_HEADER;
 if (sup_part(coCfg->codecId) &&coCfg->part) avctx->flags|=CODEC_FLAG_PART;
 if (coCfg->codecId!=CODEC_ID_SNOW && coCfg->codecId!=CODEC_ID_H261) avctx->mb_decision=coCfg->me_hq;
 if (coCfg->me_4mv && sup_4mv(coCfg->codecId)) avctx->flags|=CODEC_FLAG_4MV;
 if (sup_qpel(coCfg->codecId) && coCfg->me_qpel)
  {
   avctx->flags|=CODEC_FLAG_QPEL;
   avctx->me_subpel_quality=coCfg->me_subq;
  }
 psnr=deci->getParam2(IDFF_enc_psnr);if (psnr) avctx->flags|=CODEC_FLAG_PSNR;
 if (coCfg->me_mv0 && sup_me_mv0(coCfg->codecId) && coCfg->me_hq>0) avctx->flags|=CODEC_FLAG_MV0;
 avctx->me_cmp=coCfg->me_cmp;if (coCfg->me_cmp_chroma && !coCfg->gray) avctx->me_cmp|=FF_CMP_CHROMA;
 avctx->me_sub_cmp=coCfg->me_subcmp;if (coCfg->me_subcmp_chroma && !coCfg->gray) avctx->me_sub_cmp|=FF_CMP_CHROMA;
 avctx->mb_cmp=coCfg->mb_cmp;if (coCfg->mb_cmp_chroma && !coCfg->gray) avctx->mb_cmp|=FF_CMP_CHROMA;
 avctx->dia_size=Tlibavcodec::dia_sizes[coCfg->dia_size].size;
 avctx->last_predictor_count=coCfg->me_last_predictor_count;
 avctx->pre_me=coCfg->me_prepass;
 avctx->me_pre_cmp=coCfg->me_precmp;if (coCfg->me_precmp_chroma && !coCfg->gray) avctx->me_pre_cmp|=FF_CMP_CHROMA;
 avctx->pre_dia_size=Tlibavcodec::dia_sizes[coCfg->dia_size_pre].size;
 avctx->nsse_weight=coCfg->me_nsse_weight;
 if (sup_gray(coCfg->codecId) && coCfg->gray) avctx->flags|=CODEC_FLAG_GRAY;

 if (coCfg->isQuantControlActive())
  {
   avctx->qmin_i=coCfg->limitq(coCfg->q_i_min);avctx->qmax_i=coCfg->limitq(coCfg->q_i_max);
   avctx->qmin  =coCfg->limitq(coCfg->q_p_min);avctx->qmax  =coCfg->limitq(coCfg->q_p_max);
   avctx->qmin_b=coCfg->limitq(coCfg->q_b_min);avctx->qmax_b=coCfg->limitq(coCfg->q_b_max);
  }
 else
  {
   avctx->qmin_i=avctx->qmin=avctx->qmin_b=coCfg->getMinMaxQuant().first;
   avctx->qmax_i=avctx->qmax=avctx->qmax_b=coCfg->getMinMaxQuant().second;
  }

 if (coCfg->codecId!=CODEC_ID_MJPEG)
  {
   avctx->i_quant_factor=coCfg->i_quant_factor/100.0f;
   avctx->i_quant_offset=coCfg->i_quant_offset/100.0f;
  }
 else
  {
   avctx->i_quant_factor=1.0f;
   avctx->i_quant_offset=0.0f;
  }

 if (coCfg->trellisquant && sup_trellisQuant(coCfg->codecId))
  {
   avctx->trellis=1;
   if (coCfg->me_cbp_rd && sup_cbp_rd(coCfg->codecId)) avctx->flags|=CODEC_FLAG_CBP_RD;
  }
 if (coCfg->quant_type==QUANT::MPEG && sup_MPEGquant(coCfg->codecId))
  avctx->mpeg_quant=1;
 else if (coCfg->quant_type==QUANT::CUSTOM && sup_customQuantTables(coCfg->codecId))
  {
   avctx->mpeg_quant=1;
   avctx->intra_matrix=(uint16_t*)malloc(sizeof(uint16_t)*64);
   avctx->inter_matrix=(uint16_t*)malloc(sizeof(uint16_t)*64);
   ownmatrices=true;
   for (int i=0;i<64;i++)
    {
     avctx->intra_matrix[i]=((uint8_t*)&coCfg->qmatrix_intra_custom0)[i];
     avctx->inter_matrix[i]=((uint8_t*)&coCfg->qmatrix_inter_custom0)[i];
    }
  }

 if (sup_quantBias(coCfg->codecId))
  {
   if (coCfg->isIntraQuantBias) avctx->intra_quant_bias=coCfg->intraQuantBias;
   if (coCfg->isInterQuantBias) avctx->inter_quant_bias=coCfg->interQuantBias;
  }
 avctx->dct_algo=coCfg->dct_algo;
 if (sup_qns(coCfg->codecId)) avctx->quantizer_noise_shaping=coCfg->qns;
 if (mpeg2_codec(coCfg->codecId)) avctx->intra_dc_precision=coCfg->mpeg2_dc_prec;

 avctx->pix_fmt=PIX_FMT_YUV420P;
 switch (coCfg->codecId)
  {
   case CODEC_ID_HUFFYUV:
    {
     avctx->strict_std_compliance=FF_COMPLIANCE_EXPERIMENTAL;
     avctx->prediction_method=coCfg->huffyuv_pred;
     switch (coCfg->huffyuv_csp)
      {
       case 0:avctx->pix_fmt=PIX_FMT_YUV422P;break;
       case 1:avctx->pix_fmt=PIX_FMT_YUV420P;break;
      }
     avctx->context_model=coCfg->huffyuv_ctx;
     break;
    }
   case CODEC_ID_FFV1:
    {
     avctx->strict_std_compliance=FF_COMPLIANCE_EXPERIMENTAL;
     avctx->coder_type=coCfg->ffv1_coder;
     avctx->context_model=coCfg->ffv1_context;
     switch (coCfg->ffv1_csp)
      {
       case FOURCC_YV12:avctx->pix_fmt=PIX_FMT_YUV420P;break;
       case FOURCC_444P:avctx->pix_fmt=PIX_FMT_YUV444P;break;
       case FOURCC_422P:avctx->pix_fmt=PIX_FMT_YUV422P;break;
       case FOURCC_411P:avctx->pix_fmt=PIX_FMT_YUV411P;break;
       case FOURCC_410P:avctx->pix_fmt=PIX_FMT_YUV410P;break;
       case FOURCC_RGB3:avctx->pix_fmt=PIX_FMT_RGB32;break;
      }
     break;
    }
   case CODEC_ID_MJPEG:
    avctx->pix_fmt=PIX_FMT_YUVJ420P;
    break;
   case CODEC_ID_LJPEG:
    {
     switch (coCfg->ljpeg_csp)
      {
       case FOURCC_YV12:avctx->pix_fmt=PIX_FMT_YUVJ420P;break;
       case FOURCC_RGB3:avctx->pix_fmt=PIX_FMT_RGB32;break;
      }
     break;
    }
   case CODEC_ID_DVVIDEO:
    {
     PixelFormat lavc_csp=csp_ffdshow2lavc(csp);
     if (!coCfg->getDVprofile(avctx->width,avctx->height,lavc_csp))
      {
       deci->dbgError(_l("Can't find matching DV profile\r\nDV supports 720x480 and 720x576."));
       return ICERR_ERROR;
      }
     else
      avctx->pix_fmt=lavc_csp;
     break;
    }
   case CODEC_ID_H261:
    {
     if ((avctx->width==176 && avctx->height==144) || (avctx->width==352 && avctx->height==288))
      break;
     else
      {
       int errorbox;
       deci->getParam(IDFF_errorbox,&errorbox);
       if (errorbox)
        MessageBox(NULL,_l("h.261 supports 176x144 and 352x288"),_l("ffdshow libavcodec encoder error"),MB_ICONERROR|MB_OK);
       return ICERR_ERROR;
      }
    }
   case CODEC_ID_SVQ1:
    avctx->pix_fmt=PIX_FMT_YUV410P;break;
    break;
   case CODEC_ID_H263P:
    avctx->flags|=coCfg->H263Pflags;
    break;
   case CODEC_ID_H263:
    avctx->flags|=coCfg->H263Pflags&CODEC_FLAG_OBMC;
    break;
   case CODEC_ID_MPEG4:
    avctx->flags|=coCfg->H263Pflags&CODEC_FLAG_LOOP_FILTER;
    break;
   case CODEC_ID_MPEG2VIDEO:
    if (coCfg->svcd_scan_offset) avctx->flags|=CODEC_FLAG_SVCD_SCAN_OFFSET;
    break;
   case CODEC_ID_SNOW:
    avctx->strict_std_compliance=FF_COMPLIANCE_EXPERIMENTAL;
    avctx->prediction_method=1;//coCfg->huffyuv_pred;
    if (coCfg->me_iterative)
     avctx->me_method=ME_ITER;
    break;
  }
 if (avctx->flags&CODEC_FLAG_OBMC && coCfg->me_hq!=FF_MB_DECISION_SIMPLE) avctx->flags&=~CODEC_FLAG_OBMC;

 switch (cfgcomode)
  {
   case ENC_MODE::CBR:
    avctx->bit_rate=coCfg->bitrate1000*1000;
    avctx->bit_rate_tolerance=coCfg->ff1_vratetol*8*1000;
    avctx->qcompress=coCfg->ff1_vqcomp/100.0f;
    avctx->qblur=(coCfg->ff1_stats_mode&FFSTATS::WRITE?coCfg->ff1_vqblur2:coCfg->ff1_vqblur1)/100.0f;
    avctx->max_qdiff=coCfg->ff1_vqdiff;
    avctx->rc_qsquish=coCfg->ff1_rc_squish?1.0f:0.0f;
    avctx->rc_max_rate   =coCfg->ff1_rc_max_rate1000*1000;
    avctx->rc_min_rate   =coCfg->ff1_rc_min_rate1000*1000;
    avctx->rc_buffer_size=coCfg->ff1_rc_buffer_size;
    avctx->rc_initial_buffer_occupancy=avctx->rc_buffer_size*3/4;
    avctx->rc_buffer_aggressivity=1.0f;
    break;
   case ENC_MODE::VBR_QUAL:
   case ENC_MODE::VBR_QUANT:
    avctx->bit_rate=400000;
    break;
   case ENC_MODE::PASS2_1:
    avctx->flags|=CODEC_FLAG_QSCALE;
    break;
   case ENC_MODE::PASS2_2_EXT:
   case ENC_MODE::PASS2_2_INT:
    avctx->flags|=CODEC_FLAG_QSCALE;
    break;
   case ENC_MODE::UNKNOWN:
    break;
   default:return ICERR_ERROR;
  }

 if (sup_LAVC2PASS(coCfg->codecId))
  {
   if (coCfg->ff1_stats_mode&FFSTATS::READ && cfgcomode==ENC_MODE::CBR)
    {
     statsfile=fopen(coCfg->ff1_stats_flnm,_l("rb"));
     if (statsfile)
      {
       avctx->flags|=CODEC_FLAG_PASS2;
       int statslen=_filelength(fileno(statsfile));
       avctx->stats_in=(char*)malloc(statslen+1);
       avctx->stats_in[statslen]='\0';
       fread(avctx->stats_in,1,statslen,statsfile);
       fclose(statsfile);statsfile=NULL;
      }
    }
   if (coCfg->ff1_stats_mode&FFSTATS::WRITE)
    {
     statsfile=fopen(coCfg->ff1_stats_flnm,_l("wt"));
     if (statsfile)
      avctx->flags|=CODEC_FLAG_PASS1;
    }
  }

 if (sup_quantProps(coCfg->codecId) && coCfg->is_lavc_nr) avctx->noise_reduction=coCfg->lavc_nr;

 avctx->sample_aspect_ratio=coCfg->sar(avctx->width,avctx->height).reduce(255);

 if (sup_bframes(coCfg->codecId) && coCfg->isBframes)
  {
   avctx->max_b_frames=coCfg->max_b_frames;
   avctx->b_quant_factor=coCfg->b_quant_factor/100.0f;
   avctx->b_quant_offset=coCfg->b_quant_offset/100.0f;
   avctx->b_frame_strategy=coCfg->b_dynamic && !(avctx->flags&CODEC_FLAG_PASS2)?1:0;
   avctx->bidir_refine=coCfg->b_refine;
   if (coCfg->dx50bvop && sup_closedGop(coCfg->codecId))
    {
     avctx->flags|=CODEC_FLAG_CLOSED_GOP;
     avctx->scenechange_threshold=1000000000;
    }
  }

 if (sup_masking(coCfg->codecId))
  {
   if (coCfg->is_ff_lumi_masking) avctx->lumi_masking=coCfg->ff_lumi_masking1000/1000.0f;
   if (coCfg->is_ff_temporal_cplx_masking) avctx->temporal_cplx_masking=coCfg->ff_temporal_cplx_masking1000/1000.0f;
   if (coCfg->is_ff_spatial_cplx_masking) avctx->spatial_cplx_masking=coCfg->ff_spatial_cplx_masking1000/1000.0f;
   if (coCfg->is_ff_p_masking) avctx->p_masking=coCfg->ff_p_masking1000/1000.0f;
   if (coCfg->is_ff_dark_masking) avctx->dark_masking=coCfg->ff_dark_masking1000/1000.0f;
   if (coCfg->is_ff_border_masking) avctx->border_masking=coCfg->ff_border_masking1000/1000.0f;
   if (coCfg->ff_naq) avctx->flags|=CODEC_FLAG_NORMALIZE_AQP;
   if (coCfg->isElimLum) avctx->luma_elim_threshold=coCfg->elimLumThres;
   if (coCfg->isElimChrom && !coCfg->gray) avctx->chroma_elim_threshold=coCfg->elimChromThres;
  }
 isAdaptive=coCfg->isLAVCadaptiveQuant();
 if (isAdaptive)
  {
   avctx->lmin=coCfg->q_mb_min*FF_QP2LAMBDA;
   avctx->lmax=coCfg->q_mb_max*FF_QP2LAMBDA;
  }
 else
  {
   avctx->lmin=coCfg->getMinMaxQuant().first *FF_QP2LAMBDA;
   avctx->lmax=coCfg->getMinMaxQuant().second*FF_QP2LAMBDA;
  }

 RcOverride rces[2];int rcescount=0;
 if (coCfg->isCreditsStart && cfgcomode==ENC_MODE::CBR && coCfg->ff1_stats_mode&FFSTATS::READ)
  {
   RcOverride &rce=rces[rcescount];
   rce.start_frame=coCfg->creditsStartBegin;
   rce.end_frame=coCfg->creditsStartEnd;
   switch (coCfg->credits_mode)
    {
     case CREDITS_MODE::QUANT:rce.qscale=coCfg->limitq(coCfg->credits_quant_i);rce.quality_factor=1;break;
     case CREDITS_MODE::PERCENT:rce.qscale=0;rce.quality_factor=coCfg->credits_percent/100.0f;break;
    }
   rcescount++;
  }
 if (coCfg->isCreditsEnd && cfgcomode==ENC_MODE::CBR && coCfg->ff1_stats_mode&FFSTATS::READ)
  {
   RcOverride &rce=rces[rcescount];
   rce.start_frame=coCfg->creditsEndBegin;
   rce.end_frame=coCfg->creditsEndEnd;
   switch (coCfg->credits_mode)
    {
     case CREDITS_MODE::QUANT:rce.qscale=coCfg->limitq(coCfg->credits_quant_i);rce.quality_factor=1;break;
     case CREDITS_MODE::PERCENT:rce.qscale=0;rce.quality_factor=coCfg->credits_percent/100.0f;break;
    }
   rcescount++;
  }
 if ((avctx->rc_override_count=rcescount)!=0)
  avctx->rc_override=rces;

 // save av_log_callback and set custom av_log that shows message box.
 void (*avlogOldFunc)(AVCodecContext*, int, const char*, va_list);
 int errorbox;
 deci->getParam(IDFF_errorbox,&errorbox);
 if (errorbox)
  {
   avlogOldFunc=(void (*)(AVCodecContext*, int, const char*, va_list))(libavcodec->av_log_get_callback());
   libavcodec->av_log_set_callback(Tlibavcodec::avlogMsgBox);
  }

 int err=libavcodec->avcodec_open(avctx,avcodec);

 // restore av_log_callback
 if (errorbox)
  libavcodec->av_log_set_callback(avlogOldFunc);
 if (err<0)
  {
   avctx->codec=NULL;
   return ICERR_ERROR;
  }
 if (avctx->stats_in) {free(avctx->stats_in);avctx->stats_in=NULL;}
 codecinited=true;
 return ICERR_OK;
}

HRESULT TvideoCodecLibavcodec::compress(const TffPict &pict,TencFrameParams &params)
{
 if (coCfg->mode==ENC_MODE::VBR_QUAL)
  {
   frame->quality=(100-coCfg->qual)*40;
   avctx->flags|=CODEC_FLAG_QSCALE;
  }
 else
  if (params.quant==-1)
   avctx->flags&=~CODEC_FLAG_QSCALE;
  else
   {
    avctx->flags|=CODEC_FLAG_QSCALE;
    frame->quality=coCfg->limitq(params.quant)*FF_QP2LAMBDA;
    avctx->qmin_i=avctx->qmax_i=0;
    avctx->qmin_b=avctx->qmax_b=0;
   }

 if (isAdaptive)
  if (!coCfg->isInCredits(params.framenum))
   {
    avctx->lmin=coCfg->q_mb_min*FF_QP2LAMBDA;
    avctx->lmax=coCfg->q_mb_max*FF_QP2LAMBDA;
   }
  else
   {
    avctx->lmin=coCfg->getMinMaxQuant().first *FF_QP2LAMBDA;
    avctx->lmax=coCfg->getMinMaxQuant().second*FF_QP2LAMBDA;
   }

 frame->top_field_first=(avctx->flags&CODEC_FLAG_INTERLACED_DCT && coCfg->interlacing_tff)?1:0;

 switch (params.frametype)
  {
   case FRAME_TYPE::I:frame->pict_type=FF_I_TYPE;break;
   case FRAME_TYPE::P:frame->pict_type=FF_P_TYPE;break;
   case FRAME_TYPE::B:frame->pict_type=FF_B_TYPE;break;
   default:frame->pict_type=0;break;
  }
 bool flushing=!pict.data[0];
 if (!flushing)
  for (int i=0;i<4;i++)
   {
    frame->data[i]=(uint8_t*)pict.data[i];
    frame->linesize[i]=(int)pict.stride[i];
   }
 HRESULT hr=S_OK;
 while (frame->data[0] || flushing)
  {
   frame->pts=AV_NOPTS_VALUE;
   TmediaSample sample;
   if (FAILED(hr=sinkE->getDstBuffer(&sample,pict)))
    return hr;
   params.length=libavcodec->avcodec_encode_video(avctx,sample,sample.size(),!flushing?frame:NULL);
   if ((int)params.length<0)
    return sinkE->deliverError();
   else if (params.length==0 && flushing)
    break;
   if (coCfg->ff1_stats_mode&FFSTATS::WRITE && statsfile)
    fprintf(statsfile,"%s",avctx->stats_out);

   if (/*!isAdaptive || */!avctx->coded_frame->qscale_table)
    params.quant=int(avctx->coded_frame->quality/FF_QP2LAMBDA+0.5);
   else
    {
     unsigned int sum=0;
     for (unsigned int y=0;y<mb_height;y++)
      for (unsigned int x=0;x<mb_width;x++)
       sum+=avctx->coded_frame->qscale_table[x+y*avctx->coded_frame->qstride];
     params.quant=roundDiv(sum,mb_count);
    }

   params.kblks=avctx->i_count;
   params.mblks=avctx->p_count;
   params.ublks=avctx->skip_count;

   switch (avctx->coded_frame->pict_type)
    {
     case FF_I_TYPE:params.frametype=FRAME_TYPE::I;break;
     case FF_P_TYPE:params.frametype=FRAME_TYPE::P;break;
     case FF_B_TYPE:params.frametype=FRAME_TYPE::B;break;
     case FF_S_TYPE:params.frametype=FRAME_TYPE::GMC;break;
     case FF_SI_TYPE:params.frametype=FRAME_TYPE::SI;break;
     case FF_SP_TYPE:params.frametype=FRAME_TYPE::SP;break;
     case 0:params.frametype=FRAME_TYPE::DELAY;break;
    }
   params.keyframe=!!avctx->coded_frame->key_frame;

   if (psnr)
    {
     params.psnrY=avctx->coded_frame->error[0];
     params.psnrU=avctx->coded_frame->error[1];
     params.psnrV=avctx->coded_frame->error[2];
    }
   if (FAILED(hr=sinkE->deliverEncodedSample(sample,params)))
    return hr;
   frame->data[0]=NULL;
  }
 return hr;
}

const char* TvideoCodecLibavcodec::get_current_idct(void)
{
 if (avctx && (mpeg12_codec(codecId) || mpeg4_codec(codecId) || x264_codec(codecId) || codecId==CODEC_ID_FLV1))
  return libavcodec->avcodec_get_current_idct(avctx);
 else
  return NULL;
}

TvideoCodecLibavcodec::TcodedPictureBuffer::TcodedPictureBuffer(TvideoCodecLibavcodec* Iparent):
 parent(Iparent)
{
}

void TvideoCodecLibavcodec::TcodedPictureBuffer::init(void)
{
 onSeek();
 priorBuf.alloc(512 * 1024);
 outBuf.alloc(512 * 1024);
}

void TvideoCodecLibavcodec::TcodedPictureBuffer::onSeek(void)
{
 used_bytes = outSampleSize = priorSize = 0;
 prior_rtStart = prior_rtStop = REFTIME_INVALID;
}

/*
 * libavcodec has to receive whole access unit at the same time.
 * Here, we implement coded picture buffer (CPB).
 * This function parses access unit (AU) from the stream and buffers it.
 * We have to memorize the timestamp of the AU. 
 */
int TvideoCodecLibavcodec::TcodedPictureBuffer::append(const uint8_t *buf, int buf_size)
{
 used_bytes = outSampleSize = 0;
 int startCodePos = 0;
 for (; startCodePos < buf_size - 3 ; startCodePos++)
  {
   if (buf[startCodePos] == 0 && buf[startCodePos+1] == 0 && buf[startCodePos+2] == 1 && (buf[startCodePos+3] & 0x1f) == 0x09) // search NAL start code (00 00 01) + AU delimiter
    {
     // send to libavcodec (priorBuf + current sample(just before the start code))
     uint8_t *dstOut = (uint8_t *)outBuf.resize2(priorSize + startCodePos + FF_INPUT_BUFFER_PADDING_SIZE);
     memcpy(dstOut, priorBuf, priorSize);
     memcpy(dstOut + priorSize, buf, startCodePos);
     outSampleSize = priorSize + startCodePos;

     // copy the left of sample to priorBuf
     uint8_t *dstPrior = (uint8_t *)priorBuf.resize2(buf_size - startCodePos);
     const uint8_t *src = buf + startCodePos;
     priorSize = buf_size - startCodePos;
     memcpy(dstPrior, src, priorSize);
     if (priorSize > 3 && prior_rtStart != REFTIME_INVALID)
      {
       out_rtStart = prior_rtStart;
       out_rtStop  = prior_rtStop;
      }
     else
      {
       out_rtStart = parent->rtStart;
       out_rtStop  = parent->rtStop;
      }
     prior_rtStart = parent->rtStart;
     prior_rtStop  = parent->rtStop;
     return outSampleSize;
    }
  }

 // start code not found
 uint8_t *dstPrior = (uint8_t *)priorBuf.resize2(buf_size + priorSize);
 memcpy(dstPrior + priorSize, buf, buf_size);
 priorSize += buf_size;
 prior_rtStart = parent->rtStart;
 prior_rtStop = parent->rtStop;
 return 0;
}

int TvideoCodecLibavcodec::TcodedPictureBuffer::send(int *got_picture_ptr)
{
 if (outSampleSize < 4)
  return -1;

 int out_size = outSampleSize - used_bytes;
 if (parent->h264RandomAccess.search((uint8_t *)outBuf + used_bytes, out_size) == 0)
  return -1;

 parent->avctx->reordered_opaque = out_rtStart;
 parent->avctx->reordered_opaque2 = out_rtStop;
 parent->avctx->reordered_opaque3 = out_size;
 int used = parent->libavcodec->avcodec_decode_video(parent->avctx, parent->frame, got_picture_ptr, (uint8_t *)outBuf + used_bytes, out_size);

 if (used < 0)
  return -1;


 parent->h264RandomAccess.judgeUsability(got_picture_ptr);

 used_bytes += used;
 return used;
}

TvideoCodecLibavcodec::Th264RandomAccess::Th264RandomAccess(TvideoCodecLibavcodec *Iparent):
 parent(Iparent)
{
 onSeek();
}

void TvideoCodecLibavcodec::Th264RandomAccess::onSeek(void)
{
 recovery_mode = 1;
 recovery_frame_cnt = 0;
}

// return 0:not found, don't send it to libavcodec, 1:send it anyway.
int TvideoCodecLibavcodec::Th264RandomAccess::search(uint8_t* buf, int buf_size)
{
 if (parent->codecId == CODEC_ID_H264 && recovery_mode == 1)
  {
   int is_recovery_point = parent->libavcodec->avcodec_h264_search_recovery_point(parent->avctx, buf, buf_size, &recovery_frame_cnt);
   if (is_recovery_point == 3) // IDR
    {
     recovery_mode = 0;
     return 1;
    }
   else if (is_recovery_point == 2) // GDR, recovery_frame_cnt is valid.
    {
     recovery_mode = 2;
     return 1;
    }
   else if (is_recovery_point == 1) // I frames are not ideal for recovery, but if we ignore them, better frames may not come forever. recovery_frame_cnt is not valid.
    {
     recovery_mode = 2;
     recovery_frame_cnt = 0;
     return 1;
    }
   else
    return 0;
  }
 else
  return 1 ;
}

void TvideoCodecLibavcodec::Th264RandomAccess::judgeUsability(int *got_picture_ptr)
{
 if (parent->codecId != CODEC_ID_H264)
  return;

 if (recovery_mode ==1 || recovery_mode ==2)
  {
   recovery_frame_cnt += parent->avctx->h264_frame_num_decoded;
   recovery_mode = 3;
  }

 if (recovery_mode == 3)
  {
   if (recovery_frame_cnt <= parent->avctx->h264_frame_num_decoded)
    {
     recovery_poc = parent->avctx->h264_poc_decoded;
     recovery_mode = 4;
    }
  }

 if (recovery_mode == 4)
  {
   if (parent->avctx->h264_poc_outputed >= recovery_poc)
    {
     recovery_mode = 0;
    }
  }

 if (recovery_mode != 0)
  *got_picture_ptr = 0;
}
