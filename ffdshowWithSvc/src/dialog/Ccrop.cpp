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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Ccrop.h"

void TcropPage::init(void)
{
 tbrSetRange(IDC_TBR_ZOOMX,0,100,8);
 tbrSetRange(IDC_TBR_ZOOMY,0,100,8);
 tbrSetRange(IDC_TBR_PANSCAN_ZOOM,0,100,8);
 tbrSetRange(IDC_TBR_PANSCAN_X,-100,100,15);
 tbrSetRange(IDC_TBR_PANSCAN_Y,-100,100,15);
 addHint(IDC_ED_CROP_STOPSCAN, _l("Scan of bars will stop after this position. Set 0 to always scan for bars"));
 addHint(IDC_ED_CROP_TOLERANCE, _l("Tolerance when scanning bars"));
 addHint(IDC_ED_CROP_REFRESH, _l("Set the delay in ms to rescan for the bars"));
}

void TcropPage::cfg2dlg(void)
{
 crop2dlg();
 zoom2dlg();
 panscan2dlg();
 autocrop2dlg();
}
void TcropPage::crop2dlg(void)
{
 bool is=(cfgGet(IDFF_cropNzoomMode)==1);
 setCheck(IDC_RBT_CROP,is);
 static const int idCrop[]={IDC_ED_CROP_LEFT,IDC_ED_CROP_TOP,IDC_ED_CROP_RIGHT,IDC_ED_CROP_BOTTOM,IDC_LBL_CROP_LEFT,IDC_LBL_CROP_TOP,IDC_LBL_CROP_RIGHT,IDC_LBL_CROP_BOTTOM, 0};

 enable(is,idCrop);
 SetDlgItemInt(m_hwnd,IDC_ED_CROP_LEFT  ,cfgGet(IDFF_cropLeft  ),0);
 SetDlgItemInt(m_hwnd,IDC_ED_CROP_TOP   ,cfgGet(IDFF_cropTop   ),0);
 SetDlgItemInt(m_hwnd,IDC_ED_CROP_RIGHT ,cfgGet(IDFF_cropRight ),0);
 SetDlgItemInt(m_hwnd,IDC_ED_CROP_BOTTOM,cfgGet(IDFF_cropBottom),0);
}
void TcropPage::zoom2dlg(void)
{
 bool is=(cfgGet(IDFF_cropNzoomMode)==0);
 setCheck(IDC_RBT_ZOOM,is);
 static const int idZoom[]={IDC_LBL_ZOOMX,IDC_TBR_ZOOMX,IDC_LBL_ZOOMY,IDC_TBR_ZOOMY,IDC_CHB_MAGNIFICATION_LOCKED,0};
 enable(is,idZoom);

 setCheck(IDC_CHB_MAGNIFICATION_LOCKED,cfgGet(IDFF_magnificationLocked));
 static const int idZoomY[]={IDC_LBL_ZOOMY,IDC_TBR_ZOOMY,0};
 enable(is && !cfgGet(IDFF_magnificationLocked),idZoomY);

 tbrSet(IDC_TBR_ZOOMX,cfgGet(IDFF_magnificationX),IDC_LBL_ZOOMX);
 tbrSet(IDC_TBR_ZOOMY,cfgGet(IDFF_magnificationY),IDC_LBL_ZOOMY);
}
void TcropPage::panscan2dlg(void)
{
 setCheck(IDC_RBT_PANSCAN,cfgGet(IDFF_cropNzoomMode)==2);
 int x;
 x=cfgGet(IDFF_panscanZoom);
 setText(IDC_LBL_PANSCAN_ZOOM,_l("%s %i%%"),_(IDC_LBL_PANSCAN_ZOOM),x);
 tbrSet(IDC_TBR_PANSCAN_ZOOM,x);
 x=cfgGet(IDFF_panscanX);
 setText(IDC_LBL_PANSCAN_X,_l("%s %i%%"),_(IDC_LBL_PANSCAN_X),x);
 tbrSet(IDC_TBR_PANSCAN_X,x);
 x=cfgGet(IDFF_panscanY);
 setText(IDC_LBL_PANSCAN_Y,_l("%s %i%%"),_(IDC_LBL_PANSCAN_Y),x);
 tbrSet(IDC_TBR_PANSCAN_Y,x);
}

