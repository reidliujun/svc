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
#include "CdecoderOptions.h"
#include "Tlibavcodec.h"
#include "TvideoCodec.h"
#include "CquantTables.h"
#include "Ttranslate.h"

const TmiscPage::Tworkaround TmiscPage::workarounds[]=
{
 FF_BUG_AUTODETECT ,IDC_CHB_WORKAROUND_AUTODETECT,
 FF_BUG_OLD_MSMPEG4,IDC_CHB_WORKAROUND_OLDMSMPEG4,
 FF_BUG_XVID_ILACE ,IDC_CHB_WORKAROUND_XVIDILACE ,
 FF_BUG_UMP4       ,IDC_CHB_WORKAROUND_UMP4      ,
 FF_BUG_NO_PADDING ,IDC_CHB_WORKAROUND_NOPADDING ,
 FF_BUG_QPEL_CHROMA,IDC_CHB_WORKAROUND_QPELCHROMA,
 FF_BUG_EDGE       ,IDC_CHB_WORKAROUND_EDGE      ,
 FF_BUG_HPEL_CHROMA,IDC_CHB_WORKAROUND_HPELCHROMA,
 FF_BUG_AMV        ,IDC_CHB_WORKAROUND_AMV       ,
 FF_BUG_DC_CLIP    ,IDC_CHB_WORKAROUND_DC_CLIP   ,
 0,0
};

void TmiscPage::init(void)
{
 const TvideoCodecDec *movie;deciV->getMovieSource(&movie);
 int source=movie?movie->getType():0;
 islavc=((filterMode&IDFF_FILTERMODE_PLAYER) && (source==IDFF_MOVIE_LAVC || source==IDFF_MOVIE_LSVC)) || (filterMode&(IDFF_FILTERMODE_CONFIG|IDFF_FILTERMODE_VFW));
 for (int i=0;workarounds[i].ff_bug;i++)
  enable(islavc,workarounds[i].idc_chb);
 static const int idLavc[]={IDC_LBL_IDCT,IDC_CBX_IDCT,IDC_CHB_GRAY,IDC_LBL_BUGS,IDC_LBL_ERROR_CONCEALMENT,IDC_CBX_ERROR_CONCEALMENT,IDC_LBL_ERROR_RECOGNITION,IDC_CBX_ERROR_RECOGNITION,IDC_BT_QUANTMATRIX_EXPORT,IDC_ED_NUMTHREADS,IDC_CHB_H264_SKIP_ON_DELAY,IDC_ED_H264SKIP_ON_DELAY_TIME,0};
 enable(islavc,idLavc);
 addHint(IDC_ED_NUMTHREADS,_l("H.264 and MPEG-1/2 decoder only"));
}

void TmiscPage::cfg2dlg(void)
{
 if (islavc)
  {
   setCheck(IDC_CHB_GRAY,cfgGet(IDFF_grayscale));
   cbxSetCurSel(IDC_CBX_IDCT,cfgGet(IDFF_idct));

   int bugs=cfgGet(IDFF_workaroundBugs);
   for (int i=0;workarounds[i].ff_bug;i++)
    setCheck(workarounds[i].idc_chb,bugs&workarounds[i].ff_bug);
   cbxSetCurSel(IDC_CBX_ERROR_RECOGNITION,cfgGet(IDFF_errorRecognition));
   cbxSetCurSel(IDC_CBX_ERROR_CONCEALMENT,cfgGet(IDFF_errorConcealment));
   SetDlgItemInt(m_hwnd,IDC_ED_NUMTHREADS,cfgGet(IDFF_numLAVCdecThreads),FALSE);
   setCheck(IDC_CHB_H264_SKIP_ON_DELAY,cfgGet(IDFF_h264skipOnDelay));
   SetDlgItemInt(m_hwnd,IDC_ED_H264SKIP_ON_DELAY_TIME,cfgGet(IDFF_h264skipOnDelayTime),FALSE);
  }
 setCheck(IDC_CHB_DROP_ON_DELAY,cfgGet(IDFF_dropOnDelay));
 SetDlgItemInt(m_hwnd,IDC_ED_DROP_ON_DELAY_TIME,cfgGet(IDFF_dropOnDelayTime),FALSE);
}

