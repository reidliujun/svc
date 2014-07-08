/*
 * Copyright (c) 2002-2006 Milan Cutka
 * Levels table calculation from Avisynth v1.0 beta. Copyright 2000 Ben Rudiak-Gould
 * Ylevels by Didée
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
#include "TimgFilterLevels.h"
#include "IffdshowBase.h"
#include "ffdebug.h"

TimgFilterLevels::TimgFilterLevels(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 oldSettings.inMin=-1;
 oldMode=0;
 resetHistory();
}

bool TimgFilterLevels::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TlevelsSettings *cfg=(const TlevelsSettings*)cfg0;
 return super::is(pict,cfg) && (cfg->mode==5 || cfg->inAuto || cfg->inMin!=0 || cfg->inMax!=255 || cfg->outMin!=0 || cfg->outMax!=255 || cfg->gamma!=100);
}

HRESULT TimgFilterLevels::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TlevelsSettings *cfg=(const TlevelsSettings*)cfg0;
 const unsigned char *srcY=NULL;

 if (flag_resetHistory || cfg->mode != oldMode)
  {
   flag_resetHistory=false;
   oldMode = cfg->mode;
    if (cfg->inAuto)
     {
      if (cfg->mode != 6)
       {
        inMin=0;memsetd(inMins,inMin,sizeof(inMins));inMinSum=inMin;
        inMax=255;memsetd(inMaxs,inMax,sizeof(inMaxs));inMaxSum=inMax*HISTORY;
        minMaxPos=0;
       }
      else
       {
        inMin=cfg->inMin;
        inMax=cfg->inMax;
        inMinSum=0;
        inMaxSum=0;
       }
     }
    else
     {
      inMin=0;memsetd(inMins,inMin,sizeof(inMins));inMinSum=inMin;
      inMax=255;memsetd(inMaxs,inMax,sizeof(inMaxs));inMaxSum=inMax*HISTORY;
      minMaxPos=0;
     }
  }

 if (cfg->inAuto || (deci->getParam2(IDFF_buildHistogram) && deci->getCfgDlgHwnd()))
  {
   if (cfg->inAuto)
    {
     init(pict,cfg->full,cfg->half);
     getCur(supportedcsps,pict,cfg->full,&srcY,NULL,NULL,NULL);
    }
   CAutoLock lock(&csHistogram);
   pict.histogram(histogram,cfg->full,cfg->half);
  }
 if (cfg->inAuto)
  {
   if(cfg->mode!=6)
   {
    int newInMin,newInMax;

    unsigned int l=pictRect.dx*pictRect.dy/7000;
    for (newInMin=0;newInMin<250 && histogram[newInMin]<l;newInMin++)
     ;
    for (newInMax=255;newInMax>newInMin+1 && histogram[newInMax]<l;newInMax--)
     ;
    int diff=newInMax-newInMin;
    if (diff<40)
     {
      newInMin=limit(newInMin-20,0,254);
      newInMax=limit(newInMax+20,1,255);
     }
    inMinSum-=inMins[minMaxPos];inMins[minMaxPos]=newInMin;inMinSum+=newInMin;
    inMaxSum-=inMaxs[minMaxPos];inMaxs[minMaxPos]=newInMax;inMaxSum+=newInMax;
    minMaxPos++;if (minMaxPos==HISTORY) minMaxPos=0;
    inMin=inMinSum/HISTORY;inMax=inMaxSum/HISTORY;
   }
  else
   {
    // mode==6
    unsigned int threshold=pictRect.dx*pictRect.dy*(cfg->Ythreshold)/1000;

    unsigned int countMin, countMax;
    int x;

    for(countMin=0,x=0;x<inMin;countMin+=histogram[x],x++)
     ;
    for(countMax=0,x=255;x>inMax;countMax+=histogram[x],x--)
     ;

    if( countMin>threshold )
     {
      if(inMinSum>cfg->Ytemporal)
       {
        if(inMin>cfg->inMin-cfg->YmaxDelta)
         inMin--;
        inMinSum=0;
       }
      else
       inMinSum++;
     }
    else
     inMinSum=0;

    if( countMax>threshold )
     {
      if(inMaxSum>cfg->Ytemporal)
       {
        if(inMax<cfg->inMax+cfg->YmaxDelta)
         inMax++;
        inMaxSum=0;
       }
      else
       inMaxSum++;
     }
    else
     inMaxSum=0;
   }

   //DPRINTF("min:%i max:%i\n",inMin,inMax);
  }
 else
  {
   inMin=cfg->inMin;
   inMax=cfg->inMax;
  }
 if (cfg->is && (cfg->mode==5 || cfg->inAuto || inMin!=0 || inMax!=255 || cfg->outMin!=0 || cfg->outMax!=255 || cfg->gamma!=100 || cfg->posterize!=255))
  {
   bool equal=cfg->equal(oldSettings);
   if (cfg->inAuto || !equal)
    {
     oldSettings=*cfg;
     int divisor;
     oldSettings.calcMap(map,&divisor,inMin,inMax);
     if (!equal)
      {
	   if(cfg->mode!=6)
	   {
		   if (cfg->inAuto)
			divisor=cfg->inMax-cfg->inMin+(cfg->inMax==cfg->inMin);
		   for (int x=0;x<256;x++)
			mapchroma[x]=limit(((x-128)*(oldSettings.outMax-oldSettings.outMin)+(divisor>>1))/divisor+128,16,240);
	   }
	   else
	   { // Seb's BTB&WTW : always map chroma 
		   for (int x=0;x<=16;x++) // [0~16]=1
			mapchroma[x]=1;
		   mapchroma[128]=128; // [128]=128
		   for (int x=1;x<=112;x++) // [16~240]->[1~255]
		   {
		    divisor = x*127/112;
			mapchroma[128+x]=128+divisor;
			mapchroma[128-x]=128-divisor;
		   }
		   for (int x=240;x<=255;x++) // [240~255]=255
			mapchroma[x]=255;
	   }
      }
    }
   if (!srcY)
    {
     init(pict,cfg->full,cfg->half);
     getCur(supportedcsps,pict,cfg->full,&srcY,NULL,NULL,NULL);
    }
   unsigned char *dstY;
   getCurNext(csp1,pict,cfg->full,COPYMODE_NO,&dstY,NULL,NULL,NULL);
   if (csp_isYUVplanar(csp1))
    {
     for (unsigned int y=0;y<dy1[0];y++)
      {
       const unsigned char *src;unsigned char *dst,*dstEnd;
       for (src=srcY+stride1[0]*y,dst=dstY+stride2[0]*y,dstEnd=dst+dx1[0];dst<dstEnd-3;src+=4,dst+=4)
        *(unsigned int*)dst=(map[src[3]]<<24)|(map[src[2]]<<16)|(map[src[1]]<<8)|map[src[0]];
       for (;dst!=dstEnd;src++,dst++)
        *dst=uint8_t(map[*src]);
      }
     if (!oldSettings.onlyLuma)
      {
       const unsigned char *srcU,*srcV;
       getCur(csp1,pict,cfg->full,NULL,&srcU,&srcV,NULL);
       unsigned char *dstU,*dstV;
       getCurNext(csp1,pict,cfg->full,COPYMODE_NO,NULL,&dstU,&dstV,NULL);
       for (unsigned int y=0;y<dy1[1];y++)
        {
         const unsigned char *srcu,*srcv;unsigned char *dstu,*dstv,*dstuEnd;
         for (srcu=srcU+stride1[1]*y,srcv=srcV+stride1[2]*y,dstu=dstU+stride2[1]*y,dstv=dstV+stride2[2]*y,dstuEnd=dstu+dx1[1];dstu<dstuEnd-3;srcu+=4,srcv+=4,dstu+=4,dstv+=4)
          {
           *(unsigned int*)dstu=(mapchroma[srcu[3]]<<24)|(mapchroma[srcu[2]]<<16)|(mapchroma[srcu[1]]<<8)|mapchroma[srcu[0]];
           *(unsigned int*)dstv=(mapchroma[srcv[3]]<<24)|(mapchroma[srcv[2]]<<16)|(mapchroma[srcv[1]]<<8)|mapchroma[srcv[0]];
          }
         for (;dstu!=dstuEnd;srcu++,srcv++,dstu++,dstv++)
          {
           *dstu=uint8_t(mapchroma[*srcu]);
           *dstv=uint8_t(mapchroma[*srcv]);
          }
        }
      }
    }
   else if (csp_isYUVpacked(csp1))
    {
     int lumaoffset=pict.cspInfo.packedLumaOffset,chromaoffset=pict.cspInfo.packedChromaOffset;
     if      (lumaoffset==0 && chromaoffset==1 && oldSettings.onlyLuma==0) processPacked<0,1,0>(srcY,dstY);
     else if (lumaoffset==0 && chromaoffset==1 && oldSettings.onlyLuma==1) processPacked<0,1,1>(srcY,dstY);
     else if (lumaoffset==1 && chromaoffset==0 && oldSettings.onlyLuma==0) processPacked<1,0,0>(srcY,dstY);
     else if (lumaoffset==1 && chromaoffset==0 && oldSettings.onlyLuma==1) processPacked<1,0,1>(srcY,dstY);
    }
  }
 return parent->deliverSample(++it,pict);
}

void TimgFilterLevels::resetHistory(void)
{
 flag_resetHistory=true;
}
void TimgFilterLevels::onSeek(void)
{
 resetHistory();
}

STDMETHODIMP TimgFilterLevels::getHistogram(unsigned int dst[256])
{
 CAutoLock lock(&csHistogram);
 memcpy(dst,histogram,sizeof(histogram));
 return S_OK;
}
STDMETHODIMP TimgFilterLevels::getInAuto(int *min,int *max)
{
 *min=inMin;
 *max=inMax;
 return S_OK;
}

HRESULT TimgFilterLevels::queryInterface(const IID &iid,void **ptr) const
{
 if (iid==IID_IimgFilterLevels) {*ptr=(IimgFilterLevels*)this;return S_OK;}
 else return E_NOINTERFACE;
}
