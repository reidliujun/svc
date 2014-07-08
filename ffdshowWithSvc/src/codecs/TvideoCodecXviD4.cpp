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
#include "TvideoCodecXviD4.h"
#include "xvidcore/xvid.h"
#include "xvidcore/decoder.h"
#include "Tdll.h"
#include "TcodecSettings.h"
#include "IffdshowBase.h"
#include "dsutil.h"
#include "ffdebug.h"

const TmeXviDpreset meXviDpresets[]=
{
 _l("None")      ,0,
 _l("Very low")  ,0,
 _l("Low")       ,0,
 _l("Medium")    ,0,
 _l("High")      ,XVID_ME::HALFPELREFINE16,
 _l("Very high") ,XVID_ME::HALFPELREFINE16|XVID_ME::ADVANCEDDIAMOND16,
 _l("Ultra high"),XVID_ME::HALFPELREFINE16|XVID_ME::EXTSEARCH16|XVID_ME::HALFPELREFINE8|XVID_ME::USESQUARES16,
 NULL,0
};
const TmeXviDpreset vhqXviDpresets[]=
{
 _l("Off")           ,0,
 _l("Mode decision") ,0,
 _l("Limited search"),XVID_ME_RD::HALFPELREFINE16|XVID_ME_RD::QUARTERPELREFINE16,
 _l("Medium search") ,XVID_ME_RD::HALFPELREFINE16|XVID_ME_RD::HALFPELREFINE8|XVID_ME_RD::QUARTERPELREFINE16|XVID_ME_RD::QUARTERPELREFINE8|XVID_ME_RD::CHECKPREDICTION,
 _l("Full search")   ,XVID_ME_RD::HALFPELREFINE16|XVID_ME_RD::HALFPELREFINE8|XVID_ME_RD::QUARTERPELREFINE16|XVID_ME_RD::QUARTERPELREFINE8|XVID_ME_RD::CHECKPREDICTION|XVID_ME_RD::EXTSEARCH,
 NULL,0
};

const char_t* TvideoCodecXviD4::dllname=_l("xvidcore.dll");

int TvideoCodecXviD4::me_hq(int rd3)
{
 int rd4=0;
 if (rd3&XVID_ME_RD::HALFPELREFINE16   ) rd4|=XVID_ME_HALFPELREFINE16_RD;
 if (rd3&XVID_ME_RD::HALFPELREFINE8    ) rd4|=XVID_ME_HALFPELREFINE8_RD;
 if (rd3&XVID_ME_RD::QUARTERPELREFINE16) rd4|=XVID_ME_QUARTERPELREFINE16_RD;
 if (rd3&XVID_ME_RD::QUARTERPELREFINE8 ) rd4|=XVID_ME_QUARTERPELREFINE8_RD;
 if (rd3&XVID_ME_RD::EXTSEARCH         ) rd4|=XVID_ME_EXTSEARCH_RD;
 if (rd3&XVID_ME_RD::CHECKPREDICTION   ) rd4|=XVID_ME_CHECKPREDICTION_RD;
 return rd4;
}
int TvideoCodecXviD4::me_(int me3)
{
 int me4=0;
 if (me3&XVID_ME::ADVANCEDDIAMOND16) me4|=XVID_ME_ADVANCEDDIAMOND16;
 if (me3&XVID_ME::HALFPELREFINE16  ) me4|=XVID_ME_HALFPELREFINE16;
 if (me3&XVID_ME::EXTSEARCH16      ) me4|=XVID_ME_EXTSEARCH16;
 if (me3&XVID_ME::USESQUARES16     ) me4|=XVID_ME_USESQUARES16;
 if (me3&XVID_ME::ADVANCEDDIAMOND8 ) me4|=XVID_ME_ADVANCEDDIAMOND8;
 if (me3&XVID_ME::HALFPELREFINE8   ) me4|=XVID_ME_HALFPELREFINE8;
 if (me3&XVID_ME::EXTSEARCH8       ) me4|=XVID_ME_EXTSEARCH8;
 if (me3&(int)XVID_ME::USESQUARES8 ) me4|=XVID_ME_USESQUARES8;
 return me4;
}

