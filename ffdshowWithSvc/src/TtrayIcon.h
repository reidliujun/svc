#ifndef _TTRAYICON_H_
#define _TTRAYICON_H_

#include "ffdshow_constants.h"
#include "interfaces.h"

struct TtrayIconStartData : std::tuple<IffdshowBase*,CAMEvent*,HWND*> {};

struct Tconfig;
class Ttranslate;
struct TfilterIDFF;
class TtrayIconBase
{
private:
 const Tconfig *config;
 ATOM at;
 HWND h;
 void init(void);
 static LRESULT CALLBACK trayWndProc(HWND hwnd, UINT msg, WPARAM wprm, LPARAM lprm);
 HINSTANCE hi;
 #ifdef UNICODE
 struct NOTIFYICONDATAW
  {
   DWORD cbSize;
   HWND hWnd;
   UINT uID;
   UINT uFlags;
   UINT uCallbackMessage;
   HICON hIcon;
   WCHAR   szTip[128];
   DWORD dwState;
   DWORD dwStateMask;
   WCHAR   szInfo[256];
   union
    {
     UINT  uTimeout;
     UINT  uVersion;
    } DUMMYUNIONNAME;
   WCHAR   szInfoTitle[64];
   DWORD dwInfoFlags;
  } nid;
 #else
 struct NOTIFYICONDATAA
  {
   DWORD cbSize;
   HWND hWnd;
   UINT uID;
   UINT uFlags;
   UINT uCallbackMessage;
   HICON hIcon;
   CHAR   szTip[128];
   DWORD dwState;
   DWORD dwStateMask;
   CHAR   szInfo[256];
   union
    {
     UINT  uTimeout;
     UINT  uVersion;
    } DUMMYUNIONNAME;
   CHAR   szInfoTitle[64];
   DWORD dwInfoFlags;
  } nid;
 #endif
 enum
  {
   TRAYICON=2999,
   MSG_TRAYICON=WM_APP+9,
   TMR_TRAYICON=2998,
  };
 bool visible;
protected:
 TtrayIconBase(IffdshowBase *Ideci);
 virtual ~TtrayIconBase();
 unsigned int run(CAMEvent *ev,HWND *hwndRef);

 int mode;
 Ttranslate *tr;
 char_t classname[60];
 const char_t *tip;int icon;
 comptr<IffdshowBase> deci;
 virtual HMENU createMenu(int &ord);
 void insertSeparator(HMENU hm,int &ord);
 void insertSubmenu(HMENU hm,int &ord,const char_t *caption,bool translate,HMENU subMenu);
 int insertMenuItem(HMENU hm,int &ord,int id,const char_t *caption,bool translate,bool checked,bool enabled);
 virtual LRESULT processTrayMsg(HWND hwnd,WPARAM wprm,LPARAM lprm);
 virtual void processCmd(HMENU hm,int cmd);
public:
 void show(void),hide(void);
 int cfgGet(int id),cfgSet(int id,int val);
 template<class Ttray> static unsigned int __stdcall start(TtrayIconStartData *sd)
  {
   return (new Ttray(sd->get<1>()))->run(sd->get<2>(),sd->get<3>());
  }
};

struct TfilterIDFF;
class TtrayIconDec :public TtrayIconBase
{
protected:
 enum
  {
   IDC_FIRST_PRESET=3999,
   IDC_FIRST_GRAPH =4999,
   IDC_FIRST_FILTER=5999
  };
private:
 void insertMenuItemFilter(HMENU hm,int &ord,const TfilterIDFF *idff);
 std::hash_map<int,ffstring> graphnames;
protected:
 TtrayIconDec(IffdshowBase *Ideci);

 comptrQ<IffdshowDec> deciD;
 typedef std::pair<int,const TfilterIDFF*> TordFilters;
 static bool sortOrdFilters(const TordFilters &of1,const TordFilters &of2);
 virtual void insertSubmenuCallback(HMENU hm,int &ord,const TfilterIDFF *f) {}

 virtual HMENU createMenu(int &ord);
 void negate_Param(int id);
 virtual void processCmd(HMENU hm,int cmd);
 virtual LRESULT processTrayMsg(HWND hwnd,WPARAM wprm,LPARAM lprm);
 void showFilterCfg(const char_t *fltname);
 virtual bool showffdshowCfg(char_t* filterName){return false;}
};

class TtrayIconDecVideo :public TtrayIconDec
{
private:
 comptrQ<IffdshowDecVideo> deciV;
 HMENU makeSubtitlesMenu(void);
 enum
  {
   IDC_FIRST_SUBFILE=8999,
   IDC_FIRST_SUBLANG=10999,
   IDC_FIRST_TEXTPIN=11999,
  };
protected:
 virtual void insertSubmenuCallback(HMENU hm,int &ord,const TfilterIDFF *f);
 virtual void processCmd(HMENU hm,int cmd);
 virtual bool showffdshowCfg(char_t* filterName);
public:
 TtrayIconDecVideo(IffdshowBase *Ideci);
};

class TtrayIconDecAudio :public TtrayIconDec
{
private:
 comptrQ<IffdshowDecAudio> deciA;
 enum
  {
   IDC_FIRST_STREAM=9999
  };
protected:
 virtual HMENU createMenu(int &ord);
 virtual void processCmd(HMENU hm,int cmd);
 virtual bool showffdshowCfg(char_t* filterName);
public:
 TtrayIconDecAudio(IffdshowBase *Ideci);
};

struct IffdshowEnc;
class TtrayIconEnc :public TtrayIconBase
{
private:
 comptrQ<IffdshowEnc> deciE;
protected:
 virtual LRESULT processTrayMsg(HWND hwnd,WPARAM wprm,LPARAM lprm);
public:
 TtrayIconEnc(IffdshowBase *Ideci);
};

#endif
