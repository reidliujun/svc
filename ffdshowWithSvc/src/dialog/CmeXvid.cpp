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
#include "CmeXvid.h"
#include "xvidcore/xvid.h"

const int TmeXvidPage::idCustomVHQs[]={IDC_CHB_XVIDVHQ_MODEDECISION_BITS,IDC_CHB_XVIDVHQ_EXTSEARCH_BITS,IDC_CHB_XVIDVHQ_CHECKPREDICTION_BITS,IDC_CHB_XVIDVHQ_HALFPELREFINE16_BITS,IDC_CHB_XVIDVHQ_HALFPELREFINE8_BITS,IDC_CHB_XVIDVHQ_QUARTERPELREFINE16_BITS,IDC_CHB_XVIDVHQ_QUARTERPELREFINE8_BITS,0};

bool TmeXvidPage::enabled(void)
{
 return xvid_codec(codecId);
}

void TmeXvidPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_QPEL,cfgGet(IDFF_enc_me_qpel));
 setCheck(IDC_CHB_GMC,cfgGet(IDFF_enc_me_gmc));
 setCheck(IDC_CHB_XVID_CHROMA_ME,cfgGet(IDFF_enc_me_cmp_chroma));enable(!cfgGet(IDFF_enc_gray),IDC_CHB_XVID_CHROMA_ME);

 setCheck(IDC_CHB_XVID_ME_PRESET,!cfgGet(IDFF_enc_is_xvid_me_custom));
 enable(!cfgGet(IDFF_enc_is_xvid_me_custom),IDC_CBX_ME_XVID);
 cbxSetCurSel(IDC_CBX_ME_XVID,cfgGet(IDFF_enc_xvid_motion_search));
 static const int idCustomMEs[]={IDC_CHB_XVIDME_ADVANCEDDIAMOND8,IDC_CHB_XVIDME_ADVANCEDDIAMOND16,IDC_CHB_XVIDME_HALFPELREFINE8,IDC_CHB_XVIDME_HALFPELREFINE16,IDC_CHB_XVIDME_EXTSEARCH16,IDC_CHB_XVIDME_USESQUARES16,IDC_CHB_XVIDME_EXTSEARCH8,IDC_CHB_XVIDME_USESQUARES8,IDC_CHB_XVIDME_INTER4V,0};
 enable(cfgGet(IDFF_enc_is_xvid_me_custom),idCustomMEs);
 me2dlg();

 setCheck(IDC_CHB_XVID_VHQ_PRESET,!cfgGet(IDFF_enc_is_xvid_vhq_custom));
 enable(!cfgGet(IDFF_enc_is_xvid_vhq_custom),IDC_CBX_VHQ_XVID);
 cbxSetCurSel(IDC_CBX_VHQ_XVID,cfgGet(IDFF_enc_xvid_vhq));
 enable(cfgGet(IDFF_enc_is_xvid_vhq_custom),idCustomVHQs);
 vhq2dlg();
}
void TmeXvidPage::me2dlg(void)
{
 int me=cfgGet(IDFF_enc_is_xvid_me_custom)?cfgGet(IDFF_enc_xvid_me_custom):meXviDpresets[cfgGet(IDFF_enc_xvid_motion_search)].preset;

 setCheck(IDC_CHB_XVIDME_ADVANCEDDIAMOND16,me&XVID_ME::ADVANCEDDIAMOND16);
 setCheck(IDC_CHB_XVIDME_HALFPELREFINE16  ,me&XVID_ME::HALFPELREFINE16  );
 setCheck(IDC_CHB_XVIDME_EXTSEARCH16      ,me&XVID_ME::EXTSEARCH16      );
 setCheck(IDC_CHB_XVIDME_USESQUARES16     ,me&XVID_ME::USESQUARES16     );

 setCheck(IDC_CHB_XVIDME_ADVANCEDDIAMOND8 ,me&XVID_ME::ADVANCEDDIAMOND8 );
 setCheck(IDC_CHB_XVIDME_HALFPELREFINE8   ,me&XVID_ME::HALFPELREFINE8   );
 setCheck(IDC_CHB_XVIDME_EXTSEARCH8       ,me&XVID_ME::EXTSEARCH8       );
 setCheck(IDC_CHB_XVIDME_USESQUARES8      ,me&(int)XVID_ME::USESQUARES8 );

 if (cfgGet(IDFF_enc_is_xvid_me_custom))
  setCheck(IDC_CHB_XVIDME_INTER4V,cfgGet(IDFF_enc_xvid_me_inter4v));
 else
  setCheck(IDC_CHB_XVIDME_INTER4V,cfgGet(IDFF_enc_xvid_motion_search)>4);
}
void TmeXvidPage::vhq2dlg(void)
{
 int vhq=cfgGet(IDFF_enc_is_xvid_vhq_custom)?cfgGet(IDFF_enc_xvid_vhq_custom):vhqXviDpresets[cfgGet(IDFF_enc_xvid_vhq)].preset;
 setCheck(IDC_CHB_XVIDVHQ_EXTSEARCH_BITS         ,vhq&XVID_ME_RD::EXTSEARCH         );
 setCheck(IDC_CHB_XVIDVHQ_CHECKPREDICTION_BITS   ,vhq&XVID_ME_RD::CHECKPREDICTION   );
 setCheck(IDC_CHB_XVIDVHQ_HALFPELREFINE16_BITS   ,vhq&XVID_ME_RD::HALFPELREFINE16   );
 setCheck(IDC_CHB_XVIDVHQ_HALFPELREFINE8_BITS    ,vhq&XVID_ME_RD::HALFPELREFINE8    );
 setCheck(IDC_CHB_XVIDVHQ_QUARTERPELREFINE16_BITS,vhq&XVID_ME_RD::QUARTERPELREFINE16);
 setCheck(IDC_CHB_XVIDVHQ_QUARTERPELREFINE8_BITS ,vhq&XVID_ME_RD::QUARTERPELREFINE8 );
 if (cfgGet(IDFF_enc_is_xvid_vhq_custom))
  {
   setCheck(IDC_CHB_XVIDVHQ_MODEDECISION_BITS,cfgGet(IDFF_enc_xvid_vhq_modedecisionbits));
   enable(cfgGet(IDFF_enc_xvid_vhq_modedecisionbits),idCustomVHQs+1);
  }
 else
  setCheck(IDC_CHB_XVIDVHQ_MODEDECISION_BITS,cfgGet(IDFF_enc_xvid_vhq)>0);
}

