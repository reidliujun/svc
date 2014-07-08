/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include "CfirstPass.h"

bool TfirstPassPage::enabled(void)
{
 return sup_perFrameQuant(codecId) && cfgGet(IDFF_enc_mode)==ENC_MODE::PASS2_1;
}

void TfirstPassPage::cfg2dlg(void)
{
 setDlgItemText(m_hwnd,IDC_ED_STATS1,cfgGetStr(IDFF_enc_stats1flnm));
}

INT_PTR TfirstPassPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_STATS1:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t flnm[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_STATS1,flnm,MAX_PATH);
         cfgSet(IDFF_enc_stats1flnm,flnm);
         return TRUE;
        }
       break;
     }
    break;
  }
 return TconfPageEnc::msgProc(uMsg,wParam,lParam);
}

void TfirstPassPage::onStats1(void)
{
 char_t flnm[MAX_PATH];cfgGet(IDFF_enc_stats1flnm,flnm,MAX_PATH);
 if (dlgGetFile(true,m_hwnd,_(-IDD_FIRSTPASS,_l("Select stats file for storing first pass statistics")),_l("Stats file (*.stats)\0*.stats\0All files (*.*)\0*.*\0"),_l("stats"),flnm,_l("."),0))
  {
   cfgSet(IDFF_enc_stats1flnm,flnm);
   cfg2dlg();
  }
}

TfirstPassPage::TfirstPassPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_FIRSTPASS;
 static const int props[]={IDFF_enc_stats1flnm,0};
 propsIDs=props;
 static const TbindButton<TfirstPassPage> bt[]=
  {
   IDC_BT_STATS1,&TfirstPassPage::onStats1,
   0,NULL
  };
 bindButtons(bt);
}
