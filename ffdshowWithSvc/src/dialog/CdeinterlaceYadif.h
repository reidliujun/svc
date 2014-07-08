#ifndef _CDEINTERLACEPAGEYADIF_H_
#define _CDEINTERLACEPAGEYADIF_H_

#include "CdeinterlacePanel.h"

class TdeinterlacePageYadif : public TdeinterlacePanel
{
public:
 static TdeinterlacePanel* create(Twindow *parent);
 TdeinterlacePageYadif(Twindow *Iparent);
 virtual ~TdeinterlacePageYadif();
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void dlg2cfg(void);
 virtual void reset(void);
 INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif

