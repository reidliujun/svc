#ifndef _TCODECSETTINGS_H_
#define _TCODECSETTINGS_H_

#include "ffcodecs.h"
#include "ffImgfmt.h"
#include "rational.h"
#include "Toptions.h"

#pragma warning(push)
#pragma warning(disable:4201)

struct DVprofile;
struct TcoSettings :public Toptions
{
private:
 void onIncspChange(int,int);
public:
 TcoSettings(TintStrColl *Icoll=NULL);

 TintStrColl *options;
 void saveReg(void),loadReg(void);

 int numthreads;
 int mode;
 int bitrate1000;
 int desiredSize;

 int codecId;
 FOURCC fourcc;

 int globalHeader;
 int part;
 int interlacing,interlacing_tff;
 int dropFrames;

 int forceIncsp,incspFourcc;TcspInfos incsps;void fillIncsps(void);
 int isProc;
 int flip;

 int storeAVI;
 int storeExt;
 char_t storeExtFlnm[MAX_PATH];
 int muxer;
 int isFPSoverride;int fpsOverrideNum,fpsOverrideDen;
 int aspectMode;
 enum
  {
   ASPECT_SAR=0,
   ASPECT_DAR=1
  };
 static const char_t *aspectModes[];

 int sarX1000,sarY1000,darX1000,darY1000;
 Rational sar(unsigned int dx,unsigned int dy) const,dar(unsigned int dx,unsigned int dy) const;

 int q_i_min,q_i_max,q_p_min,q_p_max,q_b_min,q_b_max,q_mb_min,q_mb_max;
 bool isQuantControlActive(void) const
  {
   return mode!=ENC_MODE::VBR_QUANT && mode!=ENC_MODE::PASS2_1;
  }
 int i_quant_factor,i_quant_offset;
 int quant,qual;
 int trellisquant;
 int qns;
 int max_key_interval,min_key_interval;
 int keySceneChange;
 int isIntraQuantBias,intraQuantBias;
 int isInterQuantBias,interQuantBias;
 int dct_algo;
 int mpeg2_dc_prec;
 int H263Pflags;

 int me_hq;
 int me_4mv,me_qpel,me_gmc,me_mv0,me_cbp_rd;
 int me_subq;
 int me_cmp,me_cmp_chroma;
 int me_subcmp,me_subcmp_chroma;
 int mb_cmp,mb_cmp_chroma;
 int dia_size;
 int me_last_predictor_count;
 int me_prepass;
 int me_precmp,me_precmp_chroma;
 int dia_size_pre;
 int me_nsse_weight;
 int me_iterative;

 int skalSearchMetric;

 int quant_type;
 int getQuantType(int quant) const; //0 - H.263, 1 - MPEG, 4 - custom (MPEG)
 int32_t qmatrix_intra_custom0,qmatrix_intra_custom4,qmatrix_intra_custom8,qmatrix_intra_custom12,qmatrix_intra_custom16,qmatrix_intra_custom20,qmatrix_intra_custom24,qmatrix_intra_custom28,qmatrix_intra_custom32,qmatrix_intra_custom36,qmatrix_intra_custom40,qmatrix_intra_custom44,qmatrix_intra_custom48,qmatrix_intra_custom52,qmatrix_intra_custom56,qmatrix_intra_custom60;
 int32_t qmatrix_inter_custom0,qmatrix_inter_custom4,qmatrix_inter_custom8,qmatrix_inter_custom12,qmatrix_inter_custom16,qmatrix_inter_custom20,qmatrix_inter_custom24,qmatrix_inter_custom28,qmatrix_inter_custom32,qmatrix_inter_custom36,qmatrix_inter_custom40,qmatrix_inter_custom44,qmatrix_inter_custom48,qmatrix_inter_custom52,qmatrix_inter_custom56,qmatrix_inter_custom60;
 int32_t qmatrix_intra4x4Y_custom0,qmatrix_intra4x4Y_custom4,qmatrix_intra4x4Y_custom8,qmatrix_intra4x4Y_custom12;
 int32_t qmatrix_inter4x4Y_custom0,qmatrix_inter4x4Y_custom4,qmatrix_inter4x4Y_custom8,qmatrix_inter4x4Y_custom12;
 int32_t qmatrix_intra4x4C_custom0,qmatrix_intra4x4C_custom4,qmatrix_intra4x4C_custom8,qmatrix_intra4x4C_custom12;
 int32_t qmatrix_inter4x4C_custom0,qmatrix_inter4x4C_custom4,qmatrix_inter4x4C_custom8,qmatrix_inter4x4C_custom12;
 static std::pair<int,int> getMinMaxQuant(int codecId)
  {
   switch (codecId)
   {
   case CODEC_ID_X264:return std::make_pair(1,51);
   default:return std::make_pair(1,31);
   }
  }
 std::pair<int,int> getMinMaxQuant(void) const
  {
   return getMinMaxQuant(codecId);
  }
 static const int minQuant=1,maxQuant=51; //global

