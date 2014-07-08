/*
 * Copyright (c) 2004-2006 Milan Cutka
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
#include "CgenericLJPEG.h"
#include "ffdshow_mediaguids.h"

const TcspFcc TgenericLJPEGpage::cspFccs[]=
{
 {_l("YV12") ,FOURCC_YV12},
 {_l("RGB32"),FOURCC_RGB3},
 NULL
};

void TgenericLJPEGpage::init(void)
{
 for (int i=0;cspFccs[i].name;i++)
  cbxAdd(IDC_CBX_LJPEG_CSP,cspFccs[i].name,cspFccs[i].fcc);
}

bool TgenericLJPEGpage::enabled(void)
{
 return codecId==CODEC_ID_LJPEG;
}

void TgenericLJPEGpage::cfg2dlg(void)
{
 cbxSetDataCurSel(IDC_CBX_LJPEG_CSP,cfgGet(IDFF_enc_ljpeg_csp));
}

TgenericLJPEGpage::TgenericLJPEGpage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_GENERIC_LJPEG;
 static const int props[]={IDFF_enc_ljpeg_csp,0};
 propsIDs=props;
 static const TbindCombobox<TgenericLJPEGpage> cbx[]=
  {
   IDC_CBX_LJPEG_CSP,IDFF_enc_ljpeg_csp,BINDCBX_DATA,NULL,
   0
  };
 bindComboboxes(cbx);
}
