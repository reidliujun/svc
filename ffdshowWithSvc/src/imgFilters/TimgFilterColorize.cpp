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
#include "TimgFilterColorize.h"
#include "TpictPropSettings.h"
#include "simd.h"
#include "Tconfig.h"

TimgFilterColorize::TimgFilterColorize(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
#ifdef __SSE2__
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  colorizeFc=&TimgFilterColorize::colorize<Tsse2>;
 else
#endif
  colorizeFc=&TimgFilterColorize::colorize<Tmmx>;
}

bool TimgFilterColorize::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TpictPropSettings *cfg=(const TpictPropSettings*)cfg0;
 return super::is(pict,cfg) && cfg->colorizeStrength;
}

template<class _mm> void TimgFilterColorize::colorize(TffPict &pict,const TpictPropSettings *cfg,const YUVcolor &c0,const int c[3])
{
 unsigned char *dst[4];
 typename _mm::__m m0=_mm::setzero_si64();
 if (!cfg->colorizeChromaonly)
  {
   getCurNext(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,COPYMODE_DEF,&dst[0],NULL,NULL,NULL);
   typename _mm::__m strenght64_255=_mm::set1_pi16((short)(255-cfg->colorizeStrength));
   typename _mm::__m c0_64=_mm::set1_pi16(short(cfg->colorizeStrength*c0.Y));
   int strenght=cfg->colorizeStrength;
   for (unsigned int y=0;y<dy2[0];y++,dst[0]+=stride2[0])
    {
     int x=0;
     //for (;intptr_t(dst[0]+x)&15 && x<dx2[0];x++)
      //dst[0][x]=uint8_t(((255-strenght)*dst[0][x]+strenght*c[0])/256);
     const int dx202=dx2[0]-_mm::size/2;
     for (;x<=dx202;x+=_mm::size/2)
      _mm::store2(dst[0]+x,_mm::packs_pu16(_mm::srli_pi16(_mm::add_pi16(_mm::mullo_pi16(strenght64_255,_mm::unpacklo_pi8(_mm::load2(dst[0]+x),m0)),c0_64),8),m0));
     for (;x<(int)dx2[0];x++)
      dst[0][x]=uint8_t(((255-strenght)*dst[0][x]+strenght*c[0])/256);
    }
  }
 getCurNext(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,COPYMODE_DEF,NULL,&dst[1],&dst[2],NULL);
 int strenght=std::min(cfg->colorizeStrength,253);
 typename _mm::__m strenght64_255=_mm::set1_pi16((short)(255-strenght));
 typename _mm::__m m127=_mm::set1_pi16(127);
 for (unsigned int i=1;i<pict.cspInfo.numPlanes;i++)
  {
   typename _mm::__m c0_64=_mm::set1_pi16(short(strenght*c[i]));
   for (unsigned int y=0;y<dy2[i];y++,dst[i]+=stride2[i])
    {
     int x=0;
     const int dx2i2=dx2[i]-_mm::size/2;
     for (;x<=dx2i2;x+=_mm::size/2)
      _mm::store2(dst[i]+x,_mm::packs_pu16(_mm::add_pi16(_mm::srai_pi16(_mm::add_pi16(_mm::mullo_pi16(strenght64_255,_mm::sub_pi16(_mm::unpacklo_pi8(_mm::load2(dst[i]+x),m0),m127)),c0_64),8),m127),m0));
     for (;x<(int)dx2[i];x++)
      dst[i][x]=uint8_t(((255-strenght)*(dst[i][x]-127)+strenght*c[i])/256+127);
    }
  }
 _mm::empty();
}

HRESULT TimgFilterColorize::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TpictPropSettings *cfg=(const TpictPropSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   YUVcolor c0=cfg->colorizeColor;int c[3]={c0.Y,c0.U*2,c0.V*2};
   (this->*colorizeFc)(pict,cfg,c0,c);
  }
 return parent->deliverSample(++it,pict);
}
