#ifndef _TWINDOW_H_
#define _TWINDOW_H_

#include "resource.h"
#include "Crect.h"
#include "Twidget.h"
#include "interfaces.h"

class Ttranslate;
struct Tconfig;
struct Twidget;
struct TffdshowParamInfo;
class Twindow
{
private:
 void enable2(HWND hwnd,BOOL is,bool uncheck);
 char_t *cbxTextBuf;int cbxTextBufLen;
 typedef std::tuple<Twindow*,int,bool> Tenable;
 static BOOL CALLBACK enableWindowsProc(HWND hwnd,LPARAM lParam)
  {
   Tenable *e=(Tenable*)lParam;
   e->get<1>()->enable2(hwnd,e->get<2>(),e->get<3>());
   return TRUE;
  }
 static BOOL CALLBACK showWindowsProc(HWND hwnd,LPARAM lParam)
  {
   ShowWindow(hwnd,(int)lParam);
   return TRUE;
  }
 //std::map<HWND,WNDPROC> translateDefProcs;
 friend struct TwindowWidget;
 static LRESULT CALLBACK translateProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
 typedef std::map<int,strings> TtranslateCbxs;
 TtranslateCbxs translateCbxs;
 HWND hhint;
 static INT_PTR CALLBACK msgProc0(HWND m_hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
 template<class Tret,class TcreateFunc1,class TcreateFunc2> Tret createDlg_Box(int dlgId,HWND hWndParent,LPARAM lparam,TcreateFunc1 createFunc1,TcreateFunc2 createFunc2);
 void setHWND(HWND Im_hwnd,int IdialogId=0);
protected:
 HINSTANCE hi;
 const Tconfig *config;
 int filterMode;

 HMENU _(HMENU hmn);

 void enable(int is,const int id,bool uncheck=true)
  {
   HWND w=GetDlgItem(m_hwnd,id);
   if (w) enable2(w,is,uncheck);
  }
 void enable(int is,const int *wndIDs,bool uncheck=true)
  {
   if (!wndIDs)
    {
     Tenable e(this,is,uncheck);
     EnumChildWindows(m_hwnd,enableWindowsProc,LPARAM(&e));
    }
   else
    for (;*wndIDs;wndIDs++)
     enable2(GetDlgItem(m_hwnd,*wndIDs),is,uncheck);
  }
 bool enabled(int id)
  {
   return !!IsWindowEnabled(GetDlgItem(m_hwnd,id));
  }

 bool ownSetCheck;
 void setCheck(int id,int set)
  {
   ownSetCheck=true;
   SendDlgItemMessage(m_hwnd,id,BM_SETCHECK,set?BST_CHECKED:BST_UNCHECKED,0);
   ownSetCheck=false;
  }
 void setCheck3(int id,int set)
  {
   ownSetCheck=true;
   SendDlgItemMessage(m_hwnd,id,BM_SETCHECK,set==0?BST_UNCHECKED:(set==1?BST_CHECKED:BST_INDETERMINATE),0);
   ownSetCheck=false;
  }
 int getCheck(int id)
  {
   return SendDlgItemMessage(m_hwnd,id,BM_GETCHECK,0,0)==BST_CHECKED?1:0;
  }
 int getCheck3(int id)
  {
   LRESULT c=SendDlgItemMessage(m_hwnd,id,BM_GETCHECK,0,0);
   return c==BST_CHECKED?1:(c==BST_UNCHECKED?0:2);
  }

 void show(int is,int id)
  {
   ShowWindow(GetDlgItem(m_hwnd,id),is?SW_SHOW:SW_HIDE);
  }
 void show(int is)
  {
   EnumChildWindows(m_hwnd,showWindowsProc,is?SW_SHOW:SW_HIDE);
  }
 void show(int is,const int*wndIDs)
  {
   if (!wndIDs)
    show(is);
   else
    for (;*wndIDs;wndIDs++)
     show(is,*wndIDs);
  }

 static void repaint(HWND h)
  {
   InvalidateRect(h,NULL,TRUE);
  }

 static void ListView_AddCol(HWND hlv,int &ncol,int w,const char_t *txt,bool right);
 LPARAM lvGetItemParam(int id,int i);
 void lvSwapItems(int id,int i1,int i2);
 void lvSetSelItem(int id,int ii)
  {
   ListView_SetItemState(GetDlgItem(m_hwnd,id),ii,LVIS_SELECTED,LVIS_SELECTED);
  }
 int lvGetSelItem(int id)
  {
   return ListView_GetNextItem(GetDlgItem(m_hwnd,id),-1,LVNI_SELECTED);
  }
 void ListView_SetColumnText(HWND hlv,int id,const char_t *text)
  {
   LVCOLUMN lc;
   lc.mask=LVCF_TEXT;
   lc.pszText=LPTSTR(text);
   ListView_SetColumn(hlv,id,&lc);
  }

 void tbrSetRange(int id,int min,int max,int step=0,bool largethumb=true);
 void tbrSet(int id,int val,int lblId=0,const char_t *Icapt=NULL,float div=0);
 int  tbrGet(int id)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,TBM_GETPOS,0,0);
  }

 bool eval(HWND hed,int min,int max,int *val=NULL,int mult=1,bool limit=false);
 bool eval(HWND hed,double min,double max,int *val=NULL,double mult=1.0,bool limit=false);
 template<class T,class Ted> bool eval(Ted hed,T min,T max,int idff,T mult=1,bool limit=false)
  {
   int val=cfgGet(idff);
   bool ret=eval(hed,min,max,&val,mult,limit);
   if (ret)
    cfgSet(idff,val);
   return ret;
  }
 template<class T> static bool eval(const char_t *pomS,T min,T max,int *val,T mult=1,bool limit=false);

 void setText(int id,const char_t *fmt,...);
 bool isSetWindowText;
 inline BOOL setWindowText(HWND hwnd,const char_t *text)
  {
   isSetWindowText=true;
   BOOL ret=SetWindowText(hwnd,text);
   isSetWindowText=false;
   return ret;
  }
 inline BOOL setDlgItemText(HWND hwnd,int id,const char_t *text)
  {
   isSetWindowText=true;
   BOOL ret=SetDlgItemText(hwnd,id,text);
   isSetWindowText=false;
   return ret;
  }

 int cbxAdd(int id,const char_t *s)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,CB_ADDSTRING,0,LPARAM(s));
  }
 int cbxAdd(int id,const char_t *s,intptr_t data)
  {
   LRESULT ii=SendDlgItemMessage(m_hwnd,id,CB_ADDSTRING,0,LPARAM(s));
   SendDlgItemMessage(m_hwnd,id,CB_SETITEMDATA,ii,data);
   return (int)ii;
  }
 int cbxGetItemCount(int id)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,CB_GETCOUNT,0,0);
  }
 intptr_t cbxGetItemData(int id,int ii)
  {
   return (intptr_t)SendDlgItemMessage(m_hwnd,id,CB_GETITEMDATA,ii,0);
  }
 intptr_t cbxGetCurItemData(int id)
  {
   int ii=cbxGetCurSel(id);
   if (ii==CB_ERR) return 0;
   return (intptr_t)SendDlgItemMessage(m_hwnd,id,CB_GETITEMDATA,ii,0);
  }
 const char_t *cbxGetCurText(int id)
  {
   int ii=cbxGetCurSel(id);
   if (ii==CB_ERR) return NULL;
   int len=(int)SendDlgItemMessage(m_hwnd,id,CB_GETLBTEXTLEN,ii,0)+1;
   if (len>cbxTextBufLen) cbxTextBuf=(char_t*)realloc(cbxTextBuf,(cbxTextBufLen=len)*sizeof(char_t));
   SendDlgItemMessage(m_hwnd,id,CB_GETLBTEXT,ii,LPARAM(cbxTextBuf));
   return cbxTextBuf;
  }
 int cbxFindItemData(int id,int data)
  {
   int cnt=cbxGetItemCount(id);
   for (int i=0;i<cnt;i++)
    if (SendDlgItemMessage(m_hwnd,id,CB_GETITEMDATA,i,0)==data)
     return i;
   return CB_ERR;
  }
 int cbxSetCurSel(int id,int ii)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,CB_SETCURSEL,ii,0);
  }
 int cbxSetDataCurSel(int id,int data)
  {
   return cbxSetCurSel(id,cbxFindItemData(id,data));
  }
 int cbxGetCurSel(int id)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,CB_GETCURSEL,0,0);
  }
 int cbxClear(int id)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,CB_RESETCONTENT,0,0);
  }
 int cbxFindItem(int id,const char_t *s,bool exact)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,exact?CB_FINDSTRINGEXACT:CB_FINDSTRING,WPARAM(-1),LPARAM(s));
  }
 void cbxSetDroppedWidth(int id,int width)
  {
   SendDlgItemMessage(m_hwnd,id,CB_SETDROPPEDWIDTH,width,0);
  }
 int cbxGetDroppedWidth(int id)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,CB_GETDROPPEDWIDTH,0,0);
  }
 void cbxTranslate(int id,const char_t **list);

 static void insertSeparator(HMENU hm,int &ord);
 static void insertSubmenu(HMENU hm,int &ord,const char_t *caption,HMENU subMenu);
 static void insertMenuItem(HMENU hm,int &ord,int id,const char_t *caption,bool checked,bool newcolumn=false);
 static void enable(HMENU hm,int ord,bool is);
 int selectFromMenu(const char_t **items,int id,bool translate=true,int columnHeight=0); //returns -1 if nothing was selected
 int selectFromMenu(const strings &items,int id,bool translate=true,int columnHeight=0);

 bool inputString(const char_t *capt,const char_t *query,char_t *str,size_t len);

 int lbxAdd(int id,const char_t *s)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,LB_ADDSTRING,0,LPARAM(s));
  }
 int lbxAdd(int id,const char_t *s,intptr_t data)
  {
   LRESULT ii=SendDlgItemMessage(m_hwnd,id,LB_ADDSTRING,0,LPARAM(s));
   SendDlgItemMessage(m_hwnd,id,LB_SETITEMDATA,ii,data);
   return (int)ii;
  }
 int lbxClear(int id)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,LB_RESETCONTENT,0,0);
  }
 int lbxGetItemCount(int id)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,LB_GETCOUNT,0,0);
  }
 intptr_t lbxGetItemData(int id,int ii)
  {
   return (intptr_t)SendDlgItemMessage(m_hwnd,id,LB_GETITEMDATA,ii,0);
  }
 int lbxFindItemData(int id,intptr_t data)
  {
   int cnt=lbxGetItemCount(id);
   for (int i=0;i<cnt;i++)
    if (SendDlgItemMessage(m_hwnd,id,LB_GETITEMDATA,i,0)==data)
     return i;
   return LB_ERR;
  }
 intptr_t lbxGetCurItemData(int id)
  {
   LRESULT ii=SendDlgItemMessage(m_hwnd,id,LB_GETCURSEL,0,0);
   if (ii==LB_ERR) return 0;
   return SendDlgItemMessage(m_hwnd,id,LB_GETITEMDATA,ii,0);
  }
 int lbxGetCurSel(int id)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,LB_GETCURSEL,0,0);
  }
 int lbxSetCurSel(int id,int ii)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,LB_SETCURSEL,ii,0);
  }
 int lbxGetCurText(int id,char_t *buf)
  {
   LRESULT ret=SendDlgItemMessage(m_hwnd,id,LB_GETTEXT,lbxGetCurSel(id),LPARAM(buf));
   if (ret==LB_ERR)
    buf[0]='\0';
   return (int)ret;
  }
 int lbxFindItem(int id,const char_t *s,bool exact)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,exact?LB_FINDSTRINGEXACT:LB_FINDSTRING,WPARAM(-1),LPARAM(s));
  }

 int edLimitText(int id,size_t limit)
  {
   return (int)SendDlgItemMessage(m_hwnd,id,EM_SETLIMITTEXT,limit,0);
  }
 void edReadOnly(int id,bool is)
  {
   SendDlgItemMessage(m_hwnd,id,EM_SETREADONLY,is?TRUE:FALSE,0);
  }

 int  cfgGet(unsigned int i);
 void cfgGet(unsigned int i,char_t *buf,size_t buflen);
 const char_t* cfgGetStr(unsigned int i);
 void cfgSet(unsigned int i,int val);
 void cfgSet(unsigned int i,const char_t *val);

 bool changeDir(int idff,const char_t *caption);
 bool chooseColor(int idff);

 CRect getChildRect(int id) const;

 static void setSizeD(HWND m_hwnd,int dx,int dy);
 void setSizeD(int dx,int dy),setItemSizeD(int id,int dx,int dy);
 static void setSize(HWND m_hwnd,int dx,int dy);
 void setSize(int dx,int dy),setItemSize(int id,int dx,int dy);

 static void setPosD(HWND m_hwnd,int x,int y);
 void setPosD(int x,int y),setItemPosD(int id,int x,int y);
 static void setPos(HWND m_hwnd,int x,int y);
 void setPos(int x,int y),setItemPos(int id,int x,int y);

 struct TanchorInfo
  {
   int id;
   enum
    {
     LEFT  =1,
     TOP   =2,
     RIGHT =4,
     BOTTOM=8
    };
   int anchor;
  };
 struct Tanchor
  {
   Tanchor(int Iid,int Ianchors,const Twindow &parent,const CRect &parentRect);
   int id;
   int anchors;
   CPoint rules;
   CSize originalParentSize;
   void resize(const Twindow &parent,const CRect &newrect);
  };
 struct Tanchors :public std::vector<Tanchor>
  {
   void init(const TanchorInfo *ainfo,const Twindow &parent)
    {
     clear();
     add(ainfo,parent);
    }
   void add(const TanchorInfo *ainfo,const Twindow &parent);
   void resize(const Twindow &parent,const CRect &newrect);
  } anchors;

 TOOLINFO addHint(int id,const char_t *text);
 HWND createHintWindow(HWND parent,int timePop=1700,int timeInit=70,int timeReshow=7);

 void setFont(int id,HFONT hf,bool repaint=false)
  {
   SendDlgItemMessage(m_hwnd,id,WM_SETFONT,WPARAM(hf),LPARAM(repaint?TRUE:FALSE));
  }

 virtual INT_PTR msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
 virtual INT_PTR defMsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam) {return FALSE;}
 void setDlgResult(LONG res)
  {
   SetWindowLongPtr(m_hwnd,DWLP_MSGRESULT,res);
  }

 virtual Twidget* createDlgItem(int id,HWND h);

 template<class T> struct TbindCheckbox
  {
   int idc;
   int idff;
   void (T::*onClick)(void);
  };
 typedef const TbindCheckbox<Twindow> *TbindCheckboxes;
 template<class T> void bindCheckboxes(T *chb) {bindsCheckbox=(TbindCheckboxes)chb;}

 template<class T> struct TbindTrackbar
  {
   int idc;
   int idff;
   void (T::*onClick)(void);
  };
 typedef const TbindTrackbar<Twindow> *TbindTrackbars;
 template<class T> void bindHtracks(T *htbr) {bindsHtrack=(TbindTrackbars)htbr;}
 template<class T> void bindVtracks(T *vtbr) {bindsVtrack=(TbindTrackbars)vtbr;}
 virtual int getTbrIdff(int id,const TbindTrackbars bind) {return ff_abs(bind->idff);}

 template<class T> struct TbindRadiobutton
  {
   int idc;
   int idff;
   int val;
   void (T::*onClick)(void);
  };
 typedef const TbindRadiobutton<Twindow> *TbindRadioButtons;
 template<class T> void bindRadioButtons(T *rbt) {bindsRadiobutton=(TbindRadioButtons)rbt;}

 template<class T> struct TbindEditInt
  {
   int idc;
   int min,max;
   int idff;
   void (T::*onClick)(void);
  };
 typedef const TbindEditInt<Twindow> *TbindEditInts;
 template<class T> void bindEditInts(T *ed) {bindsEditInt=(TbindEditInts)ed;}

 template<class T> struct TbindEditReal
  {
   int idc;
   double min,max;
   int idff;
   double mult;
   void (T::*onClick)(void);
  };
 typedef const TbindEditReal<Twindow> *TbindEditReals;
 template<class T> void bindEditReals(T *ed) {bindsEditReal=(TbindEditReals)ed;}

 template<class TbindEdit> bool onCtlColorEdit(TbindEdit *bindsEdit,LPARAM lParam,WPARAM wParam);
 virtual void onEditChange(void) {}
 HBRUSH getRed(void);

 enum BINDCBX
  {
   BINDCBX_SEL,
   BINDCBX_DATA,
   BINDCBX_TEXT,
   BINDCBX_DATATEXT,
   BINDCBX_NONE
  };
 template<class T> struct TbindCombobox
  {
   int idc;
   int idff;
   BINDCBX mode;
   void (T::*onClick)(void);
  };
 typedef const TbindCombobox<Twindow> *TbindComboboxes;
 template<class T> void bindComboboxes(T *cbx) {bindsCombobox=(TbindComboboxes)cbx;}

 template<class T> struct TbindButton
  {
   int idc;
   void (T::*onClick)(void);
  };
 typedef const TbindButton<Twindow> *TbindButtons;
 template<class T> void bindButtons(T *bt) {bindsButton=(TbindButtons)bt;}
