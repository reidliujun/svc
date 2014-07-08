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
#include "Csharpen.h"

void TsharpenPage::init(void)
{
 tbrSetRange(IDC_TBR_SHARPEN1,0,2,1);
 tbrSetRange(IDC_TBR_SHARPEN2,0,2,1);
 tbrSetRange(IDC_TBR_SHARPEN3,0,2,1);
}

void TsharpenPage::cfg2dlg(void)
{
 int sharpenMethod=cfgGet(IDFF_sharpenMethod);
 switch (sharpenMethod)
  {
   case 0:idff1=IDFF_xsharp_strength;
          idff2=IDFF_xsharp_threshold;
          idff3=0;
          break;
   case 1:idff1=IDFF_unsharp_strength;
          idff2=IDFF_unsharp_threshold;
          idff3=0;
          break;
   case 2:idff1=IDFF_msharp_strength;
          idff2=IDFF_msharp_threshold;
          idff3=0;
          break;
   case 4:idff1=IDFF_asharpT;
          idff2=IDFF_asharpD;
          idff3=IDFF_asharpB;
          break;
   case 5:idff1=IDFF_mplayerSharpLuma;
          idff2=IDFF_mplayerSharpChroma;
          idff3=0;
          break;
  }

 xsharpen2dlg(sharpenMethod);
 unsharp2dlg(sharpenMethod);
 msharpen2dlg(sharpenMethod);
 asharp2dlg(sharpenMethod);
 mplayer2dlg(sharpenMethod);
}
void TsharpenPage::xsharpen2dlg(int sharpenMethod)
{
 setCheck(IDC_RBT_SHARPEN_XSHARPEN,sharpenMethod==0);
 if (sharpenMethod==0)
  {
   enable(1,IDC_LBL_SHARPEN2);enable(1,IDC_TBR_SHARPEN2);
   enable(0,IDC_LBL_SHARPEN3);enable(0,IDC_TBR_SHARPEN3);
   tbrSetRange(IDC_TBR_SHARPEN1,1,127,16);tbrSet(IDC_TBR_SHARPEN1,cfgGet(IDFF_xsharp_strength ),IDC_LBL_SHARPEN1);
   tbrSetRange(IDC_TBR_SHARPEN2,0,255,16);tbrSet(IDC_TBR_SHARPEN2,cfgGet(IDFF_xsharp_threshold),IDC_LBL_SHARPEN2);
  }
}
void TsharpenPage::unsharp2dlg(int sharpenMethod)
{
 setCheck(IDC_RBT_SHARPEN_UNSHARPMASK,sharpenMethod==1);
 if (sharpenMethod==1)
  {
   enable(0,IDC_LBL_SHARPEN2);enable(0,IDC_TBR_SHARPEN2);
   enable(0,IDC_LBL_SHARPEN3);enable(0,IDC_TBR_SHARPEN3);
   tbrSetRange(IDC_TBR_SHARPEN1,1,127,16);tbrSet(IDC_TBR_SHARPEN1,cfgGet(IDFF_unsharp_strength),IDC_LBL_SHARPEN1);
  }
}
void TsharpenPage::msharpen2dlg(int sharpenMethod)
{
 setCheck(IDC_RBT_SHARPEN_MSHARPEN,sharpenMethod==2);
 if (sharpenMethod==2)
  {
   enable(1,IDC_LBL_SHARPEN2);enable(1,IDC_TBR_SHARPEN2);
   enable(0,IDC_LBL_SHARPEN3);enable(0,IDC_TBR_SHARPEN3);
   tbrSetRange(IDC_TBR_SHARPEN1,1,127,16);tbrSet(IDC_TBR_SHARPEN1,cfgGet(IDFF_msharp_strength ),IDC_LBL_SHARPEN1);
   tbrSetRange(IDC_TBR_SHARPEN2,0,255,16);tbrSet(IDC_TBR_SHARPEN2,cfgGet(IDFF_msharp_threshold),IDC_LBL_SHARPEN2);
  }
 enable(sharpenMethod==2,IDC_CHB_SHARPEN_MSHARPEN_MASK);setCheck(IDC_CHB_SHARPEN_MSHARPEN_MASK,cfgGet(IDFF_msharpMask));
 enable(sharpenMethod==2,IDC_CHB_SHARPEN_MSHARPEN_HQ  );setCheck(IDC_CHB_SHARPEN_MSHARPEN_HQ  ,cfgGet(IDFF_msharpHQ  ));
}
void TsharpenPage::asharp2dlg(int sharpenMethod)
{
 setCheck(IDC_RBT_SHARPEN_ASHARP,sharpenMethod==4);
 if (sharpenMethod==4)
  {
   enable(1,IDC_LBL_SHARPEN2);enable(1,IDC_TBR_SHARPEN2);
   enable(1,IDC_LBL_SHARPEN3);enable(1,IDC_TBR_SHARPEN3);
   tbrSetRange(IDC_TBR_SHARPEN1,-100,3200);tbrSet(IDC_TBR_SHARPEN1,cfgGet(IDFF_asharpT),IDC_LBL_SHARPEN1,_(IDC_LBL_SHARPEN1,_l("Unsharp masking threshold:")),100);
   tbrSetRange(IDC_TBR_SHARPEN2,   0,1600);tbrSet(IDC_TBR_SHARPEN2,cfgGet(IDFF_asharpD),IDC_LBL_SHARPEN2,_(IDC_LBL_SHARPEN2,_l("Adaptive sharpenning strength:")),100);
   tbrSetRange(IDC_TBR_SHARPEN3,   0,400 );tbrSet(IDC_TBR_SHARPEN3,cfgGet(IDFF_asharpB),IDC_LBL_SHARPEN3,NULL,100);
  }
 enable(sharpenMethod==4,IDC_CHB_SHARPEN_ASHARP_HQBF);setCheck(IDC_CHB_SHARPEN_ASHARP_HQBF,cfgGet(IDFF_asharpHQBF));
}
void TsharpenPage::mplayer2dlg(int sharpenMethod)
{
 setCheck(IDC_RBT_SHARPEN_MPLAYER,sharpenMethod==5);
 if (sharpenMethod==5)
  {
   enable(1,IDC_LBL_SHARPEN2);enable(1,IDC_TBR_SHARPEN2);
   enable(0,IDC_LBL_SHARPEN3);enable(0,IDC_TBR_SHARPEN3);
   tbrSetRange(IDC_TBR_SHARPEN1,0,400,16);tbrSet(IDC_TBR_SHARPEN1,cfgGet(IDFF_mplayerSharpLuma  ),IDC_LBL_SHARPEN1,_(IDC_LBL_SHARPEN1,_l("Luminance sharpening:")),100);
   tbrSetRange(IDC_TBR_SHARPEN2,0,400,16);tbrSet(IDC_TBR_SHARPEN2,cfgGet(IDFF_mplayerSharpChroma),IDC_LBL_SHARPEN2,_(IDC_LBL_SHARPEN2,_l("Chroma sharpening:")),100);
  }
}

