#ifndef _TMUXEROGG_H_
#define _TMUXEROGG_H_

#include "Tmuxer.h"
#include "xiph/ogg/ogg.h"

class TmuxerOGG :public Tmuxer
{
private:
 ogg_page og;
protected:
 HANDLE out;
 int serialno;
 ogg_stream_state os;
 unsigned int written;
 int add_ogg_page(ogg_page *opage, int header_page, int index_serial = -1);
public:
 TmuxerOGG(IffdshowBase *Ideci);
 virtual ~TmuxerOGG();
 virtual size_t writeHeader(const void *data,size_t len,bool flush,const BITMAPINFOHEADER &bihdr);
 virtual size_t writeFrame(const void *data,size_t len,const TencFrameParams &frameParams);
};

#endif
