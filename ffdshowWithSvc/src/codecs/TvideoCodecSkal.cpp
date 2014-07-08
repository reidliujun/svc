/*
 * Copyright (c) 2004-2006 Milan Cutka
 * SSE calculation functions Copyright (C) 2003 by Pascal Massimino.
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
#include "TvideoCodecSkal.h"
#include "Tdll.h"
#include "ffcodecs.h"
#include "TcodecSettings.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"

const char_t* TvideoCodecSkal::dllname=_l("skl_drv_mpg.dll");

TvideoCodecSkal::TvideoCodecSkal(IffdshowBase *Ideci,IencVideoSink *IsinkE):
 Tcodec(Ideci),
 TvideoCodec(Ideci),
 TvideoCodecEnc(Ideci,IsinkE)
{
 ok=false;
 dll=new Tdll(dllname,config);
 dll->loadFunction(Skl_MP4_New_Encoder             ,"Skl_MP4_New_Encoder");
 dll->loadFunction(Skl_MP4_Delete_Encoder          ,"Skl_MP4_Delete_Encoder");
 dll->loadFunction(Skl_MP4_Enc_Prepare_Next_Frame  ,"Skl_MP4_Enc_Prepare_Next_Frame");
 dll->loadFunction(Skl_MP4_Enc_Get_Next_Frame      ,"Skl_MP4_Enc_Get_Next_Frame");
 dll->loadFunction(Skl_MP4_Enc_Get_Last_Coded_Frame,"Skl_MP4_Enc_Get_Last_Coded_Frame");
 dll->loadFunction(Skl_MP4_Enc_Encode              ,"Skl_MP4_Enc_Encode");
 dll->loadFunction(Skl_MP4_Enc_Finish_Encoding     ,"Skl_MP4_Enc_Finish_Encoding");
 dll->loadFunction(Skl_MP4_Enc_Get_Bits            ,"Skl_MP4_Enc_Get_Bits");
 dll->loadFunction(Skl_MP4_Enc_Get_Bits_Length     ,"Skl_MP4_Enc_Get_Bits_Length");
 dll->loadFunction(Skl_MP4_Enc_Set_CPU             ,"Skl_MP4_Enc_Set_CPU");
 dll->loadFunction(Skl_MP4_Enc_Set_Custom_Matrix   ,"Skl_MP4_Enc_Set_Custom_Matrix");
 dll->loadFunction(Skl_MP4_Enc_Set_Slicer          ,"Skl_MP4_Enc_Set_Slicer");
 dll->loadFunction(Skl_MP4_Enc_Get_All_Frames      ,"Skl_MP4_Enc_Get_All_Frames");
 dll->loadFunction(Skl_MP4_Enc_Ioctl               ,"Skl_MP4_Enc_Ioctl");
 dll->loadFunction(Skl_MP4_Enc_Set_Analyzer_Param_I,"Skl_MP4_Enc_Set_Analyzer_Param_I");
 dll->loadFunction(Skl_MP4_Enc_Get_Analyzer_Param_I,"Skl_MP4_Enc_Get_Analyzer_Param_I");
 dll->loadFunction(Skl_MP4_Enc_Set_Analyzer_Param_F,"Skl_MP4_Enc_Set_Analyzer_Param_F");
 dll->loadFunction(Skl_MP4_Enc_Get_Analyzer_Param_F,"Skl_MP4_Enc_Get_Analyzer_Param_F");
 dll->loadFunction(Skl_MP4_Enc_Set_Analyzer_Param_S,"Skl_MP4_Enc_Set_Analyzer_Param_S");
 dll->loadFunction(Skl_MP4_Enc_Get_Analyzer_Param_S,"Skl_MP4_Enc_Get_Analyzer_Param_S");
 dll->loadFunction(Skl_MP4_Enc_Get_Analyzer_Param_P,"Skl_MP4_Enc_Get_Analyzer_Param_P");
 ok=dll->ok;
 if (ok)
  encoders.push_back(new Tencoder(_l("Skal's MPEG 4"),CODEC_ID_SKAL));
 Enc=NULL;
}
TvideoCodecSkal::~TvideoCodecSkal()
{
 end();
 if (dll) delete dll;
}

bool TvideoCodecSkal::getVersion(const Tconfig *config,ffstring &vers,ffstring &license)
{
 Tdll *dl=new Tdll(dllname,config);
 bool res=false;
 if (dl->ok)
  {
   res=true;
   vers=_l("unknown");
   license.clear();
  }
 else
  {
   vers=_l("not found");
   license.clear();
  }
 delete dl;
 return res;
}
LRESULT TvideoCodecSkal::beginCompress(int cfgcomode,int csp,const Trect &r)
{
 Enc=Skl_MP4_New_Encoder();
 SKL_CPU_FEATURE skl_cpu;
 if      (Tconfig::cpu_flags&FF_CPU_SSE2) skl_cpu=SKL_CPU_SSE2;
 else if (Tconfig::cpu_flags&FF_CPU_SSE ) skl_cpu=SKL_CPU_SSE;
 else if (Tconfig::cpu_flags&FF_CPU_MMX ) skl_cpu=SKL_CPU_MMX;
 else skl_cpu=SKL_CPU_DETECT;
 Skl_MP4_Enc_Set_CPU(Enc,skl_cpu);

 Skl_MP4_Enc_Set_Analyzer_Param_F(Enc,"framerate",float(deci->getParam2(IDFF_enc_fpsRate))/deci->getParam2(IDFF_enc_fpsScale));

 switch (cfgcomode)
  {
   case ENC_MODE::CBR:
    Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"bitrate",coCfg->bitrate1000);
    Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"quant",0);
    break;
   case ENC_MODE::VBR_QUANT:
   case ENC_MODE::PASS2_1:
   case ENC_MODE::PASS2_2_EXT:
   case ENC_MODE::PASS2_2_INT:
    Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"quant",coCfg->limitq(coCfg->quant));
    break;
   case ENC_MODE::UNKNOWN:
    break;
   default:return ICERR_ERROR;
  }

 Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"intra-max-delay",coCfg->max_key_interval);
 if (coCfg->interlacing)
  {
   Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"interlace-dct",2);   //TODO: 1 - decide
   Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"interlace-field",2);
   Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"field-pred-probing",40);
  }
 if (coCfg->trellisquant) Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"use-trellis",1);
 if (coCfg->isSkalMasking) Skl_MP4_Enc_Set_Analyzer_Param_F(Enc,"dquant-amp",coCfg->skalMaskingAmp/100.0f);
 if (coCfg->quant_type==QUANT::MPEG)
  Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"quant-type",1);
 else if (coCfg->quant_type==QUANT::CUSTOM)
  {
   Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"quant-type",1);
   Skl_MP4_Enc_Set_Custom_Matrix(Enc,0,(const uint8_t*)&coCfg->qmatrix_inter_custom0);
   Skl_MP4_Enc_Set_Custom_Matrix(Enc,1,(const uint8_t*)&coCfg->qmatrix_intra_custom0);
  }

 if (coCfg->me_4mv) Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"4v-probing",80);

 if (coCfg->me_qpel)
  Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"subpixel",2);
 else
  Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"subpixel",1);

 if (coCfg->me_gmc)
  {
   Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"gmc-mode",1);
   Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"gmc-pts",3);
   Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"gmc-accuracy",3);  //TODO: config
  }

 Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"search-metric",coCfg->skalSearchMetric); // 0 - sad, 1 - ssd, 2 - hammard

/*
 Skl_MP4_Enc_Set_Analyzer_Param_I( "base-quant", Global_Q );
 Skl_MP4_Enc_Set_Analyzer_Param_I( "bframe", Use_BFrame );
 Skl_MP4_Enc_Set_Analyzer_Param_I( "reduced-frame", Use_Reduced );
 Skl_MP4_Enc_Set_Analyzer_Param_I( "rounding", 0 );
 Skl_MP4_Enc_Set_Analyzer_Param_I( "sad-skip-limit", SAD_Skip_Limit );
 Skl_MP4_Enc_Set_Analyzer_Param_I( "sad-intra-limit", SAD_Intra_Limit );
 Skl_MP4_Enc_Set_Analyzer_Param_I( "base-search-size", MV_Size );
 Skl_MP4_Enc_Set_Analyzer_Param_I( "base-search-size-bwd", MV_Size );
 Skl_MP4_Enc_Set_Analyzer_Param_I( "search-method", Search_Method ); // &MV_Search_Square, &MV_Search_Diamond, &MV_Search_PHODS,  &MV_Search_Log, &MV_Search_Full, &MV_Search_Zero, &MV_Search_Debug

 Skl_MP4_Enc_Set_Analyzer_Param_I( "lambda", Lambda );
 Skl_MP4_Enc_Set_Analyzer_Param_I( "intra-limit", Intra_Limit );
 Skl_MP4_Enc_Set_Analyzer_Param_I( "inter-threshold", Inter_Thresh );
 Skl_MP4_Enc_Set_Analyzer_Param_I( "buffer-size", Enc_Buf_Size );
*/

 Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"hi-mem",1);

 if (deci->getParam2(IDFF_enc_psnr))
  Skl_MP4_Enc_Set_Slicer(Enc,SKL_MP4_SLICER(Sliced_PSNR),&PSNR_Infos);

 return ICERR_OK;
}

