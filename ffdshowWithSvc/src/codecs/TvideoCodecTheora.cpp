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
#include "Tdll.h"
#include "TvideoCodecTheora.h"
#include "IffdshowBase.h"
#include "IffdshowEnc.h"
#include "TcodecSettings.h"
#include "simd.h"
#include "ffdebug.h"
#include "dsutil.h"
#include "theora/cpu.h"
#include "ffdshow_mediaguids.h"

const char_t* TvideoCodecTheora::dllname=_l("ff_theora.dll");

TvideoCodecTheora::TvideoCodecTheora(IffdshowBase *Ideci,IdecVideoSink *IsinkD):
 Tcodec(Ideci),TcodecDec(Ideci,IsinkD),
 TvideoCodecDec(Ideci,IsinkD),
 TvideoCodecEnc(Ideci,NULL),
 TvideoCodec(Ideci)
{
 create();
}
TvideoCodecTheora::TvideoCodecTheora(IffdshowBase *Ideci,IencVideoSink *IsinkE):
 Tcodec(Ideci),TcodecDec(Ideci,NULL),
 TvideoCodecDec(Ideci,NULL),
 TvideoCodecEnc(Ideci,IsinkE),
 TvideoCodec(Ideci)
{
 create();
 if (ok)
  encoders.push_back(new Tencoder(_l("Theora"),CODEC_ID_THEORA_LIB));
}

void TvideoCodecTheora::create(void)
{
 dll=new Tdll(dllname,config);
 dll->loadFunction(theora_info_init,"theora_info_init");
 dll->loadFunction(theora_comment_init,"theora_comment_init");
 dll->loadFunction(theora_decode_init,"theora_decode_init");
 dll->loadFunction(theora_decode_header,"theora_decode_header");
 dll->loadFunction(theora_decode_packetin,"theora_decode_packetin");
 dll->loadFunction(theora_decode_YUVout,"theora_decode_YUVout");
 dll->loadFunction(theora_encode_init,"theora_encode_init");
 dll->loadFunction(theora_encode_header,"theora_encode_header");
 dll->loadFunction(theora_encode_YUVin,"theora_encode_YUVin");
 dll->loadFunction(theora_encode_packetout,"theora_encode_packetout");
 dll->loadFunction(theora_clear,"theora_clear");
 dll->loadFunction(theora_info_clear,"theora_info_clear");
 dll->loadFunction(theora_comment_clear,"theora_comment_clear");
 dll->loadFunction(theora_encode_comment,"theora_encode_comment");
 dll->loadFunction(theora_encode_tables,"theora_encode_tables");
 dll->loadFunction(theora_granule_frame,"theora_granule_frame");
 dll->loadFunction(set_cpu_flags,"set_cpu_flags");
 ok=dll->ok;
 if (ok)
  {
   int cpu=0;
   if (Tconfig::cpu_flags&FF_CPU_MMX   ) cpu|=CPU_X86_MMX;
   if (Tconfig::cpu_flags&FF_CPU_3DNOW ) cpu|=CPU_X86_3DNOW;
   if (Tconfig::cpu_flags&FF_CPU_MMXEXT) cpu|=CPU_X86_MMXEXT;
   if (Tconfig::cpu_flags&FF_CPU_3DNOWEXT) cpu|=CPU_X86_3DNOWEXT;
   if (Tconfig::cpu_flags&FF_CPU_SSE   ) cpu|=CPU_X86_SSE;
   if (Tconfig::cpu_flags&FF_CPU_SSE2  ) cpu|=CPU_X86_SSE2;
   set_cpu_flags(cpu);
  }
 memset(&td,0,sizeof(td));memset(&ti,0,sizeof(ti));memset(&tc,0,sizeof(tc));
 inited=false;
 extradata=NULL;
 segmentTimeStart=0;
}
TvideoCodecTheora::~TvideoCodecTheora()
{
 end();
 if (dll) delete dll;
}
void TvideoCodecTheora::end(void)
{
 if (inited)
  {
   theora_clear(&td);
   theora_info_clear(&ti);// memset(&td,0,sizeof(td));
   tc.vendor=NULL;
   theora_comment_clear(&tc);//memset(&ti,0,sizeof(ti));
   inited=false;
  }
 if (extradata) free(extradata);extradata=NULL;
}

