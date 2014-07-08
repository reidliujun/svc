#ifndef _CPERSPECTIVEPAGE_H_
#define _CPERSPECTIVEPAGE_H_

#include "TconfPageDecVideo.h"

class TperspectivePage :public TconfPageDecVideo
{
private:
 HWND hg;
 struct
  {
   BITMAPINFOHEADER bmiHeader;
   RGBQUAD bmiColors[256];
  } bmp;
 unsigned char *bits;
 unsigned int gx,gy;int stride;
 unsigned int AVIdx,AVIdy;
 unsigned int vx,vy;
 void drawCorners(void);
 void box3( int x, int y,unsigned char c);
 void line(int x1,int y1,int x2,int y2,unsigned char c);
 static LRESULT CALLBACK bmpWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
 int drag,dragx,dragy;
 struct TwidgetPerspective : TwindowWidget
  {
  private:
   TperspectivePage *self;
  protected:
   virtual LRESULT onLbuttonDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onLbuttonUp(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual LRESULT onMouseMove(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
  public:
   TwidgetPerspective(HWND h,TperspectivePage *Iself):self(Iself),TwindowWidget(h,Iself) {allowOwnProc();}
  };
protected:
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
 virtual Twidget* createDlgItem(int id,HWND h);
public:
 TperspectivePage(TffdshowPageDec *Iparent,const TfilterIDFF *idff);
 virtual void init(void);
 virtual void cfg2dlg(void);
 virtual void translate(void);
};

#endif

