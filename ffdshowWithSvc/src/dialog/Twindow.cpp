/*
 * Copyright (c) 2002-2006 Milan Cutka
 * anchors inspired by Visual Component Library by Borland
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Twindow.h"
#include "Ttranslate.h"
#include "IffdshowBase.h"
#include "ffdshow_constants.h"
#include "Crect.h"
#include "ffdebug.h"
#include "IffdshowParamsEnum.h"
#include "TdialogSettings.h"
#include "reg.h"

//=================================== Twindow ==================================
Twindow::Twindow(IffdshowBase *Ideci):
 tr(NULL),
 wndEnabled(true),
 isSetWindowText(false),
 bindsCheckbox(NULL),bindsHtrack(NULL),bindsVtrack(NULL),bindsRadiobutton(NULL),bindsEditInt(NULL),bindsEditReal(NULL),bindsCombobox(NULL),bindsButton(NULL),
 red(NULL),
 filterMode(NULL),
 resized(false)
{
 if (Ideci)
  setDeci(Ideci);
 else
  {
   deci=NULL;
   hi=NULL;
  }
 cbxTextBuf=NULL;cbxTextBufLen=0;
 hhint=NULL;
}
Twindow::~Twindow()
{
// if (hhint) DestroyWindow(hhint);
 if (cbxTextBuf) free(cbxTextBuf);
 if (tr) tr->release();
}
void Twindow::setDeci(IffdshowBase *Ideci)
{
 deci=Ideci;
 deci->getTranslator(&tr);
 deci->getConfig(&config);
 filterMode=cfgGet(IDFF_filterMode);
 hi=(HINSTANCE)deci->getInstance2();
}

Twidget* Twindow::createDlgItem(int id,HWND h)
{
 TtrackBarsMap::const_iterator t=bindTrackbarsMap.find(id);
 if (t!=bindTrackbarsMap.end())
  return new TwidgetSubclassTbr(h,this,t->second);
 else
  {
   TcheckboxesMap::const_iterator t=bindCheckboxesMap.find(id);
   if (t!=bindCheckboxesMap.end())
    return new TwidgetSubclassChb(h,this,t->second);
   else
    return new TwindowWidget(h,this);
  }
}

void Twindow::setHWND(HWND Im_hwnd,int IdialogId)
{
 m_hwnd=Im_hwnd;
 if (IdialogId) dialogId=IdialogId;
 THWNDs wnds;getChildWindows(m_hwnd,wnds);
 for (THWNDs::const_iterator wnd=wnds.begin();wnd!=wnds.end();wnd++)
  {
   Twidget *dlgitem=createDlgItem(getId(*wnd),*wnd);
   if (tr->translateMode && dialogId!=IDD_PROPSEDIT)
    {
     char_t className[256];
     GetClassName(*wnd,className,256);
     if (stricmp(className,_l("STATIC"))==0 || stricmp(className,_l("BUTTON"))==0 || stricmp(className,_l("ComboBox"))==0 || stricmp(className,_l("syslistview32"))==0)
      {
       if (stricmp(className,_l("STATIC"))==0)
        SetWindowLong(*wnd,GWL_STYLE,GetWindowLong(*wnd,GWL_STYLE)|SS_NOTIFY);
       else if (stricmp(className,_l("BUTTON"))==0)
        {
         SetWindowLong(*wnd,GWL_STYLE,GetWindowLong(*wnd,GWL_STYLE)|BS_NOTIFY);
         if ((GetWindowLong(*wnd,GWL_STYLE)&BS_GROUPBOX)==BS_GROUPBOX)
          {
           char_t windowtext[256];
           GetWindowText(*wnd,windowtext,256);
           if (windowtext[0]!='\0')
            {
             SendMessage(*wnd,BM_SETSTYLE,BS_PUSHBUTTON,1);
             RECT r;
             GetWindowRect(*wnd,&r);
             SetWindowPos(*wnd,NULL,0,0,r.right-r.left,15,SWP_NOMOVE|SWP_NOZORDER);
            }
          }
        }
       EnableWindow(*wnd,TRUE);
       dlgitem->allowOwnProc(); //setDlgItemProc(*wnd,translateProc);
      }
    }
  }
 Twidget *dlgitem=createDlgItem(0,m_hwnd);
 //SetWindowLong(m_hwnd,GWL_USERDATA,LONG(createDlgItem()));
 if (tr->translateMode)
   dlgitem->allowOwnProc();// setDlgItemProc(m_hwnd,translateProc);
}

const char_t* Twindow::_(int item)
{
 return tr->translate(m_hwnd,dialogId,item,NULL);
}
const char_t* Twindow::_(int id,const char_t *s,bool addcbx)
{
 if (addcbx && tr->translateMode && s[0])
  {
   strings &list=translateCbxs[id];
   if (!isIn(list,ffstring(s)))
    list.push_back(s);
  }
 return tr->translate(s);
}
HMENU Twindow::_(HMENU hmn)
{
 if (hmn)
  {
   int cnt=GetMenuItemCount(hmn);
   for (int i=0;i<cnt;i++)
    {
     MENUITEMINFO mii;memset(&mii,0,sizeof(mii));
     mii.cbSize=sizeof(mii);
     mii.fMask=MIIM_STRING;
     char_t capt[256];
     mii.dwTypeData=capt;
     mii.cch=255;
     GetMenuItemInfo(hmn,i,TRUE,&mii);
     ff_strncpy(mii.dwTypeData, _(-dialogId,mii.dwTypeData), countof(capt));
     SetMenuItemInfo(hmn,i,TRUE,&mii);
    }
  }
 return hmn;
}
void Twindow::translate(void)
{
 tr->translate(m_hwnd,dialogId);
/*
 HFONT hf=(HFONT)SendMessage(m_hwnd,WM_GETFONT,NULL,NULL);
 LOGFONT lf;
 GetObject(hf,sizeof(lf),&lf);
 lf.lfCharSet=RUSSIAN_CHARSET;
 HFONT hf2=CreateFontIndirect(&lf);
 THWNDs wnds;
 getChildWindows(m_hwnd,wnds);
 for (THWNDs::const_iterator w=wnds.begin();w!=wnds.end();w++)
  SendMessage(*w,WM_SETFONT,(WPARAM)hf2,TRUE);
 SendMessage(m_hwnd,WM_SETFONT,(WPARAM)hf2,TRUE);
*/
}

