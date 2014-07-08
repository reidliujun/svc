/*
 * Copyright (c) 2004-2006 Milan Cutka
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
#include "TimgFilterBitmap.h"
#include "TbitmapSettings.h"
#include "simd.h"
#include "Tconfig.h"

template<class _mm> void TimgFilterBitmap::blend(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength)
{
 typename _mm::__m strength64=_mm::set1_pi16(short(strength/2)),invstrength64=_mm::set1_pi16(short(invstrength/2)),m0=_mm::setzero_si64(),m128=_mm::set1_pi16(64);
 for (unsigned int i=0;i<cspInfo.numPlanes;i++)
  for (unsigned int y=0;y<dy[i];y++,dst[i]+=dststride[i],src[i]+=srcstride[i])
   {
    int x=0,cnt=dx[i];
    const int cnt3=cnt-_mm::size/2+1;
    for (;x<cnt3;x+=_mm::size/2)
     {
      typename _mm::__m dst8=_mm::unpacklo_pi8(_mm::load2(dst[i]+x),m0);
      typename _mm::__m src8=_mm::unpacklo_pi8(_mm::load2(src[i]+x),m0);
      dst8=_mm::mullo_pi16(dst8,invstrength64);
      src8=_mm::mullo_pi16(src8,strength64);
      dst8=_mm::add_pi16(dst8,src8);
      dst8=_mm::add_pi16(dst8,m128);
      dst8=_mm::srai_pi16(dst8,7);
      _mm::store2(dst[i]+x,_mm::packs_pu16(dst8,m0));
     }
    for (;x<cnt;x++)
     dst[i][x]=uint8_t((invstrength*dst[i][x]+strength*src[i][x]+128)/256);
   }
 _mm::empty();
}
template<class _mm> void TimgFilterBitmap::darken(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength)
{
 typename _mm::__m strength64=_mm::set1_pi16(short(strength/2)),invstrength64=_mm::set1_pi16(short(invstrength/2)),m0=_mm::setzero_si64(),m128=_mm::set1_pi16(64);
 if (cspInfo.numPlanes==3)
  for (unsigned int i=1;i<3;i++)
   for (unsigned int y=0;y<dy[i];y++,dst[i]+=dststride[i],src[i]+=srcstride[i])
    for (unsigned int x=0;x<dx[i];x++)
     if (src[0][(x<<cspInfo.shiftX[i])+(y<<cspInfo.shiftY[i])*srcstride[0]]<dst[0][(x<<cspInfo.shiftX[i])+(y<<cspInfo.shiftY[i])*dststride[0]])
      dst[i][x]=uint8_t((invstrength*dst[i][x]+strength*src[i][x]+128)/256);
 for (unsigned int y=0;y<dy[0];y++,dst[0]+=dststride[0],src[0]+=srcstride[0])
  {
   int x=0,cnt=dx[0];
   const int cnt3=cnt-_mm::size/2+1;
   for (;x<cnt3;x+=_mm::size/2)
    {
     typename _mm::__m dst8=_mm::unpacklo_pi8(_mm::load2(dst[0]+x),m0);
     typename _mm::__m src8=_mm::unpacklo_pi8(_mm::load2(src[0]+x),m0);
     typename _mm::__m mask=_mm::cmpgt_pi16(dst8,src8);
     _mm::store2(dst[0]+x,_mm::packs_pu16(_mm::or_si64(_mm::and_si64(mask,_mm::srai_pi16(_mm::add_pi16(_mm::add_pi16(_mm::mullo_pi16(invstrength64,dst8),_mm::mullo_pi16(strength64,src8)),m128),7)),_mm::andnot_si64(mask,dst8)),m0));
    }
   for (;x<cnt;x++)
    if (src[0][x]<dst[0][x])
     dst[0][x]=uint8_t((invstrength*dst[0][x]+strength*src[0][x]+128)/256);
  }
 _mm::empty();
}
template<class _mm> void TimgFilterBitmap::lighten(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength)
{
 typename _mm::__m strength64=_mm::set1_pi16(short(strength/2)),invstrength64=_mm::set1_pi16(short(invstrength/2)),m0=_mm::setzero_si64(),m128=_mm::set1_pi16(64);
 if (cspInfo.numPlanes==3)
  for (unsigned int i=1;i<3;i++)
   for (unsigned int y=0;y<dy[i];y++,dst[i]+=dststride[i],src[i]+=srcstride[i])
    for (unsigned int x=0;x<dx[i];x++)
     if (src[0][(x<<cspInfo.shiftX[i])+(y<<cspInfo.shiftY[i])*srcstride[0]]>dst[0][(x<<cspInfo.shiftX[i])+(y<<cspInfo.shiftY[i])*dststride[0]])
      dst[i][x]=uint8_t((invstrength*dst[i][x]+strength*src[i][x]+128)/256);
 for (unsigned int y=0;y<dy[0];y++,dst[0]+=dststride[0],src[0]+=srcstride[0])
  {
   int x=0,cnt=dx[0];
   const int cnt3=cnt-_mm::size/2+1;
   for (;x<cnt3;x+=_mm::size/2)
    {
     typename _mm::__m dst8=_mm::unpacklo_pi8(_mm::load2(dst[0]+x),m0);
     typename _mm::__m src8=_mm::unpacklo_pi8(_mm::load2(src[0]+x),m0);
     typename _mm::__m mask=_mm::cmpgt_pi16(src8,dst8);
     _mm::store2(dst[0]+x,_mm::packs_pu16(_mm::or_si64(_mm::and_si64(mask,_mm::srai_pi16(_mm::add_pi16(_mm::add_pi16(_mm::mullo_pi16(invstrength64,dst8),_mm::mullo_pi16(strength64,src8)),m128),7)),_mm::andnot_si64(mask,dst8)),m0));
    }
   for (;x<cnt;x++)
    if (src[0][x]>dst[0][x])
     dst[0][x]=uint8_t((invstrength*dst[0][x]+strength*src[0][x]+128)/256);
  }
 _mm::empty();
}
template<class _mm> void TimgFilterBitmap::add(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength)
{
 typename _mm::__m strength64=_mm::set1_pi16(short(strength/2)),invstrength64=_mm::set1_pi16(short(127*invstrength/2)),m0=_mm::setzero_si64(),m127=_mm::set1_pi16(127);
 if (cspInfo.numPlanes==3)
  for (unsigned int i=1;i<3;i++)
   for (unsigned int y=0;y<dy[i];y++,dst[i]+=dststride[i],src[i]+=srcstride[i])
    {
     int x=0,cnt=dx[i];
     const int cnt3=cnt-_mm::size/2+1;
     for (;x<cnt3;x+=_mm::size/2)
      {
       typename _mm::__m dst8=_mm::unpacklo_pi8(_mm::load2(dst[i]+x),m0);
       typename _mm::__m src8=_mm::unpacklo_pi8(_mm::load2(src[i]+x),m0);
       src8=_mm::mullo_pi16(strength64,src8);
       src8=_mm::add_pi16(invstrength64,src8);
       src8=_mm::srai_pi16(src8,7);
       dst8=_mm::add_pi16(dst8,src8);
       dst8=_mm::sub_pi16(dst8,m127);
       _mm::store2(dst[i]+x,_mm::packs_pu16(dst8,m0));
      }
     for (;x<cnt;x++)
      dst[i][x]=uint8_t(dst[i][x]+(((127*invstrength)+(strength*src[i][x]))>>8)-127);
    }
 for (unsigned int y=0;y<dy[0];y++,dst[0]+=dststride[0],src[0]+=srcstride[0])
  {
   int x=0,cnt=dx[0];
   const int cnt3=cnt-_mm::size/2+1;
   for (;x<cnt3;x+=_mm::size/2)
    {
     typename _mm::__m dst8=_mm::unpacklo_pi8(_mm::load2(dst[0]+x),m0);
     typename _mm::__m src8=_mm::unpacklo_pi8(_mm::load2(src[0]+x),m0);
     typename _mm::__m Y=_mm::add_pi16(dst8,_mm::srai_pi16(_mm::mullo_pi16(strength64,src8),7));
     _mm::store2(dst[0]+x,_mm::packs_pu16(Y,m0));
    }
   for (;x<cnt;x++)
    {
     int Y=dst[0][x]+((strength*src[0][x])>>8);
     if (Y>255) Y=255;
     dst[0][x]=uint8_t(Y);
    }
  }
 _mm::empty();
}
template<class _mm> void TimgFilterBitmap::softlight(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength)
{
 typename _mm::__m strength64=_mm::set1_pi16((short)strength/2),invstrength64=_mm::set1_pi16((short)invstrength/2),m0=_mm::setzero_si64(),m128=_mm::set1_pi16(128);
 for (unsigned int i=0;i<cspInfo.numPlanes;i++)
  for (unsigned int y=0;y<dy[i];y++,dst[i]+=dststride[i],src[i]+=srcstride[i])
   {
    int x=0,cnt=dx[i];
    const int cnt3=cnt-_mm::size/2+1;
    for (;x<cnt3;x+=_mm::size/2)
     {
      typename _mm::__m dst8=_mm::unpacklo_pi8(_mm::load2(dst[i]+x),m0);
      typename _mm::__m src8=_mm::unpacklo_pi8(_mm::load2(src[i]+x),m0);
      typename _mm::__m Y=_mm::unpacklo_pi8(_mm::packs_pu16(_mm::sub_pi16(_mm::add_pi16(src8,dst8),m128),m0),m0);
      dst8=_mm::add_pi16(_mm::mullo_pi16(Y,strength64), _mm::mullo_pi16(dst8,invstrength64));
      dst8=_mm::srai_pi16(dst8,7);
      _mm::store2(dst[i]+x,_mm::packs_pu16(dst8,m0));
     }
    for (;x<cnt;x++)
     {
      int Y=limit_uint8(dst[i][x]+src[i][x]-127);
      dst[i][x]=limit_uint8((Y*strength+invstrength*dst[i][x])>>8);
     }
   }
 _mm::empty();
}
template<class _mm> void TimgFilterBitmap::exclusion(const TcspInfo &cspInfo,const unsigned int dx[3],const unsigned int dy[3],unsigned char *dst[3],const stride_t dststride[3],const unsigned char *src[3],const stride_t srcstride[3],int strength,int invstrength)
{
 typename _mm::__m strength64=_mm::set1_pi16(short(strength/2)),invstrength64=_mm::set1_pi16(short(invstrength/2)),m0=_mm::setzero_si64(),m255=_mm::set1_pi16(255);
 for (unsigned int i=0;i<3;i++)
  for (unsigned int y=0;y<dy[i];y++,dst[i]+=dststride[i],src[i]+=srcstride[i])
   {
    int x=0,cnt=dx[i];
    const int cnt3=cnt-_mm::size/2+1;
    for (;x<cnt3;x+=_mm::size/2)
     {
      typename _mm::__m dst8=_mm::unpacklo_pi8(_mm::load2(dst[i]+x),m0);
      typename _mm::__m src8=_mm::unpacklo_pi8(_mm::load2(src[i]+x),m0);
      typename _mm::__m Y=_mm::add_pi16(_mm::srli_pi16(_mm::mullo_pi16(_mm::xor_si64(dst8,m255),src8),1) , _mm::srli_pi16(_mm::mullo_pi16(_mm::xor_si64(src8,m255),dst8),1));
      Y=_mm::srai_pi16(Y,7);
      Y=_mm::srai_pi16(_mm::add_pi16(_mm::mullo_pi16(Y,strength64),_mm::mullo_pi16(invstrength64,dst8)),7);
      _mm::store2(dst[i]+x,_mm::packs_pu16(Y,m0));
     }
    for (;x<cnt;x++)
    {
     int Y=((int)(dst[i][x]^0xff)*src[i][x] + (int)(src[i][x]^0xff)*dst[i][x])>>8;
     Y=((Y*strength) + (invstrength*dst[i][x]))>>8;
     dst[i][x]=limit_uint8(Y);
    }
  }
 _mm::empty();
}

template<class _mm> TimgFilterBitmap::Tblendplane TimgFilterBitmap::getBlend(int mode)
{
 switch (mode)
  {
   case TbitmapSettings::MODE_BLEND:
   default:return blend<_mm>;
   case TbitmapSettings::MODE_DARKEN:return darken<_mm>;
   case TbitmapSettings::MODE_LIGHTEN:return lighten<_mm>;
   case TbitmapSettings::MODE_ADD:return add<_mm>;
   case TbitmapSettings::MODE_SOFTLIGHT:return softlight<_mm>;
   case TbitmapSettings::MODE_EXCLUSION:return exclusion<_mm>;
  }
}

void TimgFilterBitmap::TrenderedSubtitleLineBitmap::print(int startx, int starty /* not used */, unsigned int dx[3],int dy1[3],unsigned char *dstLn[3],const stride_t stride[3],const unsigned char *bmp[3],const unsigned char *msk[3],REFERENCE_TIME rtStart) const
{
 blend(pict->cspInfo,dx,dy,dstLn,stride,bmp,bmpmskstride,cfg->strength,255-cfg->strength);
}