HRESULT TvideoCodecSkal::compress(const TffPict &pict,TencFrameParams &params)
{
 const SKL_MP4_PIC *Pic=Skl_MP4_Enc_Prepare_Next_Frame(Enc,pict.rectFull.dx,pict.rectFull.dy);
 TffPict::copy(Pic->Y,Pic->BpS,pict.data[0],pict.stride[0],pict.rectFull.dx  ,pict.rectFull.dy  );
 TffPict::copy(Pic->U,Pic->BpS,pict.data[1],pict.stride[1],pict.rectFull.dx/2,pict.rectFull.dy/2);
 TffPict::copy(Pic->V,Pic->BpS,pict.data[2],pict.stride[2],pict.rectFull.dx/2,pict.rectFull.dy/2);
 if (params.quant!=-1)
  Skl_MP4_Enc_Set_Analyzer_Param_I(Enc,"quant",coCfg->limitq(params.quant));
 PSNR_Infos.setParams(&params);
 if (Skl_MP4_Enc_Encode(Enc))
  {
   TmediaSample sample;
   HRESULT hr;
   if (FAILED(hr=sinkE->getDstBuffer(&sample,pict)))
    return hr;
   params.length=Skl_MP4_Enc_Get_Bits_Length(Enc);
   memcpy(sample,Skl_MP4_Enc_Get_Bits(Enc),params.length);
   switch (Pic->Coding)
    {
     case 0:params.frametype=FRAME_TYPE::I;params.keyframe=true;break;
     case 1:params.frametype=FRAME_TYPE::P;break;
     case 2:params.frametype=FRAME_TYPE::B;break;
     case 3:params.frametype=FRAME_TYPE::GMC;break;
     default:
     case 4:params.frametype=FRAME_TYPE::UNKNOWN;break;
    }
   Skl_MP4_Enc_Get_Analyzer_Param_I(Enc,"quant",&params.quant);
   return sinkE->deliverEncodedSample(sample,params);
  }
 else
  return sinkE->deliverError();
}
void TvideoCodecSkal::end(void)
{
 if (Enc) Skl_MP4_Delete_Encoder(Enc);Enc=NULL;
}

