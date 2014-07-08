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
#include "CaudioSwitcher.h"

void TaudioSwitcherPage::init(void)
{
 unsigned int num=deciA->getNumStreams2();
 static const int idStream[]={IDC_LBL_AUDIOSWITCHER,IDC_CBX_AUDIOSWITCHER,0};
 if (num>1)
  {
   enable(1,idStream);
   for (unsigned int i=0;i<num;i++)
    {
     char_t descr[250];
     if (deciA->getStreamDescr(i,descr,250)==S_OK)
      {
       char_t stream[256];
       tsnprintf_s(stream, countof(stream), _TRUNCATE, _l("%u. %s"),i,descr);
       cbxAdd(IDC_CBX_AUDIOSWITCHER,stream);
      }
    }
  }
 else
  enable(0,idStream);
}

void TaudioSwitcherPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_AUDIOSWITCHER,cfgGet(IDFF_isAudioSwitcher));
 cbxSetCurSel(IDC_CBX_AUDIOSWITCHER,deciA->getCurrentStream2());
}

INT_PTR TaudioSwitcherPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CBX_AUDIOSWITCHER:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         deciA->setCurrentStream(cbxGetCurSel(IDC_CBX_AUDIOSWITCHER));
         return TRUE;
        }
       break;
     }
    break;
  }
 return TconfPageDecAudio::msgProc(uMsg,wParam,lParam);
}

TaudioSwitcherPage::TaudioSwitcherPage(TffdshowPageDec *Iparent):TconfPageDecAudio(Iparent)
{
 dialogId=IDD_AUDIOSWITCHER;
 static const TbindCheckbox<TaudioSwitcherPage> chb[]=
  {
   IDC_CHB_AUDIOSWITCHER,IDFF_isAudioSwitcher,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
}

