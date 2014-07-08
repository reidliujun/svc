/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include "Cme.h"
#include "Tlibavcodec.h"

const char_t* TmePage::mb_decisions[]=
{
 _l("simple"),
 _l("fewest bits"),
 _l("rate distortion"),
 NULL
};
const char_t* TmePage::me_cmps[]=
{
 _l("SAD"),
 _l("SSE"),
 _l("SATD"),
 _l("DCT"),
 _l("PSNR"),
 _l("BIT"),
 _l("RD"),
 _l("ZERO"),
 _l("VSAD"),
 _l("VSSE"),
 _l("NSSE"),
 _l("W53"),
 _l("W97"),
 _l("DCTMAX"),
 _l("DCT264"),
 NULL
};
const char_t* TmePage::me_prepasses[]=
{
 _l("never"),
 _l("after I frames"),
 _l("always"),
 NULL
};

void TmePage::init(void)
{
 for (int i=0;me_cmps[i];i++)
  {
   cbxAdd(IDC_CBX_ME_CMP,me_cmps[i]);
   cbxAdd(IDC_CBX_ME_SUBCMP,me_cmps[i]);
   cbxAdd(IDC_CBX_MB_CMP,me_cmps[i]);
   cbxAdd(IDC_CBX_ME_PRECMP,me_cmps[i]);
  }
}

bool TmePage::enabled(void)
{
 return sup_lavcme(codecId);
}

void TmePage::cfg2dlg(void)
{
 cbxSetCurSel(IDC_CBX_ME_CMP,cfgGet(IDFF_enc_me_cmp));setCheck(IDC_CHB_ME_CMP_CHROMA,cfgGet(IDFF_enc_me_cmp_chroma));
 cbxSetCurSel(IDC_CBX_ME_SUBCMP,cfgGet(IDFF_enc_me_subcmp));setCheck(IDC_CHB_ME_SUBCMP_CHROMA,cfgGet(IDFF_enc_me_subcmp_chroma));
 cbxSetCurSel(IDC_CBX_MB_CMP,cfgGet(IDFF_enc_mb_cmp));setCheck(IDC_CHB_MB_CMP_CHROMA,cfgGet(IDFF_enc_mb_cmp_chroma));
 cbxSetCurSel(IDC_CBX_DIA_SIZE,cfgGet(IDFF_enc_dia_size));
 static const int idDiaSize[]={IDC_LBL_DIA_SIZE,IDC_CBX_DIA_SIZE,0};enable(codecId!=CODEC_ID_SNOW,idDiaSize);
 cbxSetCurSel(IDC_CBX_MB_DECISION,cfgGet(IDFF_enc_me_hq));
 static const int idMbDesision[]={IDC_LBL_MB_DECISION,IDC_CBX_MB_DECISION,0};enable(codecId!=CODEC_ID_SNOW && codecId!=CODEC_ID_H261,idMbDesision);

 setCheck(IDC_CHB_4MV,cfgGet(IDFF_enc_me_4mv));enable(sup_4mv(codecId),IDC_CHB_4MV);
 setCheck(IDC_CHB_MV0,cfgGet(IDFF_enc_me_mv0));enable(sup_me_mv0(codecId) && cfgGet(IDFF_enc_me_hq)>0,IDC_CHB_MV0);
 setCheck(IDC_CHB_CBP_RD,cfgGet(IDFF_enc_me_cbp_rd));enable(sup_trellisQuant(codecId) && cfgGet(IDFF_enc_trellisquant),IDC_CHB_CBP_RD);

 prepass2dlg();
 qpel2dlg();

 SetDlgItemInt(m_hwnd,IDC_ED_ME_LAST_PRED_COUNT,cfgGet(IDFF_enc_me_last_predictor_count),FALSE);
 static const int idLastPredCount[]={IDC_LBL_ME_LAST_PRED_COUNT,IDC_ED_ME_LAST_PRED_COUNT,0};enable(codecId!=CODEC_ID_SNOW,idLastPredCount);
 cbxSetCurSel(IDC_CBX_ME_PREPASS,cfgGet(IDFF_enc_me_prepass));
 static const int idNotGray[]={IDC_CHB_ME_CMP_CHROMA,IDC_CHB_ME_SUBCMP_CHROMA,IDC_CHB_MB_CMP_CHROMA,0};
 enable(!sup_gray(codecId) || !cfgGet(IDFF_enc_gray),idNotGray);
 setCheck(IDC_CHB_ME_ITERATIVE,cfgGet(IDFF_enc_me_iterative));enable(codecId==CODEC_ID_SNOW,IDC_CHB_ME_ITERATIVE);
}
void TmePage::qpel2dlg(void)
{
 setCheck(IDC_CHB_QPEL,cfgGet(IDFF_enc_me_qpel));
 int is=sup_qpel(codecId);
 enable(is,IDC_CHB_QPEL);
 static const int idQpel[]={IDC_LBL_ME_SUBQ,IDC_ED_ME_SUBQ,0};
 SetDlgItemInt(m_hwnd,IDC_ED_ME_SUBQ,cfgGet(IDFF_enc_me_subq),FALSE);
 enable(is && cfgGet(IDFF_enc_me_qpel),idQpel);
}
void TmePage::prepass2dlg(void)
{
 static const int idPrepass0[]={IDC_LBL_ME_PREPASS,IDC_CBX_ME_PREPASS,0};
 enable(codecId!=CODEC_ID_SNOW,idPrepass0);
 cbxSetCurSel(IDC_CBX_ME_PRECMP,cfgGet(IDFF_enc_me_precmp));setCheck(IDC_CHB_ME_PRECMP_CHROMA,cfgGet(IDFF_enc_me_precmp_chroma));
 cbxSetCurSel(IDC_CBX_DIA_SIZE_PRE,cfgGet(IDFF_enc_dia_size_pre));
 static const int idPrepass[]={IDC_LBL_ME_PRECMP,IDC_CBX_ME_PRECMP,IDC_LBL_DIA_SIZE_PRE,IDC_CBX_DIA_SIZE_PRE,IDC_CHB_ME_PRECMP_CHROMA,0};
 int is=cfgGet(IDFF_enc_me_prepass) && codecId!=CODEC_ID_SNOW;
 enable(is,idPrepass);
 enable(is && (!sup_gray(codecId) || !cfgGet(IDFF_enc_gray)),IDC_CHB_ME_PRECMP_CHROMA);
}

