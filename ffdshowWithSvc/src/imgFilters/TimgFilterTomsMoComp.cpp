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
#include "TimgFilterTomsMoComp.h"
#include "TdeinterlaceSettings.h"
#include "Tdll.h"
#include "TimgFilters.h"
#include "TomsMoComp/TomsMoComp_ff.h"

const char_t* TimgFilterTomsMoComp::dllname=_l("TomsMoComp_ff.dll");

TimgFilterTomsMoComp::TimgFilterTomsMoComp(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 dll=new Tdll(dllname,parent->config);
 dll->loadFunction(createI,"createI");
 psrc[0]=psrc[1]=psrc[2]=NULL;
 t=NULL;
 se=-1;vf=-1;
}
TimgFilterTomsMoComp::~TimgFilterTomsMoComp()
{
 done();
 delete dll;
}
void TimgFilterTomsMoComp::done(void)
{
 for (int i=0;i<3;i++)
  {
   if (psrc[i]) aligned_free(psrc[i]);
   psrc[i]=NULL;
  }
 if (t) t->destroy();t=NULL;
 inited=false;
}
void TimgFilterTomsMoComp::onSizeChange(void)
{
 done();
}

bool TimgFilterTomsMoComp::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg)
{
 if (dll->ok && super::is(pict,cfg))
  {
   Trect pictRect=pict.getRect(cfg->full,cfg->half);
   return (pictRect.dx&3)==0;
  }
 else
  return false;
}

HRESULT TimgFilterTomsMoComp::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TdeinterlaceSettings *cfg=(const TdeinterlaceSettings*)cfg0;
   if ((pict.fieldtype & FIELD_TYPE::PROGRESSIVE_FRAME) && !cfg->deinterlaceAlways)
   {
    return parent->deliverSample(++it,pict);
   }
   init(pict,cfg->full,cfg->half);
   const unsigned char *src[4];
   bool cspChanged=getCur(FF_CSP_420P|FF_CSP_YUY2,pict,cfg->full,src);
   unsigned char *dst[4];
   cspChanged|=getNext(csp1,pict,cfg->full,dst);

   if (cspChanged || se!=cfg->tomsmocompSE || vf!=cfg->tomsmocompVF || oldstride10!=stride1[0])
    {
     oldstride10=stride1[0];
     se=cfg->tomsmocompSE;vf=cfg->tomsmocompVF;
     done();
    }

   if (!t)
    {
     //pstride[0]=(dxY/16+2)*16;pstride[1]=pstride[2]=pstride[0]/2;
     for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
      {
       pstride[i]=stride1[i];
       psrc[i]=(unsigned char*)aligned_calloc(pstride[i],dy1[i]);
      }
     twidth=dx1[0];theight=dy1[0];
     t=createI();
     inited=t->create((Tconfig::cpu_flags&FF_CPU_SSE)!=0,(Tconfig::cpu_flags&FF_CPU_MMXEXT)!=0,(Tconfig::cpu_flags&FF_CPU_3DNOW)!=0,csp1&FF_CSP_YUY2?true:false,
                      -1,se,vf!=0,
                      twidth,theight,stride1,stride2);
     frameNum=0;
    }

   if (inited)
    {
     t->GetFrame(frameNum,1,src,dst,(const unsigned char**)psrc);
     pict.fieldtype=(pict.fieldtype & ~(FIELD_TYPE::MASK_PROG | FIELD_TYPE::MASK_INT)) | FIELD_TYPE::PROGRESSIVE_FRAME;
     for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
      TffPict::copy(psrc[i],pstride[i],src[i],stride1[i],dx2[i],dy2[i]);
     frameNum++;
    }
  }

 if (pict.rectClip != pict.rectFull)
  parent->dirtyBorder=1;

 return parent->deliverSample(++it,pict);
}
void TimgFilterTomsMoComp::onSeek(void)
{
 done();
}