 int limitq(int q) const
  {
   return limit(q,getMinMaxQuant().first,getMinMaxQuant().second);
  }

 //one pass with libavcodec
 int ff1_vratetol;
 int ff1_vqcomp;
 int ff1_vqblur1,ff1_vqblur2;
 int ff1_vqdiff;
 int ff1_rc_squish,ff1_rc_max_rate1000,ff1_rc_min_rate1000,ff1_rc_buffer_size;
 int svcd_scan_offset;

 int ff1_stats_mode;
 char_t ff1_stats_flnm[MAX_PATH];

 int isCreditsStart,isCreditsEnd;
 int creditsStartBegin,creditsStartEnd,creditsEndBegin,creditsEndEnd;
 int credits_mode;
 int credits_percent,credits_quant_i,credits_quant_p;
 int credits_size_start,credits_size_end;
 int isInCredits(int frame) const;

 char_t stats1flnm[MAX_PATH],stats2flnm[MAX_PATH];
 int xvid2pass_use_write;

 int keyframe_boost;
 int kftreshold,kfreduction;
 int curve_compression_high;
 int curve_compression_low;
 int use_alt_curve;
 int alt_curve_use_auto;
 int alt_curve_auto_str;
 int alt_curve_use_auto_bonus_bias;
 int alt_curve_bonus_bias;
 int alt_curve_type;
 int alt_curve_high_dist;
 int alt_curve_low_dist;
 int alt_curve_min_rel_qual;
 int twopass_max_bitrate;
 int twopass_max_overflow_improvement;
 int twopass_max_overflow_degradation;
 int bitrate_payback_delay;
 int bitrate_payback_method;

 int isBframes;
 int max_b_frames,b_dynamic,b_refine;
 int b_quant_factor; //*100
 int b_quant_offset; //*100
 int packedBitstream,dx50bvop;

 int isElimLum,elimLumThres;
 int isElimChrom,elimChromThres;
 int is_lavc_nr,lavc_nr;
 int ff_lumi_masking1000,is_ff_lumi_masking;
 int ff_temporal_cplx_masking1000,is_ff_temporal_cplx_masking;
 int ff_spatial_cplx_masking1000,is_ff_spatial_cplx_masking;
 int ff_p_masking1000,is_ff_p_masking;
 int ff_dark_masking1000,is_ff_dark_masking;
 int ff_border_masking1000,is_ff_border_masking;
 int ff_naq;
 bool isLAVCadaptiveQuant(void) const
  {
   return lavc_codec(codecId) && sup_masking(codecId) && (is_ff_lumi_masking || is_ff_temporal_cplx_masking || is_ff_spatial_cplx_masking || is_ff_p_masking || is_ff_dark_masking);
  }
 int xvid_lum_masking;
 int xvid_chromaopt;
 int isSkalMasking,skalMaskingAmp;

 int xvid_motion_search;
 int is_xvid_me_custom,xvid_me_custom;
 int xvid_me_inter4v;
 int xvid_vhq;
 int xvid_vhq_modedecisionbits,is_xvid_vhq_custom,xvid_vhq_custom;

 int xvid_rc_reaction_delay_factor;
 int xvid_rc_averaging_period;
 int xvid_rc_buffer;

 int huffyuv_csp;
 int huffyuv_pred;
 int huffyuv_ctx;
 static const char_t *huffYUVcsps[],*huffYUVpreds[];

 int ljpeg_csp;

 int ffv1_coder;
 int ffv1_context;
 int ffv1_key_interval;
 int ffv1_csp;
 static const TcspFcc ffv1csps[];
 static const char_t *ffv1coders[];
 static const char_t *ffv1contexts[];

 int dv_profile;
 std::vector<const DVprofile*> getDVprofile(unsigned int dx,unsigned int dy) const;
 const DVprofile* getDVprofile(unsigned int dx,unsigned int dy,PixelFormat lavc_pix_fmt) const;

 int wmv9_kfsecs;
 int wmv9_ivtc,wmv9_deint;
 int wmv9_cplx;
 int wmv9_crisp;
 int wmv9_aviout;

 int theo_hq,theo_sharpness,theo_noisesensitivity;

 int x264_max_ref_frames;
 int x264_cabac;
 int x264_interlaced;
 int x264_me_inter,x264_me_intra;
 int x264_me_subpelrefine;
 int x264_me_method,x264_me_range;
 int x264_mv_range;
 int x264_i_direct_mv_pred;
 int x264_i_deblocking_filter_alphac0,x264_i_deblocking_filter_beta;
 int x264_b_bframe_pyramid;
 int x264_b_aud;
 int x264_mixed_ref;
 int x264_b_dct_decimate;
 int x264_aq_strength100;
 static const char_t *x264_me_methods[];

 FOURCC raw_fourcc;

 int gray,graycredits;
};

#pragma warning(pop)

#endif
