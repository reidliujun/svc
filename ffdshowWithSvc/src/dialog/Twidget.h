#ifndef _TWIDGET_H_
#define _TWIDGET_H_

struct Twidget
{
private:
 HWND h;
 bool ownproc;WNDPROC oldproc;
 static LRESULT CALLBACK msgProc0(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   Twidget *self=getDlgItem(hwnd);
   switch (uMsg)
    {
     case WM_LBUTTONDOWN:return self->onLbuttonDown(hwnd,uMsg,wParam,lParam);
     case WM_LBUTTONUP  :return self->onLbuttonUp(hwnd,uMsg,wParam,lParam);
     case WM_RBUTTONDOWN:return self->onRbuttonDown(hwnd,uMsg,wParam,lParam);
     case WM_MOUSEMOVE  :return self->onMouseMove(hwnd,uMsg,wParam,lParam);
     case WM_KEYDOWN    :return self->onKeyDown(hwnd,uMsg,wParam,lParam);
     case WM_KEYUP      :return self->onKeyUp(hwnd,uMsg,wParam,lParam);
     case WM_CHAR       :return self->onChar(hwnd,uMsg,wParam,lParam);
     case WM_SYSKEYDOWN :return self->onSysKeyDown(hwnd,uMsg,wParam,lParam);
     case WM_VSCROLL    :return self->onVscroll(hwnd,uMsg,wParam,lParam);
     case WM_HSCROLL    :return self->onHscroll(hwnd,uMsg,wParam,lParam);
     case WM_PAINT      :return self->onPaint(hwnd,uMsg,wParam,lParam);
     case WM_KILLFOCUS  :return self->onKillFocus(hwnd,uMsg,wParam,lParam);
     case WM_GETDLGCODE :return self->onGetDlgCode(hwnd,uMsg,wParam,lParam);
     case WM_CONTEXTMENU:return self->onContextMenu(hwnd,uMsg,wParam,lParam);
     default:return CallWindowProc(self->oldproc,hwnd,uMsg,wParam,lParam);
    }
  }
protected:
 int id;
 virtual LRESULT onLbuttonDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onLbuttonUp(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onRbuttonDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onMouseMove(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onKeyDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onKeyUp(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onChar(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onSysKeyDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onVscroll(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onHscroll(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onPaint(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onKillFocus(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onGetDlgCode(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
 virtual LRESULT onContextMenu(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
  {
   return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
  }
public:
 Twidget(HWND Ih);
 static Twidget* getDlgItem(HWND h)
  {
   return (Twidget*)GetWindowLongPtr(h,GWLP_USERDATA);
  }
 void allowOwnProc(void)
  {
   if (!ownproc)
    {
     ownproc=true;
     oldproc=(WNDPROC)SetWindowLongPtr(h,GWLP_WNDPROC,LONG_PTR(msgProc0));
    }
  }
 void restore(void)
  {
   if (ownproc) SetWindowLongPtr(h,GWLP_WNDPROC,LONG_PTR(oldproc));
   SetWindowLong(h,GWLP_USERDATA,0);
  }
};

class Twindow;
struct TwindowWidget :Twidget
{
protected:
 virtual LRESULT onContextMenu(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
public:
 Twindow *window;
 TwindowWidget(HWND Ih,Twindow *Iwindow):Twidget(Ih),window(Iwindow) {}
};

#endif
