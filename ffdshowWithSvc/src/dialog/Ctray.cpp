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
#include "Ctray.h"
#include "Ttranslate.h"
#include "TffdshowPageBase.h"

void TdlgMiscPage::init(void)
{
 for (Ttranslate::Tlanglist::const_iterator i=tr->langlist.begin();i!=tr->langlist.end();i++)
  {
   char_t desc[256];tr->langlist.getLangName(i,desc,256);
   cbxAdd(IDC_CBX_LANG,desc,Ttranslate::lang2int(i->first.c_str()));
  }
 edLimitText(IDC_ED_PATH_FFDSHOW,MAX_PATH);
 edLimitText(IDC_ED_PATH_DSCALER,MAX_PATH);
 static const int idDscaler[]={IDC_LBL_PATH_DSCALER,IDC_ED_PATH_DSCALER,IDC_BT_PATH_DSCALER,0};
 enable((filterMode&IDFF_FILTERMODE_AUDIO)==0,idDscaler);
}

void TdlgMiscPage::cfg2dlg(void)
{
 tray2dlg();
 setCheck(IDC_CHB_DLG_RESTOREPOS,cfgGet(IDFF_dlgRestorePos));
 setCheck(IDC_CHB_DLG_SHOWHINTS ,cfgGet(IDFF_showHints));
 int langint=Ttranslate::lang2int(cfgGetStr(IDFF_lang));
 if(langint==20554/*JP*/ || langint==16714/*JA*/)
#ifdef UNICODE
  langint=16714;// JA Unicode
#else
  langint=20554;// JP ANSI
#endif
 cbxSetDataCurSel(IDC_CBX_LANG,langint);
 paths2dlg();
}
void TdlgMiscPage::tray2dlg(void)
{
 int tray=cfgGet(IDFF_trayIcon);
 if (tray)
  {
   int trayType=cfgGet(IDFF_trayIconType);
   setCheck(IDC_RBT_ICON_MODERN,trayType==1);
   setCheck(IDC_RBT_ICON_CLASSIC,trayType==2);
  }
 else
  {
   setCheck(IDC_RBT_ICON_NONE,1);
  }
 setCheck(IDC_CHB_TRAYICONEXT,cfgGet(IDFF_trayIconExt));enable(tray,IDC_CHB_TRAYICONEXT);
 setCheck(IDC_CHB_STREAMSMENU,cfgGet(IDFF_streamsOptionsMenu));enable((filterMode&(IDFF_FILTERMODE_ENC|IDFF_FILTERMODE_VFW))==0,IDC_CHB_STREAMSMENU);
}
void TdlgMiscPage::paths2dlg(void)
{
 setText(IDC_ED_PATH_FFDSHOW,cfgGetStr(IDFF_installPath));
 setText(IDC_ED_PATH_DSCALER,cfgGetStr(IDFF_dscalerPath));
}

INT_PTR TdlgMiscPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_DLG_SHOWHINTS:
       parent->enableHints(!!getCheck(IDC_CHB_DLG_SHOWHINTS));
       return TRUE;
      case IDC_CBX_LANG:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         const char_t *newlang=cbxGetCurText(IDC_CBX_LANG);
         if (newlang[0])
          {
           int langint=(int)cbxGetCurItemData(IDC_CBX_LANG);
           if(langint==20554/*JP*/ || langint==16714/*JA*/)
#ifdef UNICODE
            langint=16714;// JA Unicode
#else
            langint=20554;// JP ANSI
#endif
           Twindow::cfgSet(IDFF_lang,text<char_t>((const char*)&langint));
           parent->translate();
          }
         return TRUE;
        }
       break;
      case IDC_BT_PATH_DSCALER:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         if (changeDir(IDFF_dscalerPath,_(-IDD_DLGMISC,_l("Select directory with installed DScaler"))))
          paths2dlg();
         return TRUE;
        }
       break;
      case IDC_BT_PATH_FFDSHOW:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         if (changeDir(IDFF_installPath,_(-IDD_DLGMISC,_l("Select ffdshow instalation directory"))))
          paths2dlg();
         return TRUE;
        }
       break;
     }
    break;
  }
 return TconfPageBase::msgProc(uMsg,wParam,lParam);
}

void TdlgMiscPage::onIconTypeChange(void)
{
 int it=cfgGet(IDFF_trayIconType);
 if (it==0)
  {
   cfgSet(IDFF_trayIcon,0);
  }
 else
  {
   cfgSet(IDFF_trayIconChanged,1);
   cfgSet(IDFF_trayIcon,1);
  }
}

TdlgMiscPage::TdlgMiscPage(TffdshowPageBase *Iparent):TconfPageBase(Iparent)
{
 dialogId=IDD_DLGMISC;
 static const TbindCheckbox<TdlgMiscPage> chb[]=
  {
   IDC_CHB_TRAYICONEXT,IDFF_trayIconExt,NULL,
   IDC_CHB_DLG_RESTOREPOS,IDFF_dlgRestorePos,NULL,
   IDC_CHB_STREAMSMENU,IDFF_streamsOptionsMenu,NULL,
   IDC_CHB_TRAYICON,IDFF_trayIcon,&TdlgMiscPage::tray2dlg,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindRadiobutton<TdlgMiscPage> rbt[]=
  {
   IDC_RBT_ICON_NONE,IDFF_trayIconType,0,&TdlgMiscPage::onIconTypeChange,
   IDC_RBT_ICON_MODERN,IDFF_trayIconType,1,&TdlgMiscPage::onIconTypeChange,
   IDC_RBT_ICON_CLASSIC,IDFF_trayIconType,2,&TdlgMiscPage::onIconTypeChange,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
}
