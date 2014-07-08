#ifndef _TCONFPAGEDECVIDEO_H_
#define _TCONFPAGEDECVIDEO_H_

#include "TconfPageDec.h"
#include "IffdshowDecVideo.h"

class TconfPageDecVideo :public TconfPageDec
{
protected:
 comptrQ<IffdshowDecVideo> deciV;
public:
 TconfPageDecVideo(TffdshowPageDec *Iparent,const TfilterIDFF *idff=NULL,int IfilterPageId=0):TconfPageDec(Iparent,idff,IfilterPageId)
  {
   deciV=deci;
  }
};

#endif
