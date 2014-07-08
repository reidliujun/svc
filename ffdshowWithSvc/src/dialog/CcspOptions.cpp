/*
 * Copyright (c) 2007 h.yamagata
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "CcspOptions.h"
#include "Ttranslate.h"
#include "TrgbPrimaries.h"
#include "ToutputVideoSettings.h"

void TcspOptionsPage::init(void)
{
 tbrSetRange(IDC_TBR_RGBCONV_BLACK, 0, 32, 1);
 tbrSetRange(IDC_TBR_RGBCONV_WHITE, 215, 255, 1);
 tbrSetRange(IDC_TBR_RGBCONV_CHROMA, 1, 32, 1);

 addHint(IDC_CBX_RGB_INTERLACE_METHOD, _l("This setting also applies to YV12 <-> YUY2 conversion."));
}

void TcspOptionsPage::cfg2dlg(void)
{
 int iturbt=cfgGet(IDFF_cspOptionsIturBt);
 setCheck(IDC_RBT_BT601, iturbt == TrgbPrimaries::ITUR_BT601);
 setCheck(IDC_RBT_BT709, iturbt == TrgbPrimaries::ITUR_BT709);
 int mode=cfgGet(IDFF_cspOptionsCutoffMode);
 setCheck(IDC_RBT_REC_YUV,    mode == TrgbPrimaries::RecYCbCr);
 setCheck(IDC_RBT_PC_YUV,     mode == TrgbPrimaries::PcYCbCr);
 setCheck(IDC_RBT_CUSTOM_YUV, mode == TrgbPrimaries::CutomYCbCr);

 int lock=cfgGet(IDFF_cspOptionsInterlockChroma);
 setCheck(IDC_CHB_RGBCONV_CHROMA_LOCK, lock);
 int blackCutoff  = cfgGet(IDFF_cspOptionsBlackCutoff);
 int whiteCutoff  = cfgGet(IDFF_cspOptionsWhiteCutoff);
 int chromaCutoff = cfgGet(IDFF_cspOptionsChromaCutoff);
 tbrSet(IDC_TBR_RGBCONV_BLACK,  blackCutoff,  IDC_TXT_RGBCONV_BLACK);
 tbrSet(IDC_TBR_RGBCONV_WHITE,  whiteCutoff,  IDC_TXT_RGBCONV_WHITE);
 tbrSet(IDC_TBR_RGBCONV_CHROMA, chromaCutoff, IDC_TXT_RGBCONV_CHROMA);

 char_t customRange[256];
 int fixedChromaCutoff = ToutputVideoSettings::get_cspOptionsChromaCutoffStatic(blackCutoff, whiteCutoff, chromaCutoff, lock);
 ffstring str(tr->translate(m_hwnd, dialogId, IDC_RBT_CUSTOM_YUV, NULL));
 strncpyf(customRange, countof(customRange), _l(" ( Y : %d-%d, chroma : %d-%d )"), blackCutoff, whiteCutoff, fixedChromaCutoff, 256 - fixedChromaCutoff);
 str+=customRange;
 setText(IDC_RBT_CUSTOM_YUV,str.c_str());

 static const int customs[] = {IDC_TBR_RGBCONV_BLACK,  IDC_TXT_RGBCONV_BLACK,
                               IDC_TBR_RGBCONV_WHITE,  IDC_TXT_RGBCONV_WHITE,
                               IDC_TBR_RGBCONV_CHROMA, IDC_TXT_RGBCONV_CHROMA,
                               IDC_CHB_RGBCONV_CHROMA_LOCK, 0};
 enable(mode == TrgbPrimaries::CutomYCbCr, customs);
 if (mode == TrgbPrimaries::CutomYCbCr)
  {
   static const int chromas[] = {IDC_TBR_RGBCONV_CHROMA, IDC_TXT_RGBCONV_CHROMA, 0};
   enable(!lock, chromas);
  }

 cbxSetCurSel(IDC_CBX_RGB_INTERLACE_METHOD,cfgGet(IDFF_cspOptionsRgbInterlaceMode));
 setCheck(IDC_CHB_AVISYNTH_YV12_RGB,cfgGet(IDFF_avisynthYV12_RGB));
}

void TcspOptionsPage::getTip(char_t *tipS,size_t len)
{
 const char_t *tip=_(-IDD_CSP_OPTIONS,_l("YCbCr <-> RGB conversion options"));
 ff_strncpy(tipS,tip,len);
}

void TcspOptionsPage::translate(void)
{
 TconfPageDecVideo::translate();

 cbxTranslate(IDC_CBX_RGB_INTERLACE_METHOD,ToutputVideoSettings::rgbInterlaceMethods);
}

bool TcspOptionsPage::reset(bool testonly)
{
 if (!testonly)
  {
   deci->resetParam(IDFF_cspOptionsIturBt);
   deci->resetParam(IDFF_cspOptionsCutoffMode);
   deci->resetParam(IDFF_cspOptionsBlackCutoff);
   deci->resetParam(IDFF_cspOptionsWhiteCutoff);
   deci->resetParam(IDFF_cspOptionsChromaCutoff);
   deci->resetParam(IDFF_cspOptionsInterlockChroma);
   deci->resetParam(IDFF_cspOptionsRgbInterlaceMode);
  }
 return true;
}

TcspOptionsPage::TcspOptionsPage(TffdshowPageDec *Iparent):TconfPageDecVideo(Iparent)
{
 dialogId=IDD_CSP_OPTIONS;
 helpURL=_l("http://ffdshow-tryout.wiki.sourceforge.net/RGB+conversion");
 inPreset=1;
 idffOrder=maxOrder+4;
 static const TbindRadiobutton<TcspOptionsPage> rbt[]=
  {
   IDC_RBT_BT601,      IDFF_cspOptionsIturBt,     TrgbPrimaries::ITUR_BT601, &TcspOptionsPage::cfg2dlg,
   IDC_RBT_BT709,      IDFF_cspOptionsIturBt,     TrgbPrimaries::ITUR_BT709, &TcspOptionsPage::cfg2dlg,
   IDC_RBT_REC_YUV,    IDFF_cspOptionsCutoffMode, TrgbPrimaries::RecYCbCr  , &TcspOptionsPage::cfg2dlg,
   IDC_RBT_PC_YUV,     IDFF_cspOptionsCutoffMode, TrgbPrimaries::PcYCbCr   , &TcspOptionsPage::cfg2dlg,
   IDC_RBT_CUSTOM_YUV, IDFF_cspOptionsCutoffMode, TrgbPrimaries::CutomYCbCr, &TcspOptionsPage::cfg2dlg,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindTrackbar<TcspOptionsPage> htbr[]=
  {
   IDC_TBR_RGBCONV_BLACK,  IDFF_cspOptionsBlackCutoff,  &TcspOptionsPage::cfg2dlg,
   IDC_TBR_RGBCONV_WHITE,  IDFF_cspOptionsWhiteCutoff,  &TcspOptionsPage::cfg2dlg,
   IDC_TBR_RGBCONV_CHROMA, IDFF_cspOptionsChromaCutoff, &TcspOptionsPage::cfg2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindCheckbox<TcspOptionsPage> chb[]=
  {
   IDC_CHB_RGBCONV_CHROMA_LOCK, IDFF_cspOptionsInterlockChroma, &TcspOptionsPage::cfg2dlg,
   IDC_CHB_AVISYNTH_YV12_RGB,IDFF_avisynthYV12_RGB,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindCombobox<TcspOptionsPage> cbx[]=
  {
   IDC_CBX_RGB_INTERLACE_METHOD,IDFF_cspOptionsRgbInterlaceMode,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
}
