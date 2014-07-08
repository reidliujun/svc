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
#include "CpictProp.h"
#include "TpictPropSettings.h"

const char_t* TpictPropPage::scanlines[]=
{
 _l("No"),
 _l("1"),
 _l("2"),
 _l("3"),
 _l("4"),
 NULL
};

void TpictPropPage::init(void)
{
 tbrSetRange(IDC_TBR_LUMGAIN,0,256,32);
 tbrSetRange(IDC_TBR_LUMOFFSET,-256,256,64);
 tbrSetRange(IDC_TBR_GAMMA,1,400,40);
 tbrSetRange(IDC_TBR_GAMMAR,1,400,40);
 tbrSetRange(IDC_TBR_GAMMAG,1,400,40);
 tbrSetRange(IDC_TBR_GAMMAB,1,400,40);
 tbrSetRange(IDC_TBR_HUE,-180,180,20);
 tbrSetRange(IDC_TBR_SATURATION,0,128,8);
 tbrSetRange(IDC_TBR_COLORIZESTRENGTH,0,255,16);
}

void TpictPropPage::cfg2dlg(void)
{
 lumGain2dlg();
 lumOffset2dlg();
 cbxSetCurSel(IDC_CBX_SCANLINE,cfgGet(IDFF_scanlineEffect));
 gamma2dlg();
 gammaRGB2dlg();
 hue2dlg();
 saturation2dlg();
 colorize2dlg();
 repaint(GetDlgItem(m_hwnd,IDC_IMG_COLORIZE));
 levelfix2dlg();
}

void TpictPropPage::lumGain2dlg(void)
{
 char_t s[256];int x;
 x=cfgGet(IDFF_lumGain);
 tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s %i"),_(IDC_LBL_LUMGAIN),x);
 if (x==TpictPropSettings::lumGainDef)
  strncatf(s, countof(s),_l(" (%s)"),_(IDC_LBL_LUMGAIN,_l("off")));
 setDlgItemText(m_hwnd,IDC_LBL_LUMGAIN,s);
 tbrSet(IDC_TBR_LUMGAIN,cfgGet(IDFF_lumGain));
}
void TpictPropPage::lumOffset2dlg(void)
{
 char_t s[256];int x;
 x=cfgGet(IDFF_lumOffset);
 tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s %i"), _(IDC_LBL_LUMOFFSET), x);
 if (x==TpictPropSettings::lumOffsetDef)
  strncatf(s, countof(s), _l(" (%s)"), _(IDC_LBL_LUMOFFSET, _l("off")));
 setDlgItemText(m_hwnd,IDC_LBL_LUMOFFSET,s);
 tbrSet(IDC_TBR_LUMOFFSET,cfgGet(IDFF_lumOffset));
}
void TpictPropPage::gamma2dlg(void)
{
 char_t s[256];int x;
 x=cfgGet(IDFF_gammaCorrection);
 tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s %3.2f"),_(IDC_LBL_GAMMA),float(x/100.0));
 if (x==TpictPropSettings::gammaCorrectionDef)
  strncatf(s, countof(s), _l(" (%s)"), _(IDC_LBL_GAMMA, _l("off")));
 setDlgItemText(m_hwnd,IDC_LBL_GAMMA,s);
 tbrSet(IDC_TBR_GAMMA,cfgGet(IDFF_gammaCorrection));
}
void TpictPropPage::gammaRGB2dlg(void)
{
 char_t s[256];int x;
 x=cfgGet(IDFF_gammaCorrectionR);
 tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s %3.2f"),_(IDC_LBL_GAMMARGB),float(x/100.0));
 if (x==TpictPropSettings::gammaCorrectionDef)
  strncatf(s, countof(s), _l(" (%s)"), _(IDC_LBL_GAMMARGB, _l("off")));
 tbrSet(IDC_TBR_GAMMAR,x);
 x=cfgGet(IDFF_gammaCorrectionG);
 strncatf(s, countof(s), _l(", %3.2f"), float(x/100.0));
 if (x==TpictPropSettings::gammaCorrectionDef)
  strncatf(s, countof(s), _l(" (%s)"), _(IDC_TBR_GAMMAR, _l("off")));
 tbrSet(IDC_TBR_GAMMAG,x);
 x=cfgGet(IDFF_gammaCorrectionB);
 strncatf(s, countof(s) ,_l(", %3.2f"), float(x/100.0));
 if (x==TpictPropSettings::gammaCorrectionDef)
  strncatf(s, countof(s), _l(" (%s)"),_(IDC_TBR_GAMMAG,_l("off")));
 setDlgItemText(m_hwnd,IDC_LBL_GAMMARGB,s);
 tbrSet(IDC_TBR_GAMMAB,x);
}
void TpictPropPage::hue2dlg(void)
{
 char_t s[256];int x;
 x=cfgGet(IDFF_hue);
 tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s %i"), _(IDC_LBL_HUE), x);
 if (x==TpictPropSettings::hueDef)
  strncatf(s, countof(s), _l(" (%s)"), _(IDC_LBL_HUE,_l("off")));
 setDlgItemText(m_hwnd,IDC_LBL_HUE,s);
 tbrSet(IDC_TBR_HUE,cfgGet(IDFF_hue));
}
void TpictPropPage::saturation2dlg(void)
{
 char_t s[256];int x;
 x=cfgGet(IDFF_saturation);
 tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s %i"), _(IDC_LBL_SATURATION), x);
 if (x==TpictPropSettings::saturationDef)
  strncatf(s, countof(s), _l(" (%s)") ,_(IDC_LBL_SATURATION,_l("off")));
 setDlgItemText(m_hwnd,IDC_LBL_SATURATION,s);
 tbrSet(IDC_TBR_SATURATION,cfgGet(IDFF_saturation));
}
void TpictPropPage::colorize2dlg(void)
{
 tbrSet(IDC_TBR_COLORIZESTRENGTH,cfgGet(IDFF_colorizeStrength),IDC_LBL_COLORIZESTRENGTH);
 setCheck(IDC_CHB_COLORIZE_CHROMAONLY,cfgGet(IDFF_colorizeChromaonly));
}
void TpictPropPage::levelfix2dlg(void)
{
 int is=cfgGet(IDFF_pictPropLevelFix);
 setCheck(IDC_CHB_PICTPROP_LEVELFIX_LUMA,is);
 setCheck(IDC_CHB_PICTPROP_LEVELFIX_LUMA_FULL,cfgGet(IDFF_pictPropLevelFixFull));enable(is,IDC_CHB_PICTPROP_LEVELFIX_LUMA_FULL);
}

