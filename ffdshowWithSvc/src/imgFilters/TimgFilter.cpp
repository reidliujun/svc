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
#include "TimgFilter.h"
#include "TimgFilters.h"
#include "Tlibmplayer.h"
#include "Tconvert.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecVideo.h"
#include "ToutputVideoSettings.h"

TimgFilter::TimgFilter(IffdshowBase *Ideci,Tfilters *Iparent):Tfilter(Ideci),parent((TimgFilters*)Iparent),deciV(Ideci),oldBrightness(-1)
{
 csp1=csp2=FF_CSP_NULL;
 convert1=convert2=NULL;
 pictRect.x=UINT_MAX;
}
TimgFilter::~TimgFilter()
{
 free();
}
void TimgFilter::checkBorder(TffPict &pict)
{
 if (pict.rectFull!=pict.rectClip && parent->dirtyBorder)
  {
   int brightness = deciV->getBordersBrightness();
   pict.clearBorder(brightness, deciV->getToutputVideoSettings()->brightness2luma(brightness));
   parent->dirtyBorder=0;
  }
}

void TimgFilter::free(void)
{
 own1.clear();if (convert1) delete convert1;convert1=NULL;
 own2.clear();if (convert2) delete convert2;convert2=NULL;
 own3.clear();
}

void TimgFilter::init(const TffPict &pict,int full,int half)
{
 pictHalf=half;
 Trect newRect=pict.getRect(full,half);
 if (newRect!=pictRect || newRect.sar!=pictRect.sar)
  {
   onSizeChange();
   free();
   pictRect=newRect;
  }
}

bool TimgFilter::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg)
{
 return cfg->is && cfg->show;
}

HRESULT TimgFilter::flush(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 return parent->deliverSample(++it,pict);
}

bool TimgFilter::getOutputFmt(TffPictBase &pict,const TfilterSettingsVideo *cfg)
{
 if (!is(pict,cfg)) return false;
 int supcsp1=getSupportedInputColorspaces(cfg);
 if (((pict.csp&FF_CSPS_MASK)&(supcsp1&FF_CSPS_MASK))==0)
  pict.csp=csp_bestMatch(pict.csp,supcsp1&FF_CSPS_MASK)|(supcsp1&FF_CSP_FLAGS_VFLIP);
 int supcsp2=getSupportedOutputColorspaces(cfg);
 if (((pict.csp&FF_CSPS_MASK)&(supcsp2&FF_CSPS_MASK))==0)
  pict.csp=csp_bestMatch(pict.csp,supcsp2&FF_CSPS_MASK)|(supcsp2&FF_CSP_FLAGS_VFLIP);
 return true;
}

/**
 * get addresses of current picture buffer (Current picture is usually a picture to feed into image filter.)
 * If color space conversion is necessary, own1 will be used.
 * If color space conversion is not necessary, picture buffer will not be copied.
 *
 * @param[in] csp requested color space.
 * @param[in,out] pict current picture (will be modifed if necessary)
 * @param[in] full 
 * @param[in,out] src[4] pointer to the array to receive the addresses
 * *return true if color space is changed.
 * stride will be set to TimgFilter::stride1
 */
bool TimgFilter::getCur(int csp,TffPict &pict,int full,const unsigned char **src[4])
{
 int wasAdj=pict.csp&FF_CSP_FLAGS_YUV_ADJ;
 csp_yuv_adj_to_plane(pict.csp,&pict.cspInfo,pict.rectFull.dy,pict.data,pict.stride);
 csp_yuv_order(pict.csp,pict.data,pict.stride);

 // if color space is different or FF_CSP_FLAGS_YUV_ADJ is different
 if (((csp&FF_CSPS_MASK)&(pict.csp&FF_CSPS_MASK))==0 || ((csp&FF_CSP_FLAGS_YUV_ADJ) && !wasAdj))
  {
   if (!convert1) convert1=new Tconvert(deci,pict.rectFull.dx,pict.rectFull.dy);
   pict.convertCSP(csp_bestMatch(pict.csp,csp&FF_CSPS_MASK)|(csp&(FF_CSP_FLAGS_VFLIP|FF_CSP_FLAGS_YUV_ADJ|FF_CSP_FLAGS_YUV_ORDER)),own1,convert1);
   pict.setRO(false);
  }

 bool cspChanged=csp1!=pict.csp;
 csp1=pict.csp;
 const Trect &r=pictRect; //full?pict.rectFull:pict.rectClip;
 for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
  {
   if (src[i])
    *src[i]=pict.data[i]+(full?0:pict.diff[i])+(pictHalf?r.x*pict.cspInfo.Bpp>>pict.cspInfo.shiftX[i]:0);
   stride1[i]=pict.stride[i];
   dx1[i]=r.dx>>pict.cspInfo.shiftX[i];
   dy1[i]=r.dy>>pict.cspInfo.shiftY[i];
  }
 return cspChanged;
}

