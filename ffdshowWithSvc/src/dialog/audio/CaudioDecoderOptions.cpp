/*
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
#include "CaudioDecoderOptions.h"
#include "TsampleFormat.h"

void TaudioDecoderOptionsPage::init(void)
{
 tbrSetRange(IDC_TBR_DECODER_DRC_LEVEL,0,100);
}

void TaudioDecoderOptionsPage::cfg2dlg(void)
{
 drc2dlg();
}
void TaudioDecoderOptionsPage::drc2dlg(void)
{
 setCheck(IDC_CHB_AUDIO_DECODER_DRC,cfgGet(IDFF_audio_decoder_DRC));
 int level = cfgGet(IDFF_audio_decoder_DRC_Level);
 tbrSet(IDC_TBR_DECODER_DRC_LEVEL, level);
 setText(IDC_LBL_DECODER_DRC_LEVEL,_l("%i %%"),level);
}

INT_PTR TaudioDecoderOptionsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_DECODER_DRC_LEVEL:
       cfgSet(IDFF_audio_decoder_DRC_Level, tbrGet(IDC_TBR_DECODER_DRC_LEVEL));
	   drc2dlg();
       return TRUE;
     }
 }
 return TconfPageDecAudio::msgProc(uMsg,wParam,lParam);
}

bool TaudioDecoderOptionsPage::reset(bool testonly)
{
 if (!testonly)
  {
   deci->resetParam(IDFF_audio_decoder_DRC);
   deci->resetParam(IDFF_audio_decoder_DRC_Level);
  }
 return true;
}

void TaudioDecoderOptionsPage::translate(void)
{
 TconfPageDec::translate();
}

TaudioDecoderOptionsPage::TaudioDecoderOptionsPage(TffdshowPageDec *Iparent):TconfPageDecAudio(Iparent)
{
 dialogId=IDD_AUDIODECODEROPTIONS;
 inPreset=1;
 static const TbindCheckbox<TaudioDecoderOptionsPage> chb[]=
  {
   IDC_CHB_AUDIO_DECODER_DRC,IDFF_audio_decoder_DRC,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
}
