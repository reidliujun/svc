#ifndef _TPRESETAUTOLOAD_H_
#define _TPRESETAUTOLOAD_H_

#include "Twindow.h"
#include "interfaces.h"

class TpresetAutoloadDlgBase :public TdlgWindow
{
private:
 HWND parent;
 int x,y;
 bool closing;
 struct Titem
  {
   Titem(void):wasChange(false) {}
   const char_t *name,*hint;int allowWildcard;
   int is;
   int isVal;char_t val[MAX_PATH];
   int isList,isHelp;
   bool wasChange;
  };
 typedef std::vector<Titem> Titems;
 Titems items;
 class TitemsWindow :public TdlgWindow
  {
  private:
   HFONT arrowsFont;
   Titems &items;
   comptrQ<IffdshowDec> deciD;
  protected:
   virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
  public:
   TitemsWindow(IffdshowBase *Ideci,HWND parent,Titems &Iitems);
   virtual ~TitemsWindow();
   void showItems(void);
  } *itemsw;
protected:
 comptrQ<IffdshowDec> deciD;
 HBRUSH red;
 virtual void init(void);
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TpresetAutoloadDlgBase(IffdshowBase *Ideci,HWND parent,int x,int y);
 bool show(void);
};

class TpresetAutoloadDlgVideo :public TpresetAutoloadDlgBase
{
private:
 void cond2dlg(void);
protected:
 virtual void init(void);
 virtual Twidget* createDlgItem(int id,HWND h);
 virtual INT_PTR msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
 TpresetAutoloadDlgVideo(IffdshowBase *Ideci,HWND parent,int x,int y);
 static TpresetAutoloadDlgBase* create(IffdshowBase *Ideci,HWND parent,int x,int y)
  {
   return new TpresetAutoloadDlgVideo(Ideci,parent,x,y);
  }
};

class TpresetAutoloadDlgAudio :public TpresetAutoloadDlgBase
{
protected:
 virtual void init(void);
public:
 TpresetAutoloadDlgAudio(IffdshowBase *Ideci,HWND parent,int x,int y);
 static TpresetAutoloadDlgBase* create(IffdshowBase *Ideci,HWND parent,int x,int y)
  {
   return new TpresetAutoloadDlgAudio(Ideci,parent,x,y);
  }
};

#endif
