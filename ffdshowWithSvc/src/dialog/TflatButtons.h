#ifndef _TFLATBUTTONS_H_
#define _TFLATBUTTONS_H_

#include "Twidget.h"

struct TflatButton : TwindowWidget
{
private:
 bool isMouse;
 static VOID CALLBACK trackMouseTimerProc(HWND hWnd,UINT uMsg,UINT idEvent,DWORD dwTime);
 static LRESULT CALLBACK btWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
 void drawBT(LPDRAWITEMSTRUCT dis,int id);
protected:
 virtual LRESULT onMouseMove(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
public:
 TflatButton(HWND h,Twindow *w):TwindowWidget(h,w),isMouse(false) {allowOwnProc();}
 void paint(DRAWITEMSTRUCT *dis);
};

#endif
