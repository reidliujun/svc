/*
 * Copyright (c) 2002-2006 Milan Cutka
 * algorithms based on VirtualDub and AviSynth filters by Donald A. Graft
 * asharp by Marc Fauconneau
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
#include "TimgFilterSharpen.h"
#include "TsharpenSettings.h"
#include "IffdshowBase.h"
#include "T3x3blur.h"
#include "Tconfig.h"
#include "Tlibmplayer.h"
#include "postproc/swscale.h"
#include "simd.h"

//==================================== TimgFilterAsharp ====================================
TimgFilterAsharp::TimgFilterAsharp(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 asharp_run=getAsharp();
 aline=NULL;
 oldAsharpT=INT_MAX;
}
void TimgFilterAsharp::done(void)
{
 if (aline) aligned_free(aline);aline=NULL;
}

bool TimgFilterAsharp::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TsharpenSettings *cfg=(const TsharpenSettings*)cfg0;
 return super::is(pict,cfg) && cfg->asharpT;
}

HRESULT TimgFilterAsharp::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TsharpenSettings *cfg=(const TsharpenSettings*)cfg0;
   if (oldAsharpT!=cfg->asharpT || oldAsharpD!=cfg->asharpD || oldAsharpB!=cfg->asharpB || oldAsharpHQBF!=cfg->asharpHQBF)
    {
     oldAsharpT=cfg->asharpT;oldAsharpD=cfg->asharpD;oldAsharpB=cfg->asharpB;oldAsharpHQBF=cfg->asharpHQBF;
     //TODO: remove comparisons - it shouldn't be possible to enter too small/large parameter
     double t=oldAsharpT/100.0;
     asharpCfgT=(int)(t*(4<<7));
     asharpCfgT=limit(asharpCfgT,int(-(4<<7)),int(32*(4<<7))); // yes, negatives values are accepted

     double d=oldAsharpD/100.0;
     asharpCfgD=(int)(d*(4<<7));
     asharpCfgD=limit(asharpCfgD,0,int(16*(4<<7)));

     if (oldAsharpB==0)
      asharpCfgB=asharpCfgB2=256;
     else
      {
       double b=oldAsharpB/100.0;
       asharpCfgB=(int)(256-b*64);
       asharpCfgB=limit(asharpCfgB,0,256);
       asharpCfgB2=(int)(256-b*48);
       asharpCfgB2=limit(asharpCfgB2,0,256);
      }
    }

   init(pict,cfg->full,cfg->half);
   if (!aline) aline=(unsigned char*)aligned_malloc(pictRect.dx*2);
   unsigned char *dst;
   getCurNext(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,COPYMODE_DEF,&dst,NULL,NULL,NULL);

   unsigned char *lastX2,*lastX1;
   if (oldAsharpHQBF)
    {
     lastX2=(unsigned char*)_alloca(dy1[0]);lastX1=(unsigned char*)_alloca(dy1[0]);
     for (unsigned int y=0;y<dy1[0];y++)
      {
       lastX2[y]=dst[y*stride2[0]+dx1[0]-2];
       lastX1[y]=dst[y*stride2[0]+dx1[0]-1];
      }
    }
   else
    lastX2=lastX1=NULL;

   asharp_run(dst,(int)stride2[0],dy1[0],dx1[0],asharpCfgT,asharpCfgD,asharpCfgB,asharpCfgB2,oldAsharpHQBF,aline);
   _mm_empty();

   if (oldAsharpHQBF)
    for (unsigned int y=0;y<dy1[0];y++)
     {
      dst[y*stride2[0]+dx1[0]-2]=lastX2[y];
      dst[y*stride2[0]+dx1[0]-1]=lastX1[y];
     }
  }
 return parent->deliverSample(++it,pict);
}

//==================================== TimgFilterMplayerSharp ====================================
TimgFilterMplayerSharp::TimgFilterMplayerSharp(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 libmplayer=NULL;swsc=NULL;swsf=NULL;oldmplayersharpenluma=oldmplayersharpenchroma=-1;
}
TimgFilterMplayerSharp::~TimgFilterMplayerSharp()
{
 if (libmplayer) libmplayer->Release();
}
void TimgFilterMplayerSharp::done(void)
{
 if (swsf) libmplayer->sws_freeFilter(swsf);swsf=NULL;
 if (swsc) libmplayer->sws_freeContext(swsc);swsc=NULL;
}

bool TimgFilterMplayerSharp::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TsharpenSettings *cfg=(const TsharpenSettings*)cfg0;
 return super::is(pict,cfg) && (cfg->mplayerLuma || cfg->mplayerChroma);
}

HRESULT TimgFilterMplayerSharp::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TsharpenSettings *cfg=(const TsharpenSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   bool cspChanged=false;
   const unsigned char *src[4];
   cspChanged|=getCur(SWS_IN_CSPS,pict,cfg->full,src);
   unsigned char *dst[4];
   cspChanged|=getNext(SWS_OUT_CSPS,pict,cfg->full,dst);
   if (cspChanged) done();

   if (!swsc || oldmplayersharpenluma!=cfg->mplayerLuma || oldmplayersharpenchroma!=cfg->mplayerChroma)
    {
     oldmplayersharpenluma=cfg->mplayerLuma;oldmplayersharpenchroma=cfg->mplayerChroma;
     if (!libmplayer) deci->getPostproc(&libmplayer);
     done();
     swsf=libmplayer->sws_getDefaultFilter(0,0,oldmplayersharpenluma/100.0f,oldmplayersharpenchroma/100.0f,0,0,0);
     SwsParams params;Tlibmplayer::swsInitParams(&params,0);
     swsc=libmplayer->sws_getContext(dx1[0],dy1[0],csp_ffdshow2mplayer(csp1),dx1[0],dy1[0],csp_ffdshow2mplayer(csp2),&params,swsf,NULL,NULL);
    }

   if (swsc)
    libmplayer->sws_scale_ordered(swsc,src,stride1,0,dy1[0],dst,stride2);
  }
 return parent->deliverSample(++it,pict);
}

//==================================== TimgFilterUnsharp ====================================
TimgFilterUnsharp::TimgFilterUnsharp(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 Ysum=NULL;
#ifdef __SSE2__
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  unsharpFc=&TimgFilterUnsharp::unsharp<Tsse2>;
 else
#endif
  unsharpFc=&TimgFilterUnsharp::unsharp<Tmmx>;
}
void TimgFilterUnsharp::done(void)
{
 if (Ysum) aligned_free(Ysum);Ysum=NULL;
}

bool TimgFilterUnsharp::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TsharpenSettings *cfg=(const TsharpenSettings*)cfg0;
 return super::is(pict,cfg) && cfg->unsharpStrength;
}

template<class _mm> void TimgFilterUnsharp::unsharp(const TsharpenSettings *cfg,const unsigned char *src,unsigned char *dst,unsigned short *sum)
{
 static const int C_SCALE=32;
 typename _mm::__m div9_64=_mm::set1_pi16(7281),m0=_mm::setzero_si64();
 //typename _mm::__m T64=_mm::set1_pi16((short)cfg->unsharpThreshold);
 typename _mm::__m C64=_mm::set1_pi16(short(cfg->unsharpStrength+C_SCALE)); //strength=0..250
 src+=stride1[0];dst+=stride2[0];sum+=minStride;
 for (const unsigned char *srcEnd=src+(dy1[0]-2)*stride1[0];src!=srcEnd;src+=stride1[0],sum+=minStride,dst+=stride2[0])
  {
   for (unsigned int x=0;x<dx1[0]-1;x+=_mm::size/2)
    {
     typename _mm::__m mm0=_mm::unpacklo_pi8(_mm::load2(src+stride1[0]+x  ),m0);
     typename _mm::__m mm1=_mm::unpacklo_pi8(_mm::load2(src+stride1[0]+x+1),m0);
     typename _mm::__m mm2=_mm::unpacklo_pi8(_mm::load2(src+stride1[0]+x+2),m0);
     _mm::storeU(sum+minStride+1+x,_mm::add_pi16(mm0,_mm::add_pi16(mm1,mm2)));
    }
   for (unsigned int x=0;x<dx1[0]-1;x+=_mm::size/2)
    {
     typename _mm::__m y_sum=_mm::mulhi_pi16(_mm::add_pi16(_mm::add_pi16(*(typename _mm::__m*)(sum+x),*(typename _mm::__m*)(sum+minStride+x)),*(typename _mm::__m*)(sum-minStride+x)),div9_64);
     typename _mm::__m diff=_mm::sub_pi16(_mm::unpacklo_pi8(_mm::load2(src+x),m0),y_sum);
     diff=_mm::adds_pi16(_mm::srai_pi16(_mm::mullo_pi16(diff,C64),5),y_sum);
     _mm::store2(dst+x,_mm::packs_pu16(diff,m0));
    }
  }
 _mm::empty();
}

HRESULT TimgFilterUnsharp::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TsharpenSettings *cfg=(const TsharpenSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   if (!Ysum)
    {
     minStride=(pictRect.dx/16+2)*16;
     Ysum=(unsigned short*)aligned_malloc(minStride*pictRect.dy*2);
    }

   const unsigned char *src_;
   unsigned char *dst_;
   getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&src_,NULL,NULL,NULL);
   getNext(csp1,pict,cfg->full,&dst_,NULL,NULL,NULL);
   const unsigned char *src=src_;
   unsigned char *dst=dst_;

   unsigned short *sum=Ysum;
   for (unsigned int x=1;x<dx1[0]-1;x++)
    {
     sum[x          ]=(unsigned short)(src[x-1           ]+src[x           ]+src[x+1           ]);
     sum[x+minStride]=(unsigned short)(src[x-1+stride1[0]]+src[x+stride1[0]]+src[x+1+stride1[0]]);
    }
   (this->*unsharpFc)(cfg,src,dst,sum);
   const unsigned char *srcL=src_+stride1[0],*srcR=srcL+dx1[0]-1;unsigned char *dstL=dst_+stride2[0],*dstR=dstL+dx1[0]-1;
   for (unsigned int y=1;y<dy1[0]-1;srcL+=stride1[0],srcR+=stride1[0],dstL+=stride2[0],dstR+=stride2[0],y++)
    {
     *dstL=*srcL;
     *dstR=*srcR;
    }
   memcpy(dst_,src_,dx1[0]);
   memcpy(dst_+stride2[0]*(dy1[0]-2),src_+stride1[0]*(dy1[0]-2),dx1[0]);
   memcpy(dst_+stride2[0]*(dy1[0]-1),src_+stride1[0]*(dy1[0]-1),dx1[0]);
  }
 return parent->deliverSample(++it,pict);
}

//==================================== TimgFilterXsharp =====================================
TimgFilterXsharp::TimgFilterXsharp(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 Ymin=Ymax=NULL;
#ifdef __SSE2__
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  xsharpenFc=&TimgFilterXsharp::xsharpen<Tsse2>;
 else
#endif
 if (Tconfig::cpu_flags&FF_CPU_MMXEXT)
  xsharpenFc=&TimgFilterXsharp::xsharpen<Tmmxext>;
 else
  xsharpenFc=&TimgFilterXsharp::xsharpen<Tmmx>;
}
void TimgFilterXsharp::done(void)
{
 if (Ymin) aligned_free(Ymin);Ymin=NULL;
 if (Ymax) aligned_free(Ymax);Ymax=NULL;
}

template<class _mm> void TimgFilterXsharp::xsharpen(unsigned int dx1,unsigned int dy1,const TsharpenSettings *cfg,const unsigned char *srcY,stride_t stride1,unsigned char *dstY,stride_t stride2)
{
 unsigned int dx8=(dx1/8)*8;
 const unsigned char *src=srcY,*srcEnd;
 unsigned char *min=Ymin,*max=Ymax;
 for (srcEnd=src+dy1*stride1;src!=srcEnd;src+=stride1,min+=minStride,max+=minStride)
  for (unsigned int x=1;x<dx8;x+=8)
   {
    *(__m64*)(min+x)=_mm::T64::min_pu8(_mm::T64::min_pu8(*(__m64*)(src+x-1),*(__m64*)(src+x)),*(__m64*)(src+x+1));
    *(__m64*)(max+x)=_mm::T64::max_pu8(_mm::T64::max_pu8(*(__m64*)(src+x-1),*(__m64*)(src+x)),*(__m64*)(src+x+1));
   }
 src=srcY+stride1;
 min=Ymin+minStride;max=Ymax+minStride;
 unsigned char *dst=dstY+stride2;
 typename _mm::__m strength64=_mm::set1_pi16((short)cfg->xsharpStrength);
 typename _mm::__m strengthInv64=_mm::set1_pi16((short)(127-cfg->xsharpStrength));
 typename _mm::__m thresh64=_mm::set1_pi16((short)cfg->xsharpThreshold);
 typename _mm::__m m0=_mm::setzero_si64(),m255=_mm::set1_pi8(-1);
 for (srcEnd=src+(dy1-2)*stride1;src!=srcEnd;src+=stride1,dst+=stride2,min+=minStride,max+=minStride)
  for (unsigned int x=1;x<dx8;x+=_mm::size/2)
   {
    typename _mm::__m mm1=_mm::min_pi16(_mm::min_pi16(_mm::unpacklo_pi8(_mm::load2(min+x-minStride),m0),_mm::unpacklo_pi8(_mm::load2(min+x),m0)),_mm::unpacklo_pi8(_mm::load2(min+x+minStride),m0));
    typename _mm::__m mm3=_mm::max_pi16(_mm::max_pi16(_mm::unpacklo_pi8(_mm::load2(max+x-minStride),m0),_mm::unpacklo_pi8(_mm::load2(max+x),m0)),_mm::unpacklo_pi8(_mm::load2(max+x+minStride),m0));
    typename _mm::__m mm0=_mm::unpacklo_pi8(_mm::load2(src+x),m0);
    typename _mm::__m mm2=_mm::sub_pi16(mm0,mm1),mm4=_mm::sub_pi16(mm3,mm0);
    typename _mm::__m mm5=_mm::cmpgt_pi16(mm2,mm4);
    typename _mm::__m mm6=_mm::and_si64(_mm::cmpgt_pi16(thresh64,mm4),mm5);
    typename _mm::__m mm7=_mm::and_si64(mm6,mm3);
    mm4=_mm::cmpgt_pi16(thresh64,mm2);
    mm5=_mm::xor_si64(mm5,m255);
    mm4=_mm::and_si64(mm4,mm5);
    mm1=_mm::and_si64(mm1,mm4);
    mm7=_mm::mullo_pi16(_mm::or_si64(mm7,mm1),strength64);
    mm4=_mm::or_si64(mm4,mm6);
    mm6=_mm::mullo_pi16(mm0,strengthInv64);
    mm7=_mm::and_si64(_mm::srli_pi16(_mm::add_pi16(mm7,mm6),7),mm4);
    _mm::store2(dst+x,_mm::packs_pu16(_mm::or_si64(_mm::and_si64(mm0,_mm::xor_si64(mm4,m255)),mm7),m0));
   }
 _mm::empty();
 src=srcY+stride1;dst=dstY+stride2;
 memcpy(dstY,srcY,dx1);
 memcpy(dstY+(dy1-1)*stride2,srcY+(dy1-1)*stride1,dx1);
 for (unsigned int y=1;y<dy1-1;src+=stride1,dst+=stride2,y++)
  *dst=*src;
 if (dx1&7)
  TffPict::copy(dstY+dx8,stride2,srcY+dx8,stride1,dx1&7,dy1);
 _mm_empty();
}

bool TimgFilterXsharp::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TsharpenSettings *cfg=(const TsharpenSettings*)cfg0;
 return super::is(pict,cfg) && cfg->xsharpStrength;
}

HRESULT TimgFilterXsharp::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TsharpenSettings *cfg=(const TsharpenSettings*)cfg0;
   init(pict,cfg->full,cfg->half);

   if (!Ymin)
    {
     minStride=(pictRect.dx/16+2)*16;
     Ymin=(unsigned char*)aligned_malloc(minStride*pictRect.dy*2);
     Ymax=(unsigned char*)aligned_malloc(minStride*pictRect.dy);
    }

   const unsigned char *src;
   unsigned char *dst;
   getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&src,NULL,NULL,NULL);
   getNext(csp1,pict,cfg->full,&dst,NULL,NULL,NULL);

   (this->*xsharpenFc)(dx1[0],dy1[0],cfg,src,stride1[0],dst,stride2[0]);
  }
 return parent->deliverSample(++it,pict);
}

//==================================== TimgFilterMsharp =====================================
TimgFilterMsharp::TimgFilterMsharp(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 blur=work=NULL;blur3x3=NULL;
}
void TimgFilterMsharp::done(void)
{
 if (blur) aligned_free(blur);blur=NULL;
 if (work) aligned_free(work);work=NULL;
 if (blur3x3) delete blur3x3;blur3x3=NULL;
}

bool TimgFilterMsharp::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TsharpenSettings *cfg=(const TsharpenSettings*)cfg0;
 return super::is(pict,cfg) && cfg->msharpStrength;
}

HRESULT TimgFilterMsharp::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TsharpenSettings *cfg=(const TsharpenSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   const unsigned char *src_;
   unsigned char *dst_;
   getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&src_,NULL,NULL,NULL);
   getNext(csp1,pict,cfg->full,&dst_,NULL,NULL,NULL);

   const unsigned char *src=src_;unsigned char *dst=dst_;
   int strength=cfg->msharpStrength*2,invstrength=255-strength;
   const unsigned char *srcp=src;
   const unsigned char *srcp_saved=srcp;

   if (!blur3x3) blur3x3=new T3x3blurSWS(deci,dx1[0],dy1[0]);
   if (!blur)
    {
     stride=(dx1[0]/16+2)*16;
     blur=(unsigned char*)aligned_malloc(stride*dy1[0]);
     work=(unsigned char*)aligned_malloc(stride*dy1[0]);
    }
   unsigned char *blurp=blur;
   const unsigned char *blurpn;
   unsigned char *workp=work;
   unsigned char *dstp=dst;

   int b1,b2,b4;

   blur3x3->process(src,stride1[0],blur,stride);

   blurp=blur;
   blurpn=blurp+stride;
   dstp=dst;
   // Diagonal detail detection.
  /*
   int b3;
   b1=blurp[0];
   b3=blurpn[0];
   for (y=0;y<dy1[0]-1;dstp+=stride2[0],blurp+=m.stride,blurpn+=m.stride,y++)
    for (x=0;x<dx1[0]-1;x++)
     {
      b2=blurp[x+1];
      b4=blurpn[x+1];
      if (abs(b1-b4)>=cfg->msharpThreshold || abs(b2-b3)>=cfg->msharpThreshold)
       dstp[x]=255;
      else
       dstp[x]=0;
      b1=b2;b3=b4;
     }
  */
   __m64 m255=_mm_set1_pi8(-1),m0=_mm_setzero_si64();
   __m64 threshold64=_mm_set1_pi8((char)std::max(1,cfg->msharpThreshold-1));
   for (unsigned int y=0;y<dy1[0]-1;blurp+=stride,blurpn+=stride,dstp+=stride2[0],y++)
    for (unsigned int x=0;x<dx1[0]-1;x+=8)
     {
      __m64 mm1=*(__m64*)(blurp +x+1);
      __m64 mm2=*(__m64*)(blurpn+x+1);
      __m64 mm3=*(__m64*)(blurpn+x-1);
      mm2=_mm_subs_pu8(_mm_absdif_u8(mm2,mm1),threshold64);
      mm3=_mm_subs_pu8(_mm_absdif_u8(mm3,mm1),threshold64);
      mm2=_mm_xor_si64(_mm_cmpeq_pi8(_mm_or_si64(mm2,mm3),m0),m255);
      *(__m64*)(dstp+x+1)=mm2;
     }

   if (cfg->msharpHQ)
    {
     // Vertical detail detection
     for (unsigned int x=0;x<dx1[0];x++)
      {
       blurp=blur;
       blurpn=blurp+stride;
       dstp=dst;
       b1=blurp[x];
       for (unsigned int y=0;y<dy1[0]-1;dstp+=stride2[0],blurp+=stride,blurpn+=stride,y++)
        {
         b2=blurpn[x];
         if (abs(b1-b2)>=cfg->msharpThreshold)
          dstp[x]=255;
         b1=b2;
        }
      }

     // Horizontal detail detection
     blurp=blur;
     dstp=dst;
     for (unsigned int y=0;y<dy1[0];dstp+=stride2[0],blurp+=stride,y++)
      {
       b1=blurp[0];
       for (unsigned int x=0;x<dx1[0]-1;x+=1)
        {
         b2=blurp[x+1];
         if (abs(b1-b2)>=cfg->msharpThreshold)
          dstp[x]=255;
         b1=b2;
        }
      }
    }

   // Fix up detail map borders
   dstp=dst;
   memset(dstp,0,dx1[0]);
   memset(dstp+stride2[0],0,dx1[0]);
   memset(dstp+(dy1[0]-2)*stride2[0],0,dx1[0]);
   memset(dstp+(dy1[0]-1)*stride2[0],0,dx1[0]);
   for (unsigned int y=0;y<dy1[0];dstp+=stride2[0],y++)
    {
     dstp[0]=0;
     dstp[1]=0;
     dstp[dx1[0]-1]=0;
     dstp[dx1[0]-2]=0;
    }

   if (!cfg->msharpMask)
    {
     // Fix up output frame borders
     srcp=srcp_saved;
     workp=work;
     memcpy(workp,srcp,dx1[0]);
     memcpy(workp+(dy1[0]-1)*stride,srcp+(dy1[0]-1)*stride1[0],dx1[0]);
     for (unsigned int y=0;y<dy1[0];srcp+=stride1[0],workp+=stride,y++)
      {
       workp[0]=srcp[0];
       workp[dx1[0]-1]=srcp[dx1[0]-1];
      }

     // Now sharpen the edge areas and we're done
     srcp=srcp_saved+stride1[0];
     dstp=dst+stride2[0];
     blurp=blur+stride;
     for (unsigned int y=1;y<dy1[0]-1;srcp+=stride1[0],dstp+=stride2[0],blurp+=stride,y++)
      {
       for (unsigned int x=1;x<dx1[0]-1;)
        {
         if (*(int*)(dstp+x)==0)
          {
           *(int*)(dstp+x)=*(int*)(srcp+x);
           x+=4;
           continue;
          }
         if (dstp[x])
          {
           b4=4*int(srcp[x])-3*int(blurp[x]);
           if (b4<0) b4=0; else if (b4>255) b4=255;
           dstp[x]=(unsigned char)((strength*b4+invstrength*srcp[x])>>8);
          }
         else
          dstp[x]=srcp[x];
         x++;
        }
       dstp[0]=srcp[0];
       dstp[dx1[0]-1]=srcp[dx1[0]-1];
      }
     memcpy(dst,src,dx1[0]);
     memcpy(dst+(dy1[0]-1)*stride2[0],src+(dy1[0]-1)*stride1[0],dx1[0]);
    }
   _mm_empty();
  }
 return parent->deliverSample(++it,pict);
}
