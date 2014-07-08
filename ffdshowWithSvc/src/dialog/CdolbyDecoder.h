#ifndef _CDOLBYDECODERPAGE_H_
#define _CDOLBYDECODERPAGE_H_

#include "TconfPageDecAudio.h"

class TdolbyDecoderPage :public TconfPageDecAudio
{
public:
 TdolbyDecoderPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void cfg2dlg(void);
};

#endif

