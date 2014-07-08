/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include "ffdshow_constants.h"
#include "ffdshow_mediaguids.h"
#include "TcodecSettings.h"
#include "reg.h"
#include "TffProc.h"
#include "x264/x264.h"
#include "xvidcore/xvid.h"
#include "ffImgfmt.h"
#include "Tmuxer.h"
#include "libavcodec/avcodec.h"
#include "ffmpeg/libavcodec/dvdata.h"

const char_t* TcoSettings::aspectModes[]=
{
 _l("Pixel aspect ratio"),
 _l("Display aspect ratio"),
 NULL
};

const char_t* TcoSettings::huffYUVcsps[]=
{
 _l("YUY2"),
 _l("YV12"),
 NULL
};
const char_t* TcoSettings::huffYUVpreds[]=
{
 _l("Left"),
 _l("Plane"),
 _l("Median"),
 NULL
};

const TcspFcc TcoSettings::ffv1csps[]=
{
 {_l("YV12") ,FOURCC_YV12},
 {_l("444P") ,FOURCC_444P},
 {_l("422P") ,FOURCC_422P},
 {_l("411P") ,FOURCC_411P},
 {_l("410P") ,FOURCC_410P},
 {_l("RGB32"),FOURCC_RGB3},
 NULL
};
const char_t* TcoSettings::ffv1coders[]=
{
 _l("VLC"),
 _l("AC"),
 NULL
};
const char_t* TcoSettings::ffv1contexts[]=
{
 _l("Small"),
 _l("Large"),
 NULL
};
const char_t* TcoSettings::x264_me_methods[]=
{
 _l("diamond"),
 _l("hexagonal"),
 _l("UMH"),
 _l("ESA"),
 _l("TESA"),
 NULL
};

