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
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "TDScalerSettings.h"
#include "DScaler/DS_Filter.h"
#include "reg.h"
#include "Tconfig.h"
#include "Twindow.h"
#include "Ttranslate.h"

TDScalerSettings::TDScalerSettings(IffdshowBase *Ideci,int InSettings,SETTING *IpSettings):deci(Ideci),deciV(Ideci)
{
 nSettings=InSettings;
 pSettings=IpSettings;
 wnd=NULL;
}
TDScalerSettings::~TDScalerSettings()
{
 if (wnd) DestroyWindow(wnd);
}

void TDScalerSettings::cfg2dlg(HWND m_hwnd)
{
 for (Tsets::iterator i=sets.begin();i!=sets.end();i++)
  switch (i->second->Type)
   {
    case ONOFF:
    case YESNO:
     {
      HWND hchb=GetDlgItem(m_hwnd,i->first);
      SendMessage(hchb,BM_SETCHECK,(*i->second->pValue)?BST_CHECKED:BST_UNCHECKED,0);
     }
    case ITEMFROMLIST:
     {
      HWND hcbx=GetDlgItem(m_hwnd,i->first);
      SendMessage(hcbx,CB_SETCURSEL,*i->second->pValue,0);
      break;
     }
    case SLIDER:
     {
      HWND hslider=GetDlgItem(m_hwnd,i->first);
      HWND hlbl=GetDlgItem(m_hwnd,i->first-1);
      char_t s[256];int x;
      x=*i->second->pValue;
      tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s: %i"), (const char_t*)text<char_t>(i->second->szDisplayName), x);
      SetWindowText(hlbl,s);
      SendMessage(hslider,TBM_SETPOS,TRUE,x);
      break;
     }
   }
}

void TDScalerSettings::writeops(TregOp &t)
{
 for (int i=0;i<nSettings;i++)
  {
   if (!pSettings[i].szDisplayName) break;
   if (pSettings[i].pValue!=bActive)
    t._REG_OP_N(0,text<char_t>(pSettings[i].szIniEntry),(int&)*pSettings[i].pValue,(int)*pSettings[i].pValue);
  }
}
void TDScalerSettings::cfg2str(ffstring &s)
{
 if (!pSettings) {s="";return;}
 TregOpStreamWrite t(',');
 writeops(t);
 t.end(0);
 s=(char_t*)&t.buf[0];
}
void TDScalerSettings::str2cfg(const ffstring &s)
{
 if (!pSettings) return;
 TregOpStreamRead t(s.c_str(),s.size(),',');
 deciV->lock(IDFF_lockDScaler);
 for (int i=0;i<nSettings;i++)
  {
   if (!pSettings[i].szDisplayName) break;
   int val=0;
   t._REG_OP_N(0,text<char_t>(pSettings[i].szIniEntry),val,*pSettings[i].pValue);
   //if (val!=*pSettings[i].pValue)
   if (pSettings[i].pfnOnChange)
    pSettings[i].pfnOnChange(val);
   else
    *pSettings[i].pValue=val;
  }
 deciV->unlock(IDFF_lockDScaler);
}
void TDScalerSettings::reset(void)
{
 if (!pSettings) return;
 deciV->lock(IDFF_lockDScaler);
 for (int i=0;i<nSettings;i++)
  {
   int val=pSettings[i].Default;
   if (pSettings[i].pfnOnChange)
    pSettings[i].pfnOnChange(val);
   else
    *pSettings[i].pValue=val;
  }
 deciV->unlock(IDFF_lockDScaler);
 SendMessage(parent,onChangeMsg,0,0);
}

BOOL CALLBACK TDScalerSettings::dscalerDlgEnum(HWND hwnd,LPARAM lParam)
{
 TDScalerSettings *self=(TDScalerSettings*)lParam;
 RECT r;GetWindowRect(hwnd,&r);
 if (self->cdy_top==-1) self->cdy_top=r.top;
 self->cdy=std::max(self->cdy,r.bottom);
 SetWindowPos(hwnd,NULL,0,0,self->rdx-self->sbrWidth-2,r.bottom-r.top,SWP_NOMOVE|SWP_NOZORDER);
 return TRUE;
}

