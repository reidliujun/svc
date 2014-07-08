/*
 * Copyright (c) 2002-2006 Milan Cutka
 * motion compensated frame rate doubling by Tom Barry
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
#include "TimgFilterDeinterlace.h"
#include "TdeinterlaceSettings.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "Tlibmplayer.h"
#include "Tconfig.h"
#include "simd.h"

//============================================= TimgFilterDeinterlace =============================================
TimgFilterDeinterlace::TimgFilterDeinterlace(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
}
HRESULT TimgFilterDeinterlace::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TdeinterlaceSettings *cfg=(const TdeinterlaceSettings*)cfg0;
 if ((pict.fieldtype & FIELD_TYPE::PROGRESSIVE_FRAME) && !cfg->deinterlaceAlways)
 {
  return parent->deliverSample(++it,pict);
 }
 if (cfg->swapfields)
  if (pict.fieldtype&FIELD_TYPE::INT_TFF)
   pict.fieldtype=(pict.fieldtype&~FIELD_TYPE::INT_TFF)|FIELD_TYPE::INT_BFF;
  else if (pict.fieldtype&FIELD_TYPE::INT_BFF)
   pict.fieldtype=(pict.fieldtype&~FIELD_TYPE::INT_BFF)|FIELD_TYPE::INT_TFF;

 if (pict.rectClip != pict.rectFull)
  parent->dirtyBorder=1;

 return parent->deliverSample(++it,pict);
}

//===================================== TimgFilterFramerateDoubler ====================================
TimgFilterFramerateDoubler::TimgFilterFramerateDoubler(IffdshowBase *Ideci,Tfilters *Iparent):
 TimgFilter(Ideci,Iparent),
 old(NULL)
{
 if (Tconfig::cpu_flags&FF_CPU_MMXEXT)
  {
   interpolateMotion=Tinterpolate<Tmmxext>::interpolateMotion;
   pictInterpolate=Tinterpolate<Tmmxext>::pictInterpolate;
  }
 else
  {
   interpolateMotion=Tinterpolate<Tmmx>::interpolateMotion;
   pictInterpolate=Tinterpolate<Tmmx>::pictInterpolate;
  }
#ifdef __SSE2__
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  pictInterpolate=Tinterpolate<Tsse2>::pictInterpolate;
#endif
}
void TimgFilterFramerateDoubler::done(void)
{
 if (old) delete old;old=NULL;
}

template<class Tsimd> void TimgFilterFramerateDoubler::Tinterpolate<Tsimd>::pictInterpolate(unsigned char *dst,stride_t dstStride,const unsigned char *src1,stride_t src1stride,const unsigned char *src2,stride_t src2stride,unsigned int dx,unsigned int dy)
{
 if (Tsimd::align && (intptr_t(src1)&15 || intptr_t(src2)&15 || intptr_t(dst)&15 || src1stride&15 || src2stride&15 || dstStride&15))
  {
   Tinterpolate<typename Tsimd::T64>::pictInterpolate(dst,dstStride,src1,src1stride,src2,src2stride,dx,dy);
   return;
  }
 for (unsigned int y=0;y<dy;y++,src1+=src1stride,src2+=src2stride,dst+=dstStride)
  {
   int x;
   for (x=0;x<int(dx-Tsimd::size*2+1);x+=Tsimd::size*2)
    {
     typename Tsimd::__m dest1,dest2;
     movq(dest1,src1+x);movq(dest2,src1+x+Tsimd::size);
     Tsimd::pavgb(dest1,src2+x);Tsimd::pavgb(dest2,src2+x+Tsimd::size);
     movq(dst+x,dest1);movq(dst+x+Tsimd::size,dest2);
    }
   for (;x<int(dx);x++)
    dst[x]=uint8_t((src1[x]+src2[x]+1)/2);
  }
}

template<class Tsimd> inline void TimgFilterFramerateDoubler::Tinterpolate<Tsimd>::bestInterp(__m64 &mm5,__m64 &mm6,const __m64 *paddr1,const __m64 *paddr2)
{
 __m64 m0=_mm_setzero_si64();
 __m64 mm0=*paddr1; // our 4 pixels
 __m64 mm1=*paddr2; // our pixel2 value

 __m64 mm2=mm0;     // another copy of our pixel1 value

 Tsimd::pminub(mm0,mm1);// mm0=_mm_min_pu8(mm0,mm1); // minimum value of the 2
 Tsimd::pmaxub(mm1,mm2);// mm1=_mm_max_pu8(mm1,mm2); // maximum value of the 2
 mm2=_mm_subs_pu8(mm1,mm0);// the diff
 Tsimd::pavgb(mm0,mm1);// mm0=_mm_avg_pu8(mm0,mm1); // just keep avg
 __m64 mm3=_mm_subs_pu8(mm2,mm5); // nonzero where old weights lower, else 0
 mm3=_mm_cmpeq_pi8(mm3,m0); // now ff where new better, else 00
 mm0=_mm_and_si64(mm0,mm3); // keep only better new avg pixels
 mm2=_mm_and_si64(mm2,mm3); // and weights
 mm3=_mm_cmpeq_pi8(mm3,m0); // here ff where old better, else 00
 mm5=_mm_and_si64(mm5,mm3); // keep only better old weights
 mm6=_mm_and_si64(mm6,mm3); // keep only better old avg pixels
 mm5=_mm_or_si64(mm5,mm2);  // merge new & old weights
 mm6=_mm_or_si64(mm6,mm0);  // merge new & old avg pixels
}

template<class Tsimd> void TimgFilterFramerateDoubler::Tinterpolate<Tsimd>::interpolateMotion(
 stride_t src_pit, stride_t dst_pit, stride_t prev_pit,
 unsigned int rowsize, const BYTE* srcp, BYTE* dstp, const BYTE* _pPrev,
 int FldHeight, // go H expand lines into even output lines
 int Search_Effort,int thresh)
{
 const BYTE* pPrev = _pPrev;
 const BYTE* pSrc = srcp;
 BYTE* pDest = dstp;
 int ct = ((rowsize-10) >> 3);

 memcpy(pDest, pSrc, rowsize);           // copy first line
 memcpy(pDest+dst_pit*(FldHeight-1), pSrc+src_pit*(FldHeight-1), rowsize);  // and last

 pDest += dst_pit;

 const __m64 thresh64=_mm_set1_pi8((char)thresh);
 if (Search_Effort>=9)
  for (int y=1;y<=FldHeight-2;y++,pSrc+=src_pit,pPrev+=prev_pit,pDest+=dst_pit)
   {
    *(__m64*)(pDest+0)=*(__m64*)(pSrc+0);
    *(__m64*)(pDest+rowsize-8)=*(__m64*)(pSrc+src_pit+rowsize-8);
    for (unsigned int x=8;x<rowsize-10;x+=8)
     {
      __m64 mm5=thresh64;
      __m64 mm6=*(__m64*)(pSrc+x+src_pit);
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x          -1),(const __m64*)(pPrev+x+2*prev_pit+1));
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x          +1),(const __m64*)(pPrev+x+2*prev_pit-1));
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x+2*src_pit-1),(const __m64*)(pPrev+x+  prev_pit+1));
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x+2*src_pit+1),(const __m64*)(pPrev+x+  prev_pit-1));
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x+2*src_pit  ),(const __m64*)(pPrev+x+  prev_pit  ));
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x            ),(const __m64*)(pPrev+x+2*prev_pit  ));
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x+src_pit  -1),(const __m64*)(pPrev+x+  prev_pit+1));
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x+src_pit  +1),(const __m64*)(pPrev+x+  prev_pit-1));
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x+src_pit    ),(const __m64*)(pPrev+x+  prev_pit  ));
      Tsimd::movntq(pDest+x,mm6);
     }
   }
 else if (Search_Effort>=3)
  for (int y=1;y<=FldHeight-2;y++,pSrc+=src_pit,pPrev+=prev_pit,pDest+=dst_pit)
   {
    *(__m64*)(pDest+0)=*(__m64*)(pSrc+0);
    *(__m64*)(pDest+rowsize-8)=*(__m64*)(pSrc+src_pit+rowsize-8);
    for (unsigned int x=8;x<rowsize-10;x+=8)
     {
      __m64 mm5=thresh64;
      __m64 mm6=*(__m64*)(pSrc+x+src_pit);
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x+src_pit-1),(const __m64*)(pPrev+x+prev_pit+1));
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x+src_pit+1),(const __m64*)(pPrev+x+prev_pit-1));
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x+src_pit  ),(const __m64*)(pPrev+x+prev_pit  ));
      Tsimd::movntq(pDest+x,mm6);
     }
   }
 else
  for (int y=1;y<=FldHeight-2;y++,pSrc+=src_pit,pPrev+=prev_pit,pDest+=dst_pit)
   {
    *(__m64*)(pDest+0)=*(__m64*)(pSrc+0);
    *(__m64*)(pDest+rowsize-8)=*(__m64*)(pSrc+src_pit+rowsize-8);
    for (unsigned int x=8;x<rowsize-10;x+=8)
     {
      __m64 mm5=thresh64;
      __m64 mm6=*(__m64*)(pSrc+x+src_pit);
      bestInterp(mm5,mm6,(const __m64*)(pSrc+x+src_pit),(const __m64*)(pPrev+x+prev_pit));
      Tsimd::movntq(pDest+x,mm6);
     }
   }
}
HRESULT TimgFilterFramerateDoubler::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TdeinterlaceSettings *cfg=(const TdeinterlaceSettings*)cfg0;
 if ((pict.fieldtype & FIELD_TYPE::PROGRESSIVE_FRAME) && !cfg->deinterlaceAlways)
 {
  done();
  return parent->deliverSample(++it,pict);
 }
 init(pict,true,cfg->half);
 const unsigned char *src[4];
 bool cspChanged=getCur(FF_CSPS_MASK_YUV_PLANAR,pict,true,src);
 if (cspChanged) done();
 if (old)
  {
   TffPict p1=pict;
   p1.setRO(true);
   //interpolate image
   unsigned char *dst[4];
   getNext(csp1,p1,true,dst);
   for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
    if (cfg->frameRateDoublerSE==0)
     pictInterpolate(dst[i],stride2[i],old->data[i],old->stride[i],src[i],stride1[i],dx1[i],dy1[i]);
    else
     interpolateMotion(stride1[i],stride2[i],old->stride[i],pict.cspInfo.Bpp*(dx1[i]),src[i],dst[i],old->data[i],dy1[i],cfg->frameRateDoublerSE,cfg->frameRateDoublerThreshold);
   _mm_empty();
   TfilterQueue::iterator it1=it;
   REFERENCE_TIME dur=(pict.rtStop-pict.rtStart)/2;
   p1.rtStop=pict.rtStart+dur;
   HRESULT hr=parent->deliverSample(++it1,p1);
   if (FAILED(hr)) return hr;
   pict.rtStart+=dur;
  }
 else
  old=new TffPict;
 old->copyFrom(pict,oldbuf);

 if (pict.rectClip != pict.rectFull)
  parent->dirtyBorder=1;

 return parent->deliverSample(++it,pict);
}

void TimgFilterFramerateDoubler::onSeek(void)
{
 if (old) delete old;old=NULL;
}

//========================================= TimgFilterMplayerDeinterlace ==========================================
TimgFilterMplayerDeinterlace::TimgFilterMplayerDeinterlace(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 libmplayer=NULL;
 pp_ctx=NULL;Tlibmplayer::pp_mode_defaults(pp_mode);
}
TimgFilterMplayerDeinterlace::~TimgFilterMplayerDeinterlace()
{
 if (libmplayer) libmplayer->Release();
}
void TimgFilterMplayerDeinterlace::done(void)
{
 if (pp_ctx) libmplayer->pp_free_context(pp_ctx);pp_ctx=NULL;
}
HRESULT TimgFilterMplayerDeinterlace::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TdeinterlaceSettings *cfg=(const TdeinterlaceSettings*)cfg0;
 if ((pict.fieldtype & FIELD_TYPE::PROGRESSIVE_FRAME) && !cfg->deinterlaceAlways)
 {
  return parent->deliverSample(++it,pict);
 }
 init(pict,cfg->full,cfg->half);
 bool cspChanged=false;
 const unsigned char *tempPict1[4];
 cspChanged|=getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,tempPict1);
 unsigned char *tempPict2[4];
 cspChanged|=getNext(csp1,pict,cfg->full,tempPict2);
 if (cspChanged) done();

 if (!pp_ctx)
  {
   if (!libmplayer) deci->getPostproc(&libmplayer);
   pp_ctx=libmplayer->pp_get_context(dx1[0],dy1[0],Tlibmplayer::ppCpuCaps(csp1));
  }
 const TdeinterlaceSettings::TmethodProps &methodProps=TdeinterlaceSettings::getMethod(cfg->cfgId);
 pp_mode.lumMode=pp_mode.chromMode=methodProps.id;
 libmplayer->pp_postprocess(tempPict1,stride1,
                            tempPict2,stride2,
                            dx1[0],dy1[0],
                            NULL,0,
                            &pp_mode,pp_ctx,pict.frametype&FRAME_TYPE::typemask);
 pict.fieldtype=(pict.fieldtype & ~(FIELD_TYPE::MASK_PROG | FIELD_TYPE::MASK_INT)) | FIELD_TYPE::PROGRESSIVE_FRAME;

 if (pict.rectClip != pict.rectFull)
  parent->dirtyBorder=1;

 return parent->deliverSample(++it,pict);
}