bool TvideoCodecTheora::beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags)
{
 inited=false;theorart=false;
 memset(&ill,0,sizeof(ill));
 if (mt.formattype==FORMAT_RLTheora)
  {
   theorart=true;
  }
 else if (mt.formattype==FORMAT_TheoraIll)
  {
   ill=*(const sTheoraFormatBlock*)mt.Format();
   lastSeenStartGranPos=0;
   frameDuration=(UNITS*ill.frameRateDenominator)/ill.frameRateNumerator;
   frameCount=0;
  }
 rd=pict.rectFull;
 pict.csp=FF_CSP_420P;
 Textradata extradata(mt);
 if (extradata.data && extradata.size>2)
  {
   if (theorart)
    {
     theora_info_init(&ti);
     theora_comment_init(&tc);
     const uint8_t *src=extradata.data;

     ogg_packet op;memset(&op,0,sizeof(op));
     op.b_o_s=1;

     op.packet=(unsigned char*)src+16;
     DWORD len0=*(DWORD*)&src[0*sizeof(DWORD)];
     op.bytes=len0;
     if (theora_decode_header(&ti,&tc,&op)!=0)
      return false;

     op.packet=(unsigned char*)src+16+len0;
     DWORD len1=*(DWORD*)&src[1*sizeof(DWORD)];
     op.bytes=len1;
     if (theora_decode_header(&ti,&tc,&op)!=0)
      return false;

     op.packet=(unsigned char*)src+16+len0+len1;
     DWORD len2=*(DWORD*)&src[2*sizeof(DWORD)];
     op.bytes=len2;
     if (theora_decode_header(&ti,&tc,&op)!=0)
      return false;
    }
   else if (extradata.data[2]==0)
    {
     theora_info_init(&ti);
     ti.width=rd.dx;
     ti.height=rd.dy;
     ti.fps_numerator=25;
     ti.fps_denominator=1;
     ti.aspect_numerator=1;
     ti.aspect_denominator=1;
     ti.version_major=3;
     theora_comment_init(&tc);tc.vendor="ff_theora";
     ogg_packet op;memset(&op,0,sizeof(op));
     op.packet=(unsigned char*)extradata.data;
     op.bytes=(long)extradata.size;
     op.b_o_s=1;
     long len=*(long*)op.packet;op.packet+=4;op.bytes-=4;
     if (theora_decode_header(&ti,&tc,&op)!=0)
      return false;
    }
   else
    {
     const unsigned char *ptr=extradata.data;
     theora_info_init(&ti);
     theora_comment_init(&tc);
     while (ptr<extradata.data+extradata.size)
      {
       int16_t size=*(int16_t*)ptr;ptr+=sizeof(int16_t);
       bswap(size);
       ogg_packet op;memset(&op,0,sizeof(op));
       op.packet=(unsigned char*)ptr;
       op.bytes=size;
       op.b_o_s=1;
       if (theora_decode_header(&ti,&tc,&op)!=0)
        return false;
       ptr+=size;
      }
    }
   ti.postprocessingLevel=deci->getParam2(IDFF_libtheoraPostproc)?6:0;
   if (theora_decode_init(&td,&ti)==0)
    return inited=true;
   else
    return false;
  }
 else
  {
   theora_info_init(&ti);
   theora_comment_init(&tc);
   return true;
  }
}
HRESULT TvideoCodecTheora::decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn)
{
 ogg_packet op;memset(&op,0,sizeof(op));
 if (theorart)
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
   src+=sizeof(_TheoraPacket);srcLen-=sizeof(_TheoraPacket);
   op.granulepos=packet->granulepos;
  }

 if (ill.width)
  {
   REFERENCE_TIME rtStart=0,rtStop=0;
   pIn->GetTime(&rtStart,&rtStop);
   if (rtStart!=-1 && rtStart!=lastSeenStartGranPos)
    lastSeenStartGranPos=rtStart;
  }

 op.packet=(unsigned char*)src;
 op.bytes=(long)srcLen;
 if (op.packet[0]&0x80)
  {
   op.b_o_s=1;
   inited=false;
   int ret=theora_decode_header(&ti,&tc,&op);
   return S_FALSE;
  }

 if (!inited)
  {
   ti.postprocessingLevel=deci->getParam2(IDFF_libtheoraPostproc)?6:0;
   int ret=theora_decode_init(&td,&ti);
   if (ret==0) inited=true;
  }

 if (!inited) return S_FALSE;
 theora_decode_packetin(&td,&op);
 yuv_buffer yuv;memset(&yuv,0,sizeof(yuv));
 int ret=theora_decode_YUVout(&td,&yuv);
 _mm_empty();
 if (ret==0)
  {
   if (pIn->IsPreroll()==S_OK)
    return sinkD->deliverPreroll(yuv.keyframe?FRAME_TYPE::I:FRAME_TYPE::P);
   unsigned char *data[4]={(unsigned char*)yuv.y,(unsigned char*)yuv.u,(unsigned char*)yuv.v,NULL};
   stride_t stride[4]={yuv.y_stride,yuv.uv_stride,yuv.uv_stride,0};
   if (ti.aspect_numerator)
    rd.sar=Rational((int)ti.aspect_numerator,(int)ti.aspect_denominator);
   TffPict pict(FF_CSP_420P,data,stride,rd,true,yuv.keyframe?FRAME_TYPE::I:FRAME_TYPE::P,FIELD_TYPE::PROGRESSIVE_FRAME,srcLen,pIn);
   if (ill.width)
    {
     unsigned long locMod=1<<ill.maxKeyframeInterval;
     unsigned long locInterFrameNo=(unsigned int)(lastSeenStartGranPos%locMod);
     LONGLONG locAbsFramePos=((lastSeenStartGranPos>>ill.maxKeyframeInterval))+locInterFrameNo;
     REFERENCE_TIME locTimeBase=locAbsFramePos*frameDuration/*-mSeekTimeBase*/;
     REFERENCE_TIME locFrameStart=locTimeBase+frameCount*frameDuration;
     frameCount++;
     REFERENCE_TIME locFrameEnd=locTimeBase+frameCount*frameDuration;
     pict.rtStart=locFrameStart;
     pict.rtStop=locFrameEnd;
     pict.mediatimeStart=pict.mediatimeStop=REFTIME_INVALID;
    }
   else if (theorart)
    {
     pict.rtStart=ti.fps_denominator*REF_SECOND_MULT*theora_granule_frame(&td,td.granulepos)/ti.fps_numerator;
     pict.rtStart=pict.rtStart-segmentTimeStart;
     pict.rtStop=pict.rtStart+1;
     pict.mediatimeStart=pict.mediatimeStop=REFTIME_INVALID;
    }
   return sinkD->deliverDecodedSample(pict);
  }
 else
  return S_FALSE;
}

