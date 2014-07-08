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
#include "TimgFilterTimesmooth.h"
#include "TblurSettings.h"

#define SCALE(i) (0x0000000100010001LL * (0x10000 / (i)))
#define SCALE2(i) SCALE((i)+0),SCALE((i)+1),SCALE((i)+2),SCALE((i)+3),SCALE((i)+4),\
                  SCALE((i)+5),SCALE((i)+6),SCALE((i)+7),SCALE((i)+8),SCALE((i)+9)

const int64_t TimgFilterTimesmooth::scaletab[]=
{
 0,
 0x00007fff7fff7fffLL,      // special case for 1
 0x00007fff7fff7fffLL,      // special case for 2
 SCALE(3),
 SCALE(4),
 SCALE(5),
 SCALE(6),
 SCALE(7),
 SCALE(8),
 SCALE(9),
 SCALE2(10),
 SCALE2(20),
 SCALE2(30),
 SCALE2(40),
 SCALE2(50),
 SCALE2(60),
 SCALE2(70),
 SCALE2(80),
 SCALE2(90),
 SCALE2(100),
 SCALE2(110),
 SCALE2(120),
};

TimgFilterTimesmooth::TimgFilterTimesmooth(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 oldStrength=-1;
 framecount=0;isFirstFrame=true;
 accumY=accumU=accumV=NULL;
 tempU[0]=tempU[1]=tempU[2]=tempV[0]=tempV[1]=tempV[2]=NULL;

 framecount=0;isFirstFrame=true;
 UVcount=-3;
}
void TimgFilterTimesmooth::done(void)
{
 if (accumY) aligned_free(accumY);accumY=NULL;
 if (accumU) aligned_free(accumU);accumU=NULL;
 if (accumV) aligned_free(accumV);accumV=NULL;
 if (tempU[0]) aligned_free(tempU[0]);tempU[0]=NULL;
 if (tempU[1]) aligned_free(tempU[1]);tempU[1]=NULL;
 if (tempU[2]) aligned_free(tempU[2]);tempU[2]=NULL;
 if (tempV[0]) aligned_free(tempV[0]);tempV[0]=NULL;
 if (tempV[1]) aligned_free(tempV[1]);tempV[1]=NULL;
 if (tempV[2]) aligned_free(tempV[2]);tempV[2]=NULL;
}
void TimgFilterTimesmooth::onSizeChange(void)
{
 done();
 UVcount=-3;
}

bool TimgFilterTimesmooth::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TblurSettings *cfg=(const TblurSettings*)cfg0;
 return super::is(pict,cfg) && cfg->isTempSmooth && cfg->tempSmooth;
}

