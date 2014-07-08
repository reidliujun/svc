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
#include "TimgFilterShowMV.h"
#include "TvideoCodec.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "TvisSettings.h"
#include "TimgFilters.h"

#pragma warning(disable:4799)

#define X 130
__align8(const unsigned char,TimgFilterShowMV::numsL[])=
{
 //0
 0,0,0,0,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,0,0,0,0,0,0,0,
 //1
 0,0,0,0,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,X,X,0,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,0,0,0,0,0,0,0,
 //2
 0,0,0,0,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,0,0,X,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,X,0,0,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,0,0,0,0,0,0,0,
 //3
 0,0,0,0,0,0,0,0,
 0,X,X,0,0,0,0,0,
 0,0,0,X,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,0,0,X,0,0,0,0,
 0,0,0,X,0,0,0,0,
 0,X,X,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 //4
 0,0,0,0,0,0,0,0,
 0,0,0,X,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,X,0,0,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,0,0,X,0,0,0,0,
 0,0,0,0,0,0,0,0,
 //5
 0,0,0,0,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,X,0,0,0,0,0,0,
 0,X,X,0,0,0,0,0,
 0,0,0,X,0,0,0,0,
 0,0,0,X,0,0,0,0,
 0,X,X,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 //6
 0,0,0,0,0,0,0,0,
 0,0,X,X,0,0,0,0,
 0,X,0,0,0,0,0,0,
 0,X,X,0,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 //7
 0,0,0,0,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,0,0,X,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,X,0,0,0,0,0,0,
 0,X,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 //8
 0,0,0,0,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
 //9
 0,0,0,0,0,0,0,0,
 0,0,X,0,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,X,0,X,0,0,0,0,
 0,0,X,X,0,0,0,0,
 0,0,0,X,0,0,0,0,
 0,X,X,0,0,0,0,0,
 0,0,0,0,0,0,0,0,
};
__align8(const unsigned char,TimgFilterShowMV::numsR[])=
{
 //0
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,0,0,0,
 //1
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,X,X,0,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,0,0,0,
 //2
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,0,0,X,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,X,0,0,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,0,0,0,
 //3
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,X,X,0,
 0,0,0,0,0,0,0,X,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,0,0,X,
 0,0,0,0,0,0,0,X,
 0,0,0,0,0,X,X,0,
 0,0,0,0,0,0,0,0,
 //4
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,X,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,X,0,0,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,0,0,X,
 0,0,0,0,0,0,0,0,
 //5
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,X,0,0,
 0,0,0,0,0,X,X,0,
 0,0,0,0,0,0,0,X,
 0,0,0,0,0,0,0,X,
 0,0,0,0,0,X,X,0,
 0,0,0,0,0,0,0,0,
 //6
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,X,X,
 0,0,0,0,0,X,0,0,
 0,0,0,0,0,X,X,0,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,0,0,0,
 //7
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,0,0,X,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,X,0,0,
 0,0,0,0,0,X,0,0,
 0,0,0,0,0,0,0,0,
 //8
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,0,0,0,
 //9
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,X,0,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,X,0,X,
 0,0,0,0,0,0,X,X,
 0,0,0,0,0,0,0,X,
 0,0,0,0,0,X,X,0,
 0,0,0,0,0,0,0,0,
};
#undef X

#define X 0
__align8(const unsigned char,TimgFilterShowMV::maskL[])=
{
 0,0,0,0,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,X,X,X,0,0,0,0,
 0,0,0,0,0,0,0,0,
};
__align8(const unsigned char,TimgFilterShowMV::maskR[])=
{
 0,0,0,0,0,0,0,0,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,X,X,X,
 0,0,0,0,0,0,0,0,
};
#undef X

TimgFilterShowMV::TimgFilterShowMV(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
}

