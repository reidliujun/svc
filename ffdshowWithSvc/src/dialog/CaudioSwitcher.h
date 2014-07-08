#ifndef _CAUDIOSWITCHERPAGE_H_
#define _CAUDIOSWITCHERPAGE_H_

#include "TconfPageDecAudio.h"

class TaudioSwitcherPage :public TconfPageDecAudio
{
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TaudioSwitcherPage(TffdshowPageDec *Iparent);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

