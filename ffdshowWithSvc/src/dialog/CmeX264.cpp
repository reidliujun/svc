/*
 * Copyright (c) 2004-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "CmeX264.h"
#include "x264/x264.h"

const TmeX264page::Trefinements TmeX264page::refinements[]=
{
 1,_l("1 iteration of qpel on the winner"),
 2,_l("2 iterations of qpel"),
 3,_l("halfpel on all MB types, qpel on the winner"),
 4,_l("qpel on all"),
 5,_l("more iterations"),
 6,_l("rate-distortion optimized MB types"),
 0,NULL
};
const char_t* TmeX264page::predictions[]=
{
 _l("none"),
 _l("spatial"),
 _l("temporal"),
 _l("auto"),
 NULL
};

void TmeX264page::cfg2dlg(void)
{
 int intra=cfgGet(IDFF_enc_x264_me_intra);
 setCheck(IDC_CHB_X264_ME_INTRA_I4x4     ,intra&X264_ANALYSE_I4x4     );
 setCheck(IDC_CHB_X264_ME_INTRA_I8x8     ,intra&X264_ANALYSE_I8x8     );
 setCheck(IDC_CHB_X264_ME_INTRA_PSUB16x16,intra&X264_ANALYSE_PSUB16x16);
 setCheck(IDC_CHB_X264_ME_INTRA_PSUB8x8  ,intra&X264_ANALYSE_PSUB8x8  );
 setCheck(IDC_CHB_X264_ME_INTRA_BSUB16x16,intra&X264_ANALYSE_BSUB16x16);
 int inter=cfgGet(IDFF_enc_x264_me_inter);
 setCheck(IDC_CHB_X264_ME_INTER_I4x4     ,inter&X264_ANALYSE_I4x4     );
 setCheck(IDC_CHB_X264_ME_INTER_I8x8     ,inter&X264_ANALYSE_I8x8     );
 setCheck(IDC_CHB_X264_ME_INTER_PSUB16x16,inter&X264_ANALYSE_PSUB16x16);
 setCheck(IDC_CHB_X264_ME_INTER_PSUB8x8  ,inter&X264_ANALYSE_PSUB8x8  );
 setCheck(IDC_CHB_X264_ME_INTER_BSUB16x16,inter&X264_ANALYSE_BSUB16x16);
 SetDlgItemInt(m_hwnd,IDC_ED_X264_REF_FRAMES,cfgGet(IDFF_enc_x264_max_ref_frames),FALSE);
 cbxSetDataCurSel(IDC_CBX_X264_ME_SUBPELREFINEMENT,cfgGet(IDFF_enc_x264_me_subpelrefine));
 cbxSetCurSel(IDC_CBX_X264_ME_PREDICTION,cfgGet(IDFF_enc_x264_i_direct_mv_pred));
 static const int idBpred[]={IDC_LBL_X264_ME_PREDICTION,IDC_CBX_X264_ME_PREDICTION,0};
 enable(cfgGet(IDFF_enc_isBframes),idBpred);
 SetDlgItemInt(m_hwnd,IDC_ED_X264_MV_RANGE,cfgGet(IDFF_enc_x264_mv_range),FALSE);
 setCheck(IDC_CHB_X264_CHROMA_ME,cfgGet(IDFF_enc_me_cmp_chroma));
 method2dlg();
}
void TmeX264page::method2dlg(void)
{
 int method=cfgGet(IDFF_enc_x264_me_method);
 cbxSetCurSel(IDC_CBX_X264_ME_METHOD,method);
 static const int idRange[]={IDC_LBL_X264_ME_RANGE,IDC_ED_X264_ME_RANGE,0};
 SetDlgItemInt(m_hwnd,IDC_ED_X264_ME_RANGE,cfgGet(IDFF_enc_x264_me_range),FALSE);
 enable(method>=2,idRange);
}

INT_PTR TmeX264page::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_X264_ME_INTRA_I4x4:
      case IDC_CHB_X264_ME_INTRA_I8x8:
      case IDC_CHB_X264_ME_INTRA_PSUB16x16:
      case IDC_CHB_X264_ME_INTRA_PSUB8x8:
      case IDC_CHB_X264_ME_INTRA_BSUB16x16:
       {
        int intra=0;
        if (getCheck(IDC_CHB_X264_ME_INTRA_I4x4     )) intra|=X264_ANALYSE_I4x4;
        if (getCheck(IDC_CHB_X264_ME_INTRA_I8x8     )) intra|=X264_ANALYSE_I8x8;
        if (getCheck(IDC_CHB_X264_ME_INTRA_PSUB16x16)) intra|=X264_ANALYSE_PSUB16x16;
        if (getCheck(IDC_CHB_X264_ME_INTRA_PSUB8x8  )) intra|=X264_ANALYSE_PSUB8x8;
        if (getCheck(IDC_CHB_X264_ME_INTRA_BSUB16x16)) intra|=X264_ANALYSE_BSUB16x16;
        cfgSet(IDFF_enc_x264_me_intra,intra);
        return TRUE;
       }
      case IDC_CHB_X264_ME_INTER_I4x4:
      case IDC_CHB_X264_ME_INTER_I8x8:
      case IDC_CHB_X264_ME_INTER_PSUB16x16:
      case IDC_CHB_X264_ME_INTER_PSUB8x8:
      case IDC_CHB_X264_ME_INTER_BSUB16x16:
       {
        int inter=0;
        if (getCheck(IDC_CHB_X264_ME_INTER_I4x4     )) inter|=X264_ANALYSE_I4x4;
        if (getCheck(IDC_CHB_X264_ME_INTER_I8x8     )) inter|=X264_ANALYSE_I8x8;
        if (getCheck(IDC_CHB_X264_ME_INTER_PSUB16x16)) inter|=X264_ANALYSE_PSUB16x16;
        if (getCheck(IDC_CHB_X264_ME_INTER_PSUB8x8  )) inter|=X264_ANALYSE_PSUB8x8;
        if (getCheck(IDC_CHB_X264_ME_INTER_BSUB16x16)) inter|=X264_ANALYSE_BSUB16x16;
        cfgSet(IDFF_enc_x264_me_inter,inter);
        return TRUE;
       }
     }
    break;
  }
 return TconfPageEnc::msgProc(uMsg,wParam,lParam);
}

void TmeX264page::translate(void)
{
 TconfPageEnc::translate();

 int sel=cbxGetCurSel(IDC_CBX_X264_ME_SUBPELREFINEMENT);
 cbxClear(IDC_CBX_X264_ME_SUBPELREFINEMENT);
 for (int i=0;refinements[i].name;i++)
  cbxAdd(IDC_CBX_X264_ME_SUBPELREFINEMENT,_(IDC_CBX_X264_ME_SUBPELREFINEMENT,refinements[i].name),refinements[i].id);
 cbxSetCurSel(IDC_CBX_X264_ME_SUBPELREFINEMENT,sel);

 cbxTranslate(IDC_CBX_X264_ME_PREDICTION,predictions);
 cbxTranslate(IDC_CBX_X264_ME_METHOD,TcoSettings::x264_me_methods);
}

TmeX264page::TmeX264page(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_ME_X264;
 static const int props[]={IDFF_enc_x264_me_inter,IDFF_enc_x264_me_intra,IDFF_enc_x264_max_ref_frames,IDFF_enc_x264_me_subpelrefine,IDFF_enc_x264_i_direct_mv_pred,IDFF_enc_x264_mv_range,IDFF_enc_me_cmp_chroma,IDFF_enc_x264_me_method,IDFF_enc_x264_me_range,0};
 propsIDs=props;
 static const TbindCheckbox<TmeX264page> chb[]=
  {
   IDC_CHB_X264_CHROMA_ME,IDFF_enc_me_cmp_chroma,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditInt<TmeX264page> edInt[]=
  {
   IDC_ED_X264_REF_FRAMES,1,16,IDFF_enc_x264_max_ref_frames,NULL,
   IDC_ED_X264_MV_RANGE,1,511,IDFF_enc_x264_mv_range,NULL,
   IDC_ED_X264_ME_RANGE,1,1024,IDFF_enc_x264_me_range,NULL,
   0,NULL,NULL
  };
 bindEditInts(edInt);
 static const TbindCombobox<TmeX264page> cbx[]=
  {
   IDC_CBX_X264_ME_SUBPELREFINEMENT,IDFF_enc_x264_me_subpelrefine,BINDCBX_DATA,NULL,
   IDC_CBX_X264_ME_PREDICTION,IDFF_enc_x264_i_direct_mv_pred,BINDCBX_SEL,NULL,
   IDC_CBX_X264_ME_METHOD,IDFF_enc_x264_me_method,BINDCBX_SEL,&TmeX264page::method2dlg,
   0
  };
 bindComboboxes(cbx);
}
