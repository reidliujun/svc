#ifndef _CDEINTERLACEPAGEDSCALER_H_
#define _CDEINTERLACEPAGEDSCALER_H_

#include "CdeinterlacePanel.h"

struct Tdscaler_DI;
class TdeinterlacePageDScaler :public TdeinterlacePanel
{
private:
 Tdscaler_DI *di;
public:
 enum {WM_DSCALERPAGE_CHANGE=WM_APP+21};
 TdeinterlacePageDScaler(Twindow *Iparent,const char_t *diflnm);
 virtual ~TdeinterlacePageDScaler();
 virtual void storeCfg(void);
 virtual void reset(void);
};

#endif