/**
 * get addresses of next picture buffer (Next picture buffer is a picture to be written by image filter.)
 * Image buffer is prepared for new picture (own2).
 * Picture will not be copied unless it has borders that are not going to be processed or TimgFilter::pictHalf is set true.
 *
 * @param[in] csp requested color space.
 * @param[in] pict current picture
 * @param[out] pict next picture
 * @param[in] full true: process whole image, false: process rectClip.
 * @param[in,out] dst[4] pointer to the array to receive the addresses
 * @param[in] rect2 If not NULL, pict.rectClip will be replaced by rect2.
 * *return true if color space is changed.
 * stride will be set to TimgFilter::stride2
 */
bool TimgFilter::getNext(int csp,TffPict &pict,int full,unsigned char **dst[4],const Trect *rect2)
{
 if (rect2) pict.rectFull=pict.rectClip=*rect2;
 const Trect &r=pictRect;//full?pict.rectFull:pict.rectClip;
 TffPict pictN;
 if (((csp&FF_CSPS_MASK)&(pict.csp&FF_CSPS_MASK))==0)
  {
   pict.convertCSP(csp_bestMatch(pict.csp,csp&FF_CSPS_MASK)|(csp&FF_CSP_FLAGS_YUV_ADJ),own2);
   pict.setRO(false);
   pictN=pict;
  }
 else
  {
   pictN=pict;
   pictN.convertCSP(pict.csp|(csp&FF_CSP_FLAGS_YUV_ADJ),own2);
  }
 bool copyBorder=!rect2 && !full && pict.rectClip!=pict.rectFull;
 for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
  {
   if (dst[i])
    {
     if (copyBorder)
      pictN.copyBorder(pict,i);
     if (pictHalf && !rect2)
      TffPict::copy(pictN.data[i]+(full?0:pictN.diff[i]),pictN.stride[i],pict.data[i]+(full?0:pict.diff[i]),pict.stride[i],(pictRect.dx>>pictN.cspInfo.shiftX[i])*pict.cspInfo.Bpp,pictRect.dy>>pictN.cspInfo.shiftY[i]);
     pict.diff[i]=pictN.diff[i];
     pict.ro[i]=false;
     pict.data[i]=pictN.data[i];
     pict.stride[i]=pictN.stride[i];
     *dst[i]=pict.data[i]+(full?0:pict.diff[i]);
     if (pictHalf && !rect2)
      *dst[i]+=(pictRect.x>>pict.cspInfo.shiftX[i])*pict.cspInfo.Bpp;
    }
   stride2[i]=pict.stride[i];
   dx2[i]=r.dx>>pict.cspInfo.shiftX[i];
   dy2[i]=r.dy>>pict.cspInfo.shiftY[i];
  }
 bool cspChanged=csp2!=pict.csp;
 csp2=pictN.csp;
 return cspChanged;
}

bool TimgFilter::getNext(int csp,TffPict &pict,const Trect &clipRect,unsigned char **dst[4],const Trect *rect2)
{
 bool cspChanged=getNext(csp,pict,true,dst,rect2);
 pict.rectClip=clipRect;
 pict.calcDiff();
 for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
  if (dst[i])
   *dst[i]+=pict.diff[i];
 int brightness=deciV->getBordersBrightness();
 if (cspChanged || oldBrightness != brightness)
  {
   oldBrightness = brightness;
   pict.clearBorder(brightness, deciV->getToutputVideoSettings()->brightness2luma(brightness));
  }
 return cspChanged;
}

/**
 * get addresses of next picture buffer (Next picture buffer is a picture to be written by image filter.)
 * Image buffer is prepared for new picture (\p buf).
 * Picture will be copied depending on \p copy.
 *
 * @param[in] csp requested color space.
 * @param[in] pict current picture
 * @param[out] pict next picture
 * @param[in] full true: process whole image, false: process rectClip.
 * @param[in] copy copy mode. See the header file.
 * @param[in,out] dst[4] pointer to the array to receive the addresses
 * @param buf[in] next picture buffer
 * @param[in] rect2 If not NULL, pict.rectClip will be replaced by rect2.
 * *return true if color space is changed.
 * stride will be set to TimgFilter::stride2
 */
