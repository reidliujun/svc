/*
 * Copyright (c) 2006 Milan Cutka
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
#include "CgradFun.h"

void TgradFunPage::init(void)
{
 tbrSetRange(IDC_TBR_GRADFUN_THRESHOLD,101,2000);
}

void TgradFunPage::cfg2dlg(void)
{
 tbrSet(IDC_TBR_GRADFUN_THRESHOLD,cfgGet(IDFF_gradFunThreshold),IDC_LBL_GRADFUN_THRESHOLD,NULL,100.0f);
}

TgradFunPage::TgradFunPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_GRADFUN;
 static const TbindTrackbar<TgradFunPage> htbr[]=
  {
   IDC_TBR_GRADFUN_THRESHOLD,IDFF_gradFunThreshold,&TgradFunPage::cfg2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
}
