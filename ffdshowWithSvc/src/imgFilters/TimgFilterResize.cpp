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
#include "TimgFilterResize.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "postproc/swscale.h"
#include "Tlibmplayer.h"
#include "Tconfig.h"
#include "SimpleResize.h"
#include "TimgFilters.h"
#include "2xsai.h"
#include "TffDecoderVideo.h"

TimgFilterResize::TimgFilterResize(IffdshowBase *Ideci,Tfilters *Iparent):
 TimgFilter(Ideci,Iparent),
 swsparams(new SwsParams)
{
 libmplayer=NULL;swsc=NULL;swsf=NULL;
 simple=NULL;
 sizeChanged=true;
 oldinterlace=oldWarped=false;
 inited=false;
}
TimgFilterResize::~TimgFilterResize()
{
 if (libmplayer) libmplayer->Release();
 delete swsparams;
}

void TimgFilterResize::done(void)
{
 if (swsf) libmplayer->sws_freeFilter(swsf);swsf=NULL;
 if (swsc) libmplayer->sws_freeContext(swsc);swsc=NULL;
 if (simple) delete simple;simple=NULL;
}
void TimgFilterResize::onSizeChange(void)
{
 done();
 sizeChanged=true;
}

bool TimgFilterResize::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 if (!cfg0->show) return false;
 const TresizeAspectSettings *cfg=(const TresizeAspectSettings*)cfg0;
 if (TresizeAspectSettings::methodsProps[cfg->methodLuma].library==TresizeAspectSettings::LIB_SAI)
  {
   Trect newRectFull=pict.rectFull,newRectClip=pict.rectClip;
   cfg->calcNewRects(&newRectFull, &newRectClip, !!(pict.csp & FF_CSP_FLAGS_VFLIP));
   return pict.rectFull!=newRectFull || pict.rectClip!=pict.rectClip;
  }
 else
  return true;
}

int TimgFilterResize::getSupportedInputColorspaces(const TfilterSettingsVideo *cfg0) const
{
 const TresizeAspectSettings *cfg=(const TresizeAspectSettings*)cfg0;
 switch (TresizeAspectSettings::methodsProps[cfg->methodLuma].library)
  {
   case TresizeAspectSettings::LIB_SWSCALER:return SWS_IN_CSPS;
   case TresizeAspectSettings::LIB_SIMPLE:return cfg->methodLuma==TresizeAspectSettings::METHOD_WARPED?FF_CSP_YUY2:FF_CSP_420P;
   case TresizeAspectSettings::LIB_SAI:
    switch (TresizeAspectSettings::methodsProps[cfg->methodLuma].flags)
     {
      case 0:return FF_CSP_RGB32;
      case 1:return FF_CSP_RGB16;
      case 2:return FF_CSP_RGB16;
     }
   default:
   case TresizeAspectSettings::LIB_NONE:return FF_CSPS_MASK;
  }
}
int TimgFilterResize::getSupportedOutputColorspaces(const TfilterSettingsVideo *cfg0) const
{
 const TresizeAspectSettings *cfg=(const TresizeAspectSettings*)cfg0;
 switch (TresizeAspectSettings::methodsProps[cfg->methodLuma].library)
  {
   case TresizeAspectSettings::LIB_SWSCALER:return SWS_OUT_CSPS;
   case TresizeAspectSettings::LIB_SAI:
    switch (TresizeAspectSettings::methodsProps[cfg->methodLuma].flags)
     {
      case 0:return FF_CSP_RGB32;
      case 1:return FF_CSP_RGB16;
      case 2:return FF_CSP_RGB32;
     }
   default:return getSupportedInputColorspaces(cfg);
  }
}

bool TimgFilterResize::getOutputFmt(TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 if (super::getOutputFmt(pict,cfg0))
  {
   const TresizeAspectSettings *cfg=(const TresizeAspectSettings*)cfg0;
   cfg->calcNewRects(&pict.rectFull, &pict.rectClip, !!(pict.csp & FF_CSP_FLAGS_VFLIP));
   return true;
  }
 else
  return false;
}

