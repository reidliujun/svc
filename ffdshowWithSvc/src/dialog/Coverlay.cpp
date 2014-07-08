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
#include "Coverlay.h"
#include "IhwOverlayControl.h"

void ToverlayPage::init(void)
{
 comptr<IhwOverlayControl> hwoc;
 deciV->findOverlayControl2(&hwoc);

 static const int tbrs[]={IDC_TBR_OVERLAY_BRIGHTNESS,IDC_TBR_OVERLAY_CONTRAST,IDC_TBR_OVERLAY_HUE,IDC_TBR_OVERLAY_SATURATION,IDC_TBR_OVERLAY_SHARPNESS,IDC_TBR_OVERLAY_GAMMA};
 static const int ids[][3]={{IDC_LBL_OVERLAY_BRIGHTNESS,IDC_TBR_OVERLAY_BRIGHTNESS,0},
                            {IDC_LBL_OVERLAY_CONTRAST,IDC_TBR_OVERLAY_CONTRAST,0},
                            {IDC_LBL_OVERLAY_HUE,IDC_TBR_OVERLAY_HUE,0},
                            {IDC_LBL_OVERLAY_SATURATION,IDC_TBR_OVERLAY_SATURATION,0},
                            {IDC_LBL_OVERLAY_SHARPNESS,IDC_TBR_OVERLAY_SHARPNESS,0},
                            {IDC_LBL_OVERLAY_GAMMA,IDC_TBR_OVERLAY_GAMMA,0}};

 for (int i=HWOC_BRIGHTNESS;i<=HWOC_GAMMA;i++)
  {
   bool is=hwoc?hwoc->supported(i):false;
   if (hwoc)
    {
     int min,max;
     if (hwoc->getRange(i,&min,&max)==S_OK)
      tbrSetRange(tbrs[i-1],min,max);
    }
   enable(is,ids[i-1]);
  }
}

void ToverlayPage::cfg2dlg(void)
{
 tbrSet(IDC_TBR_OVERLAY_BRIGHTNESS,cfgGet(IDFF_overlayBrightness),IDC_LBL_OVERLAY_BRIGHTNESS);
 tbrSet(IDC_TBR_OVERLAY_CONTRAST,cfgGet(IDFF_overlayContrast),IDC_LBL_OVERLAY_CONTRAST);
 tbrSet(IDC_TBR_OVERLAY_HUE,cfgGet(IDFF_overlayHue),IDC_LBL_OVERLAY_HUE);
 tbrSet(IDC_TBR_OVERLAY_SATURATION,cfgGet(IDFF_overlaySaturation),IDC_LBL_OVERLAY_SATURATION);
 tbrSet(IDC_TBR_OVERLAY_SHARPNESS,cfgGet(IDFF_overlaySharpness),IDC_LBL_OVERLAY_SHARPNESS);
 tbrSet(IDC_TBR_OVERLAY_GAMMA,cfgGet(IDFF_overlayGamma),IDC_LBL_OVERLAY_GAMMA);
}

ToverlayPage::ToverlayPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 repaint=false;
 resInter=IDC_CHB_OVERLAY;
 idffOrder=maxOrder+1;
 static const TbindTrackbar<ToverlayPage> htbr[]=
  {
   IDC_TBR_OVERLAY_BRIGHTNESS,IDFF_overlayBrightness,&ToverlayPage::cfg2dlg,
   IDC_TBR_OVERLAY_CONTRAST,IDFF_overlayContrast,&ToverlayPage::cfg2dlg,
   IDC_TBR_OVERLAY_HUE,IDFF_overlayHue,&ToverlayPage::cfg2dlg,
   IDC_TBR_OVERLAY_SATURATION,IDFF_overlaySaturation,&ToverlayPage::cfg2dlg,
   IDC_TBR_OVERLAY_SHARPNESS,IDFF_overlaySharpness,&ToverlayPage::cfg2dlg,
   IDC_TBR_OVERLAY_GAMMA,IDFF_overlayGamma,&ToverlayPage::cfg2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
}

INT_PTR ToverlayPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_OVERLAY:
       repaint=true;
       break;
     }
   break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

void ToverlayPage::onFrame(void)
{
 if (!IsWindowVisible(m_hwnd)) return;
 if(repaint)
  {
   cfg2dlg();
   repaint=false;
  }
}

bool ToverlayPage::reset(bool testonly)
{
 if (!testonly)
  {
   deci->resetParam(IDFF_overlayBrightness);
   deci->resetParam(IDFF_overlayContrast);
   deci->resetParam(IDFF_overlayHue);
   deci->resetParam(IDFF_overlaySaturation);
   deci->resetParam(IDFF_overlaySharpness);
   deci->resetParam(IDFF_overlayGamma);
   cfgSet(IDFF_overlayReset,cfgGet(IDFF_overlayReset)+1);
   repaint = true;
  }
 return true;
}
