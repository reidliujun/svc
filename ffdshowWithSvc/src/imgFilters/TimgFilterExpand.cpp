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
#include "TimgFilterExpand.h"
#include "TexpandSettings.h"
#include "TimgFilters.h"

//========================= TimgFilterExpand ==============================

TimgFilterExpand::TimgFilterExpand(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 sizeChanged=true;oldcsp=-1;
 diffx=diffy=INT_MIN;
}
void TimgFilterExpand::onSizeChange(void)
{
 diffx=diffy=INT_MIN;
 sizeChanged=true;
}

bool TimgFilterExpand::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TexpandSettings *cfg=(const TexpandSettings*)cfg0;
 return pict.rectFull.dx!=cfg->newrect.dx || pict.rectFull.dy!=cfg->newrect.dy;
}

void TimgFilterExpand::calcNewRect(const Trect &newrect,Trect &pictrectFull,Trect &pictrectClip)
{
 pictrectFull.dx=newrect.dx;pictrectFull.dy=newrect.dy;
 pictrectClip.dx=std::min(pictrectClip.dx,pictrectFull.dx);pictrectClip.x=(pictrectFull.dx-pictrectClip.dx)/2;
 pictrectClip.dy=std::min(pictrectClip.dy,pictrectFull.dy);pictrectClip.y=(pictrectFull.dy-pictrectClip.dy)/2;
}

bool TimgFilterExpand::getOutputFmt(TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 if (super::getOutputFmt(pict,cfg0))
  {
   const TexpandSettings *cfg=(const TexpandSettings*)cfg0;
   calcNewRect(cfg->newrect,pict.rectFull,pict.rectClip);
   return true;
  }
 else
  return false;
}

void TimgFilterExpand::expand(TffPict &pict,const TfilterSettingsVideo *cfg,bool diffxy)
{
 init(pict,true,0);
 if (sizeChanged || oldcsp!=pict.csp)
  {
   sizeChanged=false;oldcsp=pict.csp;
   newpict.csp=pict.csp;newpict.cspInfo=pict.cspInfo;newpict.rectFull=pict.rectFull;newpict.rectClip=pict.rectClip;
   getOutputFmt(newpict,cfg);
   if (diffxy)
    getDiffXY(pict,cfg,diffx,diffy);
   if (pictRect.dy<newpict.rectClip.dy)
    {
     dynone=pictRect.dy;
     ydif1none=diffxy?diffy:(newpict.rectClip.dy-pictRect.dy)/2;
     ydif2none=0;
    }
   else
    {
     dynone=newpict.rectClip.dy;
     ydif1none=0;
     ydif2none=diffxy ? diffy : pict.rectClip.y;//(pictRect.dy-newpict.rectClip.dy)/2;
     if (ydif2none+dynone>pict.rectFull.dy)
      ydif2none=pict.rectFull.dy-dynone;
    }
   if (pictRect.dx<newpict.rectClip.dx)
    {
     dxnone=pictRect.dx;
     xdif1none=diffxy?diffx:(newpict.rectClip.dx-pictRect.dx)/2;
     xdif2none=0;
    }
   else
    {
     dxnone=newpict.rectClip.dx;
     xdif1none=0;
     xdif2none=diffxy ? diffx : pict.rectClip.x;//(pictRect.dx-newpict.rectClip.dx)/2;
     if (xdif2none+dxnone>pict.rectFull.dx)
      xdif2none=pict.rectFull.dx-dxnone;
    }
   parent->dirtyBorder=1;
  }

 const unsigned char *src[4];
 getCur(newpict.csp,pict,true,src);
 unsigned char *dst[4];
 getNext(newpict.csp,pict,newpict.rectClip,dst,&newpict.rectFull);

 for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
  {
   const unsigned char *src0=src[i]+(ydif2none>>pict.cspInfo.shiftY[i])*stride1[i]+(xdif2none>>pict.cspInfo.shiftX[i]);
   unsigned char       *dst0=dst[i]+(ydif1none>>pict.cspInfo.shiftY[i])*stride2[i]+(xdif1none>>pict.cspInfo.shiftX[i]);
   TffPict::copy(dst0,stride2[i],src0,stride1[i],pict.cspInfo.Bpp*dxnone>>pict.cspInfo.shiftX[i],dynone>>pict.cspInfo.shiftY[i]);
  }
 if (!parent->dirtyBorder) parent->dirtyBorder=1;
}

HRESULT TimgFilterExpand::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TexpandSettings *cfg=(const TexpandSettings*)cfg0;
 if (is(pict,cfg))
  expand(pict,cfg,false);
 return it==NULL?S_OK:parent->deliverSample(++it,pict);
}

//========================= TimgFilterSubtitleExpand ==============================
TimgFilterSubtitleExpand::TimgFilterSubtitleExpand(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilterExpand(Ideci,Iparent)
{
}

void TimgFilterSubtitleExpand::expand(TffPict &pict,const TfilterSettingsVideo *cfg,bool diffxy)
{
 init(pict,true,0);
 if (sizeChanged || oldcsp!=pict.csp)
  {
   sizeChanged=false;oldcsp=pict.csp;
   newpict.csp=pict.csp;newpict.cspInfo=pict.cspInfo;newpict.rectFull=pict.rectFull;newpict.rectClip=pict.rectClip;
   getOutputFmt(newpict,cfg);
   if (diffxy)
    getDiffXY(pict,cfg,diffx,diffy);
   if (pictRect.dy<newpict.rectClip.dy)
    {
     dynone=pictRect.dy;
     ydif1none=diffxy?diffy:(newpict.rectClip.dy-pictRect.dy)/2;
     ydif2none=0;
    }
   else
    {
     dynone=newpict.rectClip.dy;
     ydif1none=0;
     ydif2none=diffxy ? diffy : pict.rectClip.y;
     if (ydif2none+dynone>pict.rectFull.dy)
      ydif2none=pict.rectFull.dy-dynone;
    }
   if (pictRect.dx<newpict.rectClip.dx)
    {
     dxnone=pictRect.dx;
     xdif1none=diffxy?diffx:(newpict.rectClip.dx-pictRect.dx)/2;
     xdif2none=0;
    }
   else
    {
     dxnone=newpict.rectClip.dx;
     xdif1none=0;
     xdif2none=diffxy ? diffx : pict.rectClip.x;
     if (xdif2none+dxnone>pict.rectFull.dx)
      xdif2none=pict.rectFull.dx-dxnone;
    }
   parent->dirtyBorder=1;
  }

 const unsigned char *src[4];
 getCur(newpict.csp,pict,true,src);
 unsigned char *dst[4];
 getNext(newpict.csp,pict,newpict.rectClip,dst,&newpict.rectFull);

 for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
  {
   const unsigned char *src0=src[i]+(ydif2none>>pict.cspInfo.shiftY[i])*stride1[i]+(xdif2none>>pict.cspInfo.shiftX[i]);
   unsigned char       *dst0=dst[i]+(ydif1none>>pict.cspInfo.shiftY[i])*stride2[i]+(xdif1none>>pict.cspInfo.shiftX[i]);
   TffPict::copy(dst0,stride2[i],src0,stride1[i],pict.cspInfo.Bpp*dxnone>>pict.cspInfo.shiftX[i],dynone>>pict.cspInfo.shiftY[i]);
  }
 if (!parent->dirtyBorder) parent->dirtyBorder=1;
}