INT_PTR TmeXvidPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_XVIDME_ADVANCEDDIAMOND16:
      case IDC_CHB_XVIDME_HALFPELREFINE16  :
      case IDC_CHB_XVIDME_EXTSEARCH16      :
      case IDC_CHB_XVIDME_USESQUARES16     :
      case IDC_CHB_XVIDME_ADVANCEDDIAMOND8 :
      case IDC_CHB_XVIDME_HALFPELREFINE8   :
      case IDC_CHB_XVIDME_EXTSEARCH8       :
      case IDC_CHB_XVIDME_USESQUARES8      :
       {
        int me=0;
        if (getCheck(IDC_CHB_XVIDME_ADVANCEDDIAMOND16)) me|=XVID_ME::ADVANCEDDIAMOND16;
        if (getCheck(IDC_CHB_XVIDME_HALFPELREFINE16  )) me|=XVID_ME::HALFPELREFINE16  ;
        if (getCheck(IDC_CHB_XVIDME_EXTSEARCH16      )) me|=XVID_ME::EXTSEARCH16      ;
        if (getCheck(IDC_CHB_XVIDME_USESQUARES16     )) me|=XVID_ME::USESQUARES16     ;

        if (getCheck(IDC_CHB_XVIDME_ADVANCEDDIAMOND8 )) me|=XVID_ME::ADVANCEDDIAMOND8 ;
        if (getCheck(IDC_CHB_XVIDME_HALFPELREFINE8   )) me|=XVID_ME::HALFPELREFINE8   ;
        if (getCheck(IDC_CHB_XVIDME_EXTSEARCH8       )) me|=XVID_ME::EXTSEARCH8       ;
        if (getCheck(IDC_CHB_XVIDME_USESQUARES8      )) me|=(int)XVID_ME::USESQUARES8 ;
        cfgSet(IDFF_enc_xvid_me_custom,me);
        return TRUE;
       }
      case IDC_CHB_XVIDVHQ_EXTSEARCH_BITS:
      case IDC_CHB_XVIDVHQ_CHECKPREDICTION_BITS:
      case IDC_CHB_XVIDVHQ_HALFPELREFINE16_BITS:
      case IDC_CHB_XVIDVHQ_HALFPELREFINE8_BITS:
      case IDC_CHB_XVIDVHQ_QUARTERPELREFINE16_BITS:
      case IDC_CHB_XVIDVHQ_QUARTERPELREFINE8_BITS:
       {
        int vhq=0;
        if (getCheck(IDC_CHB_XVIDVHQ_EXTSEARCH_BITS         )) vhq|=XVID_ME_RD::EXTSEARCH         ;
        if (getCheck(IDC_CHB_XVIDVHQ_CHECKPREDICTION_BITS   )) vhq|=XVID_ME_RD::CHECKPREDICTION   ;
        if (getCheck(IDC_CHB_XVIDVHQ_HALFPELREFINE16_BITS   )) vhq|=XVID_ME_RD::HALFPELREFINE16   ;
        if (getCheck(IDC_CHB_XVIDVHQ_HALFPELREFINE8_BITS    )) vhq|=XVID_ME_RD::HALFPELREFINE8    ;
        if (getCheck(IDC_CHB_XVIDVHQ_QUARTERPELREFINE16_BITS)) vhq|=XVID_ME_RD::QUARTERPELREFINE16;
        if (getCheck(IDC_CHB_XVIDVHQ_QUARTERPELREFINE8_BITS )) vhq|=XVID_ME_RD::QUARTERPELREFINE8 ;
        cfgSet(IDFF_enc_xvid_vhq_custom,vhq);
        return TRUE;
       }
     }
    break;
  }
 return TconfPageEnc::msgProc(uMsg,wParam,lParam);
}

