#ifndef _CDEINTERLACEPAGEKERNEL_H_
#define _CDEINTERLACEPAGEKERNEL_H_

#include "CdeinterlacePanel.h"

class TdeinterlacePageKernel : public TdeinterlacePanel
{
public:
 static TdeinterlacePanel* create(Twindow *parent);
 TdeinterlacePageKernel(Twindow *Iparent);
 virtual ~TdeinterlacePageKernel();
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void reset(void);
};

#endif

