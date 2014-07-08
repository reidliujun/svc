#ifndef _CCHANNELSWAPPAGE_H_
#define _CCHANNELSWAPPAGE_H_

#include "TconfPageDecAudio.h"

class TchannelSwapPage :public TconfPageDecAudio
{
private:
 struct Tspeaker
  {
   int cbx,speaker,idff;
  };
 static const Tspeaker speakers[];
 std::map<int,int> cbxs;
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TchannelSwapPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