void Twindow::enable2(HWND hwnd,BOOL is,bool uncheck)
{
 if (tr->translateMode) return;
 EnableWindow(hwnd,is);
 if (!is && uncheck)
  {
   char_t name[256];
   if (GetClassName(hwnd,name,255) && stricmp(name,_l("button"))==0)
    {
     ownSetCheck=true;
     SendMessage(hwnd,BM_SETCHECK,BST_UNCHECKED,0);
     ownSetCheck=false;
    }
  }
}

template<class T> static T strto(const char_t *s,char_t **e);
template<> double strto(const char_t *s,char_t **e) {return strtod(s,e);}
template<> int strto(const char_t *s,char_t **e) {return strtol(s,e,10);}
template<class T> bool Twindow::eval(const char_t *pomS,T min,T max,int *val,T mult,bool dolimit)
{
 if (pomS[0]=='\0') return false;
 char_t *stop=NULL;
 T x=strto<T>(pomS,&stop);
 if (*stop) return false;
 if (!isIn(x,min,max))
  if (!dolimit)
   return false;
  else
   x=limit(x,min,max);
 if (val) *val=int(x*mult); //round?
 return true;
}
template bool Twindow::eval(const char_t *pomS,int min,int max,int *val,int mult,bool dolimit);
template bool Twindow::eval(const char_t *pomS,double min,double max,int *val,double mult,bool dolimit);

bool Twindow::eval(HWND hed,int min,int max,int *val,int mult,bool limit)
{
 char_t pomS[256];
 GetWindowText(hed,pomS,255);
 return eval(pomS,min,max,val,mult,limit);
}
bool Twindow::eval(HWND hed,double min,double max,int *val,double mult,bool limit)
{
 char_t pomS[256];
 GetWindowText(hed,pomS,255);
 return eval(pomS,min,max,val,mult,limit);
}

bool Twindow::inputString(const char_t *capt,const char_t *query,char_t *str,size_t len)
{
 TdlgInputString dlg(capt,query,str,len,deci);
 return dlg.execute(m_hwnd);
}

void Twindow::ListView_AddCol(HWND hlv,int &ncol,int w,const char_t *txt,bool right)
{
 LVCOLUMN lvc;
 lvc.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
 lvc.iSubItem=ncol;
 lvc.pszText=LPTSTR(txt);
 lvc.cx=w;
 lvc.fmt=(right)?LVCFMT_RIGHT:LVCFMT_LEFT;
 ListView_InsertColumn(hlv,ncol,&lvc);
 ncol++;
}
LPARAM Twindow::lvGetItemParam(int id,int i)
{
 LVITEM lvi;
 lvi.mask=LVIF_PARAM;
 lvi.iItem=i;
 lvi.iSubItem=0;
 ListView_GetItem(GetDlgItem(m_hwnd,id),&lvi);
 return lvi.lParam;
}
void Twindow::lvSwapItems(int id,int i1,int i2)
{
 HWND hlv=GetDlgItem(m_hwnd,id);
 LVITEM lvi1;char_t text1[256];int check1;
 lvi1.mask=LVIF_PARAM|LVIF_STATE|LVIF_TEXT;
 lvi1.iItem=i1;lvi1.iSubItem=0;
 lvi1.pszText=text1;lvi1.cchTextMax=256;
 ListView_GetItem(hlv,&lvi1);
 check1=ListView_GetCheckState(hlv,i1);
 LVITEM lvi2;char_t text2[256];int check2;
 lvi2.mask=LVIF_PARAM|LVIF_STATE|LVIF_TEXT;
 lvi2.iItem=i2;lvi2.iSubItem=0;
 lvi2.pszText=text2;lvi2.cchTextMax=256;
 ListView_GetItem(hlv,&lvi2);
 check2=ListView_GetCheckState(hlv,i2);
 std::swap(lvi1.iItem,lvi2.iItem);
 ListView_SetItem(hlv,&lvi1);ListView_SetCheckState(hlv,i1,check2);
 ListView_SetItem(hlv,&lvi2);ListView_SetCheckState(hlv,i2,check1);
 lvSetSelItem(id,i2);
 ListView_EnsureVisible(hlv,i2,FALSE);
}

void Twindow::cbxTranslate(int id,const char_t **list)
{
 int sel=cbxGetCurSel(id);
 cbxClear(id);
 for (int i=0;list[i];i++)
  cbxAdd(id,_(id,list[i]));
 cbxSetCurSel(id,sel);
}