TvideoCodecXviD4::TvideoCodecXviD4(IffdshowBase *Ideci,IdecVideoSink *IsinkD):
 Tcodec(Ideci),TcodecDec(Ideci,IsinkD),
 TvideoCodec(Ideci),
 TvideoCodecDec(Ideci,IsinkD),
 TvideoCodecEnc(Ideci,NULL)
{
 create();
}

TvideoCodecXviD4::TvideoCodecXviD4(IffdshowBase *Ideci,IencVideoSink *IsinkE):
 Tcodec(Ideci),TcodecDec(Ideci,NULL),
 TvideoCodec(Ideci),
 TvideoCodecDec(Ideci,NULL),
 TvideoCodecEnc(Ideci,IsinkE)
{
 create();
 if (ok)
  encoders.push_back(new Tencoder(_l("XviD"),CODEC_ID_XVID4));
}

void TvideoCodecXviD4::create(void)
{
 frame=new xvid_enc_frame_t;
 enchandle=NULL;
 dechandle=NULL;
 extradata=NULL;
 dll=new Tdll(dllname,config);
 dll->loadFunction(xvid_global,"xvid_global");
 dll->loadFunction(xvid_encore,"xvid_encore");
 dll->loadFunction(xvid_decore,"xvid_decore");
 dll->loadFunction(xvid_plugin_single,"xvid_plugin_single");
 dll->loadFunction(xvid_plugin_lumimasking,"xvid_plugin_lumimasking");
 if (dll->ok)
  {
   xvid_gbl_init_t init;
   memset(&init,0,sizeof(init));
   init.version=XVID_VERSION;
   init.cpu_flags=(unsigned int)(XVID_CPU_ASM|XVID_CPU_FORCE);
   if (Tconfig::cpu_flags&FF_CPU_MMX     ) init.cpu_flags|=XVID_CPU_MMX;
   if (Tconfig::cpu_flags&FF_CPU_MMXEXT  ) init.cpu_flags|=XVID_CPU_MMXEXT;
   if (Tconfig::cpu_flags&FF_CPU_SSE     ) init.cpu_flags|=XVID_CPU_SSE;
   if (Tconfig::cpu_flags&FF_CPU_SSE2    ) init.cpu_flags|=XVID_CPU_SSE2;
   if (Tconfig::cpu_flags&FF_CPU_SSE3    ) init.cpu_flags|=XVID_CPU_SSE3;
   if (Tconfig::cpu_flags&FF_CPU_3DNOW   ) init.cpu_flags|=XVID_CPU_3DNOW;
   if (Tconfig::cpu_flags&FF_CPU_3DNOWEXT) init.cpu_flags|=XVID_CPU_3DNOWEXT;
   if (xvid_global(NULL,XVID_GBL_INIT,&init,NULL)==0)
    {
     ok=true;
     quantBytes=4;
     return;
    }
  }
 ok=false;
}
TvideoCodecXviD4::~TvideoCodecXviD4()
{
 end();
 if (dll)
  {
   delete dll;
   dll=NULL;
  }
 delete frame;
}
bool TvideoCodecXviD4::getVersion(const Tconfig *config,ffstring &vers,ffstring &license)
{
 Tdll *dl=new Tdll(dllname,config);
 int (*xvid_global)(void *handle, int opt, void *param1, void *param2);
 dl->loadFunction(xvid_global,"xvid_global");
 bool res=false;
 if (xvid_global)
  {
   res=true;
   xvid_gbl_info_t info;
   memset(&info,0,sizeof(info));
   info.version=XVID_VERSION;
   xvid_global(0,XVID_GBL_INFO,&info,NULL);
   vers=ffstring::intToStr(XVID_VERSION_MAJOR(info.actual_version))+_l(".")+ffstring::intToStr(XVID_VERSION_MINOR(info.actual_version))+_l(".")+ffstring::intToStr(XVID_VERSION_PATCH(info.actual_version));
   license.clear();
  }
 else
  {
   vers=_l("XviD: not found");
   license.clear();
  }
 delete dl;
 return res;
}

