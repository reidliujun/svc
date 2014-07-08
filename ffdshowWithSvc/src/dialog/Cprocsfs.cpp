/*
 * Copyright (c) 2004-2006 Milan Cutka
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
#include "Cprocsfs.h"
#include "TsampleFormat.h"

const char_t* TprocsfsPage::noiseShapings[]=
{
 _l("none"),
 _l("light"),
 _l("medium"),
 _l("heavy"),
 NULL,
};

void TprocsfsPage::cfg2dlg(void)
{
 preferred2dlg();
 dither2dlg();
}
void TprocsfsPage::preferred2dlg(void)
{
 int prefsfs=cfgGet(IDFF_preferredsfs);
 setCheck(IDC_CHB_PREFERRED_PCM16  ,prefsfs&TsampleFormat::SF_PCM16  );
 setCheck(IDC_CHB_PREFERRED_PCM32  ,prefsfs&TsampleFormat::SF_PCM32  );
 setCheck(IDC_CHB_PREFERRED_FLOAT32,prefsfs&TsampleFormat::SF_FLOAT32);
}
void TprocsfsPage::dither2dlg(void)
{
 setCheck(IDC_CHB_DITHERING,cfgGet(IDFF_dithering));
 cbxSetCurSel(IDC_CBX_NOISESHAPING,cfgGet(IDFF_noiseShaping));
}

INT_PTR TprocsfsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_PREFERRED_PCM16:
      case IDC_CHB_PREFERRED_PCM32:
      case IDC_CHB_PREFERRED_FLOAT32:
       {
        int prefsfs=0;
        if (getCheck(IDC_CHB_PREFERRED_PCM16  )) prefsfs|=TsampleFormat::SF_PCM16;
        if (getCheck(IDC_CHB_PREFERRED_PCM32  )) prefsfs|=TsampleFormat::SF_PCM32;
        if (getCheck(IDC_CHB_PREFERRED_FLOAT32)) prefsfs|=TsampleFormat::SF_FLOAT32;
        if (prefsfs)
         cfgSet(IDFF_preferredsfs,prefsfs);
        else
         setCheck(LOWORD(wParam),!getCheck(LOWORD(wParam)));
        return TRUE;
       }
     }
    break;
  }
 return TconfPageDecAudio::msgProc(uMsg,wParam,lParam);
}
bool TprocsfsPage::reset(bool testonly)
{
 if (!testonly)
  {
   deci->resetParam(IDFF_preferredsfs);
   deci->resetParam(IDFF_noiseShaping);
   deci->resetParam(IDFF_dithering);
  }
 return true;
}

void TprocsfsPage::translate(void)
{
 TconfPageDec::translate();

 cbxTranslate(IDC_CBX_NOISESHAPING,noiseShapings);
}

TprocsfsPage::TprocsfsPage(TffdshowPageDec *Iparent):TconfPageDecAudio(Iparent)
{
 dialogId=IDD_PROCSFS;
 inPreset=1;
 static const TbindCheckbox<TprocsfsPage> chb[]=
  {
   IDC_CHB_DITHERING,IDFF_dithering,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindCombobox<TprocsfsPage> cbx[]=
  {
   IDC_CBX_NOISESHAPING,IDFF_noiseShaping,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
}