TcoSettings::TcoSettings(TintStrColl *Icoll):Toptions(Icoll),options(Icoll)
{
 static const TintOptionT<TcoSettings> iopts[]=
  {
   IDFF_numthreads  ,&TcoSettings::numthreads ,1,          8,_l(""),1,
     _l("numthreads"),1,
   IDFF_enc_mode        ,&TcoSettings::mode       ,0,          5,_l(""),1,
     _l("mode"),ENC_MODE::CBR,
   IDFF_enc_bitrate1000 ,&TcoSettings::bitrate1000,1,      10000,_l(""),1,
     _l("bitrate1000"),900,
   IDFF_enc_desiredSize ,&TcoSettings::desiredSize,1,4*1024*1024,_l(""),1,
     _l("desiredSize"),570000,
   IDFF_enc_quant       ,&TcoSettings::quant      ,minQuant,maxQuant,_l(""),1,
     _l("quant"),2,
   IDFF_enc_qual        ,&TcoSettings::qual       ,0,        100,_l(""),1,
     _l("qual"),85,

   IDFF_enc_codecId ,&TcoSettings::codecId         ,1,10000,_l(""),1,
     _l("codecId"),CODEC_ID_MPEG4,
   IDFF_enc_fourcc  ,(TintVal)&TcoSettings::fourcc ,1,    1,_l(""),1,
     _l("fourcc"),FOURCC_XVID,

   IDFF_enc_max_key_interval,&TcoSettings::max_key_interval,0,500,_l(""),1,
     _l("max_key_interval"),250,
   IDFF_enc_min_key_interval,&TcoSettings::min_key_interval,0,500,_l(""),1,
     _l("min_key_interval"),1,
   IDFF_enc_keySceneChange  ,&TcoSettings::keySceneChange  ,0,100,_l(""),1,
     _l("keySceneChange"),80,
   IDFF_enc_globalHeader    ,&TcoSettings::globalHeader    ,0,  0,_l(""),1,
     _l("globalHeader"),0,
   IDFF_enc_part            ,&TcoSettings::part            ,0,  0,_l(""),1,
     _l("part"),0,
   IDFF_enc_interlacing     ,&TcoSettings::interlacing     ,0,  0,_l(""),1,
     _l("interlacing"),0,
   IDFF_enc_interlacing_tff ,&TcoSettings::interlacing_tff ,0,  0,_l(""),1,
     _l("interlacing_tff"),0,
   IDFF_enc_gray            ,&TcoSettings::gray            ,0,  0,_l(""),1,
     _l("gray"),0,
   IDFF_enc_isBframes       ,&TcoSettings::isBframes       ,0,  0,_l(""),1,
     _l("isBframes"),0,
   IDFF_enc_max_b_frames    ,&TcoSettings::max_b_frames    ,1,  8,_l(""),1,
     _l("max_b_frames"),1,
   IDFF_enc_b_dynamic       ,&TcoSettings::b_dynamic       ,0,  0,_l(""),1,
     _l("b_dynamic"),1,
   IDFF_enc_b_refine        ,&TcoSettings::b_refine        ,0,  0,_l(""),1,
     _l("b_refine"),0,
   IDFF_enc_packedBitstream ,&TcoSettings::packedBitstream ,0,  0,_l(""),1,
     _l("packedBitstream"),0,
   IDFF_enc_dx50bvop        ,&TcoSettings::dx50bvop        ,0,  0,_l(""),1,
     _l("dx50bvop"),0,
   IDFF_enc_aspectMode      ,&TcoSettings::aspectMode      ,0,  1,_l(""),1,
     _l("aspectMode"),ASPECT_SAR,
   IDFF_enc_sarX1000        ,&TcoSettings::sarX1000        ,1,255000,_l(""),1,
     _l("sarX1000"),1000,
   IDFF_enc_sarY1000        ,&TcoSettings::sarY1000        ,1,255000,_l(""),1,
     _l("sarY1000"),1000,
   IDFF_enc_darX1000        ,&TcoSettings::darX1000        ,1,255000,_l(""),1,
     _l("darX1000"),4000,
   IDFF_enc_darY1000        ,&TcoSettings::darY1000        ,1,255000,_l(""),1,
     _l("darY1000"),3000,
   IDFF_enc_H263Pflags      ,&TcoSettings::H263Pflags      ,1,  1,_l(""),1,
     _l("H263Pflags"),0,

   IDFF_enc_huffyuv_csp  ,&TcoSettings::huffyuv_csp ,0,1,_l(""),1,
     _l("huffyuv_csp"),1,
   IDFF_enc_huffyuv_pred ,&TcoSettings::huffyuv_pred,0,2,_l(""),1,
     _l("huffyuv_pred"),1,
   IDFF_enc_huffyuv_ctx  ,&TcoSettings::huffyuv_ctx ,0,1,_l(""),1,
     _l("huffyuv_ctx"),0,

   IDFF_enc_ljpeg_csp  ,&TcoSettings::ljpeg_csp ,0,0,_l(""),1,
     _l("ljpeg_csp"),FOURCC_YV12,

   IDFF_enc_ffv1_coder  ,&TcoSettings::ffv1_coder  ,0, 1,_l(""),1,
     _l("ffv1_coder"),0,
   IDFF_enc_ffv1_context,&TcoSettings::ffv1_context,0,10,_l(""),1,
     _l("ffv1_context"),0,
   IDFF_enc_ffv1_key_interval,&TcoSettings::ffv1_key_interval,1,500,_l(""),1,
     _l("ffv1_key_interval"),10,
   IDFF_enc_ffv1_csp    ,&TcoSettings::ffv1_csp    ,1, 1,_l(""),1,
     _l("ffv1_csp"),FOURCC_YV12,

   IDFF_enc_dv_profile  ,&TcoSettings::dv_profile  ,DV_PROFILE_AUTO,countof(dv_profiles)-1,_l(""),1,
     _l("dv_profile"),DV_PROFILE_AUTO,

   IDFF_enc_wmv9_kfsecs,&TcoSettings::wmv9_kfsecs,1, 10,_l(""),1,
     _l("wmv9_kfsecs"),6,
   IDFF_enc_wmv9_ivtc  ,&TcoSettings::wmv9_ivtc  ,0,  0,_l(""),1,
     _l("wmv9_ivtc"),0,
   IDFF_enc_wmv9_deint ,&TcoSettings::wmv9_deint ,0,  0,_l(""),1,
     _l("wmv9_deint"),0,
   IDFF_enc_wmv9_cplx  ,&TcoSettings::wmv9_cplx  ,0, 10,_l(""),1,
     _l("wmv9_cplx"),0,
   IDFF_enc_wmv9_crisp ,&TcoSettings::wmv9_crisp ,0,100,_l(""),1,
     _l("wmv9_crisp"),80,
   IDFF_enc_wmv9_aviout,&TcoSettings::wmv9_aviout,0,  0,_l(""),1,
     _l("wmv9_aviout"),1,

   IDFF_enc_forceIncsp,&TcoSettings::forceIncsp ,0,0,_l(""),1,
     _l("forceIncsp"),0,
   IDFF_enc_incsp     ,&TcoSettings::incspFourcc,1,1,_l(""),1,
     _l("incsp"),FOURCC_YV12,
   IDFF_enc_isProc    ,&TcoSettings::isProc     ,0,0,_l(""),1,
     _l("isProc"),0,
   IDFF_enc_flip      ,&TcoSettings::flip       ,0,0,_l(""),1,
     _l("flip"),0,
   IDFF_enc_dropFrames,&TcoSettings::dropFrames ,0,0,_l(""),1,
     _l("dropFrames"),0,

   IDFF_enc_storeAVI       ,&TcoSettings::storeAVI       ,0,0,_l(""),1,
     _l("storeAVI"),1,
   IDFF_enc_storeExt       ,&TcoSettings::storeExt       ,0,0,_l(""),1,
     _l("storeExt"),0,
   IDFF_enc_muxer          ,&TcoSettings::muxer          ,0,4,_l(""),1,
     _l("muxer"),Tmuxer::MUXER_FILE,
   IDFF_enc_ff1_stats_mode ,&TcoSettings::ff1_stats_mode ,0,3,_l(""),1,
     _l("ff1_stats_mode"),0,
   IDFF_enc_isFPSoverride  ,&TcoSettings::isFPSoverride  ,0,0,_l(""),1,
     _l("isFPSoverride"),0,
   IDFF_enc_fpsOverrideNum ,&TcoSettings::fpsOverrideNum,1,200*1000,_l(""),1,
     _l("fpsOverride1000"),25000,
   IDFF_enc_fpsOverrideDen ,&TcoSettings::fpsOverrideDen,1,200*1000,_l(""),1,
     _l("fpsOverrideDen"),1000,

   IDFF_enc_me_hq                  ,&TcoSettings::me_hq                  ,0,2,_l(""),1,
     _l("me_hq"),1,
   IDFF_enc_me_4mv                 ,&TcoSettings::me_4mv                 ,0,0,_l(""),1,
     _l("me_4mv"),1,
   IDFF_enc_me_qpel                ,&TcoSettings::me_qpel                ,0,0,_l(""),1,
     _l("me_qpel"),0,
   IDFF_enc_me_gmc                 ,&TcoSettings::me_gmc                 ,0,0,_l(""),1,
     _l("me_gmc"),0,
   IDFF_enc_me_mv0                 ,&TcoSettings::me_mv0                 ,0,0,_l(""),1,
     _l("me_mv0"),0,
   IDFF_enc_me_cbp_rd              ,&TcoSettings::me_cbp_rd              ,0,0,_l(""),1,
     _l("me_cbp_rd"),0,
   IDFF_enc_me_cmp                 ,&TcoSettings::me_cmp                 ,0,14,_l(""),1,
     _l("me_cmp"),0,
   IDFF_enc_me_cmp_chroma          ,&TcoSettings::me_cmp_chroma          ,0,0,_l(""),1,
     _l("me_cmp_chroma"),1,
   IDFF_enc_me_subcmp              ,&TcoSettings::me_subcmp              ,0,14,_l(""),1,
     _l("me_subcmp"),0,
   IDFF_enc_me_subcmp_chroma       ,&TcoSettings::me_subcmp_chroma       ,0,0,_l(""),1,
     _l("me_subcmp_chroma"),0,
   IDFF_enc_mb_cmp                 ,&TcoSettings::mb_cmp                 ,0,14,_l(""),1,
     _l("mb_cmp"),0,
   IDFF_enc_mb_cmp_chroma          ,&TcoSettings::mb_cmp_chroma          ,0,0,_l(""),1,
     _l("mb_cmp_chroma"),0,
   IDFF_enc_dia_size               ,&TcoSettings::dia_size               ,0,9,_l(""),1,
     _l("dia_size"),4,
   IDFF_enc_me_last_predictor_count,&TcoSettings::me_last_predictor_count,0,99,_l(""),1,
     _l("me_last_predictor_count"),0,
   IDFF_enc_me_prepass             ,&TcoSettings::me_prepass             ,0,2,_l(""),1,
     _l("me_prepass"),1,
   IDFF_enc_me_precmp              ,&TcoSettings::me_precmp              ,0,14,_l(""),1,
     _l("me_precmp"),0,
   IDFF_enc_me_precmp_chroma       ,&TcoSettings::me_precmp_chroma       ,0,0,_l(""),1,
     _l("me_precmp_chroma"),0,
   IDFF_enc_dia_size_pre           ,&TcoSettings::dia_size_pre           ,0,9,_l(""),1,
     _l("dia_size_pre"),4,
   IDFF_enc_me_subq                ,&TcoSettings::me_subq                ,1,8,_l(""),1,
     _l("me_subq"),8,
   IDFF_enc_me_nsse_weight         ,&TcoSettings::me_nsse_weight         ,0,256,_l(""),1,
     _l("me_nsse_weight"),8,
   IDFF_enc_me_iterative           ,&TcoSettings::me_iterative           ,0,0,_l(""),1,
     _l("me_iterative"),0,

   IDFF_enc_xvid_motion_search       ,&TcoSettings::xvid_motion_search       ,0,6,_l(""),1,
     _l("xvid_motion_search"),5,
   IDFF_enc_is_xvid_me_custom        ,&TcoSettings::is_xvid_me_custom        ,0,0,_l(""),1,
     _l("is_xvid_me_custom"),0,
   IDFF_enc_xvid_me_custom           ,&TcoSettings::xvid_me_custom           ,1,1,_l(""),1,
     _l("xvid_me_custom"),XVID_ME::HALFPELREFINE16|XVID_ME::EXTSEARCH16|XVID_ME::HALFPELREFINE8|XVID_ME::USESQUARES16,
   IDFF_enc_xvid_me_inter4v          ,&TcoSettings::xvid_me_inter4v          ,0,0,_l(""),1,
     _l("xvid_me_inter4v"),1,
   IDFF_enc_xvid_vhq                 ,&TcoSettings::xvid_vhq                 ,0,4,_l(""),1,
     _l("xvid_vhq"),0,
   IDFF_enc_xvid_vhq_modedecisionbits,&TcoSettings::xvid_vhq_modedecisionbits,0,0,_l(""),1,
     _l("xvid_vhq_modedecisionbits"),1,
   IDFF_enc_is_xvid_vhq_custom       ,&TcoSettings::is_xvid_vhq_custom       ,0,0,_l(""),1,
     _l("is_xvid_vhq_custom"),0,
   IDFF_enc_xvid_vhq_custom          ,&TcoSettings::xvid_vhq_custom          ,1,1,_l(""),1,
     _l("xvid_vhq_custom"),XVID_ME_RD::HALFPELREFINE16|XVID_ME_RD::HALFPELREFINE8|XVID_ME_RD::QUARTERPELREFINE16|XVID_ME_RD::QUARTERPELREFINE8|XVID_ME_RD::CHECKPREDICTION|XVID_ME_RD::EXTSEARCH,

   IDFF_enc_skalSearchMetric,&TcoSettings::skalSearchMetric,0,2,_l(""),1,
     _l("skalSearchMetric"),0,

   IDFF_enc_quant_type            ,&TcoSettings::quant_type,0,5,_l(""),1,
     _l("quant_type"),QUANT::H263,
   IDFF_enc_qmatrix_intra_custom0 ,&TcoSettings::qmatrix_intra_custom0 ,1,1,_l(""),1,
     _l("qmatrix_intra_custom0") ,0x13121108,
   IDFF_enc_qmatrix_intra_custom1 ,&TcoSettings::qmatrix_intra_custom4 ,1,1,_l(""),1,
     _l("qmatrix_intra_custom1") ,0x1b191715,
   IDFF_enc_qmatrix_intra_custom2 ,&TcoSettings::qmatrix_intra_custom8 ,1,1,_l(""),1,
     _l("qmatrix_intra_custom2") ,0x15131211,
   IDFF_enc_qmatrix_intra_custom3 ,&TcoSettings::qmatrix_intra_custom12,1,1,_l(""),1,
     _l("qmatrix_intra_custom3") ,0x1c1b1917,
   IDFF_enc_qmatrix_intra_custom4 ,&TcoSettings::qmatrix_intra_custom16,1,1,_l(""),1,
     _l("qmatrix_intra_custom4") ,0x17161514,
   IDFF_enc_qmatrix_intra_custom5 ,&TcoSettings::qmatrix_intra_custom20,1,1,_l(""),1,
     _l("qmatrix_intra_custom5") ,0x1e1c1a18,
   IDFF_enc_qmatrix_intra_custom6 ,&TcoSettings::qmatrix_intra_custom24,1,1,_l(""),1,
     _l("qmatrix_intra_custom6") ,0x18171615,
   IDFF_enc_qmatrix_intra_custom7 ,&TcoSettings::qmatrix_intra_custom28,1,1,_l(""),1,
     _l("qmatrix_intra_custom7") ,0x201e1c1a,
   IDFF_enc_qmatrix_intra_custom8 ,&TcoSettings::qmatrix_intra_custom32,1,1,_l(""),1,
     _l("qmatrix_intra_custom8") ,0x1a181716,
   IDFF_enc_qmatrix_intra_custom9 ,&TcoSettings::qmatrix_intra_custom36,1,1,_l(""),1,
     _l("qmatrix_intra_custom9") ,0x23201e1c,
   IDFF_enc_qmatrix_intra_custom10,&TcoSettings::qmatrix_intra_custom40,1,1,_l(""),1,
     _l("qmatrix_intra_custom10"),0x1c1a1817,
   IDFF_enc_qmatrix_intra_custom11,&TcoSettings::qmatrix_intra_custom44,1,1,_l(""),1,
     _l("qmatrix_intra_custom11"),0x2623201e,
   IDFF_enc_qmatrix_intra_custom12,&TcoSettings::qmatrix_intra_custom48,1,1,_l(""),1,
     _l("qmatrix_intra_custom12"),0x1e1c1a19,
   IDFF_enc_qmatrix_intra_custom13,&TcoSettings::qmatrix_intra_custom52,1,1,_l(""),1,
     _l("qmatrix_intra_custom13"),0x29262320,
   IDFF_enc_qmatrix_intra_custom14,&TcoSettings::qmatrix_intra_custom56,1,1,_l(""),1,
     _l("qmatrix_intra_custom14"),0x201e1c1b,
   IDFF_enc_qmatrix_intra_custom15,&TcoSettings::qmatrix_intra_custom60,1,1,_l(""),1,
     _l("qmatrix_intra_custom15"),0x2d292623,
   IDFF_enc_qmatrix_inter_custom0 ,&TcoSettings::qmatrix_inter_custom0 ,1,1,_l(""),1,
     _l("qmatrix_inter_custom0") ,0x13121110,
   IDFF_enc_qmatrix_inter_custom1 ,&TcoSettings::qmatrix_inter_custom4 ,1,1,_l(""),1,
     _l("qmatrix_inter_custom1") ,0x17161514,
   IDFF_enc_qmatrix_inter_custom2 ,&TcoSettings::qmatrix_inter_custom8 ,1,1,_l(""),1,
     _l("qmatrix_inter_custom2") ,0x14131211,
   IDFF_enc_qmatrix_inter_custom3 ,&TcoSettings::qmatrix_inter_custom12,1,1,_l(""),1,
     _l("qmatrix_inter_custom3") ,0x18171615,
   IDFF_enc_qmatrix_inter_custom4 ,&TcoSettings::qmatrix_inter_custom16,1,1,_l(""),1,
     _l("qmatrix_inter_custom4") ,0x15141312,
   IDFF_enc_qmatrix_inter_custom5 ,&TcoSettings::qmatrix_inter_custom20,1,1,_l(""),1,
     _l("qmatrix_inter_custom5") ,0x19181716,
   IDFF_enc_qmatrix_inter_custom6 ,&TcoSettings::qmatrix_inter_custom24,1,1,_l(""),1,
     _l("qmatrix_inter_custom6") ,0x16151413,
   IDFF_enc_qmatrix_inter_custom7 ,&TcoSettings::qmatrix_inter_custom28,1,1,_l(""),1,
     _l("qmatrix_inter_custom7") ,0x1b1a1817,
   IDFF_enc_qmatrix_inter_custom8 ,&TcoSettings::qmatrix_inter_custom32,1,1,_l(""),1,
     _l("qmatrix_inter_custom8") ,0x17161514,
   IDFF_enc_qmatrix_inter_custom9 ,&TcoSettings::qmatrix_inter_custom36,1,1,_l(""),1,
     _l("qmatrix_inter_custom9") ,0x1c1b1a19,
   IDFF_enc_qmatrix_inter_custom10,&TcoSettings::qmatrix_inter_custom40,1,1,_l(""),1,
     _l("qmatrix_inter_custom10"),0x18171615,
   IDFF_enc_qmatrix_inter_custom11,&TcoSettings::qmatrix_inter_custom44,1,1,_l(""),1,
     _l("qmatrix_inter_custom11"),0x1e1c1b1a,
   IDFF_enc_qmatrix_inter_custom12,&TcoSettings::qmatrix_inter_custom48,1,1,_l(""),1,
     _l("qmatrix_inter_custom12"),0x1a181716,
   IDFF_enc_qmatrix_inter_custom13,&TcoSettings::qmatrix_inter_custom52,1,1,_l(""),1,
     _l("qmatrix_inter_custom13"),0x1f1e1c1b,
   IDFF_enc_qmatrix_inter_custom14,&TcoSettings::qmatrix_inter_custom56,1,1,_l(""),1,
     _l("qmatrix_inter_custom14"),0x1b191817,
   IDFF_enc_qmatrix_inter_custom15,&TcoSettings::qmatrix_inter_custom60,1,1,_l(""),1,
     _l("qmatrix_inter_custom15"),0x211f1e1c,

   IDFF_enc_qmatrix_intra4x4Y_custom0 ,&TcoSettings::qmatrix_intra4x4Y_custom0 ,1,1,_l(""),1,
     _l("qmatrix_intra4x4Y_custom0") ,0x10101010,
   IDFF_enc_qmatrix_intra4x4Y_custom1 ,&TcoSettings::qmatrix_intra4x4Y_custom4 ,1,1,_l(""),1,
     _l("qmatrix_intra4x4Y_custom1") ,0x10101010,
   IDFF_enc_qmatrix_intra4x4Y_custom2 ,&TcoSettings::qmatrix_intra4x4Y_custom8 ,1,1,_l(""),1,
     _l("qmatrix_intra4x4Y_custom2") ,0x10101010,
   IDFF_enc_qmatrix_intra4x4Y_custom3 ,&TcoSettings::qmatrix_intra4x4Y_custom12,1,1,_l(""),1,
     _l("qmatrix_intra4x4Y_custom3") ,0x10101010,

   IDFF_enc_qmatrix_inter4x4Y_custom0 ,&TcoSettings::qmatrix_inter4x4Y_custom0 ,1,1,_l(""),1,
     _l("qmatrix_inter4x4Y_custom0") ,0x10101010,
   IDFF_enc_qmatrix_inter4x4Y_custom1 ,&TcoSettings::qmatrix_inter4x4Y_custom4 ,1,1,_l(""),1,
     _l("qmatrix_inter4x4Y_custom1") ,0x10101010,
   IDFF_enc_qmatrix_inter4x4Y_custom2 ,&TcoSettings::qmatrix_inter4x4Y_custom8 ,1,1,_l(""),1,
     _l("qmatrix_inter4x4Y_custom2") ,0x10101010,
   IDFF_enc_qmatrix_inter4x4Y_custom3 ,&TcoSettings::qmatrix_inter4x4Y_custom12,1,1,_l(""),1,
     _l("qmatrix_inter4x4Y_custom3") ,0x10101010,

   IDFF_enc_qmatrix_intra4x4C_custom0 ,&TcoSettings::qmatrix_intra4x4C_custom0 ,1,1,_l(""),1,
     _l("qmatrix_intra4x4C_custom0") ,0x10101010,
   IDFF_enc_qmatrix_intra4x4C_custom1 ,&TcoSettings::qmatrix_intra4x4C_custom4 ,1,1,_l(""),1,
     _l("qmatrix_intra4x4C_custom1") ,0x10101010,
   IDFF_enc_qmatrix_intra4x4C_custom2 ,&TcoSettings::qmatrix_intra4x4C_custom8 ,1,1,_l(""),1,
     _l("qmatrix_intra4x4C_custom2") ,0x10101010,
   IDFF_enc_qmatrix_intra4x4C_custom3 ,&TcoSettings::qmatrix_intra4x4C_custom12,1,1,_l(""),1,
     _l("qmatrix_intra4x4C_custom3") ,0x10101010,

   IDFF_enc_qmatrix_inter4x4C_custom0 ,&TcoSettings::qmatrix_inter4x4C_custom0 ,1,1,_l(""),1,
     _l("qmatrix_inter4x4C_custom0") ,0x10101010,
   IDFF_enc_qmatrix_inter4x4C_custom1 ,&TcoSettings::qmatrix_inter4x4C_custom4 ,1,1,_l(""),1,
     _l("qmatrix_inter4x4C_custom1") ,0x10101010,
   IDFF_enc_qmatrix_inter4x4C_custom2 ,&TcoSettings::qmatrix_inter4x4C_custom8 ,1,1,_l(""),1,
     _l("qmatrix_inter4x4C_custom2") ,0x10101010,
   IDFF_enc_qmatrix_inter4x4C_custom3 ,&TcoSettings::qmatrix_inter4x4C_custom12,1,1,_l(""),1,
     _l("qmatrix_inter4x4C_custom3") ,0x10101010,

   IDFF_enc_q_i_min          ,&TcoSettings::q_i_min          ,minQuant,maxQuant,_l(""),1,
     _l("q_i_min"),2,
   IDFF_enc_q_i_max          ,&TcoSettings::q_i_max          ,minQuant,maxQuant,_l(""),1,
     _l("q_i_max"),31,
   IDFF_enc_i_quant_factor   ,&TcoSettings::i_quant_factor   ,-3100,3100,_l(""),1,
     _l("i_quant_factor"),-80,
   IDFF_enc_i_quant_offset   ,&TcoSettings::i_quant_offset   ,-3100,3100,_l(""),1,
     _l("i_quant_offset"),0,
   IDFF_enc_q_p_min          ,&TcoSettings::q_p_min          ,minQuant,maxQuant,_l(""),1,
     _l("q_p_min"),2,
   IDFF_enc_q_p_max          ,&TcoSettings::q_p_max          ,minQuant,maxQuant,_l(""),1,
     _l("q_p_max"),31,
   IDFF_enc_q_b_min          ,&TcoSettings::q_b_min          ,minQuant,maxQuant,_l(""),1,
     _l("q_b_min"),2,
   IDFF_enc_q_b_max          ,&TcoSettings::q_b_max          ,minQuant,maxQuant,_l(""),1,
     _l("q_b_max"),31,
   IDFF_enc_q_mb_min         ,&TcoSettings::q_mb_min         ,minQuant,maxQuant,_l(""),1,
     _l("q_mb_min"),2,
   IDFF_enc_q_mb_max         ,&TcoSettings::q_mb_max         ,minQuant,maxQuant,_l(""),1,
     _l("q_mb_max"),31,
   IDFF_enc_trellisquant     ,&TcoSettings::trellisquant     ,    0,   0,_l(""),1,
     _l("trellisquant"),0,
   IDFF_enc_qns              ,&TcoSettings::qns              ,    0,   3,_l(""),1,
     _l("qns"),0,
   IDFF_enc_b_quant_factor   ,&TcoSettings::b_quant_factor   ,-3100,3100,_l(""),1,
     _l("b_quant_factor"),125,
   IDFF_enc_b_quant_offset   ,&TcoSettings::b_quant_offset   ,-3100,3100,_l(""),1,
     _l("b_quant_offset"),125,
   IDFF_enc_isInterQuantBias ,&TcoSettings::isInterQuantBias ,    0,   0,_l(""),1,
     _l("isInterQuantBias"),0,
   IDFF_enc_interQuantBias   ,&TcoSettings::interQuantBias   , -512, 512,_l(""),1,
     _l("interQuantBias"),0,
   IDFF_enc_isIntraQuantBias ,&TcoSettings::isIntraQuantBias ,    0,   0,_l(""),1,
     _l("isIntraQuantBias"),0,
   IDFF_enc_intraQuantBias   ,&TcoSettings::intraQuantBias   , -512, 512,_l(""),1,
     _l("intraQuantBias"),0,
   IDFF_enc_dct_algo         ,&TcoSettings::dct_algo         ,    0,   4,_l(""),1,
     _l("dct_algo"),0,
   IDFF_enc_mpeg2_dc_prec    ,&TcoSettings::mpeg2_dc_prec    ,    0,   3,_l(""),1,
     _l("mp2e_mpeg2_dc_prec"),1,

   IDFF_enc_ff1_vratetol       ,&TcoSettings::ff1_vratetol       ,1,1024*10,_l(""),1,
     _l("ff1_vratetol"),1024,
   IDFF_enc_ff1_vqcomp         ,&TcoSettings::ff1_vqcomp         ,0,100,_l(""),1,
     _l("ff1_vqcomp"),50,
   IDFF_enc_ff1_vqblur1        ,&TcoSettings::ff1_vqblur1        ,0,100,_l(""),1,
     _l("ff1_vqblur1"),50,
   IDFF_enc_ff1_vqblur2        ,&TcoSettings::ff1_vqblur2        ,0,100,_l(""),1,
     _l("ff1_vqblur2"),50,
   IDFF_enc_ff1_vqdiff         ,&TcoSettings::ff1_vqdiff         ,0, 31,_l(""),1,
     _l("ff1_vqdiff"),3,
   IDFF_enc_ff1_rc_squish      ,&TcoSettings::ff1_rc_squish      ,0,  0,_l(""),1,
     _l("ff1_rc_squish"),0,
   IDFF_enc_ff1_rc_max_rate1000,&TcoSettings::ff1_rc_max_rate1000,1,  1,_l(""),1,
     _l("ff1_rc_max_rate1000"),0,
   IDFF_enc_ff1_rc_min_rate1000,&TcoSettings::ff1_rc_min_rate1000,1,  1,_l(""),1,
     _l("ff1_rc_min_rate1000"),0,
   IDFF_enc_ff1_rc_buffer_size ,&TcoSettings::ff1_rc_buffer_size ,1,  1,_l(""),1,
     _l("ff1_rc_buffer_size"),0,

   IDFF_enc_svcd_scan_offset  ,&TcoSettings::svcd_scan_offset  ,0,  0,_l(""),1,
     _l("svcd_scan_offset"),0,

   IDFF_enc_xvid_rc_reaction_delay_factor,&TcoSettings::xvid_rc_reaction_delay_factor,0,10000000,_l(""),1,
     _l("xvid_rc_reaction_delay_factor"),16,
   IDFF_enc_xvid_rc_averaging_period     ,&TcoSettings::xvid_rc_averaging_period     ,0,10000000,_l(""),1,
     _l("xvid_rc_averaging_period"),100,
   IDFF_enc_xvid_rc_buffer               ,&TcoSettings::xvid_rc_buffer               ,0,10000000,_l(""),1,
     _l("xvid_rc_buffer"),100,

   IDFF_enc_isCreditsStart    ,&TcoSettings::isCreditsStart    ,0,0,_l(""),1,
     _l("isCreditsStart"),0,
   IDFF_enc_isCreditsEnd      ,&TcoSettings::isCreditsEnd      ,0,0,_l(""),1,
     _l("isCreditsEnd"),0,
   IDFF_enc_creditsStartBegin ,&TcoSettings::creditsStartBegin ,0,10000000,_l(""),1,
     _l("creditsStartBegin"),0,
   IDFF_enc_creditsStartEnd   ,&TcoSettings::creditsStartEnd   ,0,10000000,_l(""),1,
     _l("creditsStartEnd"),0,
   IDFF_enc_creditsEndBegin   ,&TcoSettings::creditsEndBegin   ,0,10000000,_l(""),1,
     _l("creditsEndBegin"),0,
   IDFF_enc_creditsEndEnd     ,&TcoSettings::creditsEndEnd     ,0,10000000,_l(""),1,
     _l("creditsEndEnd"),0,
   IDFF_enc_credits_mode      ,&TcoSettings::credits_mode      ,0,2,_l(""),1,
     _l("credits_mode"),CREDITS_MODE::PERCENT,
   IDFF_enc_credits_percent   ,&TcoSettings::credits_percent   ,1,100,_l(""),1,
     _l("credits_percent"),20,
   IDFF_enc_credits_quant_i   ,&TcoSettings::credits_quant_i   ,minQuant,maxQuant,_l(""),1,
     _l("credits_quant_i"),20,
   IDFF_enc_credits_quant_p   ,&TcoSettings::credits_quant_p   ,minQuant,maxQuant,_l(""),1,
     _l("credits_quant_p"),20,
   IDFF_enc_credits_size_start,&TcoSettings::credits_size_start,1,10000000,_l(""),1,
     _l("credits_size_start"),10000,
   IDFF_enc_credits_size_end  ,&TcoSettings::credits_size_end  ,1,10000000,_l(""),1,
     _l("credits_size_end"),10000,
   IDFF_enc_graycredits       ,&TcoSettings::graycredits       ,0,0,_l(""),1,
     _l("graycredits"),0,

   IDFF_enc_xvid2pass_use_write             ,&TcoSettings::xvid2pass_use_write             ,0,0,_l(""),1,
     _l("xvid2pass_use_write"),0,
   IDFF_enc_twopass_max_bitrate             ,&TcoSettings::twopass_max_bitrate             ,1,10000*1000,_l(""),1,
     _l("twopass_max_bitrate"),10000*1000,
   IDFF_enc_twopass_max_overflow_improvement,&TcoSettings::twopass_max_overflow_improvement,0,100,_l(""),1,
     _l("twopass_max_overflow_improvement"),60,
   IDFF_enc_twopass_max_overflow_degradation,&TcoSettings::twopass_max_overflow_degradation,0,100,_l(""),1,
     _l("twopass_max_overflow_degradation"),60,
   IDFF_enc_keyframe_boost                  ,&TcoSettings::keyframe_boost                  ,0,100,_l(""),1,
     _l("keyframe_boost"),0,
   IDFF_enc_kftreshold                      ,&TcoSettings::kftreshold                      ,0,1000000,_l(""),1,
     _l("kftreshold"),10,
   IDFF_enc_kfreduction                     ,&TcoSettings::kfreduction                     ,0,100,_l(""),1,
     _l("kfreduction"),30,
   IDFF_enc_curve_compression_high          ,&TcoSettings::curve_compression_high          ,0,100,_l(""),1,
     _l("curve_compression_high"),25,
   IDFF_enc_curve_compression_low           ,&TcoSettings::curve_compression_low           ,0,100,_l(""),1,
     _l("curve_compression_low"),10,
   IDFF_enc_bitrate_payback_delay           ,&TcoSettings::bitrate_payback_delay           ,0,1000000,_l(""),1,
     _l("bitrate_payback_delay"),250,
   IDFF_enc_bitrate_payback_method          ,&TcoSettings::bitrate_payback_method          ,0,1,_l(""),1,
     _l("bitrate_payback_method"),0,
   IDFF_enc_use_alt_curve                   ,&TcoSettings::use_alt_curve                   ,0,0,_l(""),1,
     _l("use_alt_curve"),1,
   IDFF_enc_alt_curve_type                  ,&TcoSettings::alt_curve_type                  ,0,2,_l(""),1,
     _l("alt_curve_type"),1,
   IDFF_enc_alt_curve_high_dist             ,&TcoSettings::alt_curve_high_dist             ,0,5000,_l(""),1,
     _l("alt_curve_high_dist"),500,
   IDFF_enc_alt_curve_low_dist              ,&TcoSettings::alt_curve_low_dist              ,0,5000,_l(""),1,
     _l("alt_curve_low_dist"),90,
   IDFF_enc_alt_curve_use_auto              ,&TcoSettings::alt_curve_use_auto              ,0,0,_l(""),1,
     _l("alt_curve_use_auto"),1,
   IDFF_enc_alt_curve_auto_str              ,&TcoSettings::alt_curve_auto_str              ,0,100,_l(""),1,
     _l("alt_curve_auto_str"),30,
   IDFF_enc_alt_curve_min_rel_qual          ,&TcoSettings::alt_curve_min_rel_qual          ,0,100,_l(""),1,
     _l("alt_curve_min_rel_qual"),50,
   IDFF_enc_alt_curve_use_auto_bonus_bias   ,&TcoSettings::alt_curve_use_auto_bonus_bias   ,0,0,_l(""),1,
     _l("alt_curve_use_auto_bonus_bias"),1,
   IDFF_enc_alt_curve_bonus_bias            ,&TcoSettings::alt_curve_bonus_bias            ,0,5000,_l(""),1,
     _l("alt_curve_bonus_bias"),50,

   IDFF_enc_xvid_lum_masking            ,&TcoSettings::xvid_lum_masking            ,0,0,_l(""),1,
     _l("xvid_lum_masking"),0,
   IDFF_enc_xvid_chromaopt              ,&TcoSettings::xvid_chromaopt              ,0,0,_l(""),1,
     _l("xvid_chromaopt"),0,
   IDFF_enc_isElimLum                   ,&TcoSettings::isElimLum                   ,0,0,_l(""),1,
     _l("isElimLum"),0,
   IDFF_enc_elimLumThres                ,&TcoSettings::elimLumThres                ,-1000,1000,_l(""),1,
     _l("elimLumThres"),-4,
   IDFF_enc_isElimChrom                 ,&TcoSettings::isElimChrom                 ,0,0,_l(""),1,
     _l("isElimChrom"),0,
   IDFF_enc_elimChromThres              ,&TcoSettings::elimChromThres              ,-1000,1000,_l(""),1,
     _l("elimChromThres"),7,
   IDFF_enc_is_lavc_nr                  ,&TcoSettings::is_lavc_nr                  ,0,0,_l(""),1,
     _l("is_lavc_nr"),0,
   IDFF_enc_lavc_nr                     ,&TcoSettings::lavc_nr                     ,0,10000,_l(""),1,
     _l("lavc_nr"),100,
   IDFF_enc_is_ff_lumi_masking          ,&TcoSettings::is_ff_lumi_masking          ,0,0,_l(""),1,
     _l("is_ff_lumi_masking"),0,
   IDFF_enc_ff_lumi_masking1000         ,&TcoSettings::ff_lumi_masking1000         ,0,1000,_l(""),1,
     _l("ff_lumi_masking1000"),70,
   IDFF_enc_is_ff_temporal_cplx_masking ,&TcoSettings::is_ff_temporal_cplx_masking ,0,0,_l(""),1,
     _l("is_ff_temporal_cplx_masking"),0,
   IDFF_enc_ff_temporal_cplx_masking1000,&TcoSettings::ff_temporal_cplx_masking1000,0,1000,_l(""),1,
     _l("ff_temporal_cplx_masking1000"),0,
   IDFF_enc_is_ff_spatial_cplx_masking  ,&TcoSettings::is_ff_spatial_cplx_masking  ,0,0,_l(""),1,
     _l("is_ff_spatial_cplx_masking"),0,
   IDFF_enc_ff_spatial_cplx_masking1000 ,&TcoSettings::ff_spatial_cplx_masking1000 ,0,1000,_l(""),1,
     _l("ff_spatial_cplx_masking1000"),0,
   IDFF_enc_is_ff_p_masking             ,&TcoSettings::is_ff_p_masking             ,0,0,_l(""),1,
     _l("is_ff_p_masking"),0,
   IDFF_enc_ff_p_masking1000            ,&TcoSettings::ff_p_masking1000            ,0,1000,_l(""),1,
     _l("ff_p_masking1000"),0,
   IDFF_enc_is_ff_dark_masking          ,&TcoSettings::is_ff_dark_masking          ,0,0,_l(""),1,
     _l("is_ff_dark_masking"),0,
   IDFF_enc_ff_dark_masking1000         ,&TcoSettings::ff_dark_masking1000         ,0,1000,_l(""),1,
     _l("ff_dark_masking1000"),100,
   IDFF_enc_is_ff_border_masking        ,&TcoSettings::is_ff_border_masking        ,0,0,_l(""),1,
     _l("is_ff_border_masking"),0,
   IDFF_enc_ff_border_masking1000       ,&TcoSettings::ff_border_masking1000       ,0,1000,_l(""),1,
     _l("ff_border_masking1000"),0,
   IDFF_enc_ff_naq                      ,&TcoSettings::ff_naq                      ,0,0,_l(""),1,
     _l("ff_naq"),1,
   IDFF_enc_isSkalMasking               ,&TcoSettings::isSkalMasking               ,0,0,_l(""),1,
     _l("isSkalMasking"),0,
   IDFF_enc_skalMaskingAmp              ,&TcoSettings::skalMaskingAmp              ,0,4000,_l(""),1,
     _l("skalMaskingAmp"),800,

   IDFF_enc_theo_hq              ,&TcoSettings::theo_hq              ,0,0,_l(""),1,
     _l("theo_hq"),0,
   IDFF_enc_theo_sharpness       ,&TcoSettings::theo_sharpness       ,0,2,_l(""),1,
     _l("theo_sharpness"),1,
   IDFF_enc_theo_noisesensitivity,&TcoSettings::theo_noisesensitivity,0,6,_l(""),1,
     _l("theo_noisesensitivity"),1,

   IDFF_enc_raw_fourcc,(TintVal)&TcoSettings::raw_fourcc,0,0,_l(""),1,
     _l("raw_fourcc"),FOURCC_RGB2,
   IDFF_enc_x264_max_ref_frames             ,&TcoSettings::x264_max_ref_frames             , 1,  16,_l(""),1,
       _l("x264_max_ref_frames"),1,
   IDFF_enc_x264_cabac                      ,&TcoSettings::x264_cabac                      , 0,   0,_l(""),1,
       _l("x264_cabac"),1,
   IDFF_enc_x264_interlaced                 ,&TcoSettings::x264_interlaced                 , 0,   0,_l(""),1,
       _l("x264_interlaced"),0,
   IDFF_enc_x264_me_inter                   ,&TcoSettings::x264_me_inter                   , 1,   1,_l(""),1,
       _l("x264_me_inter2"),X264_ANALYSE_I4x4|X264_ANALYSE_I8x8|X264_ANALYSE_PSUB16x16|X264_ANALYSE_BSUB16x16,
   IDFF_enc_x264_me_intra                   ,&TcoSettings::x264_me_intra                   , 1,   1,_l(""),1,
       _l("x264_me_intra2"),X264_ANALYSE_I4x4|X264_ANALYSE_I8x8,
   IDFF_enc_x264_me_subpelrefine            ,&TcoSettings::x264_me_subpelrefine            , 1,   6,_l(""),1,
       _l("x264_me_subpelrefine"),5,
   IDFF_enc_x264_mv_range                   ,&TcoSettings::x264_mv_range                   , 1, 511,_l(""),1,
       _l("x264_mv_range"),511,
   IDFF_enc_x264_me_method                  ,&TcoSettings::x264_me_method                  , 0,   4,_l(""),1,
       _l("x264_me_method"),X264_ME_HEX,
   IDFF_enc_x264_me_range                   ,&TcoSettings::x264_me_range                   , 4,1024,_l(""),1,
       _l("x264_me_range"),16,
   IDFF_enc_x264_i_direct_mv_pred           ,&TcoSettings::x264_i_direct_mv_pred           , 0,   3,_l(""),1,
       _l("x264_i_direct_mv_pred"),X264_DIRECT_PRED_SPATIAL,
   IDFF_enc_x264_i_deblocking_filter_alphac0,&TcoSettings::x264_i_deblocking_filter_alphac0,-6,   6,_l(""),1,
       _l("x264_i_deblocking_filter_alphac0"),0,
   IDFF_enc_x264_i_deblocking_filter_beta   ,&TcoSettings::x264_i_deblocking_filter_beta   ,-6,   6,_l(""),1,
       _l("x264_i_deblocking_filter_beta"),0,
   IDFF_enc_x264_b_bframe_pyramid           ,&TcoSettings::x264_b_bframe_pyramid           , 0,   0,_l(""),1,
       _l("x264_b_bframe_pyramid"),0,
   IDFF_enc_x264_b_aud                      ,&TcoSettings::x264_b_aud                      , 0,   0,_l(""),1,
       _l("x264_b_aud"),0,
   IDFF_enc_x264_mixed_ref                  ,&TcoSettings::x264_mixed_ref                  , 0,   0,_l(""),1,
       _l("x264_mixed_ref"),0,
   IDFF_enc_x264_b_dct_decimate             ,&TcoSettings::x264_b_dct_decimate             , 0,   0,_l(""),1,
       _l("x264_b_dct_decimate"),1,
   IDFF_enc_x264_aq_strength100             ,&TcoSettings::x264_aq_strength100             , 1, 100,_l(""),1,
       _l("x264_aq_strength100"),30,
   0,
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_enc_ff1_stats_flnm                  ,(TstrVal)&TcoSettings::ff1_stats_flnm         ,MAX_PATH,0,_l(""),1,
     _l("ff1_stats_flnm"),_l("\\video.ffstats"),
   IDFF_enc_storeExtFlnm                    ,(TstrVal)&TcoSettings::storeExtFlnm           ,MAX_PATH,0,_l(""),1,
     _l("storeExtFlnm"),_l(""),

   IDFF_enc_stats1flnm                      ,(TstrVal)&TcoSettings::stats1flnm             ,MAX_PATH,0,_l(""),1,
     _l("stats1flnm"),_l("\\video.stats"),
   IDFF_enc_stats2flnm                      ,(TstrVal)&TcoSettings::stats2flnm             ,MAX_PATH,0,_l(""),1,
     _l("stats2flnm"),_l("\\videogk.stats"),
   0
  };
 addOptions(sopts);
 setOnChange(IDFF_enc_forceIncsp,this,&TcoSettings::onIncspChange);
 setOnChange(IDFF_enc_incsp,this,&TcoSettings::onIncspChange);

 static const TcreateParamList1 listAspectMode(aspectModes);setParamList(IDFF_enc_aspectMode,&listAspectMode);
 static const TcreateParamList1 listHuffYUVcaps(huffYUVcsps);setParamList(IDFF_enc_huffyuv_csp,&listHuffYUVcaps);
 static const TcreateParamList1 listHuffYUVpreds(huffYUVpreds);setParamList(IDFF_enc_huffyuv_pred,&listHuffYUVpreds);
 static const TcreateParamList1 listFFV1coders(ffv1coders);setParamList(IDFF_enc_ffv1_coder,&listFFV1coders);
 static const TcreateParamList1 listFFV1contexts(ffv1contexts);setParamList(IDFF_enc_ffv1_context,&listFFV1contexts);
 static const TcreateParamList2<TcspFcc> listFFV1csp(ffv1csps,&TcspFcc::name);setParamList(IDFF_enc_ffv1_csp,&listFFV1csp);
 static const TcreateParamList1 listMuxer(Tmuxer::muxers);setParamList(IDFF_enc_muxer,&listMuxer);
 static const TcreateParamList1 listX264meMethod(x264_me_methods);setParamList(IDFF_enc_x264_me_method,&listX264meMethod);
}

