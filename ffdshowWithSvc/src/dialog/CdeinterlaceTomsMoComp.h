#ifndef _CDEINTERLACEPAGETOMSMOCOMP_H_
#define _CDEINTERLACEPAGETOMSMOCOMP_H_

#include "CdeinterlacePanel.h"

class TdeinterlacePageTomsMoComp : public TdeinterlacePanel
{
public:
 static TdeinterlacePanel* create(Twindow *parent);
 TdeinterlacePageTomsMoComp(Twindow *Iparent);
 virtual ~TdeinterlacePageTomsMoComp();
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void reset(void);
};

#endif

