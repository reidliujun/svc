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
#include "wmv9/ff_wmv9.h"
#include "TvideoCodecWmv9.h"
#include "Tdll.h"
#include "ffcodecs.h"
#include "TcodecSettings.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "dsutil.h"

const char_t* TvideoCodecWmv9::dllname=_l("ff_wmv9.dll");

TvideoCodecWmv9::TvideoCodecWmv9(IffdshowBase *Ideci,IdecVideoSink *IsinkD):
 Tcodec(Ideci),TcodecDec(Ideci,IsinkD),
 TvideoCodec(Ideci),
 TvideoCodecDec(Ideci,IsinkD),
 TvideoCodecEnc(Ideci,NULL)
{
 create();
}
TvideoCodecWmv9::TvideoCodecWmv9(IffdshowBase *Ideci,IencVideoSink *IsinkE):
 Tcodec(Ideci),TcodecDec(Ideci,NULL),
 TvideoCodec(Ideci),
 TvideoCodecDec(Ideci,NULL),
 TvideoCodecEnc(Ideci,IsinkE),
 codecIndex(-1)
{
 create();
 if (ok)
  {
   size_t cnt=wmv9->getCodecCount();
   infos=new Tff_wmv9codecInfo[cnt];
   for (size_t i=0;i<cnt;i++)
    {
     const Tff_wmv9codecInfo *info;
     wmv9->getCodecInfo(i,&info);
     infos[info->index]=*info;
     encoders.push_back(new TencoderWmv9(info->name,CodecID(CODEC_ID_WMV9_LIB+info->index),Tfourccs(info->fcc,0),info->cmplxMax));
    }
  }
}

void TvideoCodecWmv9::create(void)
{
 ok=false;
 wmv9=NULL;infos=NULL;
 dll=new Tdll(dllname,config);
 dll->loadFunction(createWmv9,"createWmv9");
 dll->loadFunction(destroyWmv9,"destroyWmv9");
 if (dll->ok)
  {
   wmv9=createWmv9();
   ok=wmv9->getOk();
  }
}
TvideoCodecWmv9::~TvideoCodecWmv9()
{
 end();
 if (dll)
  {
   if (wmv9) destroyWmv9(wmv9);
   delete dll;
  }
 if (infos) delete []infos;
}

const char_t* TvideoCodecWmv9::getName(void) const
{
 tsprintf(codecName,_l("wmv9"),codecIndex>=0?_l(" ("):_l(""),codecIndex>=0?infos[codecIndex].name:_l(""),codecIndex>=0?_l(")"):_l(""));
 return codecName;
}

void TvideoCodecWmv9::getCompressColorspaces(Tcsps &csps,unsigned int outDx,unsigned int outDy)
{
 if (coCfg->wmv9_aviout)
  switch (infos[coCfg->codecId-CODEC_ID_WMV9_LIB].fcc)
   {
    case mmioFOURCC('M','S','S','1'):
    case mmioFOURCC('M','S','S','2'):
     csps.addEnd(FF_CSP_NULL,FF_CSP_RGB24|FF_CSP_FLAGS_VFLIP,FF_CSP_RGB32|FF_CSP_FLAGS_VFLIP,FF_CSP_RGB15|FF_CSP_FLAGS_VFLIP,FF_CSP_RGB16|FF_CSP_FLAGS_VFLIP,FF_CSP_NULL);
     return;
   }
 csps.add(FF_CSP_420P|FF_CSP_FLAGS_YUV_ADJ);
}

