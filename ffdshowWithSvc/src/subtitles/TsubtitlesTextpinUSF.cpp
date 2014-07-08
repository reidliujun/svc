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
#include "TsubtitlesTextpinUSF.h"
#include "TsubreaderUSF.h"

TsubtitlesTextpinUSF::TsubtitlesTextpinUSF(int Itype,IffdshowBase *Ideci,const unsigned char *extradata,unsigned int extradatalen):TsubtitlesTextpin(Itype,Ideci)
{
 subs=new TsubreaderUSF2(TstreamMem(extradata,extradatalen,Tstream::ENC_UTF8),deci,true);
}

TsubtitlesTextpinUSF::~TsubtitlesTextpinUSF()
{
}

void TsubtitlesTextpinUSF::addSubtitle(REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,const TsubtitlesSettings *cfg, bool utf8)
{
 ((TsubreaderUSF2*)subs)->addSub(start,stop,data,datalen);
 init();
}

void TsubtitlesTextpinUSF::resetSubtitles(void)
{
 subs->clear();
}
