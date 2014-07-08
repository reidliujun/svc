#ifndef _CPROCSFSPAGE_H_
#define _CPROCSFSPAGE_H_

#include "TconfPageDecAudio.h"

class TprocsfsPage :public TconfPageDecAudio
{
private:
 static const char_t *noiseShapings[];
 void preferred2dlg(void),dither2dlg(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TprocsfsPage(TffdshowPageDec *Iparent);
 virtual bool reset(bool testonly=false);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