//-------------------------- decompression ----------------------------
bool TvideoCodecXviD4::beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags)
{
 xvid_dec_create_t dr;
 dr.version=XVID_VERSION;
 dr.width=0;
 dr.height=0;
 extradata=new Textradata(mt,8);
 int res=xvid_decore(NULL,XVID_DEC_CREATE,&dr,NULL);
 if (res<0) return false;
 dechandle=dr.handle;
 pictbuf.clear();
 quantsDx=(pict.rectFull.dx+15)>>4;quantsDy=(pict.rectFull.dy+15)>>4;
 pict.csp=FF_CSP_420P;
 return true;
}
HRESULT TvideoCodecXviD4::flushDec(void)
{
 return decompress(NULL,0,NULL);
}
HRESULT TvideoCodecXviD4::decompress(const unsigned char *src0,size_t srcLen0,IMediaSample *pIn)
{
 xvid_dec_frame_t xframe;
 xframe.version=XVID_VERSION;
 xframe.general=XVID_LOWDELAY;
 //TODO: XVID_DISCONTINUITY

 xframe.output.csp=XVID4_CSP_PLANAR;
 xframe.brightness=0;
 const unsigned char *src;size_t srcLen;TbyteBuffer buf;
 if (extradata && extradata->size)
  {
   buf.append(extradata->data,extradata->size);
   buf.append(src0,srcLen0);
   src=&*buf.begin();
   srcLen=buf.size();
   delete extradata;extradata=NULL;
  }
 else
  {
   src=src0;
   srcLen=srcLen0;
  }
 if (pIn)
  if (FAILED(pIn->GetTime(&rtStart,&rtStop)))
   rtStart=rtStop=_I64_MIN;
repeat:
 xframe.bitstream=(void*)src;
 xframe.length=src0?(int)srcLen:-1;
 xframe.output.plane[0]=pict.data[0];xframe.output.plane[1]=pict.data[1];xframe.output.plane[2]=pict.data[2];
 xframe.output.stride[0]=(int)pict.stride[0];xframe.output.stride[1]=(int)pict.stride[1];xframe.output.stride[2]=(int)pict.stride[2];
 xvid_dec_stats_t stats;memset(&stats,0,sizeof(stats));
 stats.version=XVID_VERSION;
 int length=xvid_decore(dechandle,XVID_DEC_DECODE,&xframe,&stats);
 if (extradata) {delete extradata;extradata=NULL;}
 if (length<0 || (stats.type==XVID_TYPE_NOTHING && length>0))
  return S_FALSE;
 if (stats.type==XVID_TYPE_VOL)
  {
   pict.alloc(stats.data.vol.width,stats.data.vol.height,FF_CSP_420P,pictbuf);
   if (stats.data.vol.par==XVID_PAR_EXT)
    pict.rectFull.sar=pict.rectClip.sar=Rational(stats.data.vol.par_width,stats.data.vol.par_height);
   else if (connectedSplitter && stats.data.vol.par==XVID_PAR_11_VGA) // With MPC's internal matroska splitter, AR is not reliable.
    pict.rectFull.sar=containerSar;
   else
    {
     static const int pars[][2]=
      {
       { 1, 1},
       {12,11},
       {10,11},
       {16,11},
       {40,33},
       { 0, 0},
      };
     pict.rectFull.sar=pict.rectClip.sar=Rational(pars[stats.data.vol.par-1][0],pars[stats.data.vol.par-1][1]);
    }
   src+=length;
   srcLen-=length;
   goto repeat;
  }
 else
  {
   switch (stats.type)
    {
     case XVID_TYPE_IVOP:pict.frametype=FRAME_TYPE::I;break;
     case XVID_TYPE_PVOP:pict.frametype=FRAME_TYPE::P;break;
     case XVID_TYPE_BVOP:pict.frametype=FRAME_TYPE::B;break;
     case XVID_TYPE_SVOP:pict.frametype=FRAME_TYPE::GMC;break;
    }
   if (pIn && pIn->IsPreroll()==S_OK)
    return sinkD->deliverPreroll(pict.frametype);
   quants=stats.data.vop.qscale;
   quantsStride=stats.data.vop.qscale_stride;
   quantType=1;
   TffPict p1=pict;
   DECODER *dec=(DECODER *)dechandle;
   p1.fieldtype=dec->interlacing?(dec->top_field_first?FIELD_TYPE::INT_TFF:FIELD_TYPE::INT_BFF):FIELD_TYPE::PROGRESSIVE_FRAME;
   if (pIn)
    {
     p1.rtStart=rtStart;
     p1.rtStop=rtStop;
    }
   else
    {
     p1.rtStart=rtStop;
     p1.rtStop=rtStop+(rtStop-rtStart);
    }
   p1.setRO(true);
   return sinkD->deliverDecodedSample(p1);
  }
}

//-------------------------- compression ----------------------------
LRESULT TvideoCodecXviD4::beginCompress(int cfgcomode,int csp,const Trect &r)
{
 xvid_enc_create_t cr;
 memset(&cr,0,sizeof(cr));
 cr.version=XVID_VERSION;
 std::vector<xvid_enc_plugin_t> plugins;
 xvid_enc_zone_t zoneQual;
 if (coCfg->numthreads>1 && sup_threads(coCfg->codecId))
 cr.num_threads=deci->getParam2(IDFF_numthreads);
 else
 cr.num_threads=0;
 switch (cfgcomode)
  {
   case ENC_MODE::UNKNOWN:
   case ENC_MODE::CBR:
   case ENC_MODE::VBR_QUAL:
    {
     xvid_plugin_single_t single;
     memset(&single,0,sizeof(single));
     single.version=XVID_VERSION;
     single.bitrate=coCfg->bitrate1000*1000;
     single.reaction_delay_factor=coCfg->xvid_rc_reaction_delay_factor;
     single.averaging_period=coCfg->xvid_rc_averaging_period;
     single.buffer=coCfg->xvid_rc_buffer;
     xvid_enc_plugin_t plg;
     plg.func=xvid_plugin_single;
     plg.param=&single;
     plugins.push_back(plg);
     if (cfgcomode==ENC_MODE::VBR_QUAL)
      {
       cr.zones=&zoneQual;
       cr.num_zones=1;
       zoneQual.frame=0;
       zoneQual.mode=XVID_ZONE_QUANT;
       zoneQual.increment=3200-31*coCfg->qual;
       zoneQual.base=100;
      }
     break;
    }
   case ENC_MODE::VBR_QUANT:
   case ENC_MODE::PASS2_1:
   case ENC_MODE::PASS2_2_EXT:
   case ENC_MODE::PASS2_2_INT:
    break;
   default:
    return ICERR_ERROR;
  }
 cr.width=r.dx;cr.height=r.dy;
 cr.fbase=deci->getParam2(IDFF_enc_fpsRate);cr.fincr=deci->getParam2(IDFF_enc_fpsScale);

 if (coCfg->isQuantControlActive())
  {
   cr.min_quant[0]=coCfg->limitq(coCfg->q_i_min);cr.max_quant[0]=coCfg->limitq(coCfg->q_i_max);
   cr.min_quant[1]=coCfg->limitq(coCfg->q_p_min);cr.max_quant[1]=coCfg->limitq(coCfg->q_p_max);
   cr.min_quant[2]=coCfg->limitq(coCfg->q_b_min);cr.max_quant[2]=coCfg->limitq(coCfg->q_b_max);
  }
 else
  {
   cr.min_quant[0]=cr.min_quant[1]=cr.min_quant[2]=coCfg->getMinMaxQuant().first ;
   cr.max_quant[0]=cr.max_quant[1]=cr.max_quant[2]=coCfg->getMinMaxQuant().second;
  }

 cr.max_key_interval=coCfg->max_key_interval;

 if (coCfg->isBframes)
  {
   cr.max_bframes=coCfg->max_b_frames;
   cr.bquant_ratio=coCfg->b_quant_factor;
   cr.bquant_offset=coCfg->b_quant_offset;
   if (coCfg->packedBitstream) cr.global|=XVID_GLOBAL_PACKED;
   if (coCfg->dx50bvop) cr.global|=XVID_GLOBAL_CLOSED_GOP;
  }

 psnr=deci->getParam2(IDFF_enc_psnr);
 if (psnr) cr.global|=XVID_GLOBAL_EXTRASTATS_ENABLE;

 if (coCfg->xvid_lum_masking)
  {
   xvid_enc_plugin_t plg;
   plg.func=xvid_plugin_lumimasking;
   plg.param=NULL;
   plugins.push_back(plg);
  }

 cr.plugins=&plugins[0];
 cr.num_plugins=(int)plugins.size();

 int res=xvid_encore(NULL,XVID_ENC_CREATE,&cr,NULL);
 switch (res)
  {
   case XVID_ERR_FAIL:return ICERR_ERROR;
   case XVID_ERR_MEMORY:return ICERR_MEMORY;
   case XVID_ERR_FORMAT:return ICERR_BADFORMAT;
  }
 enchandle=cr.handle;

 memset(frame,0,sizeof(*frame));
 frame->version=XVID_VERSION;

 frame->vop_flags|=XVID_VOP_HALFPEL;
 frame->vop_flags|=XVID_VOP_HQACPRED;

 if (coCfg->trellisquant) frame->vop_flags|=XVID_VOP_TRELLISQUANT;

 if (coCfg->me_gmc) frame->vol_flags|=XVID_VOL_GMC;
 if (coCfg->interlacing)
  {
   frame->vol_flags|=XVID_VOL_INTERLACING;
   if (coCfg->interlacing_tff)
    frame->vop_flags|=XVID_VOP_TOPFIELDFIRST;
  }
 if (coCfg->xvid_chromaopt && !coCfg->gray) frame->vop_flags|=XVID_VOP_CHROMAOPT;

 if (coCfg->is_xvid_me_custom)
  {
   frame->motion=me_(coCfg->xvid_me_custom);
   if (coCfg->xvid_me_inter4v) frame->vop_flags|=XVID_VOP_INTER4V;
  }
 else
  {
   frame->motion=me_(meXviDpresets[coCfg->xvid_motion_search].preset);
   if (coCfg->xvid_motion_search>4) frame->vop_flags|=XVID_VOP_INTER4V;
  }

 if (coCfg->is_xvid_vhq_custom)
  {
   frame->motion|=me_hq(coCfg->xvid_vhq_custom);
   if (coCfg->xvid_vhq_modedecisionbits) frame->vop_flags|=XVID_VOP_MODEDECISION_RD;
  }
 else
  {
   frame->motion|=me_hq(vhqXviDpresets[coCfg->xvid_vhq].preset);
   if (coCfg->xvid_vhq>0) frame->vop_flags|=XVID_VOP_MODEDECISION_RD;
  }

 if (coCfg->me_qpel)
  {
   frame->vol_flags|=XVID_VOL_QUARTERPEL;
   frame->motion|=XVID_ME_QUARTERPELREFINE16|XVID_ME_QUARTERPELREFINE8;
  }
 if (coCfg->me_gmc)
  {
   frame->vol_flags|=XVID_VOL_GMC;
   frame->motion|=XVID_ME_GME_REFINE;
  }
 if (coCfg->me_cmp_chroma && !coCfg->gray) frame->motion|=XVID_ME_CHROMA_PVOP|XVID_ME_CHROMA_BVOP;

 frame->par=XVID_PAR_EXT;
 Rational sar=coCfg->sar(r.dx,r.dy).reduce(255);
 frame->par_width=sar.num;frame->par_height=sar.den;

 if (psnr) frame->vol_flags|=XVID_VOL_EXTRASTATS;

 frame->input.csp=XVID4_CSP_PLANAR;

 if (coCfg->quant_type==QUANT::MPEG)
  frame->quant_intra_matrix=frame->quant_inter_matrix=NULL;
 else if (coCfg->quant_type==QUANT::CUSTOM)
  {
   frame->quant_intra_matrix=(unsigned char*)&coCfg->qmatrix_intra_custom0;
   frame->quant_inter_matrix=(unsigned char*)&coCfg->qmatrix_inter_custom0;
  }

 return ICERR_OK;
}

