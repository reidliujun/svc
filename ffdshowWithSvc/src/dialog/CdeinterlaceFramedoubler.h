#ifndef _CDEINTERLACEPAGEFRAMEDOUBLER_H_
#define _CDEINTERLACEPAGEFRAMEDOUBLER_H_

#include "CdeinterlacePanel.h"

class TdeinterlacePageFramedoubler : public TdeinterlacePanel
{
public:
 static TdeinterlacePanel* create(Twindow *parent);
 TdeinterlacePageFramedoubler(Twindow *Iparent);
 virtual ~TdeinterlacePageFramedoubler();
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void reset(void);
 virtual void translate(void);
};

#endif

