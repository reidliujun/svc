#ifndef _CDEINTERLACEPANEL_H_
#define _CDEINTERLACEPANEL_H_

#include "Twindow.h"

class TdeinterlacePanel :public TdlgWindow
{
public:
 TdeinterlacePanel(int IdialogId,Twindow *parent):TdlgWindow(IdialogId,parent->deci) {}
 virtual void cfg2dlg(void) {}
 virtual void storeCfg(void) {}
 virtual void reset(void) {}
};

#endif

