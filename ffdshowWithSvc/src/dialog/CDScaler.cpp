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
#include "CDScaler.h"
#include "TDScalerSettings.h"
#include "TimgFilterDScaler.h"

TDScalerSettings* TDScalerPageFlt::load(const char_t *flnm)
{
 if (flt) delete flt;
 flt=new Tdscaler_FLT(flnm,deci);
 bActive=(long*)&flt->fm->bActive;
 return flt->settings;
}

void TDScalerPageFlt::cfg2dlg(void)
{
 const char_t *fltflnm=cfgGetStr(IDFF_dscalerFltflnm);
 if (fltflnm[0])
  {
   cbxClear(IDC_CBX_DSCALER_FLTFLNM);
   char_t dsk[MAX_PATH],dir[MAX_PATH];
   _splitpath_s(fltflnm,dsk,MAX_PATH,dir,MAX_PATH,NULL,0,NULL,0);
   char_t fltmask[MAX_PATH];
   _makepath_s(fltmask,MAX_PATH,dsk,dir,_l("FLT_*"),_l("dll"));
   strings fltflnms;
   findFiles(fltmask,fltflnms);
   for (strings::const_iterator i=fltflnms.begin();i!=fltflnms.end();i++)
    cbxAdd(IDC_CBX_DSCALER_FLTFLNM,i->c_str());
   SendDlgItemMessage(m_hwnd,IDC_CBX_DSCALER_FLTFLNM,CB_SELECTSTRING,WPARAM(-1),LPARAM(fltflnm));
   settings=load(fltflnm);
   if (settings)
    {
     settings->str2cfg(cfgGetStr(IDFF_dscalerCfg));
     HWND place=GetDlgItem(m_hwnd,IDC_PLC_DSCALER);
     RECT placer,parentr;
     GetWindowRect(m_hwnd,&parentr);
     GetWindowRect(place,&placer);
     OffsetRect(&placer,-parentr.left,-parentr.top);
     HWND hprm=settings->createWindow(hi,this,placer,WM_DSCALER_CHANGE,bActive);
     if (hprm) SetWindowPos(hprm,place,placer.left,placer.top,placer.right-placer.left,placer.bottom-placer.top,SWP_SHOWWINDOW);
    }
  }
}

void TDScalerPageFlt::storeCfg(void)
{
 if (!settings) return;
 ffstring s;
 settings->cfg2str(s);
 if (s.empty() || s.size()==0) return;
 cfgSet(IDFF_dscalerCfg,s.c_str());
}

INT_PTR TDScalerPageFlt::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DSCALER_CHANGE:
    storeCfg();
    return TRUE;
   case WM_DESTROY:
    if (settings)
     {
      DestroyWindow(settings->wnd);
      settings->wnd=NULL;
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_BT_DSCALER_FLTFLNM:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         char_t fltflnm[MAX_PATH];cfgGet(IDFF_dscalerFltflnm,fltflnm,MAX_PATH);
         //Windows doesn't show dll files if "Show all files" is unchecked in Explorer's Folder Options
         if (dlgGetFile(false,m_hwnd,_(-IDD_DSCALER,_l("Select DScaler filter")),_l("DScaler filters (FLT_*.dll)\0FLT_*.dll\0\0"),_l("dll"),fltflnm,_l("."),0))
          {
           storeCfg();
           cfgSet(IDFF_dscalerFltflnm,fltflnm);
           cfg2dlg();
          }
         return TRUE;
        }
       break;
      case IDC_CBX_DSCALER_FLTFLNM:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         char_t fltflnm[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_CBX_DSCALER_FLTFLNM,fltflnm,MAX_PATH);
         if (fltflnm[0])
          {
           storeCfg();
           cfgSet(IDFF_dscalerFltflnm,fltflnm);
           cfg2dlg();
          }
         return TRUE;
        }
       break;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

bool TDScalerPageFlt::reset(bool testonly)
{
 if (!testonly && settings) settings->reset();
 return true;
}

TDScalerPageFlt::TDScalerPageFlt(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_DSCALER;
 settings=NULL;
 bActive=NULL;
 flt=NULL;
}

TDScalerPageFlt::~TDScalerPageFlt()
{
 if (flt) delete flt;
}