HRESULT TimgFilterTimesmooth::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TblurSettings *cfg=(const TblurSettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   const unsigned char *srcY,*srcU,*srcV;
   getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,&srcY,&srcU,&srcV,NULL);
   unsigned char *dstY=NULL,*dstU=NULL,*dstV=NULL;
   if (!isFirstFrame || !cfg->tempSmoothColor)
    getNext(csp1,pict,cfg->full,&dstY,&dstU,&dstV,NULL);
   if (cfg->tempSmooth!=oldStrength)
    {
     oldStrength=cfg->tempSmooth;
     for (int i=0;i<=510;i++)
      {
       int sqerr=((i-255)*(i-255))>>oldStrength;
       if (sqerr>16) sqerr=16;
       sqerr=16-sqerr;
       square_table[i]=sqerr;
      }
    }

   if (!accumY)
    {
     accumY=(unsigned char*)aligned_malloc(dx1[0]*dy1[0]*KERNEL);
     memset(accumY,0,dx1[0]*dy1[0]*KERNEL);
    }
   smooth(srcY,stride1[0],dstY,stride2[0],accumY,dx1[0],dy1[0]);
   if (cfg->tempSmoothColor)
    {
     if (!accumU)
      {
       accumU=(unsigned char*)aligned_malloc(dx1[1]*dy1[1]*KERNEL);
       memset(accumU,128,dx1[1]*dy1[1]*KERNEL);
      }
     smooth(srcU,stride1[1],dstU,stride2[1],accumU,dx1[1],dy1[1]);
     if (!accumV)
      {
       accumV=(unsigned char*)aligned_malloc(dx1[2]*dy1[2]*KERNEL);
       memset(accumV,128,dx1[2]*dy1[2]*KERNEL);
      }
     smooth(srcV,stride1[2],dstV,stride2[1],accumV,dx1[2],dy1[2]);
    }
   else
    {
     if (!tempU[0])
      {
       tempU[0]=(unsigned char*)aligned_malloc(dx1[1]*dy1[1]);
       tempU[1]=(unsigned char*)aligned_malloc(dx1[1]*dy1[1]);
       tempU[2]=(unsigned char*)aligned_malloc(dx1[1]*dy1[1]);
       tempV[0]=(unsigned char*)aligned_malloc(dx1[2]*dy1[2]);
       tempV[1]=(unsigned char*)aligned_malloc(dx1[2]*dy1[2]);
       tempV[2]=(unsigned char*)aligned_malloc(dx1[2]*dy1[2]);
      }
     TffPict::copy(tempU[uint32_t(UVcount)%3],dx1[1],srcU,stride1[1],dx1[1],dy1[1]);//dst=tempU[UVcount%3];for (y=0;y<dyUV;y++) memcpy(dst+dxUV*y,srcU+stride1[1]*y,dxUV);
     TffPict::copy(tempV[uint32_t(UVcount)%3],dx1[2],srcV,stride1[2],dx1[2],dy1[2]);//dst=tempV[UVcount%3];for (y=0;y<dyUV;y++) memcpy(dst+dxUV*y,srcV+stride1[2]*y,dxUV);
     if (UVcount>=0)
      {
       TffPict::copy(dstU,stride2[1],tempU[uint32_t(UVcount-2)%3],dx1[1],dx1[1],dy1[1]);
       TffPict::copy(dstV,stride2[2],tempV[uint32_t(UVcount-2)%3],dx1[1],dx1[2],dy1[2]);
      }
     else
      {
       TffPict::copy(dstU,stride2[1],srcU,stride1[1],dx1[1],dy1[1]);
       TffPict::copy(dstU,stride2[2],srcU,stride1[2],dx1[2],dy1[2]);
      }
     UVcount++;
    }
   isFirstFrame=false;
   if (++framecount>=KERNEL) framecount=0;
  }
 return parent->deliverSample(++it,pict);
}
void TimgFilterTimesmooth::smooth(const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,unsigned char *Iaccum,unsigned int dx,unsigned int dy)
{
 int offset1=framecount;
 int offset2=((framecount+KERNEL-(KERNEL/2))%KERNEL);

 if (isFirstFrame)
  {
   unsigned char *accum=Iaccum;
   /*
   int w=dx,x;
   int y=dy;
   const unsigned char *srcdst=src;
   do
    {
     x=w;
     do
      {
       accum[0]=accum[1]=accum[2]=accum[3]=accum[4]=accum[5]=accum[6]=*srcdst++;
      } while(--x);
     srcdst+=stride-dx;
    } while(--y);
   */
   //accum[0]=accum[1]=accum[2]=accum[3]=accum[4]=accum[5]=accum[6]=src[stride*(dy-1)+(dx-1)];
   int w=dx,x;
   int y=dy;
   do
    {
     x=w;
     do
      {
       accum[0] = accum[1] = accum[2] = accum[3] = *src++;
       accum += KERNEL;
      } while(--x);
     src+=srcStride-dx;
    } while(--y);
   return;
  }
 unsigned int w=dx,x;
 unsigned int y=dy;
 unsigned char *accum=Iaccum;

 do
  {
   x=w;
   do
    {
     const unsigned char center=accum[offset2];
     const int *crtab=square_table+255-center;
     accum[offset1]=*src;src++;
     int raccum=0;
     int count=0;
     for(int i=0;i<KERNEL;i++)
      {
       const int c=*accum++;
       const int sqerr=crtab[c];
       raccum+=sqerr*c;
       count+=sqerr;
      }
     int divisor=((long*)scaletab)[2*count+1];
     raccum=(raccum*divisor)>>16;
     *dst=(unsigned char)raccum;dst++;
    } while(--x);
   src+=srcStride-dx;dst+=dstStride-dx;
  } while(--y);
}

void TimgFilterTimesmooth::onSeek(void)
{
 done();
 UVcount=-3;
}
