/*
 * Copyright (c) 2002-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "TencStats.h"

void TencStats::init(unsigned int Idx,unsigned int Idy,int csp)
{
 dx=Idx;dy=Idy;
 const TcspInfo *cspInfo=csp_getInfo(csp);
 for (int i=0;i<3;i++)
  mult[i]=(1<<cspInfo->shiftX[i])*(1<<cspInfo->shiftY[i]);
 sumQuants=0;count=0;
 sumFramesize=0;
 sumPsnrY=sumPsnrU=sumPsnrV=0;
 memset(quantCount,0,sizeof(quantCount));
}
void TencStats::add(const TencFrameParams &frame)
{
 count++;
 sumFramesize+=frame.length;
 sumPsnrY+=frame.psnrY;sumPsnrU+=frame.psnrU;sumPsnrV+=frame.psnrV;
 if (frame.quant<=0) return;
 sumQuants+=frame.quant;
 if (isIn(frame.quant,1,51))
  quantCount[frame.quant]++;
}
void TencStats::calcPSNR(double *psnrY,double *psnrU,double *psnrV) const
{
 double f=dx*dy*255.0*255.0*count;
 if (f!=0)
  {
   *psnrY=psnr(sumPsnrY*mult[0]/f);
   *psnrU=psnr(sumPsnrU*mult[1]/f);
   *psnrV=psnr(sumPsnrV*mult[2]/f);
  }
 else
  *psnrY=*psnrU=*psnrV=0;
}
