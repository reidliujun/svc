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
#include "TresizeAspectSettings.h"
#include "CresizeSettings.h"

const TresizeSettingsPage::TparamsIDs TresizeSettingsPage::idsLuma=
{
 IDFF_resizeMethodLuma,
 IDFF_resizeBicubicLumaParam,IDFF_resizeGaussLumaParam,IDFF_resizeLanczosLumaParam,
 IDC_LBL_RESIZE_PARAM_LUMA,IDC_TBR_RESIZE_PARAM_LUMA
};
const TresizeSettingsPage::TparamsIDs TresizeSettingsPage::idsChroma=
{
 IDFF_resizeMethodChroma,
 IDFF_resizeBicubicChromaParam,IDFF_resizeGaussChromaParam,IDFF_resizeLanczosChromaParam,
 IDC_LBL_RESIZE_PARAM_CHROMA,IDC_TBR_RESIZE_PARAM_CHROMA
};

void TresizeSettingsPage::init(void)
{
 tbrSetRange(IDC_TBR_RESIZE_GBLUR_LUM    ,0,200,20);
 tbrSetRange(IDC_TBR_RESIZE_GBLUR_CHROM  ,0,200,20);
 tbrSetRange(IDC_TBR_RESIZE_SHARPEN_LUM  ,0,200,20);
 tbrSetRange(IDC_TBR_RESIZE_SHARPEN_CHROM,0,200,20);
 tbrSetRange(IDC_TBR_RESIZE_HWARP        ,800,1300,200);
 tbrSetRange(IDC_TBR_RESIZE_VWARP        ,800,1300,200);
 tbrSetRange(IDC_TBR_RESIZE_PARAM_LUMA,0,1);tbrSetRange(IDC_TBR_RESIZE_PARAM_CHROMA,0,1);
}