INT_PTR TsharpenPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_SHARPEN1:
      case IDC_TBR_SHARPEN2:
      case IDC_TBR_SHARPEN3:
       {
        if (idff1) cfgSet(idff1,tbrGet(IDC_TBR_SHARPEN1));
        if (idff2) cfgSet(idff2,tbrGet(IDC_TBR_SHARPEN2));
        if (idff3) cfgSet(idff3,tbrGet(IDC_TBR_SHARPEN3));
        switch (cfgGet(IDFF_sharpenMethod))
         {
          case 0:xsharpen2dlg(0);break;
          case 1:unsharp2dlg(1);break;
          case 2:msharpen2dlg(2);break;
          case 4:asharp2dlg(4);break;
          case 5:mplayer2dlg(5);break;
         }
        return TRUE;
       }
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

Twidget* TsharpenPage::createDlgItem(int id,HWND h)
{
 if (id==IDC_TBR_SHARPEN1 || id==IDC_TBR_SHARPEN2 || id==IDC_TBR_SHARPEN3)
  return new TwidgetSubclassTbr(h,this,NULL);
 else
  return TconfPageDecVideo::createDlgItem(id,h);
}

int TsharpenPage::getTbrIdff(int id,const TbindTrackbars bind)
{
 switch (id)
  {
   case IDC_TBR_SHARPEN1:return idff1;
   case IDC_TBR_SHARPEN2:return idff2;
   case IDC_TBR_SHARPEN3:return idff3;
   default:return 0;
  }
}

TsharpenPage::TsharpenPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_SHARPEN;
 static const TbindCheckbox<TsharpenPage> chb[]=
  {
   IDC_CHB_SHARPEN_MSHARPEN_MASK,IDFF_msharpMask,NULL,
   IDC_CHB_SHARPEN_MSHARPEN_HQ,IDFF_msharpHQ,NULL,
   IDC_CHB_SHARPEN_ASHARP_HQBF,IDFF_asharpHQBF,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindRadiobutton<TsharpenPage> rbt[]=
  {
   IDC_RBT_SHARPEN_XSHARPEN,IDFF_sharpenMethod,0,&TsharpenPage::cfg2dlg,
   IDC_RBT_SHARPEN_UNSHARPMASK,IDFF_sharpenMethod,1,&TsharpenPage::cfg2dlg,
   IDC_RBT_SHARPEN_MSHARPEN,IDFF_sharpenMethod,2,&TsharpenPage::cfg2dlg,
   IDC_RBT_SHARPEN_ASHARP,IDFF_sharpenMethod,4,&TsharpenPage::cfg2dlg,
   IDC_RBT_SHARPEN_MPLAYER,IDFF_sharpenMethod,5,&TsharpenPage::cfg2dlg,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
}