void Twindow::tbrSet(int id,int val,int lblId,const char_t *Icapt,float div)
{
 SendDlgItemMessage(m_hwnd,id,TBM_SETPOS,TRUE,val);
 if (lblId)
  {
   const char_t *capt=((Icapt && Icapt[0])?Icapt:_(lblId));
   char_t s[256];
   if (div==0)
    tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s %i"), capt, val);
   else
    tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s %g"), capt, float(val/div));
   setDlgItemText(m_hwnd,lblId,s);
  }
}
void Twindow::tbrSetRange(int id,int min,int max,int step,bool largethumb)
{
 SendDlgItemMessage(m_hwnd,id,TBM_SETRANGEMIN,TRUE,min);
 SendDlgItemMessage(m_hwnd,id,TBM_SETRANGEMAX,TRUE,max);
 SendDlgItemMessage(m_hwnd,id,TBM_SETLINESIZE,0,1);
 SendDlgItemMessage(m_hwnd,id,TBM_SETPAGESIZE,0,step?step:std::max(1,(max-min)/15));

 if (largethumb)
  {
   HWND hcbx=GetDlgItem(m_hwnd,id);
   SetWindowLong(hcbx,GWL_STYLE,GetWindowLong(hcbx,GWL_STYLE)|TBS_FIXEDLENGTH);
   RECT r;
   GetWindowRect(hcbx,&r);
   if (GetWindowStyle(hcbx)&TBS_VERT)
    SendMessage(hcbx,TBM_SETTHUMBLENGTH,r.right-r.left-2,0);
   else
    SendMessage(hcbx,TBM_SETTHUMBLENGTH,r.bottom-r.top-2,0);
  }
}

int Twindow::cfgGet(unsigned int i)
{
 return deci->getParam2(i);
}
void Twindow::cfgGet(unsigned int i,char_t *buf,size_t buflen)
{
 deci->getParamStr(i,buf,buflen);
}
const char_t* Twindow::cfgGetStr(unsigned int i)
{
 return deci->getParamStr2(i);
}
void Twindow::cfgSet(unsigned int i,int val)
{
 deci->putParam(i,val);
}
void Twindow::cfgSet(unsigned int i,const char_t *val)
{
 deci->putParamStr(i,val);
}

void Twindow::setText(int id,const char_t *fmt,...)
{
 char_t s[1024];
 va_list args;
 va_start(args,fmt);
 int len=_vsnprintf(s,1024,fmt,args);
 va_end(args);
 if (len<-1) return;
 s[1023]='\0';
 setDlgItemText(m_hwnd,id,s);
}

void Twindow::msg(const char_t *text,const char_t *capt)
{
 MessageBox(m_hwnd,text,capt,MB_ICONINFORMATION|MB_OK);
}
void Twindow::err(const char_t *text,const char_t *capt)
{
 MessageBox(m_hwnd,text,capt?capt:_(-dialogId,_l("Error")),MB_ICONERROR|MB_OK);
}

void Twindow::insertSeparator(HMENU hm,int &ord)
{
 MENUITEMINFO mii;
 mii.cbSize=sizeof(mii);
 mii.fMask=MIIM_TYPE;
 mii.fType=MFT_SEPARATOR;
 InsertMenuItem(hm,ord++,TRUE,&mii);
}

void Twindow::insertSubmenu(HMENU hm,int &ord,const char_t *caption,HMENU subMenu)
{
 MENUITEMINFO mii;
 mii.cbSize=sizeof(mii);
 mii.fMask=MIIM_TYPE|MIIM_SUBMENU;
 mii.fType=MFT_STRING;
 mii.hSubMenu=subMenu;
 mii.dwTypeData=LPTSTR(caption);
 mii.cch=(LONG)strlen(mii.dwTypeData);
 InsertMenuItem(hm,ord++,TRUE,&mii);
}

void Twindow::insertMenuItem(HMENU hm,int &ord,int id,const char_t *caption,bool checked,bool newcolumn)
{
 MENUITEMINFO mii;
 mii.cbSize=sizeof(mii);
 mii.fMask=MIIM_ID|MIIM_STATE|MIIM_TYPE;
 mii.fType=MFT_STRING|(newcolumn?MFT_MENUBREAK:0);
 mii.wID=id;
 mii.fState=(checked)?MFS_CHECKED:0;
 mii.dwTypeData=LPTSTR(caption);
 mii.cch=(LONG)strlen(mii.dwTypeData);
 InsertMenuItem(hm,ord++,TRUE,&mii);
}
void Twindow::enable(HMENU hm,int ord,bool is)
{
 MENUITEMINFO mii;
 mii.cbSize=sizeof(mii);
 mii.fMask=MIIM_STATE;
 GetMenuItemInfo(hm,ord,TRUE,&mii);
 if (is)
  mii.fState|=MFS_ENABLED;
 else
  mii.fState|=MFS_DISABLED;
 SetMenuItemInfo(hm,ord,TRUE,&mii);
}
int Twindow::selectFromMenu(const char_t **items,int id,bool translate,int columnHeight)
{
 HMENU hm=CreatePopupMenu();
 for (int ord=0,i=0;items[ord];i++)
  insertMenuItem(hm,ord,i+1,items[i],false,columnHeight && i && (i%columnHeight)==0);
 RECT r;
 if (id)
  GetWindowRect(GetDlgItem(m_hwnd,id),&r);
 else
  {
   POINT pt;
   GetCursorPos(&pt);
   r.left=pt.x;r.bottom=pt.y;
  }
 int cmd=TrackPopupMenu(translate?_(hm):hm,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,r.left-1,r.bottom,0,m_hwnd,0);
 DestroyMenu(hm);
 return cmd?cmd-1:-1;
}
int Twindow::selectFromMenu(const strings &items,int id,bool translate,int columnHeight)
{
 Tstrptrs ptritems;
 for (strings::const_iterator s=items.begin();s!=items.end();s++)
  ptritems.push_back(s->c_str());
 ptritems.push_back(NULL);
 return selectFromMenu(&ptritems[0],id,translate,columnHeight);
}

bool Twindow::changeDir(int idff,const char_t *caption)
{
 char_t dir[MAX_PATH];cfgGet(idff,dir,MAX_PATH);
 if (dlgGetDir(m_hwnd,dir,caption))
  {
   cfgSet(idff,dir);
   return true;
  }
 else
  return false;
}

