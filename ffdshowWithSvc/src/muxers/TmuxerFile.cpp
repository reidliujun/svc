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
#include "TmuxerFile.h"
#include "ffdshow_constants.h"
#include "IffdshowBase.h"
#include "TencStats.h"

TmuxerFile::TmuxerFile(IffdshowBase *Ideci):Tmuxer(Ideci)
{
 const char_t *storeExtFlnm=deci->getParamStr2(IDFF_enc_storeExtFlnm);
 if (strchr(storeExtFlnm,'%'))
  {
   ff_strncpy(flnmmask, storeExtFlnm, countof(flnmmask));
   f=NULL;
  }
 else
  {
   f=fopen(storeExtFlnm,_l("wb"));
   flnmmask[0]='\0';
  }
}
TmuxerFile::~TmuxerFile()
{
 if (f) fclose(f);
}

size_t TmuxerFile::write(const void *data,size_t len,int framenum)
{
 if (flnmmask[0])
  {
   char_t flnm[MAX_PATH];
   tsnprintf_s(flnm, countof(flnm), _TRUNCATE, flnmmask, framenum);
   FILE *f=fopen(flnm,_l("wb"));if (!f) return 0;
   len=fwrite(data,1,len,f);
   fclose(f);
   return len;
  }
 else
  return f?fwrite(data,1,len,f):0;
}
size_t TmuxerFile::writeHeader(const void *data,size_t len,bool flush,const BITMAPINFOHEADER &bihdr)
{
 return (data && len)?write(data,len,-1):0;
}
size_t TmuxerFile::writeFrame(const void *data,size_t len,const TencFrameParams &frameParams)
{
 return write(data,len,frameParams.framenum);
}
