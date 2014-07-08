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
#include "Cqos.h"
#include "Tlibavcodec.h"
#include "TvideoCodec.h"

void TqosPage::init(void)
{
 const TvideoCodecDec *movie;deciV->getMovieSource(&movie);
 int source=movie?movie->getType():0;
 islavc=((filterMode&IDFF_FILTERMODE_PLAYER) && source==IDFF_MOVIE_LAVC) || (filterMode&(IDFF_FILTERMODE_CONFIG|IDFF_FILTERMODE_VFW));
 static const int idLavc[]={IDC_CHB_H264_SKIP_ON_DELAY,IDC_ED_H264SKIP_ON_DELAY_TIME,IDC_CMB_H264_SKIP_METHOD,0};
 enable(islavc,idLavc);
}

void TmiscPage::cfg2dlg(void)
{
 if (islavc)
  {
   setCheck(IDC_CHB_H264_SKIP_ON_DELAY,cfgGet(IDFF_h264skipOnDelay));
   SetDlgItemInt(m_hwnd,IDC_ED_H264SKIP_ON_DELAY_TIME,cfgGet(IDFF_h264skipOnDelayTime),FALSE);
   cbxSetCurSel(IDC_CMB_H264_SKIP_METHOD,cfgGet(IDFF_fastH264));
  }
 setCheck(IDC_CHB_DROP_ON_DELAY,cfgGet(IDFF_dropOnDelay));
 SetDlgItemInt(m_hwnd,IDC_ED_DROP_ON_DELAY_TIME,cfgGet(IDFF_dropOnDelayTime),FALSE);
}

INT_PTR TmiscPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}
bool TmiscPage::reset(bool testonly)
{
 if (!testonly)
  {
   deci->resetParam(IDFF_dropOnDelay);
   deci->resetParam(IDFF_dropOnDelayTime);
   deci->resetParam(IDFF_h264skipOnDelay);
   deci->resetParam(IDFF_h264skipOnDelayTime);
   deci->resetParam(IDFF_fastH264);
  }
 return true;
}

void TmiscPage::translate(void)
{
 TconfPageBase::translate();
}

void TmiscPage::getTip(char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("IDCT: %s"), Tlibavcodec::idctNames[cfgGet(IDFF_idct)]);
 if (cfgGet(IDFF_grayscale))
  strncatf(tipS, len, _l("\nGrayscale"));
 int bugs=cfgGet(IDFF_workaroundBugs);
 if (bugs && bugs!=FF_BUG_AUTODETECT)
  strncatf(tipS, len, _l("\nBugs workaround"));
}

TqosPage::TqosPage(TffdshowPageDec *Iparent):TconfPageDecVideo(Iparent)
{
 dialogId=IDD_QOS;
 inPreset=1;
 idffOrder=maxOrder+2;
 static const TbindCheckbox<TmiscPage> chb[]=
  {
   IDC_CHB_DROP_ON_DELAY,IDFF_dropOnDelay,NULL,
   IDC_CHB_H264_SKIP_ON_DELAY,IDFF_h264skipOnDelay,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditInt<TmiscPage> edInt[]=
  {
   IDC_ED_DROP_ON_DELAY_TIME,0,20000,IDFF_dropOnDelayTime,NULL,
   IDC_ED_H264SKIP_ON_DELAY_TIME,0,20000,IDFF_h264skipOnDelayTime,NULL,
   0,NULL,NULL
  };
 bindEditInts(edInt);
 static const TbindCombobox<TmiscPage> cbx[]=
  {
   IDC_CMB_H264_SKIP_METHOD,IDFF_fastH264,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
}
