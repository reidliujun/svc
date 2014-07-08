#ifndef _CDECODERSETTINGSAGE_H_
#define _CDECODERSETTINGSPAGE_H_

#include "TconfPageDecAudio.h"

class TaudioDecoderOptionsPage :public TconfPageDecAudio
{
private:
 void drc2dlg(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TaudioDecoderOptionsPage(TffdshowPageDec *Iparent);
 virtual void init(void);
 virtual bool reset(bool testonly=false);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif
