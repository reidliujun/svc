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
#include "CsubtitlesText.h"
#include "TsubtitleText.h"
#include "TffdshowPageDec.h"
#include "TsubtitlesSettings.h"

void TsubtitlesTextPage::init(void)
{
 tbrSetRange(IDC_TBR_SUB_LINESPACING,0,200,10);
 strings dicts=TtextFixBase::getDicts(config);
 for (strings::const_iterator d=dicts.begin();d!=dicts.end();d++)
  cbxAdd(IDC_CBX_SUBFIX_ORTOGRAPHY,d->c_str());
 cbxSetDroppedWidth(IDC_CBX_SUBFIX_IL,250);
 cbxSetDroppedWidth(IDC_CBX_SUB_WORDWRAP,250);
}

void TsubtitlesTextPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_SUB_OVERLAP,cfgGet(IDFF_subTimeOverlap));
 split2dlg();
 linespacing2dlg();
 min2dlg();
 fix2dlg();
}
void TsubtitlesTextPage::split2dlg(void)
{
 int is=cfgGet(IDFF_fontSplitting);
 setCheck(IDC_CHB_SUB_SPLIT,is);
 SetDlgItemInt(m_hwnd,IDC_ED_SUB_SPLIT_BORDER,cfgGet(IDFF_subSplitBorder),FALSE);
 static const int idBorders[]={IDC_LBL_SUB_SPLIT_BORDER,IDC_ED_SUB_SPLIT_BORDER,IDC_LBL_SUB_SPLIT_BORDER2,0};
 enable(is,idBorders);
 cbxSetCurSel(IDC_CBX_SUB_WORDWRAP,cfgGet(IDFF_subWordWrap));
 enable(is,IDC_CBX_SUB_WORDWRAP);
}
void TsubtitlesTextPage::linespacing2dlg(void)
{
 int ls=cfgGet(IDFF_subLinespacing);
 tbrSet(IDC_TBR_SUB_LINESPACING,ls);
 setText(IDC_LBL_SUB_LINESPACING,_l("%s %i%%"),_(IDC_LBL_SUB_LINESPACING),ls);
}
void TsubtitlesTextPage::min2dlg(void)
{
 int ismin=cfgGet(IDFF_subIsMinDuration);
 setCheck(IDC_CHB_SUB_MINDURATION,ismin);
 static const int mins[]={IDC_CBX_SUB_MINDURATION,IDC_LBL_SUB_MINDURATION,IDC_ED_SUB_MINDURATION,IDC_LBL_SUB_MINDURATION2,0};
 enable(ismin,mins);
 int mintype=cfgGet(IDFF_subMinDurationType);
 cbxSetCurSel(IDC_CBX_SUB_MINDURATION,mintype);
 switch (mintype)
  {
   case 0:SetDlgItemInt(m_hwnd,IDC_ED_SUB_MINDURATION,cfgGet(IDFF_subMinDurationSub ),FALSE);break;
   case 1:SetDlgItemInt(m_hwnd,IDC_ED_SUB_MINDURATION,cfgGet(IDFF_subMinDurationLine),FALSE);break;
   case 2:SetDlgItemInt(m_hwnd,IDC_ED_SUB_MINDURATION,cfgGet(IDFF_subMinDurationChar),FALSE);break;
  }
}
void TsubtitlesTextPage::fix2dlg(void)
{
 int fix=cfgGet(IDFF_subFix);
 setCheck(IDC_CHB_SUBFIX_AP,fix&TtextFixBase::fixAP);
 setCheck(IDC_CHB_SUBFIX_IL,fix&TtextFixBase::fixIl);
 cbxSetCurSel(IDC_CBX_SUBFIX_IL,cfgGet(IDFF_subFixLang));
 enable(fix&TtextFixBase::fixIl,IDC_CBX_SUBFIX_IL);
 setCheck(IDC_CHB_SUBFIX_PUNCTUATION,fix&TtextFixBase::fixPunctuation);
 setCheck(IDC_CHB_SUBFIX_ORTOGRAPHY,fix&TtextFixBase::fixOrtography);
 int ii=cbxFindItem(IDC_CBX_SUBFIX_ORTOGRAPHY,cfgGetStr(IDFF_subFixDict),true);
 if (ii==-1) ii=0;
 cbxSetCurSel(IDC_CBX_SUBFIX_ORTOGRAPHY,ii);
 enable(fix&TtextFixBase::fixOrtography,IDC_CBX_SUBFIX_ORTOGRAPHY);
 setCheck(IDC_CHB_SUBFIX_CAPITAL,fix&TtextFixBase::fixCapital);
 setCheck(IDC_CHB_SUBFIX_NUMBERS,fix&TtextFixBase::fixNumbers);
 setCheck(IDC_CHB_SUBFIX_HEARING,fix&TtextFixBase::fixHearingImpaired);
}

