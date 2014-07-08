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
#include "CdeinterlaceTomsMoComp.h"
#include "IffdshowBase.h"
#include "ffdshow_constants.h"

TdeinterlacePanel* TdeinterlacePageTomsMoComp::create(Twindow *parent)
{
 return new TdeinterlacePageTomsMoComp(parent);
}

TdeinterlacePageTomsMoComp::TdeinterlacePageTomsMoComp(Twindow *parent):TdeinterlacePanel(IDD_DEINTERLACE_TOMSMOCOMP,parent)
{
 static const TbindCheckbox<TdeinterlacePageTomsMoComp> chb[]=
  {
   IDC_CHB_TOMSMOCOMP_VF,IDFF_tomocompVF,NULL,
   0
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TdeinterlacePageTomsMoComp> htbr[]=
  {
   IDC_TBR_TOMSMOCOMP_SEARCH,IDFF_tomocompSE,&TdeinterlacePageTomsMoComp::cfg2dlg,
   0
  };
 bindHtracks(htbr);

 createDialog(dialogId,parent->m_hwnd);
}
TdeinterlacePageTomsMoComp::~TdeinterlacePageTomsMoComp()
{
 DestroyWindow(m_hwnd);
}

void TdeinterlacePageTomsMoComp::init(void)
{
 translate();
 tbrSetRange(IDC_TBR_TOMSMOCOMP_SEARCH,0,30,3);
}

void TdeinterlacePageTomsMoComp::cfg2dlg(void)
{
 tbrSet(IDC_TBR_TOMSMOCOMP_SEARCH,cfgGet(IDFF_tomocompSE),IDC_LBL_TOMSMOCOMP_SEARCH);
 setCheck(IDC_CHB_TOMSMOCOMP_VF,cfgGet(IDFF_tomocompVF));
}

void TdeinterlacePageTomsMoComp::reset(void)
{
 deci->resetParam(IDFF_tomocompSE);
 deci->resetParam(IDFF_tomocompVF);
}