bool TimgFilter::getCurNext(int csp,TffPict &pict,int full,int copy,unsigned char **dst[4],Tbuffer &buf)
{
 bool flip=false;
 csp_yuv_adj_to_plane(pict.csp,&pict.cspInfo,pict.rectFull.dy,pict.data,pict.stride);csp_yuv_order(pict.csp,pict.data,pict.stride);
 TffPict pictN;
 if (((csp&FF_CSPS_MASK)&(pict.csp&FF_CSPS_MASK))==0)
  {
   bool flip1 = !!((pict.csp ^ csp) & FF_CSP_FLAGS_VFLIP);
   int cspOptionsRgbInterlaceMode = deci->getParam2(IDFF_cspOptionsRgbInterlaceMode);
   if (!convert2 || cspOptionsRgbInterlaceMode != old_cspOptionsRgbInterlaceMode)
    {
     old_cspOptionsRgbInterlaceMode = cspOptionsRgbInterlaceMode;
     convert2=new Tconvert(deci,pict.rectFull.dx,pict.rectFull.dy);
    }
   pict.convertCSP((csp_bestMatch(pict.csp, csp & FF_CSPS_MASK) & ~FF_CSP_FLAGS_VFLIP) | (csp & (FF_CSP_FLAGS_YUV_ADJ | FF_CSP_FLAGS_INTERLACED)), buf, convert2);
   pict.setRO(false);
   pictN=pict;
   if (flip1)
    pictN.rectClip.y = pictN.rectFull.dy - pictN.rectClip.y - pictN.rectClip.dy;
  }
 else
  {
   pictN = pict;
   flip = !!((pict.csp ^ csp) & FF_CSP_FLAGS_VFLIP);
   if (flip)
    pictN.rectClip.y = pictN.rectFull.dy - pictN.rectClip.y - pictN.rectClip.dy;
   pictN.convertCSP((pict.csp & ~FF_CSP_FLAGS_VFLIP) | (csp & (FF_CSP_FLAGS_YUV_ADJ | FF_CSP_FLAGS_VFLIP)),buf);
  }
 const Trect r=pictRect;//=full?pict.rectFull:pict.rectClip;
 if (copy==COPYMODE_DEF)
  copy = full ? COPYMODE_FULL : COPYMODE_CLIP;
 if (flip)
  copy = COPYMODE_FULL;
 bool copyBorder=!full && pict.rectClip!=pict.rectFull;
 for (unsigned int i=0;i<pictN.cspInfo.numPlanes;i++)
  if (dst[i])
   {
    dx1[i]=dx2[i]=r.dx>>pict.cspInfo.shiftX[i];
    dy1[i]=dy2[i]=r.dy>>pict.cspInfo.shiftY[i];
    if (pict.ro[i] || flip)
     {
      pict.ro[i]=false;
      switch (copy)
       {
        case COPYMODE_CLIP:
         if (copyBorder) pictN.copyBorder(pict,i);
         TffPict::copy(pictN.data[i]+pictN.diff[i],pictN.stride[i],pict.data[i]+pict.diff[i],pict.stride[i],(pict.rectClip.dx>>pict.cspInfo.shiftX[i])*pict.cspInfo.Bpp,pict.rectClip.dy>>pict.cspInfo.shiftY[i],flip);
         break;
        case COPYMODE_FULL:
         TffPict::copy(pictN.data[i],pictN.stride[i],pict.data[i],pict.stride[i],(pict.rectFull.dx>>pict.cspInfo.shiftX[i])*pict.cspInfo.Bpp,pict.rectFull.dy>>pict.cspInfo.shiftY[i],flip);
         break;
        case COPYMODE_NO:
         if (pictHalf)
          TffPict::copy(pictN.data[i],pictN.stride[i],pict.data[i],pict.stride[i],(pictRect.dx>>pict.cspInfo.shiftX[i])*pict.cspInfo.Bpp,pictRect.dy>>pict.cspInfo.shiftY[i],flip);
       }
      pict.data[i]=pictN.data[i];
      pict.stride[i]=pictN.stride[i];
      pict.diff[i]=pictN.diff[i];
     }
    stride2[i]=pict.stride[i];
    *dst[i]=pict.data[i]+(full?0:pict.diff[i])+(pictHalf?r.x*pict.cspInfo.Bpp>>pict.cspInfo.shiftX[i]:0);
   }
 bool cspChanged=csp2!=pict.csp;
 pict.csp=csp2=pictN.csp;
 pict.rectClip.y = pictN.rectClip.y;
 if (full) checkBorder(pict);
 return cspChanged;
}

bool TimgFilter::screenToPict(CPoint &pt)
{
 GetCursorPos(&pt);
 int wx,wy;
 if (SUCCEEDED(deciV->getVideoWindowPos(&wx,&wy,NULL,NULL)))
  {
   pt-=CPoint(wx,wy);
   CRect dst;
   if (SUCCEEDED(deciV->getVideoDestRect(&dst)))
    {
     pt-=CPoint(dst.TopLeft());
     pt.scale(dx2[0],dst.Width(),dy2[0],dst.Height());
     if (isIn(pt.x,0L,long(dx2[0]-1)) && isIn(pt.y,0L,long(dy2[0]-1)))
      return true;
    }
  }
 return false;
}
