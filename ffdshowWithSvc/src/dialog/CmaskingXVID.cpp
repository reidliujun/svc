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
#include "CmaskingXVID.h"

bool TmaskingPageXvid::enabled(void)
{
 return xvid_codec(codecId);
}

void TmaskingPageXvid::cfg2dlg(void)
{
 setCheck(IDC_CHB_XVID_MASKING,cfgGet(IDFF_enc_xvid_lum_masking));
 setCheck(IDC_CHB_XVID_CHROMAOPT,cfgGet(IDFF_enc_xvid_chromaopt));enable(!cfgGet(IDFF_enc_gray),IDC_CHB_XVID_CHROMAOPT);
}

TmaskingPageXvid::TmaskingPageXvid(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_MASKING_XVID;
 static const int props[]={IDFF_enc_xvid_lum_masking,IDFF_enc_xvid_chromaopt,0};
 propsIDs=props;
 static const TbindCheckbox<TmaskingPageXvid> chb[]=
  {
   IDC_CHB_XVID_MASKING,IDFF_enc_xvid_lum_masking,NULL,
   IDC_CHB_XVID_CHROMAOPT,IDFF_enc_xvid_chromaopt,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
}