void TresizeSettingsPage::cfg2dlg(void)
{
 cbxSetDataCurSel(IDC_CBX_RESIZE_METHOD_LUMA  ,cfgGet(IDFF_resizeMethodLuma  ));paramLuma2dlg();
 cbxSetDataCurSel(IDC_CBX_RESIZE_METHOD_CHROMA,cfgGet(IDFF_resizeMethodChroma));paramChroma2dlg();
 blurSharpen2dlg();
 setCheck3(IDC_CHB_RESIZE_INTERLACED,cfgGet(IDFF_resizeInterlaced));
 setCheck(IDC_CHB_RESIZE_ACCURATE_ROUNDING,cfgGet(IDFF_resizeAccurateRounding));
}
void TresizeSettingsPage::blurSharpen2dlg(void)
{
 int x;
 setText(IDC_LBL_RESIZE_GBLUR_LUM    ,_l("%s %3.2f"),_(IDC_LBL_RESIZE_GBLUR_LUM    ),(x=cfgGet(IDFF_resizeGblurLum    ))/100.0f);tbrSet(IDC_TBR_RESIZE_GBLUR_LUM    ,x);
 setText(IDC_LBL_RESIZE_GBLUR_CHROM  ,_l("%s %3.2f"),_(IDC_LBL_RESIZE_GBLUR_CHROM  ),(x=cfgGet(IDFF_resizeGblurChrom  ))/100.0f);tbrSet(IDC_TBR_RESIZE_GBLUR_CHROM  ,x);
 setText(IDC_LBL_RESIZE_SHARPEN_LUM  ,_l("%s %3.2f"),_(IDC_LBL_RESIZE_SHARPEN_LUM  ),(x=cfgGet(IDFF_resizeSharpenLum  ))/100.0f);tbrSet(IDC_TBR_RESIZE_SHARPEN_LUM  ,x);
 setText(IDC_LBL_RESIZE_SHARPEN_CHROM,_l("%s %3.2f"),_(IDC_LBL_RESIZE_SHARPEN_CHROM),(x=cfgGet(IDFF_resizeSharpenChrom))/100.0f);tbrSet(IDC_TBR_RESIZE_SHARPEN_CHROM,x);
 static const int idSh[]={IDC_TBR_RESIZE_SHARPEN_LUM,IDC_LBL_RESIZE_SHARPEN_LUM,
                          IDC_TBR_RESIZE_SHARPEN_CHROM,IDC_LBL_RESIZE_SHARPEN_CHROM,
                          IDC_TBR_RESIZE_GBLUR_LUM,IDC_LBL_RESIZE_GBLUR_LUM,
                          IDC_TBR_RESIZE_GBLUR_CHROM,IDC_LBL_RESIZE_GBLUR_CHROM,IDC_CHB_RESIZE_ACCURATE_ROUNDING,0};
 int method=cfgGet(IDFF_resizeMethodLuma);
 enable(TresizeAspectSettings::methodsProps[method].library==TresizeAspectSettings::LIB_SWSCALER,idSh);
}
void TresizeSettingsPage::warp2dlg(void)
{
 int x;
 setText(IDC_LBL_RESIZE_HWARP,_l("%s %4.3f"),_(IDC_LBL_RESIZE_HWARP),(x=cfgGet(IDFF_resizeSimpleWarpXparam))/1000.0f);tbrSet(IDC_TBR_RESIZE_HWARP,x);
 setText(IDC_LBL_RESIZE_VWARP,_l("%s %4.3f"),_(IDC_LBL_RESIZE_VWARP),(x=cfgGet(IDFF_resizeSimpleWarpYparam))/1000.0f);tbrSet(IDC_TBR_RESIZE_VWARP,x);
 static const int idWarp[]={IDC_LBL_RESIZE_HWARP,IDC_TBR_RESIZE_HWARP,IDC_LBL_RESIZE_VWARP,IDC_TBR_RESIZE_VWARP,0};
 enable(cfgGet(IDFF_resizeMethodLuma)==TresizeAspectSettings::METHOD_WARPED,idWarp);
}
void TresizeSettingsPage::paramLuma2dlg(void)
{
 param2dlg(idsLuma);
 static const int idsChroma[]={IDC_LBL_RESIZE_METHOD_CHROMA,IDC_CBX_RESIZE_METHOD_CHROMA,IDC_LBL_RESIZE_PARAM_CHROMA,IDC_TBR_RESIZE_PARAM_CHROMA,0};
 int locked=cfgGet(IDFF_resizeMethodsLocked);
 int sws=TresizeAspectSettings::methodsProps[cfgGet(IDFF_resizeMethodLuma)].library==TresizeAspectSettings::LIB_SWSCALER;
 enable(!locked && sws,idsChroma);
 setCheck(IDC_CHB_RESIZE_METHOD_LOCK,locked);enable(sws,IDC_CHB_RESIZE_METHOD_LOCK);
 warp2dlg();
 blurSharpen2dlg();
 paramChroma2dlg();
}
void TresizeSettingsPage::paramChroma2dlg(void)
{
 param2dlg(idsChroma);
 int locked=cfgGet(IDFF_resizeMethodsLocked);
 int sws=TresizeAspectSettings::methodsProps[cfgGet(IDFF_resizeMethodLuma)].library==TresizeAspectSettings::LIB_SWSCALER;
 static const int idsChroma[]={IDC_LBL_RESIZE_PARAM_CHROMA,IDC_TBR_RESIZE_PARAM_CHROMA,0};
 if (locked || !sws)
  enable(0,idsChroma);
}
void TresizeSettingsPage::param2dlg(const TparamsIDs &ids)
{
 int max,step1,step2,pos;
 char_t pomS[256];
 double realPos;
 const int idParam[]={ids.idc_lbl,ids.idc_tbr,0};
 int method=cfgGet(ids.idff_method);
 const char_t *name=_l("Parameter");

 switch (method)
  {
   case TresizeAspectSettings::METHOD_SWS_BICUBIC:
    max=400;step1=10;step2=40;pos=cfgGet(ids.idff_bicubicParam);
    realPos=-0.01*pos;
    pos=400-pos;
    break;
   case TresizeAspectSettings::METHOD_SWS_GAUSS:
    max=100;step1=5 ;step2=20;pos=cfgGet(ids.idff_gaussParam);
    realPos=0.1*pos;
    break;
   case TresizeAspectSettings::METHOD_SWS_LANCZOS:
    max=10 ;step1=1 ;step2=2 ;pos=cfgGet(ids.idff_lanczosParam);
    name=_l("Number of taps:");
    realPos=pos;
    break;
   default:
    enable(0,idParam);
    tbrSet(ids.idc_tbr,0);
    setDlgItemText(m_hwnd,ids.idc_lbl,_(ids.idc_lbl));
    return;
  };
 int sws=TresizeAspectSettings::methodsProps[cfgGet(IDFF_resizeMethodLuma)].library==TresizeAspectSettings::LIB_SWSCALER;
 enable(sws,idParam);
 tbrSetRange(ids.idc_tbr,0,max);
 //SendDlgItemMessage(m_hwnd,ids.idc_tbr,TBM_SETRANGE,TRUE,MAKELPARAM(0,max));
 SendDlgItemMessage(m_hwnd,ids.idc_tbr,TBM_SETLINESIZE,0,step1);
 SendDlgItemMessage(m_hwnd,ids.idc_tbr,TBM_SETPAGESIZE,0,step2);
 tbrSet(ids.idc_tbr,pos);
 if (realPos==0)
  tsnprintf_s(pomS, countof(pomS), _TRUNCATE,_l("%s %s"),_(ids.idc_lbl,name),_(ids.idc_lbl,_l("default")));
 else
  tsnprintf_s(pomS, countof(pomS), _TRUNCATE, _l("%s %3.2f"), _(ids.idc_lbl,name), realPos);
 setDlgItemText(m_hwnd,ids.idc_lbl,pomS);
}

