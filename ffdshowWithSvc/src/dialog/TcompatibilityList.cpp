/*
 * Copyright (c) 2007 h.yamagata
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
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "ffdshow_constants.h"
#include "Ttranslate.h"
#include "TflatButtons.h"
#include "Cdirectshowcontrol.h"
#include "Tconfig.h"
#include "TglobalSettings.h"
#include "TcompatibilityList.h"

//================================= TcompatibilityList ==================================
TcompatibilityList::TcompatibilityList(IffdshowBase *Ideci,HWND Iparent,int x,int y,int target):TdlgWindow(IDD_COMPATIBILITY,Ideci),parent(Iparent),deciD(Ideci)
{
 this->x=x;this->y=y;
 IDFF_target=target;
 static const TbindButton<TcompatibilityList> bt[]=
  {
   IDC_BT_ADDCOMPATIBLEFILE,&TcompatibilityList::onAddFile,
   IDC_BT_COMP_RESET,&TcompatibilityList::onReset,
   0,NULL
  };
 bindButtons(bt); 
}
void TcompatibilityList::init(void)
{
 resizeDialog();
 translate();
 const char_t *capt,*exp;
 if (IDFF_target==IDFF_whitelist)
  {
   capt=tr->translate(m_hwnd,IDD_DIRECTSHOWCONTROL,IDC_CHB_COMPATIBILITYLIST,NULL);
   exp= tr->translate(m_hwnd,dialogId,IDC_TXT_COMPATIBILITYLIST,NULL);
  }
 else
  {
   capt=tr->translate(m_hwnd,IDD_DIRECTSHOWCONTROL,IDC_CHB_BLACKLIST,NULL);
   exp= _(-IDD_COMPATIBILITY,_l("List the names of executable files which are incompatible with ffdshow or which you don't want to use ffdshow, one for each line."));
  }
 if (capt && capt[0]) setWindowText(m_hwnd,capt);
 if (exp && exp[0]) setText(IDC_TXT_COMPATIBILITYLIST,exp);
 edLimitText(IDC_ED_COMPATIBILITYLIST,MAX_COMPATIBILITYLIST_LENGTH);
 cfg2dlg(NULL);
}
bool TcompatibilityList::show(void)
{
 return !!dialogBox(dialogId,parent);
}

void TcompatibilityList::cfg2dlg(const char_t *newFileName)
{
 cfg2dlgI(cfgGetStr(IDFF_target),newFileName,getDelimit());
}
void TcompatibilityList::dlg2dlg(const char_t *newFileName)
{
 char_t complist0[MAX_COMPATIBILITYLIST_LENGTH];
 GetDlgItemText(m_hwnd,IDC_ED_COMPATIBILITYLIST,complist0,MAX_COMPATIBILITYLIST_LENGTH);
 cfg2dlgI(complist0,newFileName,_l("\r\n"));
}
void TcompatibilityList::cfg2dlgI(const char_t *complist0,const char_t *newFileName,const char_t *delimit)
{
 ffstring complistResult;
 TdirectshowControlPageDec::convertDelimit(complist0,delimit,complistResult,_l("\r\n"),newFileName);
 SetDlgItemText(m_hwnd,IDC_ED_COMPATIBILITYLIST,complistResult.c_str());
}
void TcompatibilityList::dlg2cfg(void)
{
 char_t complist0[MAX_COMPATIBILITYLIST_LENGTH];
 GetDlgItemText(m_hwnd,IDC_ED_COMPATIBILITYLIST,complist0,MAX_COMPATIBILITYLIST_LENGTH);
 ffstring complistResult;

 TdirectshowControlPageDec::convertDelimit(complist0,_l("\r\n"),complistResult,getDelimit());
 cfgSet(IDFF_target,complistResult.c_str());
}
const char_t* TcompatibilityList::getDelimit(void)
{
  return _l(";");
}
INT_PTR TcompatibilityList::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_INITDIALOG:
    init();
    return TRUE;
   case WM_DESTROY:
    break; 
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDOK:
       {
        dlg2cfg();
        bool wasChange=true;
        EndDialog(m_hwnd,wasChange);
        return TRUE;
       }
      case IDCANCEL:
       {
        bool wasChange=false;
        EndDialog(m_hwnd,wasChange);
        return TRUE;
       }
     }
    break;
  }
 return TdlgWindow::msgProc(uMsg,wParam,lParam);
}
void TcompatibilityList::onAddFile(void)
{
 char_t newFileName[MAX_PATH]=_l("");
 const char_t* mask=_l("Executable (*.exe)\0*.exe\0");
 ffstring startdir(config->pth);
 if (startdir[startdir.size()-1]=='\\')
  startdir+=_l("..");
 else
  startdir+=_l("\\..");

 const char_t *capt;
 if (IDFF_target==IDFF_whitelist)
  capt=_l("Add compatible executable file");
 else
  capt=_l("Add incompatible executable file");

 if (dlgGetFile(false,m_hwnd,_(-IDD_COMPATIBILITY,capt),mask,_l("exe"),newFileName,startdir.c_str(),0))
  {
   char_t filename[MAX_PATH];
   extractfilename(newFileName,filename);
   dlg2dlg(filename);
  }
}
void TcompatibilityList::onReset(void)
{
 if (MessageBox(NULL,
      _(-IDD_COMPATIBILITY,_l("Are you sure?")),
      _(-IDD_COMPATIBILITY,_l("Load default settings")),
      MB_ICONQUESTION|MB_OKCANCEL)==IDOK)
  {
   if (IDFF_target==IDFF_whitelist)
    {
     ffstring complistResult;
     TdirectshowControlPageDec::convertDelimit(WHITELIST_EXE_FILENAME,_l(";"),complistResult,_l("\r\n")); // to sort
     SetDlgItemText(m_hwnd,IDC_ED_COMPATIBILITYLIST,complistResult.c_str());
    }
   if (IDFF_target==IDFF_blacklist)
    {
     ffstring complistResult;
     TdirectshowControlPageDec::convertDelimit(BLACKLIST_EXE_FILENAME,_l(";"),complistResult,_l("\r\n"));
     SetDlgItemText(m_hwnd,IDC_ED_COMPATIBILITYLIST,complistResult.c_str());
    }
  }
}