void TmePage::translate(void)
{
 TconfPageEnc::translate();

 cbxTranslate(IDC_CBX_MB_DECISION,mb_decisions);
 cbxTranslate(IDC_CBX_ME_PREPASS,me_prepasses);

 int sel1=cbxGetCurSel(IDC_CBX_DIA_SIZE),sel2=cbxGetCurSel(IDC_CBX_DIA_SIZE_PRE);
 cbxClear(IDC_CBX_DIA_SIZE);cbxClear(IDC_CBX_DIA_SIZE_PRE);
 for (int i=0;Tlibavcodec::dia_sizes[i].descr;i++)
  {
   const char_t *descr=_(IDC_CBX_DIA_SIZE,Tlibavcodec::dia_sizes[i].descr);
   cbxAdd(IDC_CBX_DIA_SIZE,descr);
   cbxAdd(IDC_CBX_DIA_SIZE_PRE,descr);
  }
 cbxSetCurSel(IDC_CBX_DIA_SIZE,sel1);cbxSetCurSel(IDC_CBX_DIA_SIZE_PRE,sel2);
}

TmePage::TmePage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_ME;
 helpURL=_l("Motion+estimation.html");
 static const int props[]={IDFF_enc_me_hq,IDFF_enc_me_4mv,IDFF_enc_me_qpel,IDFF_enc_me_cmp,IDFF_enc_me_cmp_chroma,IDFF_enc_me_subcmp,IDFF_enc_me_subcmp_chroma,IDFF_enc_mb_cmp,IDFF_enc_mb_cmp_chroma,IDFF_enc_dia_size,IDFF_enc_me_last_predictor_count,IDFF_enc_me_prepass,IDFF_enc_me_precmp,IDFF_enc_me_precmp_chroma,IDFF_enc_dia_size_pre,IDFF_enc_me_subq,IDFF_enc_me_mv0,0};
 propsIDs=props;
 static const TbindCheckbox<TmePage> chb[]=
  {
   IDC_CHB_QPEL,IDFF_enc_me_qpel,&TmePage::qpel2dlg,
   IDC_CHB_4MV,IDFF_enc_me_4mv,NULL,
   IDC_CHB_MV0,IDFF_enc_me_mv0,NULL,
   IDC_CHB_CBP_RD,IDFF_enc_me_cbp_rd,NULL,
   IDC_CHB_ME_PRECMP_CHROMA,IDFF_enc_me_precmp_chroma,NULL,
   IDC_CHB_MB_CMP_CHROMA,IDFF_enc_mb_cmp_chroma,NULL,
   IDC_CHB_ME_SUBCMP_CHROMA,IDFF_enc_me_subcmp_chroma,NULL,
   IDC_CHB_ME_CMP_CHROMA,IDFF_enc_me_cmp_chroma,NULL,
   IDC_CHB_ME_ITERATIVE,IDFF_enc_me_iterative,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditInt<TmePage> edInt[]=
  {
   IDC_ED_ME_LAST_PRED_COUNT,0,99,IDFF_enc_me_last_predictor_count,NULL,
   IDC_ED_ME_SUBQ,1,8,IDFF_enc_me_subq,NULL,
   0
  };
 bindEditInts(edInt);
 static const TbindCombobox<TmePage> cbx[]=
  {
   IDC_CBX_ME_CMP,IDFF_enc_me_cmp,BINDCBX_SEL,NULL,
   IDC_CBX_ME_SUBCMP,IDFF_enc_me_subcmp,BINDCBX_SEL,NULL,
   IDC_CBX_DIA_SIZE,IDFF_enc_dia_size,BINDCBX_SEL,NULL,
   IDC_CBX_MB_CMP,IDFF_enc_mb_cmp,BINDCBX_SEL,NULL,
   IDC_CBX_ME_PRECMP,IDFF_enc_me_precmp,BINDCBX_SEL,NULL,
   IDC_CBX_ME_PREPASS,IDFF_enc_me_prepass,BINDCBX_SEL,&TmePage::prepass2dlg,
   IDC_CBX_DIA_SIZE_PRE,IDFF_enc_dia_size_pre,BINDCBX_SEL,NULL,
   IDC_CBX_MB_DECISION,IDFF_enc_me_hq,BINDCBX_SEL,&TmePage::cfg2dlg,
   0
  };
 bindComboboxes(cbx);
}
