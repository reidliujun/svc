/*
 * Copyright (c) 2002-2006 Milan Cutka
 * based on AviSynth filter by Donald A. Graft
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
#include "TimgFilterChroma.h"
#include "TpictPropSettings.h"
#include "simd.h"
#include "Tconfig.h"

TimgFilterChroma::TimgFilterChroma(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
#ifdef __SSE2__
 if (Tconfig::cpu_flags&FF_CPU_SSE2)
  chromaFc=&TimgFilterChroma::chroma<Tsse2>;
 else
#endif
  chromaFc=&TimgFilterChroma::chroma<Tmmx>;
 for (int i=-180,ii=0;i<=180;ii++,i++)
  {
   double Hue=(i*3.1415926)/180.0;
   hueSin[ii]=short(sin(Hue)*128);
   hueCos[ii]=short(cos(Hue)*128);
  }
}

bool TimgFilterChroma::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TpictPropSettings *cfg=(const TpictPropSettings*)cfg0;
 return super::is(pict,cfg) && (cfg->hue!=cfg->hueDef || cfg->saturation!=cfg->saturationDef);
}

template<class _mm> void TimgFilterChroma::chroma(const TpictPropSettings *cfg,const unsigned char *srcU,const unsigned char *srcV,unsigned char *dstU,unsigned char *dstV)
{
 typename _mm::__m Sin64=_mm::set1_pi16(hueSin[cfg->hue+180]);
 typename _mm::__m Cos64=_mm::set1_pi16(hueCos[cfg->hue+180]);
 typename _mm::__m Sat64=_mm::set1_pi16((short)cfg->saturation);
 typename _mm::__m m0=_mm::setzero_si64(),m128=_mm::set1_pi16(128);
 for (const unsigned char *srcUend=srcU+stride1[1]*dy1[1];srcU!=srcUend;srcU+=stride1[1],srcV+=stride1[2],dstU+=stride2[1],dstV+=stride2[2])
  {
   int x=0;
   const int dx113=dx1[1]-_mm::size/2+1;
   for (;x<dx113;x+=_mm::size/2)
    {
     typename _mm::__m u=_mm::subs_pi16(_mm::unpacklo_pi8(_mm::load2(srcU+x),m0),m128);
     typename _mm::__m v=_mm::subs_pi16(_mm::unpacklo_pi8(_mm::load2(srcV+x),m0),m128);
     typename _mm::__m u1=_mm::add_pi16(_mm::srai_pi16(_mm::mullo_pi16(_mm::add_pi16(_mm::srai_pi16(_mm::mullo_pi16(u,Cos64),7),_mm::srai_pi16(_mm::mullo_pi16(v,Sin64),7)),Sat64),6),m128);
     typename _mm::__m v1=_mm::add_pi16(_mm::srai_pi16(_mm::mullo_pi16(_mm::sub_pi16(_mm::srai_pi16(_mm::mullo_pi16(v,Cos64),7),_mm::srai_pi16(_mm::mullo_pi16(u,Sin64),7)),Sat64),6),m128);
     _mm::store2(dstU+x,_mm::packs_pu16(u1,m0));
     _mm::store2(dstV+x,_mm::packs_pu16(v1,m0));
    }
   for (;x<(int)dx1[1];x++)
    {
     int u=srcU[x]-128;
     int v=srcV[x]-128;
     int u2=((u*hueCos[cfg->hue+180])>>7)+((v*hueSin[cfg->hue+180])>>7);
     int v2=((v*hueCos[cfg->hue+180])>>7)-((u*hueSin[cfg->hue+180])>>7);
     u=((u2*cfg->saturation)>>6)+128;
     v=((v2*cfg->saturation)>>6)+128;
     dstU[x]=limit_uint8(u);
     dstV[x]=limit_uint8(v);
    }
  }
 _mm::empty();
}

HRESULT TimgFilterChroma::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TpictPropSettings *cfg=(const TpictPropSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   const unsigned char *srcU,*srcV;
   getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,NULL,&srcU,&srcV,NULL);
   unsigned char *dstU,*dstV;
   getCurNext(csp1,pict,cfg->full,COPYMODE_NO,NULL,&dstU,&dstV,NULL);
   (this->*chromaFc)(cfg,srcU,srcV,dstU,dstV);
  }
 return parent->deliverSample(++it,pict);
}
