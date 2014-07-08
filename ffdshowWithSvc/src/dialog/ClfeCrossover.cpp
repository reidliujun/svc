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
#include "ClfeCrossover.h"

void TlfeCrossoverPage::init(void)
{
 tbrSetRange(IDC_TBR_LFECROSSOVER_FREQ,1,300);
 tbrSetRange(IDC_TBR_LFECROSSOVER_GAIN,-1000,1000);
}

void TlfeCrossoverPage::cfg2dlg(void)
{
 tbrSet(IDC_TBR_LFECROSSOVER_FREQ,cfgGet(IDFF_LFEcrossoverFreq),IDC_LBL_LFECROSSOVER_FREQ);
 int db=cfgGet(IDFF_LFEcrossoverGain);
 tbrSet(IDC_TBR_LFECROSSOVER_GAIN,db);
 setText(IDC_LBL_LFECROSSOVER_GAIN,_l("%s %.2f db"),_(IDC_LBL_LFECROSSOVER_GAIN),db/100.0f);
 setCheck(IDC_CHB_LFECROSSOVER_LRCUT,cfgGet(IDFF_LFEcutLR));
}

TlfeCrossoverPage::TlfeCrossoverPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecAudio(Iparent,idff)
{
 resInter=IDC_CHB_LFECROSSOVER;
 static const TbindCheckbox<TlfeCrossoverPage> chb[]=
  {
   IDC_CHB_LFECROSSOVER_LRCUT,IDFF_LFEcutLR,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TlfeCrossoverPage> htbr[]=
  {
   IDC_TBR_LFECROSSOVER_FREQ,IDFF_LFEcrossoverFreq,&TlfeCrossoverPage::cfg2dlg,
   IDC_TBR_LFECROSSOVER_GAIN,IDFF_LFEcrossoverGain,&TlfeCrossoverPage::cfg2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
}
