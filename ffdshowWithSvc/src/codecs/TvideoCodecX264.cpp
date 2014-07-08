/*
 * Copyright (c) 2004-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "TvideoCodecX264.h"
#include "Tdll.h"
#include "ffcodecs.h"
#include "TcodecSettings.h"
#include "x264/x264.h"
#define X264_BFRAME_MAX 16
#include "x264/common/frame.h"
#include "libavcodec/avcodec.h"
#include "IffdshowBase.h"

const char_t* TvideoCodecX264::dllname=_l("ff_x264.dll");

TvideoCodecX264::TvideoCodecX264(IffdshowBase *Ideci,IencVideoSink *Isink):
 Tcodec(Ideci),
 TvideoCodecEnc(Ideci,Isink),
 TvideoCodec(Ideci)
{
 ok=false;
 dll=new Tdll(dllname,config);
 dll->loadFunction(x264_encoder_open   ,"x264_encoder_open"   );
 dll->loadFunction(x264_encoder_headers,"x264_encoder_headers");
 dll->loadFunction(x264_encoder_encode ,"x264_encoder_encode" );
 dll->loadFunction(x264_encoder_close  ,"x264_encoder_close"  );
 dll->loadFunction(x264_param_default  ,"x264_param_default"  );
 dll->loadFunction(x264_picture_alloc  ,"x264_picture_alloc"  );
 dll->loadFunction(x264_picture_clean  ,"x264_picture_clean"  );
 dll->loadFunction(x264_nal_encode     ,"x264_nal_encode"     );
 ok=dll->ok;
 if (ok)
  {
   encoders.push_back(new Tencoder(_l("H.264"),CODEC_ID_X264));
   encoders.push_back(new Tencoder(_l("H.264 lossless"),CODEC_ID_X264_LOSSLESS));
  }
 h=NULL;lossless=false;
}
TvideoCodecX264::~TvideoCodecX264()
{
 end();
 if (dll) delete dll;
}

const char_t* TvideoCodecX264::getName(void) const
{
 tsnprintf_s(codecName, countof(codecName), _TRUNCATE, _l("x264%s"), lossless ? _l(" lossless") : _l(""));
 return codecName;
}

LRESULT __declspec(align(16))(TvideoCodecX264::beginCompress(int cfgcomode,int csp,const Trect &r))
{
 x264_param_t param;
 x264_param_default(&param);

 param.cpu=0;
 if (Tconfig::cpu_flags&FF_CPU_MMX     ) param.cpu|=X264_CPU_MMX;
 if (Tconfig::cpu_flags&FF_CPU_MMXEXT  ) param.cpu|=X264_CPU_MMXEXT;
 if (Tconfig::cpu_flags&FF_CPU_SSE     ) param.cpu|=X264_CPU_SSE;
 if (Tconfig::cpu_flags&FF_CPU_SSE2    ) param.cpu|=X264_CPU_SSE2;
 if (Tconfig::cpu_flags&FF_CPU_SSE3    ) param.cpu|=X264_CPU_SSE3;
 if (Tconfig::cpu_flags&FF_CPU_SSSE3   ) param.cpu|=X264_CPU_SSSE3;

 /* Video Properties */
 param.i_width =r.dx;
 param.i_height=r.dy;

 /* they will be reduced to be 0 < x <= 65535 and prime */
 Rational sar=coCfg->sar(r.dx,r.dy);
 param.vui.i_sar_width=sar.num;
 param.vui.i_sar_height=sar.den;

 /* Used for rate control only */
 param.i_fps_num=deci->getParam2(IDFF_enc_fpsRate);param.i_fps_den=deci->getParam2(IDFF_enc_fpsScale);

 // Maximum number of reference frames
 param.i_frame_reference=coCfg->x264_max_ref_frames;

 // every i_iframe are intra
 param.i_keyint_max=coCfg->max_key_interval;
 param.i_keyint_min=coCfg->min_key_interval;

 if (sup_bframes(coCfg->codecId) && coCfg->isBframes)
  {
   // how many b-frame between 2 references pictures
   param.i_bframe=coCfg->max_b_frames;
   if (coCfg->max_b_frames>1)
   param.analyse.b_weighted_bipred=1;
   param.analyse.i_direct_mv_pred=coCfg->x264_i_direct_mv_pred;
   param.analyse.i_direct_8x8_inference=-1;
   param.i_bframe_adaptive=coCfg->b_dynamic;
   param.b_bframe_pyramid=coCfg->x264_b_bframe_pyramid;
  }

 param.b_deblocking_filter=coCfg->codecId!=CODEC_ID_X264_LOSSLESS && (coCfg->H263Pflags&CODEC_FLAG_LOOP_FILTER)?1:0;
 param.i_deblocking_filter_alphac0=coCfg->x264_i_deblocking_filter_alphac0;
 param.i_deblocking_filter_beta=coCfg->x264_i_deblocking_filter_beta;
 param.b_cabac=coCfg->x264_cabac;
 param.b_interlaced=coCfg->x264_interlaced;

 param.analyse.inter=coCfg->x264_me_inter;
 param.analyse.intra=coCfg->x264_me_intra;
 if ((param.analyse.inter|param.analyse.intra)&X264_ANALYSE_I8x8)
  param.analyse.b_transform_8x8=1;
 param.analyse.i_subpel_refine=coCfg->x264_me_subpelrefine;
 param.analyse.i_mv_range=coCfg->x264_mv_range;
 param.analyse.i_mv_range_thread=-1;
 param.analyse.i_me_method=coCfg->x264_me_method;
 if (coCfg->x264_me_method>=2)
  param.analyse.i_me_range=coCfg->x264_me_range;
 param.analyse.b_chroma_me=coCfg->me_cmp_chroma;
 param.analyse.b_mixed_references=coCfg->x264_mixed_ref;
 param.analyse.b_dct_decimate=coCfg->x264_b_dct_decimate;

 param.i_threads=coCfg->numthreads;
 param.b_deterministic=0;

 if (coCfg->codecId==CODEC_ID_X264_LOSSLESS)
  {
   lossless=true;
   param.rc.i_rc_method=X264_RC_CQP;
   param.rc.i_qp_constant=0;
  }
 else
  {
   if (coCfg->mode==ENC_MODE::CBR || coCfg->mode==ENC_MODE::VBR_QUAL)
    {
     param.rc.i_rc_method=X264_RC_ABR;
     param.rc.i_bitrate=coCfg->bitrate1000;
     param.rc.i_qp_min=coCfg->limitq(coCfg->q_p_min);
     param.rc.i_qp_max=coCfg->limitq(coCfg->q_p_max);
     param.rc.f_qcompress=coCfg->ff1_vqcomp/100.0f;
     param.rc.f_qblur=(coCfg->ff1_stats_mode&FFSTATS::WRITE?coCfg->ff1_vqblur2:coCfg->ff1_vqblur1)/100.0f;
     param.rc.i_qp_constant=0;
     if (coCfg->mode==ENC_MODE::VBR_QUAL)
     param.rc.i_rc_method=X264_RC_CRF;
      param.rc.f_rf_constant=float(50*(100-coCfg->qual)/100+1);
      param.rc.i_aq_mode=X264_AQ_VARIANCE;
     param.rc.f_aq_strength=coCfg->x264_aq_strength100/100.0f;
    }
   else
    {
     param.rc.i_rc_method=X264_RC_CQP;
     param.rc.i_qp_constant=coCfg->limitq(coCfg->quant);
     param.rc.f_ip_factor=1.0f;
     param.rc.f_pb_factor=1.0f;
    }
   param.analyse.i_trellis=coCfg->trellisquant;
   if (coCfg->is_lavc_nr) param.analyse.i_noise_reduction=coCfg->lavc_nr;
   if (coCfg->quant_type==5)
    param.i_cqm_preset=X264_CQM_JVT;
   else if (coCfg->quant_type==4)
    {
     param.i_cqm_preset=X264_CQM_CUSTOM;
     memcpy(param.cqm_4iy,&coCfg->qmatrix_intra4x4Y_custom0,16);
     memcpy(param.cqm_4py,&coCfg->qmatrix_inter4x4Y_custom0,16);
     memcpy(param.cqm_4ic,&coCfg->qmatrix_intra4x4C_custom0,16);
     memcpy(param.cqm_4pc,&coCfg->qmatrix_inter4x4C_custom0,16);
     memcpy(param.cqm_8iy,&coCfg->qmatrix_intra_custom0,64);
     memcpy(param.cqm_8py,&coCfg->qmatrix_inter_custom0,64);
    }
  }

 param.analyse.b_psnr=deci->getParam2(IDFF_enc_psnr);
 param.analyse.b_ssim=0;
 if (coCfg->ff1_stats_mode&FFSTATS::READ && cfgcomode==ENC_MODE::CBR)
  {
   param.rc.b_stat_read=1;
   param.rc.psz_stat_in=(char*)coCfg->ff1_stats_flnm;
   if (coCfg->ff1_stats_mode==FFSTATS::RW)
    param.rc.b_stat_write=1;
  }
 if (coCfg->ff1_stats_mode&FFSTATS::WRITE)
  {
   param.rc.b_stat_write=1;
   param.rc.psz_stat_out=(char*)coCfg->ff1_stats_flnm;
  }
 param.i_log_level=X264_LOG_NONE;
 param.b_aud=coCfg->x264_b_aud;

 x264_zone_t zones[2];int zonescount=0;
 if (coCfg->isCreditsStart && cfgcomode==ENC_MODE::CBR)
  {
   x264_zone_t &zone=zones[zonescount];
   zone.i_start=coCfg->creditsStartBegin;
   zone.i_end=coCfg->creditsStartEnd;
   switch (coCfg->credits_mode)
    {
     case CREDITS_MODE::QUANT:zone.i_qp=coCfg->limitq(coCfg->credits_quant_i);zone.b_force_qp=1;break;
     case CREDITS_MODE::PERCENT:zone.b_force_qp=0;zone.f_bitrate_factor=coCfg->credits_percent/100.0f;break;
    }
   zonescount++;
  }
 if (coCfg->isCreditsEnd && cfgcomode==ENC_MODE::CBR)
  {
   x264_zone_t &zone=zones[zonescount];
   zone.i_start=coCfg->creditsEndBegin;
   zone.i_end=coCfg->creditsEndEnd;
   switch (coCfg->credits_mode)
    {
     case CREDITS_MODE::QUANT:zone.i_qp=coCfg->limitq(coCfg->credits_quant_i);zone.b_force_qp=1;break;
     case CREDITS_MODE::PERCENT:zone.b_force_qp=0;zone.f_bitrate_factor=coCfg->credits_percent/100.0f;break;
    }
   zonescount++;
  }
 if ((param.rc.i_zones=zonescount)!=0)
  param.rc.zones=zones;

 h=x264_encoder_open(&param);
 if (!h) return ICERR_ERROR;
 return ICERR_OK;
}
void TvideoCodecX264::fill_x264_pict(x264_frame_t *out,const TffPict *in)
{
 TffPict::copy(out->plane[0],out->i_stride[0],in->data[0],in->stride[0],in->rectFull.dx  ,in->rectFull.dy  );
 TffPict::copy(out->plane[1],out->i_stride[1],in->data[1],in->stride[1],in->rectFull.dx/2,in->rectFull.dy/2);
 TffPict::copy(out->plane[2],out->i_stride[2],in->data[2],in->stride[2],in->rectFull.dx/2,in->rectFull.dy/2);

 out->i_type=X264_TYPE_AUTO;
 out->i_qpplus1=0;
}
HRESULT __declspec(align(16))(TvideoCodecX264::compress(const TffPict &pict,TencFrameParams &params))
{
 int i_nal;
 x264_nal_t *nal;
 x264_picture_t pic_out;
again:
 if (x264_encoder_encode(h,&nal,&i_nal,pict.data[0]?(void*)fill_x264_pict:NULL,&pict,&pic_out)<0)
  return sinkE->deliverError();
 if (!pict.data[0] && i_nal==0)
  return S_OK;
 params.length=0;

 TmediaSample sample;
 HRESULT hr;
 if (FAILED(hr=sinkE->getDstBuffer(&sample,pict)))
  return hr;
 uint8_t *dst=sample;size_t dstLen=sample.size();
 for(int i=0;i<i_nal;i++)
  {
   int i_data=(int)dstLen,i_size;
   if ((i_size=x264_nal_encode(dst,&i_data,1,&nal[i]))>0)
    {
     dst+=i_size;
     dstLen-=i_size;
     params.length+=i_size;;
    }
   else if(i_size<0)
    return sinkE->deliverError();
  }
 switch (pic_out.i_type)
  {
   case X264_TYPE_IDR:
   case X264_TYPE_I:
    params.frametype=FRAME_TYPE::I;
    params.keyframe=pic_out.i_type==X264_TYPE_IDR || (pic_out.i_type==X264_TYPE_I && coCfg->x264_max_ref_frames==1);
    break;
   case X264_TYPE_P:
    params.frametype=FRAME_TYPE::P;
    break;
   case X264_TYPE_B:
   case X264_TYPE_BREF:
    params.frametype=FRAME_TYPE::B;
    break;
   default:
    params.frametype=FRAME_TYPE::DELAY;
    break;
  }
 if (params.frametype!=FRAME_TYPE::UNKNOWN)
  {
   if (coCfg->mode==ENC_MODE::CBR || coCfg->mode==ENC_MODE::VBR_QUAL)
    params.quant=pic_out.i_qpplus1;
   params.psnrY=pic_out.i_sqe_y;
   params.psnrU=pic_out.i_sqe_u;
   params.psnrV=pic_out.i_sqe_v;
  }
 if (FAILED(hr=sinkE->deliverEncodedSample(sample,params)))
  return hr;
 if (!pict.data[0])
  goto again;
 return hr;
}

void TvideoCodecX264::end(void)
{
 if (ok)
  {
   if (h) x264_encoder_close(h);h=NULL;
  }
 lossless=false;
}
