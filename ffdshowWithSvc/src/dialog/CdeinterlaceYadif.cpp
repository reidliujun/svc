/*
 * Copyright (c) 2008 h.yamagata
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
#include "Twindow.h"
#include "CdeinterlaceYadif.h"
#include "IffdshowBase.h"
#include "ffdshow_constants.h"
#include "TdeinterlaceSettings.h"

TdeinterlacePanel* TdeinterlacePageYadif::create(Twindow *parent)
{
 return new TdeinterlacePageYadif(parent);
}

TdeinterlacePageYadif::TdeinterlacePageYadif(Twindow *parent):TdeinterlacePanel(IDD_DEINTERLACE_YADIF,parent)
{
 static const TbindCombobox<TdeinterlacePageYadif> cbx[]=
  {
   IDC_CBX_YADIF_PARITY, IDFF_yadifFieldOrder, BINDCBX_DATA, &TdeinterlacePageYadif::cfg2dlg,
   0
  };
 bindComboboxes(cbx);

 createDialog(dialogId,parent->m_hwnd);
}
TdeinterlacePageYadif::~TdeinterlacePageYadif()
{
 DestroyWindow(m_hwnd);
}

void TdeinterlacePageYadif::init(void)
{
 translate();

 int sel=cbxGetCurSel(IDC_CBX_YADIF_PARITY);
 cbxClear(IDC_CBX_YADIF_PARITY);
 for (int i = 0 ; TdeinterlaceSettings::yadifParitySEs[i].name ; i++)
  cbxAdd(IDC_CBX_YADIF_PARITY,
         _(IDC_CBX_YADIF_PARITY,
           TdeinterlaceSettings::yadifParitySEs[i].name),
           TdeinterlaceSettings::yadifParitySEs[i].id);
 cbxSetCurSel(IDC_CBX_YADIF_PARITY,sel);
}

void TdeinterlacePageYadif::cfg2dlg(void)
{
 setCheck(IDC_CHB_YADIF_SKIP_CHECK, !!(cfgGet(IDFF_yadifMode) & 2));
 setCheck(IDC_CHB_YADIF_DOUBLE_FRAMERATE, cfgGet(IDFF_yadifMode) & 1);
 int se=cfgGet(IDFF_yadifFieldOrder);
 cbxSetDataCurSel(IDC_CBX_YADIF_PARITY,se);
}

void TdeinterlacePageYadif::dlg2cfg(void)
{
 int mode = getCheck(IDC_CHB_YADIF_SKIP_CHECK) * 2 + getCheck(IDC_CHB_YADIF_DOUBLE_FRAMERATE);
 cfgSet(IDFF_yadifMode, mode);
}

void TdeinterlacePageYadif::reset(void)
{
 deci->resetParam(IDFF_yadifMode);
}

INT_PTR TdeinterlacePageYadif::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_CHB_YADIF_SKIP_CHECK:
      case IDC_CHB_YADIF_DOUBLE_FRAMERATE:
       dlg2cfg();
       return TRUE;
     }    
    break;
  }
 return TdeinterlacePanel::msgProc(uMsg,wParam,lParam);
}
