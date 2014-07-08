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
#include "TmuxerOGG.h"
#include "IffdshowBase.h"
#include "ffdshow_constants.h"
#include "TencStats.h"

TmuxerOGG::TmuxerOGG(IffdshowBase *Ideci):Tmuxer(Ideci)
{
 out=CreateFile(deci->getParamStr2(IDFF_enc_storeExtFlnm),GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);if (out==INVALID_HANDLE_VALUE) return;
 serialno=rand();
 ogg_stream_init(&os, serialno);
}
TmuxerOGG::~TmuxerOGG()
{
 if (out!=INVALID_HANDLE_VALUE)
  {
   CloseHandle(out);
   ogg_stream_clear(&os);
  }
}
int TmuxerOGG::add_ogg_page(ogg_page *opage, int header_page, int index_serial)
{
 DWORD bytes;

 WriteFile(out,opage->header, opage->header_len,&bytes, NULL);
 if (bytes != DWORD(opage->header_len)) return 1;
 written+=bytes;

 WriteFile(out,opage->body, opage->body_len, &bytes,NULL);
 if (bytes != DWORD(opage->body_len)) return 1;
 written+=bytes;

 return 0;
}
size_t TmuxerOGG::writeHeader(const void *data,size_t len,bool flush,const BITMAPINFOHEADER &bihdr)
{
 if (out==INVALID_HANDLE_VALUE || len!=0) return 0;
 written=0;
 if (data)
  {
   ogg_packet *op=(ogg_packet*)data;
   ogg_stream_packetin(&os,op);
   if (flush)
    {
     while (1)
      {
       int result=ogg_stream_flush(&os,&og);
       if (result==0) break;
       add_ogg_page(&og,0,0);
      }
    }
  }
 return written;
}
size_t TmuxerOGG::writeFrame(const void *data,size_t len,const TencFrameParams &frameParams)
{
 if (out==INVALID_HANDLE_VALUE || !data || !frameParams.priv) return 0;
 written=0;
 ogg_packet *op=(ogg_packet*)frameParams.priv;
 ogg_stream_packetin(&os,op);
 while (1)
  {
   int result=ogg_stream_flush(&os,&og);
   if (result==0) break;
   add_ogg_page(&og,0,0);
  }
 return written;
}
