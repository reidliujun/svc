/*
 * Copyright (c) 2002-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "TpresetAutoload.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "ffdshow_constants.h"
#include "Ttranslate.h"
#include "TflatButtons.h"

//========================== TpresetAutoloadDlgBase::TitemsWindow ===========================
TpresetAutoloadDlgBase::TitemsWindow::TitemsWindow(IffdshowBase *Ideci,HWND parent,Titems &Iitems):TdlgWindow(IDD_PRESET_AUTOLOAD_ITEMS,Ideci),items(Iitems),deciD(Ideci)
{
 LOGFONT lf={18L,0L,0L,0L,FW_NORMAL,0,0,0,SYMBOL_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE+DEFAULT_PITCH,_l("Webdings")};
 arrowsFont=CreateFontIndirect(&lf);
 createDialog(dialogId,parent);
}
TpresetAutoloadDlgBase::TitemsWindow::~TitemsWindow()
{
 DeleteObject(arrowsFont);
 DestroyWindow(m_hwnd);
}
void TpresetAutoloadDlgBase::TitemsWindow::showItems(void)
{
 THWNDs hwnds;getChildWindows(m_hwnd,hwnds);
 for (THWNDs::const_iterator h=hwnds.begin();h!=hwnds.end();h++)
  DestroyWindow(*h);
 int y=0;
 HFONT fnt=(HFONT)SendMessage(m_hwnd,WM_GETFONT,0,0);
 CRect pr;GetWindowRect(m_hwnd,&pr);int prdlgdx=170; //TODO: compute
 int id=0;
 int sbrWidth=GetSystemMetrics(SM_CXVSCROLL)+4;
 for (Titems::const_iterator i=items.begin();i!=items.end();i++,id++)
  {
   CRect r(CPoint(0,y),CSize(prdlgdx,10));MapDialogRect(m_hwnd,&r);
   HWND hchb=CreateWindow(_l("BUTTON"),_(-IDD_PRESET_AUTOLOAD_ITEMS,i->name),WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX|WS_TABSTOP,r.left,r.top,r.Width()-sbrWidth,r.Height(),m_hwnd,HMENU(1000+10*id+1),hi,0);
   SendMessage(hchb,WM_SETFONT,WPARAM(fnt),TRUE);
   setCheck(1000+10*id+1,i->is);
   y+=11;
   if (i->isVal)
    {
     static const int dxbt1=10;
     int dxbt=(i->isList?dxbt1:0)+(i->isHelp?dxbt1:0);
     r=CRect(CPoint(0,y),CSize(prdlgdx-dxbt,12));MapDialogRect(m_hwnd,&r);
     HWND hed=CreateWindowEx(WS_EX_CLIENTEDGE,_l("EDIT"),NULL,WS_CHILD|WS_VISIBLE|ES_LEFT|ES_AUTOHSCROLL|WS_TABSTOP,r.left,r.top,r.Width()-sbrWidth,r.Height(),m_hwnd,HMENU(1000+10*id+2),hi,0);
     SendMessage(hed,WM_SETFONT,WPARAM(fnt),TRUE);
     SendMessage(hed,EM_SETLIMITTEXT,255,0);
     setWindowText(hed,i->val);
     if (i->hint && i->hint[0])
      addHint(1000+10*id+2,i->hint);
     int btx=prdlgdx-dxbt;
     if (i->isList)
      {
       r=CRect(CPoint(btx,y),CSize(10,12));MapDialogRect(m_hwnd,&r);btx+=dxbt1;
       HWND hbt=CreateWindow(_l("BUTTON"),_l("6"),WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON|WS_TABSTOP,r.left-sbrWidth,r.top,r.Width(),r.Height(),m_hwnd,HMENU(1000+10*id+3),hi,0);
       SendMessage(hbt,WM_SETFONT,WPARAM(arrowsFont),TRUE);
      }
     if (i->isHelp)
      {
       r=CRect(CPoint(btx,y),CSize(10,12));MapDialogRect(m_hwnd,&r);
       HWND hbt=CreateWindow(_l("BUTTON"),_l("?"),WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON|WS_TABSTOP,r.left-sbrWidth,r.top,r.Width(),r.Height(),m_hwnd,HMENU(1000+10*id+4),hi,0);
       SendMessage(hbt,WM_SETFONT,WPARAM(fnt),TRUE);
      }
     y+=14;
    }
  }
 SCROLLINFO si;
 si.cbSize=sizeof(si);
 si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
 si.nMin=0;
 CRect r(0,0,0,y);MapDialogRect(m_hwnd,&r);
 si.nMax=r.Height();
 si.nPage=pr.Height();
 si.nPos=0;
 SetScrollInfo(m_hwnd,SB_VERT,&si,TRUE);
}

INT_PTR TpresetAutoloadDlgBase::TitemsWindow::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 if (uMsg==WM_COMMAND)
  {
   int id=LOWORD(wParam)-1000;
   if (id>0 && id/10<(int)items.size())
    {
     Titem &item=items[id/10];
     if ((id%10)==1)
      {
       item.is=1-item.is;
       item.wasChange=true;
      }
     else if ((id%10)==2)
      {
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         GetDlgItemText(m_hwnd,LOWORD(wParam),item.val,256);
         item.wasChange=true;
        }
      }
     else if ((id%10)==3)
      {
       Tstrptrs mitems;
       const char_t *mitem;
       for (unsigned int li=0;(mitem=deciD->getPresetAutoloadItemList(id/10,li))!=NULL;li++)
        mitems.push_back(mitem);
       mitems.push_back(NULL);
       int idx=selectFromMenu(&mitems[0],LOWORD(wParam),false,20);
       if (idx>=0)
        {
         if (item.val[0]!='\0')
          strncat_s(item.val, countof(item.val), _l(";"), _TRUNCATE);
         strncat_s(item.val, countof(item.val), mitems[idx], _TRUNCATE);
         HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam)-1);
         SetWindowText(hed,item.val);
         size_t len=strlen(item.val);
         SendMessage(hed,EM_SETSEL,len,len);
        }
      }
     else if ((id%10)==4)
      {
       const char_t *help;
       if (deciD->getPresetAutoloadItemHelp(id/10,&help)==S_OK)
        msg(_(-IDD_PRESET_AUTOLOAD_ITEMS,help),_(-IDD_PRESET_AUTOLOAD_ITEMS,_l("Preset autoload condition help")));
      }
    }
  }
 else if (uMsg==WM_VSCROLL)
  {
   SCROLLINFO si;
   si.cbSize = sizeof (si);
   si.fMask  = SIF_ALL;
   GetScrollInfo (m_hwnd, SB_VERT, &si);
   int oldpos = si.nPos;
   switch (LOWORD (wParam))
    {
     case SB_LINELEFT:si.nPos--;break;
     case SB_LINERIGHT:si.nPos++;break;
     case SB_PAGELEFT:si.nPos-=si.nPage;break;
     case SB_PAGERIGHT:si.nPos+=si.nPage;break;
     case SB_THUMBTRACK:si.nPos=si.nTrackPos;break;
     default:break;
    }
   si.fMask = SIF_POS;
   SetScrollInfo(m_hwnd,SB_VERT,&si,TRUE);
   GetScrollInfo(m_hwnd,SB_VERT,&si);
   if (si.nPos!=oldpos) ScrollWindow(m_hwnd,0,oldpos-si.nPos,NULL,NULL);
  }
 return TdlgWindow::msgProc(uMsg,wParam,lParam);
}

//================================= TpresetAutoloadDlgBase ==================================
TpresetAutoloadDlgBase::TpresetAutoloadDlgBase(IffdshowBase *Ideci,HWND Iparent,int x,int y):TdlgWindow(IDD_PRESET_AUTOLOAD,Ideci),parent(Iparent),deciD(Ideci)
{
 this->x=x;this->y=y;
 itemsw=NULL;
 static const TbindRadiobutton<TpresetAutoloadDlgBase> rbt[]=
  {
   IDC_RBT_PRESETAUTOLOAD_OR,  IDFF_presetAutoloadLogic, 0, NULL,
   IDC_RBT_PRESETAUTOLOAD_AND, IDFF_presetAutoloadLogic, 1, NULL,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
}
void TpresetAutoloadDlgBase::init(void)
{
 translate();
 const char_t *capt=tr->translate(m_hwnd,dialogId,0,NULL);
 if (capt && capt[0]) setWindowText(m_hwnd,capt);
 char_t presetname[MAX_PATH];
 deciD->getActivePresetName(presetname, countof(presetname));
 setText(IDC_LBL_AUTOLOAD,_(IDC_LBL_AUTOLOAD,_l("Preset '%s' will be autoloaded ...")),presetname);

 itemsw=new TitemsWindow(deci,m_hwnd,items);
 CRect ir=getChildRect(IDC_PLC_AUTOLOAD_ITEMS);
 SetWindowPos(itemsw->m_hwnd,GetDlgItem(m_hwnd,IDC_PLC_AUTOLOAD_ITEMS),ir.left,ir.top,ir.Width(),ir.Height(),0);
 unsigned int cnt=deciD->getPresetAutoloadItemsCount2();items.resize(cnt);
 for (unsigned int i=0;i<cnt;i++)
  deciD->getPresetAutoloadItemInfo(i,&items[i].name,&items[i].hint,&items[i].allowWildcard,&items[i].is,&items[i].isVal,items[i].val,sizeof(items[i].val),&items[i].isList,&items[i].isHelp);
 itemsw->showItems();
 setCheck(IDC_RBT_PRESETAUTOLOAD_OR  ,deci->getParam2(IDFF_presetAutoloadLogic) == 0);
 setCheck(IDC_RBT_PRESETAUTOLOAD_AND ,deci->getParam2(IDFF_presetAutoloadLogic) != 0);
}
bool TpresetAutoloadDlgBase::show(void)
{
 return !!dialogBox(dialogId,parent);
}

INT_PTR TpresetAutoloadDlgBase::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_INITDIALOG:
    init();
    red=CreateSolidBrush(RGB(255,0,0));
    return TRUE;
   case WM_DESTROY:
    DeleteObject(red);
    if (itemsw) delete itemsw;itemsw=NULL;
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDCLOSE:
      case IDCANCEL:
       {
        bool wasChange=false;
        for (unsigned int i=0;i<items.size();i++)
         {
          deciD->setPresetAutoloadItem(i,items[i].is,items[i].val);
          wasChange|=items[i].wasChange;
         }
        EndDialog(m_hwnd,wasChange);
        return TRUE;
       }
     }
    break;
/*
   case WM_NCACTIVATE:
    if (wParam==FALSE)
     {
      EndDialog(m_hwnd,0);
      return TRUE;
     }
    break;*/
  }
 return TdlgWindow::msgProc(uMsg,wParam,lParam);
}

