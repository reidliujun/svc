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
#include "TsubtitlesTextpinVobSub.h"
#include "TsubreaderVobsub.h"
#include "TsubtitleVobsub.h"
#include "spudec.h"
#include "Tstream.h"

#define DVDSUB

TsubtitlesTextpinVobsub::TsubtitlesTextpinVobsub(int Itype,IffdshowBase *Ideci,const unsigned char *data,unsigned int datalen):TsubtitlesTextpin(Itype,Ideci)
{
 sub_format=Tsubreader::SUB_VOBSUB;
 TstreamMem fs(data,datalen);
#ifndef DVDSUB
 spu=NULL;
 subs=new TsubreaderVobsub(fs,NULL,deci,&spu);
 subs->push_back(new TsubtitleVobsub(spu,NULL));
#else
 subs=new Tsubreader;
 idx_parse(fs);
#endif
}

void TsubtitlesTextpinVobsub::resetSubtitles(void)
{
#ifndef DVDSUB
 if (spu) spu->spudec_reset();
#endif
 TsubtitlesTextpin::resetSubtitles();
}

void TsubtitlesTextpinVobsub::addSubtitle(REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,const TsubtitlesSettings *cfg,bool utf8)
{
#ifndef DVDSUB
 int timer=(int)(90*start/10000LL);
 spu->spudec_assemble(data,datalen,timer);
#else
 Tsubtitle *sub=new TsubtitleDVD(start,data,datalen,this);
 subs->push_back(sub);
#endif
}