INT_PTR CALLBACK TDScalerSettings::dscalerDlgProc(HWND m_hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 TDScalerSettings *self=(TDScalerSettings*)GetWindowLongPtr(m_hwnd,GWLP_USERDATA);
 switch (uMsg)
  {
   case WM_INITDIALOG:
    {
     self=(TDScalerSettings*)lParam;
     SetWindowLongPtr(m_hwnd,GWLP_USERDATA,LONG_PTR(self));
     self->cdy_top=-1;
     self->cdy=0;
     self->sbrWidth=GetSystemMetrics(SM_CXVSCROLL);
     EnumChildWindows(m_hwnd,dscalerDlgEnum,LPARAM(self));
     self->cdy-=self->cdy_top-2;
     for (Tsets::iterator i=self->sets.begin();i!=self->sets.end();i++)
      switch (i->second->Type)
       {
        case ITEMFROMLIST:
         {
          HWND hcbx=GetDlgItem(m_hwnd,i->first);
          for (int j=0;j<=i->second->MaxValue;j++)
           SendMessage(hcbx,CB_ADDSTRING,0,LPARAM((const char_t*)text<char_t>(i->second->pszList[j])));
          break;
         }
        case SLIDER:
         {
          HWND hslider=GetDlgItem(m_hwnd,i->first);
          SendMessage(hslider,TBM_SETRANGE,TRUE,MAKELPARAM(i->second->MinValue,i->second->MaxValue));
          SendMessage(hslider,TBM_SETLINESIZE,0,i->second->StepValue);
          SendMessage(hslider,TBM_SETPAGESIZE,0,i->second->StepValue*10);
          SetWindowLong(hslider,GWL_STYLE,GetWindowLong(hslider,GWL_STYLE)|TBS_FIXEDLENGTH);
          RECT r;
          GetWindowRect(hslider,&r);
          SendMessage(hslider,TBM_SETTHUMBLENGTH,r.bottom-r.top-2,0);
          break;
         }
       }
     self->cfg2dlg(m_hwnd);
     SCROLLINFO si;
     si.cbSize=sizeof(si);
     si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;
     si.nMin=0;
     si.nMax=self->cdy;
     si.nPage=self->rdy;
     si.nPos=0;
     SetScrollInfo(m_hwnd,SB_VERT,&si,TRUE);
     if (self->cdy<self->rdy)
      {
       self->sbrWidth=0;
       EnumChildWindows(m_hwnd,dscalerDlgEnum,LPARAM(self));
      }
     return TRUE;
    }
   case WM_COMMAND:
    {
     int id=LOWORD(wParam);
     Tsets::iterator s=self->sets.find(id);
     if (s==self->sets.end()) break;
     if (s->second->Type==YESNO || s->second->Type==ONOFF)
      *s->second->pValue=!*s->second->pValue;
     else if (s->second->Type==ITEMFROMLIST && HIWORD(wParam)==CBN_SELCHANGE)
      *s->second->pValue=(long)SendDlgItemMessage(m_hwnd,id,CB_GETCURSEL,0,0);
     else break;
     SendMessage(self->parent,self->onChangeMsg,0,0);
     return TRUE;
    }
   case WM_HSCROLL:
    {
     int id=Twindow::getId(HWND(lParam));
     Tsets::iterator s=self->sets.find(id);
     if (s==self->sets.end()) break;
     if (s->second->Type==SLIDER)
      {
       *s->second->pValue=(long)SendDlgItemMessage(m_hwnd,id,TBM_GETPOS,0,0);
       SendMessage(self->parent,self->onChangeMsg,0,0);
       self->cfg2dlg(m_hwnd);
       return TRUE;
      }
    }
   case WM_VSCROLL:
    {
     SCROLLINFO si;
     si.cbSize = sizeof (si);
     si.fMask  = SIF_ALL;
     // Save the position for comparison later on
     GetScrollInfo (m_hwnd, SB_VERT, &si);
     int oldpos = si.nPos;
     switch (LOWORD (wParam))
      {
       case SB_LINELEFT:si.nPos--;break;
       case SB_LINERIGHT:si.nPos++;break;
       case SB_PAGELEFT:si.nPos-=si.nPage;break;
       case SB_PAGERIGHT:si.nPos+=si.nPage;break;
       case SB_THUMBTRACK:si.nPos=si.nTrackPos;break;
       default:break;
      }
     // Set the position and then retrieve it.  Due to adjustments
     //   by Windows it may not be the same as the value set.
     si.fMask = SIF_POS;
     SetScrollInfo(m_hwnd,SB_VERT,&si,TRUE);
     GetScrollInfo(m_hwnd,SB_VERT,&si);

     // If the position has changed, scroll the window
     if (si.nPos!=oldpos) ScrollWindow(m_hwnd,0,oldpos-si.nPos,NULL,NULL);
     return TRUE;
    }
  }
 return FALSE;
}

