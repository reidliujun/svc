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
#include "CdeinterlaceKernel.h"
#include "IffdshowBase.h"
#include "ffdshow_constants.h"

TdeinterlacePanel* TdeinterlacePageKernel::create(Twindow *parent)
{
 return new TdeinterlacePageKernel(parent);
}

TdeinterlacePageKernel::TdeinterlacePageKernel(Twindow *parent):TdeinterlacePanel(IDD_DEINTERLACE_KERNEL,parent)
{
 static const TbindCheckbox<TdeinterlacePageKernel> chb[]=
  {
   IDC_CHB_KERNELDEINT_SHARP,IDFF_kernelDeintSharp,NULL,
   IDC_CHB_KERNELDEINT_TWOWAY,IDFF_kernelDeintTwoway,NULL,
   IDC_CHB_KERNELDEINT_LINKED,IDFF_kernelDeintLinked,NULL,
   IDC_CHB_KERNELDEINT_MAP,IDFF_kernelDeintMap,NULL,
   0
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TdeinterlacePageKernel> htbr[]=
  {
   IDC_TBR_KERNELDEINT_THRESHOLD,IDFF_kernelDeintThreshold,&TdeinterlacePageKernel::cfg2dlg,
   0
  };
 bindHtracks(htbr);
 createDialog(dialogId,parent->m_hwnd);
}
TdeinterlacePageKernel::~TdeinterlacePageKernel()
{
 DestroyWindow(m_hwnd);
}

void TdeinterlacePageKernel::init(void)
{
 translate();
 tbrSetRange(IDC_TBR_KERNELDEINT_THRESHOLD,0,255,10);
}

void TdeinterlacePageKernel::cfg2dlg(void)
{
 tbrSet(IDC_TBR_KERNELDEINT_THRESHOLD,cfgGet(IDFF_kernelDeintThreshold),IDC_LBL_KERNELDEINT_THRESHOLD);
 setCheck(IDC_CHB_KERNELDEINT_SHARP,cfgGet(IDFF_kernelDeintSharp));
 setCheck(IDC_CHB_KERNELDEINT_TWOWAY,cfgGet(IDFF_kernelDeintTwoway));
 setCheck(IDC_CHB_KERNELDEINT_LINKED,cfgGet(IDFF_kernelDeintLinked));
 setCheck(IDC_CHB_KERNELDEINT_MAP,cfgGet(IDFF_kernelDeintMap));
}

void TdeinterlacePageKernel::reset(void)
{
 deci->resetParam(IDFF_kernelDeintThreshold);
 deci->resetParam(IDFF_kernelDeintSharp);
 deci->resetParam(IDFF_kernelDeintTwoway);
 deci->resetParam(IDFF_kernelDeintMap);
 deci->resetParam(IDFF_kernelDeintLinked);
}
