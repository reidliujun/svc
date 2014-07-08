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
#include "CmaskingSKAL.h"

void TmaskingPageSkal::cfg2dlg(void)
{
 setCheck(IDC_CHB_SKAL_MASKING,cfgGet(IDFF_enc_isSkalMasking));
 static const int idMasking[]={IDC_LBL_SKAL_MASKING,IDC_ED_SKAL_MASKING,0};
 setText(IDC_ED_SKAL_MASKING,_l("%g"),float(cfgGet(IDFF_enc_skalMaskingAmp)/100.0));
 enable(cfgGet(IDFF_enc_isSkalMasking),idMasking);
}

TmaskingPageSkal::TmaskingPageSkal(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_MASKING_SKAL;
 static const int props[]={IDFF_enc_isSkalMasking,IDFF_enc_skalMaskingAmp,0};
 propsIDs=props;
 static const TbindCheckbox<TmaskingPageSkal> chb[]=
  {
   IDFF_enc_isSkalMasking,IDFF_enc_isSkalMasking,&TmaskingPageSkal::cfg2dlg,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditReal<TmaskingPageSkal> edReal[]=
  {
   IDC_ED_SKAL_MASKING,1.0,50.0,IDFF_enc_skalMaskingAmp,100.0,NULL,
   0
  };
 bindEditReals(edReal);
}