#define SSD(WIDTH,HEIGHT,BPS)                  \
  uint32_t Sum = 0;                            \
  int H = HEIGHT;                              \
  while(H-->0) {                               \
    for(int i=-WIDTH; i<0; i++) {              \
      int32_t D = Src[WIDTH+i] - Dst[WIDTH+i]; \
      Sum += D*D;                              \
    }                                          \
    Src += (BPS);                              \
    Dst += (BPS);                              \
  }                                            \
  return Sum

uint32_t TvideoCodecSkal::PSNR_INFOS::SSD_16x16(const uint8_t *Dst, const uint8_t *Src, int32_t BpS) { SSD(16,16,BpS); }
uint32_t TvideoCodecSkal::PSNR_INFOS::SSD_8x8(const uint8_t *Dst, const uint8_t *Src, int32_t BpS)   { SSD(8,8,BpS); }

#undef SSD

void TvideoCodecSkal::PSNR_INFOS::Store(uint8_t *Dst, const uint8_t *Src, int W, int H, int BpS)
{
 W = (W+7) & ~7;
 for(int y=0; y<H; ++y, Src+=BpS, Dst+=BpS)
  memcpy(Dst, Src, W);
}
uint32_t TvideoCodecSkal::PSNR_INFOS::Get_SSE(uint8_t *Ref, const uint8_t *Src, int W, int H, int BpS)
{
 uint32_t SSE = 0;
 int x,y;
 for(y=0; (y+16)<=H; y+=16)
  {
   for(x=0; (x+16)<=W; x+=16) SSE += SSD_16x16(Ref+x,Src+x,BpS);
   if ((x+8)<=W) SSE += SSD_8x8(Ref+x,Src+x,BpS);
   Src += 16*BpS;
   Ref += 16*BpS;
  }
 if ((y+8)<=H)
  for(x=0; x<W; x+=8) SSE += SSD_8x8(Ref+x,Src+x,BpS);
   return SSE;
}
void TvideoCodecSkal::PSNR_INFOS::Check(int W, int H, int BpS)
{
 const size_t Needed = BpS*H + 2*(BpS/2)*(H/2);
 if (Y==0 || Size!=Needed)
  {
   Clear();
   Y = (uint8_t*)aligned_malloc(Needed);
   U = Y + BpS*H;
   V = U + BpS/2;
   Size = Needed;
  }
}
void TvideoCodecSkal::PSNR_INFOS::Clear(void)
{
 if (Y!=0)
  {
   aligned_free( Y ); Y=0; Size=0;
  }
}