void TcoSettings::saveReg(void)
{
 TregOpRegWrite t(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOWENC);
 reg_op(t);
}
void TcoSettings::loadReg(void)
{
 TregOpRegRead t(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT _l("\\") FFDSHOWENC);
 reg_op(t);
 fillIncsps();
}
int TcoSettings::isInCredits(int frame) const
{
 if (isCreditsStart && frame>=creditsStartBegin && frame<=creditsStartEnd)
  return CREDITS_POS::START;
 else if (isCreditsEnd && frame>=creditsEndBegin && frame<=creditsEndEnd)
  return CREDITS_POS::END;
 else
  return CREDITS_POS::NONE;
}
int TcoSettings::getQuantType(int quant) const
{
 switch (quant_type)
  {
   case QUANT::MOD:
    return quant<4?QUANT::MPEG:QUANT::H263;
   case QUANT::MOD_NEW:
    return quant<4?QUANT::H263:QUANT::MPEG;
   default:
    return quant_type;
  }
}
const DVprofile* TcoSettings::getDVprofile(unsigned int dx,unsigned int dy,PixelFormat lavc_pix_fmt) const
{
 if (dv_profile==DV_PROFILE_AUTO)
  {
   AVCodecContext avctx;
   avctx.width=dx;
   avctx.height=dy;
   avctx.pix_fmt=lavc_pix_fmt;
   return dv_codec_profile(&avctx);
  }
 else
  {
   const DVprofile *profile=dv_profiles+dv_profile;
   return (profile->width==(int)dx && profile->height==(int)dy && profile->pix_fmt==lavc_pix_fmt)?profile:NULL;
  }
}
std::vector<const DVprofile*> TcoSettings::getDVprofile(unsigned int dx,unsigned int dy) const
{
 std::vector<const DVprofile*> profiles;
 if (dv_profile==DV_PROFILE_AUTO)
  {
   for (int i=0; i<countof(dv_profiles);i++)
    if ((int)dx==dv_profiles[i].width && (int)dy==dv_profiles[i].height)
     profiles.push_back(dv_profiles+i);
  }
 else
  {
   const DVprofile *profile=dv_profiles+dv_profile;
   if (profile->width==(int)dx && profile->height==(int)dy)
    profiles.push_back(profile);
  }
 return profiles;
}
void TcoSettings::onIncspChange(int,int)
{
 fillIncsps();
}
void TcoSettings::fillIncsps(void)
{
 incsps.clear();
 if (forceIncsp)
  if (incspFourcc<10)
   {
    for (int i=0;i<FF_CSPS_NUM;i++)
     if (csp_inFOURCCmask(1<<i,incspFourcc))
      incsps.push_back(cspInfos+i);
   }
  else
   incsps.push_back(csp_getInfoFcc(incspFourcc));
}

Rational TcoSettings::sar(unsigned int dx,unsigned int dy) const
{
 switch (aspectMode)
  {
   case ASPECT_SAR:
    return Rational(sarX1000,sarY1000);
   default:
   case ASPECT_DAR:
    {
     Trect r(0,0,dx,dy);
     r.setDar(Rational(darX1000,darY1000));
     return r.sar;
    }
  }
}
Rational TcoSettings::dar(unsigned int dx,unsigned int dy) const
{
 switch (aspectMode)
  {
   case ASPECT_SAR:
    return Trect(0,0,dx,dy,sarX1000,sarY1000).dar();
   default:
   case ASPECT_DAR:
    return Rational(darX1000,darY1000);
  }
}
