/*
 * Copyright (c) 2003-2006 Milan Cutka
 * based on ogmtools by Moritz Bunkus
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
#include "TmuxerOGM.h"
#include "xiph/ogm/ogmstreams.h"
#include "TffPict.h"
#include "TencStats.h"
#include "ffdshow_constants.h"
#include "IffdshowBase.h"

TmuxerOGM::TmuxerOGM(IffdshowBase *Ideci):TmuxerOGG(Ideci)
{
 tempbuf=(char*)malloc(max_frame_size=1024*1024);// max frame size (should be enough)
 packetno = 0;
 next_is_key = -1;
 old_granulepos = 0;last_granulepos = 0;
}
TmuxerOGM::~TmuxerOGM()
{
 if (out!=INVALID_HANDLE_VALUE)
  {
   process(NULL,0,1,0,1);
   free(tempbuf);
  }
}
size_t TmuxerOGM::writeHeader(const void *data,size_t len,bool flush,const BITMAPINFOHEADER &bihdr)
{
 if (out==INVALID_HANDLE_VALUE) return 0;
 written=0;
 produce_header_packets(bihdr);
 return written;
}
size_t TmuxerOGM::writeFrame(const void *data,size_t len,const TencFrameParams &frameParams)
{
 if (out==INVALID_HANDLE_VALUE) return 0;
 written=0;
 process(data,len,1,(frameParams.frametype&FRAME_TYPE::I)?1:0,0);
 return written;
}
void TmuxerOGM::produce_header_packets(const BITMAPINFOHEADER &bihdr)
{
 *((unsigned char *)tempbuf) = PACKET_TYPE_HEADER;
 stream_header sh;
 memset(&sh, 0, sizeof(stream_header));
 strcpy(sh.streamtype, "video");
 memcpy(sh.subtype,&bihdr.biCompression,4);
 sh.size = sizeof(sh);
 fps=(double)deci->getParam2(IDFF_enc_fpsRate)/deci->getParam2(IDFF_enc_fpsScale);
 sh.time_unit = (ogg_int64_t)((double)REF_SECOND_MULT / fps);
 sample_rate =fps;
 sh.samples_per_unit = 1;
 sh.default_len = 1;
 sh.buffersize = max_frame_size;
 sh.bits_per_sample = 24;
 sh.sh.video.width =bihdr.biWidth ;
 sh.sh.video.height=bihdr.biHeight;
 memcpy(&tempbuf[1], &sh, sizeof(stream_header));
 ogg_packet op;
 op.packet = (unsigned char *)tempbuf;
 op.bytes = 1 + sh.size;
 op.b_o_s = 1;
 op.e_o_s = 0;
 op.packetno = 0;
 op.granulepos = 0;
 ogg_stream_packetin(&os, &op);
 packetno++;
 flush_pages(PACKET_TYPE_HEADER);
}
int TmuxerOGM::flush_pages(int header_page)
{
 ogg_page page;
 while (ogg_stream_flush(&os, &page))
  {
   add_ogg_page(&page, header_page, next_is_key);
   next_is_key = -1;
  }
 return 0;
}
int TmuxerOGM::queue_pages(int header_page)
{
 ogg_page page;
 while (ogg_stream_pageout(&os, &page))
  {
   add_ogg_page(&page, header_page, next_is_key);
   next_is_key = -1;
  }
 return 0;
}
TmuxerOGM::stamp_t TmuxerOGM::make_timestamp(ogg_int64_t granulepos)
{
 stamp_t stamp;

#ifndef INCORRECT_INTERLEAVING
 stamp = (stamp_t)((double)old_granulepos * (double)1000000.0 / sample_rate);
 old_granulepos = granulepos;
#else
 stamp = (stamp_t)((double)granulepos * (double)1000000.0 / sample_raten);
#endif

 return stamp;
}
ogg_page* TmuxerOGM::copy_ogg_page(ogg_page *src)
{
 ogg_page *dst;

 if (src == NULL) return NULL;

 dst = (ogg_page *)malloc(sizeof(ogg_page));
 if (src->header_len == 0)
  {
   deci->dbgWrite(_l("FATAL: copy_ogg_page: src->header_len == 0"));
   return NULL;
  }
 dst->header = (unsigned char *)malloc(src->header_len);
 dst->header_len = src->header_len;
 memcpy(dst->header, src->header, src->header_len);
 if (src->body_len != 0)
  {
   dst->body = (unsigned char *)malloc(src->body_len);
   dst->body_len = src->body_len;
   memcpy(dst->body, src->body, src->body_len);
  }
 else
  {
   dst->body = (unsigned char *)malloc(1);
   dst->body_len = 0;
   *(dst->body) = 0;
  }
 return dst;
}
void TmuxerOGM::next_page_contains_keyframe(int serial)
{
 next_is_key = serial;
}
int TmuxerOGM::process(const void *buf, size_t size, int num_frames,int key, int last_frame)
{
 ogg_packet op;
 int offset;

 if (!last_frame)
  {
   if (key)
    {
     flush_pages();
     next_page_contains_keyframe(serialno);
    }
   *((unsigned char *)tempbuf) = (unsigned char)(key ? PACKET_IS_SYNCPOINT : 0);
   if (num_frames == 1)
     offset = 0;
   else
    {
     unsigned char *bptr;
     int nf, idx;

     for (offset = 3; offset >= 0; offset--)
      if (num_frames > (1 << (8 * offset)))
       break;
     offset++;
     tempbuf[0] |= (((offset & 3) << 6) + ((offset & 4) >> 1));
     bptr = (unsigned char *)&tempbuf[1];
     nf = num_frames;
     for (idx = 0; idx < offset; idx++)
      {
       *(bptr + idx) = (unsigned char)(nf & 0xFF);
       nf >>= 8;
      }
    }
   memcpy(&tempbuf[1 + offset], buf, size);
   op.bytes = long(size + 1 + offset);
   op.packet = (unsigned char *)tempbuf;
   op.b_o_s = 0;
   op.e_o_s = last_frame;
   op.granulepos = last_granulepos + num_frames - 1;
   op.packetno = packetno;
   ogg_stream_packetin(&os, &op);
   queue_pages();
   last_granulepos += num_frames;
  }
 else
  {
   *((unsigned char *)tempbuf) = 0;
   op.bytes = 1;
   op.packet = (unsigned char *)tempbuf;
   op.b_o_s = 0;
   op.e_o_s = 1;
   op.granulepos = last_granulepos;
   op.packetno = packetno;
   ogg_stream_packetin(&os, &op);
   flush_pages();
   last_granulepos++;
  }
 packetno++;
 return EMOREDATA;
}
