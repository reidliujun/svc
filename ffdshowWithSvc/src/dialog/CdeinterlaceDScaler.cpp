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
#include "CdeinterlaceDScaler.h"
#include "IffdshowBase.h"
#include "ffdshow_constants.h"
#include "TimgFilterDScalerDeinterlace.h"
#include "TDScalerSettings.h"

TdeinterlacePageDScaler::TdeinterlacePageDScaler(Twindow *parent,const char_t *flnm):TdeinterlacePanel(0,parent)
{
 di=new Tdscaler_DI(flnm,parent->deci);
 m_hwnd=NULL;
 if (!di->settings) return;
 di->settings->str2cfg(deci->getParamStr2(IDFF_dscalerDIcfg));
 HWND place=GetDlgItem(parent->m_hwnd,IDC_PLC_DEINT);
 RECT placer,parentr;
 GetWindowRect(parent->m_hwnd,&parentr);
 GetWindowRect(place,&placer);
 OffsetRect(&placer,-parentr.left,-parentr.top);
 m_hwnd=di->settings->createWindow(hi,parent,placer,WM_DSCALERPAGE_CHANGE,NULL);
 if (m_hwnd) SetWindowPos(m_hwnd,place,placer.left,placer.top,placer.right-placer.left,placer.bottom-placer.top,SWP_SHOWWINDOW);
}

TdeinterlacePageDScaler::~TdeinterlacePageDScaler()
{
 //DestroyWindow(m_hwnd);
 delete di;
}

void TdeinterlacePageDScaler::storeCfg(void)
{
 if (!di->settings) return;
 ffstring s;
 di->settings->cfg2str(s);
 if (s.empty() || s.size()==0) return;
 cfgSet(IDFF_dscalerDIcfg,s.c_str());
}

void TdeinterlacePageDScaler::reset(void)
{
 if (di->settings) di->settings->reset();
}