INT_PTR TresizeSettingsPage::setParams(int pos,const TparamsIDs &ids)
{
 int id;
 switch (cfgGet(ids.idff_method))
  {
   case TresizeAspectSettings::METHOD_SWS_BICUBIC:id=ids.idff_bicubicParam;pos=400-pos;break;
   case TresizeAspectSettings::METHOD_SWS_GAUSS:id=ids.idff_gaussParam;break;
   case TresizeAspectSettings::METHOD_SWS_LANCZOS:id=ids.idff_lanczosParam;break;
   default:return TRUE;
  };
 cfgSet(id,pos);
 param2dlg(ids);
 return TRUE;
}

INT_PTR TresizeSettingsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_RESIZE_PARAM_LUMA:
       return setParams(tbrGet(IDC_TBR_RESIZE_PARAM_LUMA),idsLuma);
      case IDC_TBR_RESIZE_PARAM_CHROMA:
       return setParams(tbrGet(IDC_TBR_RESIZE_PARAM_CHROMA),idsChroma);
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_RESIZE_INTERLACED:
       cfgSet(IDFF_resizeInterlaced,getCheck3(IDC_CHB_RESIZE_INTERLACED));
       return TRUE;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

void TresizeSettingsPage::translate(void)
{
 TconfPageBase::translate();

 int selLuma=cbxGetCurSel(IDC_CBX_RESIZE_METHOD_LUMA),selChroma=cbxGetCurSel(IDC_CBX_RESIZE_METHOD_CHROMA);
 cbxClear(IDC_CBX_RESIZE_METHOD_LUMA);cbxClear(IDC_CBX_RESIZE_METHOD_CHROMA);
 for (int i=0;TresizeAspectSettings::methodsOrder[i]!=-1;i++)
  {
   int method=TresizeAspectSettings::methodsOrder[i];
   cbxAdd(IDC_CBX_RESIZE_METHOD_LUMA  ,_(IDC_CBX_RESIZE_METHOD_LUMA,TresizeAspectSettings::methodsProps[method].name),TresizeAspectSettings::methodsProps[method].id);
   if (TresizeAspectSettings::methodsProps[method].library==TresizeAspectSettings::LIB_SWSCALER)
    cbxAdd(IDC_CBX_RESIZE_METHOD_CHROMA,_(IDC_CBX_RESIZE_METHOD_CHROMA,TresizeAspectSettings::methodsProps[method].name),TresizeAspectSettings::methodsProps[method].id);
  }
 cbxSetCurSel(IDC_CBX_RESIZE_METHOD_LUMA,selLuma);cbxSetCurSel(IDC_CBX_RESIZE_METHOD_CHROMA,selChroma);
}

TresizeSettingsPage::TresizeSettingsPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff,3)
{
 dialogId=IDD_RESIZESETTINGS;
 static const TbindCheckbox<TresizeSettingsPage> chb[]=
  {
   IDC_CHB_RESIZE_METHOD_LOCK,IDFF_resizeMethodsLocked,&TresizeSettingsPage::cfg2dlg,
   IDC_CHB_RESIZE_ACCURATE_ROUNDING,IDFF_resizeAccurateRounding,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TresizeSettingsPage> htbr[]=
  {
   IDC_TBR_RESIZE_GBLUR_LUM,IDFF_resizeGblurLum,&TresizeSettingsPage::blurSharpen2dlg,
   IDC_TBR_RESIZE_GBLUR_CHROM,IDFF_resizeGblurChrom,&TresizeSettingsPage::blurSharpen2dlg,
   IDC_TBR_RESIZE_SHARPEN_LUM,IDFF_resizeSharpenLum,&TresizeSettingsPage::blurSharpen2dlg,
   IDC_TBR_RESIZE_SHARPEN_CHROM,IDFF_resizeSharpenChrom,&TresizeSettingsPage::blurSharpen2dlg,
   IDC_TBR_RESIZE_HWARP,IDFF_resizeSimpleWarpXparam,&TresizeSettingsPage::warp2dlg,
   IDC_TBR_RESIZE_VWARP,IDFF_resizeSimpleWarpYparam,&TresizeSettingsPage::warp2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindCombobox<TresizeSettingsPage> cbx[]=
  {
   IDC_CBX_RESIZE_METHOD_LUMA  ,IDFF_resizeMethodLuma  ,BINDCBX_DATA,&TresizeSettingsPage::paramLuma2dlg,
   IDC_CBX_RESIZE_METHOD_CHROMA,IDFF_resizeMethodChroma,BINDCBX_DATA,&TresizeSettingsPage::paramChroma2dlg,
   0
  };
 bindComboboxes(cbx);
}
