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
#include "TimgFilterOffset.h"
#include "ToffsetSettings.h"
#include "TsampleFormat.h"

//================================= TimgFilterOffset =================================
TimgFilterOffset::TimgFilterOffset(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 Y_X=Y_Y=U_X=U_Y=V_X=V_Y=-1000;
}
void TimgFilterOffset::onSizeChange(void)
{
 Y_X=Y_Y=U_X=U_Y=V_X=V_Y=-1000;
}
void TimgFilterOffset::offset(int c,const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,unsigned int dx,unsigned int dy,int offsetX,int offsetY)
{
 unsigned int x1,x2;
 if (offsetX>0)
  {
   x1=offsetX;x2=0;
  }
 else
  {
   offsetX=abs(offsetX);
   x1=0;x2=offsetX;
  }
 unsigned int y1,y2;
 if (offsetY>0)
  {
   y1=offsetY;y2=0;
  }
 else
  {
   offsetY=abs(offsetY);
   y1=0;y2=offsetY;
  }
 for (unsigned char *dstCx=dst+(x2?dx-offsetX:0),*dstCXend=dstCx+dy*dstStride;dstCx<dstCXend;dstCx+=dstStride)
  memset(dstCx,c,offsetX);
 for (unsigned char *dstCy=dst+(y1?dy-offsetY:0)*dstStride,*dstCYend=dstCy+offsetY*dstStride;dstCy<dstCYend;dstCy+=dstStride)
  memset(dstCy,c,dx);
 src+=y1*srcStride;dst+=y2*dstStride;
 for (unsigned int y=0;y<dy-offsetY;src+=srcStride,dst+=dstStride,y++)
  memcpy(dst+x1,src+x2,dx-offsetX);
}
bool TimgFilterOffset::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const ToffsetSettings *cfg=(const ToffsetSettings*)cfg0;
 return super::is(pict,cfg) && (cfg->Y_X || cfg->Y_Y || cfg->U_X || cfg->U_Y || cfg->V_X || cfg->V_Y);
}
HRESULT TimgFilterOffset::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const ToffsetSettings *cfg=(const ToffsetSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   if (cfg->Y_X || cfg->Y_Y)
    {
     const unsigned char *srcY;
     getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&srcY,NULL,NULL,NULL);
     unsigned char *dstY;
     getNext(csp1,pict,cfg->full,&dstY,NULL,NULL,NULL);
     if (cfg->Y_X!=Y_X || cfg->Y_Y!=Y_Y)
      {
       Y_X=cfg->Y_X;Y_Y=cfg->Y_Y;
       pict.clear(cfg->full,0);
      }
     offset(0,srcY,stride1[0],dstY,stride2[0],dx1[0],dy1[0],cfg->Y_X,-cfg->Y_Y);
    }
   if (cfg->U_X || cfg->U_Y)
    {
     const unsigned char *srcU;
     getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,NULL,&srcU,NULL,NULL);
     unsigned char *dstU;
     getNext(csp1,pict,cfg->full,NULL,&dstU,NULL,NULL);
     if (cfg->U_X!=U_X || cfg->U_Y!=U_Y)
      {
       U_X=cfg->U_X;U_Y=cfg->U_Y;
       pict.clear(cfg->full,1);
      }
     offset(128,srcU,stride1[1],dstU,stride2[1],dx1[1],dy1[1],cfg->U_X>>pict.cspInfo.shiftX[1],-cfg->U_Y>>pict.cspInfo.shiftY[1]);
    }
   if (cfg->V_X || cfg->V_Y)
    {
     const unsigned char *srcV;
     getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,NULL,NULL,&srcV,NULL);
     unsigned char *dstV;
     getNext(csp1,pict,cfg->full,NULL,NULL,&dstV,NULL);
     if (cfg->V_X!=V_X || cfg->V_Y!=V_Y)
      {
       V_X=cfg->V_X;V_Y=cfg->V_Y;
       pict.clear(cfg->full,2);
      }
     offset(128,srcV,stride1[2],dstV,stride2[2],dx1[2],dy1[2],cfg->V_X>>pict.cspInfo.shiftX[2],-cfg->V_Y>>pict.cspInfo.shiftY[2]);
    }
  }
 return parent->deliverSample(++it,pict);
}

