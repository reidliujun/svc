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
#include "CmeSkal.h"

const char_t* TmeSkalPage::metrics[]=
{
 _l("SAD"),
 _l("SSD"),
 _l("Hammard"),
 NULL
};

void TmeSkalPage::init(void)
{
 for (int i=0;metrics[i];i++)
  cbxAdd(IDC_CBX_SKAL_SEARCHMETRIC,metrics[i]);
}

void TmeSkalPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_4MV,cfgGet(IDFF_enc_me_4mv));
 setCheck(IDC_CHB_QPEL,cfgGet(IDFF_enc_me_qpel));
 setCheck(IDC_CHB_GMC,cfgGet(IDFF_enc_me_gmc));
 cbxSetCurSel(IDC_CBX_SKAL_SEARCHMETRIC,cfgGet(IDFF_enc_skalSearchMetric));
}

TmeSkalPage::TmeSkalPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_ME_SKAL;
 static const int props[]={IDFF_enc_me_4mv,IDFF_enc_me_qpel,IDFF_enc_me_gmc,IDFF_enc_skalSearchMetric,0};
 propsIDs=props;
 static const TbindCheckbox<TmeSkalPage> chb[]=
  {
   IDC_CHB_4MV,IDFF_enc_me_4mv,NULL,
   IDC_CHB_QPEL,IDFF_enc_me_qpel,NULL,
   IDC_CHB_GMC,IDFF_enc_me_gmc,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindCombobox<TmeSkalPage> cbx[]=
  {
   IDC_CBX_SKAL_SEARCHMETRIC,IDFF_enc_skalSearchMetric,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
}
