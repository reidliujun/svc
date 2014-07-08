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
#include "Cwarpsharp.h"
#include "TwarpsharpSettings.h"

void TwarpsharpPage::init(void)
{
 tbrSetRange(IDC_TBR_SHARPEN3,0,2,1);
#ifdef __GNUC__
 enable(0,IDC_RBT_WARPSHARP_AWARPSHARP);
#endif
}
void TwarpsharpPage::cfg2dlg(void)
{
 int warpsharpMethod=cfgGet(IDFF_warpsharpMethod);
 warpsharp2dlg(warpsharpMethod);
 awarpsharp2dlg(warpsharpMethod);
}
void TwarpsharpPage::warpsharp2dlg(int sharpenMethod)
{
 setCheck(IDC_RBT_WARPSHARP_WARPSHARP,sharpenMethod==0);
 if (sharpenMethod==0)
  {
   enable(0,IDC_LBL_SHARPEN3);enable(0,IDC_TBR_SHARPEN3);
   tbrSetRange(IDC_TBR_SHARPEN1,0,255,16);tbrSet(IDC_TBR_SHARPEN1,cfgGet(IDFF_warpsharpDepth    ),IDC_LBL_SHARPEN1);
   tbrSetRange(IDC_TBR_SHARPEN2,0,255,16);tbrSet(IDC_TBR_SHARPEN2,cfgGet(IDFF_warpsharpThreshold),IDC_LBL_SHARPEN2);
  }
}
void TwarpsharpPage::awarpsharp2dlg(int sharpenMethod)
{
 setCheck(IDC_RBT_WARPSHARP_AWARPSHARP,sharpenMethod==1);
 if (sharpenMethod==1)
  {
   enable(1,IDC_LBL_SHARPEN3);enable(1,IDC_TBR_SHARPEN3);
   tbrSetRange(IDC_TBR_SHARPEN1,0,6400,16);tbrSet(IDC_TBR_SHARPEN1,cfgGet(IDFF_awarpsharpDepth ),IDC_LBL_SHARPEN1,NULL,100.0f);
   tbrSetRange(IDC_TBR_SHARPEN2,0,99  , 5);tbrSet(IDC_TBR_SHARPEN2,cfgGet(IDFF_awarpsharpThresh),IDC_LBL_SHARPEN2,NULL,100.0f);
   tbrSetRange(IDC_TBR_SHARPEN3,0,4   , 1);tbrSet(IDC_TBR_SHARPEN3,cfgGet(IDFF_awarpsharpBlur  ),IDC_LBL_SHARPEN3);
  }
 cbxSetCurSel(IDC_CBX_WARPSHARP_AWARPSHARP_CM,cfgGet(IDFF_awarpsharpCM));
 cbxSetCurSel(IDC_CBX_WARPSHARP_AWARPSHARP_BM,cfgGet(IDFF_awarpsharpBM));
 static const int idAwarpsharp[]={IDC_LBL_WARPSHARP_AWARPSHARP_CM,IDC_CBX_WARPSHARP_AWARPSHARP_CM,IDC_LBL_WARPSHARP_AWARPSHARP_BM,IDC_CBX_WARPSHARP_AWARPSHARP_BM,0};
 enable(sharpenMethod==1,idAwarpsharp);
}
void TwarpsharpPage::awarpsharp2dlg1(void)
{
 awarpsharp2dlg(1);
}

INT_PTR TwarpsharpPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_SHARPEN1:
      case IDC_TBR_SHARPEN2:
      case IDC_TBR_SHARPEN3:
       switch (cfgGet(IDFF_warpsharpMethod))
        {
         case 0:cfgSet(IDFF_warpsharpDepth    ,tbrGet(IDC_TBR_SHARPEN1));
                cfgSet(IDFF_warpsharpThreshold,tbrGet(IDC_TBR_SHARPEN2));
                warpsharp2dlg(0);
                break;
         case 1:cfgSet(IDFF_awarpsharpDepth ,tbrGet(IDC_TBR_SHARPEN1));
                cfgSet(IDFF_awarpsharpThresh,tbrGet(IDC_TBR_SHARPEN2));
                cfgSet(IDFF_awarpsharpBlur  ,tbrGet(IDC_TBR_SHARPEN3));
                awarpsharp2dlg(1);
                break;
        }
       return TRUE;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

Twidget* TwarpsharpPage::createDlgItem(int id,HWND h)
{
 if (id==IDC_TBR_SHARPEN1 || id==IDC_TBR_SHARPEN1 || id==IDC_TBR_SHARPEN1)
  return new TwidgetSubclassTbr(h,this,NULL);
 else
  return TconfPageDecVideo::createDlgItem(id,h);
}
int TwarpsharpPage::getTbrIdff(int id,const TbindTrackbars bind)
{
 int mode=cfgGet(IDFF_warpsharpMethod);
 switch (id)
  {
   case IDC_TBR_SHARPEN1:return mode==0?IDFF_warpsharpDepth:IDFF_awarpsharpDepth;
   case IDC_TBR_SHARPEN2:return mode==0?IDFF_warpsharpThreshold:IDFF_awarpsharpThresh;
   case IDC_TBR_SHARPEN3:return mode==0?0:IDFF_awarpsharpBlur;
   default:return 0;
  }
}

void TwarpsharpPage::translate(void)
{
 TconfPageDec::translate();

 cbxTranslate(IDC_CBX_WARPSHARP_AWARPSHARP_CM,TwarpsharpSettings::cms);
 cbxTranslate(IDC_CBX_WARPSHARP_AWARPSHARP_BM,TwarpsharpSettings::bms);
}

TwarpsharpPage::TwarpsharpPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_WARPSHARP;
 helpURL=_l("Warpsharp.html");
 static const TbindRadiobutton<TwarpsharpPage> rbt[]=
  {
   IDC_RBT_WARPSHARP_WARPSHARP,IDFF_warpsharpMethod,0,&TwarpsharpPage::cfg2dlg,
   IDC_RBT_WARPSHARP_AWARPSHARP,IDFF_warpsharpMethod,1,&TwarpsharpPage::cfg2dlg,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindCombobox<TwarpsharpPage> cbx[]=
  {
   IDC_CBX_WARPSHARP_AWARPSHARP_CM,IDFF_awarpsharpCM,BINDCBX_SEL,&TwarpsharpPage::awarpsharp2dlg1,
   IDC_CBX_WARPSHARP_AWARPSHARP_BM,IDFF_awarpsharpBM,BINDCBX_SEL,&TwarpsharpPage::awarpsharp2dlg1,
   0
  };
 bindComboboxes(cbx);
}
