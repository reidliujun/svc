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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "TsubtitlesSettings.h"
#include "TbitmapSettings.h"
#include "Cbitmap.h"

void TbitmapPage::init(void)
{
 edLimitText(IDC_ED_BITMAP_FLNM,MAX_PATH);
 tbrSetRange(IDC_TBR_BITMAP_POSX,0,100,10);
 tbrSetRange(IDC_TBR_BITMAP_POSY,0,100,10);
 tbrSetRange(IDC_TBR_BITMAP_OPACITY,0,256);
}

void TbitmapPage::cfg2dlg(void)
{
 setDlgItemText(m_hwnd,IDC_ED_BITMAP_FLNM,cfgGetStr(IDFF_bitmapFlnm));
 pos2dlg();
 cbxSetCurSel(IDC_CBX_BITMAP_ALIGN,cfgGet(IDFF_bitmapAlign));
 cbxSetCurSel(IDC_CBX_BITMAP_MODE,cfgGet(IDFF_bitmapMode));
 opacity2dlg();
}
void TbitmapPage::pos2dlg(void)
{
 char_t s[260];int x;

 x=cfgGet(IDFF_bitmapPosx);
 TsubtitlesSettings::getPosHoriz(x, s, this, IDC_LBL_BITMAP_POSX, countof(s));
 setDlgItemText(m_hwnd,IDC_LBL_BITMAP_POSX,s);
 tbrSet(IDC_TBR_BITMAP_POSX,x);

 x=cfgGet(IDFF_bitmapPosy);
 TsubtitlesSettings::getPosVert(x, s, this, IDC_LBL_BITMAP_POSY, countof(s));
 setDlgItemText(m_hwnd,IDC_LBL_BITMAP_POSY,s);
 tbrSet(IDC_TBR_BITMAP_POSY,x);
}
void TbitmapPage::opacity2dlg(void)
{
 int o=cfgGet(IDFF_bitmapStrength);
 tbrSet(IDC_TBR_BITMAP_OPACITY,o);
 setText(IDC_LBL_BITMAP_OPACITY,_l("%s %i%%"),_(IDC_LBL_BITMAP_OPACITY),100*o/256);
}

INT_PTR TbitmapPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_BITMAP_FLNM:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t flnm[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_BITMAP_FLNM,flnm,MAX_PATH);
         cfgSet(IDFF_bitmapFlnm,flnm);
        }
       return TRUE;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

void TbitmapPage::onFlnm(void)
{
 char_t flnm[MAX_PATH];cfgGet(IDFF_bitmapFlnm,flnm,MAX_PATH);
 if (dlgGetFile(false,m_hwnd,_(-IDD_BITMAP,_l("Load image file")),_l("All supported (*.jpg,*.bmp,*.gif,*.png)\0*.bmp;*.jpg;*.jpeg;*.gif;*.png\0Windows Bitmap (*.bmp)\0*.bmp\0JPEG (*.jpg)\0*.jpg\0Compuserve Graphics Interchange (*.gif)\0*.gif\0Portable Network Graphics (*.png)\0*.png"),_l("bmp"),flnm,_l("."),0))
  {
   setDlgItemText(m_hwnd,IDC_ED_BITMAP_FLNM,flnm);
   cfgSet(IDFF_bitmapFlnm,flnm);
  }
}

void TbitmapPage::translate(void)
{
 TconfPageBase::translate();

 cbxTranslate(IDC_CBX_BITMAP_ALIGN,TsubtitlesSettings::alignments);
 cbxTranslate(IDC_CBX_BITMAP_MODE,TbitmapSettings::modes);
}

TbitmapPage::TbitmapPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_BITMAP;
 static const TbindTrackbar<TbitmapPage> htbr[]=
  {
   IDC_TBR_BITMAP_POSX,IDFF_bitmapPosx,&TbitmapPage::pos2dlg,
   IDC_TBR_BITMAP_POSY,IDFF_bitmapPosy,&TbitmapPage::pos2dlg,
   IDC_TBR_BITMAP_OPACITY,IDFF_bitmapStrength,&TbitmapPage::opacity2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindCombobox<TbitmapPage> cbx[]=
  {
   IDC_CBX_BITMAP_ALIGN,IDFF_bitmapAlign,BINDCBX_SEL,NULL,
   IDC_CBX_BITMAP_MODE,IDFF_bitmapMode,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
 static const TbindButton<TbitmapPage> bt[]=
  {
   IDC_BT_BITMAP_FLNM,&TbitmapPage::onFlnm,
   0,NULL
  };
 bindButtons(bt);
}

