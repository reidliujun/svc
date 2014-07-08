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
#include "Tswscale.h"
#include "Tlibmplayer.h"
#include "ffImgfmt.h"
#include "Tconfig.h"
#include "postproc/swscale.h"

Tswscale::Tswscale(Tlibmplayer *Ilibmplayer):libmplayer(Ilibmplayer)
{
 swsc=NULL;
}
Tswscale::~Tswscale()
{
 done();
}
bool Tswscale::init(unsigned int Idx,unsigned int Idy,int incsp,int outcsp,const int yuv2rgbTable[6])
{
 done();
 int sw_incsp=csp_ffdshow2mplayer(incsp),sw_outcsp=csp_ffdshow2mplayer(outcsp);
 dx=Idx;dy=Idy;
 SwsParams params;Tlibmplayer::swsInitParams(&params,SWS_POINT);
 swsc=libmplayer->sws_getContext(dx,dy,sw_incsp,dx,dy,sw_outcsp,&params,NULL,NULL,yuv2rgbTable);
 return !!swsc;
}
void Tswscale::done(void)
{
 if (swsc) libmplayer->sws_freeContext(swsc);swsc=NULL;
}
bool Tswscale::convert(const uint8_t* src[], const stride_t srcStride[], uint8_t* dst[], stride_t dstStride[])
{
 return swsc && libmplayer->sws_scale_ordered(swsc,src,srcStride,0,dy,dst,dstStride)>0;
}