void TcropPage::autocrop2dlg(void)
{
 int mode = cfgGet(IDFF_cropNzoomMode);
 setCheck(IDC_RBT_AUTOCROPV,mode==3);
 setCheck(IDC_RBT_AUTOCROPH,mode==4);
 setCheck(IDC_RBT_AUTOCROPVH,mode==5);
 static const int idautoCrop[]={IDC_LBL_CROP_TOLERANCE, IDC_ED_CROP_TOLERANCE, IDC_LBL_CROP_REFRESH, IDC_ED_CROP_REFRESH, IDC_LBL_CROP_STOPSCAN, IDC_ED_CROP_STOPSCAN, 0};
 enable(mode>=3 && mode<=5,idautoCrop);
 SetDlgItemInt(m_hwnd,IDC_ED_CROP_TOLERANCE,cfgGet(IDFF_cropTolerance),0);
 SetDlgItemInt(m_hwnd,IDC_ED_CROP_REFRESH,cfgGet(IDFF_cropRefreshDelay),0);
 SetDlgItemInt(m_hwnd,IDC_ED_CROP_STOPSCAN,cfgGet(IDFF_cropStopScan),0);
}

INT_PTR TcropPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_ZOOMX:
      case IDC_TBR_ZOOMY:
       cfgSet(IDFF_magnificationX,tbrGet(IDC_TBR_ZOOMX));
       cfgSet(IDFF_magnificationY,tbrGet(cfgGet(IDFF_magnificationLocked)?IDC_TBR_ZOOMX:IDC_TBR_ZOOMY));
       zoom2dlg();
       return TRUE;
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_MAGNIFICATION_LOCKED:
       cfgSet(IDFF_magnificationLocked,getCheck(IDC_CHB_MAGNIFICATION_LOCKED));
       cfgSet(IDFF_magnificationY,cfgGet(IDFF_magnificationX));
       zoom2dlg();
       return TRUE;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

TcropPage::TcropPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_CROP;
 static const TbindTrackbar<TcropPage> htbr[]=
  {
   IDC_TBR_PANSCAN_ZOOM,IDFF_panscanZoom,&TcropPage::panscan2dlg,
   IDC_TBR_PANSCAN_X,IDFF_panscanX,&TcropPage::panscan2dlg,
   IDC_TBR_PANSCAN_Y,IDFF_panscanY,&TcropPage::panscan2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindRadiobutton<TcropPage> rbt[]=
  {
   IDC_RBT_ZOOM,IDFF_cropNzoomMode,0,&TcropPage::cfg2dlg,
   IDC_RBT_CROP,IDFF_cropNzoomMode,1,&TcropPage::cfg2dlg,
   IDC_RBT_PANSCAN,IDFF_cropNzoomMode,2,&TcropPage::cfg2dlg,
   IDC_RBT_AUTOCROPV,IDFF_cropNzoomMode,3,&TcropPage::cfg2dlg,
   IDC_RBT_AUTOCROPH,IDFF_cropNzoomMode,4,&TcropPage::cfg2dlg,
   IDC_RBT_AUTOCROPVH,IDFF_cropNzoomMode,5,&TcropPage::cfg2dlg,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindEditInt<TcropPage> edInt[]=
  {
   IDC_ED_CROP_LEFT  ,0,2048,IDFF_cropLeft  ,NULL,
   IDC_ED_CROP_RIGHT ,0,2048,IDFF_cropRight ,NULL,
   IDC_ED_CROP_TOP   ,0,2048,IDFF_cropTop   ,NULL,
   IDC_ED_CROP_BOTTOM,0,2048,IDFF_cropBottom,NULL,
   IDC_ED_CROP_TOLERANCE,0,2048,IDFF_cropTolerance,NULL,
   IDC_ED_CROP_REFRESH,0,3600000,IDFF_cropRefreshDelay,NULL,
   IDC_ED_CROP_STOPSCAN,0,3600000,IDFF_cropStopScan,NULL,
   0
  };
 bindEditInts(edInt);
}

