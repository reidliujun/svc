/*
 * Copyright (c) 2002-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Cin.h"
#include "TffProc.h"
#include "ffdshow_mediaguids.h"

void TinPage::init(void)
{
 if (!ffproc)
  deciE->getFFproc(&ffproc);
}

void TinPage::cfg2dlg(void)
{
 in2dlg();
 incsp2dlg();
 interlaced2dlg();
 lavcnr2dlg();
}

void TinPage::in2dlg(void)
{
 setCheck(IDC_CHB_PROC,cfgGet(IDFF_enc_isProc));
 setCheck(IDC_CHB_FLIP,cfgGet(IDFF_enc_flip));
 static const int idProc[]={IDC_CHB_PROC,IDC_BT_PROC_CONFIG,0};
 enable(ffproc?1:0,idProc);
}
void TinPage::incsp2dlg(void)
{
 setCheck(IDC_CHB_FORCEINCSP,cfgGet(IDFF_enc_forceIncsp));enable(cfgGet(IDFF_enc_forceIncsp),IDC_CBX_INCSP);
 cbxSetDataCurSel(IDC_CBX_INCSP,cfgGet(IDFF_enc_incsp));
}
void TinPage::interlaced2dlg(void)
{
 setCheck(IDC_CHB_DY_INTERLACED,cfgGet(IDFF_isDyInterlaced));
 SetDlgItemInt(m_hwnd,IDC_ED_DY_INTERLACED,cfgGet(IDFF_dyInterlaced),FALSE);
 enable(cfgGet(IDFF_isDyInterlaced),IDC_ED_DY_INTERLACED);
}
void TinPage::lavcnr2dlg(void)
{
 int is=sup_lavcQuant(codecId) || codecId==CODEC_ID_X264;
 static const int idLavcNR[]={IDC_LBL_LAVC_NR,IDC_ED_LAVC_NR,0};
 int is_lavc_nr=cfgGet(IDFF_enc_is_lavc_nr);
 setCheck(IDC_CHB_LAVC_NR,is_lavc_nr);enable(is,IDC_CHB_LAVC_NR);
 SetDlgItemInt(m_hwnd,IDC_ED_LAVC_NR,cfgGet(IDFF_enc_lavc_nr),FALSE);
 enable(is_lavc_nr && is,idLavcNR);
}

void TinPage::onProcConfig(void)
{
 ffproc->config(m_hwnd);
}

void TinPage::translate(void)
{
 TconfPageEnc::translate();

 int ii=cbxGetCurSel(IDC_CBX_INCSP);
 cbxClear(IDC_CBX_INCSP);
 cbxAdd(IDC_CBX_INCSP,_(IDC_CBX_INCSP,_l("all YUV")),FOURCC_MASK_YUV);
 cbxAdd(IDC_CBX_INCSP,_(IDC_CBX_INCSP,_l("all RGB")),FOURCC_MASK_RGB);
 cbxAdd(IDC_CBX_INCSP,_(IDC_CBX_INCSP,_l("8-bit palletized")),FOURCC_MASK_PAL);
 for (int i=0;cspFccs[i].name;i++)
  cbxAdd(IDC_CBX_INCSP,cspFccs[i].name,cspFccs[i].fcc);
 cbxSetCurSel(IDC_CBX_INCSP,ii);
}

TinPage::TinPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_IN;
 ffproc=NULL;
 static const int props[]={IDFF_enc_flip,IDFF_enc_forceIncsp,IDFF_enc_incsp,IDFF_enc_isProc,IDFF_enc_is_lavc_nr,IDFF_enc_lavc_nr,0};
 propsIDs=props;
 static const TbindCheckbox<TinPage> chb[]=
  {
   IDC_CHB_PROC,IDFF_enc_isProc,NULL,
   IDC_CHB_FLIP,IDFF_enc_flip,NULL,
   IDC_CHB_FORCEINCSP,IDFF_enc_forceIncsp,&TinPage::incsp2dlg,
   IDC_CHB_DY_INTERLACED,IDFF_isDyInterlaced,&TinPage::interlaced2dlg,
   IDC_CHB_LAVC_NR,IDFF_enc_is_lavc_nr,&TinPage::lavcnr2dlg,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditInt<TinPage> edInt[]=
  {
   IDC_ED_DY_INTERLACED,0,4096,IDFF_dyInterlaced,NULL,
   IDC_ED_LAVC_NR,0,10000,IDFF_enc_lavc_nr,NULL,
   0
  };
 bindEditInts(edInt);
 static const TbindCombobox<TinPage> cbx[]=
  {
   IDC_CBX_INCSP,IDFF_enc_incsp,BINDCBX_DATA,NULL,
   0
  };
 bindComboboxes(cbx);
 static const TbindButton<TinPage> bt[]=
  {
   IDC_BT_PROC_CONFIG,&TinPage::onProcConfig,
   0,NULL
  };
 bindButtons(bt);
}
