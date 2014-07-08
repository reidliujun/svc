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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Cblur.h"

void TblurPage::init(void)
{
 tbrSetRange(IDC_TBR_BLUR_SOFTEN,0,255,16);
 tbrSetRange(IDC_TBR_BLUR_TEMPSMOOTH,0,10,1);
 tbrSetRange(IDC_TBR_BLUR_SMOOTHLUMA,0,5000,100);
 tbrSetRange(IDC_TBR_BLUR_SMOOTHCHROMA,0,5000,100);
 tbrSetRange(IDC_TBR_BLUR_GRADUAL,5,100,5);
 tbrSetRange(IDC_TBR_BLUR_MPLAYER1,0,5000,500);
 tbrSetRange(IDC_TBR_BLUR_MPLAYER2,0,5000,500);
 tbrSetRange(IDC_TBR_BLUR_MPLAYER3,0,5000,500);
 tbrSetRange(IDC_TBR_BLUR_SWSCALER_RADIUS,1,9);
 tbrSetRange(IDC_TBR_BLUR_SWSCALER_LUMA  ,0,400,40);
 tbrSetRange(IDC_TBR_BLUR_SWSCALER_CHROMA,0,400,40);
 tbrSetRange(IDC_TBR_BLUR_DENOISE3D_LUMA  ,0,2000,100);
 tbrSetRange(IDC_TBR_BLUR_DENOISE3D_CHROMA,0,2000,100);
 tbrSetRange(IDC_TBR_BLUR_DENOISE3D_TIME  ,0,2000,100);
}

void TblurPage::cfg2dlg(void)
{
 soften2dlg();
 smooth2dlg();
 tempsmooth2dlg();
 gradual2dlg();
 mplayer2dlg();
 swscaler2dlg();
 denoise3d2dlg();
}

void TblurPage::tempsmooth2dlg(void)
{
 tbrSet(IDC_TBR_BLUR_TEMPSMOOTH,cfgGet(IDFF_tempSmooth),IDC_CHB_BLUR_TEMPSMOOTH);
 bool is=!!cfgGet(IDFF_blurIsTempSmooth);
 setCheck(IDC_CHB_BLUR_TEMPSMOOTH,is);
 enable(is,IDC_CHB_BLUR_TEMPSMOOTH_COLOR);
 setCheck(IDC_CHB_BLUR_TEMPSMOOTH_COLOR,cfgGet(IDFF_tempSmoothColor));
}
void TblurPage::denoise3d2dlg(void)
{
 int isDenoise3d=cfgGet(IDFF_blurIsDenoise3d);
 setCheck(IDC_CHB_BLUR_DENOISE3D,isDenoise3d);
 enable(isDenoise3d,IDC_CHB_BLUR_DENOISE3DHQ);setCheck(IDC_CHB_BLUR_DENOISE3DHQ,cfgGet(IDFF_denoise3Dhq));

 int x;char_t s[256];
 tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s - "), _(IDC_CHB_BLUR_DENOISE3D));
 x=cfgGet(IDFF_denoise3Dluma);
 strncatf(s, countof(s), _l("%s%.2f"),_(IDC_CHB_BLUR_DENOISE3D,_l("luma:")),x/100.0f);tbrSet(IDC_TBR_BLUR_DENOISE3D_LUMA,x);
 x=cfgGet(IDFF_denoise3Dchroma);
 strncatf(s, countof(s), _l(", %s%.2f"),_(IDC_CHB_BLUR_DENOISE3D,_l("chroma:")),x/100.0f);tbrSet(IDC_TBR_BLUR_DENOISE3D_CHROMA,x);
 x=cfgGet(IDFF_denoise3Dtime);
 strncatf(s, countof(s), _l(", %s%.2f"),_(IDC_CHB_BLUR_DENOISE3D,_l("time:")),x/100.0f);tbrSet(IDC_TBR_BLUR_DENOISE3D_TIME,x);
 setDlgItemText(m_hwnd,IDC_CHB_BLUR_DENOISE3D,s);
}
void TblurPage::mplayer2dlg(void)
{
 setCheck(IDC_CHB_BLUR_MPLAYER,cfgGet(IDFF_blurIsMplayerTNR));
 int x;char_t s[256];
 tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s "), _(IDC_CHB_BLUR_MPLAYER));
 x=cfgGet(IDFF_mplayerTNR1);
 strncatf(s, countof(s), _l("%i")  ,x);tbrSet(IDC_TBR_BLUR_MPLAYER1,x);
 x=cfgGet(IDFF_mplayerTNR2);
 strncatf(s, countof(s), _l(", %i"),x);tbrSet(IDC_TBR_BLUR_MPLAYER2,x);
 x=cfgGet(IDFF_mplayerTNR3);
 strncatf(s, countof(s), _l(", %i"),x);tbrSet(IDC_TBR_BLUR_MPLAYER3,x);
 setDlgItemText(m_hwnd,IDC_CHB_BLUR_MPLAYER,s);
}
void TblurPage::swscaler2dlg(void)
{
 setCheck(IDC_CHB_BLUR_SWSCALER,cfgGet(IDFF_blurIsMplayer));
 int x;char_t s[256];
 tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s - "), _(IDC_CHB_BLUR_SWSCALER));
 x=cfgGet(IDFF_mplayerBlurRadius);
 strncatf(s, countof(s),_l("%s%i"),_(IDC_CHB_BLUR_SWSCALER,_l("radius:")),x);tbrSet(IDC_TBR_BLUR_SWSCALER_RADIUS,x);
 x=cfgGet(IDFF_mplayerBlurLuma);
 strncatf(s, countof(s), _l(", %s%.2f"),_(IDC_CHB_BLUR_SWSCALER,_l("luma blur:")),x/100.0f);tbrSet(IDC_TBR_BLUR_SWSCALER_LUMA,x);
 x=cfgGet(IDFF_mplayerBlurChroma);
 strncatf(s, countof(s), _l(", %s%.2f"),_(IDC_CHB_BLUR_SWSCALER,_l("chroma blur:")),x/100.0f);tbrSet(IDC_TBR_BLUR_SWSCALER_CHROMA,x);
 setDlgItemText(m_hwnd,IDC_CHB_BLUR_SWSCALER,s);
}
void TblurPage::gradual2dlg(void)
{
 setCheck(IDC_CHB_BLUR_GRADUAL,cfgGet(IDFF_blurIsGradual));
 tbrSet(IDC_TBR_BLUR_GRADUAL,cfgGet(IDFF_gradualStrength),IDC_CHB_BLUR_GRADUAL);
}
void TblurPage::smooth2dlg(void)
{
 setCheck(IDC_CHB_BLUR_SMOOTHLUMA,cfgGet(IDFF_blurIsSmoothLuma));
 tbrSet(IDC_TBR_BLUR_SMOOTHLUMA,cfgGet(IDFF_smoothStrengthLuma),IDC_CHB_BLUR_SMOOTHLUMA);
 setCheck(IDC_CHB_BLUR_SMOOTHCHROMA,cfgGet(IDFF_blurIsSmoothChroma));
 tbrSet(IDC_TBR_BLUR_SMOOTHCHROMA,cfgGet(IDFF_smoothStrengthChroma),IDC_CHB_BLUR_SMOOTHCHROMA);
}
void TblurPage::soften2dlg(void)
{
 setCheck(IDC_CHB_BLUR_SOFTEN,cfgGet(IDFF_blurIsSoften));
 tbrSet(IDC_TBR_BLUR_SOFTEN,cfgGet(IDFF_blurStrength),IDC_CHB_BLUR_SOFTEN);
}

