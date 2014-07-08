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
#include "TbitrateDlg.h"
#include "ffdshow_constants.h"
#include "IffdshowBase.h"
#include "Ttranslate.h"

const int TbitrateDlg::mpeg12_frame_rate_tab[][2]=
{
 {24000, 1001},
 {   24,    1},
 {   25,    1},
 {30000, 1001},
 {   30,    1},
 {   50,    1},
 {60000, 1001},
 {   60,    1},
// Xing's 15fps: (9)
 {   15,    1},
// libmpeg3's "Unofficial economy rates": (10-13)
 {    5,    1},
 {   10,    1},
 {   12,    1},
 {   15,    1},
 {    0,    0},
};


void TbitrateDlg::init(void)
{
 FILE *statsfile=fopen(cfgGetStr(IDFF_enc_ff1_stats_flnm),_l("rb"));
 if (!statsfile)
  {
   numframes=0;
   return;
  }
 int statslen=_filelength(fileno(statsfile));
 char* stats_in=(char*)malloc(statslen+1);
 stats_in[statslen]='\0';
 fread(stats_in,1,statslen,statsfile);
 fclose(statsfile);statsfile=NULL;
 char *p=stats_in;
 for(numframes=-1;p;numframes++)
  p=strchr(p+1,';');
 free(stats_in);
 setText(IDC_LBL_BR_INFO,_l("%s %i"),_(IDC_LBL_BR_INFO),numframes);
 for (int i=0;mpeg12_frame_rate_tab[i][0];i++)
  {
   char_t pomS[30];
   tsnprintf_s(pomS, countof(pomS), _TRUNCATE, _l("%g"), float(mpeg12_frame_rate_tab[i][0])/mpeg12_frame_rate_tab[i][1]);
   cbxAdd(IDC_CBX_BR_FPS,pomS,i);
  }
 setText(IDC_CBX_BR_FPS,_l("%g"),float(cfgGet(IDFF_dlgBpsFps1000)/1000.0));
 SetDlgItemInt(m_hwnd,IDC_ED_BR_LEN,cfgGet(IDFF_dlgBpsLen),FALSE);
 time2dlg();
 bps2dlg();
}
void TbitrateDlg::time2dlg(void)
{
 double fps=getFps();
 if (fps==0)
  setDlgItemText(m_hwnd,IDC_LBL_BR_TIME,_l(""));
 else
  setText(IDC_LBL_BR_TIME,_l("%s %i s"),_(IDC_LBL_BR_TIME),int(numframes/fps));
}
void TbitrateDlg::bps2dlg(void)
{
 unsigned int bps=getbps();
 setText(IDC_LBL_BR_BPS,_l("%s %u kbps"),_(IDC_LBL_BR_BPS),bps);
}

INT_PTR TbitrateDlg::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_INITDIALOG:
    {
     //resizeDialog();
     translate();
     const char_t *capt=tr->translate(m_hwnd,dialogId,0,NULL);
     if (capt && capt[0]) setWindowText(m_hwnd,capt);
     init();
     return TRUE;
    }
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDOK:
       {
        EndDialog(m_hwnd,LOWORD(wParam));
        unsigned int bps=getbps();
        if (bps>0) cfgSet(IDFF_enc_bitrate1000,bps);
        cfgSet(IDFF_dlgBpsFps1000,int(1000*getFps()));
        cfgSet(IDFF_dlgBpsLen,getLen());
        return TRUE;
       }
      case IDCANCEL:
       EndDialog(m_hwnd,LOWORD(wParam));
       return TRUE;
      case IDC_CBX_BR_FPS:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         HWND hcbx=GetDlgItem(m_hwnd,IDC_CBX_BR_FPS);
         PostMessage(m_hwnd,WM_COMMAND,MAKEWPARAM(IDC_CBX_BR_FPS,CBN_EDITCHANGE),LPARAM(hcbx));
         return TRUE;
        }
       else if (HIWORD(wParam)==CBN_EDITCHANGE)
        {
         time2dlg();
         bps2dlg();
        }
       return TRUE;
      case IDC_ED_BR_LEN:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        bps2dlg();
       return TRUE;
     }
    break;
  }
 return TdlgWindow::msgProc(uMsg,wParam,lParam);
}

uint32_t TbitrateDlg::getLen(void) //in bytes
{
 BOOL ok;
 unsigned int len=GetDlgItemInt(m_hwnd,IDC_ED_BR_LEN,&ok,FALSE);
 return ok?len:0;
}
double TbitrateDlg::getFps(void)
{
 char_t Fpss[26];
 GetDlgItemText(m_hwnd,IDC_CBX_BR_FPS,Fpss,25);
 if (Fpss[0]=='\0') return 0;
 char_t *stop=NULL;
 double x=strtod(Fpss,&stop);
 if (*stop || x<0) return 0;
 return x;
}
unsigned int TbitrateDlg::getbps(void)
{
 double fps=getFps();
 if (fps==0)
  return 0;
 else
  return (unsigned int)(8*1000*(getLen()/1024.0)/(numframes/fps)+0.5);
}

TbitrateDlg::TbitrateDlg(HWND hParent,IffdshowBase *Ideci):deci(Ideci),TdlgWindow(IDD_BITRATE,Ideci)
{
 dialogBox(dialogId,hParent);
}
