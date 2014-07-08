/*
 * Copyright (c) 2005,2006 Milan Cutka
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
#include "TsubtitleUSF.h"
#include "TsubreaderUSF.h"
#include "Tconvert.h"
#include "ffImgfmt.h"

TsubtitleUSF2::TsubtitleUSF2(TsubreaderUSF2 *Isubs,int Iidx,REFERENCE_TIME Istart,REFERENCE_TIME Istop):
 subs(Isubs),
 idx(Iidx),
 wrd(false),
 ok(false),
 first(true)
{
 start=Istart;
 stop=Istop;
}
void TsubtitleUSF2::print(REFERENCE_TIME time,bool wasseek,Tfont &f,bool forceChange,TrenderedSubtitleLines::TprintPrefs &prefs) const
{
 if (first || subs->dx!=prefs.dx || subs->dy!=prefs.dy)
  {
   first=false;
   wrd.dx[0]=prefs.dx;wrd.dx[1]=wrd.dx[2]=prefs.dx/2;
   wrd.dy[0]=prefs.dy;wrd.dy[1]=wrd.dy[2]=prefs.dy/2;
   wrd.dxCharY=wrd.dx[0]; wrd.dyCharY=wrd.dy[0];
   if (subs->dx!=prefs.dx || subs->dy!=prefs.dy)
    subs->create(prefs.dx,prefs.dy);
   wrd.bmpmskstride[0]=subs->stride[0];wrd.bmpmskstride[1]=subs->stride[1];wrd.bmpmskstride[2]=subs->stride[2];
  }

 int ms=int(1000*time/REF_SECOND_MULT);
 int32_t **lines;
 int dx,dy;
 bool wasChange;
 void *bmp=subs->getSub(idx,ms,&dx,&dy,&lines,&wasChange);
 if (!bmp) return;
 wrd.bmp[0]=subs->img[0];wrd.bmp[1]=subs->img[1];wrd.bmp[2]=subs->img[2];
 wrd.msk[0]=subs->alpha[0];wrd.msk[1]=subs->alpha[1];wrd.msk[2]=subs->alpha[1];
 if (wasChange)
  {
   stride_t srcStride=int((lines[1]-lines[0])*sizeof(int32_t));
   subs->convert->convert(FF_CSP_RGB32,(const uint8_t**)lines,&srcStride,FF_CSP_420P|FF_CSP_FLAGS_YUV_JPEG,wrd.bmp,wrd.bmpmskstride,NULL);
   for (int y=0;y<dy;y++)
    {
     unsigned char *msk[3]={wrd.msk[0]+y*wrd.bmpmskstride[0],wrd.msk[1]+(y/2)*wrd.bmpmskstride[1],wrd.msk[2]+(y/2)*wrd.bmpmskstride[2]};
     for (int x=0;x<dx;x++)
      {
       uint8_t m=((uint8_t*)&lines[y][x])[3];
       msk[0][x]=m;
       msk[1][x/2]=0;
       msk[2][x/2]=0;
      }
    }
  }
 int dy1[3];
 dy1[0]=wrd.dy[0];dy1[1]=wrd.dy[1];dy1[2]=wrd.dy[2];
 wrd.print(0,0/*dummy*/,wrd.dx,dy1,prefs.dst,prefs.stride,(const unsigned char**)wrd.bmp,(const unsigned char**)wrd.msk);
}
