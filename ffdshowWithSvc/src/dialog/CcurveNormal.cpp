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
#include "CcurveNormal.h"

bool TcurveNormalPage::enabled(void)
{
 int mode=cfgGet(IDFF_enc_mode);
 return sup_perFrameQuant(codecId) && (mode==ENC_MODE::PASS2_2_EXT || mode==ENC_MODE::PASS2_2_INT);
}

void TcurveNormalPage::cfg2dlg(void)
{
 SetDlgItemInt(m_hwnd,IDC_ED_CURVENORMAL_HIGH,cfgGet(IDFF_enc_curve_compression_high),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_CURVENORMAL_LOW ,cfgGet(IDFF_enc_curve_compression_low ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_CURVENORMAL_BITRATEPAYBACKDELAY,cfgGet(IDFF_enc_bitrate_payback_delay),FALSE);
 setCheck(IDC_RBT_CURVENORMAL_PAYBACKWITHBIAS,cfgGet(IDFF_enc_bitrate_payback_method)==0);
 setCheck(IDC_RBT_CURVENORMAL_PAYBACKPROPORTIONALLY,cfgGet(IDFF_enc_bitrate_payback_method)!=0);
}

TcurveNormalPage::TcurveNormalPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_CURVENORMAL;
 static const int props[]={IDFF_enc_curve_compression_high,IDFF_enc_curve_compression_low,IDFF_enc_bitrate_payback_delay,IDFF_enc_bitrate_payback_method,0};
 propsIDs=props;
 static const TbindRadiobutton<TcurveNormalPage> rbt[]=
  {
   IDC_RBT_CURVENORMAL_PAYBACKWITHBIAS,IDFF_enc_bitrate_payback_method,0,NULL,
   IDC_RBT_CURVENORMAL_PAYBACKPROPORTIONALLY,IDFF_enc_bitrate_payback_method,1,NULL,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindEditInt<TcurveNormalPage> edInt[]=
  {
   IDC_ED_CURVENORMAL_HIGH,0,100,IDFF_enc_curve_compression_high,NULL,
   IDC_ED_CURVENORMAL_LOW ,0,100,IDFF_enc_curve_compression_low ,NULL,
   IDC_ED_CURVENORMAL_BITRATEPAYBACKDELAY,1,10000000,IDFF_enc_bitrate_payback_delay,NULL,
   0
  };
 bindEditInts(edInt);
}

