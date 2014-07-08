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
#include "CgenericHuffyuv.h"
#include "Ttranslate.h"

void TgenericHuffyuvPage::init(void)
{
 for (int i=0;TcoSettings::huffYUVcsps[i];i++)
  cbxAdd(IDC_CBX_HUFFYUV_CSP,TcoSettings::huffYUVcsps[i]);
 addHint(IDC_CHB_HUFFYUV_CTX,_l("Better compression, incompatible with original HuffYUV codec"));
}

bool TgenericHuffyuvPage::enabled(void)
{
 return codecId==CODEC_ID_HUFFYUV;
}

void TgenericHuffyuvPage::cfg2dlg(void)
{
 cbxSetCurSel(IDC_CBX_HUFFYUV_CSP,cfgGet(IDFF_enc_huffyuv_csp));
 cbxSetCurSel(IDC_CBX_HUFFYUV_PRED,cfgGet(IDFF_enc_huffyuv_pred));
 setCheck(IDC_CHB_HUFFYUV_CTX,cfgGet(IDFF_enc_huffyuv_ctx));
 setCheck(IDC_CHB_GRAY,cfgGet(IDFF_enc_gray));enable(0,IDC_CHB_GRAY); //maybe lavc will support grayscale huffyuv encoding some day (or should I try to modify it?)
}

void TgenericHuffyuvPage::translate(void)
{
 TconfPageEnc::translate();

 cbxTranslate(IDC_CBX_HUFFYUV_PRED,TcoSettings::huffYUVpreds);
}

TgenericHuffyuvPage::TgenericHuffyuvPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_GENERIC_HUFFYUV;
 static const int props[]={IDFF_enc_huffyuv_csp,IDFF_enc_huffyuv_pred,IDFF_enc_huffyuv_ctx,0};
 propsIDs=props;
 static const TbindCheckbox<TgenericHuffyuvPage> chb[]=
  {
   IDC_CHB_GRAY,IDFF_enc_gray,NULL,
   IDC_CHB_HUFFYUV_CTX,IDFF_enc_huffyuv_ctx,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindCombobox<TgenericHuffyuvPage> cbx[]=
  {
   IDC_CBX_HUFFYUV_CSP,IDFF_enc_huffyuv_csp,BINDCBX_SEL,NULL,
   IDC_CBX_HUFFYUV_PRED,IDFF_enc_huffyuv_pred,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
}