bool Twindow::chooseColor(int idff)
{
 CHOOSECOLOR cc;
 memset(&cc,0,sizeof(cc));
 cc.lStructSize=sizeof(cc);
 cc.hwndOwner=m_hwnd;
 cc.rgbResult=cfgGet(idff);
 static const int idffDlgCustColors[16]={IDFF_dlgCustColor0,IDFF_dlgCustColor1,IDFF_dlgCustColor2,IDFF_dlgCustColor3,IDFF_dlgCustColor4,IDFF_dlgCustColor5,IDFF_dlgCustColor6,IDFF_dlgCustColor7,IDFF_dlgCustColor8,IDFF_dlgCustColor9,IDFF_dlgCustColor10,IDFF_dlgCustColor11,IDFF_dlgCustColor12,IDFF_dlgCustColor13,IDFF_dlgCustColor14,IDFF_dlgCustColor15};
 COLORREF custColors[16];
 for (int i=0;i<16;i++)
  custColors[i]=cfgGet(idffDlgCustColors[i]);
 cc.lpCustColors=custColors;
 cc.Flags=CC_ANYCOLOR|CC_FULLOPEN|CC_RGBINIT;
 if (ChooseColor(&cc))
  {
   cfgSet(idff,cc.rgbResult);
   for (int i=0;i<16;i++)
    cfgSet(idffDlgCustColors[i],custColors[i]);
   return true;
  }
 else
  return false;
}

void Twindow::setSize(HWND hwnd,int dx,int dy)
{
 SetWindowPos(hwnd,NULL,0,0,dx,dy,SWP_NOMOVE|SWP_NOZORDER);
}
void Twindow::setSize(int dx,int dy)
{
 setSize(m_hwnd,dx,dy);
}
void Twindow::setItemSize(int id,int dx,int dy)
{
 setSize(GetDlgItem(m_hwnd,id),dx,dy);
}
void Twindow::setSizeD(HWND hwnd,int dx,int dy)
{
 CRect r;
 GetWindowRect(hwnd,&r);
 SetWindowPos(hwnd,NULL,0,0,r.Width()+dx,r.Height()+dy,SWP_NOMOVE|SWP_NOZORDER);
}
void Twindow::setSizeD(int dx,int dy)
{
 setSizeD(m_hwnd,dx,dy);
}
void Twindow::setItemSizeD(int id,int dx,int dy)
{
 setSizeD(GetDlgItem(m_hwnd,id),dx,dy);
}

