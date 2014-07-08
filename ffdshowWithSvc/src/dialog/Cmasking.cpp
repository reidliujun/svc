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
#include "Cmasking.h"

bool TmaskingPage::enabled(void)
{
 return lavc_codec(codecId) && sup_masking(codecId);
}

void TmaskingPage::cfg2dlg(void)
{
 masking2dlg();
 elim2dlg();
}

void TmaskingPage::masking2dlg(void)
{
 bool is=sup_masking(codecId);
 setCheck(IDC_CHB_LUMI_MASKING,cfgGet(IDFF_enc_is_ff_lumi_masking));setText(IDC_ED_LUMI_MASKING,_l("%g"),float(cfgGet(IDFF_enc_ff_lumi_masking1000)/1000.0));enable(is && cfgGet(IDFF_enc_is_ff_lumi_masking),IDC_ED_LUMI_MASKING);
 setCheck(IDC_CHB_TEMPORAL_CPLX_MASKING,cfgGet(IDFF_enc_is_ff_temporal_cplx_masking));setText(IDC_ED_TEMPORAL_CPLX_MASKING,_l("%g"),float(cfgGet(IDFF_enc_ff_temporal_cplx_masking1000)/1000.0));enable(is && cfgGet(IDFF_enc_is_ff_temporal_cplx_masking),IDC_ED_TEMPORAL_CPLX_MASKING);
 setCheck(IDC_CHB_SPATIAL_CPLX_MASKING,cfgGet(IDFF_enc_is_ff_spatial_cplx_masking));setText(IDC_ED_SPATIAL_CPLX_MASKING,_l("%g"),float(cfgGet(IDFF_enc_ff_spatial_cplx_masking1000)/1000.0));enable(is && cfgGet(IDFF_enc_is_ff_spatial_cplx_masking),IDC_ED_SPATIAL_CPLX_MASKING);
 setCheck(IDC_CHB_P_MASKING,cfgGet(IDFF_enc_is_ff_p_masking));setText(IDC_ED_P_MASKING,_l("%g"),float(cfgGet(IDFF_enc_ff_p_masking1000)/1000.0));enable(is && cfgGet(IDFF_enc_is_ff_p_masking),IDC_ED_P_MASKING);
 setCheck(IDC_CHB_DARK_MASKING,cfgGet(IDFF_enc_is_ff_dark_masking));setText(IDC_ED_DARK_MASKING,_l("%g"),float(cfgGet(IDFF_enc_ff_dark_masking1000)/1000.0));enable(is && cfgGet(IDFF_enc_is_ff_dark_masking),IDC_ED_DARK_MASKING);
 setCheck(IDC_CHB_BORDER_MASKING,cfgGet(IDFF_enc_is_ff_border_masking));setText(IDC_ED_BORDER_MASKING,_l("%g"),float(cfgGet(IDFF_enc_ff_dark_masking1000)/1000.0));enable(is && cfgGet(IDFF_enc_is_ff_border_masking),IDC_ED_BORDER_MASKING);
 setCheck(IDC_CHB_NAQ,cfgGet(IDFF_enc_ff_naq));enable(deciE->isLAVCadaptiveQuant(),IDC_CHB_NAQ);
}
void TmaskingPage::elim2dlg(void)
{
 int is=sup_masking(codecId);
 enable(is,IDC_LBL_ELIM);

 setCheck(IDC_CHB_ELIM_LUM,cfgGet(IDFF_enc_isElimLum));enable(is,IDC_CHB_ELIM_LUM);
 SetDlgItemInt(m_hwnd,IDC_ED_ELIM_LUM,abs(cfgGet(IDFF_enc_elimLumThres)),FALSE);
 setCheck(IDC_CHB_ELIM_LUM_DC,cfgGet(IDFF_enc_elimLumThres)<0);
 static const int idElimLum[]={IDC_LBL_ELIM_LUM,IDC_ED_ELIM_LUM,IDC_CHB_ELIM_LUM_DC,0};
 enable(is && cfgGet(IDFF_enc_isElimLum),idElimLum);

 setCheck(IDC_CHB_ELIM_CHROM,cfgGet(IDFF_enc_isElimChrom));enable(is && !cfgGet(IDFF_enc_gray),IDC_CHB_ELIM_CHROM);
 SetDlgItemInt(m_hwnd,IDC_ED_ELIM_CHROM,abs(cfgGet(IDFF_enc_elimChromThres)),FALSE);
 setCheck(IDC_CHB_ELIM_CHROM_DC,cfgGet(IDFF_enc_elimChromThres)<0);
 static const int idElimChrom[]={IDC_LBL_ELIM_CHROM,IDC_ED_ELIM_CHROM,IDC_CHB_ELIM_CHROM_DC,0};
 enable(is && cfgGet(IDFF_enc_isElimChrom) && !cfgGet(IDFF_enc_gray),idElimChrom);
}