INT_PTR TsubtitlesTextPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_SUB_MINDURATION:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam));
         if (hed!=GetFocus()) return FALSE;
         repaint(hed);
         switch (LOWORD(wParam))
          {
           case IDC_ED_SUB_MINDURATION:
            {
             static const int idffmins[]={IDFF_subMinDurationSub,IDFF_subMinDurationLine,IDFF_subMinDurationChar};
             eval(hed,1,3600000,idffmins[cbxGetCurSel(IDC_CBX_SUB_MINDURATION)]);
             break;
            }
          }
         return TRUE;
        }
       break;
      case IDC_CHB_SUBFIX_AP:
      case IDC_CHB_SUBFIX_IL:
      case IDC_CHB_SUBFIX_PUNCTUATION:
      case IDC_CHB_SUBFIX_ORTOGRAPHY:
      case IDC_CHB_SUBFIX_CAPITAL:
      case IDC_CHB_SUBFIX_NUMBERS:
      case IDC_CHB_SUBFIX_HEARING:
       {
        int fix=0;
        if (getCheck(IDC_CHB_SUBFIX_AP         )) fix|=TtextFixBase::fixAP;
        if (getCheck(IDC_CHB_SUBFIX_IL         )) fix|=TtextFixBase::fixIl;
        if (getCheck(IDC_CHB_SUBFIX_PUNCTUATION)) fix|=TtextFixBase::fixPunctuation;
        if (getCheck(IDC_CHB_SUBFIX_ORTOGRAPHY )) fix|=TtextFixBase::fixOrtography;
        if (getCheck(IDC_CHB_SUBFIX_CAPITAL    )) fix|=TtextFixBase::fixCapital;
        if (getCheck(IDC_CHB_SUBFIX_NUMBERS    )) fix|=TtextFixBase::fixNumbers;
        if (getCheck(IDC_CHB_SUBFIX_HEARING    )) fix|=TtextFixBase::fixHearingImpaired;
        cfgSet(IDFF_subFix,fix);
        fix2dlg();
        break;
       }
     }
    break;
   case WM_CTLCOLOREDIT:
    {
     HWND hwnd=HWND(lParam);
     bool ok;
     switch (getId(hwnd))
      {
       case IDC_ED_SUB_MINDURATION:
        ok=eval(hwnd,1,3600000);break;
       default:return FALSE;
      }
     if (!ok)
      {
       HDC dc=HDC(wParam);
       SetBkColor(dc,RGB(255,0,0));
       return INT_PTR(getRed());
      }
     else return FALSE;
    }
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

void TsubtitlesTextPage::translate(void)
{
 TconfPageBase::translate();

 cbxTranslate(IDC_CBX_SUB_MINDURATION,TsubtitlesSettings::durations);
 cbxTranslate(IDC_CBX_SUBFIX_IL,TsubtitlesSettings::fixIls);
 cbxTranslate(IDC_CBX_SUB_WORDWRAP,TsubtitlesSettings::wordWraps);
}

TsubtitlesTextPage::TsubtitlesTextPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff,4)
{
 dialogId=IDD_SUBTITLESTEXT;
 static const TbindCheckbox<TsubtitlesTextPage> chb[]=
  {
   IDC_CHB_SUB_SPLIT,IDFF_fontSplitting,&TsubtitlesTextPage::split2dlg,
   IDC_CHB_SUB_OVERLAP,IDFF_subTimeOverlap,NULL,
   IDC_CHB_SUB_MINDURATION,IDFF_subIsMinDuration,&TsubtitlesTextPage::min2dlg,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TsubtitlesTextPage> htbr[]=
  {
   IDC_TBR_SUB_LINESPACING,IDFF_subLinespacing,&TsubtitlesTextPage::linespacing2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindCombobox<TsubtitlesTextPage> cbx[]=
  {
   IDC_CBX_SUB_MINDURATION,IDFF_subMinDurationType,BINDCBX_SEL,&TsubtitlesTextPage::min2dlg,
   IDC_CBX_SUBFIX_IL,IDFF_subFixLang,BINDCBX_SEL,NULL,
   IDC_CBX_SUBFIX_ORTOGRAPHY,IDFF_subFixDict,BINDCBX_TEXT,NULL,
   IDC_CBX_SUB_WORDWRAP,IDFF_subWordWrap,BINDCBX_SEL,&TsubtitlesTextPage::split2dlg,
   0
  };
 bindComboboxes(cbx);
 static const TbindEditInt<TsubtitlesTextPage> edInt[]=
  {
   IDC_ED_SUB_SPLIT_BORDER,0,4096,IDFF_subSplitBorder,NULL,
   0
  };
 bindEditInts(edInt);
}
