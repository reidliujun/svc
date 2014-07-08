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
#include "Clogoaway.h"
#include "TlogoawaySettings.h"

const TlogoawayPage::Tcbxs TlogoawayPage::cbxs=
{
 IDC_CBX_LOGOAWAY_BORDER_N,IDFF_logoawayBordn_mode,
 IDC_CBX_LOGOAWAY_BORDER_S,IDFF_logoawayBords_mode,
 IDC_CBX_LOGOAWAY_BORDER_E,IDFF_logoawayBorde_mode,
 IDC_CBX_LOGOAWAY_BORDER_W,IDFF_logoawayBordw_mode,
 IDC_CBX_LOGOAWAY_POINT_NE,IDFF_logoawayPointne,
 IDC_CBX_LOGOAWAY_POINT_NW,IDFF_logoawayPointnw,
 IDC_CBX_LOGOAWAY_POINT_SE,IDFF_logoawayPointse,
 IDC_CBX_LOGOAWAY_POINT_SW,IDFF_logoawayPointsw
};

void TlogoawayPage::init(void)
{
 SendDlgItemMessage(m_hwnd,IDC_UD_LOGOAWAY_X ,UDM_SETRANGE,0,MAKELONG(16384,0));
 SendDlgItemMessage(m_hwnd,IDC_UD_LOGOAWAY_Y ,UDM_SETRANGE,0,MAKELONG(16384,0));
 SendDlgItemMessage(m_hwnd,IDC_UD_LOGOAWAY_DX,UDM_SETRANGE,0,MAKELONG(16384,4));
 SendDlgItemMessage(m_hwnd,IDC_UD_LOGOAWAY_DY,UDM_SETRANGE,0,MAKELONG(16384,4));
 tbrSetRange(IDC_TBR_LOGOAWAY_BLUR,0,10,2);
 tbrSetRange(IDC_TBR_LOGOAWAY_XY,0,10,2);
}

void TlogoawayPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_LOGOAWAY_LUMAONLY,cfgGet(IDFF_logoawayLumaOnly));
 SetDlgItemInt(m_hwnd,IDC_ED_LOGOAWAY_X ,deci->getParam2(IDFF_logoawayX ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_LOGOAWAY_Y ,deci->getParam2(IDFF_logoawayY ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_LOGOAWAY_DX,deci->getParam2(IDFF_logoawayDx),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_LOGOAWAY_DY,deci->getParam2(IDFF_logoawayDy),FALSE);
 mode2dlg();
 borders2dlg();
 points2dlg();
}
void TlogoawayPage::mode2dlg(void)
{
 int mode=cfgGet(IDFF_logoawayMode);
 cbxSetDataCurSel(IDC_CBX_LOGOAWAY_MODE,mode);

 static const int idBlur[]={IDC_LBL_LOGOAWAY_BLUR,IDC_TBR_LOGOAWAY_BLUR,0};
 static const int idXY[]={IDC_LBL_LOGOAWAY_XY,IDC_TBR_LOGOAWAY_XY,0};
 static const int idSolid[]={IDC_LBL_LOGOAWAY_SOLIDCOLOR,IDC_IMG_LOGOAWAY_SOLIDCOLOR,0};
 static const int idParambitmap[]={IDC_LBL_LOGOAWAY_PARAMBITMAP,IDC_ED_LOGOAWAY_PARAMBITMAP,IDC_BT_LOGOAWAY_PARAMBITMAP,0};
 if (mode==TlogoawaySettings::MODE_SOLIDFILL)
  {
   show(0,idBlur);show(0,idXY);show(1,idSolid);
  }
 else
  {
   show(0,idSolid);show(1,idBlur);show(1,idXY);
   tbrSet(IDC_TBR_LOGOAWAY_BLUR,cfgGet(IDFF_logoawayBlur),IDC_LBL_LOGOAWAY_BLUR,_(IDC_LBL_LOGOAWAY_BLUR,mode==TlogoawaySettings::MODE_UGLARM || mode==TlogoawaySettings::MODE_SHAPEUGLARM?_l("Exponent:"):_l("Blur:")));
   enable(mode==TlogoawaySettings::MODE_XY || mode==TlogoawaySettings::MODE_SHAPEXY || mode==TlogoawaySettings::MODE_UGLARM || mode==TlogoawaySettings::MODE_SHAPEUGLARM,idBlur);
   tbrSet(IDC_TBR_LOGOAWAY_XY,cfgGet(IDFF_logoawayVhweight),IDC_LBL_LOGOAWAY_XY);
   enable(mode==TlogoawaySettings::MODE_XY || mode==TlogoawaySettings::MODE_SHAPEXY,idXY);
  }
 enable(mode==TlogoawaySettings::MODE_SHAPEXY || mode==TlogoawaySettings::MODE_SHAPEUGLARM,idParambitmap);
 setDlgItemText(m_hwnd,IDC_ED_LOGOAWAY_PARAMBITMAP,cfgGetStr(IDFF_logoawayParamBitmapFlnm));
}
void TlogoawayPage::borders2dlg(void)
{
 for (int i=0;i<4;i++)
  cbxSetDataCurSel(cbxs.borders[i].id,cfgGet(cbxs.borders[i].idff));
}
void TlogoawayPage::points2dlg(void)
{
 for (int i=0;i<4;i++)
  cbxSetDataCurSel(cbxs.points[i].id,cfgGet(cbxs.points[i].idff));
}

INT_PTR TlogoawayPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CBX_LOGOAWAY_MODE:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         cfgSet(IDFF_logoawayMode,(int)cbxGetCurItemData(IDC_CBX_LOGOAWAY_MODE));
         mode2dlg();
         return TRUE;
        }
       break;
      case IDC_CBX_LOGOAWAY_BORDER_PRESETS:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         const TlogoawaySettings::TborderPreset &preset=TlogoawaySettings::borderPresets[cbxGetCurSel(IDC_CBX_LOGOAWAY_BORDER_PRESETS)];
         cfgSet(IDFF_logoawayBordn_mode,preset.n);
         cfgSet(IDFF_logoawayBordw_mode,preset.w);
         cfgSet(IDFF_logoawayBorde_mode,preset.e);
         cfgSet(IDFF_logoawayBords_mode,preset.s);
         borders2dlg();
         return TRUE;
        }
       break;
      case IDC_IMG_LOGOAWAY_SOLIDCOLOR:
       if (HIWORD(wParam)==STN_CLICKED)
        {
         if (chooseColor(IDFF_logoawaySolidcolor))
          InvalidateRect(GetDlgItem(m_hwnd,IDC_IMG_LOGOAWAY_SOLIDCOLOR),NULL,FALSE);
         return TRUE;
        }
       break;
      default:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        for (int i=0;i<8;i++)
         if (LOWORD(wParam)==cbxs.cbxs[i].id)
          {
           cfgSet(cbxs.cbxs[i].idff,(int)cbxGetCurItemData(cbxs.cbxs[i].id));
           return TRUE;
          }
       break;
      case IDC_ED_LOGOAWAY_PARAMBITMAP:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t flnm[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_LOGOAWAY_PARAMBITMAP,flnm,MAX_PATH);
         cfgSet(IDFF_logoawayParamBitmapFlnm,flnm);
        }
       return TRUE;
     }
    break;
   case WM_DRAWITEM:
    if (wParam==IDC_IMG_LOGOAWAY_SOLIDCOLOR)
     {
      LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
      LOGBRUSH lb;
      lb.lbColor=cfgGet(IDFF_logoawaySolidcolor);
      lb.lbStyle=BS_SOLID;
      HBRUSH br=CreateBrushIndirect(&lb);
      FillRect(dis->hDC,&dis->rcItem,br);
      DeleteObject(br);
      return TRUE;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

void TlogoawayPage::onParamFlnm(void)
{
 char_t flnm[MAX_PATH];cfgGet(IDFF_logoawayParamBitmapFlnm,flnm,MAX_PATH);
 if (dlgGetFile(false,m_hwnd,_(-IDD_LOGOAWAY,_l("Load image file")),_l("All supported (*.jpg,*.bmp,*.gif)\0*.bmp;*.jpg;*.jpeg;*.gif\0Windows Bitmap (*.bmp)\0*.bmp\0JPEG (*.jpg)\0*.jpg\0Compuserve Graphics Interchange (*.gif)\0*.gif\0"),_l("bmp"),flnm,_l("."),0))
  {
   setDlgItemText(m_hwnd,IDC_ED_LOGOAWAY_PARAMBITMAP,flnm);
   cfgSet(IDFF_logoawayParamBitmapFlnm,flnm);
  }
}

void TlogoawayPage::translate(void)
{
 TconfPageBase::translate();

 int sel=cbxGetCurSel(IDC_CBX_LOGOAWAY_MODE);
 cbxClear(IDC_CBX_LOGOAWAY_MODE);
 for (int i=0;TlogoawaySettings::modes[i].name;i++)
  cbxAdd(IDC_CBX_LOGOAWAY_MODE,_(IDC_CBX_LOGOAWAY_MODE,TlogoawaySettings::modes[i].name),TlogoawaySettings::modes[i].id);
 cbxSetCurSel(IDC_CBX_LOGOAWAY_MODE,sel);

 sel=cbxGetCurSel(IDC_CBX_LOGOAWAY_BORDER_PRESETS);
 cbxClear(IDC_CBX_LOGOAWAY_BORDER_PRESETS);
 for (int i=0;TlogoawaySettings::borderPresets[i].name;i++)
  cbxAdd(IDC_CBX_LOGOAWAY_BORDER_PRESETS,_(IDC_CBX_LOGOAWAY_BORDER_PRESETS,TlogoawaySettings::borderPresets[i].name));
 cbxSetCurSel(IDC_CBX_LOGOAWAY_BORDER_PRESETS,sel==CB_ERR?0:sel);

 int cbx;
 for (cbx=0;cbx<4;cbx++)
  {
   sel=cbxGetCurSel(cbxs.points[cbx].id);
   cbxClear(cbxs.points[cbx].id);
   for (int i=5;TlogoawaySettings::navigators[i];i++)
    cbxAdd(cbxs.points[cbx].id,_(cbxs.points[cbx].id,TlogoawaySettings::navigators[i]),i);
   cbxSetCurSel(cbxs.points[cbx].id,sel);
  }
 for (cbx=0;cbx<4;cbx++)
  {
   sel=cbxGetCurSel(cbxs.borders[cbx].id);
   cbxClear(cbxs.borders[cbx].id);
   for (int i=1;TlogoawaySettings::borderModes[i];i++)
    cbxAdd(cbxs.borders[cbx].id,_(cbxs.borders[cbx].id,TlogoawaySettings::borderModes[i]),i);
   cbxSetCurSel(cbxs.borders[cbx].id,sel);
  }
}

TlogoawayPage::TlogoawayPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_LOGOAWAY;
 static const TbindCheckbox<TlogoawayPage> chb[]=
  {
   IDC_CHB_LOGOAWAY_LUMAONLY,IDFF_logoawayLumaOnly,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TlogoawayPage> htbr[]=
  {
   IDC_TBR_LOGOAWAY_BLUR,IDFF_logoawayBlur,&TlogoawayPage::mode2dlg,
   IDC_TBR_LOGOAWAY_XY,IDFF_logoawayVhweight,&TlogoawayPage::mode2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindEditInt<TlogoawayPage> edInt[]=
  {
   IDC_ED_LOGOAWAY_X ,0,16384,IDFF_logoawayX ,NULL,
   IDC_ED_LOGOAWAY_Y ,0,16384,IDFF_logoawayY ,NULL,
   IDC_ED_LOGOAWAY_DX,4,16384,IDFF_logoawayDx,NULL,
   IDC_ED_LOGOAWAY_DY,4,16384,IDFF_logoawayDy,NULL,
   0
  };
 bindEditInts(edInt);
 static const TbindButton<TlogoawayPage> bt[]=
  {
   IDC_BT_LOGOAWAY_PARAMBITMAP,&TlogoawayPage::onParamFlnm,
   0,NULL
  };
 bindButtons(bt);
}