void Twindow::setPos(HWND hwnd,int x,int y)
{
 SetWindowPos(hwnd,NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
}
void Twindow::setPos(int x,int y)
{
 setPos(m_hwnd,x,y);
}
void Twindow::setItemPos(int id,int x,int y)
{
 setPos(GetDlgItem(m_hwnd,id),x,y);
}
void Twindow::setItemPosD(int id,int dx,int dy)
{
 HWND h=GetDlgItem(m_hwnd,id);
 CRect r;GetWindowRect(h,&r);
 CPoint pt=r.TopLeft();
 ScreenToClient(m_hwnd,&pt);
 SetWindowPos(h,NULL,pt.x+dx,pt.y+dy,0,0,SWP_NOSIZE|SWP_NOZORDER);
}

HWND Twindow::createHintWindow(HWND parent,int timePop,int timeInit,int timeReshow)
{
 HWND hhint=CreateWindowEx(WS_EX_TOPMOST,
                          TOOLTIPS_CLASS,
                          NULL,
                          WS_POPUP|TTS_NOPREFIX|TTS_ALWAYSTIP,
                          CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                          parent,NULL,hi,NULL
                         );
 SetWindowPos(hhint,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
 SendMessage(hhint,TTM_SETDELAYTIME,TTDT_AUTOPOP,MAKELONG(timePop,0));
 SendMessage(hhint,TTM_SETDELAYTIME,TTDT_INITIAL,MAKELONG(timeInit,0));
 SendMessage(hhint,TTM_SETDELAYTIME,TTDT_RESHOW,MAKELONG(timeReshow,0));
 SendMessage(hhint,TTM_SETMAXTIPWIDTH,0,470);
 return hhint;
}

TOOLINFO Twindow::addHint(int id,const char_t *text)
{
 if (!hhint) hhint=createHintWindow(m_hwnd,15000);
 TOOLINFO ti;
 ti.cbSize=sizeof(TOOLINFO);
 ti.uFlags=TTF_SUBCLASS|TTF_IDISHWND;
 ti.hwnd=m_hwnd;
 ti.uId=(LPARAM)GetDlgItem(m_hwnd,id);
 ti.lpszText=LPTSTR(_(-dialogId,text));
 SendMessage(hhint,TTM_ADDTOOL,0,(LPARAM)&ti);
 return ti;
}

void Twindow::enableHints(bool is)
{
 if (hhint)
  SendMessage(hhint,TTM_ACTIVATE,is?TRUE:FALSE,0);
}

CRect Twindow::getChildRect(int id) const
{
 CRect rs;GetWindowRect(GetDlgItem(m_hwnd,id),&rs);
 CPoint tl=rs.TopLeft();ScreenToClient(m_hwnd,&tl);
 CPoint br=rs.BottomRight();ScreenToClient(m_hwnd,&br);
 return CRect(tl,br);
}

Twindow::Tanchor::Tanchor(int Iid,int Ianchors,const Twindow &parent,const CRect &parentRect):id(Iid),anchors(Ianchors)
{
 CRect r=parent.getChildRect(id);
 if (anchors==(TanchorInfo::LEFT|TanchorInfo::TOP)) return;
 if (TanchorInfo::RIGHT&anchors)
   if (TanchorInfo::LEFT&anchors)
    rules.x=r.Width();
   else
    rules.x=r.left;
  else
   rules.x=r.left+r.Width()/2;

 if (TanchorInfo::BOTTOM&anchors)
  if (TanchorInfo::TOP&anchors)
   rules.y=r.Height();
  else
   rules.y=r.top;
 else
  rules.y=r.top+r.Height()/2;
 originalParentSize=CSize(parentRect.Width(),parentRect.Height());
}

void Twindow::Tanchor::resize(const Twindow &parent,const CRect &newrect)
{
 HWND h=GetDlgItem(parent.m_hwnd,id);
 if (!h) return;
 CRect r=parent.getChildRect(id);
 int newLeft=r.left;
 int newTop=r.top;
 int newWidth=r.Width();
 int newHeight=r.Height();
 CSize parentSize=newrect.Size();
 if (TanchorInfo::RIGHT&anchors)
  if (TanchorInfo::LEFT&anchors)
   newWidth=parentSize.cx-(originalParentSize.cx-rules.x);
  else
   newLeft=parentSize.cx-(originalParentSize.cx-rules.x);
 else if (!(TanchorInfo::LEFT&anchors))
   newLeft=rules.x*parentSize.cx/originalParentSize.cx-newWidth/2;

 if (TanchorInfo::BOTTOM&anchors)
  if (TanchorInfo::TOP&anchors)
   newHeight=parentSize.cy-(originalParentSize.cy-rules.y);
  else
   newTop=parentSize.cy-(originalParentSize.cy-rules.y);
 else if (!(TanchorInfo::TOP&anchors))
  newTop=rules.y*parentSize.cy/originalParentSize.cy-newHeight/2;
 MoveWindow(h,newLeft,newTop,newWidth,newHeight,TRUE);
 repaint(h);
}

void Twindow::Tanchors::add(const TanchorInfo *ainfo,const Twindow &parent)
{
 CRect r;GetWindowRect(parent.m_hwnd,&r);
 for (int i=0;ainfo[i].id;i++)
  push_back(Tanchor(ainfo[i].id,ainfo[i].anchor,parent,r));
}
void Twindow::Tanchors::resize(const Twindow &parent,const CRect &newrect)
{
 for (iterator a=begin();a!=end();a++)
  a->resize(parent,newrect);
}

template<class Tret,class TcreateFunc1,class TcreateFunc2> Tret Twindow::createDlg_Box(int dlgId,HWND hWndParent,LPARAM lparam,TcreateFunc1 createFunc1,TcreateFunc2 createFunc2)
{
 resized=false;

 if (bindsHtrack)
  for (int i=0;i<bindsHtrack[i].idc;i++)
   bindTrackbarsMap.insert(std::make_pair(bindsHtrack[i].idc,bindsHtrack+i));
 if (bindsVtrack)
  for (int i=0;i<bindsVtrack[i].idc;i++)
   bindTrackbarsMap.insert(std::make_pair(bindsVtrack[i].idc,bindsVtrack+i));
 if (bindsCheckbox)
  for (int i=0;i<bindsCheckbox[i].idc;i++)
   bindCheckboxesMap.insert(std::make_pair(bindsCheckbox[i].idc,bindsCheckbox+i));
 HINSTANCE hi=this->hi?this->hi:g_hInst;//HACK!!
 if (!tr || !tr->isCustomFont())
  {
   char_t lang[MAX_PATH],pth[MAX_PATH];
   getpth(pth);
   getUILanguage(pth,lang);
   Ttranslate tr1(g_hInst,pth); // Create temporary translator to calculate window size.
   tr1.init(lang,0);            // don't care translatorMode, because we just want size.
   DLGTEMPLATE *dlg=tr1.translateDialogTemplate(dlgId);
   Tret ret=createFunc2(hi,dlg,hWndParent,msgProc0,lparam);
   LocalFree(dlg);
   return ret;
  }
 else
  {
   DLGTEMPLATE *dlg=tr->translateDialogTemplate(dlgId);
   Tret ret=createFunc2(hi,dlg,hWndParent,msgProc0,lparam);
   LocalFree(dlg);
   return ret;
  }
}

void Twindow::resizeDialog()
{
 resizeDialog(m_hwnd);
}

void Twindow::resizeDialog(HWND hwnd)
{
 if(!hwnd || resized)
  return;

 int scale;

 if (tr)
  scale=Ttranslate::lastHorizontalScale=tr->font.horizontalScale;
 else
  scale=Ttranslate::lastHorizontalScale;

 if (scale == 100)
  return;

 resizeDialog(hwnd,false,scale);

 THWNDs wnds;

 getChildWindows(hwnd,wnds);

 for (THWNDs::const_iterator wnd=wnds.begin();wnd!=wnds.end();wnd++)
  if (*wnd)
   resizeDialog(*wnd,true,scale);

 resized=true;
}

void Twindow::resizeDialog(HWND hwnd, bool relative, int scale)
{
 RECT dlgRect;

 if(!GetWindowRect(hwnd,&dlgRect))
  return;

 HWND parentHwnd=GetParent(hwnd);

 if (relative)
  {
   ScreenToClient(parentHwnd,(LPPOINT)&dlgRect.left);
   ScreenToClient(parentHwnd,(LPPOINT)&dlgRect.right);
  }

 int dlgWidth=(dlgRect.right-dlgRect.left);
 int dlgHeight=(dlgRect.bottom-dlgRect.top);

 if (relative)
  dlgRect.left=dlgRect.left*scale/100;
 else
  dlgRect.left-=dlgWidth*(scale-100)/200;

 int id=GetWindowID(hwnd);

 char_t className[32];

 GetClassName(hwnd,className,32);

 if (stricmp(className,_l("EDIT")) == 0 &&
     parentHwnd)
  {
   // Don't resize edit controls that have a combobox as their parent -
   // those have already been resized when the combobox was resized

   char_t parentClassName[32];

   GetClassName(parentHwnd,parentClassName,32);

   if (stricmp(parentClassName,_l("COMBOBOX")) == 0)
    return;
  }

 bool isStatic=(stricmp(className,_l("STATIC")) == 0);

 if (isStatic && id == IDC_BMP_LEVELS_CURVES)
  // Curves diagram looks bad when stretched
  ;
 else if (isStatic && 
          (id == IDC_STATIC &&
           SendMessage(hwnd,STM_GETIMAGE,IMAGE_BITMAP,0)))
  // Center static bitmaps without scaling
  dlgRect.left+=dlgWidth*(scale-100)/200;
 else
  dlgWidth=dlgWidth*scale/100;

 SetWindowPos(
  hwnd,
  0,
  dlgRect.left,
  dlgRect.top,
  dlgWidth,
  dlgHeight,
  SWP_NOZORDER);
}

HWND Twindow::createDialog(int dlgId,HWND hWndParent)
{
 return createDialog(dlgId,hWndParent,(LPARAM)new TwindowWidget(NULL,this));
}

HWND Twindow::createDialog(int dlgId,HWND hWndParent,LPARAM lparam)
{
 HWND hwnd=createDlg_Box<HWND>(dlgId,hWndParent,lparam,CreateDialogParam,CreateDialogIndirectParam);

 resizeDialog(hwnd);

 return hwnd;
}

INT_PTR Twindow::dialogBox(int dlgId,HWND hWndParent)
{
 return dialogBox(dlgId,hWndParent,(LPARAM)new TwindowWidget(NULL,this));
}

INT_PTR Twindow::dialogBox(int dlgId,HWND hWndParent,LPARAM lparam)
{
 INT_PTR ptr=createDlg_Box<INT_PTR>(dlgId,hWndParent,lparam,DialogBoxParam,DialogBoxIndirectParam);

 resizeDialog();

 return ptr;
}

bool Twindow::onTrack(TbindTrackbars bindsTrack,LPARAM lParam)
{
 if (bindsTrack)
  {
   int id=getId(HWND(lParam));
   for (int i=0;bindsTrack[i].idc;i++)
    if (bindsTrack[i].idc==id)
     {
      if (bindsTrack[i].idff<0)
       cfgSet(-bindsTrack[i].idff,-tbrGet(bindsTrack[i].idc));
      else
       cfgSet(bindsTrack[i].idff,tbrGet(bindsTrack[i].idc));
      if (bindsTrack[i].onClick)
       (this->*bindsTrack[i].onClick)();
      return true;
     }
  }
 return false;
}

template<class TbindEdit> bool Twindow::onCtlColorEdit(TbindEdit *bindsEdit,LPARAM lParam,WPARAM wParam)
{
 HWND hwnd=HWND(lParam);
 int id=getId(hwnd);
 for (int i=0;bindsEdit[i].idc;i++)
  if (bindsEdit[i].idc==id)
   if (!eval(hwnd,bindsEdit[i].min,bindsEdit[i].max))
    {
     HDC dc=HDC(wParam);
     SetBkColor(dc,RGB(255,0,0));
     return true;
    }
 return false;
}
HBRUSH Twindow::getRed(void)
{
 if (!red) red=CreateSolidBrush(RGB(255,0,0));
 return red;
}

INT_PTR Twindow::msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_INITDIALOG:
    resizeDialog();
    init();
    //subClass();
    break;
   case WM_DESTROY:
    {
     THWNDs wnds;getChildWindows(m_hwnd,wnds);
     for (THWNDs::const_iterator wnd=wnds.begin();wnd!=wnds.end();wnd++)
      {
       ::Twidget *item=Twidget::getDlgItem(*wnd);
       if (item)
        {
         item->restore();
         delete item;
        }
      }
     ::Twidget *item=Twidget::getDlgItem(m_hwnd);
     if (item)
      {
       item->restore();
       delete item;
      }
     if (red) DeleteObject(red);red=NULL;
     break;
    }
   case WM_HSCROLL:
    if (onTrack(bindsHtrack,lParam))
     return TRUE;
    break;
   case WM_VSCROLL:
    if (onTrack(bindsVtrack,lParam))
     return TRUE;
    break;
   case WM_COMMAND:
    {
     if (bindsCheckbox)
      for (int i=0;bindsCheckbox[i].idc;i++)
       if (bindsCheckbox[i].idc==LOWORD(wParam))
        {
         if (bindsCheckbox[i].idff<0)
          cfgSet(-bindsCheckbox[i].idff,!getCheck(LOWORD(wParam)));
         else
          cfgSet(bindsCheckbox[i].idff,getCheck(LOWORD(wParam)));
         if (bindsCheckbox[i].onClick)
          (this->*bindsCheckbox[i].onClick)();
         return TRUE;
        }
     if (bindsRadiobutton)
      for (int i=0;bindsRadiobutton[i].idc;i++)
       if (bindsRadiobutton[i].idc==LOWORD(wParam) && getCheck(LOWORD(bindsRadiobutton[i].idc)))
        {
         cfgSet(bindsRadiobutton[i].idff,bindsRadiobutton[i].val);
         if (bindsRadiobutton[i].onClick)
          (this->*bindsRadiobutton[i].onClick)();
         return TRUE;
        }
     if (bindsButton)
      if (HIWORD(wParam)==BN_CLICKED) // works also for static
       for (int i=0;bindsButton[i].idc;i++)
        if (bindsButton[i].idc==LOWORD(wParam) && bindsButton[i].onClick)
         {
          (this->*bindsButton[i].onClick)();
          return TRUE;
         }
     if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
      {
       if (LOWORD(wParam)==IDC_ED_HELP && tr->translateMode)
        onEditChange();
       if (bindsEditInt)
        for (int i=0;bindsEditInt[i].idc;i++)
         if (bindsEditInt[i].idc==LOWORD(wParam))
          {
           HWND hed=GetDlgItem(m_hwnd,bindsEditInt[i].idc);
           if (hed!=GetFocus()) return FALSE;
           repaint(hed);
           eval(hed,bindsEditInt[i].min,bindsEditInt[i].max,bindsEditInt[i].idff);
           if (bindsEditInt[i].onClick)
            (this->*bindsEditInt[i].onClick)();
           return TRUE;
          }
       if (bindsEditReal)
        for (int i=0;bindsEditReal[i].idc;i++)
         if (bindsEditReal[i].idc==LOWORD(wParam))
          {
           HWND hed=GetDlgItem(m_hwnd,bindsEditReal[i].idc);
           if (hed!=GetFocus()) return FALSE;
           repaint(hed);
           eval(hed,bindsEditReal[i].min,bindsEditReal[i].max,bindsEditReal[i].idff,bindsEditReal[i].mult);
           if (bindsEditReal[i].onClick)
            (this->*bindsEditReal[i].onClick)();
           return TRUE;
          }
      }
     else if (HIWORD(wParam)==CBN_SELCHANGE)
      {
       if (bindsCombobox)
        for (int i=0;bindsCombobox[i].idc;i++)
         if (bindsCombobox[i].idc==LOWORD(wParam))
          {
           switch (bindsCombobox[i].mode)
            {
             case BINDCBX_DATA:cfgSet(bindsCombobox[i].idff,(int)cbxGetCurItemData(bindsCombobox[i].idc));break;
             case BINDCBX_SEL:cfgSet(bindsCombobox[i].idff,cbxGetCurSel(bindsCombobox[i].idc));break;
             case BINDCBX_TEXT:cfgSet(bindsCombobox[i].idff,cbxGetCurText(bindsCombobox[i].idc));break;
             case BINDCBX_DATATEXT:cfgSet(bindsCombobox[i].idff,(const char_t*)cbxGetCurItemData(bindsCombobox[i].idc));break;
            }
           if (bindsCombobox[i].onClick)
            (this->*bindsCombobox[i].onClick)();
           return TRUE;
          }
      }
     break;
    }
   case WM_CTLCOLOREDIT:
    if (bindsEditInt)
     if (onCtlColorEdit(bindsEditInt,lParam,wParam))
      return INT_PTR(getRed());
    if (bindsEditReal)
     if (onCtlColorEdit(bindsEditReal,lParam,wParam))
      return INT_PTR(getRed());
    break;
  }
 return defMsgProc(uMsg,wParam,lParam);
}

