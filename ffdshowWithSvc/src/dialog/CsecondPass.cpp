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
#include "CsecondPass.h"
#include "TperfectDlg.h"
#include "Tlibavcodec.h"

void TsecondPassPage::init(void)
{
 perfectDlg=NULL;
}

bool TsecondPassPage::enabled(void)
{
 return sup_perFrameQuant(codecId) && (cfgGet(IDFF_enc_mode)==ENC_MODE::PASS2_2_EXT || cfgGet(IDFF_enc_mode)==ENC_MODE::PASS2_2_INT);
}

void TsecondPassPage::cfg2dlg(void)
{
 setDlgItemText(m_hwnd,IDC_ED_STATS1,cfgGetStr(IDFF_enc_stats1flnm));
 setDlgItemText(m_hwnd,IDC_ED_STATS2,cfgGetStr(IDFF_enc_stats2flnm));
 SetDlgItemInt(m_hwnd,IDC_ED_MAX_BITRATE,cfgGet(IDFF_enc_twopass_max_bitrate)/1000,FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_MAX_OVERFLOW_IMPR,cfgGet(IDFF_enc_twopass_max_overflow_improvement),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_MAX_OVERFLOW_DEGR,cfgGet(IDFF_enc_twopass_max_overflow_degradation),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_KFBOOST,cfgGet(IDFF_enc_keyframe_boost),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_KFTRESHOLD,cfgGet(IDFF_enc_kftreshold),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_KFREDUCTION,cfgGet(IDFF_enc_kfreduction),FALSE);
 static const int idStats2[]={IDC_LBL_STATS2,IDC_ED_STATS2,IDC_BT_STATS2,0};
 enable(cfgGet(IDFF_enc_mode)==ENC_MODE::PASS2_2_EXT,idStats2);
 setCheck(IDC_CHB_XVID2PASS_USE_WRITE,cfgGet(IDFF_enc_xvid2pass_use_write));
}
INT_PTR TsecondPassPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    if (perfectDlg) delete perfectDlg;perfectDlg=NULL;
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_STATS1:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t stats1flnm[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_STATS1,stats1flnm,MAX_PATH);
         cfgSet(IDFF_enc_stats1flnm,stats1flnm);
         return TRUE;
        }
       break;
      case IDC_ED_STATS2:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t stats2flnm[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_STATS2,stats2flnm,MAX_PATH);
         cfgSet(IDFF_enc_stats2flnm,stats2flnm);
         return TRUE;
        }
       break;
     }
    break;
  }
 return TconfPageEnc::msgProc(uMsg,wParam,lParam);
}
void TsecondPassPage::onPerfectDestroy(void)
{
 delete perfectDlg;perfectDlg=NULL;
}
void TsecondPassPage::onStats1(void)
{
 char_t stats1flnm[MAX_PATH];cfgGet(IDFF_enc_stats1flnm,stats1flnm,MAX_PATH);
 if (dlgGetFile(false,m_hwnd,_(-IDD_SECONDPASS,_l("Select stats file with stored first pass statistics")),_l("Stats file (*.stats)\0*.stats\0All files (*.*)\0*.*\0"),_l("stats"),stats1flnm,_l("."),0))
  {
   cfgSet(IDFF_enc_stats1flnm,stats1flnm);
   cfg2dlg();
  }
}
void TsecondPassPage::onStats2(void)
{
 char_t stats2flnm[MAX_PATH];cfgGet(IDFF_enc_stats2flnm,stats2flnm,MAX_PATH);
 if (dlgGetFile(false,m_hwnd,_(-IDD_SECONDPASS,_l("Select stats file processed with external program")),_l("Stats file (*.stats)\0*.stats\0All files (*.*)\0*.*\0"),_l("stats"),stats2flnm,_l("."),0))
  {
   cfgSet(IDFF_enc_stats2flnm,stats2flnm);
   cfg2dlg();
  }
}
void TsecondPassPage::onSimulate(void)
{
 if (!perfectDlg)
  {
   perfectDlg=new TperfectDlg(this);
   perfectDlg->show();
  }
 else
  {
   delete perfectDlg;
   perfectDlg=NULL;
  }
}

TsecondPassPage::TsecondPassPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_SECONDPASS;
 static const int props[]={IDFF_enc_stats1flnm,IDFF_enc_stats2flnm,IDFF_enc_twopass_max_bitrate,IDFF_enc_twopass_max_overflow_improvement,IDFF_enc_twopass_max_overflow_degradation,IDFF_enc_keyframe_boost,IDFF_enc_kftreshold,IDFF_enc_kfreduction,IDFF_enc_xvid2pass_use_write,0};
 propsIDs=props;
 static const TbindCheckbox<TsecondPassPage> chb[]=
  {
   IDC_CHB_XVID2PASS_USE_WRITE,IDFF_enc_xvid2pass_use_write,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditInt<TsecondPassPage> edInt[]=
  {
   IDC_ED_MAX_OVERFLOW_IMPR,0,100,IDFF_enc_twopass_max_overflow_improvement,NULL,
   IDC_ED_MAX_OVERFLOW_DEGR,0,100,IDFF_enc_twopass_max_overflow_degradation,NULL,
   IDC_ED_KFBOOST,0,100,IDFF_enc_keyframe_boost,NULL,
   IDC_ED_KFTRESHOLD,0,10000000,IDFF_enc_kftreshold,NULL,
   IDC_ED_KFREDUCTION,0,100,IDFF_enc_kfreduction,NULL,
   0
  };
 bindEditInts(edInt);
 static const TbindEditReal<TsecondPassPage> edReal[]=
  {
   IDC_ED_MAX_BITRATE,1,10000,IDFF_enc_twopass_max_bitrate,1000,NULL,
   0
  };
 bindEditReals(edReal);
 static const TbindButton<TsecondPassPage> bt[]=
  {
   IDC_BT_STATS1,&TsecondPassPage::onStats1,
   IDC_BT_STATS2,&TsecondPassPage::onStats2,
   IDC_BT_SECOND_SIMULATE,&TsecondPassPage::onSimulate,
   0,NULL
  };
 bindButtons(bt);
}
