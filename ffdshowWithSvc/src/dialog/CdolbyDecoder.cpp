/*
 * Copyright (c) 2004-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "CdolbyDecoder.h"

void TdolbyDecoderPage::cfg2dlg(void)
{
 SetDlgItemInt(m_hwnd,IDC_ED_DOLBYDECODER_DELAY,cfgGet(IDFF_dolbyDecoderDelay),FALSE);
}

TdolbyDecoderPage::TdolbyDecoderPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecAudio(Iparent,idff)
{
 resInter=IDC_CHB_DOLBYDECODER;
 static const TbindEditInt<TdolbyDecoderPage> edInt[]=
  {
   IDC_ED_DOLBYDECODER_DELAY,0,1000,IDFF_dolbyDecoderDelay,NULL,
   0
  };
 bindEditInts(edInt);
}
