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
#include "TvideoCodecUncompressed.h"
#include "TcodecSettings.h"
#include "dsutil.h"
#include "IffdshowDecVideo.h"

TvideoCodecUncompressed::TvideoCodecUncompressed(IffdshowBase *Ideci,IdecVideoSink *IsinkD):
 Tcodec(Ideci),TcodecDec(Ideci,IsinkD),
 TvideoCodec(Ideci),
 TvideoCodecDec(Ideci,IsinkD),
 TvideoCodecEnc(Ideci,NULL)
{
 ok=true;
}
TvideoCodecUncompressed::TvideoCodecUncompressed(IffdshowBase *Ideci,IencVideoSink *IsinkE):
 Tcodec(Ideci),TcodecDec(Ideci,NULL),
 TvideoCodec(Ideci),
 TvideoCodecDec(Ideci,NULL),
 TvideoCodecEnc(Ideci,IsinkE)
{
 ok=true;
 encoders.push_back(new Tencoder(_l("Uncompressed"),CODEC_ID_RAW,Tfourccs(0)));
}

void TvideoCodecUncompressed::getVersion(char **vers)
{
 if (!vers) return;
 static char ver[]="Uncompressed movie source 1.0";
 *vers=ver;
}

//------------------------------- decompress -------------------------------
bool TvideoCodecUncompressed::beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags)
{
 switch (codecId)
  {
   case CODEC_ID_YV12:csp=FF_CSP_420P|FF_CSP_FLAGS_YUV_ADJ;break;
   case CODEC_ID_I420:csp=FF_CSP_420P|FF_CSP_FLAGS_YUV_ADJ|FF_CSP_FLAGS_YUV_ORDER;break;
   case CODEC_ID_YUY2:csp=FF_CSP_YUY2;break;
   case CODEC_ID_YVYU:csp=FF_CSP_YVYU;break;
   case CODEC_ID_UYVY:csp=FF_CSP_UYVY;break;
   case CODEC_ID_VYUY:csp=FF_CSP_VYUY;break;
   case CODEC_ID_RGB2:csp=FF_CSP_RGB24|FF_CSP_FLAGS_VFLIP;break;
   case CODEC_ID_RGB3:csp=FF_CSP_RGB32|FF_CSP_FLAGS_VFLIP;break;
   case CODEC_ID_RGB5:csp=FF_CSP_RGB15|FF_CSP_FLAGS_VFLIP;break;
   case CODEC_ID_RGB6:csp=FF_CSP_RGB16|FF_CSP_FLAGS_VFLIP;break;
   case CODEC_ID_BGR2:csp=FF_CSP_BGR24;break;
   case CODEC_ID_BGR3:csp=FF_CSP_BGR32;break;
   case CODEC_ID_BGR5:csp=FF_CSP_BGR15;break;
   case CODEC_ID_BGR6:csp=FF_CSP_BGR16;break;
   case CODEC_ID_CLJR:csp=FF_CSP_CLJR;break;
   case CODEC_ID_444P:csp=FF_CSP_444P;break;
   case CODEC_ID_422P:csp=FF_CSP_422P;break;
   case CODEC_ID_411P:csp=FF_CSP_411P;break;
   case CODEC_ID_410P:csp=FF_CSP_410P;break;
   case CODEC_ID_Y800:csp=FF_CSP_Y800;break;
   case CODEC_ID_NV12:csp=FF_CSP_NV12|FF_CSP_FLAGS_YUV_ORDER;break;
   case CODEC_ID_NV21:csp=FF_CSP_NV12;break;
   case CODEC_ID_YV16:csp=FF_CSP_YV16;break;
   //case CODEC_ID_PAL1:csp=FF_CSP_PAL1;break;
   //case CODEC_ID_PAL4:csp=FF_CSP_PAL4;break;
   case CODEC_ID_PAL8:csp=FF_CSP_PAL8|FF_CSP_FLAGS_VFLIP;break;
   default:return false;
  }
 cspInfo=csp_getInfo(pict.csp=csp);
 if (csp_isRGB(csp))
  stride[0]=(((pict.rectFull.dx*cspInfo->Bpp<<3)+31)&~31)>>3;
 else if (csp_isPAL(csp))
  {
   BITMAPINFOHEADER bih;
   ExtractBIH(mt,&bih);
   memset(palette,0,sizeof(palette));
   Textradata extradata(mt);
   if (extradata.data && extradata.size)
    memcpy(palette,extradata.data,std::min(extradata.size,sizeof(RGBQUAD)*size_t(1<<bih.biBitCount)));
   stride[0]=(((pict.rectFull.dx*cspInfo->Bpp<<3)+31)&~31)>>3;
  }
 else
  stride[0]=cspInfo->Bpp*pict.rectFull.dx;
 stride[1]=stride[2]=stride[3]=0;
 rd=pict.rectFull;
 return true;
}
HRESULT TvideoCodecUncompressed::decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn)
{
/*
 if (cspInfo->Bpp==1)
  stride[0]=dx;
 else
  stride[0]=srcLen/dy;
*/
 unsigned char *data[4]={(unsigned char*)src,NULL,NULL,NULL};
 for(unsigned int i=1;i<cspInfo->numPlanes;i++)
  {
   data[i]=data[i-1]+(rd.dy>>cspInfo->shiftY[i-1])*stride[i-1];
   stride[i]=stride[0]>>cspInfo->shiftX[i];
  }
 TffPict pict(csp,data,stride,rd,true,pIn,Tpalette(palette,palcolors),isInterlacedRawVideo);
 pict.frametype=FRAME_TYPE::I;
 return sinkD->deliverDecodedSample(pict);
}

