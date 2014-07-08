/*
 * Copyright (c) 2002-2006 Milan Cutka
 * warpsharp idea by Avery Lee, implementation based on Warpsharp filter for AviSynth by ...
 * aWarpSharp by Marc Fauconneau
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
#include "TimgFilterWarpsharp.h"
#include "TwarpsharpSettings.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "T3x3blur.h"
#include "Tconfig.h"
#include "Tlibmplayer.h"
#include "postproc/swscale.h"
#include "simd.h"

//=========================================== TimgFilterWarpsharp ===========================================
TimgFilterWarpsharp::TimgFilterWarpsharp(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 blur=NULL;
 swsblur=NULL;
 libmplayer=NULL;
#ifdef __SSE2__
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  warpsharpbumpFc=warpsharpbump<Tsse2>;
 else
#endif
  warpsharpbumpFc=warpsharpbump<Tmmx>;
}
TimgFilterWarpsharp::~TimgFilterWarpsharp()
{
 if (libmplayer) libmplayer->Release();
}

void TimgFilterWarpsharp::done(void)
{
 if (blur)
  {
   aligned_free(blur);
   blur=NULL;
  }
 if (swsblur) libmplayer->sws_freeContext(swsblur);swsblur=NULL;
}

template<class _mm> void TimgFilterWarpsharp::warpsharpbump(const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,unsigned int dxY,unsigned int dyY,int wsthresh)
{
 memset(dst,0,dxY);src+=srcStride;dst+=dstStride;
 typename _mm::__m m0=_mm::setzero_si64(),m3=_mm::set1_pi16(3);
 typename _mm::__m wsthresh64=_mm::set1_pi16(short(0xffff-wsthresh));
 for (const unsigned char *srcEnd=src+(dyY-2)*srcStride;src!=srcEnd;src+=srcStride,dst+=dstStride)
  {
   dst[0]=0;dst[dxY-1]=0;
   for (unsigned int x=1;x<dxY-1;x+=_mm::size/2)
    {
     typename _mm::__m dx=_mm::unpacklo_pi8(_mm::load2(src+x-srcStride-1),m0);
     dx=_mm::add_pi16( dx,_mm::unpacklo_pi8(_mm::load2(src+x          -1),m0));
     dx=_mm::add_pi16( dx,_mm::unpacklo_pi8(_mm::load2(src+x-srcStride-1),m0));
     dx=_mm::sub_pi16( dx,_mm::unpacklo_pi8(_mm::load2(src+x+srcStride+1),m0));
     dx=_mm::sub_pi16( dx,_mm::unpacklo_pi8(_mm::load2(src+x          +1),m0));
     dx=_mm::sub_pi16( dx,_mm::unpacklo_pi8(_mm::load2(src+x+srcStride+1),m0));
     dx=abs_16<_mm>(dx);

     typename _mm::__m dy=_mm::unpacklo_pi8(_mm::load2(src+x-srcStride-1),m0);
     dy=_mm::add_pi16( dy,_mm::unpacklo_pi8(_mm::load2(src+x-srcStride  ),m0));
     dy=_mm::add_pi16( dy,_mm::unpacklo_pi8(_mm::load2(src+x-srcStride+1),m0));
     dy=_mm::sub_pi16( dy,_mm::unpacklo_pi8(_mm::load2(src+x+srcStride-1),m0));
     dy=_mm::sub_pi16( dy,_mm::unpacklo_pi8(_mm::load2(src+x+srcStride  ),m0));
     dy=_mm::sub_pi16( dy,_mm::unpacklo_pi8(_mm::load2(src+x+srcStride+1),m0));
     dy=abs_16<_mm>(dy);

     typename _mm::__m level=_mm::add_pi16(_mm::mullo_pi16(_mm::add_pi16(dx,dy),m3),absdif_s16<_mm>(dx,dy));
     level=_mm::adds_pu16(level,wsthresh64);
     level=_mm::subs_pu16(level,wsthresh64);
     _mm::store2(dst+x,_mm::packs_pu16(level,m0));
    }
  }
 _mm::empty();
 memset(dst,0,dxY);
}
void TimgFilterWarpsharp::warpsharpblur(const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,unsigned int dx,unsigned int dy)
{
 if (swsblur)
  libmplayer->sws_scale_ordered(swsblur,&src,&srcStride,0,dy,&dst,&dstStride);
 else
  TffPict::copy(dst,dstStride,src,srcStride,dx,dy);
}

void TimgFilterWarpsharp::warpsharp(const unsigned char *src,unsigned char *dst,const TwarpsharpSettings *cfg)
{
 int wsdepth=cfg->warpsharpDepth;

 warpsharpbumpFc(src,stride1[0],dst,stride2[0],dx1[0],dy1[0],cfg->warpsharpThreshold);
 if (!blur)
  {
   blurworkStride=(dx1[0]/16+2)*16;
   blur=(unsigned char*)aligned_malloc(blurworkStride*dy1[0]);
  }
 if (!swsblur)
  {
   SwsFilter swsf;
   swsf.lumV=swsf.lumH=libmplayer->sws_getConstVec(1/5.0,5);
   swsf.lumV->coeff[0]=1;
   swsf.lumV->coeff[1]=4;
   swsf.lumV->coeff[2]=6;
   swsf.lumV->coeff[3]=4;
   swsf.lumV->coeff[4]=1;
   libmplayer->sws_normalizeVec(swsf.lumV,1);
   swsf.chrH=swsf.chrV=NULL;
   SwsParams params;Tlibmplayer::swsInitParams(&params,0);
   swsblur=libmplayer->sws_getContext(dx1[0],dy1[0],IMGFMT_Y800,dx1[0],dy1[0],IMGFMT_Y800,&params,&swsf,NULL,NULL);
   libmplayer->sws_freeVec(swsf.lumV);
  }
 warpsharpblur(dst,stride2[0],blur,blurworkStride,dx1[0],dy1[0]);

 memcpy(dst,src,dx1[0]);dst+=stride2[0];src+=stride1[0];
 memcpy(dst,src,dx1[0]);dst+=stride2[0];src+=stride1[0];
 src+=stride1[0];
 for (const unsigned char *srcEnd=src+(dy1[0]-6)*stride1[0],*bum=blur+blurworkStride*2;src!=srcEnd;src+=stride1[0],dst+=stride2[0],bum+=blurworkStride)
  {
   dst[0]=src[0];dst[1]=src[1];
   dst[dx1[0]-2]=src[dx1[0]-2];dst[dx1[0]-1]=src[dx1[0]-1];
   for (unsigned int x=2;x<dx1[0]-2;x++)
    {
     int dispx=bum[x-1]-bum[x+1];
     int dispy=bum[x-blurworkStride]-bum[x+blurworkStride];
     dispx=dispx*wsdepth+512;dispy=dispy*wsdepth+512;
     if (dispx<-2*1024) dispx=-2*1024; else if (dispx>2*1024) dispx=2*1024;
     if (dispy<-2*1024) dispy=-2*1024; else if (dispy>2*1024) dispy=2*1024;
     int qx=(dispx>>2)&255,qy=(dispy>>2)&255;
     dispx>>=10;dispy>>=10;
     dst[x]=warpcubic.interpolate(src+x+stride1[0]*(dispy-1)+(dispx-1)+1,stride1[0],qx,qy);
    }
  }
 _mm_empty();
 memcpy(dst,src,dx1[0]);src+=stride1[0];dst+=stride2[0];
 memcpy(dst,src,dx1[0]);src+=stride1[0];dst+=stride2[0];
 memcpy(dst,src,dx1[0]);
}

bool TimgFilterWarpsharp::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TwarpsharpSettings *cfg=(const TwarpsharpSettings*)cfg0;
 return super::is(pict,cfg0) && cfg->warpsharpDepth;
}

HRESULT TimgFilterWarpsharp::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TwarpsharpSettings *cfg=(const TwarpsharpSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   if (!libmplayer) deci->getPostproc(&libmplayer);
   const unsigned char *srcY;
   getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&srcY,NULL,NULL,NULL);
   unsigned char *dstY;
   getNext(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&dstY,NULL,NULL,NULL);
   warpsharp(srcY,dstY,cfg);
  }
 return parent->deliverSample(++it,pict);
}

//=========================================== TimgFilterAwarpsharp ===========================================
TimgFilterAwarpsharp::TimgFilterAwarpsharp(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 memset(&aws,0,sizeof(aws));
}
void TimgFilterAwarpsharp::done(void)
{
 if (aws.work.yplane.ptr)
  {
   aligned_free(aws.work.yplane.ptr);
   aws.work.yplane.ptr=NULL;
  }
}

bool TimgFilterAwarpsharp::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TwarpsharpSettings *cfg=(const TwarpsharpSettings*)cfg0;
 return super::is(pict,cfg0) && cfg->warpsharpDepth;
}

HRESULT TimgFilterAwarpsharp::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 #if !defined(__GNUC__) && !defined(WIN64)
 if (is(pict,cfg0))
  {
   const TwarpsharpSettings *cfg=(const TwarpsharpSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   getCur(FF_CSP_420P|FF_CSP_FLAGS_YUV_ADJ,pict,cfg->full,(const unsigned char**)&aws.src.yplane.ptr,(const unsigned char**)&aws.src.uplane.ptr,(const unsigned char**)&aws.src.vplane.ptr,NULL);
   getNext(csp1|FF_CSP_FLAGS_YUV_ADJ,pict,cfg->full,&aws.dst.yplane.ptr,&aws.dst.uplane.ptr,&aws.dst.vplane.ptr,NULL);
   if (!aws.work.yplane.ptr)
    {
     aws.work.yplane.width=aws.work.yplane.pitch=dx1[0];aws.work.yplane.height=dy1[0];aws.work.yplane.ptr=(unsigned char*)aligned_calloc(dx1[0]*dy1[0]*pict.cspInfo.bpp/8,1);
     aws.work.uplane.width=aws.work.uplane.pitch=dx1[1];aws.work.uplane.height=dy1[1];aws.work.uplane.ptr=aws.work.yplane.ptr+dx1[0]*dy1[0];
     aws.work.vplane.width=aws.work.vplane.pitch=dx1[2];aws.work.vplane.height=dy1[2];aws.work.vplane.ptr=aws.work.uplane.ptr+dx1[1]*dy1[1];
     pict.clear(cfg->full);
    }
   aws.blurlevel=cfg->awarpsharpBlur;
   aws.cm=cfg->awarpsharpCM;
   aws.bm=cfg->awarpsharpBM;
   aws.depth=(int)((cfg->awarpsharpDepth/100.0)*512*aws.blurlevel/4.0+0.5);
   aws.thresh=(int)((cfg->awarpsharpThresh/100.0)*256+0.5);
   aws.src.yplane.width=dx1[0];aws.src.yplane.height=dy1[0];aws.src.uplane.width=dx1[1];aws.src.uplane.height=dy1[1];aws.src.vplane.width=dx1[2];aws.src.vplane.height=dy1[2];aws.src.yplane.pitch=stride1[0];aws.src.uplane.pitch=stride1[1];aws.src.vplane.pitch=stride1[2];
   aws.dst.yplane.width=dx1[0];aws.dst.yplane.height=dy1[0];aws.dst.uplane.width=dx1[1];aws.dst.uplane.height=dy1[1];aws.dst.vplane.width=dx1[2];aws.dst.vplane.height=dy1[2];aws.dst.yplane.pitch=stride2[0];aws.dst.uplane.pitch=stride2[1];aws.dst.vplane.pitch=stride2[2];
   aws_run(&aws);
   _mm_empty();
  }
 #endif
 return parent->deliverSample(++it,pict);
}
