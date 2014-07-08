#ifndef _TCONFPAGEDECAUDIO_H_
#define _TCONFPAGEDECAUDIO_H_

#include "TconfPageDec.h"
#include "IffdshowDecAudio.h"

class TffdshowPageDec;
class TconfPageDecAudio :public TconfPageDec
{
protected:
 comptrQ<IffdshowDecAudio> deciA;
public:
 TconfPageDecAudio(TffdshowPageDec *Iparent,const TfilterIDFF *idff=NULL,int IfilterPageId=0):TconfPageDec(Iparent,idff,IfilterPageId)
  {
   deciA=deci;
  }
};

#endif
