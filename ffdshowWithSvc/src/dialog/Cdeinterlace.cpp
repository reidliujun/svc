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
#include "Cdeinterlace.h"
#include "TdeinterlaceSettings.h"
#include "Tconfig.h"
#include "CdeinterlacePanel.h"
#include "CdeinterlaceDScaler.h"
#include "TimgFilterDScalerDeinterlace.h"

void TdeinterlacePage::init(void)
{
 panel=NULL;dscalerstart=0;
 place=GetDlgItem(m_hwnd,IDC_PLC_DEINT);
 RECT parentr;
 GetWindowRect(m_hwnd,&parentr);
 GetWindowRect(place,&placer);
 OffsetRect(&placer,-parentr.left,-parentr.top);
 cbxSetDroppedWidth(IDC_CBX_DEINT_METHOD,300);
 const char_t *dsflnm=cfgGetStr(IDFF_dscalerDIflnm);
 const char_t *dsdir=cfgGetStr(IDFF_dscalerPath);
 if (dsdir[0]=='\0' && dsflnm[0]!='\0')
  {
   char_t dsdir1[MAX_PATH];extractfilepath(dsflnm,dsdir1);
   cfgSet(IDFF_dscalerPath,dsdir1);
  }
}

void TdeinterlacePage::cfg2dlg(void)
{
 fillDeinterlacers();
 deint2dlg();
}
void TdeinterlacePage::deint2dlg(void)
{
 setCheck(IDC_CHB_DEINTERLACEALWAYS,cfgGet(IDFF_deinterlaceAlways));
 setCheck(IDC_CHB_SWAP_FIELDS,cfgGet(IDFF_swapFields));//enable(cfgGet(IDFF_isDeinterlace),IDC_CHB_SWAP_FIELDS);
deint2dlgAgain:
 int method=cfgGet(IDFF_deinterlaceMethod);
 if (panel) delete panel;panel=NULL;
 const TdeinterlaceSettings::TmethodProps &methodProps=TdeinterlaceSettings::getMethod(method);
 if (methodProps.id!=TdeinterlaceSettings::DSCALER)
  {
   cbxSetDataCurSel(IDC_CBX_DEINT_METHOD,method);
   if (methodProps.create)
    panel=methodProps.create(this);
  }
 else
  {
   strings::const_iterator diflnm=std::find(diflnms.begin(),diflnms.end(),cfgGetStr(IDFF_dscalerDIflnm));
   if (diflnm!=diflnms.end())
    {
     cbxSetCurSel(IDC_CBX_DEINT_METHOD,int(diflnm-diflnms.begin()+dscalerstart));
     panel=new TdeinterlacePageDScaler(this,diflnm->c_str());
    }
   else
    {
     cfgSet(IDFF_deinterlaceMethod,0);
     goto deint2dlgAgain;
    }
  }
 if (panel)
  {
   SetWindowPos(panel->m_hwnd,place,placer.left,placer.top,0,0,SWP_NOSIZE);
   panel->cfg2dlg();
  }
}
INT_PTR TdeinterlacePage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    if (panel) delete panel;
    break;
   case TdeinterlacePageDScaler::WM_DSCALERPAGE_CHANGE:
    panel->storeCfg();
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CBX_DEINT_METHOD:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         int ii=cbxGetCurSel(IDC_CBX_DEINT_METHOD);
         if (dscalerstart && ii>=dscalerstart)
          cfgSet(IDFF_dscalerDIflnm,diflnms[ii-dscalerstart].c_str());
        }
       break;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

void TdeinterlacePage::fillDeinterlacers(void)
{
 int ii=cbxGetCurSel(IDC_CBX_DEINT_METHOD);
 cbxClear(IDC_CBX_DEINT_METHOD);

 int i;
 for (i=0;TdeinterlaceSettings::methodProps[i].name;i++)
  if (!(TdeinterlaceSettings::methodProps[i].id==TdeinterlaceSettings::FRAMERATEDOUBLER && !(Tconfig::cpu_flags&FF_CPU_MMXEXT)) && TdeinterlaceSettings::methodProps[i].id!=TdeinterlaceSettings::DSCALER)
   cbxAdd(IDC_CBX_DEINT_METHOD,_(IDC_CBX_DEINT_METHOD,TdeinterlaceSettings::methodProps[i].name),TdeinterlaceSettings::methodProps[i].cfgId);

 const char_t *dscalerDir=cfgGetStr(IDFF_dscalerPath);
 if (!dscalerDir || !dscalerDir[0])
  {
   dscalerstart=0;
   return;
  }
 char_t dsmask[MAX_PATH];
 _makepath_s(dsmask,MAX_PATH,NULL,dscalerDir,_l("DI_*"),_l("dll"));
 strings dis;findFiles(dsmask,dis,true);
 dscalerstart=i-1;diflnms.clear();
 for (strings::const_iterator di=dis.begin();di!=dis.end();di++)
  {
   char_t flnm[MAX_PATH];
   extractfilename(di->c_str(),flnm);
   if (stricmp(flnm,_l("DI_Adaptive.dll"))!=0)
    {
     char_t diname[256];
     Tdscaler_DI didll(di->c_str(),deci,true);
     if (didll.fm)
      {
       tsnprintf_s(diname, countof(diname), _TRUNCATE, _l("DScaler: %s"),(const char_t*)text<char_t>(didll.fm->szName));
       cbxAdd(IDC_CBX_DEINT_METHOD,diname,9);
       diflnms.push_back(*di);
      }
    }
  }
 cbxSetCurSel(IDC_CBX_DEINT_METHOD,ii);
}
void TdeinterlacePage::translate(void)
{
 TconfPageDec::translate();
 fillDeinterlacers();
}

void TdeinterlacePage::getTip(char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("Method: %s"), cbxGetItemCount(IDC_CBX_DEINT_METHOD) ? cbxGetCurText(IDC_CBX_DEINT_METHOD) : _l("DScaler"));
}
bool TdeinterlacePage::reset(bool testonly)
{
 if (!testonly)
  {
   if (panel) panel->reset();
   deci->resetParam(IDFF_swapFields);
  }
 return true;
}

TdeinterlacePage::TdeinterlacePage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_DEINTERLACE;
 static const TbindCheckbox<TdeinterlacePage> chb[]=
  {
   IDC_CHB_DEINTERLACEALWAYS,IDFF_deinterlaceAlways,NULL,
   IDC_CHB_SWAP_FIELDS,IDFF_swapFields,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindCombobox<TdeinterlacePage> cbx[]=
  {
   IDC_CBX_DEINT_METHOD,IDFF_deinterlaceMethod,BINDCBX_DATA,&TdeinterlacePage::deint2dlg,
   0
  };
 bindComboboxes(cbx);
}
