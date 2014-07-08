#ifndef _CPOSTPROCPAGE_H_
#define _CPOSTPROCPAGE_H_

#include "TconfPageDecVideo.h"

class TpostProcPage :public TconfPageDecVideo
{
private:
 void postProc2dlg(void),setPPchbs(void);
 void strength2dlg(void);
 void mplayer2dlg(int is),mplayer2dlg_1(void),nic2dlg(int is),nic2dlg_1(void);
 void onAuto(void);
 static const int idMplayer[],idNics[],idSPP[];
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TpostProcPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
 virtual void onFrame(void);
};

#endif

