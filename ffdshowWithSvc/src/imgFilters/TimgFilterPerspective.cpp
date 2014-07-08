/*
 * Copyright (c) 2002-2006 Milan Cutka
 * uses code from mplayer perspective filter written by Michael Niedermayer
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
#include "TimgFilterPerspective.h"
#include "IffdshowDecVideo.h"

TimgFilterPerspective::TimgFilterPerspective(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 old.x1=-1;
 pv=NULL;
}
void TimgFilterPerspective::done(void)
{
 if (pv) aligned_free(pv);pv=NULL;
}
void TimgFilterPerspective::onSizeChange(void)
{
 done();
}

void TimgFilterPerspective::initPv(int W, int H,bool src,int border)
{
 pvStride= W;
 pv= (int32_t(*)[2])aligned_malloc(W*H*2*sizeof(int32_t));

 double a,b,c,d,e,f,g,h,D;
 int x,y;

 g= (  (ref[0][0] - ref[1][0] - ref[2][0] + ref[3][0])*(ref[2][1] - ref[3][1])
     - (ref[0][1] - ref[1][1] - ref[2][1] + ref[3][1])*(ref[2][0] - ref[3][0]))*H;
 h= (  (ref[0][1] - ref[1][1] - ref[2][1] + ref[3][1])*(ref[1][0] - ref[3][0])
     - (ref[0][0] - ref[1][0] - ref[2][0] + ref[3][0])*(ref[1][1] - ref[3][1]))*W;
 D=   (ref[1][0] - ref[3][0])*(ref[2][1] - ref[3][1])
    - (ref[2][0] - ref[3][0])*(ref[1][1] - ref[3][1]);

 a= D*(ref[1][0] - ref[0][0])*H + g*ref[1][0];
 b= D*(ref[2][0] - ref[0][0])*W + h*ref[2][0];
 c= D*ref[0][0]*W*H;
 d= D*(ref[1][1] - ref[0][1])*H + g*ref[1][1];
 e= D*(ref[2][1] - ref[0][1])*W + h*ref[2][1];
 f= D*ref[0][1]*W*H;
 if (!src) for (y=0;y<H;y++) for (x=0;x<W;x++) pv[x+y*W][0]=pv[x+y*W][1]=-1;

 for(y=0;y<H;y++)
  for(x=0;x<W;x++)
   {
    int u, v;
    double uu,vv;
    uu=(a*x + b*y + c)/(g*x + h*y + D*W*H);
    vv=(d*x + e*y + f)/(g*x + h*y + D*W*H);

    u= (int)floor( SUB_PIXELS*uu + 0.5);
    v= (int)floor( SUB_PIXELS*vv + 0.5);
    if (src)
     {
      pv[x + y*W][0]=isIn(u>>SUB_PIXEL_BITS,0,W-1)?u:-1;
      pv[x + y*W][1]=isIn(v>>SUB_PIXEL_BITS,0,H-1)?v:-1;
     }
    else
     if (uu>=border && uu<W-border && vv>=border && vv<H-border)
      {
       int i=(int)(limit<double>(floor(uu+0.5),0,W-1)+limit<double>(floor(vv+0.5),0,H-1)*W);
       pv[i][0]=(int)floor(SUB_PIXELS*x+0.5);
       pv[i][1]=(int)floor(SUB_PIXELS*y+0.5);
      }
   }
}
void TimgFilterPerspective::resampleNone(uint8_t *dst, stride_t dstStride, const uint8_t *src, stride_t srcStride, int w, int h, int chromaX,int chromaY,unsigned char black)
{
 int (*pv01)[2]=pv;
 int chroma2=1<<chromaX;
 int chroma3=w<<(2*chromaY);
 for(const unsigned char *dstEnd=dst+h*dstStride;dst!=dstEnd;pv01+=chroma3,dst+=dstStride-w)
  {
   int (*pv01ln)[2]=pv01;
   for(const unsigned char *dstLnEnd=dst+w;dst<dstLnEnd;dst++,pv01ln+=chroma2)
    {
     int u=(*pv01ln)[0];
     int v=(*pv01ln)[1];
     if (u<0 || v<0)
      {
       *dst=black;
       continue;
      }
     u>>=chromaX;v>>=chromaY;
     u>>=SUB_PIXEL_BITS;
     v>>=SUB_PIXEL_BITS;
     *dst=src[u+v*srcStride];
    }
  }
}
void TimgFilterPerspective::resampleLinear(uint8_t *dst, stride_t dstStride, const uint8_t *src, stride_t srcStride, int w, int h, int chromaX,int chromaY,unsigned char black)
{
 int (*pv01)[2]=pv;
 int chroma2=1<<chromaX;
 int chroma3=w<<(2*chromaY);
 for(const unsigned char *dstEnd=dst+h*dstStride;dst!=dstEnd;pv01+=chroma3,dst+=dstStride-w)
  {
   int (*pv01ln)[2]=pv01;
   for(const unsigned char *dstLnEnd=dst+w;dst<dstLnEnd;dst++,pv01ln+=chroma2)
    {
     int u=(*pv01ln)[0];
     int v=(*pv01ln)[1];
     if (u<0 || v<0)
      {
       *dst=black;
       continue;
      }
     u>>=chromaX;v>>=chromaY;
     stride_t index=(u>>SUB_PIXEL_BITS)+(v>>SUB_PIXEL_BITS)*srcStride;
     int subU=u&(SUB_PIXELS-1);
     int subV=v&(SUB_PIXELS-1);
     int subUI=SUB_PIXELS-subU;
     int subVI=SUB_PIXELS-subV;

     int sum= subVI*(subUI*src[index          ] + subU*src[index          +1])
             +subV *(subUI*src[index+srcStride] + subU*src[index+srcStride+1]);
     *dst=uint8_t((sum+(1<<(SUB_PIXEL_BITS*2-1)))>>(SUB_PIXEL_BITS*2));
    }
  }
}
void TimgFilterPerspective::resampleCubic(uint8_t *dst, stride_t dstStride, const uint8_t *src, stride_t srcStride, int w, int h, int chromaX,int chromaY,unsigned char black)
{
 int (*pv01)[2]=pv;
 int chroma2=1<<chromaX;
 int chroma3=w<<(2*chromaY);
 for(const unsigned char *dstEnd=dst+h*dstStride;dst!=dstEnd;pv01+=chroma3,dst+=dstStride-w)
  {
   int (*pv01ln)[2]=pv01;
   for(const unsigned char *dstLnEnd=dst+w;dst<dstLnEnd;dst++,pv01ln+=chroma2)
    {
     int u=(*pv01ln)[0];
     int v=(*pv01ln)[1];
     if (u<0 || v<0)
      {
       *dst=black;
       continue;
      }
     u>>=chromaX;v>>=chromaY;
     int a=SUB_PIXEL_BITS;
     *dst=cubic.interpolate(src+((u>>SUB_PIXEL_BITS)-1)+((v>>SUB_PIXEL_BITS)-1)*srcStride,srcStride,u&(SUB_PIXELS-1),v&(SUB_PIXELS-1));
    }
  }
}

HRESULT TimgFilterPerspective::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TperspectiveSettings *cfg=(const TperspectiveSettings*)cfg0;
 init(pict,cfg->full,cfg->half);
 const unsigned char *srcY,*srcU,*srcV;
 getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&srcY,&srcU,&srcV,NULL);
 unsigned char *dstY,*dstU,*dstV;
 getNext(csp1,pict,cfg->full,&dstY,&dstU,&dstV,NULL);

 if (!pv || !cfg->equal(old))
  {
   old=*cfg;
   done();
   ref[0][0]=(old.x1*dx1[0])/100;ref[0][1]=(old.y1*dy1[0])/100;
   ref[1][0]=(old.x2*dx1[0])/100;ref[1][1]=(old.y2*dy1[0])/100;
   ref[2][0]=(old.x3*dx1[0])/100;ref[2][1]=(old.y3*dy1[0])/100;
   ref[3][0]=(old.x4*dx1[0])/100;ref[3][1]=(old.y4*dy1[0])/100;
   static const int borders[]={0,1,2};
   initPv(dx1[0],dy1[0],!!old.isSrc,borders[old.interpolation]);
  }
 int interpolation= old.interpolation;
 if(pict.rectFull.dx>=768 && interpolation==2) // work around bug
  interpolation= 1;
 switch (interpolation)
  {
   case 0:resampleNone(dstY,stride2[0],srcY,stride1[0],dx1[0],dy1[0],pict.cspInfo.shiftX[0],pict.cspInfo.shiftY[0],0  );
          resampleNone(dstU,stride2[1],srcU,stride1[1],dx1[1],dy1[1],pict.cspInfo.shiftX[1],pict.cspInfo.shiftY[1],128);
          resampleNone(dstV,stride2[2],srcV,stride1[2],dx1[2],dy1[2],pict.cspInfo.shiftX[2],pict.cspInfo.shiftY[2],128);
          break;
   case 1:resampleLinear(dstY,stride2[0],srcY,stride1[0],dx1[0],dy1[0],pict.cspInfo.shiftX[0],pict.cspInfo.shiftY[0],0  );
          resampleLinear(dstU,stride2[1],srcU,stride1[1],dx1[1],dy1[1],pict.cspInfo.shiftX[1],pict.cspInfo.shiftY[1],128);
          resampleLinear(dstV,stride2[2],srcV,stride1[2],dx1[2],dy1[2],pict.cspInfo.shiftX[2],pict.cspInfo.shiftY[2],128);
          break;
   case 2:resampleCubic(dstY,stride2[0],srcY,stride1[0],dx1[0],dy1[0],pict.cspInfo.shiftX[0],pict.cspInfo.shiftY[0],0  );
          resampleCubic(dstU,stride2[1],srcU,stride1[1],dx1[1],dy1[1],pict.cspInfo.shiftX[1],pict.cspInfo.shiftY[1],128);
          resampleCubic(dstV,stride2[2],srcV,stride1[2],dx1[2],dy1[2],pict.cspInfo.shiftX[2],pict.cspInfo.shiftY[2],128);
          _mm_empty();
          break;
  }
 return parent->deliverSample(++it,pict);
}