void TmeXvidPage::translate(void)
{
 TconfPageEnc::translate();

 int sel;

 sel=cbxGetCurSel(IDC_CBX_ME_XVID);
 cbxClear(IDC_CBX_ME_XVID);
 for (int i=0;meXviDpresets[i].name;i++)
  cbxAdd(IDC_CBX_ME_XVID,_(IDC_CBX_ME_XVID,meXviDpresets[i].name));
 cbxSetCurSel(IDC_CBX_ME_XVID,sel);

 sel=cbxGetCurSel(IDC_CBX_VHQ_XVID);
 cbxClear(IDC_CBX_VHQ_XVID);
 for (int i=0;vhqXviDpresets[i].name;i++)
  cbxAdd(IDC_CBX_VHQ_XVID,_(IDC_CBX_VHQ_XVID,vhqXviDpresets[i].name));
 cbxSetCurSel(IDC_CBX_VHQ_XVID,sel);
}

TmeXvidPage::TmeXvidPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_ME_XVID;
 static const int props[]={IDFF_enc_me_qpel,IDFF_enc_me_gmc,IDFF_enc_me_cmp_chroma,IDFF_enc_xvid_motion_search,IDFF_enc_is_xvid_me_custom,IDFF_enc_xvid_me_custom,IDFF_enc_xvid_me_inter4v,IDFF_enc_xvid_vhq,IDFF_enc_xvid_vhq_modedecisionbits,IDFF_enc_is_xvid_vhq_custom,IDFF_enc_xvid_vhq_custom,0};
 propsIDs=props;
 static const TbindCheckbox<TmeXvidPage> chb[]=
  {
   IDC_CHB_QPEL,IDFF_enc_me_qpel,NULL,
   IDC_CHB_GMC,IDFF_enc_me_gmc,NULL,
   IDC_CHB_XVID_CHROMA_ME,IDFF_enc_me_cmp_chroma,NULL,
   IDC_CHB_XVID_ME_PRESET,-IDFF_enc_is_xvid_me_custom,&TmeXvidPage::cfg2dlg,
   IDC_CHB_XVIDME_INTER4V,IDFF_enc_xvid_me_inter4v,NULL,
   IDC_CHB_XVID_VHQ_PRESET,-IDFF_enc_is_xvid_vhq_custom,&TmeXvidPage::cfg2dlg,
   IDC_CHB_XVIDVHQ_MODEDECISION_BITS,IDFF_enc_xvid_vhq_modedecisionbits,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindCombobox<TmeXvidPage> cbx[]=
  {
   IDC_CBX_ME_XVID,IDFF_enc_xvid_motion_search,BINDCBX_SEL,&TmeXvidPage::me2dlg,
   IDC_CBX_VHQ_XVID,IDFF_enc_xvid_vhq,BINDCBX_SEL,&TmeXvidPage::vhq2dlg,
   0
  };
 bindComboboxes(cbx);
}
