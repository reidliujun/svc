#ifndef _TMUXERFILE_H_
#define _TMUXERFILE_H_

#include "Tmuxer.h"

class TmuxerFile :public Tmuxer
{
private:
 FILE *f;
 char_t flnmmask[MAX_PATH];
 size_t write(const void *data,size_t len,int framenum);
public:
 TmuxerFile(IffdshowBase *Ideci);
 virtual ~TmuxerFile();
 virtual size_t writeHeader(const void *data,size_t len,bool flush,const BITMAPINFOHEADER &bihdr);
 virtual size_t writeFrame(const void *data,size_t len,const TencFrameParams &frameParams);
};

#endif
