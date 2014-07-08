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
#include "ConepassXvid.h"

bool TonePassXvidPage::enabled(void)
{
 return xvid_codec(codecId) && cfgGet(IDFF_enc_mode)==ENC_MODE::CBR;
}

void TonePassXvidPage::cfg2dlg(void)
{
 SetDlgItemInt(m_hwnd,IDC_ED_XVID_REACTION_DELAY_FACTOR,cfgGet(IDFF_enc_xvid_rc_reaction_delay_factor),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_XVID_AVERAGING_PERIOD,cfgGet(IDFF_enc_xvid_rc_averaging_period),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_XVID_BUFFER,cfgGet(IDFF_enc_xvid_rc_buffer),FALSE);
}

TonePassXvidPage::TonePassXvidPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_ONEPASS_XVID;
 static const int props[]={IDFF_enc_xvid_rc_reaction_delay_factor,IDFF_enc_xvid_rc_averaging_period,IDFF_enc_xvid_rc_buffer,0};
 propsIDs=props;
 static const TbindEditInt<TonePassXvidPage> edInt[]=
  {
   IDC_ED_XVID_REACTION_DELAY_FACTOR,0,10000000,IDFF_enc_xvid_rc_reaction_delay_factor,NULL,
   IDC_ED_XVID_AVERAGING_PERIOD,0,10000000,IDFF_enc_xvid_rc_averaging_period,NULL,
   IDC_ED_XVID_BUFFER,0,10000000,IDFF_enc_xvid_rc_buffer,NULL,
   0
  };
 bindEditInts(edInt);
}