TvideoCodecSkal::PSNR_INFOS::PSNR_INFOS(void)
{
 Size = 0;
 Y=U=V=NULL;
}
TvideoCodecSkal::PSNR_INFOS::~PSNR_INFOS()
{
 Clear();
}

void TvideoCodecSkal::Sliced_PSNR(const SKL_MP4_PIC *Pic, int yo, int Height, void* Data)
{
 PSNR_INFOS *I = (PSNR_INFOS*)Data;

 if (Height==0)    // start/end of scan?
  {
   if (yo==0)
    {      // start of scan
     I->Check(Pic->Width, Pic->Height, Pic->BpS);
     I->Store(I->Y, Pic->Y, Pic->Width,   Pic->Height,   Pic->BpS);
     I->Store(I->U, Pic->U, Pic->Width/2, Pic->Height/2, Pic->BpS);
     I->Store(I->V, Pic->V, Pic->Width/2, Pic->Height/2, Pic->BpS);
    }
   else
    {            // end of scan
     I->params->psnrY = I->Get_SSE(I->Y, Pic->Y, Pic->Width,   Pic->Height,   Pic->BpS);
     I->params->psnrU = I->Get_SSE(I->U, Pic->U, Pic->Width/2, Pic->Height/2, Pic->BpS);
     I->params->psnrV = I->Get_SSE(I->V, Pic->V, Pic->Width/2, Pic->Height/2, Pic->BpS);
    }
  }
 else { /* we're in a slice. */ }
}