LRESULT TvideoCodecTheora::beginCompress(int cfgcomode,int csp,const Trect &r)
{
 this->cfgcomode=cfgcomode;
 theora_info_init(&ti);
 ti.width=ti.frame_width=r.dx;
 ti.height=ti.frame_height=r.dy;
 ti.fps_numerator=deci->getParam2(IDFF_enc_fpsRate);
 ti.fps_denominator=deci->getParam2(IDFF_enc_fpsScale);
 Rational sar=coCfg->sar(r.dx,r.dy).reduce(10000);
 ti.aspect_numerator=sar.num;
 ti.aspect_denominator=sar.den;
 ti.sharpness=coCfg->theo_sharpness;
 ti.noise_sensitivity=coCfg->theo_noisesensitivity;
 ti.dropframes_p=coCfg->dropFrames;
 ti.quick_p=coCfg->theo_hq?0:1;
 ti.keyframe_auto_p=1;
 ti.keyframe_frequency=coCfg->max_key_interval;
 ti.keyframe_frequency_force=coCfg->max_key_interval;
 ti.keyframe_auto_threshold=coCfg->keySceneChange;
 ti.keyframe_mindistance=coCfg->min_key_interval;

 switch (cfgcomode)
  {
   case ENC_MODE::UNKNOWN:
   case ENC_MODE::CBR:
    ti.target_bitrate=coCfg->bitrate1000*1000;
    ti.keyframe_data_target_bitrate=ogg_uint32_t(ti.target_bitrate*1.5);
    ti.quality=0;
    break;
   case ENC_MODE::VBR_QUAL:
   case ENC_MODE::VBR_QUANT:
   case ENC_MODE::PASS2_1:
   case ENC_MODE::PASS2_2_INT:
   case ENC_MODE::PASS2_2_EXT:
    ti.quality=63*coCfg->qual/100;
    ti.target_bitrate=0;ti.keyframe_data_target_bitrate=0;
    break;
   default:
    return ICERR_ERROR;
  }
 if (theora_encode_init(&td,&ti)==0)
  {
   inited=true;
   firstframe=true;
   ogg_packet op;
   TbyteBuffer buf;
   int16_t sizeb;
   theora_encode_header(&td,&op);deciE->muxHeader(&op,0,false);buf.append(bswap(sizeb=(int16_t)op.bytes));buf.append(op.packet,op.bytes);
   theora_comment_init(&tc);
   theora_encode_comment(&tc,&op);deciE->muxHeader(&op,0,false);buf.append(bswap(sizeb=(int16_t)op.bytes));buf.append(op.packet,op.bytes);
   theora_encode_tables(&td,&op);deciE->muxHeader(&op,0,true);buf.append(bswap(sizeb=(int16_t)op.bytes));buf.append(op.packet,op.bytes);
   if (extradata) free(extradata);
   extradatalen=buf.size();
   extradata=(unsigned char*)malloc(buf.size());
   memcpy(extradata,&buf[0],buf.size());
   return ICERR_OK;
  }
 else
  return ICERR_ERROR;
}

