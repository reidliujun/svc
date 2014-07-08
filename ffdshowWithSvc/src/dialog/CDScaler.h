#ifndef _CDSCALERPAGE_H_
#define _CDSCALERPAGE_H_

#include "TconfPageDecVideo.h"

struct TDScalerSettings;
struct Tdscaler_FLT;
class TDScalerPageFlt :public TconfPageDecVideo
{
private:
 long *bActive;
 TDScalerSettings *settings;
 void storeCfg(void);
 TDScalerSettings* load(const char_t *flnm);
 Tdscaler_FLT *flt;
 enum {WM_DSCALER_CHANGE=WM_APP+20};
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TDScalerPageFlt(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual ~TDScalerPageFlt();
 virtual void cfg2dlg(void);
 virtual bool reset(bool testonly);
};

#endif

