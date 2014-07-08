/*
 * Copyright (c) 2005,2006 Milan Cutka
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
#include "Tmuxer.h"
#include "TmuxerFile.h"
#include "ffImgfmt.h"
#include "ffcodecs.h"
#include "TmuxerOGM.h"
#include "TmuxerOGG.h"
#include "IffdshowBase.h"

const char_t* Tmuxer::muxers[]=
{
 _l("Raw frames"),
 _l("OGM"),
 _l("OGG (works with Theora only)"),
 NULL
};

Tmuxer* Tmuxer::getMuxer(int id,IffdshowBase *deci)
{
 switch (id)
  {
   case MUXER_FILE:return new TmuxerFile(deci);
   case MUXER_OGM:return new TmuxerOGM(deci);
   case MUXER_OGG:return new TmuxerOGG(deci);
   default:return NULL;
  }
}

Tmuxer::Tmuxer(IffdshowBase *Ideci):deci(Ideci)
{
}

Tmuxer::~Tmuxer()
{
}
