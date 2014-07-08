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
#include "TversionsDlg.h"
#include "IffdshowBase.h"
#include "Ttranslate.h"

TversionsDlg::TversionsDlg(IffdshowBase *Ideci,HWND parent,const TversionInfo *Ifcs):TdlgWindow(IDD_VERSIONS,Ideci),fcs(Ifcs)
{
 dialogBox(dialogId,parent);
}

INT_PTR TversionsDlg::msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_INITDIALOG:
    {
     hlv=GetDlgItem(m_hwnd,IDC_LV_VERSIONS);
     ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP,LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP);
     translate();
     const char_t *capt=tr->translate(m_hwnd,dialogId,0,NULL);
     if (capt && capt[0]) setWindowText(m_hwnd,capt);
     int ncol=0;
     ListView_AddCol(hlv,ncol,114,_(IDC_LV_VERSIONS,_l("Library")),false);
     ListView_AddCol(hlv,ncol,370,_(IDC_LV_VERSIONS,_l("Version")),false);
     for (int i=0,j=0;fcs[i].name;i++)
      {
       LVITEM lvi;memset(&lvi,0,sizeof(lvi));
       lvi.mask=LVIF_TEXT;
       lvi.iItem=j;
       lvi.pszText=LPTSTR(fcs[i].name);
       ListView_InsertItem(hlv,&lvi);
       ffstring ver,lic;
       fcs[i].fc(config,ver,lic);
       ListView_SetItemText(hlv,j,1,LPTSTR(ver.c_str()));
       j++;
       if (!lic.empty())
        {
         strings lines;strtok(lic.c_str(),_l("\n"),lines);
         for (strings::const_iterator l=lines.begin();l!=lines.end();l++,j++)
          {
           lvi.iItem=j;
           lvi.pszText=_l("");
           ListView_InsertItem(hlv,&lvi);
           ListView_SetItemText(hlv,j,1,LPTSTR(l->c_str()));
          }
        }
      }
     return TRUE;
    }
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDOK:
      case IDCANCEL:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         EndDialog(m_hwnd,LOWORD(wParam));
         return TRUE;
        }
       break;
     };
    break;
  }
 return TdlgWindow::msgProc(uMsg,wParam,lParam);
}