HRESULT TimgFilterResize::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TresizeAspectSettings *cfg=(const TresizeAspectSettings*)cfg0;
 init(pict,cfg->full,0);
 if (sizeChanged || !cfg->equal(oldSettings) || oldSettings.is!=cfg->is || oldSettings.full!=cfg->full || oldcsp != pict.csp)
  {
   sizeChanged=false;
   oldSettings=*cfg;
   oldcsp=pict.csp;
   done();
   inited=false;
   newpict.rectFull=pict.rectFull;
   newpict.rectClip=pict.rectClip;
   newpict.csp=pict.csp;
   getOutputFmt(newpict,cfg);
   enum TffdshowDecVideo::DOWNSTREAM_FILTER_TYPE downstream=(TffdshowDecVideo::DOWNSTREAM_FILTER_TYPE)deciV->get_downstreamID();
   char_t outputfourcc[20];
   deciV->getOutputFourcc(outputfourcc,20);
   if (downstream==TffdshowDecVideo::DVOBSUB && strncmp(outputfourcc,_l("YV12"),4)==0) // vsfilter accepts only multiple of 8 on YV12.
    newpict.rectFull.dx=(newpict.rectFull.dx+7)&~7;
   if ((pict.rectClip!=newpict.rectClip || pict.rectFull!=newpict.rectFull)
      &&!(   pict.cspInfo.id==FF_CSP_420P       // I want to avoid resizing here. YV12 odd number lines.
          && pict.rectFull==newpict.rectFull
          && newpict.rectClip.dy==(pict.rectClip.dy&~1)
          && newpict.rectClip.dx==pict.rectClip.dx
          && newpict.rectClip.x==pict.rectClip.x
          && newpict.rectClip.y==pict.rectClip.y
          ))
    {
     switch (TresizeAspectSettings::methodsProps[oldSettings.methodLuma].library)
      {
       case TresizeAspectSettings::LIB_SWSCALER:
        memset(swsparams,0,sizeof(*swsparams));
        swsparams->cpu=Tconfig::sws_cpu_flags;
        swsparams->methodLuma.method=TresizeAspectSettings::methodsProps[oldSettings.methodLuma].flags;
        switch (oldSettings.methodLuma)
         {
          case TresizeAspectSettings::METHOD_SWS_BICUBIC:swsparams->methodLuma.param=oldSettings.bicubicLumaParam;break;
          case TresizeAspectSettings::METHOD_SWS_GAUSS:swsparams->methodLuma.param=oldSettings.gaussLumaParam;break;
          case TresizeAspectSettings::METHOD_SWS_LANCZOS:swsparams->methodLuma.param=oldSettings.lanczosLumaParam;break;
         }
        if (oldSettings.methodsLocked)
         swsparams->methodChroma=swsparams->methodLuma;
        else
         {
          swsparams->methodChroma.method=TresizeAspectSettings::methodsProps[oldSettings.methodChroma].flags;
          switch (oldSettings.methodChroma)
           {
            case TresizeAspectSettings::METHOD_SWS_BICUBIC:swsparams->methodChroma.param=oldSettings.bicubicChromaParam;break;
            case TresizeAspectSettings::METHOD_SWS_GAUSS:swsparams->methodChroma.param=oldSettings.gaussChromaParam;break;
            case TresizeAspectSettings::METHOD_SWS_LANCZOS:swsparams->methodChroma.param=oldSettings.lanczosChromaParam;break;
           }
         }
        break;
       case TresizeAspectSettings::LIB_NONE:
        if (pictRect.dy<newpict.rectClip.dy)
         {
          dynone=pictRect.dy;
          ydif1none=(newpict.rectClip.dy-pictRect.dy)/2;
          ydif2none=0;
         }
        else
         {
          dynone=newpict.rectClip.dy;
          ydif1none=0;
          ydif2none=(pictRect.dy-newpict.rectClip.dy)/2;
         }
        if (pictRect.dx<newpict.rectClip.dx)
         {
          dxnone=pictRect.dx;
          xdif1none=(newpict.rectClip.dx-pictRect.dx)/2;
          xdif2none=0;
         }
        else
         {
          dxnone=newpict.rectClip.dx;
          xdif1none=0;
          xdif2none=(pictRect.dx-newpict.rectClip.dx)/2;
         }
        break;
      }
     parent->dirtyBorder=1;
     inited=true;
    }
  }

 if (inited)
  {
   parent->adhocDVDsub(it, pict); // draw DVD subtitles and menu before resize, if it is not done.

   bool interlace;
   switch (cfg->interlaced)
    {
     case 0 :interlace=false;break;
     case 1 :interlace=true;break;
     case 2 :interlace=pict.fieldtype&FIELD_TYPE::MASK_INT?true:false;break;
     default:interlace=false;break;
    }
   switch (TresizeAspectSettings::methodsProps[oldSettings.methodLuma].library)
    {
     case TresizeAspectSettings::LIB_SWSCALER:
      {
       if (pict.rectFull.dx!=pict.rectClip.dx)
        parent->dirtyBorder=1;
       bool cspChanged=false;
       const unsigned char *src[4];
       cspChanged|=getCur(SWS_IN_CSPS,pict,cfg->full,src);
       unsigned char *dst[4];
       cspChanged|=getNext(SWS_OUT_CSPS,pict,newpict.rectClip,dst,&newpict.rectFull);
       if (cspChanged || !swsc || oldinterlace!=interlace)
        {
         if (!libmplayer) deci->getPostproc(&libmplayer);
         if (swsc) libmplayer->sws_freeContext(swsc);swsc=NULL;
         if (swsf) libmplayer->sws_freeFilter(swsf);swsf=NULL;
         oldinterlace=interlace;
         if(cfg->accurateRounding)
          swsparams->subsampling|=SWS_ACCURATE_RND;
         swsf=libmplayer->sws_getDefaultFilter(oldSettings.GblurLum/100.0f,oldSettings.GblurChrom/100.0f,oldSettings.sharpenLum/100.0f,oldSettings.sharpenChrom/100.0f,0,0,0);
         if (!oldinterlace)
          swsc=libmplayer->sws_getContext(pictRect.dx,pictRect.dy,csp_ffdshow2mplayer(csp1),newpict.rectClip.dx,newpict.rectClip.dy,csp_ffdshow2mplayer(csp2),swsparams,swsf,NULL,NULL);
         else
          swsc=libmplayer->sws_getContext(pictRect.dx,pictRect.dy/2,csp_ffdshow2mplayer(csp1),newpict.rectClip.dx,newpict.rectClip.dy/2,csp_ffdshow2mplayer(csp2),swsparams,swsf,NULL,NULL);
        }
       if (!oldinterlace)
        libmplayer->sws_scale_ordered(swsc,src,stride1,0,pictRect.dy,dst,stride2);
       else
        {
         stride_t stride1I[]={stride1[0]*2,stride1[1]*2,stride1[2]*2,stride1[3]*2};
         stride_t stride2I[]={stride2[0]*2,stride2[1]*2,stride2[2]*2,stride2[3]*2};
         libmplayer->sws_scale_ordered(swsc,src,stride1I,0,pictRect.dy/2,dst,stride2I);
         for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
          {
           src[i]+=stride1[i];
           dst[i]+=stride2[i];
          }
         libmplayer->sws_scale_ordered(swsc,src,stride1I,0,pictRect.dy/2,dst,stride2I);
        }
       break;
      }
     case TresizeAspectSettings::LIB_NONE:
      {
       const unsigned char *src[4];
       getCur(pict.csp,pict,cfg->full,src);
       unsigned char *dst[4];
       getNext(pict.csp,pict,newpict.rectClip,dst,&newpict.rectFull);
       for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
        {
         const unsigned char *src0=src[i]+(ydif2none>>pict.cspInfo.shiftY[i])*stride1[i]+(xdif2none>>pict.cspInfo.shiftX[i]);
         unsigned char       *dst0=dst[i]+(ydif1none>>pict.cspInfo.shiftY[i])*stride2[i]+(xdif1none>>pict.cspInfo.shiftX[i]);
         TffPict::copy(dst0,stride2[i],src0,stride1[i],pict.cspInfo.Bpp*dxnone>>pict.cspInfo.shiftX[i],dynone>>pict.cspInfo.shiftY[i]);
        }
       break;
      }
     case TresizeAspectSettings::LIB_SIMPLE:
      {
       bool warped=oldSettings.methodLuma==TresizeAspectSettings::METHOD_WARPED;
       if (!simple || oldinterlace!=interlace || oldWarped!=warped)
        {
         oldinterlace=interlace;oldWarped=warped;
         SimpleResize::VideoInfo vi;
         vi.width=pictRect.dx;vi.height=pictRect.dy/(oldinterlace?2:1);
         vi.IsYV12=!warped;vi.IsYUY2=!vi.IsYV12;
         simple=new SimpleResize(vi,newpict.rectClip.dx,newpict.rectClip.dy/(oldinterlace?2:1),warped?cfg->simpleWarpXparam/1000.0:1.0,warped?cfg->simpleWarpYparam/1000.0:1.0,false);
        }
       if (simple->ok)
        {
         SimpleResize::PVideoFrame srcFrame;
         getCur(warped?FF_CSP_YUY2:FF_CSP_420P,pict,cfg->full,(const unsigned char**)srcFrame.ptr);
         for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
          {
           srcFrame.pitch[i]=stride1[i];
           srcFrame.rowSize[i]=dx1[i]*pict.cspInfo.Bpp;
           srcFrame.height[i]=dy1[i];
          }
         SimpleResize::PVideoFrame dstFrame;
         getNext(warped?FF_CSP_YUY2:FF_CSP_420P,pict,newpict.rectClip,dstFrame.ptr,&newpict.rectFull);
         for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
          {
           dstFrame.pitch[i]=stride2[i];
           dstFrame.rowSize[i]=(newpict.rectClip.dx*pict.cspInfo.Bpp)>>pict.cspInfo.shiftX[i];
           dstFrame.height[i]=newpict.rectClip.dy>>pict.cspInfo.shiftY[i];
          }
         if (!oldinterlace)
          simple->GetFrame(&srcFrame,&dstFrame);
         else
          {
           for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
            {
             srcFrame.height[i]/=2;srcFrame.pitch[i]*=2;
             dstFrame.height[i]/=2;dstFrame.pitch[i]*=2;
            }
           simple->GetFrame(&srcFrame,&dstFrame);
           for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
            {
             srcFrame.ptr[i]+=stride1[i];
             dstFrame.ptr[i]+=stride2[i];
            }
           simple->GetFrame(&srcFrame,&dstFrame);
          }
        }
       break;
      }
     case TresizeAspectSettings::LIB_SAI:
      switch (TresizeAspectSettings::methodsProps[oldSettings.methodLuma].flags)
       {
        case 0:
         {
          const unsigned char *src;
          getCur(FF_CSP_RGB32,pict,cfg->full,&src,NULL,NULL,NULL);
          unsigned char *dst;
          getNext(FF_CSP_RGB32,pict,newpict.rectClip,&dst,NULL,NULL,NULL,&newpict.rectFull);
          T2xSaI::super(src,stride1[0],dst,stride2[0],dx1[0],dy1[0]);
          break;
         }
        case 1:
         {
          const unsigned char *src;
          getCur(FF_CSP_RGB16,pict,cfg->full,&src,NULL,NULL,NULL);
          unsigned char *dst;
          getNext(FF_CSP_RGB16,pict,newpict.rectClip,&dst,NULL,NULL,NULL,&newpict.rectFull);
          T2xSaI::_2xSaI(src,stride1[0],dst,dx1[0],dy1[0],stride2[0]);
          break;
         }
        case 2:
         {
          const unsigned char *src;
          getCur(FF_CSP_RGB16,pict,cfg->full,&src,NULL,NULL,NULL);
          unsigned char *dst;
          getNext(FF_CSP_RGB32,pict,newpict.rectClip,&dst,NULL,NULL,NULL,&newpict.rectFull);
          Thq2x::hq2x_32(src,dst,dx1[0],dy1[0],stride1[0],stride2[0]);
          break;
         }
       }
      break;
    }
   if (!parent->dirtyBorder) parent->dirtyBorder=1;
  }
 return parent->deliverSample(++it,pict);
}