INT_PTR TpictPropPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_IMG_COLORIZE:
       if (HIWORD(wParam)==STN_CLICKED)
        {
         if (chooseColor(IDFF_colorizeColor))
          InvalidateRect(GetDlgItem(m_hwnd,IDC_IMG_COLORIZE),NULL,FALSE);
         return TRUE;
        }
       break;
     }
    break;
   case WM_DRAWITEM:
    if (wParam==IDC_IMG_COLORIZE)
     {
      LPDRAWITEMSTRUCT dis=LPDRAWITEMSTRUCT(lParam);
      LOGBRUSH lb;
      lb.lbColor=cfgGet(IDFF_colorizeColor);
      lb.lbStyle=BS_SOLID;
      HBRUSH br=CreateBrushIndirect(&lb);
      FillRect(dis->hDC,&dis->rcItem,br);
      DeleteObject(br);
      return TRUE;
     }
    else
     break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

void TpictPropPage::translate(void)
{
 TconfPageDecVideo::translate();

 cbxTranslate(IDC_CBX_SCANLINE,scanlines);
}

TpictPropPage::TpictPropPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_PICTPROP;
 static const TbindCheckbox<TpictPropPage> chb[]=
  {
   IDC_CHB_COLORIZE_CHROMAONLY,IDFF_colorizeChromaonly,NULL,
   IDC_CHB_PICTPROP_LEVELFIX_LUMA,IDFF_pictPropLevelFix,&TpictPropPage::levelfix2dlg,
   IDC_CHB_PICTPROP_LEVELFIX_LUMA_FULL,IDFF_pictPropLevelFixFull,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TpictPropPage> htbr[]=
  {
   IDC_TBR_LUMGAIN,IDFF_lumGain,&TpictPropPage::lumGain2dlg,
   IDC_TBR_LUMOFFSET,IDFF_lumOffset,&TpictPropPage::lumOffset2dlg,
   IDC_TBR_GAMMA,IDFF_gammaCorrection,&TpictPropPage::gamma2dlg,
   IDC_TBR_GAMMAR,IDFF_gammaCorrectionR,&TpictPropPage::gammaRGB2dlg,
   IDC_TBR_GAMMAG,IDFF_gammaCorrectionG,&TpictPropPage::gammaRGB2dlg,
   IDC_TBR_GAMMAB,IDFF_gammaCorrectionB,&TpictPropPage::gammaRGB2dlg,
   IDC_TBR_HUE,IDFF_hue,&TpictPropPage::hue2dlg,
   IDC_TBR_SATURATION,IDFF_saturation,&TpictPropPage::saturation2dlg,
   IDC_TBR_COLORIZESTRENGTH,IDFF_colorizeStrength,&TpictPropPage::colorize2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindCombobox<TpictPropPage> cbx[]=
  {
   IDC_CBX_SCANLINE,IDFF_scanlineEffect,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
}
