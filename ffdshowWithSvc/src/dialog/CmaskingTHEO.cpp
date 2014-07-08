/*
 * Copyright (c) 2005,2006 Milan Cutka
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
#include "CmaskingTHEO.h"

const char_t* TmaskingPageTheo::sharpenesss[]=
{
 _l("high detail"),
 _l("normal"),
 _l("smooth"),
 NULL
};

bool TmaskingPageTheo::enabled(void)
{
 return theora_codec(codecId);
}

void TmaskingPageTheo::init(void)
{
 tbrSetRange(IDC_TBR_THEO_NOISESENSITIVITY,0,6);
}

void TmaskingPageTheo::cfg2dlg(void)
{
 cbxSetCurSel(IDC_CBX_THEO_SHARPNESS,cfgGet(IDFF_enc_theo_sharpness));
 sensitivity2dlg();
}

void TmaskingPageTheo::sensitivity2dlg(void)
{
 tbrSet(IDC_TBR_THEO_NOISESENSITIVITY,cfgGet(IDFF_enc_theo_noisesensitivity),IDC_LBL_THEO_NOISESENSITIVITY);
}

void TmaskingPageTheo::translate(void)
{
 TconfPageEnc::translate();

 cbxTranslate(IDC_CBX_THEO_SHARPNESS,sharpenesss);
}

TmaskingPageTheo::TmaskingPageTheo(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_MASKING_THEO;
 static const int props[]={IDFF_enc_theo_sharpness,IDFF_enc_theo_noisesensitivity,0};
 propsIDs=props;
 static const TbindCombobox<TmaskingPageTheo> cbx[]=
  {
   IDC_CBX_THEO_SHARPNESS,IDFF_enc_theo_sharpness,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
 static const TbindTrackbar<TmaskingPageTheo> htbr[]=
  {
   IDC_TBR_THEO_NOISESENSITIVITY,IDFF_enc_theo_noisesensitivity,&TmaskingPageTheo::sensitivity2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
}
