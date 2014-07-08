 /*
 * Copyright (c) 2005,2006 Milan Cutka
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
#include "CresizeBorders.h"
#include "TresizeAspectSettings.h"

void TresizeBordersPage::init(void)
{
 tbrSetRange(IDC_TBR_BORDER_HORIZ_PERCENT,0,100,10);
 tbrSetRange(IDC_TBR_BORDER_VERT_PERCENT ,0,100,10);
 tbrSetRange(IDC_TBR_BORDER_HORIZ_PIXELS,0,300,16);
 tbrSetRange(IDC_TBR_BORDER_VERT_PIXELS ,0,300,16);
 tbrSetRange(IDC_TBR_BORDER_HORIZ_DIV ,0,100,10);
 tbrSetRange(IDC_TBR_BORDER_VERT_DIV ,0,100,10);
 tbrSetRange(IDC_TBR_BORDER_BRIGHTNESS ,0,255,10);

 addHint(IDC_TBR_BORDER_BRIGHTNESS, _l("Brightness x is interpreted as RGB(x,x,x) and converted to luma using the settings in RGB conversion page, if necessary."));
}

void TresizeBordersPage::cfg2dlg(void)
{
 int inside=cfgGet(IDFF_bordersInside);
 setCheck(IDC_RBT_BORDER_INSIDE,inside==1);setCheck(IDC_RBT_BORDER_OUTSIDE,inside==0);
 int units=cfgGet(IDFF_bordersUnits);
 int lock=cfgGet(IDFF_bordersLocked);
 setCheck(IDC_CHB_BORDERSLOCKED,lock);enable(!lock,IDC_LBL_BORDER_VERT);
 static const int idHorizPercent[]={IDC_LBL_BORDER_HORIZ_PERCENT,IDC_TBR_BORDER_HORIZ_PERCENT,0};
 static const int idVertPercent[] ={IDC_LBL_BORDER_VERT_PERCENT ,IDC_TBR_BORDER_VERT_PERCENT ,0};
 static const int idHorizPixels[]={IDC_LBL_BORDER_HORIZ_PIXELS,IDC_TBR_BORDER_HORIZ_PIXELS,0};
 static const int idVertPixels[] ={IDC_LBL_BORDER_VERT_PIXELS ,IDC_TBR_BORDER_VERT_PIXELS ,0};
 setCheck(IDC_RBT_BORDER_PERCENT,units==0);enable(units==0,idHorizPercent);enable(units==0 && !lock,idVertPercent);
 setCheck(IDC_RBT_BORDER_PIXELS ,units==1);enable(units==1,idHorizPixels );enable(units==1 && !lock,idVertPixels );
 int x;
 x=cfgGet(IDFF_bordersPercentX);tbrSet(IDC_TBR_BORDER_HORIZ_PERCENT,x);setText(IDC_LBL_BORDER_HORIZ_PERCENT,_l("%i%%"),x);
 x=cfgGet(IDFF_bordersPercentY);tbrSet(IDC_TBR_BORDER_VERT_PERCENT ,x);setText(IDC_LBL_BORDER_VERT_PERCENT ,_l("%i%%"),x);
 tbrSet(IDC_TBR_BORDER_HORIZ_PIXELS,cfgGet(IDFF_bordersPixelsX),IDC_LBL_BORDER_HORIZ_PIXELS);
 tbrSet(IDC_TBR_BORDER_VERT_PIXELS ,cfgGet(IDFF_bordersPixelsY),IDC_LBL_BORDER_VERT_PIXELS );
 div2dlg();
}

void TresizeBordersPage::div2dlg(void)
{
 int x;
 x=cfgGet(IDFF_bordersDivX);
 tbrSet(IDC_TBR_BORDER_HORIZ_DIV,x);
 setText(IDC_LBL_BORDER_LEFT_NUM,_l("%i"),x);
 setText(IDC_LBL_BORDER_RIGHT_NUM,_l("%i"),100-x);
 int y;
 y=cfgGet(IDFF_bordersDivY);
 tbrSet(IDC_TBR_BORDER_VERT_DIV,y);
 setText(IDC_LBL_BORDER_TOP_NUM,_l("%i"),y);
 setText(IDC_LBL_BORDER_BOTTOM_NUM,_l("%i"),100-y);
 int brightness;
 brightness=cfgGet(IDFF_bordersBrightness);
 tbrSet(IDC_TBR_BORDER_BRIGHTNESS,brightness);
 setText(IDC_LBL_BORDER_BRIGHTNESS_NUM,_l("%i"),brightness);
}

INT_PTR TresizeBordersPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_BORDER_HORIZ_PERCENT:
      case IDC_TBR_BORDER_VERT_PERCENT:
       cfgSet(IDFF_bordersPercentX,tbrGet(IDC_TBR_BORDER_HORIZ_PERCENT));
       cfgSet(IDFF_bordersPercentY,tbrGet(cfgGet(IDFF_bordersLocked)?IDC_TBR_BORDER_HORIZ_PERCENT:IDC_TBR_BORDER_VERT_PERCENT));
       cfg2dlg();
       return TRUE;
      case IDC_TBR_BORDER_HORIZ_PIXELS:
      case IDC_TBR_BORDER_VERT_PIXELS:
       cfgSet(IDFF_bordersPixelsX,tbrGet(IDC_TBR_BORDER_HORIZ_PIXELS));
       cfgSet(IDFF_bordersPixelsY,tbrGet(cfgGet(IDFF_bordersLocked)?IDC_TBR_BORDER_HORIZ_PIXELS:IDC_TBR_BORDER_VERT_PIXELS));
       cfg2dlg();
       return TRUE;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

void TresizeBordersPage::onLock(void)
{
 cfgSet(IDFF_bordersLocked,getCheck(IDC_CHB_BORDERSLOCKED));
 cfgSet(IDFF_bordersPercentY,cfgGet(IDFF_bordersPercentX));
 cfgSet(IDFF_bordersPixelsY,cfgGet(IDFF_bordersPixelsX));
 cfg2dlg();
}

Twidget* TresizeBordersPage::createDlgItem(int id,HWND h)
{
 if (id==IDC_TBR_BORDER_HORIZ_PIXELS || id==IDC_TBR_BORDER_HORIZ_PERCENT || id==IDC_TBR_BORDER_VERT_PIXELS || id==IDC_TBR_BORDER_VERT_PERCENT)
  return new TwidgetSubclassTbr(h,this,NULL);
 else
  return TconfPageDecVideo::createDlgItem(id,h);
}
int TresizeBordersPage::getTbrIdff(int id,const TbindTrackbars bind)
{
 switch (id)
  {
   case IDC_TBR_BORDER_HORIZ_PERCENT:return IDFF_bordersPercentX;
   case IDC_TBR_BORDER_VERT_PERCENT :return IDFF_bordersPercentY;
   case IDC_TBR_BORDER_HORIZ_PIXELS :return IDFF_bordersPixelsX;
   case IDC_TBR_BORDER_VERT_PIXELS  :return IDFF_bordersPixelsY;
   default:return 0;
  }
}

TresizeBordersPage::TresizeBordersPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff,2)
{
 dialogId=IDD_RESIZEBORDERS;
 static const TbindRadiobutton<TresizeBordersPage> rbt[]=
  {
   IDC_RBT_BORDER_INSIDE ,IDFF_bordersInside,1,&TresizeBordersPage::cfg2dlg,
   IDC_RBT_BORDER_OUTSIDE,IDFF_bordersInside,0,&TresizeBordersPage::cfg2dlg,
   IDC_RBT_BORDER_PERCENT,IDFF_bordersUnits,0,&TresizeBordersPage::cfg2dlg,
   IDC_RBT_BORDER_PIXELS ,IDFF_bordersUnits,1,&TresizeBordersPage::cfg2dlg,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindButton<TresizeBordersPage> bt[]=
  {
   IDC_CHB_BORDERSLOCKED,&TresizeBordersPage::onLock,
   0,NULL
  };
 bindButtons(bt);
 static const TbindTrackbar<TresizeBordersPage> htbr[]=
  {
   IDC_TBR_BORDER_HORIZ_DIV,IDFF_bordersDivX,&TresizeBordersPage::div2dlg,
   IDC_TBR_BORDER_VERT_DIV,IDFF_bordersDivY,&TresizeBordersPage::div2dlg,
   IDC_TBR_BORDER_BRIGHTNESS,IDFF_bordersBrightness,&TresizeBordersPage::div2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
}
