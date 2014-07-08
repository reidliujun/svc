/*
 * Copyright (c) 2002-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Ccredits.h"

void TcreditsPage::init(void)
{
 tbrSetRange(IDC_TBR_CREDITS_PERCENT,1,100,10);
}

bool TcreditsPage::enabled(void)
{
 int enc_mode=cfgGet(IDFF_enc_mode);
 return (sup_perFrameQuant(codecId) && ((enc_mode!=ENC_MODE::CBR && enc_mode!=ENC_MODE::PASS2_2_EXT && enc_mode!=ENC_MODE::VBR_QUAL) || (enc_mode==ENC_MODE::CBR && lavc_codec(codecId) && cfgGet(IDFF_enc_ff1_stats_mode)&FFSTATS::READ))) || codecId==CODEC_ID_X264;
}

void TcreditsPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_CREDITS_GRAY,cfgGet(IDFF_enc_graycredits));

 start2dlg();
 end2dlg();

 percent2dlg();
 quant2dlg();
 size2dlg();
}
void TcreditsPage::startend2dlg(void)
{
 percent2dlg();
 quant2dlg();
 size2dlg();

 enable(cfgGet(IDFF_enc_isCreditsStart) || cfgGet(IDFF_enc_isCreditsEnd),IDC_GRP_CREDITS_REDUCTION);
 enable(sup_gray(codecId) && !lavc_codec(codecId) && (cfgGet(IDFF_enc_isCreditsStart) || cfgGet(IDFF_enc_isCreditsEnd)) && !cfgGet(IDFF_enc_gray),IDC_CHB_CREDITS_GRAY);
}
void TcreditsPage::start2dlg(void)
{
 static const int idCreditsStart[]={IDC_LBL_CREDITS_START_START,IDC_LBL_CREDITS_START_END,IDC_ED_CREDITS_START_START,IDC_ED_CREDITS_START_END,0};
 setCheck(IDC_CHB_IS_CREDITS_START,cfgGet(IDFF_enc_isCreditsStart));enable(cfgGet(IDFF_enc_isCreditsStart),idCreditsStart);
 SetDlgItemInt(m_hwnd,IDC_ED_CREDITS_START_START,cfgGet(IDFF_enc_creditsStartBegin),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_CREDITS_START_END  ,cfgGet(IDFF_enc_creditsStartEnd  ),FALSE);
 startend2dlg();
}
void TcreditsPage::end2dlg(void)
{
 setCheck(IDC_CHB_IS_CREDITS_END,cfgGet(IDFF_enc_isCreditsEnd));
 SetDlgItemInt(m_hwnd,IDC_ED_CREDITS_END_START,cfgGet(IDFF_enc_creditsEndBegin),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_CREDITS_END_END  ,cfgGet(IDFF_enc_creditsEndEnd  ),FALSE);
 static const int idCreditsEnd[]={IDC_LBL_CREDITS_END_START,IDC_LBL_CREDITS_END_END,IDC_ED_CREDITS_END_START,IDC_ED_CREDITS_END_END,0};
 enable(cfgGet(IDFF_enc_isCreditsEnd),idCreditsEnd);
 startend2dlg();
}
void TcreditsPage::percent2dlg(void)
{
 setCheck(IDC_RBT_CREDITS_PERCENT,cfgGet(IDFF_enc_credits_mode)==CREDITS_MODE::PERCENT);
 int x=cfgGet(IDFF_enc_credits_percent);
 tbrSet(IDC_TBR_CREDITS_PERCENT,x);
 setText(IDC_RBT_CREDITS_PERCENT,_l("%%: %i"),x);
 int mode=cfgGet(IDFF_enc_mode);
 bool is=(cfgGet(IDFF_enc_isCreditsStart) || cfgGet(IDFF_enc_isCreditsEnd)) && (mode==ENC_MODE::VBR_QUAL || mode==ENC_MODE::VBR_QUANT || mode==ENC_MODE::PASS2_2_INT || (mode==ENC_MODE::CBR && lavc_codec(codecId) && cfgGet(IDFF_enc_ff1_stats_mode)&FFSTATS::READ) || codecId==CODEC_ID_X264);
 enable(is,IDC_RBT_CREDITS_PERCENT);
 enable(is && cfgGet(IDFF_enc_credits_mode)==CREDITS_MODE::PERCENT,IDC_TBR_CREDITS_PERCENT);
}
void TcreditsPage::quant2dlg(void)
{
 int mode=cfgGet(IDFF_enc_mode);
 bool is=(cfgGet(IDFF_enc_isCreditsStart) || cfgGet(IDFF_enc_isCreditsEnd)) && (mode==ENC_MODE::VBR_QUAL || mode==ENC_MODE::VBR_QUANT || mode==ENC_MODE::PASS2_1 || mode==ENC_MODE::PASS2_2_INT || (mode==ENC_MODE::CBR && lavc_codec(codecId) && cfgGet(IDFF_enc_ff1_stats_mode)&FFSTATS::READ) || codecId==CODEC_ID_X264);
 setCheck(IDC_RBT_CREDITS_QUANT,cfgGet(IDFF_enc_credits_mode)==CREDITS_MODE::QUANT);
 enable(is,IDC_RBT_CREDITS_QUANT);
 static const int idCreditsQuant[]={IDC_LBL_CREDITS_I_QUANT,IDC_ED_CREDITS_I_QUANT,IDC_LBL_CREDITS_P_QUANT,IDC_ED_CREDITS_P_QUANT,0};
 enable(is && cfgGet(IDFF_enc_credits_mode)==CREDITS_MODE::QUANT,idCreditsQuant);

 SetDlgItemInt(m_hwnd,IDC_ED_CREDITS_I_QUANT,cfgGet(IDFF_enc_credits_quant_i),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_CREDITS_P_QUANT,cfgGet(IDFF_enc_credits_quant_p),FALSE);
}
void TcreditsPage::size2dlg(void)
{
 bool is=(cfgGet(IDFF_enc_isCreditsStart) || cfgGet(IDFF_enc_isCreditsEnd)) && (cfgGet(IDFF_enc_mode)==ENC_MODE::PASS2_2_INT);
 setCheck(IDC_RBT_CREDITS_SIZE,cfgGet(IDFF_enc_credits_mode)==CREDITS_MODE::SIZE);
 enable(is,IDC_RBT_CREDITS_SIZE);
 static const int idCreditsSize[]={IDC_LBL_CREDITS_SIZE_START,IDC_LBL_CREDITS_SIZE_END,IDC_ED_CREDITS_SIZE_START,IDC_ED_CREDITS_SIZE_END,0};
 enable(is && cfgGet(IDFF_enc_credits_mode)==CREDITS_MODE::SIZE,idCreditsSize);

 SetDlgItemInt(m_hwnd,IDC_ED_CREDITS_SIZE_START,cfgGet(IDFF_enc_credits_size_start),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_CREDITS_SIZE_END  ,cfgGet(IDFF_enc_credits_size_end  ),FALSE);
}
INT_PTR TcreditsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_CREDITS_I_QUANT:
      case IDC_ED_CREDITS_P_QUANT:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam));
         if (hed!=GetFocus()) return FALSE;
         repaint(hed);
         switch (LOWORD(wParam))
          {
           case IDC_ED_CREDITS_I_QUANT:eval(hed,parent->qmin,parent->qmax,IDFF_enc_credits_quant_i);break;
           case IDC_ED_CREDITS_P_QUANT:eval(hed,parent->qmin,parent->qmax,IDFF_enc_credits_quant_p);break;
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
       case IDC_ED_CREDITS_I_QUANT:
       case IDC_ED_CREDITS_P_QUANT:
        ok=eval(hwnd,parent->qmin,parent->qmax);break;
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
 return TconfPageEnc::msgProc(uMsg,wParam,lParam);
}

TcreditsPage::TcreditsPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_CREDITS;
 static const int props[]={IDFF_enc_isCreditsStart,IDFF_enc_isCreditsEnd,IDFF_enc_creditsStartBegin,IDFF_enc_creditsStartEnd,IDFF_enc_creditsEndBegin,IDFF_enc_creditsEndEnd,IDFF_enc_credits_mode,IDFF_enc_credits_percent,IDFF_enc_credits_quant_i,IDFF_enc_credits_quant_p,IDFF_enc_credits_size_start,IDFF_enc_credits_size_end,IDFF_enc_graycredits,0};
 propsIDs=props;
 static const TbindCheckbox<TcreditsPage> chb[]=
  {
   IDC_CHB_IS_CREDITS_START,IDFF_enc_isCreditsStart,&TcreditsPage::start2dlg,
   IDC_CHB_IS_CREDITS_END,IDFF_enc_isCreditsEnd,&TcreditsPage::end2dlg,
   IDC_CHB_CREDITS_GRAY,IDFF_enc_graycredits,NULL,
   0,0,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TcreditsPage> htbr[]=
  {
   IDC_TBR_CREDITS_PERCENT,IDFF_enc_credits_percent,&TcreditsPage::percent2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindRadiobutton<TcreditsPage> rbt[]=
  {
   IDC_RBT_CREDITS_PERCENT,IDFF_enc_credits_mode,CREDITS_MODE::PERCENT,&TcreditsPage::cfg2dlg,
   IDC_RBT_CREDITS_QUANT,IDFF_enc_credits_mode,CREDITS_MODE::QUANT,&TcreditsPage::cfg2dlg,
   IDC_RBT_CREDITS_SIZE,IDFF_enc_credits_mode,CREDITS_MODE::SIZE,&TcreditsPage::cfg2dlg,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindEditInt<TcreditsPage> edInt[]=
  {
   IDC_ED_CREDITS_START_START,0,10000000,IDFF_enc_creditsStartBegin,NULL,
   IDC_ED_CREDITS_START_END,0,10000000,IDFF_enc_creditsStartEnd,NULL,
   IDC_ED_CREDITS_END_START,0,10000000,IDFF_enc_creditsEndBegin,NULL,
   IDC_ED_CREDITS_END_END,0,10000000,IDFF_enc_creditsEndEnd,NULL,
   IDC_ED_CREDITS_SIZE_START,1,1000000,IDFF_enc_credits_size_start,NULL,
   IDC_ED_CREDITS_SIZE_END,1,1000000,IDFF_enc_credits_size_end,NULL,
   0
  };
 bindEditInts(edInt);
}
