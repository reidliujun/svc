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
#include "Cout.h"
#include "TbitrateDlg.h"
#include "Tdll.h"
#include "Tmuxer.h"
#include "TcodecSettings.h"

void ToutPage::init(void)
{
 SendDlgItemMessage(m_hwnd,IDC_BT_ASPECT,WM_SETFONT,WPARAM(parent->arrowsFont),LPARAM(false));
 SendDlgItemMessage(m_hwnd,IDC_BT_FPS,WM_SETFONT,WPARAM(parent->arrowsFont),LPARAM(false));
}

void ToutPage::cfg2dlg(void)
{
 out2dlg();
 stats2dlg();
 fps2dlg();
 aspect2dlg();
}

void ToutPage::out2dlg(void)
{
 setCheck(IDC_CHB_STORE_AVI,cfgGet(IDFF_enc_storeAVI));
 setCheck(IDC_CHB_STORE_EXTERNAL,cfgGet(IDFF_enc_storeExt));
 static const int idStoreExt[]={IDC_CBX_MUXER,IDC_BT_STORE_EXTERNAL,IDC_ED_STORE_EXTERNAL,0};
 setDlgItemText(m_hwnd,IDC_ED_STORE_EXTERNAL,cfgGetStr(IDFF_enc_storeExtFlnm));
 cbxSetCurSel(IDC_CBX_MUXER,cfgGet(IDFF_enc_muxer));
 enable(cfgGet(IDFF_enc_storeExt),idStoreExt);
}
void ToutPage::stats2dlg(void)
{
 bool is=sup_LAVC2PASS(codecId);
 int enc_mode=cfgGet(IDFF_enc_mode);
 int ff1_stats_mode=cfgGet(IDFF_enc_ff1_stats_mode);
 if (is && ff1_stats_mode&FFSTATS::READ && enc_mode!=ENC_MODE::CBR) cfgSet(IDFF_enc_ff1_stats_mode,FFSTATS::UNUSED);
 if ((ff1_stats_mode&FFSTATS::RW)==FFSTATS::RW)
  setCheck(IDC_RBT_LAVC_STATS_RW,1);
 else
  {
   setCheck(IDC_RBT_LAVC_STATS_NONE,ff1_stats_mode==0);
   setCheck(IDC_RBT_LAVC_STATS_WRITE,ff1_stats_mode&FFSTATS::WRITE);
   setCheck(IDC_RBT_LAVC_STATS_READ,ff1_stats_mode&FFSTATS::READ && is);
   setCheck(IDC_RBT_LAVC_STATS_RW,0);
  }
 static const int idStats[]={IDC_LBL_LAVC_STATS,IDC_RBT_LAVC_STATS_NONE,IDC_RBT_LAVC_STATS_WRITE,IDC_RBT_LAVC_STATS_READ,IDC_RBT_LAVC_STATS_RW,IDC_ED_LAVC_STATS,IDC_BT_LAVC_STATS,0};
 enable(is,idStats);
 enable(is && ff1_stats_mode!=0,idStats+5);
 enable(enc_mode==ENC_MODE::CBR && is,IDC_RBT_LAVC_STATS_READ);enable(enc_mode==ENC_MODE::CBR && is,IDC_RBT_LAVC_STATS_RW);
 setDlgItemText(m_hwnd,IDC_ED_LAVC_STATS,cfgGetStr(IDFF_enc_ff1_stats_flnm));
 enable(is && ff1_stats_mode&FFSTATS::READ,IDC_BT_BITRATE);
}
void ToutPage::fps2dlg(void)
{
 setCheck(IDC_CHB_FPS,cfgGet(IDFF_enc_isFPSoverride));
 SetDlgItemInt(m_hwnd,IDC_ED_FPS_NUM,cfgGet(IDFF_enc_fpsOverrideNum),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_FPS_DEN,cfgGet(IDFF_enc_fpsOverrideDen),FALSE);
 int is=(filterMode&IDFF_FILTERMODE_VFW)==0;
 enable(is,IDC_CHB_FPS);
 static const int idFps[]={IDC_ED_FPS_NUM,IDC_ED_FPS_DEN,IDC_BT_FPS,0};
 enable(is && cfgGet(IDFF_enc_isFPSoverride),idFps);
}
void ToutPage::aspect2dlg(void)
{
 int is=sup_aspect(codecId);
 static const int idAspects[]={IDC_CBX_ASPECT,IDC_ED_ASPECTX,IDC_LBL_ASPECT,IDC_ED_ASPECTY,IDC_BT_ASPECT,0};
 enable(is,idAspects);
 int aspectmode=cfgGet(IDFF_enc_aspectMode);
 cbxSetCurSel(IDC_CBX_ASPECT,aspectmode);
 setText(IDC_ED_ASPECTX,_l("%g"),cfgGet(aspectmode==TcoSettings::ASPECT_SAR?IDFF_enc_sarX1000:IDFF_enc_darX1000)/1000.0f);
 setText(IDC_ED_ASPECTY,_l("%g"),cfgGet(aspectmode==TcoSettings::ASPECT_SAR?IDFF_enc_sarY1000:IDFF_enc_darY1000)/1000.0f);
}

