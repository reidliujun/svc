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
#include "Cgrab.h"
#include "Tconfig.h"
#include "TgrabSettings.h"
#include "TffdshowPageDec.h"
#include "Tlibavcodec.h"

void TgrabPage::init(void)
{
 edLimitText(IDC_ED_GRAB_NAME,30);
 for (char i=0;i<=6;i++)
  {
   char_t n[]={i+'0',0};
   cbxAdd(IDC_CBX_GRAB_DIGITS,n);
  }
 const Tconfig *config;deci->getConfig(&config);
 Tlibavcodec *lavc;
 deci->getLibavcodec(&lavc);
 for (int i=0;TgrabSettings::formats[i].name;i++)
  {
   int codec=TgrabSettings::formats[i].codec;
   if (codec==0 || ((codec==IDFF_MOVIE_LAVC || codec==IDFF_MOVIE_LSVC) && lavc && lavc->ok && !lavc->dec_only) || (codec!=IDFF_MOVIE_LAVC && config->isDecoder[codec]))
    cbxAdd(IDC_CBX_GRAB_FORMAT,TgrabSettings::formats[i].name,i);
  }
 if (lavc) lavc->Release();
 enable(filterMode&IDFF_FILTERMODE_PLAYER,IDC_BT_GRAB_NOW);
 tbrSetRange(IDC_TBR_GRAB_QUAL,0,100,10);
}

void TgrabPage::cfg2dlg(void)
{
 setDlgItemText(m_hwnd,IDC_ED_GRAB_PATH,cfgGetStr(IDFF_grabPath  ));
 setDlgItemText(m_hwnd,IDC_ED_GRAB_NAME,cfgGetStr(IDFF_grabPrefix));
 cbxSetCurSel(IDC_CBX_GRAB_DIGITS,cfgGet(IDFF_grabDigits));
 int format=cfgGet(IDFF_grabFormat);
 if (cbxSetDataCurSel(IDC_CBX_GRAB_FORMAT,format)==CB_ERR)
  {
   format=(int)cbxGetItemData(IDC_CBX_GRAB_FORMAT,0);
   cfgSet(IDFF_grabFormat,format);
   parent->setChange();
   cbxSetCurSel(IDC_CBX_GRAB_FORMAT,0);
  }

 int mode=cfgGet(IDFF_grabMode);
 setCheck(IDC_RBT_GRAB_EVERY ,mode==0);
 setCheck(IDC_RBT_GRAB_SINGLE,mode==1);enable(mode==1,IDC_ED_GRAB_FRAMENUM );SetDlgItemInt(m_hwnd,IDC_ED_GRAB_FRAMENUM,cfgGet(IDFF_grabFrameNum ),FALSE);
 setCheck(IDC_RBT_GRAB_RANGE ,mode==2);enable(mode==2,IDC_ED_GRAB_FRAMENUM1);enable(mode==2,IDC_ED_GRAB_FRAMENUM2);SetDlgItemInt(m_hwnd,IDC_ED_GRAB_FRAMENUM1,cfgGet(IDFF_grabFrameNum1),FALSE);SetDlgItemInt(m_hwnd,IDC_ED_GRAB_FRAMENUM2,cfgGet(IDFF_grabFrameNum2),FALSE);
 static const int idStep[]={IDC_LBL_GRAB_STEP,IDC_ED_GRAB_STEP,IDC_LBL_GRAB_STEP2,0};
 enable(mode==0 || mode==2,idStep);
 SetDlgItemInt(m_hwnd,IDC_ED_GRAB_STEP,cfgGet(IDFF_grabStep),FALSE);

 tbrSet(IDC_TBR_GRAB_QUAL,cfgGet(IDFF_grabQual),IDC_LBL_GRAB_QUAL);
 enable(format!=1,IDC_LBL_GRAB_QUAL);
 enable(format!=1,IDC_TBR_GRAB_QUAL);
}

INT_PTR TgrabPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_BT_GRAB_PATH:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         if (changeDir(IDFF_grabPath,_(-IDD_GRAB,_l("Select directory for storing images"))))
          cfg2dlg();
         return TRUE;
        }
       break;
      case IDC_ED_GRAB_NAME:
       if (HIWORD(wParam)==EN_UPDATE)
        {
         char_t prefix[31];GetDlgItemText(m_hwnd,IDC_ED_GRAB_NAME,prefix,30);
         cfgSet(IDFF_grabPrefix,prefix);
         return TRUE;
        }
       break;
      case IDC_ED_GRAB_PATH:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t path[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_GRAB_PATH,path,MAX_PATH);
         cfgSet(IDFF_grabPath,path);
         return TRUE;
        }
       break;
      case IDC_BT_GRAB_NOW:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         deciV->grabNow();
         return TRUE;
        }
       break;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}
TgrabPage::TgrabPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_GRAB;
 static const TbindTrackbar<TgrabPage> htbr[]=
  {
   IDC_TBR_GRAB_QUAL,IDFF_grabQual,&TgrabPage::cfg2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindRadiobutton<TgrabPage> rbt[]=
  {
   IDC_RBT_GRAB_EVERY,IDFF_grabMode,0,&TgrabPage::cfg2dlg,
   IDC_RBT_GRAB_SINGLE,IDFF_grabMode,1,&TgrabPage::cfg2dlg,
   IDC_RBT_GRAB_RANGE,IDFF_grabMode,2,&TgrabPage::cfg2dlg,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindEditInt<TgrabPage> edInt[]=
  {
   IDC_ED_GRAB_FRAMENUM ,0,1000000,IDFF_grabFrameNum ,NULL,
   IDC_ED_GRAB_FRAMENUM1,0,1000000,IDFF_grabFrameNum1,NULL,
   IDC_ED_GRAB_FRAMENUM2,0,1000000,IDFF_grabFrameNum2,NULL,
   IDC_ED_GRAB_STEP,1,1000,IDFF_grabStep,NULL,
   0
  };
 bindEditInts(edInt);
 static const TbindCombobox<TgrabPage> cbx[]=
  {
   IDC_CBX_GRAB_DIGITS,IDFF_grabDigits,BINDCBX_SEL,NULL,
   IDC_CBX_GRAB_FORMAT,IDFF_grabFormat,BINDCBX_DATA,&TgrabPage::cfg2dlg,
   0
  };
 bindComboboxes(cbx);
}
