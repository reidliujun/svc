#ifndef _CRESAMPLEPAGE_H_
#define _CRESAMPLEPAGE_H_

#include "TconfPageDecAudio.h"

class TresamplePage :public TconfPageDecAudio
{
private:
 HWND hedResample;
 void if2dlg(void);
 void onResampleFreqMenu(void),onResampleCondMenu(void),onResampleMenu(int idc,int idff);
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual Twidget* createDlgItem(int id,HWND h);
public:
 TresamplePage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void applySettings(void);
 virtual void translate(void);
};

#endif

