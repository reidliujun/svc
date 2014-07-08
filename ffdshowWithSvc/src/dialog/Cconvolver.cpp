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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Cconvolver.h"
#include "TsampleFormat.h"
#include <shlwapi.h>

const int TconvolverPage::speakersIdffs[]={IDFF_convolverFileL,IDFF_convolverFileR,IDFF_convolverFileC,IDFF_convolverFileLFE,IDFF_convolverFileSL,IDFF_convolverFileSR};

void TconvolverPage::init(void)
{
 tbrSetRange(IDC_TBR_CONVOLVER_MIXINGSTRENGTH,0,100);
 tbrSetRange(IDC_TBR_CONVOLVER_LEVELADJUSTDB,-300,300);
 hlv=GetDlgItem(m_hwnd,IDC_LV_CONVOLVER_FILES);
 int ncol=0;
 ListView_AddCol(hlv,ncol,cfgGet(IDFF_lvConvolverWidth0),_(IDC_LV_CONVOLVER_FILES,_l("Channel")),false);
 ListView_AddCol(hlv,ncol,cfgGet(IDFF_lvConvolverWidth1),_(IDC_LV_CONVOLVER_FILES,_l("File")),false);
 ListView_AddCol(hlv,ncol,cfgGet(IDFF_lvConvolverWidth2),_(IDC_LV_CONVOLVER_FILES,_l("Format")),false);
 ListView_SetExtendedListViewStyle(hlv,ListView_GetExtendedListViewStyle(hlv)|LVS_EX_FULLROWSELECT);
 ListView_SetItemCount(hlv,6);
 lvSetSelItem(IDC_LV_CONVOLVER_FILES,cfgGet(IDFF_lvConvolverSelected));
}

void TconvolverPage::cfg2dlg(void)
{
 const char_t *flnm=cfgGetStr(IDFF_convolverFile);
 setDlgItemText(m_hwnd,IDC_ED_CONVOLVER_FILE,flnm);
 format2dlg(flnm);
 int x=cfgGet(IDFF_convolverMixingStrength);
 tbrSet(IDC_TBR_CONVOLVER_MIXINGSTRENGTH,x);
 setText(IDC_LBL_CONVOLVER_MIXINGSTRENGTH,_l("%s %i%%"),_(IDC_LBL_CONVOLVER_MIXINGSTRENGTH),x);
 mapping2dlg();
 adjust2dlg();
}
void TconvolverPage::mapping2dlg(void)
{
 int mapping=cfgGet(IDFF_convolverMappingMode);
 setCheck(IDC_CHB_CONVOLVER_MAPPING,mapping);
 static const int idAuto[]={IDC_LBL_CONVOLVER_FILE,IDC_ED_CONVOLVER_FILE,IDC_BT_CONVOLVER_FILE,IDC_LBL_CONVOLVER_WAV_FORMAT2,0};
 static const int idManual[]={IDC_LV_CONVOLVER_FILES,IDC_BT_CONVOLVER_FILES_SET,IDC_BT_CONVOLVER_FILES_CLEAR,0};
 enable(mapping==0,idAuto);
 enable(mapping!=0,idManual);
 enableMapping(mapping!=0 && lvGetSelItem(IDC_LV_CONVOLVER_FILES)!=-1);
}
void TconvolverPage::enableMapping(bool is)
{
 static const int idFilesBt[]={IDC_BT_CONVOLVER_FILES_SET,IDC_BT_CONVOLVER_FILES_CLEAR,0};
 enable(is,idFilesBt);
}