void TimgFilterShowMV::drawNum(const unsigned char *src,const unsigned char *mask,unsigned char *dst,stride_t stride)
{
 const __m64 *src8=(const __m64*)src;
#if 0
 for (int i=0;i<8;i++,dst+=stride,src8++)
  *(__m64*)dst=_mm_add_pi8(*(__m64*)dst,*src8);
#else
 *(__m64*)dst=_mm_add_pi8(*(__m64*)dst,src8[0]);dst+=stride;
 *(__m64*)dst=_mm_add_pi8(*(__m64*)dst,src8[1]);dst+=stride;
 *(__m64*)dst=_mm_add_pi8(*(__m64*)dst,src8[2]);dst+=stride;
 *(__m64*)dst=_mm_add_pi8(*(__m64*)dst,src8[3]);dst+=stride;
 *(__m64*)dst=_mm_add_pi8(*(__m64*)dst,src8[4]);dst+=stride;
 *(__m64*)dst=_mm_add_pi8(*(__m64*)dst,src8[5]);dst+=stride;
 *(__m64*)dst=_mm_add_pi8(*(__m64*)dst,src8[6]);dst+=stride;
 *(__m64*)dst=_mm_add_pi8(*(__m64*)dst,src8[7]);dst+=stride;
#endif
}

bool TimgFilterShowMV::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TvisSettings *cfg=(const TvisSettings*)cfg0;
 return super::is(pict,cfg) && (cfg->mv || cfg->quants || cfg->graph);
}

HRESULT TimgFilterShowMV::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TvisSettings *cfg=(const TvisSettings*)cfg0;
   const TvideoCodecDec *movie;
   deciV->getMovieSource(&movie);
   init(pict,0,0);
   unsigned char *dstY=NULL;
   if (movie && (movie->caps()&(TvideoCodec::CAPS::VIS_MV|TvideoCodec::CAPS::VIS_QUANTS)))
    {
     if (cfg->mv)
      {
       getCurNext(FF_CSPS_MASK_YUV_PLANAR,pict,0,COPYMODE_DEF,&dstY,NULL,NULL,NULL);
       movie->drawMV(dstY,dx1[0],stride2[0],dy1[0]);
      }
     if (cfg->quants && movie->quants)
      {
       if (!dstY) getCurNext(FF_CSPS_MASK_YUV_PLANAR,pict,0,COPYMODE_DEF,&dstY,NULL,NULL,NULL);
       const unsigned char *quants=(const unsigned char*)movie->quants;
       for (unsigned int y=4;y<=dy1[0]-12;y+=16,quants+=movie->quantsStride*movie->quantBytes)
        {
         const unsigned char *q=quants;
         for (unsigned int x=0;x<=dx1[0]-12;x+=16,q+=movie->quantBytes)
          {
           unsigned int qq=*q;if (qq>99) qq=99;
           if (qq/10)
            {
             drawNum(numsL+(qq/10)*64,maskL,dstY+(x+4)+y*stride2[0],stride2[0]);
             drawNum(numsR+(qq%10)*64,maskR,dstY+(x+4)+y*stride2[0],stride2[0]);
            }
           else
            drawNum(numsL+qq*64,maskL,dstY+(x+6)+y*stride2[0],stride2[0]);
          }
        }
       _mm_empty();
      }
    }
   if (cfg->graph)
    {
     if (!dstY) getCurNext(FF_CSPS_MASK_YUV_PLANAR,pict,0,COPYMODE_DEF,&dstY,NULL,NULL,NULL);
     if (frames.empty()) frames.resize(2000);
     frames.erase(frames.begin());frames.push_back(Tframe(pict.srcSize,pict.frametype));
     unsigned char *dst=dstY+(dy1[0]-1)*stride2[0];
     size_t max=0;
     for (unsigned int i=2000-dx1[0];i<2000;i++)
      if (frames[i].first>max)
       max=frames[i].first;
     if (max)
      for (unsigned int i=2000-dx1[0];i<2000;i++,dst++)
       {
        Tframe &f=frames[i];
        unsigned char c;
        switch (f.second&FRAME_TYPE::typemask)
         {
          case FRAME_TYPE::I:c=255;break;
          case FRAME_TYPE::P:c=210;break;
          case FRAME_TYPE::B:c=170;break;
          default:c=130;break;
         }
        for (unsigned char *dst1=dst,*dst1end=dst1-std::min(int((dy1[0]/4)*f.first/max),int(dy1[0])-1)*stride2[0];dst1!=dst1end;dst1-=stride2[0])
         *dst1=c;
       }
    }
  }
 return parent->deliverSample(++it,pict);
}

void TimgFilterShowMV::onSeek(void)
{
 frames.clear();
}
