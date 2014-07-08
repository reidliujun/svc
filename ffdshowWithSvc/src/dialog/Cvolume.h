#ifndef _CVOLUMEPAGE_H_
#define _CVOLUMEPAGE_H_

#include "TconfPageDecAudio.h"

struct IaudioFilterVolume;
class TvolumePage :public TconfPageDecAudio
{
private:
 void switchDb(void);
 void volTbr2cfg(int idc,int idff,int mult),cfg2volTbr(int tbr,int lbl,int idff,int lbltran,int mult);

 void normalize2dlg(void),master2dlg(void),speakers2dlg(void),mute2dlg(void);
 static const int tbrs[8],lbls[8],pbrs[8],idffs[8],idffsMS[8],mutes[8],solos[8];
 IaudioFilterVolume *filter;
 struct TwidgetMS : TwindowWidget
  {
   int idff;
   TwidgetMS(HWND h,Twindow *w,int Iidff):TwindowWidget(h,w),idff(Iidff) {}
  };
 static const TbindTrackbar<TvolumePage> htbr[];
 struct TvolumeTbr : TwidgetSubclassTbr
  {
  private:
   const bool &isdb;
  protected:
   virtual void getEditMinMaxVal(const TffdshowParamInfo &info,int *min,int *max,int *val);
   virtual void storeEditValue(const TffdshowParamInfo &info,int min,int max,const char_t *valS);
  public:
   TvolumeTbr(const bool &Iisdb,HWND h,Twindow *Iself,const TbindTrackbar<Twindow> *Ibind):isdb(Iisdb),TwidgetSubclassTbr(h,Iself,Ibind) {}
  };
 bool isdb;
protected:
 virtual Twidget* createDlgItem(int id,HWND h);
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TvolumePage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void onFrame(void);
};

#endif

