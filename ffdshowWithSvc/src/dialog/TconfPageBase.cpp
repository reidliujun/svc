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
#include "Ttranslate.h"
#include "TconfPageBase.h"
#include "TffdshowPageBase.h"
#include "Tstream.h"

TconfPageBase::TconfPageBase(TffdshowPageBase *Iparent,int IfilterPageId):Twindow(Iparent->deci)
{
 m_hwnd=NULL;
 parent=Iparent;
 hwndParent=parent->m_hwnd;
 helpStr=NULL;
 dialogId=0;
 inPreset=0;
 filterID=0;filterPageID=IfilterPageId;
 helpURL=NULL;
 static const int props[]={0};
 propsIDs=props;
}
TconfPageBase::~TconfPageBase()
{
 if (helpStr) free(helpStr);
}

const char_t* TconfPageBase::dialogName(void)
{
 const char_t *name=tr->translate(dialogId);
 ff_strncpy(dialogNameRes, name, countof(dialogNameRes));
 return dialogNameRes;
}

void TconfPageBase::createWindow(void)
{
 m_hwnd=createDialog(dialogId,hwndParent);
 assert(m_hwnd);
 ShowWindow(m_hwnd,SW_HIDE);
 setWindowText(m_hwnd,dialogName());
 translate();
 if (tr->translateMode && dialogId!=IDD_ABOUT)
  {
   HWND hed=GetDlgItem(m_hwnd,IDC_ED_HELP);
   if (hed)
    {
     SendMessage(hed,EM_SETREADONLY,FALSE,0);
     SetWindowLong(hed,GWL_STYLE,GetWindowLong(hed,GWL_STYLE)|ES_WANTRETURN);
    }
  }
}
void TconfPageBase::destroyWindow(void)
{
 DestroyWindow(m_hwnd);
 m_hwnd=NULL;
 wndEnabled=true;
}

char_t* TconfPageBase::loadText(int resId)
{
 HRSRC rsrc=FindResource(hi,MAKEINTRESOURCE(resId),_l("TEXT"));
 if (!rsrc) return NULL;
 HGLOBAL hglb=LoadResource(hi,rsrc);
 int len=SizeofResource(hi,rsrc);
 TstreamMem ms((const unsigned char*)LockResource(hglb),len,Tstream::ENC_AUTODETECT);
 ms.stripEOLN(false);
 ms.convertUtoD(true);
 len=len*2+3;
 char_t *helpStr0=(char_t*)calloc(len,sizeof(char_t)),*helpStr=helpStr0;
 while (ms.fgets(helpStr,len*sizeof(char_t)))
  helpStr=strchr(helpStr,'\0');
 return helpStr0;
}
void TconfPageBase::loadHelpStr(int dialogId)
{
 if (helpStr) free(helpStr);
 helpStr=loadText(dialogId);
 if (helpStr)
  {
   setDlgItemText(m_hwnd,IDC_ED_HELP,helpStr);
   SendDlgItemMessage(m_hwnd,IDC_ED_HELP,EM_SETSEL,0,-1);
  }
}
void TconfPageBase::translate(void)
{
 HWND hed=GetDlgItem(m_hwnd,IDC_ED_HELP);
 if (hed) setWindowText(hed,_l(""));
 Twindow::translate();

 if (hed && GetWindowTextLength(hed)==0)
  {
   loadHelpStr(dialogId);
   SendMessage(hed,EM_SETMODIFY,0,0);
  }
}

/*
void TconfPageBase::subClass(void)
{
 if (bindsCheckbox)
  for (int i=0;bindsCheckbox[i].idc;i++)
   subClass(bindsCheckbox[i].idc,bindsCheckbox+i,chbWndProc);
 if (bindsRadiobutton)
  for (int i=0;bindsRadiobutton[i].idc;i++)
   subClass(bindsRadiobutton[i].idc,bindsRadiobutton+i,rbtWndProc);
 if (bindsButton)
  for (int i=0;bindsRadiobutton[i].idc;i++)
   subClass(bindsButton[i].idc,bindsButton+i,btWndProc);
 if (bindsHtrack)
  for (int i=0;bindsHtrack[i].idc;i++)
   subClass(bindsHtrack[i].idc,bindsHtrack+i,tbrWndProc);
 if (bindsVtrack)
  for (int i=0;bindsVtrack[i].idc;i++)
   subClass(bindsVtrack[i].idc,bindsVtrack+i,tbrWndProc);
}
LRESULT CALLBACK TconfPageBase::chbWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 TwidgetSubclass *subclass=(TwidgetSubclass*)getDlgItem(hwnd);
 switch (uMsg)
  {
   case BM_SETCHECK:
    if (!subclass->self->ownSetCheck)
     {
      TbindCheckbox *bind=(TbindCheckbox*)(subclass->bind);
      if (bind->idff<0)
       subclass->self->cfgSet(bind->idff,!wParam);
      else
       subclass->self->cfgSet(bind->idff,wParam);
      if (bind->onClick)
       ((subclass->self)->*(bind->onClick))();
     }
    break;
  };
 return CallWindowProc(subclass->oldproc,hwnd,uMsg,wParam,lParam);
}
LRESULT CALLBACK TconfPageBase::rbtWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 TwidgetSubclass *subclass=(TwidgetSubclass*)getDlgItem(hwnd);
 switch (uMsg)
  {
   case BM_SETCHECK:
    if (!subclass->self->ownSetCheck)
     {
      TbindRadiobutton *bind=(TbindRadiobutton*)(subclass->bind);
      if (wParam)
       {
        subclass->self->cfgSet(bind->idff,bind->val);
        if (bind->onClick)
         ((subclass->self)->*(bind->onClick))();
       }
     }
    break;
  };
 return CallWindowProc(subclass->oldproc,hwnd,uMsg,wParam,lParam);
}
LRESULT CALLBACK TconfPageBase::btWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 TwidgetSubclass *subclass=(TwidgetSubclass*)getDlgItem(hwnd);
 switch (uMsg)
  {
   case BM_SETSTATE:
    if (wParam==0)
     {
      TbindButton *bind=(TbindButton*)(subclass->bind);
      ((subclass->self)->*(bind->onClick))();
     }
    break;
  };
 return CallWindowProc(subclass->oldproc,hwnd,uMsg,wParam,lParam);
}
*/

void TconfPageBase::onEditChange(void)
{
 parent->setChange();
}

void TconfPageBase::resize(const CRect &newrect)
{
 setSize(newrect.Width(),newrect.Height());
 anchors.resize(*this,newrect);
}
