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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Ccrystality.h"

void TcrystalityPage::init(void)
{
 tbrSetRange(IDC_TBR_CRYSTALITY_BEXT_LEVEL     ,0,100);
 tbrSetRange(IDC_TBR_CRYSTALITY_ECHO_LEVEL     ,0,100);
 tbrSetRange(IDC_TBR_CRYSTALITY_STEREO_LEVEL   ,0,100);
 tbrSetRange(IDC_TBR_CRYSTALITY_FILTER_LEVEL   ,1,  3);
 tbrSetRange(IDC_TBR_CRYSTALITY_FEEDBACK_LEVEL ,0,100);
 tbrSetRange(IDC_TBR_CRYSTALITY_HARMONICS_LEVEL,0,100);
}

void TcrystalityPage::cfg2dlg(void)
{
 tbrSet(IDC_TBR_CRYSTALITY_BEXT_LEVEL     ,cfgGet(IDFF_crystality_bext_level     ),IDC_LBL_CRYSTALITY_BEXT_LEVEL);
 tbrSet(IDC_TBR_CRYSTALITY_ECHO_LEVEL     ,cfgGet(IDFF_crystality_echo_level     ),IDC_LBL_CRYSTALITY_ECHO_LEVEL);
 tbrSet(IDC_TBR_CRYSTALITY_STEREO_LEVEL   ,cfgGet(IDFF_crystality_stereo_level   ),IDC_LBL_CRYSTALITY_STEREO_LEVEL);
 tbrSet(IDC_TBR_CRYSTALITY_FILTER_LEVEL   ,cfgGet(IDFF_crystality_filter_level   ),IDC_LBL_CRYSTALITY_FILTER_LEVEL);
 tbrSet(IDC_TBR_CRYSTALITY_FEEDBACK_LEVEL ,cfgGet(IDFF_crystality_feedback_level ),IDC_LBL_CRYSTALITY_FEEDBACK_LEVEL);
 tbrSet(IDC_TBR_CRYSTALITY_HARMONICS_LEVEL,cfgGet(IDFF_crystality_harmonics_level),IDC_LBL_CRYSTALITY_HARMONICS_LEVEL);
}

TcrystalityPage::TcrystalityPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecAudio(Iparent,idff)
{
 resInter=IDC_CHB_CRYSTALITY;
 static const TbindTrackbar<TcrystalityPage> htbr[]=
  {
   IDC_TBR_CRYSTALITY_BEXT_LEVEL,IDFF_crystality_bext_level,&TcrystalityPage::cfg2dlg,
   IDC_TBR_CRYSTALITY_ECHO_LEVEL,IDFF_crystality_echo_level,&TcrystalityPage::cfg2dlg,
   IDC_TBR_CRYSTALITY_STEREO_LEVEL,IDFF_crystality_stereo_level,&TcrystalityPage::cfg2dlg,
   IDC_TBR_CRYSTALITY_FILTER_LEVEL,IDFF_crystality_filter_level,&TcrystalityPage::cfg2dlg,
   IDC_TBR_CRYSTALITY_FEEDBACK_LEVEL,IDFF_crystality_feedback_level,&TcrystalityPage::cfg2dlg,
   IDC_TBR_CRYSTALITY_HARMONICS_LEVEL,IDFF_crystality_harmonics_level,&TcrystalityPage::cfg2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
}

