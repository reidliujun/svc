#ifndef _CAVISYNTHPAGE_H_
#define _CAVISYNTHPAGE_H_

#include "TconfPageDecVideo.h"

class TavisynthPage :public TconfPageDecVideo
{
private:
 void onLoad(void),onSave(void),onUseCurrent(void);
 static const char_t *avs_mask;
 static const char_t *pulldownModes[];

protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
 TavisynthPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
 virtual void applySettings(void);
};

#endif

