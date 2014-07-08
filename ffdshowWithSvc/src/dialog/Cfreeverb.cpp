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
#include "Cfreeverb.h"

void TfreeverbPage::init(void)
{
 tbrSetRange(IDC_TBR_FREEVERB_ROOMSIZE,0,1000);
 tbrSetRange(IDC_TBR_FREEVERB_DAMPING,0,1000);
 tbrSetRange(IDC_TBR_FREEVERB_WET,0,1000);
 tbrSetRange(IDC_TBR_FREEVERB_DRY,0,1000);
 tbrSetRange(IDC_TBR_FREEVERB_WIDTH,0,1000);
}

void TfreeverbPage::cfg2dlg(void)
{
 tbrSet(IDC_TBR_FREEVERB_ROOMSIZE,cfgGet(IDFF_freeverbRoomsize),IDC_LBL_FREEVERB_ROOMSIZE,NULL,1000.0);
 tbrSet(IDC_TBR_FREEVERB_DAMPING ,cfgGet(IDFF_freeverbDamp    ),IDC_LBL_FREEVERB_DAMPING ,NULL,1000.0);
 tbrSet(IDC_TBR_FREEVERB_WET     ,cfgGet(IDFF_freeverbWet     ),IDC_LBL_FREEVERB_WET     ,NULL,1000.0);
 tbrSet(IDC_TBR_FREEVERB_DRY     ,cfgGet(IDFF_freeverbDry     ),IDC_LBL_FREEVERB_DRY     ,NULL,1000.0);
 tbrSet(IDC_TBR_FREEVERB_WIDTH   ,cfgGet(IDFF_freeverbWidth   ),IDC_LBL_FREEVERB_WIDTH   ,NULL,1000.0);
}

TfreeverbPage::TfreeverbPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecAudio(Iparent,idff)
{
 resInter=IDC_CHB_FREEVERB;
 static const TbindTrackbar<TfreeverbPage> htbr[]=
  {
   IDC_TBR_FREEVERB_ROOMSIZE,IDFF_freeverbRoomsize,&TfreeverbPage::cfg2dlg,
   IDC_TBR_FREEVERB_DAMPING,IDFF_freeverbDamp,&TfreeverbPage::cfg2dlg,
   IDC_TBR_FREEVERB_WET,IDFF_freeverbWet,&TfreeverbPage::cfg2dlg,
   IDC_TBR_FREEVERB_DRY,IDFF_freeverbDry,&TfreeverbPage::cfg2dlg,
   IDC_TBR_FREEVERB_WIDTH,IDFF_freeverbWidth,&TfreeverbPage::cfg2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
}
