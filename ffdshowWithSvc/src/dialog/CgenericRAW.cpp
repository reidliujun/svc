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
#include "CgenericRAW.h"

void TgenericRAWpage::init(void)
{
 for (int i=0;cspFccs[i].name;i++)
  if (cspFccs[i].supEnc)
   cbxAdd(IDC_CBX_RAW_FOURCC,cspFccs[i].name,cspFccs[i].fcc);
}

void TgenericRAWpage::cfg2dlg(void)
{
 cbxSetDataCurSel(IDC_CBX_RAW_FOURCC,cfgGet(IDFF_enc_raw_fourcc));
}

TgenericRAWpage::TgenericRAWpage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_GENERIC_RAW;
 static const int props[]={IDFF_enc_raw_fourcc,0};
 propsIDs=props;
 static const TbindCombobox<TgenericRAWpage> cbx[]=
  {
   IDC_CBX_RAW_FOURCC,IDFF_enc_raw_fourcc,BINDCBX_DATA,NULL,
   0
  };
 bindComboboxes(cbx);
}
