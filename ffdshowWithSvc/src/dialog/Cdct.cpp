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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Cdct.h"
#include "CquantTables.h"

const int TdctPage::eds[]=
{
 IDC_ED_DCT0,
 IDC_ED_DCT1,
 IDC_ED_DCT2,
 IDC_ED_DCT3,
 IDC_ED_DCT4,
 IDC_ED_DCT5,
 IDC_ED_DCT6,
 IDC_ED_DCT7
};
const int TdctPage::idffs[]=
{
 IDFF_dct0,
 IDFF_dct1,
 IDFF_dct2,
 IDFF_dct3,
 IDFF_dct4,
 IDFF_dct5,
 IDFF_dct6,
 IDFF_dct7
};

void TdctPage::init(void)
{
 tbrSetRange(IDC_TBR_DCT_QUANT,1,31,3);
}

void TdctPage::cfg2dlg(void)
{
 int mode=cfgGet(IDFF_dctMode);
 setCheck(IDC_RBT_DCT_COEFFS,mode==0);coeffs2dlg();
 setCheck(IDC_RBT_DCT_H263  ,mode==1);
 setCheck(IDC_RBT_DCT_MPEG  ,mode==2);
 quant2dlg();
}

void TdctPage::coeffs2dlg(void)
{
 for (int i=0;i<8;i++)
  setText(eds[i],_l("%.2f"),float(cfgGet(idffs[i])/1000.0));
}
void TdctPage::quant2dlg(void)
{
 tbrSet(IDC_TBR_DCT_QUANT,cfgGet(IDFF_dctQuant),IDC_LBL_DCT_QUANT);
}

void TdctPage::onMatrix(void)
{
 TdctQuantDlg dlg(m_hwnd,deci);
 dlg.show();
}

TdctPage::TdctPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_DCT;
 static const TbindEditReal<TdctPage> edReal[]=
  {
   IDC_ED_DCT0,0.0,3.0,IDFF_dct0,1000.0,NULL,
   IDC_ED_DCT1,0.0,3.0,IDFF_dct1,1000.0,NULL,
   IDC_ED_DCT2,0.0,3.0,IDFF_dct2,1000.0,NULL,
   IDC_ED_DCT3,0.0,3.0,IDFF_dct3,1000.0,NULL,
   IDC_ED_DCT4,0.0,3.0,IDFF_dct4,1000.0,NULL,
   IDC_ED_DCT5,0.0,3.0,IDFF_dct5,1000.0,NULL,
   IDC_ED_DCT6,0.0,3.0,IDFF_dct6,1000.0,NULL,
   IDC_ED_DCT7,0.0,3.0,IDFF_dct7,1000.0,NULL,
   0
  };
 bindEditReals(edReal);
 static const TbindRadiobutton<TdctPage> rbt[]=
  {
   IDC_RBT_DCT_COEFFS,IDFF_dctMode,0,NULL,
   IDC_RBT_DCT_H263  ,IDFF_dctMode,1,NULL,
   IDC_RBT_DCT_MPEG  ,IDFF_dctMode,2,NULL,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindTrackbar<TdctPage> htbr[]=
  {
   IDC_TBR_DCT_QUANT,IDFF_dctQuant,&TdctPage::quant2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindButton<TdctPage> bt[]=
  {
   IDC_BT_DCT_MPEG,&TdctPage::onMatrix,
   0,NULL
  };
 bindButtons(bt);
}