LRESULT TvideoCodecWmv9::beginCompress(int cfgcomode,int csp,const Trect &r)
{
 if (!wmv9) return ICERR_ERROR;
 Tff_wmv9cfg cfg9;memset(&cfg9,0,sizeof(cfg9));
 cfg9.codecIndex=codecIndex=coCfg->codecId-CODEC_ID_WMV9_LIB;
 cfg9.bitrate=coCfg->bitrate1000*1024;
 cfg9.width=r.dx;
 cfg9.height=r.dy;
 cfg9.csp=csp;
 cfg9.fps=double(deci->getParam2(IDFF_enc_fpsRate))/deci->getParam2(IDFF_enc_fpsScale);
 cfg9.quality=coCfg->qual;
 cfg9.crisp=coCfg->wmv9_crisp;
 cfg9.seckf=coCfg->wmv9_kfsecs;
 cfg9.bUseVBR=(cfgcomode==ENC_MODE::CBR)?FALSE:TRUE;
 cfg9.vbr_mode=(cfgcomode==ENC_MODE::CBR)?VBR_OFF:VBR_QUALITYBASED;
 cfg9.vbrquality=coCfg->qual;
 cfg9.maxbitrate=1000000;
 cfg9.ivtc=!!coCfg->wmv9_ivtc;
 cfg9.deint=!!coCfg->wmv9_deint;
 cfg9.cplx=std::min(coCfg->wmv9_cplx,infos[cfg9.codecIndex].cmplxMax);
 cfg9.avioutput=!!coCfg->wmv9_aviout;
 if (coCfg->storeExt)
  {
   cfg9.flnm=coCfg->storeExtFlnm[0]?coCfg->storeExtFlnm:NULL;
   deci->putParam(IDFF_enc_ownStoreExt,!cfg9.avioutput);
  }
 return wmv9->start(cfg9)?ICERR_OK:ICERR_ERROR;
}
bool TvideoCodecWmv9::getExtradata(const void* *ptr,size_t *len)
{
 if (!len || !wmv9) return false;
 wmv9->getExtradata(ptr,len);
 return true;
}
HRESULT TvideoCodecWmv9::compress(const TffPict &pict,TencFrameParams &params)
{
 TmediaSample sample;
 HRESULT hr;
 if (FAILED(hr=sinkE->getDstBuffer(&sample,pict)))
  return hr;
 int len=wmv9->write(params.framenum,pict.csp,pict.data,pict.stride,sample);
 if (len<0)
  {
   params.frametype=FRAME_TYPE::I;
   params.keyframe=true;
  }
 else
  params.frametype=FRAME_TYPE::P;
 params.quant=1;
 params.length=abs(len);
 return sinkE->deliverEncodedSample(sample,params);
}
void TvideoCodecWmv9::end(void)
{
 if (wmv9) wmv9->end();
 codecIndex=-1;
}

bool TvideoCodecWmv9::testMediaType(FOURCC fcc,const CMediaType &mt)
{
 if (wmv9)
  {
   const Tff_wmv9codecInfo *codec=wmv9->findCodec(fcc);
   if (!codec) return CODEC_ID_NONE;
   codecIndex=codec->index;
   return !!codec;
  }
 else
  return CODEC_ID_NONE;
}

bool TvideoCodecWmv9::beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags)
{
 Textradata extradata(mt);
 rd=pict.rectFull;
 if (wmv9->decStart(infcc,deciV->getAVIfps1000_2()/1000.0,pict.rectFull.dx,pict.rectFull.dy,extradata.data,extradata.size,&csp))
  {
   pict.csp=csp;
   return true;
  }
 else
  return false;
}

HRESULT TvideoCodecWmv9::decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn)
{
 unsigned char *dst[4]={NULL,NULL,NULL,NULL};stride_t stride[4]={0,0,0,0};
 if (wmv9->decompress(src,srcLen,&dst[0],&stride[0])!=0 && dst[0])
  {
   if (pIn->IsPreroll()==S_OK)
    return sinkD->deliverPreroll(pIn->IsSyncPoint()?FRAME_TYPE::I:FRAME_TYPE::P);
   TffPict pict(csp,dst,stride,rd,true,pIn);
   return sinkD->deliverDecodedSample(pict);
  }
 else return S_FALSE;
}