TimgFilterBitmap::TimgFilterBitmap(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent),l(&w),ls(&l)
{
 bitmap=NULL;
 oldflnm[0]='\0';
 oldmode=-1;
}
TimgFilterBitmap::~TimgFilterBitmap()
{
 if (bitmap) delete bitmap;
}

bool TimgFilterBitmap::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TbitmapSettings *cfg=(const TbitmapSettings*)cfg0;
 return super::is(pict,cfg) && cfg->strength && cfg->flnm[0];
}

HRESULT TimgFilterBitmap::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TbitmapSettings *cfg=(const TbitmapSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   unsigned char *dst[4];
   bool cspChanged=getCurNext(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,COPYMODE_DEF,dst);

   if (!bitmap || cspChanged || stricmp(oldflnm,cfg->flnm)!=0)
    {
     ff_strncpy(oldflnm, cfg->flnm, countof(oldflnm));
     if (bitmap) delete bitmap;
     bitmap=new TffPict(csp2,cfg->flnm,bitmapbuf,deci);
     for (int i=0;i<3;i++)
      {
       w.dx[i]=bitmap->rectFull.dx>>bitmap->cspInfo.shiftX[i];
       w.dy[i]=bitmap->rectFull.dy>>bitmap->cspInfo.shiftY[i];
       w.bmp[i]=bitmap->data[i];
       w.bmpmskstride[i]=bitmap->stride[i];
      }
     w.dxCharY=w.dx[0];w.dyCharY=w.dy[0];
    }

   if (bitmap->rectFull.dx!=0)
    {
     if (oldmode!=cfg->mode)
    #ifdef __SSE2__
      if (Tconfig::cpu_flags&FF_CPU_SSE2)
       w.blend=getBlend<Tsse2>(oldmode=cfg->mode);
      else
    #endif
       w.blend=getBlend<Tmmx>(oldmode=cfg->mode);
     TrenderedSubtitleLines::TprintPrefs prefs(deci);
     prefs.dst=dst;
     prefs.stride=stride2;
     prefs.shiftX=pict.cspInfo.shiftX;prefs.shiftY=pict.cspInfo.shiftY;
     prefs.dx=dx2[0];prefs.dy=dy2[0];
     prefs.xpos=cfg->posx;prefs.ypos=cfg->posy;if (cfg->posmode==1) {prefs.xpos*=-1;prefs.ypos*=-1;}
     prefs.align=cfg->align;
     prefs.linespacing=100;
     prefs.cspBpp = pict.cspInfo.Bpp;
     prefs.csp=pict.csp;
     w.pict=&pict;
     w.cfg=cfg;
     ls.print(prefs);
    }
  }
 return parent->deliverSample(++it,pict);
}
