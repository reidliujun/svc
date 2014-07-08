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
#include "CchannelSwap.h"
#include "TsampleFormat.h"

const TchannelSwapPage::Tspeaker TchannelSwapPage::speakers[]=
{
 IDC_CBX_CHANNELSWAP_L   ,SPEAKER_FRONT_LEFT   ,IDFF_channelSwapL,
 IDC_CBX_CHANNELSWAP_C   ,SPEAKER_FRONT_CENTER ,IDFF_channelSwapC,
 IDC_CBX_CHANNELSWAP_R   ,SPEAKER_FRONT_RIGHT  ,IDFF_channelSwapR,
 IDC_CBX_CHANNELSWAP_SL  ,SPEAKER_BACK_LEFT    ,IDFF_channelSwapSL,
 IDC_CBX_CHANNELSWAP_REAR,SPEAKER_BACK_CENTER  ,IDFF_channelSwapREAR,
 IDC_CBX_CHANNELSWAP_SR  ,SPEAKER_BACK_RIGHT   ,IDFF_channelSwapSR,
 IDC_CBX_CHANNELSWAP_LFE ,SPEAKER_LOW_FREQUENCY,IDFF_channelSwapLFE,
 IDC_CBX_CHANNELSWAP_AL  ,SPEAKER_SIDE_LEFT    ,IDFF_channelSwapAL,
 IDC_CBX_CHANNELSWAP_AR  ,SPEAKER_SIDE_RIGHT   ,IDFF_channelSwapAR
};

void TchannelSwapPage::cfg2dlg(void)
{
 cbxs.clear();
 for (int i=0;i<countof(speakers);i++)
  {
   cbxs[speakers[i].cbx]=i;
   int speaker=cfgGet(speakers[i].idff);
   for (int j=0;j<countof(speakers);j++)
    if (speakers[j].speaker==speaker)
     {
      cbxSetCurSel(speakers[i].cbx,j);
      break;
     }
  }
}

INT_PTR TchannelSwapPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CBX_CHANNELSWAP_L:
      case IDC_CBX_CHANNELSWAP_C:
      case IDC_CBX_CHANNELSWAP_R:
      case IDC_CBX_CHANNELSWAP_SL:
      case IDC_CBX_CHANNELSWAP_REAR:
      case IDC_CBX_CHANNELSWAP_SR:
      case IDC_CBX_CHANNELSWAP_LFE:
      case IDC_CBX_CHANNELSWAP_AL:
      case IDC_CBX_CHANNELSWAP_AR:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         int ii=cbxs[LOWORD(wParam)];
         const Tspeaker &spk=speakers[cbxGetCurItemData(LOWORD(wParam))];
         int swapidff=0;
         for (int i=0;i<countof(speakers);i++)
          if (i!=ii && cfgGet(speakers[i].idff)==spk.speaker)
           {
            swapidff=speakers[i].idff;
            break;
           }
         int idff=speakers[ii].idff;
         if (swapidff)
          cfgSet(swapidff,cfgGet(idff));
         cfgSet(idff,spk.speaker);
         cfg2dlg();
         return TRUE;
        }
       break;
     }
    break;
  }
 return TconfPageDecAudio::msgProc(uMsg,wParam,lParam);
}

void TchannelSwapPage::translate(void)
{
 TconfPageDecAudio::translate();

 for (int i=0;i<countof(speakers);i++)
  {
   int sel=cbxGetCurSel(speakers[i].cbx);
   cbxClear(speakers[i].cbx);
   for (int j=0;j<countof(speakers);j++)
    cbxAdd(speakers[i].cbx,_(speakers[i].cbx,TsampleFormat::getSpeakerName(speakers[j].speaker)),j);
   cbxSetCurSel(speakers[i].cbx,sel);
  }
}

TchannelSwapPage::TchannelSwapPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecAudio(Iparent,idff)
{
 resInter=IDC_CHB_CHANNELSWAP;
}