INT_PTR TmiscPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_WORKAROUND_AUTODETECT:
      case IDC_CHB_WORKAROUND_OLDMSMPEG4:
      case IDC_CHB_WORKAROUND_XVIDILACE:
      case IDC_CHB_WORKAROUND_UMP4:
      case IDC_CHB_WORKAROUND_NOPADDING:
      case IDC_CHB_WORKAROUND_QPELCHROMA:
      case IDC_CHB_WORKAROUND_EDGE:
      case IDC_CHB_WORKAROUND_HPELCHROMA:
      case IDC_CHB_WORKAROUND_AMV:
      case IDC_CHB_WORKAROUND_DC_CLIP:
       {
        int bugs=0;
        for (int i=0;workarounds[i].ff_bug;i++)
         if (getCheck(workarounds[i].idc_chb)) bugs|=workarounds[i].ff_bug;
        deci->putParam(IDFF_workaroundBugs,bugs);
        return TRUE;
       }
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}
bool TmiscPage::reset(bool testonly)
{
 if (!testonly)
  {
   deci->resetParam(IDFF_idct);
   deci->resetParam(IDFF_workaroundBugs);
   deci->resetParam(IDFF_errorConcealment);
   deci->resetParam(IDFF_errorRecognition);
   deci->resetParam(IDFF_numLAVCdecThreads);
   deci->resetParam(IDFF_dropOnDelay);
   deci->resetParam(IDFF_dropOnDelayTime);
   deci->resetParam(IDFF_h264skipOnDelay);
   deci->resetParam(IDFF_h264skipOnDelayTime);
  }
 return true;
}

void TmiscPage::translate(void)
{
 TconfPageBase::translate();

 cbxTranslate(IDC_CBX_IDCT,Tlibavcodec::idctNames);
 cbxTranslate(IDC_CBX_ERROR_CONCEALMENT,Tlibavcodec::errorConcealments);
 cbxTranslate(IDC_CBX_ERROR_RECOGNITION,Tlibavcodec::errorRecognitions);
}

void TmiscPage::getTip(char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("IDCT: %s"),Tlibavcodec::idctNames[cfgGet(IDFF_idct)]);
 if (cfgGet(IDFF_grayscale))
  strncatf(tipS, len, _l("\nGrayscale"));
 int bugs=cfgGet(IDFF_workaroundBugs);
 if (bugs && bugs!=FF_BUG_AUTODETECT)
  strncatf(tipS, len, _l("\nBugs workaround"));
}

void TmiscPage::onMatrixExport(void)
{
 uint8_t inter[64],intra[64];
 if (deciV->getQuantMatrices(intra,inter)!=S_OK)
  {
   err(_(-IDD_DECODEROPTIONS,_l("No quantization matrices available.")));
   return;
  }
 TcurrentQuantDlg dlg(m_hwnd,deci,inter,intra);
 dlg.show();
}

TmiscPage::TmiscPage(TffdshowPageDec *Iparent):TconfPageDecVideo(Iparent)
{
 dialogId=IDD_DECODEROPTIONS;
 inPreset=1;
 static const TbindCheckbox<TmiscPage> chb[]=
  {
   IDC_CHB_GRAY,IDFF_grayscale,NULL,
   IDC_CHB_DROP_ON_DELAY,IDFF_dropOnDelay,NULL,
   IDC_CHB_H264_SKIP_ON_DELAY,IDFF_h264skipOnDelay,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditInt<TmiscPage> edInt[]=
  {
   IDC_ED_NUMTHREADS,1,8,IDFF_numLAVCdecThreads,NULL,
   IDC_ED_DROP_ON_DELAY_TIME,0,20000,IDFF_dropOnDelayTime,NULL,
   IDC_ED_H264SKIP_ON_DELAY_TIME,0,20000,IDFF_h264skipOnDelayTime,NULL,
   0,NULL,NULL
  };
 bindEditInts(edInt);
 static const TbindCombobox<TmiscPage> cbx[]=
  {
   IDC_CBX_IDCT,IDFF_idct,BINDCBX_SEL,NULL,
   IDC_CBX_ERROR_RECOGNITION,IDFF_errorRecognition,BINDCBX_SEL,NULL,
   IDC_CBX_ERROR_CONCEALMENT,IDFF_errorConcealment,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
 static const TbindButton<TmiscPage> bt[]=
  {
   IDC_BT_QUANTMATRIX_EXPORT,&TmiscPage::onMatrixExport,
   0,NULL
  };
 bindButtons(bt);
}

