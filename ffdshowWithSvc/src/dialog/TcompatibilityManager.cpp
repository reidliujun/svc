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
#include "TcompatibilityManager.h"
#include "string.h"

//================================= TcompatibilityManager ==================================
int TcompatibilityManager::s_mode;

TcompatibilityManager::TcompatibilityManager(IffdshowBase *Ideci,HWND Iparent,const char_t *exe):TdlgWindow(IDD_COMPATIBILITY_MANAGER,Ideci),parent(Iparent),deciD(Ideci),filename(exe)
{
 static const TbindRadiobutton<TcompatibilityManager> rbt[]=
  {
   IDC_RBT_COMP_1,IDFF_compManagerMode,1,&TcompatibilityManager::onModeChange,
   IDC_RBT_COMP_2,IDFF_compManagerMode,2,&TcompatibilityManager::onModeChange,
   IDC_RBT_COMP_3,IDFF_compManagerMode,3,&TcompatibilityManager::onModeChange,
   IDC_RBT_COMP_4,IDFF_compManagerMode,4,&TcompatibilityManager::onModeChange,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindCheckbox<TcompatibilityManager> chb[]=
  {
   IDC_CHB_COMP_DONTASK,IDFF_isCompMgr,&TcompatibilityManager::onDontAsk,
   IDC_CHB_COMP_SEND,NULL,&TcompatibilityManager::onChbSend,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
}

void TcompatibilityManager::init(void)
{
 translate();
 setcaption();
 const char_t *reason1,*reason2;
 reason1= tr->translate(m_hwnd,dialogId,IDC_TXT_COMP_1,NULL);
 reason2= tr->translate(m_hwnd,dialogId,IDC_TXT_COMP_2,NULL);
 ffstring reason(reason1);
 reason+=filename+reason2;
 if (reason.size()>0) setText(IDC_TXT_COMP_1,reason.c_str());
 mode=1;chbsend=false;
 cfgSet(IDFF_compManagerMode,mode);
 cfg2dlg();
}

void TcompatibilityManager::setcaption(void)
{
 const char_t *capt=tr->translate(m_hwnd,IDD_COMPATIBILITY_MANAGER,0,_l("ffdshow compatibility manager"));
 ffstring caption(capt);
 if (time>=0)
  {
   const char_t *time_msg1=_(-IDD_COMPATIBILITY_MANAGER,_l("(This dialog will close in"));
   const char_t *time_msg2=_(-IDD_COMPATIBILITY_MANAGER,_l("seconds.)"));
   char_t time_num[20];
   tsprintf(time_num,_l(" %d "),(TCOMPATIBILITYMANAGER_TIMEOUT-time)/10);
   caption=caption+_l("   ")+time_msg1+time_num+time_msg2;
  }
 setWindowText(m_hwnd,caption.c_str());
}

void TcompatibilityManager::onModeChange(void)
{
 time=-1;setcaption();
 mode=cfgGet(IDFF_compManagerMode);
 cfg2dlg();
}

void TcompatibilityManager::cfg2dlg(void)
{
 bool show_report_checkbox = false;

 setCheck(IDC_RBT_COMP_1, mode==1);
 setCheck(IDC_RBT_COMP_2, mode==2);
 setCheck(IDC_RBT_COMP_3, mode==3);
 setCheck(IDC_RBT_COMP_4, mode==4);
 
 /* only enable reporting if filename is not already on the list */ 
 if(mode==2) {
 	ffstring blacklist_lowercase(BLACKLIST_EXE_FILENAME);
 	blacklist_lowercase.ConvertToLowerCase();
 	ffstring filename_lowercase = filename.ConvertToLowerCase();
 	show_report_checkbox = blacklist_lowercase.find(filename_lowercase) == ffstring::npos;
 } else {
 	if(mode==4) {
 	  ffstring whitelist_lowercase(WHITELIST_EXE_FILENAME);
 	  whitelist_lowercase.ConvertToLowerCase();
 	  ffstring filename_lowercase = filename.ConvertToLowerCase();
 		show_report_checkbox = whitelist_lowercase.find(filename_lowercase) == ffstring::npos;
 	}
 }
 enable(show_report_checkbox,IDC_CHB_COMP_SEND);
 Twindow::show(show_report_checkbox,IDC_CHB_COMP_SEND);

 char_t *msg=NULL;
 switch (mode)
  {
   case 1: msg=_l("ffdshow will behave as if it were not installed.");
    break;
   case 2: msg=_l("ffdshow will behave as if it were not installed.\r\n")
               _l("The application will be added to \"DirectShow control\"->\"Don't use ffdshow in\".");
    break;
   case 3: msg=_l("ffdshow will be used if it is set to decode a format.");
    break;
   case 4: msg=_l("ffdshow will be used if it is set to decode a format.\r\n")
               _l("The application will be added to \"DirectShow control\"->\"Use ffdshow only in\".");
    break;
  }
 if (msg)
  setText(IDC_EDT_COMP_1,_(-IDD_COMPATIBILITY_MANAGER,msg));
}

void TcompatibilityManager::onDontAsk(void)
{
 time=-1;setcaption();
 char_t *msg=_l("ffdshow won't be used in unknown applications.\r\n")
             _l("To enable this dialog again, check \"DirectShow control\"->\"Show dialog when an unknown application tries to load ffdshow\".");
 setText(IDC_EDT_COMP_1,_(-IDD_COMPATIBILITY_MANAGER,msg));
 cfgSet(IDFF_isCompMgrChanged,1);
 cfgSet(IDFF_isCompMgr,!getCheck(IDC_CHB_COMP_DONTASK));
}

void TcompatibilityManager::onChbSend(void)
{
 time=-1;setcaption();
 chbsend=!chbsend;
 setCheck(IDC_CHB_COMP_SEND,chbsend);
 char_t *msg=_l("Thank you. We are gathering a list of (in)compatible applications. By sending us the filename of this application you are helping us to improve ffdshow.");
 setText(IDC_EDT_COMP_1,_(-IDD_COMPATIBILITY_MANAGER,msg));
}

int TcompatibilityManager::show(void)
{
 dialogBox(dialogId,parent);
 s_mode=mode; // set global variable.
 return mode;
}

INT_PTR TcompatibilityManager::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    KillTimer(m_hwnd,1);
    break;
   case WM_TIMER:
    SetWindowPos(m_hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
    if ((time % 10)==0)
     setcaption();
    if (time>=0) time++;
    if (time>TCOMPATIBILITYMANAGER_TIMEOUT)
     {
      EndDialog(m_hwnd,true);
      return TRUE;
     }
    return 0;
   case WM_INITDIALOG:
    init();
    SetTimer(m_hwnd,1,100,NULL);
    time=0;
    return TRUE;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDOK:
       {
        if( (mode==2 || mode==4) && chbsend )
         {
          int revision;
          ffstring targetlist;
          if(mode==4) {
            targetlist = "whitelist";
          } else {
            targetlist = "blacklist";
          }
          TregOpRegRead tNSI(HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT _l("\\") FFDSHOW);
          tNSI._REG_OP_N(0,_l("revision"),revision,0);
          char_t param[MAX_PATH+80];
          ffstring instpath(cfgGetStr(IDFF_installPath));
          if (instpath.size() && instpath[instpath.size()-1]!='\\')
          instpath=instpath+_l("\\");
          tsnprintf_s(param, countof(param), _TRUNCATE, _l("\"%sopenIE.js\" \"%s\" %d %s"), instpath.c_str(), filename.c_str(), revision, targetlist.c_str());
          if ((int)ShellExecute(NULL, _l("open"),_l("wscript.exe") ,param , NULL, SW_SHOWDEFAULT)<=32)
           {
            char_t url[2084];
            tsnprintf_s(url, countof(url), _TRUNCATE, _l("http://ffdshow-tryout.sourceforge.net/compmgr.php?app=%s&rev=%d&type=%s"),filename.c_str(), revision, targetlist.c_str());
            ShellExecute(NULL, _l("open"),url , NULL, NULL, SW_HIDE);
           }
         }
        EndDialog(m_hwnd,true);
        return TRUE;
       }
     }
    break;
  }
 return TdlgWindow::msgProc(uMsg,wParam,lParam);
}