INT_PTR ToutPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_STORE_EXTERNAL:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t storeExtFlnm[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_STORE_EXTERNAL,storeExtFlnm,MAX_PATH);
         cfgSet(IDFF_enc_storeExtFlnm,storeExtFlnm);
         return TRUE;
        }
       break;
      case IDC_ED_LAVC_STATS:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t stats_flnm[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_LAVC_STATS,stats_flnm,MAX_PATH);
         cfgSet(IDFF_enc_ff1_stats_flnm,stats_flnm);
         return TRUE;
        }
       break;
      case IDC_ED_ASPECTX:
      case IDC_ED_ASPECTY:
      case IDC_ED_FPS_NUM:
      case IDC_ED_FPS_DEN:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam));
         if (hed!=GetFocus()) return FALSE;
         repaint(hed);
         int aspectmode=cbxGetCurSel(IDC_CBX_ASPECT);
         switch (LOWORD(wParam))
          {
           case IDC_ED_ASPECTX:eval(hed,0.1,255.0,aspectmode==TcoSettings::ASPECT_SAR?IDFF_enc_sarX1000:IDFF_enc_darX1000,1000.0);break;
           case IDC_ED_ASPECTY:eval(hed,0.1,255.0,aspectmode==TcoSettings::ASPECT_SAR?IDFF_enc_sarY1000:IDFF_enc_darY1000,1000.0);break;
           case IDC_ED_FPS_NUM:eval(hed,1,200*1000,IDFF_enc_fpsOverrideNum);break;
           case IDC_ED_FPS_DEN:eval(hed,1,200*1000,IDFF_enc_fpsOverrideDen);break;
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
       case IDC_ED_ASPECTX:
       case IDC_ED_ASPECTY:
        ok=eval(hwnd,0.1,255.0);
        break;
       case IDC_ED_FPS_NUM:
       case IDC_ED_FPS_DEN:
        ok=eval(hwnd,1,200*1000);
        break;
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

void ToutPage::onStoreExternal(void)
{
 char_t storeExtFlnm[MAX_PATH];cfgGet(IDFF_enc_storeExtFlnm,storeExtFlnm,MAX_PATH);
 if (dlgGetFile(true,m_hwnd,_(-IDD_OUT,_l("Select file for storing frames")),_l("All files (*.*)\0*.*\0"),_l(""),storeExtFlnm,_l("."),0))
  {
   cfgSet(IDFF_enc_storeExtFlnm,storeExtFlnm);
   out2dlg();
  }
}
void ToutPage::onLavcStats(void)
{
 char_t stats_flnm[MAX_PATH];cfgGet(IDFF_enc_ff1_stats_flnm,stats_flnm,MAX_PATH);
 if (dlgGetFile(cfgGet(IDFF_enc_ff1_stats_mode)==1,m_hwnd,_(-IDD_OUT,cfgGet(IDFF_enc_ff1_stats_mode)==1?_l("Select file for storing encoding statistics"):_l("Select file with stored encoding statistics")),_l("Libavcodec stats file (*.ffstats)\0*.ffstats\0All files (*.*)\0*.*\0"),_l("ffstats"),stats_flnm,_l("."),0))
  {
   cfgSet(IDFF_enc_ff1_stats_flnm,stats_flnm);
   cfg2dlg();
  }
}
void ToutPage::onBitrate(void)
{
 if (fileexists(cfgGetStr(IDFF_enc_ff1_stats_flnm)))
  {
   TbitrateDlg dlg(m_hwnd,deci);
   parentE->quick2dlg(true);
  }
}
void ToutPage::onAspect(void)
{
 RECT r;
 GetWindowRect(GetDlgItem(m_hwnd,IDC_BT_ASPECT),&r);
 int aspectmode=cbxGetCurSel(IDC_CBX_ASPECT);
 HMENU hmn=CreatePopupMenu();
 const Taspect *aspects=aspectmode==0?sampleAspects:displayAspects;
 for (int ord=0;aspects[ord].caption;)
  insertMenuItem(hmn,ord,ord+100,aspects[ord].caption,false);
 int cmd=TrackPopupMenu(_(hmn),TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,r.left-1,r.bottom,0,m_hwnd,0);
 if (cmd>=100)
  {
   const Taspect &a=aspects[cmd-100];
   cfgSet(aspectmode==TcoSettings::ASPECT_SAR?IDFF_enc_sarX1000:IDFF_enc_darX1000,int(a.x*1000));
   cfgSet(aspectmode==TcoSettings::ASPECT_SAR?IDFF_enc_sarY1000:IDFF_enc_darY1000,int(a.y*1000));
   aspect2dlg();
  }
 DestroyMenu(hmn);
}
void ToutPage::onFps(void)
{
 strings mitems;
 for (int i=0;TbitrateDlg::mpeg12_frame_rate_tab[i][0];i++)
  {
   char_t pomS[30];
   tsnprintf_s(pomS, countof(pomS), _TRUNCATE, _l("%g"), float(TbitrateDlg::mpeg12_frame_rate_tab[i][0])/TbitrateDlg::mpeg12_frame_rate_tab[i][1]);
   mitems.push_back(pomS);
  }
 int idx=selectFromMenu(mitems,IDC_BT_FPS,false,20);
 if (idx>=0)
  {
   cfgSet(IDFF_enc_fpsOverrideNum,TbitrateDlg::mpeg12_frame_rate_tab[idx][0]);
   cfgSet(IDFF_enc_fpsOverrideDen,TbitrateDlg::mpeg12_frame_rate_tab[idx][1]);
   fps2dlg();
  }
}
void ToutPage::translate(void)
{
 TconfPageEnc::translate();

 cbxTranslate(IDC_CBX_MUXER,Tmuxer::muxers);
 cbxTranslate(IDC_CBX_ASPECT,TcoSettings::aspectModes);
}

ToutPage::ToutPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent),parentE(Iparent)
{
 dialogId=IDD_OUT;
 static const int props[]={IDFF_enc_storeAVI,IDFF_enc_storeExt,IDFF_enc_storeExtFlnm,IDFF_enc_muxer,IDFF_enc_ff1_stats_mode,IDFF_enc_ff1_stats_flnm,IDFF_enc_aspectMode,IDFF_enc_isFPSoverride,IDFF_enc_fpsOverrideNum,IDFF_enc_fpsOverrideDen,IDFF_enc_sarX1000,IDFF_enc_sarY1000,IDFF_enc_darX1000,IDFF_enc_darY1000,0};
 propsIDs=props;
 static const TbindCheckbox<ToutPage> chb[]=
  {
   IDC_CHB_STORE_EXTERNAL,IDFF_enc_storeExt,&ToutPage::out2dlg,
   IDC_CHB_STORE_AVI,IDFF_enc_storeAVI,NULL,
   IDC_CHB_FPS,IDFF_enc_isFPSoverride,&ToutPage::fps2dlg,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindRadiobutton<ToutPage> rbt[]=
  {
   IDC_RBT_LAVC_STATS_NONE,IDFF_enc_ff1_stats_mode,FFSTATS::UNUSED,&ToutPage::stats2dlg,
   IDC_RBT_LAVC_STATS_WRITE,IDFF_enc_ff1_stats_mode,FFSTATS::WRITE,&ToutPage::stats2dlg,
   IDC_RBT_LAVC_STATS_READ,IDFF_enc_ff1_stats_mode,FFSTATS::READ,&ToutPage::stats2dlg,
   IDC_RBT_LAVC_STATS_RW,IDFF_enc_ff1_stats_mode,FFSTATS::RW,&ToutPage::stats2dlg,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindCombobox<ToutPage> cbx[]=
  {
   IDC_CBX_MUXER,IDFF_enc_muxer,BINDCBX_SEL,NULL,
   IDC_CBX_ASPECT,IDFF_enc_aspectMode,BINDCBX_SEL,&ToutPage::aspect2dlg,
   0
  };
 bindComboboxes(cbx);
 static const TbindButton<ToutPage> bt[]=
  {
   IDC_BT_STORE_EXTERNAL,&ToutPage::onStoreExternal,
   IDC_BT_LAVC_STATS,&ToutPage::onLavcStats,
   IDC_BT_BITRATE,&ToutPage::onBitrate,
   IDC_BT_ASPECT,&ToutPage::onAspect,
   IDC_BT_FPS,&ToutPage::onFps,
   0,NULL
  };
 bindButtons(bt);
}