//-------------------------------- compress --------------------------------
void TvideoCodecUncompressed::getCompressColorspaces(Tcsps &csps,unsigned int outDx,unsigned int outDy)
{
 for (int i=0;cspFccs[i].name;i++)
  if (cspFccs[i].supEnc && coCfg->raw_fourcc==cspFccs[i].fcc)
   {
    csps.add(cspFccs[i].csp|(cspFccs[i].flip?FF_CSP_FLAGS_VFLIP:0));
    return;
   }
}
bool TvideoCodecUncompressed::prepareHeader(BITMAPINFOHEADER *outhdr)
{
 const TcspInfo *cspInfo=csp_getInfoFcc(coCfg->raw_fourcc);
 if (!cspInfo) return false;
 outhdr->biCompression=cspInfo->fcc;
 outhdr->biBitCount=WORD(cspInfo->bpp);
 outhdr->biSizeImage=cspInfo->bpp*outhdr->biWidth*outhdr->biHeight/8;
 return true;
}
LRESULT TvideoCodecUncompressed::beginCompress(int cfgcomode,int csp,const Trect &r)
{
 cspInfo=csp_getInfo(csp);
 stride[0]=cspInfo->Bpp*r.dx;
 return ICERR_OK;
}
HRESULT TvideoCodecUncompressed::compress(const TffPict &pict,TencFrameParams &params)
{
 const unsigned char *src=pict.data[0];
 TmediaSample sample;
 HRESULT hr;
 if (FAILED(hr=sinkE->getDstBuffer(&sample,pict)))
  return hr;
 uint8_t *dst=sample;size_t dstLen=sample.size();
 if (pict.csp&FF_CSP_FLAGS_YUV_ADJ)
  for (int i=0;i<3;i++)
   for (unsigned int y=0;y<pict.rectFull.dy>>pict.cspInfo.shiftY[i];src+=pict.stride[i],dst+=stride[0]>>pict.cspInfo.shiftX[i],y++)
    memcpy(dst,src,stride[0]>>pict.cspInfo.shiftX[i]);
 else
  for (unsigned int y=0;y<pict.rectFull.dy;src+=pict.stride[0],dst+=stride[0],y++)
   memcpy(dst,src,stride[0]);
 params.frametype=FRAME_TYPE::I;params.keyframe=true;
 params.length=(unsigned int)dstLen;
 return sinkE->deliverEncodedSample(sample,params);
}
