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
#include "Conepass.h"

//rc_eq - ?
 //rc_buffer_aggressivity
//rc_initial_cplx - ?

bool TonePassPage::enabled(void)
{
 return sup_lavcOnePass(codecId) && cfgGet(IDFF_enc_mode)==ENC_MODE::CBR;
}

void TonePassPage::cfg2dlg(void)
{
 SetDlgItemInt(m_hwnd,IDC_ED_VRATETOL,cfgGet(IDFF_enc_ff1_vratetol),FALSE);
 setText(IDC_ED_VQCOMP ,_l("%g"),float(cfgGet(IDFF_enc_ff1_vqcomp )/100.0));
 setText(IDC_ED_VQBLUR1,_l("%g"),float(cfgGet(IDFF_enc_ff1_vqblur1)/100.0));
 setText(IDC_ED_VQBLUR2,_l("%g"),float(cfgGet(IDFF_enc_ff1_vqblur2)/100.0));
 SetDlgItemInt(m_hwnd,IDC_ED_VQDIFF,cfgGet(IDFF_enc_ff1_vqdiff),FALSE);
 setCheck(IDC_CHB_RC_SQUISH,cfgGet(IDFF_enc_ff1_rc_squish));
 setText(IDC_ED_MIN_BITRATE,_l("%i"),cfgGet(IDFF_enc_ff1_rc_min_rate1000));
 setText(IDC_ED_MAX_BITRATE,_l("%i"),cfgGet(IDFF_enc_ff1_rc_max_rate1000));
 setText(IDC_ED_BUFFER_SIZE,_l("%i"),cfgGet(IDFF_enc_ff1_rc_buffer_size));
 static const int lavconly[]={IDC_LBL_VRATETOL,IDC_ED_VRATETOL,IDC_LBL_VRATETOL2,IDC_LBL_VQDIFF,IDC_ED_VQDIFF,IDC_LBL_MIN_BITRATE,IDC_ED_MIN_BITRATE,IDC_LBL_MIN_BITRATE2,IDC_LBL_MAX_BITRATE,IDC_ED_MAX_BITRATE,IDC_LBL_MAX_BITRATE2,IDC_LBL_BUFFER_SIZE,IDC_ED_BUFFER_SIZE,IDC_LBL_BUFFER_SIZE2,IDC_CHB_RC_SQUISH,0};
 enable(!x264_codec(codecId),lavconly);
}

TonePassPage::TonePassPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_ONEPASS;
 static const int props[]={IDFF_enc_ff1_vratetol,IDFF_enc_ff1_vqcomp,IDFF_enc_ff1_vqblur1,IDFF_enc_ff1_vqblur2,IDFF_enc_ff1_vqdiff,IDFF_enc_ff1_rc_squish,IDFF_enc_ff1_rc_min_rate1000,IDFF_enc_ff1_rc_max_rate1000,IDFF_enc_ff1_rc_buffer_size,0};
 propsIDs=props;
 static const TbindCheckbox<TonePassPage> chb[]=
  {
   IDC_CHB_RC_SQUISH,IDFF_enc_ff1_rc_squish,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditInt<TonePassPage> edInt[]=
  {
   IDC_ED_VRATETOL,1,1024*10,IDFF_enc_ff1_vratetol,NULL,
   IDC_ED_VQDIFF,0,31,IDFF_enc_ff1_vqdiff,NULL,
   IDC_ED_MIN_BITRATE,0,100000,IDFF_enc_ff1_rc_min_rate1000,NULL,
   IDC_ED_MAX_BITRATE,0,100000,IDFF_enc_ff1_rc_max_rate1000,NULL,
   IDC_ED_BUFFER_SIZE,0,INT_MAX,IDFF_enc_ff1_rc_buffer_size,NULL,
   0
  };
 bindEditInts(edInt);
 static const TbindEditReal<TonePassPage> edReal[]=
  {
   IDC_ED_VQCOMP,0.0,1.0,IDFF_enc_ff1_vqcomp,100.0,NULL,
   IDC_ED_VQBLUR1,0.0,1.0,IDFF_enc_ff1_vqblur1,100.0,NULL,
   IDC_ED_VQBLUR2,0.0,1.0,IDFF_enc_ff1_vqblur2,100.0,NULL,
   0
  };
 bindEditReals(edReal);
}
