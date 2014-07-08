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
#include "CmaskingX264.h"

bool TmaskingPageX264::enabled(void)
{
 return codecId==CODEC_ID_X264;
}

void TmaskingPageX264::init(void)
{
 tbrSetRange(IDC_TBR_X264_AQ_STRENGTH,1,100);
}

void TmaskingPageX264::cfg2dlg(void)
{
 tbrSet(IDC_TBR_X264_AQ_STRENGTH,cfgGet(IDFF_enc_x264_aq_strength100),IDC_LBL_X264_AQ_STRENGTH);
 static const int idAQ[]={IDC_TBR_X264_AQ_STRENGTH,IDC_LBL_X264_AQ_STRENGTH,0};
}

TmaskingPageX264::TmaskingPageX264(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_MASKING_X264;
 static const int props[]={IDFF_enc_x264_aq_strength100,0};
 propsIDs=props;
 static const TbindTrackbar<TmaskingPageX264> htbr[]=
  {
   IDC_TBR_X264_AQ_STRENGTH,IDFF_enc_x264_aq_strength100,&TmaskingPageX264::cfg2dlg,
   0,NULL,NULL
  };
 bindHtracks(htbr);
}