ffstring TconvolverPage::getWavDesc(const char_t *flnm,int idc)
{
 TwavReader<float> wav(flnm,true);
 if (wav.empty())
  return _(idc,_l("invalid"));
 else
  {
   char_t desc[256];
   wav.descriptionPCM(desc,256);
   return desc+ffstring(_l(", "))+ffstring::intToStr((int)wav[0].size())+_l(" samples");
  }
}
void TconvolverPage::format2dlg(const char_t *flnm)
{
 setDlgItemText(m_hwnd,IDC_LBL_CONVOLVER_WAV_FORMAT2,getWavDesc(flnm,IDC_LBL_CONVOLVER_WAV_FORMAT2).c_str());
}
void TconvolverPage::adjust2dlg(void)
{
 int x=cfgGet(IDFF_convolverLevelAdjustDB);
 tbrSet(IDC_TBR_CONVOLVER_LEVELADJUSTDB,x);
 setText(IDC_LBL_CONVOLVER_LEVELADJUSTDB,_l("%s %-.1f dB"),_(IDC_LBL_CONVOLVER_LEVELADJUSTDB),x/10.0f);
 int aut=cfgGet(IDFF_convolverLevelAdjustAuto);
 setCheck(IDC_CHB_CONVOLVER_LEVELADJUST_AUTO,aut);
 static const int idAdjust[]={IDC_LBL_CONVOLVER_LEVELADJUSTDB,IDC_TBR_CONVOLVER_LEVELADJUSTDB,0};
 enable(!aut,idAdjust);
}

INT_PTR TconvolverPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    cfgSet(IDFF_lvConvolverWidth0,ListView_GetColumnWidth(hlv,0));
    cfgSet(IDFF_lvConvolverWidth1,ListView_GetColumnWidth(hlv,1));
    cfgSet(IDFF_lvConvolverWidth2,ListView_GetColumnWidth(hlv,2));
    cfgSet(IDFF_lvConvolverSelected,lvGetSelItem(IDC_LV_CONVOLVER_FILES));
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_CONVOLVER_FILE:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t flnm[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_CONVOLVER_FILE,flnm,MAX_PATH);
         cfgSet(IDFF_convolverFile,flnm);
         format2dlg(flnm);
        }
       return TRUE;
     }
    break;
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (nmhdr->hwndFrom==hlv && nmhdr->idFrom==IDC_LV_CONVOLVER_FILES)
      switch (nmhdr->code)
       {
        case LVN_ITEMCHANGED:
         {
          NMLISTVIEW *nmlv=LPNMLISTVIEW(lParam);
          enableMapping(nmlv->iItem!=-1);
          return TRUE;
         }
        case NM_DBLCLK:
         {
          NMITEMACTIVATE *nmia=(NMITEMACTIVATE*)lParam;
          if (nmia->iItem==-1 || nmia->iSubItem!=1) return FALSE;
          onManualFileSet();
          return TRUE;
         }
        case LVN_GETDISPINFO:
         {
          NMLVDISPINFO *nmdi=(NMLVDISPINFO*)lParam;
          int i=nmdi->item.iItem;
          if (i==-1) break;
          if (nmdi->item.mask&LVIF_TEXT)
           switch (nmdi->item.iSubItem)
            {
             case 0:
              {
               static const int speakers[]={SPEAKER_FRONT_LEFT,SPEAKER_FRONT_RIGHT,SPEAKER_FRONT_CENTER,SPEAKER_LOW_FREQUENCY,SPEAKER_BACK_LEFT,SPEAKER_BACK_RIGHT};
               strcpy(nmdi->item.pszText,_(IDC_LV_CONVOLVER_FILES,TsampleFormat::getSpeakerName(speakers[i])));
               break;
              }
             case 1:
              {
               int dx=ListView_GetColumnWidth(hlv,1);
               char_t flnm[MAX_PATH];cfgGet(speakersIdffs[i],flnm,MAX_PATH);
               HDC dc=GetDC(hlv);
               HGDIOBJ old=SelectObject(dc,(HGDIOBJ)SendMessage(hlv,WM_GETFONT,0,0));
               PathCompactPath(dc,flnm,std::max(20,dx-15));
               SelectObject(dc,old);
               ReleaseDC(hlv,dc);
               strcpy(nmdi->item.pszText,flnm);
               break;
              }
             case 2:
              if (const char_t *flnm=cfgGetStr(speakersIdffs[i]))
               if (flnm[0] && !speakerFormats[i])
                speakerFormats[i]=getWavDesc(flnm,IDC_LV_CONVOLVER_FILES);
              strcpy(nmdi->item.pszText,speakerFormats[i].c_str());
              break;
            }
          return TRUE;
         }
       }
     break;
    }
  }
 return TconfPageDecAudio::msgProc(uMsg,wParam,lParam);
}

