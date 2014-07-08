#ifndef _CMIXERPAGE_H_
#define _CMIXERPAGE_H_

#include "TconfPageDecAudio.h"

struct IaudioFilterMixer;
class TmixerPage :public TconfPageDecAudio
{
private:
 static const int idffs[9*9],idcs[9*9+1];
 static bool isEdit(int idc);
 void matrix2dlg(void),slev2dlg(void),clev2dlg(void),lfe2dlg(void),dlg2out(void);
 struct TwidgetMixer : TwindowWidget
  {
   int idff;
   TwidgetMixer(HWND h,Twindow *w):TwindowWidget(h,w) {}
  };
 IaudioFilterMixer *filter;
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual Twidget* createDlgItem(int id,HWND h);
public:
 TmixerPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
 virtual void onFrame(void);
 virtual void onNewFilter(void);
};

#endif

