/*
 * Copyright (c) 2002-2006 Milan Cutka
 * luminance gain and offset based on DVD2AVI source code
 * fast gamma correction by Michael Herf (http://www.stereopsis.com)
 * RGB gamma correction working in YUV from mplayer's vf_eq2 filter by Hampa Hug, Daniel Moreno, Richard Felker
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
#include "TimgFilterLuma.h"
#include "TpictPropSettings.h"
#include "Tlibmplayer.h"
#include "IffdshowBase.h"
#include "simd.h"
#include "Tconfig.h"

static void calcGamma(int &oldGamma,int newGamma,unsigned int tab[256],unsigned int shift)
{
 if (oldGamma!=newGamma)
  {
   oldGamma=newGamma;
   double gamma=100.0/(oldGamma);
   for(int i=0;i<256;i++)
    tab[i]=(unsigned char)(255.0*pow((i/255.0),gamma))<<shift;
  }
}

//======================================= TimgFilterLuma =======================================
TimgFilterLuma::TimgFilterLuma(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 oldGamma=-1;
#ifdef __SSE2__
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  processLumaFc=&TimgFilterLuma::processLuma<Tsse2>;
 else
#endif
  processLumaFc=&TimgFilterLuma::processLuma<Tmmx>;
}
void TimgFilterLuma::processGamma(const unsigned char *srcY,unsigned char *dstY,const TpictPropSettings *cfg)
{
 if (oldGamma!=cfg->gammaCorrection)
  {
   oldGamma=cfg->gammaCorrection;
   double gamma=100.0/(oldGamma);
   for(int i=0;i<256;i++)
    {
     gammaTab[i]=(unsigned char)(255.0*pow((i/255.0),gamma));
     gammaTab8[i]=gammaTab[i]<<8;
     gammaTab16[i]=gammaTab8[i]<<8;
     gammaTab24[i]=gammaTab16[i]<<8;
    }
  }
 if (oldGamma!=cfg->gammaCorrectionDef)
  for (unsigned int y=0;y<dy1[0];y++)
   {
    const unsigned char *src;unsigned char *dst,*dstEnd;
    for (src=srcY+stride1[0]*y,dst=dstY+stride2[0]*y,dstEnd=dst+dx1[0];dst<dstEnd;src+=4,dst+=4)
     *(unsigned int*)dst=gammaTab24[src[3]]|gammaTab16[src[2]]|gammaTab8[src[1]]|gammaTab[src[0]];
     //*(unsigned int*)dst=(gammaTab[src[3]]<<24)|(gammaTab[src[2]]<<16)|(gammaTab[src[1]]<<8)|gammaTab[src[0]];
   }
}

bool TimgFilterLuma::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 if (super::is(pict,cfg0))
  {
   const TpictPropSettings *cfg=(const TpictPropSettings*)cfg0;
   bool procgainoffset=cfg->lumGain!=cfg->lumGainDef || cfg->lumOffset!=cfg->lumOffsetDef;
   if (procgainoffset) return true;
   bool procgamma=cfg->gammaCorrection!=cfg->gammaCorrectionDef;
   if (procgamma) return true;
  }
 return false;
}

template<class _mm> void TimgFilterLuma::processLuma(const TpictPropSettings *cfg,const unsigned char *srcY,unsigned char *dstY,int ystep)
{
 typedef typename _mm::__m __m;
 __m lumGain64=_mm::set1_pi16((short)cfg->lumGain);
 __m lumOffset64=_mm::set1_pi16((short)cfg->lumOffset);
 __m m0=_mm::setzero_si64(),m0040=_mm::set1_pi16(0x0040);
 int dx7=dx1[0]-_mm::size+1;
 int y=0;
 for (const unsigned char *srcYend=srcY+dy1[0]*stride1[0];srcY!=srcYend;srcY+=stride1[0],dstY+=stride2[0],y++)
  if (!ystep || (y%(ystep*2))<ystep)
   {
    int x=0;
    if (_mm::align)
     for (;intptr_t(dstY+x)&(_mm::align-1) && x<(int)dx1[0];x++)
      dstY[x]=limit_uint8((srcY[x]*cfg->lumGain>>7)+cfg->lumOffset);
    for (;x<dx7;x+=_mm::size)
     {
      __m mm1=*(__m*)(srcY+x);
      __m a=_mm::add_pi16(_mm::srli_pi16(_mm::add_pi16(_mm::mullo_pi16(_mm::unpacklo_pi8(mm1,m0),lumGain64),m0040),7),lumOffset64);
      __m b=_mm::add_pi16(_mm::srli_pi16(_mm::add_pi16(_mm::mullo_pi16(_mm::unpackhi_pi8(mm1,m0),lumGain64),m0040),7),lumOffset64);
      _mm::store2(dstY+x            ,_mm::packs_pu16(a,m0));
      _mm::store2(dstY+x+_mm::size/2,_mm::packs_pu16(b,m0));
     }
    for (;x<(int)dx1[0];x++)
     dstY[x]=limit_uint8((srcY[x]*cfg->lumGain>>7)+cfg->lumOffset);
   }
 else
  if (dstY!=srcY)
   memcpy(dstY,srcY,dx1[0]);
 _mm::empty();
}
HRESULT TimgFilterLuma::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TpictPropSettings *cfg=(const TpictPropSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   if (cfg->lumGain!=cfg->lumGainDef || cfg->lumOffset!=cfg->lumOffsetDef)
    {
     const unsigned char *srcY;
     getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&srcY,NULL,NULL,NULL);
     unsigned char *dstY;
     getCurNext(csp1,pict,cfg->full,COPYMODE_NO,&dstY,NULL,NULL,NULL);
     (this->*processLumaFc)(cfg,srcY,dstY,cfg->scanlineEffect);
    }
   if (cfg->gammaCorrection!=cfg->gammaCorrectionDef)
    {
     const unsigned char *srcY;
     getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&srcY,NULL,NULL,NULL);
     unsigned char *dstY;
     getCurNext(csp1,pict,cfg->full,COPYMODE_NO,&dstY,NULL,NULL,NULL);
     processGamma(srcY,dstY,cfg);
    }
  }
 return parent->deliverSample(++it,pict);
}

//===================================== TimgFilterGammaRGB =====================================
TimgFilterGammaRGB::TimgFilterGammaRGB(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 oldGammaR=oldGammaG=oldGammaB=-1;
}
void TimgFilterGammaRGB::processGammaRGB32(const unsigned char *src,unsigned char *dst,const TpictPropSettings *cfg)
{
 calcGamma(oldGammaR,cfg->gammaCorrectionR,gammaTabR,16);
 calcGamma(oldGammaG,cfg->gammaCorrectionG,gammaTabG,8);
 calcGamma(oldGammaB,cfg->gammaCorrectionB,gammaTabB,0);
 for (const unsigned char *srcEnd=src+stride1[0]*dy1[0];src!=srcEnd;src+=stride1[0],dst+=stride2[0])
  {
   const unsigned char *srcLn=src;
   unsigned int *dstLn=(unsigned int*)dst;
   for (const unsigned int *dstLnEnd=dstLn+dx1[0];dstLn<dstLnEnd;srcLn+=4,dstLn++)
    *dstLn=gammaTabB[srcLn[0]]|gammaTabG[srcLn[1]]|gammaTabR[srcLn[2]];
  }
}
void TimgFilterGammaRGB::processGammaYUV(TffPict &pict,const unsigned char *src[4],unsigned char *dst[4],const TpictPropSettings *cfg)
{
 if (oldGammaR!=cfg->gammaCorrectionR || oldGammaG!=cfg->gammaCorrectionG || oldGammaB!=cfg->gammaCorrectionB)
  {
   oldGammaR=cfg->gammaCorrectionR;oldGammaG=cfg->gammaCorrectionG;oldGammaB=cfg->gammaCorrectionB;
   double gammaR=oldGammaR/100.0;
   double gammaG=oldGammaG/100.0;
   double gammaB=oldGammaB/100.0;
   double gY = 1.0/(0.299*gammaR+0.587*gammaG+0.114*gammaB);
   double gU = 1.0/sqrt (gammaB / gammaG);
   double gV = 1.0/sqrt (gammaR / gammaG);
   for(int i=0;i<256;i++)
    {
     gammaTabYUV[0][i]=(unsigned char)(255.0*pow((i/255.0),gY));
     gammaTabYUV[1][i]=(unsigned char)(255.0*pow((i/255.0),gU));
     gammaTabYUV[2][i]=(unsigned char)(255.0*pow((i/255.0),gV));
    }
  }
 for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
  for (const unsigned char *srcEnd=src[i]+dy1[i]*stride1[i];src[i]!=srcEnd;src[i]+=stride1[i],dst[i]+=stride2[i])
   {
    unsigned char *dstLn=dst[i];
    for (const unsigned char *srcLn=src[i],*srcLnEnd=src[i]+dx1[i];srcLn<srcLnEnd;srcLn+=4,dstLn+=4)
     *(unsigned int*)dstLn=(gammaTabYUV[i][srcLn[3]]<<24)|(gammaTabYUV[i][srcLn[2]]<<16)|(gammaTabYUV[i][srcLn[1]]<<8)|gammaTabYUV[i][srcLn[0]];
     //*dstLn=gammaTabYUV[i][*srcLn];
   }
}

bool TimgFilterGammaRGB::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TpictPropSettings *cfg=(const TpictPropSettings*)cfg0;
 return super::is(pict,cfg) && (cfg->gammaCorrectionR!=cfg->gammaCorrectionDef || cfg->gammaCorrectionG!=cfg->gammaCorrectionDef || cfg->gammaCorrectionB!=cfg->gammaCorrectionDef);
}

HRESULT TimgFilterGammaRGB::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TpictPropSettings *cfg=(const TpictPropSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   const unsigned char *src[4];
   getCur(FF_CSP_RGB32|FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&src[0],&src[1],&src[2],&src[3]);
   unsigned char *dst[4];
   getCurNext(csp1,pict,cfg->full,COPYMODE_NO,&dst[0],&dst[1],&dst[2],&dst[3]);
   if (csp_isYUVplanar(csp1))
    processGammaYUV(pict,src,dst,cfg);
   else
    processGammaRGB32(src[0],dst[0],cfg);
  }
 return parent->deliverSample(++it,pict);
}

//===================================== TimgFilterLevelFix =====================================
TimgFilterLevelFix::TimgFilterLevelFix(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 pp_ctx=NULL;Tlibmplayer::pp_mode_defaults(pp_mode);
 libmplayer=NULL;
}
TimgFilterLevelFix::~TimgFilterLevelFix()
{
 if (libmplayer) libmplayer->Release();
}

void TimgFilterLevelFix::done(void)
{
 if (pp_ctx) libmplayer->pp_free_context(pp_ctx);pp_ctx=NULL;
}
void TimgFilterLevelFix::onSizeChange(void)
{
 done();
}

bool TimgFilterLevelFix::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TpictPropSettings *cfg=(const TpictPropSettings*)cfg0;
 return super::is(pict,cfg) && cfg->levelfix;
}

HRESULT TimgFilterLevelFix::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TpictPropSettings *cfg=(const TpictPropSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   const unsigned char *tempPict1[4];
   bool cspChanged=getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,tempPict1);
   unsigned char *tempPict2[4];
   cspChanged|=getNext(csp1,pict,cfg->full,tempPict2);
   if (cspChanged) done();

   if (!pp_ctx)
    {
     if (!libmplayer) deci->getPostproc(&libmplayer);
     pp_ctx=libmplayer->pp_get_context(dx1[0],dy1[0],Tlibmplayer::ppCpuCaps(csp1));
    }

   pp_mode.lumMode=LUM_LEVEL_FIX;
   if (cfg->levelfixFull)
    {
     pp_mode.minAllowedY=0;
     pp_mode.maxAllowedY=255;
    }
   else
    {
     pp_mode.minAllowedY=16;
     pp_mode.maxAllowedY=234;
    }
   libmplayer->pp_postprocess(tempPict1,stride1,
                              tempPict2,stride2,
                              dx1[0],dy1[0],
                              NULL,0,
                              &pp_mode,pp_ctx,pict.frametype&FRAME_TYPE::typemask);
  }
 return parent->deliverSample(++it,pict);
}
