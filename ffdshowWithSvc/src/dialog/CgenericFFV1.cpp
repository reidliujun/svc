/*
 * Copyright (c) 2003-2006 Milan Cutka
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
#include "CgenericFFV1.h"

void TgenericFFV1page::init(void)
{
 for (int i=0;TcoSettings::ffv1csps[i].name;i++)
  cbxAdd(IDC_CBX_FFV1_CSP,TcoSettings::ffv1csps[i].name,TcoSettings::ffv1csps[i].fcc);
 for (int i=0;TcoSettings::ffv1coders[i];i++)
  cbxAdd(IDC_CBX_FFV1_CODER,TcoSettings::ffv1coders[i]);
 for (int i=0;TcoSettings::ffv1contexts[i];i++)
  cbxAdd(IDC_CBX_FFV1_CONTEXT,TcoSettings::ffv1contexts[i]);
}

bool TgenericFFV1page::enabled(void)
{
 return codecId==CODEC_ID_FFV1;
}

void TgenericFFV1page::cfg2dlg(void)
{
 cbxSetDataCurSel(IDC_CBX_FFV1_CSP,cfgGet(IDFF_enc_ffv1_csp));
 cbxSetCurSel(IDC_CBX_FFV1_CODER,cfgGet(IDFF_enc_ffv1_coder));
 cbxSetCurSel(IDC_CBX_FFV1_CONTEXT,cfgGet(IDFF_enc_ffv1_context));
 //SetDlgItemInt(m_hwnd,IDC_ED_FFV1_CONTEXT,cfgGet(IDFF_enc_ffv1_context),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_MAXKEYINTERVAL,cfgGet(IDFF_enc_ffv1_key_interval),FALSE);
}

TgenericFFV1page::TgenericFFV1page(TffdshowPageEnc *Iparent) :TconfPageEnc(Iparent)
{
 dialogId=IDD_GENERIC_FFV1;
 static const int props[]={IDFF_enc_ffv1_csp,IDFF_enc_ffv1_coder,IDFF_enc_ffv1_context,IDFF_enc_ffv1_key_interval,0};
 propsIDs=props;
 static const TbindEditInt<TgenericFFV1page> edInt[]=
  {
   //IDC_ED_FFV1_CONTEXT  ,0,10 ,IDFF_enc_ffv1_context    ,NULL,
   IDC_ED_MAXKEYINTERVAL,1,500,IDFF_enc_ffv1_key_interval,NULL,
   0
  };
 bindEditInts(edInt);
 static const TbindCombobox<TgenericFFV1page> cbx[]=
  {
   IDC_CBX_FFV1_CSP,IDFF_enc_ffv1_csp,BINDCBX_DATA,NULL,
   IDC_CBX_FFV1_CODER,IDFF_enc_ffv1_coder,BINDCBX_SEL,NULL,
   IDC_CBX_FFV1_CONTEXT,IDFF_enc_ffv1_context,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
}