INT_PTR CALLBACK Twindow::msgProc0(HWND m_hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_MEASUREITEM:
    if (wParam==IDC_CBX_FONT_CHARSET || wParam==IDC_CBX_ENCODER)
     {
      MEASUREITEMSTRUCT *mis=(MEASUREITEMSTRUCT*)lParam;
      UINT h=(UINT)SendMessage(GetDlgItem(m_hwnd,wParam==IDC_CBX_FONT_CHARSET?IDC_CBX_FONT_NAME:IDC_CBX_FOURCC),CB_GETITEMHEIGHT,WPARAM((int)mis->itemID==-1?-1:0),0);
      mis->itemHeight=h;
      SetWindowLongPtr(m_hwnd,DWLP_MSGRESULT,TRUE);
      return TRUE;
     }
    break;
   case WM_INITDIALOG:
    {
     TwindowWidget *self=(TwindowWidget*)lParam;
     if (self) self->window->setHWND(m_hwnd,0);
     break;
    }
   case WM_NCDESTROY:
    return FALSE;
   case WM_NOTIFYFORMAT:
    {
     #ifdef UNICODE
      SetWindowLongPtr(m_hwnd,DWLP_MSGRESULT,NFR_UNICODE);
     #else
      SetWindowLongPtr(m_hwnd,DWLP_MSGRESULT,NFR_ANSI);
     #endif
     break;
    }
  }
 TwindowWidget *self=(TwindowWidget*)Twidget::getDlgItem(m_hwnd);
 return self?self->window->msgProc(uMsg,wParam,lParam):1;
}

