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
#include "Cdelay.h"

void TdelayPage::cfg2dlg(void)
{
 SetDlgItemInt(m_hwnd,IDC_ED_DELAY_L  ,cfgGet(IDFF_delayL  ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_DELAY_C  ,cfgGet(IDFF_delayC  ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_DELAY_R  ,cfgGet(IDFF_delayR  ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_DELAY_SL ,cfgGet(IDFF_delaySL ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_DELAY_SR ,cfgGet(IDFF_delaySR ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_DELAY_BC ,cfgGet(IDFF_delayBC ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_DELAY_LFE,cfgGet(IDFF_delayLFE),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_DELAY_AL ,cfgGet(IDFF_delayAL ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_DELAY_AR ,cfgGet(IDFF_delayAR ),FALSE);
}

TdelayPage::TdelayPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecAudio(Iparent,idff)
{
 resInter=IDC_CHB_DELAY;
 static const TbindEditInt<TdelayPage> edInt[]=
  {
   IDC_ED_DELAY_L  ,0,20000,IDFF_delayL  ,NULL,
   IDC_ED_DELAY_C  ,0,20000,IDFF_delayC  ,NULL,
   IDC_ED_DELAY_R  ,0,20000,IDFF_delayR  ,NULL,
   IDC_ED_DELAY_SL ,0,20000,IDFF_delaySL ,NULL,
   IDC_ED_DELAY_SR ,0,20000,IDFF_delaySR ,NULL,
   IDC_ED_DELAY_BC ,0,20000,IDFF_delayBC ,NULL,
   IDC_ED_DELAY_LFE,0,20000,IDFF_delayLFE,NULL,
   IDC_ED_DELAY_AL ,0,20000,IDFF_delayAL ,NULL,
   IDC_ED_DELAY_AR ,0,20000,IDFF_delayAR ,NULL,
   0
  };
 bindEditInts(edInt);
}
