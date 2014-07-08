#ifndef _CLEVELSPAGE_H_
#define _CLEVELSPAGE_H_

#include "TconfPageDecVideo.h"

struct IimgFilterLevels;
class TlevelsPage :public TconfPageDecVideo
{
private:
 HBITMAP bmpGradient;HDC hdcGradient;HFONT fontGradient,fontCurier;HGDIOBJ oldHDCgradient;
 void drawTriangle(DRAWITEMSTRUCT *dis,int cl,bool fade=false);
 void levels2dlg(void),map2dlg(void),mode2dlg(void);
 void onCurveLoad(void);char_t curvesflnm[MAX_PATH];
 HWND hIn,hOut;
 int bdx;
 static LRESULT CALLBACK bmpWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
 struct
  {
   BITMAPINFOHEADER bmiHeader;
   RGBQUAD bmiColors[256];
  } histogramBmp,curvesBmp;
 unsigned char histogramBits[256*256];
 unsigned int map[256];bool isMap;
 bool startup;
 struct TwidgetLevels : TwindowWidget
  {
  private:
   TlevelsPage *levelsPage;
   const int idffMin,idffMax;
   int dragIdff;
   int dx;
  protected:
   virtual LRESULT onLbuttonDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onLbuttonUp(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onMouseMove(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
  public:
   TwidgetLevels(HWND h,TlevelsPage *Iself,int IidffMin,int IidffMax):levelsPage(Iself),TwindowWidget(h,Iself),dragIdff(0),idffMin(IidffMin),idffMax(IidffMax),dx(0) {allowOwnProc();}
   void draw(LPDRAWITEMSTRUCT dis);
  } *wIn;
 struct TwidgetCurves : TwindowWidget
  {
  private:
   TlevelsPage *levelsPage;
   HWND h;
   int dxy;
   typedef array_vector<CPoint,20> CPoints;
   CPoints pt;
   void load(void),save(void),print(void);
   HBRUSH br;
   int curpoint,dragpoint;
   std::pair<int,int> findPoint(int sx,int sy);
  protected:
   virtual LRESULT onLbuttonDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onLbuttonUp(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onMouseMove(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
  public:
   static const int idffs[];
   TwidgetCurves(HWND h,TlevelsPage *Iself);
   virtual ~TwidgetCurves();
   void draw(LPDRAWITEMSTRUCT dis);
   bool setCursor(void);
   void reset(void) {load();}
  } *wCurves;
 HWND hcurves;
 IimgFilterLevels *filter;
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual Twidget* createDlgItem(int id,HWND h);
public:
 TlevelsPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
 virtual void onFrame(void);
 virtual bool reset(bool testonly=false);
};

#endif