private:
 TbindButtons bindsButton;
 TbindComboboxes bindsCombobox;
 TbindEditInts bindsEditInt;
 TbindEditReals bindsEditReal;
 TbindRadioButtons bindsRadiobutton;
 TbindCheckboxes bindsCheckbox;typedef std::hash_map<int,const TbindCheckbox<Twindow>*> TcheckboxesMap;TcheckboxesMap bindCheckboxesMap;
 TbindTrackbars bindsHtrack,bindsVtrack;typedef std::hash_map<int,const TbindTrackbar<Twindow>*> TtrackBarsMap;TtrackBarsMap bindTrackbarsMap;
 bool onTrack(TbindTrackbars bindsTrack,LPARAM lParam);
 HBRUSH red;
 struct TwidgetSubclass : TwindowWidget
  {
  protected:
   Twindow *self;
  public:
   TwidgetSubclass(HWND h,Twindow *Iself):self(Iself),TwindowWidget(h,Iself) {}
  };
protected:
 struct TwidgetSubclassTbr : TwidgetSubclass
  {
  protected:
   const TbindTrackbar<Twindow> *bind;
   virtual LRESULT onContextMenu(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
   virtual bool onReset(void);
   virtual bool onEdit(void);
   virtual void getEditMinMaxVal(const TffdshowParamInfo &info,int *min,int *max,int *val);
   virtual void storeEditValue(const TffdshowParamInfo &info,int min,int max,const char_t *valS);
  public:
   TwidgetSubclassTbr(HWND h,Twindow *Iself,const TbindTrackbar<Twindow> *Ibind):TwidgetSubclass(h,Iself),bind(Ibind) {allowOwnProc();}
  };
 struct TwidgetSubclassChb : TwidgetSubclass
  {
  private:
   const TbindCheckbox<Twindow> *bind;
  protected:
   virtual LRESULT onContextMenu(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
  public:
   TwidgetSubclassChb(HWND h,Twindow *Iself,const TbindCheckbox<Twindow> *Ibind):TwidgetSubclass(h,Iself),bind(Ibind) {allowOwnProc();}
  };
public:
 comptrQ<IffdshowBase> deci;
 HWND m_hwnd;
 int dialogId;
 Ttranslate *tr;
 const char_t* _(int item);
 const char_t* _(int id,const char_t *s,bool addcbx=true);
 Twindow(IffdshowBase *Ideci);
 virtual ~Twindow();
 void setDeci(IffdshowBase *Ideci);
 virtual void translate(void);

 virtual void init(void) {}
 virtual void cfg2dlg(void) {}

 bool resized;
 void resizeDialog();
 void resizeDialog(HWND hwnd);
 static void resizeDialog(HWND hwnd, bool relative,int scale);

 bool wndEnabled;
 bool enable(void)
  {
   bool is=wndEnabled=enabled();
   Tenable e(this,is);
   EnumChildWindows(m_hwnd,enableWindowsProc,LPARAM(&e));
   return is;
  }
 virtual bool enabled(void) {return true;}
 void show(bool is)
  {
   ShowWindow(m_hwnd,is?SW_SHOW:SW_HIDE);
  }
 virtual void enableHints(bool is);

 HWND createDialog(int dlgId,HWND hWndParent),createDialog(int dlgId,HWND hWndParent,LPARAM lparam);
 INT_PTR dialogBox(int dlgId,HWND hWndParent),dialogBox(int dlgId,HWND hWndParent,LPARAM lparam);

 void msg(const char_t *text,const char_t *capt=_l("ffdshow"));
 void err(const char_t *text,const char_t *capt=NULL);

 static int getId(HWND h) {return GetWindowLong(h,GWL_ID);}
};

class TdlgWindow :public Twindow
{
public:
 TdlgWindow(int IdialogId,IffdshowBase *Ideci=NULL);
};

class TdlgPropsEdit :public TdlgWindow
{
public:
 typedef std::vector< std::pair<ffstring,ffstring> > TstringProps;
private:
 HFONT italicFont;
 TstringProps &props;
 HWND hlv;
protected:
 virtual INT_PTR msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
public:
 TdlgPropsEdit(TstringProps &Iprops,IffdshowBase *deci);
 virtual ~TdlgPropsEdit();
 bool execute(HWND wndParent);
};

class TdlgInputString : public TdlgWindow
{
private:
 const char_t *capt,*query;
 char_t *str;
 size_t len;
protected:
 virtual INT_PTR msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
public:
 TdlgInputString(const char_t *Icapt,const char_t *Iquery,char_t *Istr,size_t Ilen,IffdshowBase *deci);
 bool execute(HWND wndParent);
};

#endif