LRESULT Twindow::TwidgetSubclassTbr::onContextMenu(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 static const char_t *menu[]={_l("Reset"),_l("Edit"),NULL};
 switch (self->selectFromMenu(menu,0))
  {
   case 0:
    if (!onReset())
     return 0;
    else
     break;
   case 1:
    {
     if (!onEdit())
      return 0;
     else
      break;
    }
   default:
    return 0;
  }
 if (bind && bind->onClick)
  (self->*bind->onClick)();
 else
  self->cfg2dlg();
 return 0;
}
bool Twindow::TwidgetSubclassTbr::onReset(void)
{
 return self->deci->resetParam(self->getTbrIdff(id,bind))==S_OK;
}
void Twindow::TwidgetSubclassTbr::getEditMinMaxVal(const TffdshowParamInfo &info,int *min,int *max,int *val)
{
 *min=info.min;
 *max=info.max;
 *val=self->cfgGet(info.id);
}
void Twindow::TwidgetSubclassTbr::storeEditValue(const TffdshowParamInfo &info,int min,int max,const char_t *valS)
{
 self->eval(valS,min,max,info.id,1,true);
}
bool Twindow::TwidgetSubclassTbr::onEdit(void)
{
 int idff=self->getTbrIdff(id,bind);
 TffdshowParamInfo info;memset(&info,0,sizeof(info));
 if (self->deci->getParamInfo(idff,&info)!=S_OK)
  return false;
 int min,max,val;getEditMinMaxVal(info,&min,&max,&val);
 char_t valS[20];_itoa(val,valS,10);
 char_t query[100];tsprintf(query,self->_(0,_l("Enter value in range from %i to %i")),min,max);
 if (!self->inputString(self->_(0,_l("Edit trackbar value")),query,valS,20))
  return false;
 storeEditValue(info,min,max,valS);
 return true;
}