HWND TDScalerSettings::createWindow(HINSTANCE hi,const Twindow *Iparent,const RECT &r,LONG IonChangeMsg,long *IbActive)
{
 if (!pSettings || nSettings==0) return wnd=NULL;
 parent=Iparent->m_hwnd;onChangeMsg=IonChangeMsg;
 bActive=IbActive;

 sets.clear();

 WCHAR *p=(WCHAR*)LocalAlloc(LPTR,300000);
 WCHAR *pdlgtemplate=p;

 *p++=1; // dlgver
 *p++=0xffff; // signature
 DWORD helpId=0;
 *p++ = LOWORD (helpId);
 *p++ = HIWORD (helpId);
 DWORD lExtendedStyle=WS_EX_CONTROLPARENT;
 *p++ = LOWORD (lExtendedStyle);
 *p++ = HIWORD (lExtendedStyle);
 DWORD lStyle = DS_SETFONT | DS_FIXEDSYS| DS_CONTROL/* | DS_CENTER*/ | WS_CHILD|WS_TABSTOP|WS_VSCROLL;
 *p++ = LOWORD (lStyle);
 *p++ = HIWORD (lStyle);
 WCHAR *nitems=p;*p++=0;  // NumberOfItems
 *p++ = 0;               // x
 *p++ = 0;               // y
 *p++ = 240;             // cx
 *p++ = 200;             // cy
 *p++ = 0;               // Menu
 *p++ = 0;               // Class

 int nchar;
 nchar = nCopyAnsiToWideChar(p,_l("Title 2"));
 p += nchar;

 bool customfont=Iparent->tr->isCustomFont();
 *p++ = WCHAR(customfont?Iparent->tr->font.pointsize:8);
 *p++ = 0;               // weight
 BYTE *pb=(BYTE*)p;
 pb[0]=customfont?Iparent->tr->font.italic:0;
 pb[1]=1; // charset
 p++;
 nchar = nCopyAnsiToWideChar(p,Iparent->tr->font.typeface[0]?Iparent->tr->font.typeface:_l("MS Shell Dlg"));  // Face name
 p += nchar;

 rdx=r.right-r.left;rdy=r.bottom-r.top;

 WORD y=1,id=1;
 for (int i=0;i<nSettings;i++)
  {
   SETTING &s=pSettings[i];
   if (s.Type==NOT_PRESENT || s.pValue==bActive) continue;
   else if (s.Type==ONOFF || s.Type==YESNO)
    {
     p = lpwAlign (p);
     helpId=0;
     *p++ = LOWORD (helpId);
     *p++ = HIWORD (helpId);
     lExtendedStyle=0;
     *p++ = LOWORD (lExtendedStyle);
     *p++ = HIWORD (lExtendedStyle);
     lStyle = BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE | WS_CHILD|WS_TABSTOP;
     *p++ = LOWORD (lStyle);
     *p++ = HIWORD (lStyle);
     *p++ = 2;          // x
     *p++ = y;y+=10;    // y
     *p++ = 200;        // cx
     *p++ = 10;         // cy
     *p++ = id;         // ID
     *p++ = 0;*p++ = 0xffff;*p++ = 0x0080;     // button
     nchar = nCopyAnsiToWideChar (p, text<char_t>(s.szDisplayName));
     p += nchar;
     *p++ = 0;
     (*nitems)++;
     sets[id]=&s;id++;
    }
   else if (s.Type==SLIDER)
    {
     p = lpwAlign (p);
     helpId=0;
     *p++ = LOWORD (helpId);
     *p++ = HIWORD (helpId);
     lExtendedStyle=0;
     *p++ = LOWORD (lExtendedStyle);
     *p++ = HIWORD (lExtendedStyle);
     lStyle = SS_LEFTNOWORDWRAP | SS_NOPREFIX | WS_VISIBLE | WS_CHILD;
     *p++ = LOWORD (lStyle);
     *p++ = HIWORD (lStyle);
     *p++ = 2;          // x
     *p++ = y;y+=10;    // y
     *p++ = 200;        // cx
     *p++ = 10;         // cy
     *p++ = id;         // ID
     *p++ = 0;*p++ = 0xffff;*p++ = 0x0082;     // static
     nchar = nCopyAnsiToWideChar (p, text<char_t>(s.szDisplayName));
     p += nchar;
     *p++ = 0;
     (*nitems)++;
     id++;

     p = lpwAlign (p);
     *p++ = LOWORD (helpId);
     *p++ = HIWORD (helpId);
     *p++ = LOWORD (lExtendedStyle);
     *p++ = HIWORD (lExtendedStyle);
     lStyle = TBS_BOTH | TBS_NOTICKS | WS_VISIBLE | WS_CHILD|WS_TABSTOP;
     *p++ = LOWORD (lStyle);
     *p++ = HIWORD (lStyle);
     *p++ = 0;          // x
     *p++ = y;y+=10;    // y
     *p++ = 200;        // cx
     *p++ = 10;         // cy
     *p++ = id;         // ID
     *p++ = 0;nchar = nCopyAnsiToWideChar (p, _l("msctls_trackbar32"));
     p += nchar;
     nchar = nCopyAnsiToWideChar (p, text<char_t>(s.szDisplayName));
     p += nchar;
     *p++ = 0;
     (*nitems)++;
     sets[id]=&s;id++;
    }
   else if (s.Type==ITEMFROMLIST)
    {
     p = lpwAlign (p);
     helpId=0;
     *p++ = LOWORD (helpId);
     *p++ = HIWORD (helpId);
     lExtendedStyle=0;
     *p++ = LOWORD (lExtendedStyle);
     *p++ = HIWORD (lExtendedStyle);
     lStyle = SS_LEFTNOWORDWRAP | SS_NOPREFIX | WS_VISIBLE | WS_CHILD;
     *p++ = LOWORD (lStyle);
     *p++ = HIWORD (lStyle);
     *p++ = 2;          // x
     *p++ = y;y+=10;    // y
     *p++ = 200;        // cx
     *p++ = 10;         // cy
     *p++ = 0;          // ID
     *p++ = 0;*p++ = 0xffff;*p++ = 0x0082;     // static
     nchar = nCopyAnsiToWideChar (p,text<char_t>(s.szDisplayName));
     p += nchar;
     *p++ = 0;
     (*nitems)++;

     p = lpwAlign (p);
     *p++ = LOWORD (helpId);
     *p++ = HIWORD (helpId);
     *p++ = LOWORD (lExtendedStyle);
     *p++ = HIWORD (lExtendedStyle);
     lStyle = CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD|WS_TABSTOP;
     *p++ = LOWORD (lStyle);
     *p++ = HIWORD (lStyle);
     *p++ = 2;          // x
     *p++ = y;y+=15;    // y
     *p++ = 200;        // cx
     *p++ = 100;        // cy
     *p++ = id;         // ID
     *p++ = 0;*p++ = 0xffff;*p++ = 0x0085;     // combobox
     nchar = nCopyAnsiToWideChar (p, text<char_t>(s.szDisplayName));
     p += nchar;
     *p++ = 0;
     (*nitems)++;
     sets[id]=&s;id++;
    }
  }
 wnd=CreateDialogIndirectParam(hi,LPCDLGTEMPLATEA(pdlgtemplate),parent,dscalerDlgProc,LPARAM(this));
 LocalFree(pdlgtemplate);
 return wnd;
}

int TDScalerSettings::dscalerCpu(void)
{
 long cpu=0;
 if (Tconfig::cpu_flags&FF_CPU_MMX)      cpu|=FEATURE_MMX;
 if (Tconfig::cpu_flags&FF_CPU_MMXEXT)   cpu|=FEATURE_MMXEXT;
 if (Tconfig::cpu_flags&FF_CPU_3DNOW)    cpu|=FEATURE_3DNOW;
 if (Tconfig::cpu_flags&FF_CPU_3DNOWEXT) cpu|=FEATURE_3DNOWEXT;
 if (Tconfig::cpu_flags&FF_CPU_SSE)      cpu|=FEATURE_SSE;
 if (Tconfig::cpu_flags&FF_CPU_SSE2)     cpu|=FEATURE_SSE2;
 return cpu;
}
