#ifndef _CDEINTERLACEPAGEDGBOB_H_
#define _CDEINTERLACEPAGEDGBOB_H_

#include "CdeinterlacePanel.h"

class TdeinterlacePageDGbob : public TdeinterlacePanel
{
public:
 static TdeinterlacePanel* create(Twindow *parent);
 TdeinterlacePageDGbob(Twindow *Iparent);
 virtual ~TdeinterlacePageDGbob();
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
 virtual void reset(void);
};

#endif