TblurPage::TblurPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_BLUR;
 static const TbindCheckbox<TblurPage> chb[]=
  {
   IDC_CHB_BLUR_SOFTEN,IDFF_blurIsSoften,NULL,
   IDC_CHB_BLUR_TEMPSMOOTH,IDFF_blurIsTempSmooth,&TblurPage::tempsmooth2dlg,
   IDC_CHB_BLUR_TEMPSMOOTH_COLOR,IDFF_tempSmoothColor,NULL,
   IDC_CHB_BLUR_SMOOTHLUMA,IDFF_blurIsSmoothLuma,NULL,
   IDC_CHB_BLUR_SMOOTHCHROMA,IDFF_blurIsSmoothChroma,NULL,
   IDC_CHB_BLUR_GRADUAL,IDFF_blurIsGradual,NULL,
   IDC_CHB_BLUR_MPLAYER,IDFF_blurIsMplayerTNR,NULL,
   IDC_CHB_BLUR_SWSCALER,IDFF_blurIsMplayer,NULL,
   IDC_CHB_BLUR_DENOISE3D,IDFF_blurIsDenoise3d,&TblurPage::denoise3d2dlg,
   IDC_CHB_BLUR_DENOISE3DHQ,IDFF_denoise3Dhq,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TblurPage> htbr[]=
  {
   IDC_TBR_BLUR_SOFTEN,IDFF_blurStrength,&TblurPage::soften2dlg,
   IDC_TBR_BLUR_TEMPSMOOTH,IDFF_tempSmooth,&TblurPage::tempsmooth2dlg,
   IDC_TBR_BLUR_SMOOTHLUMA,IDFF_smoothStrengthLuma,&TblurPage::smooth2dlg,
   IDC_TBR_BLUR_SMOOTHCHROMA,IDFF_smoothStrengthChroma,&TblurPage::smooth2dlg,
   IDC_TBR_BLUR_GRADUAL,IDFF_gradualStrength,&TblurPage::gradual2dlg,
   IDC_TBR_BLUR_MPLAYER1,IDFF_mplayerTNR1,&TblurPage::mplayer2dlg,
   IDC_TBR_BLUR_MPLAYER2,IDFF_mplayerTNR2,&TblurPage::mplayer2dlg,
   IDC_TBR_BLUR_MPLAYER3,IDFF_mplayerTNR3,&TblurPage::mplayer2dlg,
   IDC_TBR_BLUR_SWSCALER_RADIUS,IDFF_mplayerBlurRadius,&TblurPage::swscaler2dlg,
   IDC_TBR_BLUR_SWSCALER_LUMA,IDFF_mplayerBlurLuma,&TblurPage::swscaler2dlg,
   IDC_TBR_BLUR_SWSCALER_CHROMA,IDFF_mplayerBlurChroma,&TblurPage::swscaler2dlg,
   IDC_TBR_BLUR_DENOISE3D_LUMA,IDFF_denoise3Dluma,&TblurPage::denoise3d2dlg,
   IDC_TBR_BLUR_DENOISE3D_CHROMA,IDFF_denoise3Dchroma,&TblurPage::denoise3d2dlg,
   IDC_TBR_BLUR_DENOISE3D_TIME,IDFF_denoise3Dtime,&TblurPage::denoise3d2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
}