ffstring TconvolverPage::selectFile(int idff,int idc)
{
 char_t flnm[MAX_PATH];cfgGet(idff,flnm,MAX_PATH);
 if (dlgGetFile(false,m_hwnd,_(-IDD_CONVOLVER,_l("Select impulse file")),_l("Wave files (*.wav,*.wv)\0*.wav;*.wv\0All files (*.*)\0*.*\0"),_l("wav"),flnm,_l("."),0))
  {
   if (idc) setDlgItemText(m_hwnd,idc,flnm);
   cfgSet(idff,flnm);
   return flnm;
  }
 else
  return ffstring();
}
void TconvolverPage::onFile(void)
{
 if (ffstring flnm=selectFile(IDFF_convolverFile,IDC_ED_CONVOLVER_FILE))
  format2dlg(flnm.c_str());
}
void TconvolverPage::onManualFileSet(void)
{
 int i=lvGetSelItem(IDC_LV_CONVOLVER_FILES);
 if (i!=-1)
  if (ffstring flnm=selectFile(speakersIdffs[i]))
   {
    speakerFormats[i]=getWavDesc(flnm.c_str(),IDC_LV_CONVOLVER_FILES);
    ListView_RedrawItems(hlv,i,i);
   }
}
void TconvolverPage::onManualFileClear(void)
{
 int i=lvGetSelItem(IDC_LV_CONVOLVER_FILES);
 if (i!=-1)
  {
   cfgSet(speakersIdffs[i],_l(""));
   speakerFormats[i].clear();
   ListView_RedrawItems(hlv,i,i);
  }
}

void TconvolverPage::translate(void)
{
 TconfPageDecAudio::translate();
 ListView_SetColumnText(hlv,0,_(IDC_LV_CONVOLVER_FILES,_l("Channel")));
 ListView_SetColumnText(hlv,1,_(IDC_LV_CONVOLVER_FILES,_l("File")));
 ListView_SetColumnText(hlv,2,_(IDC_LV_CONVOLVER_FILES,_l("Format")));
}

TconvolverPage::TconvolverPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecAudio(Iparent,idff)
{
 resInter=IDC_CHB_CONVOLVER;
 static const TbindButton<TconvolverPage> bt[]=
  {
   IDC_BT_CONVOLVER_FILE,&TconvolverPage::onFile,
   IDC_BT_CONVOLVER_FILES_SET,&TconvolverPage::onManualFileSet,
   IDC_BT_CONVOLVER_FILES_CLEAR,&TconvolverPage::onManualFileClear,
   0,NULL
  };
 bindButtons(bt);
 static const TbindCheckbox<TconvolverPage> chb[]=
  {
   IDC_CHB_CONVOLVER_LEVELADJUST_AUTO,IDFF_convolverLevelAdjustAuto,&TconvolverPage::adjust2dlg,
   IDC_CHB_CONVOLVER_MAPPING,IDFF_convolverMappingMode,&TconvolverPage::mapping2dlg,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TconvolverPage> htbr[]=
  {
   IDC_TBR_CONVOLVER_MIXINGSTRENGTH,IDFF_convolverMixingStrength,&TconvolverPage::cfg2dlg,
   IDC_TBR_CONVOLVER_LEVELADJUSTDB,IDFF_convolverLevelAdjustDB,&TconvolverPage::adjust2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
}

