#ifndef _CWINAMP2PAGE_H_
#define _CWINAMP2PAGE_H_

#include "TconfPageDecAudio.h"

class Twinamp2;
class Twinamp2dsp;
class Twinamp2page :public TconfPageDecAudio
{
private:
 Twinamp2 *winamp2;
 Twinamp2dsp *currentdsp;
 void dir2dlg(void),dsps2dlg(void),filter2dlg(void);
 void selectFilter(const char_t *flnm,const char_t *filtername);
 void onDir(void),onConfig(void);
 void onChbMultichannel(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 Twinamp2page(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

