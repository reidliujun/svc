/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include "Coffset.h"

void ToffsetPage::init(void)
{
 tbrSetRange(IDC_TBR_OFFSETY_X ,-32,32,4);
 tbrSetRange(IDC_TBR_OFFSETY_Y ,-32,32,4);
 tbrSetRange(IDC_TBR_OFFSETUV_X,-32,32,4);
 tbrSetRange(IDC_TBR_OFFSETUV_Y,-32,32,4);
}

void ToffsetPage::cfg2dlg(void)
{
 offset2dlg();
 setCheck(IDC_CHB_TRANSFORM_FLIP,cfgGet(IDFF_transfFlip));
 setCheck(IDC_CHB_TRANSFORM_MIRROR,cfgGet(IDFF_transfMirror));
}

void ToffsetPage::offset2dlg(void)
{
 tbrSet(IDC_TBR_OFFSETY_X,cfgGet(IDFF_offsetY_X),IDC_LBL_OFFSETY_X);
 tbrSet(IDC_TBR_OFFSETY_Y,cfgGet(IDFF_offsetY_Y),IDC_LBL_OFFSETY_Y);
 tbrSet(IDC_TBR_OFFSETUV_X,cfgGet(IDFF_offsetU_X),IDC_LBL_OFFSETUV_X);
 tbrSet(IDC_TBR_OFFSETUV_Y,cfgGet(IDFF_offsetU_Y),IDC_LBL_OFFSETUV_Y);
}

INT_PTR ToffsetPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_OFFSETUV_X:
      case IDC_TBR_OFFSETUV_Y:
       cfgSet(IDFF_offsetU_X,tbrGet(IDC_TBR_OFFSETUV_X));
       cfgSet(IDFF_offsetU_Y,tbrGet(IDC_TBR_OFFSETUV_Y));
       cfgSet(IDFF_offsetV_X,tbrGet(IDC_TBR_OFFSETUV_X));
       cfgSet(IDFF_offsetV_Y,tbrGet(IDC_TBR_OFFSETUV_Y));
       offset2dlg();
       return TRUE;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

ToffsetPage::ToffsetPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_OFFSET;
 static const TbindTrackbar<ToffsetPage> htbr[]=
  {
   IDC_TBR_OFFSETY_X,IDFF_offsetY_X,&ToffsetPage::offset2dlg,
   IDC_TBR_OFFSETY_Y,IDFF_offsetY_Y,&ToffsetPage::offset2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindCheckbox<ToffsetPage> chb[]=
  {
   IDC_CHB_TRANSFORM_FLIP,IDFF_transfFlip,NULL,
   IDC_CHB_TRANSFORM_MIRROR,IDFF_transfMirror,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
}
