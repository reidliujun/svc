#ifndef _TVERSIONS_H_
#define _TVERSIONS_H_

#include "Twindow.h"
#include "interfaces.h"

class TversionsDlg :public TdlgWindow
{
public:
 typedef bool (*TgetVersionFc)(const Tconfig *config,ffstring &vers,ffstring &license);
 struct TversionInfo
  {
   const char_t *name;
   TgetVersionFc fc;
  };
private:
 HWND hlv;
 const TversionInfo *fcs;
protected:
 virtual INT_PTR msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
public:
 TversionsDlg(IffdshowBase *Ideci,HWND parent,const TversionInfo *fcs);
};

#endif
