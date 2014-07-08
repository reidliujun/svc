/*
 * Copyright (c) 2002-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "CcurveAlt.h"

const char_t* TcurveAltPage::aggressions[]=
{
 _l("Low - cosine curve"),
 _l("Medium - linear"),
 _l("High - sine curve"),
 NULL
};

bool TcurveAltPage::enabled(void)
{
 int mode=cfgGet(IDFF_enc_mode);
 return sup_perFrameQuant(codecId) && (mode==ENC_MODE::PASS2_2_EXT || mode==ENC_MODE::PASS2_2_INT);
}

void TcurveAltPage::cfg2dlg(void)
{
 int is=cfgGet(IDFF_enc_use_alt_curve);
 setCheck(IDC_CHB_USE_ALT_CURVE,is);
 static const int idCurveAlt[]={IDC_LBL_CURVE_TYPE,IDC_CBX_CURVE_TYPE,IDC_LBL_CURVEALT_DISTHIGH,IDC_ED_CURVEALT_DISTHIGH,IDC_LBL_CURVEALT_DISTHIGH_PERCENT,IDC_LBL_CURVEALT_DISTLOW,IDC_ED_CURVEALT_DISTLOW,IDC_LBL_CURVEALT_DISTLOW_PERCENT,IDC_RBT_CURVEALT_USEAUTO,0};
 enable(is,idCurveAlt);

 cbxSetCurSel(IDC_CBX_CURVE_TYPE,cfgGet(IDFF_enc_alt_curve_type));
 SetDlgItemInt(m_hwnd,IDC_ED_CURVEALT_DISTHIGH,cfgGet(IDFF_enc_alt_curve_high_dist),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_CURVEALT_DISTLOW ,cfgGet(IDFF_enc_alt_curve_low_dist ),FALSE);

 setCheck(IDC_RBT_CURVEALT_USEAUTO,cfgGet(IDFF_enc_alt_curve_use_auto));enable(is,IDC_RBT_CURVEALT_USEAUTO);
 static const int idAutoStr[]={IDC_ED_CURVEALT_AUTOSTR,IDC_LBL_CURVEALT_AUTOSTR,0};
 enable(is && cfgGet(IDFF_enc_alt_curve_use_auto),idAutoStr);
 SetDlgItemInt(m_hwnd,IDC_ED_CURVEALT_AUTOSTR,cfgGet(IDFF_enc_alt_curve_auto_str),FALSE);

 setCheck(IDC_RBT_CURVEALT_USEMANUAL,!cfgGet(IDFF_enc_alt_curve_use_auto));enable(is,IDC_RBT_CURVEALT_USEMANUAL);
 static const int idManualStr[]={IDC_ED_CURVEALT_MINRELQUAL,IDC_LBL_CURVEALT_MINRELQUAL,0};
 enable(is && !cfgGet(IDFF_enc_alt_curve_use_auto),idManualStr);
 SetDlgItemInt(m_hwnd,IDC_ED_CURVEALT_MINRELQUAL,cfgGet(IDFF_enc_alt_curve_min_rel_qual),FALSE);

 setCheck(IDC_CHB_CURVEALT_BONUSBIASMANUAL,!cfgGet(IDFF_enc_alt_curve_use_auto_bonus_bias));enable(is,IDC_CHB_CURVEALT_BONUSBIASMANUAL);
 enable(is && !cfgGet(IDFF_enc_alt_curve_use_auto_bonus_bias),IDC_ED_CURVEALT_BONUSBIAS);
 SetDlgItemInt(m_hwnd,IDC_ED_CURVEALT_BONUSBIAS,cfgGet(IDFF_enc_alt_curve_bonus_bias),FALSE);
}

void TcurveAltPage::translate(void)
{
 TconfPageEnc::translate();

 cbxTranslate(IDC_CBX_CURVE_TYPE,aggressions);
}

TcurveAltPage::TcurveAltPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_CURVEALT;
 static const int props[]={IDFF_enc_use_alt_curve,IDFF_enc_alt_curve_type,IDFF_enc_alt_curve_high_dist,IDFF_enc_alt_curve_low_dist,IDFF_enc_alt_curve_use_auto,IDFF_enc_alt_curve_auto_str,IDFF_enc_alt_curve_min_rel_qual,IDFF_enc_alt_curve_use_auto_bonus_bias,IDFF_enc_alt_curve_bonus_bias,0};
 propsIDs=props;
 static const TbindCheckbox<TcurveAltPage> chb[]=
  {
   IDC_CHB_USE_ALT_CURVE,IDFF_enc_use_alt_curve,&TcurveAltPage::cfg2dlg,
   IDC_CHB_CURVEALT_BONUSBIASMANUAL,-IDFF_enc_alt_curve_use_auto_bonus_bias,&TcurveAltPage::cfg2dlg,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindRadiobutton<TcurveAltPage> rbt[]=
  {
   IDC_RBT_CURVEALT_USEAUTO,IDFF_enc_alt_curve_use_auto,1,&TcurveAltPage::cfg2dlg,
   IDC_RBT_CURVEALT_USEMANUAL,IDFF_enc_alt_curve_use_auto,0,&TcurveAltPage::cfg2dlg,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindEditInt<TcurveAltPage> edInt[]=
  {
   IDC_ED_CURVEALT_DISTHIGH  ,0,5000,IDFF_enc_alt_curve_high_dist   ,NULL,
   IDC_ED_CURVEALT_DISTLOW   ,0,5000,IDFF_enc_alt_curve_low_dist    ,NULL,
   IDC_ED_CURVEALT_AUTOSTR   ,0, 100,IDFF_enc_alt_curve_auto_str    ,NULL,
   IDC_ED_CURVEALT_MINRELQUAL,0, 100,IDFF_enc_alt_curve_min_rel_qual,NULL,
   IDC_ED_CURVEALT_BONUSBIAS ,0,5000,IDFF_enc_alt_curve_bonus_bias  ,NULL,
   0
  };
 bindEditInts(edInt);
 static const TbindCombobox<TcurveAltPage> cbx[]=
  {
   IDC_CBX_CURVE_TYPE,IDFF_enc_alt_curve_type,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
}
