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
#include "TsubtitlesTextpin.h"
#include "TsubtitlesTextpinText.h"
#include "TsubtitlesTextpinVobSub.h"
#include "TsubtitlesTextpinDVD.h"
#include "TsubtitlesTextpinUSF.h"
#include "Tsubreader.h"

TsubtitlesTextpin::TsubtitlesTextpin(int Itype,IffdshowBase *Ideci):Tsubtitles(Ideci),type(Itype)
{
 init();
}

void TsubtitlesTextpin::resetSubtitles(void)
{
 init();
}

TsubtitlesTextpin* TsubtitlesTextpin::create(int type,const unsigned char *extradata,unsigned int extradatalen,IffdshowBase *Ideci)
{
 switch (type)
  {
   case Tsubreader::SUB_VOBSUB:
    return new TsubtitlesTextpinVobsub(type,Ideci,extradata,extradatalen);
   case Tsubreader::SUB_DVD:
   case Tsubreader::SUB_CVD:
   case Tsubreader::SUB_SVCD:
    return new TsubtitlesTextpinDVD(type,Ideci);
   case Tsubreader::SUB_USF:
    return new TsubtitlesTextpinUSF(type,Ideci,extradata,extradatalen);
   default:
    return new TsubtitlesTextpinText(type,Ideci,extradata,extradatalen);
  }
}
