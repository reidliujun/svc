/*
 * Copyright (c) 2004-2006 Milan Cutka
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
#include "CdeinterlaceFramedoubler.h"
#include "IffdshowBase.h"
#include "ffdshow_constants.h"
#include "TdeinterlaceSettings.h"

TdeinterlacePanel* TdeinterlacePageFramedoubler::create(Twindow *parent)
{
 return new TdeinterlacePageFramedoubler(parent);
}

TdeinterlacePageFramedoubler::TdeinterlacePageFramedoubler(Twindow *parent):TdeinterlacePanel(IDD_DEINTERLACE_FRAMEDBL,parent)
{
 static const TbindTrackbar<TdeinterlacePageFramedoubler> htbr[]=
  {
   IDC_TBR_FRAMEDOUBLE_THRESHOLD,IDFF_frameRateDoublerThreshold,&TdeinterlacePageFramedoubler::cfg2dlg,
   0
  };
 bindHtracks(htbr);
 static const TbindCombobox<TdeinterlacePageFramedoubler> cbx[]=
  {
   IDC_CBX_FRAMEDOUBLE_SE,IDFF_frameRateDoublerSE,BINDCBX_DATA,&TdeinterlacePageFramedoubler::cfg2dlg,
   0
  };
 bindComboboxes(cbx);

 createDialog(dialogId,parent->m_hwnd);
}
TdeinterlacePageFramedoubler::~TdeinterlacePageFramedoubler()
{
 DestroyWindow(m_hwnd);
}

void TdeinterlacePageFramedoubler::init(void)
{
 translate();
 tbrSetRange(IDC_TBR_FRAMEDOUBLE_THRESHOLD,0,255,16);
}

void TdeinterlacePageFramedoubler::cfg2dlg(void)
{
 int se=cfgGet(IDFF_frameRateDoublerSE);
 cbxSetDataCurSel(IDC_CBX_FRAMEDOUBLE_SE,se);
 tbrSet(IDC_TBR_FRAMEDOUBLE_THRESHOLD,cfgGet(IDFF_frameRateDoublerThreshold),IDC_LBL_FRAMEDOUBLE_THRESHOLD);
 static const int idFramedoubleThr[]={IDC_LBL_FRAMEDOUBLE_THRESHOLD,IDC_TBR_FRAMEDOUBLE_THRESHOLD,0};
 enable(se!=0,idFramedoubleThr);
}

void TdeinterlacePageFramedoubler::translate(void)
{
 TdeinterlacePanel::translate();

 int sel=cbxGetCurSel(IDC_CBX_FRAMEDOUBLE_SE);
 cbxClear(IDC_CBX_FRAMEDOUBLE_SE);
 for (int i=0;TdeinterlaceSettings::frameRateDoublerSEs[i].name;i++)
  cbxAdd(IDC_CBX_FRAMEDOUBLE_SE,_(IDC_CBX_FRAMEDOUBLE_SE,TdeinterlaceSettings::frameRateDoublerSEs[i].name),TdeinterlaceSettings::frameRateDoublerSEs[i].id);
 cbxSetCurSel(IDC_CBX_FRAMEDOUBLE_SE,sel);
}

void TdeinterlacePageFramedoubler::reset(void)
{
 deci->resetParam(IDFF_frameRateDoublerThreshold);
 deci->resetParam(IDFF_frameRateDoublerSE);
}