bool TvideoCodecTheora::getExtradata(const void* *ptr,size_t *len)
{
 if (!inited || !len) return false;
 *len=extradatalen;
 if (ptr) *ptr=extradata;
 return true;
}

HRESULT TvideoCodecTheora::compress(const TffPict &pict,TencFrameParams &params)
{
 yuv_buffer yuv;
 yuv.y_width =ti.width      ;yuv.uv_width =yuv.y_width /2;
 yuv.y_stride=(int)pict.stride[0];yuv.uv_stride=(int)pict.stride[1];
 yuv.y_height=ti.height     ;yuv.uv_height=yuv.y_height/2;

 yuv.y=pict.data[0];
 yuv.u=pict.data[1];
 yuv.v=pict.data[2];

 int fixedq=(params.quant!=-1)?int(63-63*pow(double(coCfg->limitq(params.quant)-1),0.2)/pow(29.0,0.2)+0.5):-1; //no math here, just have tried few values
 int keyframe;
 int res=theora_encode_YUVin(&td,&yuv,&fixedq,&keyframe);
 if (res<0)
  {
   params.frametype=FRAME_TYPE::DELAY;
   params.length=0;
   return sinkE->deliverError();
  }

 if (theora_encode_packetout(&td,0,&openc)==0)
  return sinkE->deliverError();
 else
  {
   TmediaSample sample;
   HRESULT hr;
   if (FAILED(hr=sinkE->getDstBuffer(&sample,pict)))
    return hr;
   memcpy(sample,openc.packet,openc.bytes);params.length=openc.bytes;
   params.priv=&openc;
   params.frametype=keyframe?(params.keyframe=true,FRAME_TYPE::I):FRAME_TYPE::P;
   if (params.quant==-1) params.quant=30*(63-fixedq)/63+1;

   return sinkE->deliverEncodedSample(sample,params);
  }
}

bool TvideoCodecTheora::onSeek(REFERENCE_TIME segmentStart)
{
 segmentTimeStart=segmentStart;
 return true;
}