INT_PTR TmaskingPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_ELIM_LUM:
      case IDC_ED_ELIM_CHROM:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam));
         if (hed!=GetFocus()) return FALSE;
         repaint(hed);
         switch (LOWORD(wParam))
          {
           case IDC_ED_ELIM_LUM  :eval(hed,0,1000,IDFF_enc_elimLumThres  ,getCheck(IDC_CHB_ELIM_LUM_DC  )?-1:1);break;
           case IDC_ED_ELIM_CHROM:eval(hed,0,1000,IDFF_enc_elimChromThres,getCheck(IDC_CHB_ELIM_CHROM_DC)?-1:1);break;
          }
         return TRUE;
        }
       break;
      case IDC_CHB_ELIM_LUM_DC:
       cfgSet(IDFF_enc_elimLumThres,(getCheck(IDC_CHB_ELIM_LUM_DC)?-1:1)*abs(cfgGet(IDFF_enc_elimLumThres)));
       return TRUE;
      case IDC_CHB_ELIM_CHROM_DC:
       cfgSet(IDFF_enc_elimChromThres,(getCheck(IDC_CHB_ELIM_CHROM_DC)?-1:1)*abs(cfgGet(IDFF_enc_elimChromThres)));
       return TRUE;
     }
    break;
   case WM_CTLCOLOREDIT:
    {
     HWND hwnd=HWND(lParam);
     bool ok;
     switch (getId(hwnd))
      {
       case IDC_ED_ELIM_LUM:
       case IDC_ED_ELIM_CHROM:
        ok=eval(hwnd,0,1000);break;
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

TmaskingPage::TmaskingPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_MASKING;
 static const int props[]={IDFF_enc_isElimLum,IDFF_enc_elimLumThres,IDFF_enc_isElimChrom,IDFF_enc_elimChromThres,IDFF_enc_is_ff_lumi_masking,IDFF_enc_ff_lumi_masking1000,IDFF_enc_is_ff_temporal_cplx_masking,IDFF_enc_ff_temporal_cplx_masking1000,IDFF_enc_is_ff_spatial_cplx_masking,IDFF_enc_ff_spatial_cplx_masking1000,IDFF_enc_is_ff_p_masking,IDFF_enc_ff_p_masking1000,IDFF_enc_is_ff_dark_masking,IDFF_enc_ff_dark_masking1000,IDFF_enc_is_ff_border_masking,IDFF_enc_ff_border_masking1000,IDFF_enc_ff_naq,0};
 propsIDs=props;
 helpURL=_l("Masking.html");
 static const TbindCheckbox<TmaskingPage> chb[]=
  {
   IDC_CHB_ELIM_LUM,IDFF_enc_isElimLum,&TmaskingPage::elim2dlg,
   IDC_CHB_ELIM_CHROM,IDFF_enc_isElimChrom,&TmaskingPage::elim2dlg,
   IDC_CHB_LUMI_MASKING,IDFF_enc_is_ff_lumi_masking,&TmaskingPage::masking2dlg,
   IDC_CHB_TEMPORAL_CPLX_MASKING,IDFF_enc_is_ff_temporal_cplx_masking,&TmaskingPage::masking2dlg,
   IDC_CHB_SPATIAL_CPLX_MASKING,IDFF_enc_is_ff_spatial_cplx_masking,&TmaskingPage::masking2dlg,
   IDC_CHB_P_MASKING,IDFF_enc_is_ff_p_masking,&TmaskingPage::masking2dlg,
   IDC_CHB_DARK_MASKING,IDFF_enc_is_ff_dark_masking,&TmaskingPage::masking2dlg,
   IDC_CHB_BORDER_MASKING,IDFF_enc_is_ff_border_masking,&TmaskingPage::masking2dlg,
   IDC_CHB_NAQ,IDFF_enc_ff_naq,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditReal<TmaskingPage> edReal[]=
  {
   IDC_ED_LUMI_MASKING         ,0.0,1.0,IDFF_enc_ff_lumi_masking1000,1000.0,NULL,
   IDC_ED_TEMPORAL_CPLX_MASKING,0.0,1.0,IDFF_enc_ff_temporal_cplx_masking1000,1000.0,NULL,
   IDC_ED_SPATIAL_CPLX_MASKING ,0.0,1.0,IDFF_enc_ff_spatial_cplx_masking1000,1000.0,NULL,
   IDC_ED_P_MASKING            ,0.0,1.0,IDFF_enc_ff_p_masking1000,1000.0,NULL,
   IDC_ED_DARK_MASKING         ,0.0,1.0,IDFF_enc_ff_dark_masking1000,1000.0,NULL,
   IDC_ED_BORDER_MASKING       ,0.0,1.0,IDFF_enc_ff_border_masking1000,1000.0,NULL,
   0
  };
 bindEditReals(edReal);
}
