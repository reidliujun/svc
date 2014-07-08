/*
 * Copyright (c) 2003-2006 Milan Cutka
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
#include "TpresetsEncDlg.h"
#include "TpresetEnc.h"
#include "IffdshowBase.h"
#include "Ttranslate.h"

TpresetsEncDlg::TpresetsEncDlg(HWND parent,IffdshowBase *Ideci,TpresetsEnc &Ipresets):presets(Ipresets),TdlgWindow(IDD_PRESETSENC,Ideci)
{
 static const TbindButton<TpresetsEncDlg> bt[]=
  {
   IDC_BT_PRESET_RENAME,&TpresetsEncDlg::onRename,
   IDC_BT_PRESET_REMOVE,&TpresetsEncDlg::onRemove,
   0,NULL
  };
 bindButtons(bt);
 dialogBox(dialogId,parent);
}

INT_PTR TpresetsEncDlg::msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_INITDIALOG:
    init();
    return TRUE;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDOK:
      case IDCANCEL:
       EndDialog(m_hwnd,LOWORD(wParam));
       return TRUE;
      case IDC_CBX_PRESETS:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         int i=cbxGetCurSel(IDC_CBX_PRESETS);
         selectPreset(i+2);
         return TRUE;
        }
       break;
     };
    break;
  }
 return TdlgWindow::msgProc(uMsg,wParam,lParam);
}

void TpresetsEncDlg::onRename(void)
{
 TpresetEnc *preset=(TpresetEnc*)cbxGetCurItemData(IDC_CBX_PRESETS);
 char_t presetName[MAX_PATH];
 ff_strncpy(presetName, preset->name, countof(presetName));
 if (inputString(_(-IDD_PRESETSENC,_l("Rename preset")),_(-IDD_PRESETSENC,_l("Enter new preset name")),presetName,MAX_PATH) && presetName[0])
  {
   preset->rename(presetName);
   fillPresets();
  }
}
void TpresetsEncDlg::onRemove(void)
{
 TpresetEnc *preset=(TpresetEnc*)cbxGetCurItemData(IDC_CBX_PRESETS);
 if (MessageBox(m_hwnd,_(-IDD_PRESETSENC,_l("Are you sure you want to delete selected preset?")),_(-IDD_PRESETSENC,_l("Confirm preset removal")),MB_YESNO|MB_ICONQUESTION)==IDYES)
  {
   presets.remove(preset);
   fillPresets();
  }
}

void TpresetsEncDlg::init(void)
{
 translate();
 const char_t *capt=tr->translate(m_hwnd,dialogId,0,NULL);
 if (capt && capt[0]) setWindowText(m_hwnd,capt);
 hlv=GetDlgItem(m_hwnd,IDC_LV_PRESETENC);
 int ncol=0;
 RECT r;
 GetWindowRect(hlv,&r);
 ListView_AddCol(hlv,ncol,(r.right-r.left)/2,_(IDC_LV_PRESETENC,_l("Property")),false);
 ListView_AddCol(hlv,ncol,(r.right-r.left)/2-25,_(IDC_LV_PRESETENC,_l("Value")),false);
 ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

 fillPresets();
 selectPreset(2);
}
void TpresetsEncDlg::fillPresets(void)
{
 int ii=cbxGetCurSel(IDC_CBX_PRESETS);
 SendDlgItemMessage(m_hwnd,IDC_CBX_PRESETS,CB_RESETCONTENT,0,0);
 for (size_t i=2;i<presets.size();i++)
  cbxAdd(IDC_CBX_PRESETS,presets[i].name,intptr_t(&presets[i]));
 selectPreset(ii+2);
}

void TpresetsEncDlg::selectPreset(int i)
{
 static const int idPreset[]={IDC_BT_PRESET_RENAME,IDC_BT_PRESET_REMOVE,0};
 if (cbxSetCurSel(IDC_CBX_PRESETS,i-2)==CB_ERR)
  {
   enable(false,idPreset);
   return;
  }
 enable(true,idPreset);

 ListView_DeleteAllItems(hlv);
 TpresetEnc &preset=presets[i];
 int ii=0;
 for (TpresetEnc::Tvals::const_iterator v=preset.vals.begin();v!=preset.vals.end();v++,ii++)
  {
   const Tval &val=v->second;
   LVITEM lvi;memset(&lvi,0,sizeof(lvi));
   lvi.mask=LVIF_TEXT|LVIF_PARAM;
   lvi.iItem=ii;
   lvi.pszText=LPTSTR(v->first);
   lvi.lParam=LPARAM(&val);
   int iii=ListView_InsertItem(hlv,&lvi);
   char_t valS[20];
   ListView_SetItemText(hlv,iii,1,LPTSTR(val.s.empty()?_itoa(val.i,valS,10):val.s.c_str()));
  }
}
