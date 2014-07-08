#ifndef _TMUXER_H_
#define _TMUXER_H_

#include "interfaces.h"

struct TencFrameParams;
class Tmuxer
{
protected:
 comptr<IffdshowBase> deci;
public:
 enum
  {
   MUXER_FILE=0,
   MUXER_OGM =1,
   MUXER_OGG =2
  };
 static const char_t *muxers[];
 static Tmuxer* getMuxer(int id,IffdshowBase *deci);
 Tmuxer(IffdshowBase *Ideci);
 virtual ~Tmuxer();
 virtual size_t writeHeader(const void *data,size_t len,bool flush,const BITMAPINFOHEADER &bihdr) {return 0;}
 virtual size_t writeFrame(const void *data,size_t len,const TencFrameParams &frameParams)=0;
};

#endif
