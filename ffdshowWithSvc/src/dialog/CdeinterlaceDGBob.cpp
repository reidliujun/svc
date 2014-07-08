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
#include "CdeinterlaceDGBob.h"
#include "IffdshowBase.h"
#include "ffdshow_constants.h"
#include "TdeinterlaceSettings.h"

TdeinterlacePanel* TdeinterlacePageDGbob::create(Twindow *parent)
{
 return new TdeinterlacePageDGbob(parent);
}

TdeinterlacePageDGbob::TdeinterlacePageDGbob(Twindow *parent):TdeinterlacePanel(IDD_DEINTERLACE_DGBOB,parent)
{
 static const TbindCheckbox<TdeinterlacePageDGbob> chb[]=
  {
   IDC_CHB_DGBOB_AP,IDFF_dgbobAP,NULL,
   0
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TdeinterlacePageDGbob> htbr[]=
  {
   IDC_TBR_DGBOB_THRESHOLD,IDFF_dgbobThreshold,&TdeinterlacePageDGbob::cfg2dlg,
   0
  };
 bindHtracks(htbr);
 static const TbindCombobox<TdeinterlacePageDGbob> cbx[]=
  {
   IDC_CBX_DGBOB_MODE,IDFF_dgbobMode,BINDCBX_SEL,&TdeinterlacePageDGbob::cfg2dlg,
   0
  };
 bindComboboxes(cbx);

 createDialog(dialogId,parent->m_hwnd);
}
TdeinterlacePageDGbob::~TdeinterlacePageDGbob()
{
 DestroyWindow(m_hwnd);
}

void TdeinterlacePageDGbob::init(void)
{
 translate();
 tbrSetRange(IDC_TBR_DGBOB_THRESHOLD,0,60);
}

void TdeinterlacePageDGbob::cfg2dlg(void)
{
 tbrSet(IDC_TBR_DGBOB_THRESHOLD,cfgGet(IDFF_dgbobThreshold),IDC_LBL_DGBOB_THRESHOLD);
 cbxSetCurSel(IDC_CBX_DGBOB_MODE,cfgGet(IDFF_dgbobMode));
 setCheck(IDC_CHB_DGBOB_AP,cfgGet(IDFF_dgbobAP));
}

void TdeinterlacePageDGbob::translate(void)
{
 TdeinterlacePanel::translate();

 cbxTranslate(IDC_CBX_DGBOB_MODE,TdeinterlaceSettings::dgbobModes);
}

void TdeinterlacePageDGbob::reset(void)
{
 deci->resetParam(IDFF_dgbobMode);
 deci->resetParam(IDFF_dgbobThreshold);
 deci->resetParam(IDFF_dgbobAP);
}