//================================== TimgFilterFlip ==================================
TimgFilterFlip::TimgFilterFlip(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
}
bool TimgFilterFlip::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const ToffsetSettings *cfg=(const ToffsetSettings*)cfg0;
 return super::is(pict,cfg) && cfg->flip;
}
HRESULT TimgFilterFlip::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const ToffsetSettings *cfg=(const ToffsetSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   unsigned char *ptr[4];
   getCurNext(pict.csp,pict,cfg->full,COPYMODE_DEF,ptr);
   for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
    {
     unsigned char *src=ptr[i],*dst=ptr[i]+stride2[i]*(dy2[i]-1);
     unsigned int dx=dx2[i]*pict.cspInfo.Bpp,dy=dy2[i]/2;
     void *tmp=_alloca(dx);
     for (unsigned int y=0;y<dy;y++,src+=stride2[i],dst-=stride2[i])
      {
       memcpy(tmp,src,dx);
       memcpy(src,dst,dx);
       memcpy(dst,tmp,dx);
      }
    }
  }
 return parent->deliverSample(++it,pict);
}

//================================== TimgFilterMirror ==================================
TimgFilterMirror::TimgFilterMirror(IffdshowBase *Ideci,Tfilters *Iparent):
 TimgFilter(Ideci,Iparent),
 mirrorFc(NULL)
{
}
bool TimgFilterMirror::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const ToffsetSettings *cfg=(const ToffsetSettings*)cfg0;
 return super::is(pict,cfg) && cfg->mirror;
}
template<class pixel_t> void TimgFilterMirror::mirror(int i,unsigned int dx,unsigned char *line)
{
 pixel_t *src=(pixel_t*)line;
 pixel_t *dst=(pixel_t*)line+(dx2[i]-1);
 for (unsigned int x=0;x<dx;x++,src++,dst--)
  std::swap(*src,*dst);
}
HRESULT TimgFilterMirror::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const ToffsetSettings *cfg=(const ToffsetSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   unsigned char *ptr[4];
   bool cspChanged=getCurNext(pict.csp,pict,cfg->full,COPYMODE_DEF,ptr);
   if (cspChanged)
    switch (pict.cspInfo.Bpp)
     {
      case 1:mirrorFc=&TimgFilterMirror::mirror<int8_t>;break;
      case 2:mirrorFc=&TimgFilterMirror::mirror<int16_t>;
      case 3:mirrorFc=&TimgFilterMirror::mirror<int24_t>;break;
      case 4:mirrorFc=&TimgFilterMirror::mirror<int32_t>;break;
      default:mirrorFc=NULL;break;
     }
   if (csp_isYUVpacked(pict.csp))
    {
     unsigned char *src=ptr[0],*dst=ptr[0]+(dx2[0]-1)*2;
     int dx=dx2[0];
     if (pict.cspInfo.packedLumaOffset==0 && pict.cspInfo.packedChromaOffset==1)
      for (unsigned int y=0;y<dy2[0];y++,src+=stride2[0],dst+=stride2[0])
       for (int x=0;x<dx;x+=4)
        {
         std::swap(src[x  ],dst[-x+2]);
         std::swap(src[x+3],dst[-x+1]);
         std::swap(src[x+2],dst[-x  ]);
         std::swap(src[x+1],dst[-x+3]);
        }
     else
      for (unsigned int y=0;y<dy2[0];y++,src+=stride2[0],dst+=stride2[0])
       for (int x=0;x<dx;x+=4)
        {
         std::swap(src[x  ],dst[-x+2]);
         std::swap(src[x+3],dst[-x+1]);
         std::swap(src[x+2],dst[-x  ]);
         std::swap(src[x+1],dst[-x+3]);
        }
    }
   else
    if (mirrorFc)
     for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
      {
       unsigned int dx=dx2[i]/2;
       for (unsigned int y=0;y<dy2[i];y++,ptr[i]+=stride2[i])
        (this->*mirrorFc)(i,dx,ptr[i]);
      }
  }
 return parent->deliverSample(++it,pict);
}
