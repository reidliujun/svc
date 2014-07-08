#ifndef _CEQPAGE_H_
#define _CEQPAGE_H_

#include "TconfPageDecAudio.h"

class TeqPage :public TconfPageDecAudio
{
private:
 static const int tbrs[10],idffsFreq[10],idffsDb[10],lblsdb[10],eds[10];
 void lblsdb2dlg(void);
 void onWinampLoad(void);
 float getEqDb(int lowdb,int highdb,int i);
 struct TwinampQ1preset
  {
   ffstring name;
   double preamp,db[10];
  };
 static const TbindTrackbar<TeqPage> htbr[];
 struct TeqTbr : TwidgetSubclassTbr
  {
  private:
   int index;
   TeqPage *self;
  protected:
   virtual void getEditMinMaxVal(const TffdshowParamInfo &info,int *min,int *max,int *val);
   virtual void storeEditValue(const TffdshowParamInfo &info,int min,int max,const char_t *valS);
  public:
   TeqTbr(int Ii,HWND h,TeqPage *Iself,const TbindTrackbar<Twindow> *Ibind):index(Ii),self(Iself),TwidgetSubclassTbr(h,Iself,Ibind) {}
  };
 bool isdb;
 static bool presetsSort(const TwinampQ1preset &p1,const TwinampQ1preset &p2);
protected:
 virtual Twidget* createDlgItem(int id,HWND h);
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TeqPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
};

#endif

