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
#include "ffcodecs.h"
#include "CgenericWmv9.h"
#include "TvideoCodecWmv9.h"

bool TgenericWmv9page::enabled(void)
{
 return wmv9_codec(codecId);
}

void TgenericWmv9page::cfg2dlg(void)
{
 if (!wmv9_codec(codecId)) return;
 int aviout=cfgGet(IDFF_enc_wmv9_aviout);
 setCheck(IDC_CHB_WMV9_ASFOUT,!aviout);
 SetDlgItemInt(m_hwnd,IDC_ED_WMV9_KFSECS,cfgGet(IDFF_enc_wmv9_kfsecs),FALSE);
 static const int idCplx[]={IDC_LBL_WMV9_CPLX,IDC_ED_WMV9_CPLX,0};
 SetDlgItemInt(m_hwnd,IDC_ED_WMV9_CPLX,cfgGet(IDFF_enc_wmv9_cplx),FALSE);
 enc=NULL;
 deciE->getEncoder(codecId,(const Tencoder**)&enc);
 enable(enc && enc->cplxMax,idCplx);
 SetDlgItemInt(m_hwnd,IDC_ED_WMV9_CRISP,cfgGet(IDFF_enc_wmv9_crisp),FALSE);enable(aviout,IDC_ED_WMV9_CRISP);enable(aviout,IDC_LBL_WMV9_CRISP);
 setCheck(IDC_CHB_WMV9_DEINT,cfgGet(IDFF_enc_wmv9_deint));
 setCheck(IDC_CHB_WMV9_IVTC,cfgGet(IDFF_enc_wmv9_ivtc));enable(!aviout,IDC_CHB_WMV9_IVTC);
}

INT_PTR TgenericWmv9page::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_WMV9_CPLX:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam));
         if (hed!=GetFocus()) return FALSE;
         repaint(hed);
         switch (LOWORD(wParam))
          {
           case IDC_ED_WMV9_CPLX:eval(hed,0,enc?enc->cplxMax:0,IDFF_enc_wmv9_cplx);break;
          }
         return TRUE;
        }
       break;
     }
    break;
   case WM_CTLCOLOREDIT:
    {
     HWND hwnd=HWND(lParam);
     bool ok;
     switch (getId(hwnd))
      {
       case IDC_ED_WMV9_CPLX:ok=eval(hwnd,0,enc?enc->cplxMax:0);break;
       default:goto colorEnd;
      }
     if (!ok)
      {
       HDC dc=HDC(wParam);
       SetBkColor(dc,RGB(255,0,0));
       return INT_PTR(getRed());
      }
     else return FALSE;
     colorEnd:;
    }
  }
 return TconfPageEnc::msgProc(uMsg,wParam,lParam);
}

TgenericWmv9page::TgenericWmv9page(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_GENERIC_WMV9;
 static const int props[]={IDFF_enc_wmv9_kfsecs,IDFF_enc_wmv9_cplx,IDFF_enc_wmv9_deint,IDFF_enc_wmv9_ivtc,IDFF_enc_wmv9_crisp,0};
 propsIDs=props;
 static const TbindCheckbox<TgenericWmv9page> chb[]=
  {
   IDC_CHB_WMV9_ASFOUT,-IDFF_enc_wmv9_aviout,&TgenericWmv9page::cfg2dlg,
   IDC_CHB_WMV9_DEINT,IDFF_enc_wmv9_deint,NULL,
   IDC_CHB_WMV9_IVTC,IDFF_enc_wmv9_ivtc,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditInt<TgenericWmv9page> edInt[]=
  {
   IDC_ED_WMV9_KFSECS,1,10,IDFF_enc_wmv9_kfsecs,NULL,
   IDC_ED_WMV9_CRISP,0,100,IDFF_enc_wmv9_crisp,NULL,
   0
  };
 bindEditInts(edInt);
}