HRESULT TvideoCodecXviD4::compress(const TffPict &srcpict,TencFrameParams &params)
{
 if (!srcpict.data[0])
  {
   frame->input.csp=XVID4_CSP_NULL;
   frame->input.plane[0]=frame->input.plane[1]=frame->input.plane[2]=NULL;
  }
 else
  {
   frame->input.plane[0]=srcpict.data[0];frame->input.plane[1]=srcpict.data[1];frame->input.plane[2]=srcpict.data[2];
   frame->input.stride[0]=(int)srcpict.stride[0];frame->input.stride[1]=(int)srcpict.stride[1];frame->input.stride[2]=(int)srcpict.stride[2];
  }

 if (params.quant==-1)
  frame->quant=0;
 else
  frame->quant=coCfg->limitq(params.quant);

 if (params.quanttype==QUANT::MPEG || params.quanttype==QUANT::CUSTOM)
  frame->vol_flags|=XVID_VOL_MPEGQUANT;
 else
  frame->vol_flags&=~XVID_VOL_MPEGQUANT;

 switch (params.frametype)
  {
   case FRAME_TYPE::I:frame->type=XVID_TYPE_IVOP;break;
   case FRAME_TYPE::P:frame->type=XVID_TYPE_PVOP;break;
   case FRAME_TYPE::B:frame->type=XVID_TYPE_BVOP;break;
   //case FRAME::SKIP:frame->type=XVID_TYPE_NOTHING;break;
   //case FRAME::PAD:frame->type=XVID_TYPE_;break;
   case FRAME_TYPE::DELAY:frame->type=XVID_TYPE_NOTHING;break;
   default:frame->type=XVID_TYPE_AUTO;
  }

 if (params.gray)
  frame->vop_flags|=XVID_VOP_GREYSCALE;
 else
  frame->vop_flags&=~XVID_VOP_GREYSCALE;

 HRESULT hr=S_OK;
again:
 TmediaSample sample;
 if (FAILED(hr=sinkE->getDstBuffer(&sample,srcpict)))
  return hr;
 frame->bitstream=sample;frame->length=sample.size();

 xvid_enc_stats_t stats;
 memset(&stats,0,sizeof(stats));
 stats.version=XVID_VERSION;
 int length=xvid_encore(enchandle,XVID_ENC_ENCODE,frame,&stats);
 if ((length==0 || length==XVID_ERR_END) && !srcpict.data[0])
  return S_OK;
 else if (length<0)
  return sinkE->deliverError();

 params.quant=stats.quant;
 params.keyframe=frame->out_flags&XVID_KEYFRAME?true:false;
 switch (stats.type)
  {
   case XVID_TYPE_PVOP:params.frametype=FRAME_TYPE::P;break;
   case XVID_TYPE_IVOP:params.frametype=FRAME_TYPE::I;break;
   case XVID_TYPE_BVOP:params.frametype=FRAME_TYPE::B;break;
   case XVID_TYPE_NOTHING:params.frametype=FRAME_TYPE::DELAY;break;
   default:params.frametype=FRAME_TYPE::UNKNOWN;break;
  }

 params.kblks=stats.kblks;
 params.mblks=stats.mblks;
 params.ublks=stats.ublks;

 if (length==0)
  {
   frame->length=1;
   *(uint8_t*)frame->bitstream=0x7f;
  }

 if (psnr)
  {
   params.psnrY=stats.sse_y;
   params.psnrU=stats.sse_u;
   params.psnrV=stats.sse_v;
  }
 params.length=length;
 if (FAILED(hr=sinkE->deliverEncodedSample(sample,params)))
  return hr;
 if (!srcpict.data[0])
  goto again;
 return hr;
}
void TvideoCodecXviD4::end(void)
{
 if (enchandle) xvid_encore(enchandle,XVID_ENC_DESTROY,NULL,NULL);enchandle=NULL;
 if (dechandle) xvid_decore(dechandle,XVID_DEC_DESTROY,NULL,NULL);dechandle=NULL;
 pictbuf.clear();
 if (extradata) delete extradata;extradata=NULL;
}