LRESULT Twindow::TwidgetSubclassChb::onContextMenu(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (self->tr->translateMode) return TwidgetSubclass::onContextMenu(hwnd,uMsg,wParam,lParam);
 static const char_t *menu[]={_l("Reset"),NULL};
 if (self->selectFromMenu(menu,0)==0)
  if (self->deci->resetParam(ff_abs(bind->idff))!=S_OK)
   return 0;
  else
   if (bind->onClick)
    (self->*bind->onClick)();
   else
    self->cfg2dlg();
 return 0;
}

//================================= TdlgWindow =================================
TdlgWindow::TdlgWindow(int IdialogId,IffdshowBase *Ideci):Twindow(Ideci)
{
 dialogId=IdialogId;
}

//=============================== TdlgPropsEdit ================================
TdlgPropsEdit::TdlgPropsEdit(TstringProps &Iprops,IffdshowBase *deci):TdlgWindow(IDD_PROPSEDIT,deci),props(Iprops)
{
 italicFont=NULL;
}
TdlgPropsEdit::~TdlgPropsEdit()
{
 if (italicFont) DeleteObject(italicFont);
}
bool TdlgPropsEdit::execute(HWND wndParent)
{
 return dialogBox(dialogId,wndParent)==IDOK;
}
INT_PTR TdlgPropsEdit::msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_INITDIALOG:
    {
     hlv=GetDlgItem(m_hwnd,IDC_LV_PROPS);
     int ncol=0;
     ListView_AddCol(hlv,ncol,300,_l("Original:/Translation"),false);
     ListView_SetItemCount(hlv,props.size()*2);
     ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
     return TRUE;
    }
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDOK:
      case IDCANCEL:
       if (HWND(lParam)==GetDlgItem(m_hwnd,IDOK) || HWND(lParam)==GetDlgItem(m_hwnd,IDCANCEL))
        {
         EndDialog(m_hwnd,LOWORD(wParam));
         return TRUE;
        }
       break;
     };
    break;
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (nmhdr->idFrom==IDC_LV_PROPS)
      switch (nmhdr->code)
       {
        case LVN_GETDISPINFO:
         {
          NMLVDISPINFO *nmdi=(NMLVDISPINFO*)lParam;
          int i=nmdi->item.iItem;
          if (i==-1) break;
          if (nmdi->item.mask&LVIF_TEXT)
           switch (nmdi->item.iSubItem)
            {
             case 0:
              {
               //strcpy(nmdi->item.pszText,((i&1)==0)?props[i/2].first.c_str():props[i/2].second.c_str());
               nmdi->item.pszText=(LPTSTR)((i&1)==0?props[i/2].first.c_str():props[i/2].second.c_str());
               break;
              }
            }
          return TRUE;
         }
        case NM_CUSTOMDRAW:
         {
          NMLVCUSTOMDRAW *lvcd=LPNMLVCUSTOMDRAW(lParam);
          if (lvcd->nmcd.dwDrawStage==CDDS_PREPAINT)
           {
            setDlgResult(CDRF_NOTIFYITEMDRAW);
            return TRUE;
           }
          if (lvcd->nmcd.dwDrawStage==CDDS_ITEMPREPAINT)
           {
            if (!italicFont)
             {
              LOGFONT oldFont;
              HFONT hf=(HFONT)GetCurrentObject(lvcd->nmcd.hdc,OBJ_FONT);
              GetObject(hf,sizeof(LOGFONT),&oldFont);
              oldFont.lfItalic=TRUE;
              italicFont=CreateFontIndirect(&oldFont);
             }
            if ((lvcd->nmcd.dwItemSpec&1)==0)
             SelectObject(lvcd->nmcd.hdc,italicFont);
            setDlgResult(CDRF_NEWFONT);
            return TRUE;
           }
          return TRUE;
         }
        case LVN_BEGINLABELEDIT:
         {
          NMLVDISPINFO *nmdi=(NMLVDISPINFO*)lParam;
          if ((nmdi->item.iItem&1)==0)
           {
            setDlgResult(TRUE);
            return TRUE;
           }
          break;
         }
        case LVN_ENDLABELEDIT:
         {
          NMLVDISPINFO *nmdi=(NMLVDISPINFO*)lParam;
          if (nmdi->item.pszText)
           {
            props[nmdi->item.iItem/2].second=nmdi->item.pszText;
            setDlgResult(TRUE);
            return TRUE;
           }
          break;
         }
        case LVN_KEYDOWN:
         {
          NMLVKEYDOWN *nmkd=(NMLVKEYDOWN*)lParam;
          if (nmkd->wVKey==VK_F2)
           {
            ListView_EditLabel(hlv,ListView_GetNextItem(hlv,-1,LVNI_SELECTED));
            return TRUE;
           }
          break;
         }
       }
     break;
    }
  }
 return Twindow::msgProc(uMsg,wParam,lParam);
}

//============================== TdlgInputString ===============================
TdlgInputString::TdlgInputString(const char_t *Icapt,const char_t *Iquery,char_t *Istr,size_t Ilen,IffdshowBase *deci):
 TdlgWindow(IDD_INPUTSTRING,deci),
 capt(Icapt),
 query(Iquery),
 str(Istr),
 len(Ilen)
{
}
bool TdlgInputString::execute(HWND wndParent)
{
 return dialogBox(dialogId,wndParent)==IDOK;
}
INT_PTR TdlgInputString::msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_INITDIALOG:
    setWindowText(m_hwnd,capt);
    setDlgItemText(m_hwnd,IDC_LBL_INPUTSTRING,query);
    setDlgItemText(m_hwnd,IDC_ED_INPUTSTRING,str);
    edLimitText(IDC_ED_INPUTSTRING,len-1);
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDOK:
       GetDlgItemText(m_hwnd,IDC_ED_INPUTSTRING,str,int(len));
      case IDCANCEL:
       EndDialog(m_hwnd,LOWORD(wParam));
       return TRUE;
     }
  };
 return TdlgWindow::msgProc(uMsg,wParam,lParam);
}
