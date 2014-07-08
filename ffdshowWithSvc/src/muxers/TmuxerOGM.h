#ifndef _TMUXEROGM_H_
#define _TMUXEROGM_H_

#include "TmuxerOGG.h"

class TmuxerOGM :public TmuxerOGG
{
private:
 double fps,sample_rate;
 int max_frame_size;
 int packetno;
 int next_is_key;
 char *tempbuf;
 ogg_int64_t last_granulepos,old_granulepos;
 void produce_header_packets(const BITMAPINFOHEADER &bihdr);
 int flush_pages(int header_page = 0);
 int queue_pages(int header_page = 0);
 typedef double stamp_t;
 enum
  {
   EMOREDATA  =-1,
   EMALLOC    =-2,
   EBADHEADER =-3,
   EBADEVENT  =-4,
   EOTHER     =-5
  };
 ogg_page *copy_ogg_page(ogg_page *);
 stamp_t make_timestamp(ogg_int64_t granulepos);
 void next_page_contains_keyframe(int serial);
 int process(const void *buf, size_t size, int num_frames,int key, int last_frame);
public:
 TmuxerOGM(IffdshowBase *Ideci);
 virtual ~TmuxerOGM();
 virtual size_t writeHeader(const void *data,size_t len,bool flush,const BITMAPINFOHEADER &bihdr);
 virtual size_t writeFrame(const void *data,size_t len,const TencFrameParams &frameParams);
};

#endif
