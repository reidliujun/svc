/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include "CshowMV.h"

void TshowMVpage::cfg2dlg(void)
{
 setCheck(IDC_CHB_VIS_MV,cfgGet(IDFF_visMV));enable((filterMode&IDFF_FILTERMODE_VIDEORAW)==0,IDC_CHB_VIS_MV);
 setCheck(IDC_CHB_VIS_QUANTS,cfgGet(IDFF_visQuants));enable((filterMode&IDFF_FILTERMODE_VIDEORAW)==0,IDC_CHB_VIS_QUANTS);
 setCheck(IDC_CHB_VIS_GRAPH,cfgGet(IDFF_visGraph));
}

TshowMVpage::TshowMVpage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_VISUALIZATIONS;
 static const TbindCheckbox<TshowMVpage> chb[]=
  {
   IDC_CHB_VIS_MV,IDFF_visMV,NULL,
   IDC_CHB_VIS_QUANTS,IDFF_visQuants,NULL,
   IDC_CHB_VIS_GRAPH,IDFF_visGraph,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
}
