/*
 * Copyright (c) 2002-2006 Milan Cutka
 * based on idea by TheUnforgiven
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
#include "Tperfect.h"
#include "2pass.h"

Tperfect::Tperfect(void)
{
 framesCount=NULL;
 init();
}
void Tperfect::init(void)
{
 ready=false;
 avg=0;
 avgmin=avgmax=0;
 Psize=0;Pnum=0;
 if (framesCount) free(framesCount);framesCount=NULL;
 framesCountCount=0;
}

void Tperfect::analyze(const TxvidStats *stats)
{
 init();
 if (!stats->ok) return;
 unsigned int Pmax=0,Pmin=INT_MAX;
 for (size_t i=0;i<stats->size();i++)
  {
   DWORD bytes=(*stats)[i]->bytes;
   if (((*stats)[i]->quant&Txvid_2pass::NNSTATS_KEYFRAME)==0 || bytes==0)
    {
     Pnum++;
     Psize+=bytes;
     if (bytes<Pmin) Pmin=bytes;
     else if (bytes>Pmax) Pmax=bytes;
    }
  }
 if (Pnum)
  {
   avg=float(Psize)/Pnum;
   avgmax=100*Pmax/avg;
   avgmin=100*Pmin/avg;

   framesCount=(unsigned int*)calloc(framesCountCount=Pmax+1,sizeof(unsigned int));
   for (size_t i=0;i<stats->size();i++)
    if (((*stats)[i]->quant&Txvid_2pass::NNSTATS_KEYFRAME)==0 || (*stats)[i]->bytes==0)
     framesCount[(*stats)[i]->bytes]++;

   ready=true;
  }
}

unsigned int Tperfect::getFramesCount(unsigned int size)
{
 return (size>=framesCountCount)?0:framesCount[size];
}
