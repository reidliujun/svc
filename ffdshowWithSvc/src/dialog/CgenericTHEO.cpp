/*
 * Copyright (c) 2002-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "CgenericTHEO.h"

void TgenericTHEOpage::init(void)
{
 tbrSetRange(IDC_TBR_MAXKEYINTERVAL,1,500,10);
 tbrSetRange(IDC_TBR_MINKEYINTERVAL,1,500,10);
 tbrSetRange(IDC_TBR_KEYSCENECHANGE,0,100);
}

void TgenericTHEOpage::cfg2dlg(void)
{
 kf2dlg();
 scenechange2dlg();
 setCheck(IDC_CHB_THEO_HQ,cfgGet(IDFF_enc_theo_hq));
 setCheck(IDC_CHB_DROP_FRAMES,cfgGet(IDFF_enc_dropFrames));
}
void TgenericTHEOpage::kf2dlg(void)
{
 int x;
 x=cfgGet(IDFF_enc_max_key_interval);
 setText(IDC_LBL_MAXKEYINTERVAL,_l("%s %i"),_(IDC_LBL_MAXKEYINTERVAL),x);
 tbrSet(IDC_TBR_MAXKEYINTERVAL,x);
 x=cfgGet(IDFF_enc_min_key_interval);
 setText(IDC_LBL_MINKEYINTERVAL,_l("%s %i"),_(IDC_LBL_MINKEYINTERVAL),x);
 tbrSet(IDC_TBR_MINKEYINTERVAL,x);
 static const int idMaxKey[]={IDC_LBL_MAXKEYINTERVAL,IDC_TBR_MAXKEYINTERVAL,0};
 enable(sup_maxKeySet(codecId),idMaxKey);
 static const int idMinKey[]={IDC_LBL_MINKEYINTERVAL,IDC_TBR_MINKEYINTERVAL,0};
 enable(sup_minKeySet(codecId),idMinKey);
}
void TgenericTHEOpage::scenechange2dlg(void)
{
 int x=cfgGet(IDFF_enc_keySceneChange);
 setText(IDC_LBL_KEYSCENECHANGE,_l("%s %i%%"),_(IDC_LBL_KEYSCENECHANGE),x);
 tbrSet(IDC_TBR_KEYSCENECHANGE,x);
}

INT_PTR TgenericTHEOpage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_MAXKEYINTERVAL:
       cfgSet(IDFF_enc_max_key_interval,std::max(cfgGet(IDFF_enc_min_key_interval),tbrGet(IDC_TBR_MAXKEYINTERVAL)));
       kf2dlg();
       return TRUE;
      case IDC_TBR_MINKEYINTERVAL:
       cfgSet(IDFF_enc_min_key_interval,std::min(cfgGet(IDFF_enc_max_key_interval),tbrGet(IDC_TBR_MINKEYINTERVAL)));
       kf2dlg();
       return TRUE;
     }
    break;
  }
 return TconfPageEnc::msgProc(uMsg,wParam,lParam);
}

TgenericTHEOpage::TgenericTHEOpage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_GENERIC_THEO;
 static const int props[]={IDFF_enc_max_key_interval,IDFF_enc_min_key_interval,IDFF_enc_theo_hq,IDFF_enc_dropFrames,IDFF_enc_keySceneChange,0};
 propsIDs=props;
 static const TbindCheckbox<TgenericTHEOpage> chb[]=
  {
   IDC_CHB_THEO_HQ,IDFF_enc_theo_hq,NULL,
   IDC_CHB_DROP_FRAMES,IDFF_enc_dropFrames,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TgenericTHEOpage> htbr[]=
  {
   IDC_TBR_KEYSCENECHANGE,IDFF_enc_keySceneChange,&TgenericTHEOpage::scenechange2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
}
