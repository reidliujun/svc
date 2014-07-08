#ifndef _TPRESETSENCDLG_H_
#define _TPRESETSENCDLG_H_

#include "Twindow.h"

struct TpresetsEnc;
struct TpresetsEncDlg :public TdlgWindow
{
private:
 TpresetsEnc &presets;
 HWND hlv;
 void init(void);
 void fillPresets(void);
 void selectPreset(int i);
 void onRemove(void),onRename(void);
protected:
 virtual INT_PTR msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
public:
 TpresetsEncDlg(HWND parent,IffdshowBase *Ideci,TpresetsEnc &Ipresets);
};

#endif