//================================= TpresetAutoloadDlgVideo =================================
TpresetAutoloadDlgVideo::TpresetAutoloadDlgVideo(IffdshowBase *Ideci,HWND parent,int x,int y):TpresetAutoloadDlgBase(Ideci,parent,x,y)
{
 static const TbindCheckbox<TpresetAutoloadDlgVideo> chb[]=
  {
   IDC_CHB_PRESETAUTOLOAD_SIZE,IDFF_presetAutoloadSize,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditInt<TpresetAutoloadDlgVideo> edInt[]=
  {
   IDC_ED_PRESETAUTOLOAD_SIZE_XMIN,1,16384,IDFF_presetAutoloadSizeXmin,NULL,
   IDC_ED_PRESETAUTOLOAD_SIZE_XMAX,1,16384,IDFF_presetAutoloadSizeXmax,NULL,
   IDC_ED_PRESETAUTOLOAD_SIZE_YMIN,1,16384,IDFF_presetAutoloadSizeYmin,NULL,
   IDC_ED_PRESETAUTOLOAD_SIZE_YMAX,1,16384,IDFF_presetAutoloadSizeYmax,NULL,
   0
  };
 bindEditInts(edInt);
}
void TpresetAutoloadDlgVideo::init(void)
{
 TpresetAutoloadDlgBase::init();
 setCheck(IDC_CHB_PRESETAUTOLOAD_SIZE    ,deci->getParam2(IDFF_presetAutoloadSize));
 SetDlgItemInt(m_hwnd,IDC_ED_PRESETAUTOLOAD_SIZE_XMIN,deci->getParam2(IDFF_presetAutoloadSizeXmin),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_PRESETAUTOLOAD_SIZE_XMAX,deci->getParam2(IDFF_presetAutoloadSizeXmax),FALSE);
 cond2dlg();
 SetDlgItemInt(m_hwnd,IDC_ED_PRESETAUTOLOAD_SIZE_YMIN,deci->getParam2(IDFF_presetAutoloadSizeYmin),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_PRESETAUTOLOAD_SIZE_YMAX,deci->getParam2(IDFF_presetAutoloadSizeYmax),FALSE);
}
void TpresetAutoloadDlgVideo::cond2dlg(void)
{
 setDlgItemText(m_hwnd,IDC_BT_PRESETAUTOLOAD_SIZE_COMP,_(IDC_BT_PRESETAUTOLOAD_SIZE_COMP,cfgGet(IDFF_presetAutoloadSizeCond)==1?_l("or"):_l("and")));
}
Twidget* TpresetAutoloadDlgVideo::createDlgItem(int id,HWND h)
{
 if (id==IDC_BT_PRESETAUTOLOAD_SIZE_COMP)
  return new TflatButton(h,this);
 else
  return TpresetAutoloadDlgBase::createDlgItem(id,h);
}
INT_PTR TpresetAutoloadDlgVideo::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_BT_PRESETAUTOLOAD_SIZE_COMP:
       cfgSet(IDFF_presetAutoloadSizeCond,1-cfgGet(IDFF_presetAutoloadSizeCond));
       cond2dlg();
       return TRUE;
     }
    break;
   case WM_DRAWITEM:
    switch (wParam)
     {
      case IDC_BT_PRESETAUTOLOAD_SIZE_COMP:
       {
        DRAWITEMSTRUCT *dis=(LPDRAWITEMSTRUCT)lParam;
        ((TflatButton*)Twidget::getDlgItem(dis->hwndItem))->paint(dis);
        return TRUE;
       }
     }
    break;
  }
 return TpresetAutoloadDlgBase::msgProc(uMsg,wParam,lParam);
}

//================================= TpresetAutoloadDlgAudio =================================
TpresetAutoloadDlgAudio::TpresetAutoloadDlgAudio(IffdshowBase *Ideci,HWND parent,int x,int y):TpresetAutoloadDlgBase(Ideci,parent,x,y)
{
}
void TpresetAutoloadDlgAudio::init(void)
{
 TpresetAutoloadDlgBase::init();
 static const int idSize[]={IDC_CHB_PRESETAUTOLOAD_SIZE,IDC_ED_PRESETAUTOLOAD_SIZE_XMIN,IDC_ED_PRESETAUTOLOAD_SIZE_XMAX,IDC_BT_PRESETAUTOLOAD_SIZE_COMP,IDC_ED_PRESETAUTOLOAD_SIZE_YMIN,IDC_ED_PRESETAUTOLOAD_SIZE_YMAX,IDC_LBL_PRESETAUTOLOAD_WIDTH,IDC_LBL_PRESETAUTOLOAD_HEIGHT,0};
 enable(0,idSize);
}
