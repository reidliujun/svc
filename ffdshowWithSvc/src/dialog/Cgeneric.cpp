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
#include "Cgeneric.h"
#include "libavcodec/avcodec.h"

void TgenericPage::init(void)
{
 tbrSetRange(IDC_TBR_MAXKEYINTERVAL,1,500,10);
 tbrSetRange(IDC_TBR_MINKEYINTERVAL,1,500,10);

 hlv=GetDlgItem(m_hwnd,IDC_LV_GENERIC);
 ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_CHECKBOXES,LVS_EX_CHECKBOXES);
 int ncol=0;
 RECT r;
 GetWindowRect(hlv,&r);
 ListView_AddCol(hlv,ncol,r.right-r.left-36,_l("Option"),false);
 SendMessage(hlv,LVM_SETBKCOLOR,0,GetSysColor(COLOR_BTNFACE));
 SendMessage(hlv,LVM_SETTEXTBKCOLOR,0,GetSysColor(COLOR_BTNFACE));
}

void TgenericPage::cfg2dlg(void)
{
 flags.clear();
 if (sup_globalheader(codecId)) flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Store global headers in extradata")),IDFF_enc_globalHeader,1,false));
 if (sup_gray(codecId)) flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Greyscale")),IDFF_enc_gray,1,false));
 if (sup_interlace(codecId))
  {
   flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Interlaced encoding")),IDFF_enc_interlacing,1,false));
   if (codecId!=CODEC_ID_SKAL)
    flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Top field first (for intelaced encoding only)")),IDFF_enc_interlacing_tff,1,false));
  }
 if (sup_part(codecId)) flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Data partitioning")),IDFF_enc_part,1,false));
 switch (codecId)
  {
   case CODEC_ID_H263P:
    //flags.push_back(Tflag("H263+ : H263 Advanced intra coding / MPEG4 AC prediction",IDFF_enc_H263Pflags,CODEC_FLAG_AC_PRED));
    flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Unlimited motion vector")),IDFF_enc_H263Pflags,CODEC_FLAG_H263P_UMV,false));
    flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Alternative inter vlc")),IDFF_enc_H263Pflags,CODEC_FLAG_H263P_AIV,false));
    flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Sliced structure")),IDFF_enc_H263Pflags,CODEC_FLAG_H263P_SLICE_STRUCT,false));
    break;
   case CODEC_ID_MPEG2VIDEO:
    flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Reserve space for SVCD scan offset user data")),IDFF_enc_svcd_scan_offset,1,false));
    break;
   case CODEC_ID_X264:
   case CODEC_ID_X264_LOSSLESS:
    flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Cabac")),IDFF_enc_x264_cabac,1,false));
    flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Generate access unit delimiters")),IDFF_enc_x264_b_aud,1,false));
    flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Each MB partition can independently select a reference frame")),IDFF_enc_x264_mixed_ref,1,false));
    flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Transform coefficient thresholding on P-frames")),IDFF_enc_x264_b_dct_decimate,1,false));
    flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Interlaced")),IDFF_enc_x264_interlaced,1,false));
    break;
  }
if (codecId==CODEC_ID_X264)
  flags.push_back(std::make_tuple(_(IDC_LV_GENERIC,_l("Loop filter")),IDFF_enc_H263Pflags,CODEC_FLAG_LOOP_FILTER,true));

 if ((codecId==CODEC_ID_H263 || codecId==CODEC_ID_H263P) && cfgGet(IDFF_enc_me_hq)==FF_MB_DECISION_SIMPLE)
  flags.push_back(std::make_tuple(_l("OBMC"),IDFF_enc_H263Pflags,CODEC_FLAG_OBMC,false));

 nostate=true;
 int iig=lvGetSelItem(IDC_LV_GENERIC);
 ListView_DeleteAllItems(hlv);
 for (Tflags::const_iterator f=flags.begin();f!=flags.end();f++)
  {
   LVITEM lvi;memset(&lvi,0,sizeof(lvi));
   lvi.mask=LVIF_TEXT|LVIF_PARAM;
   lvi.pszText=LPTSTR(f->get<NAME>());
   lvi.lParam=LPARAM(&*f);
   lvi.iItem=100;
   int ii=ListView_InsertItem(hlv,&lvi);
   ListView_SetCheckState(hlv,ii,cfgGet(f->get<IDFF>())&f->get<VAL>());
  }
 lvSetSelItem(IDC_LV_GENERIC,iig);
 nostate=false;

 kf2dlg();
 b2dlg();

 static const int idThreads[]={IDC_LBL_NUMTHREADS,IDC_ED_NUMTHREADS,0};
 SetDlgItemInt(m_hwnd,IDC_ED_NUMTHREADS,cfgGet(IDFF_numthreads),FALSE);
 enable(sup_threads(codecId),idThreads);
}
void TgenericPage::kf2dlg(void)
{
 int x;
 x=cfgGet(IDFF_enc_max_key_interval);
 setText(IDC_LBL_MAXKEYINTERVAL,_l("%s %i"),_(IDC_LBL_MAXKEYINTERVAL),x);
 tbrSet(IDC_TBR_MAXKEYINTERVAL,x);
 x=cfgGet(IDFF_enc_min_key_interval);
 setText(IDC_LBL_MINKEYINTERVAL,_l("%s %i"),_(IDC_LBL_MINKEYINTERVAL),x);
 tbrSet(IDC_TBR_MINKEYINTERVAL,x);
 static const int idMaxKey[]={IDC_LBL_MAXKEYINTERVAL,IDC_TBR_MAXKEYINTERVAL,0};
 enable(sup_maxKeySet(codecId),idMaxKey);
 static const int idMinKey[]={IDC_LBL_MINKEYINTERVAL,IDC_TBR_MINKEYINTERVAL,0};
 enable(sup_minKeySet(codecId),idMinKey);
}
void TgenericPage::b2dlg(void)
{
 setCheck(IDC_CHB_B,cfgGet(IDFF_enc_isBframes));
 bool is=sup_bframes(codecId);
 enable(is,IDC_CHB_B);
 SetDlgItemInt(m_hwnd,IDC_ED_B_MAX,cfgGet(IDFF_enc_max_b_frames),FALSE);
 setCheck(IDC_CHB_PACKEDBITSTREAM,cfgGet(IDFF_enc_packedBitstream));
 setCheck(IDC_CHB_DX50BVOP,cfgGet(IDFF_enc_dx50bvop));
 setCheck(IDC_CHB_B_DYNAMIC,cfgGet(IDFF_enc_b_dynamic));
 setCheck(IDC_CHB_B_REFINE,cfgGet(IDFF_enc_b_refine));
 static const int idBframes[]={IDC_LBL_B_MAX,IDC_ED_B_MAX,IDC_CHB_B_DYNAMIC,IDC_CHB_B_REFINE,IDC_CHB_PACKEDBITSTREAM,IDC_CHB_DX50BVOP,IDC_CHB_B_PYRAMID,IDC_CHB_B_RDO,0};
 is&=!!cfgGet(IDFF_enc_isBframes);
 enable(is,idBframes);
 enable(is && sup_packedBitstream(codecId),IDC_CHB_PACKEDBITSTREAM);
 enable(is && sup_closedGop(codecId),IDC_CHB_DX50BVOP);
 enable(is && sup_adaptiveBframes(codecId),IDC_CHB_B_DYNAMIC);
 enable(is && lavc_codec(codecId),IDC_CHB_B_REFINE);
}

INT_PTR TgenericPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_MAXKEYINTERVAL:
       cfgSet(IDFF_enc_max_key_interval,std::max(cfgGet(IDFF_enc_min_key_interval),tbrGet(IDC_TBR_MAXKEYINTERVAL)));
       kf2dlg();
       return TRUE;
      case IDC_TBR_MINKEYINTERVAL:
       cfgSet(IDFF_enc_min_key_interval,std::min(cfgGet(IDFF_enc_max_key_interval),tbrGet(IDC_TBR_MINKEYINTERVAL)));
       kf2dlg();
       return TRUE;
     }
    break;
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (!nostate && nmhdr->hwndFrom==hlv && nmhdr->idFrom==IDC_LV_GENERIC)
      switch (nmhdr->code)
       {
        case LVN_ITEMCHANGED:
         {
          LPNMLISTVIEW nmlv=LPNMLISTVIEW(lParam);
          if (nmlv->uChanged&LVIF_STATE && ((nmlv->uOldState&4096)!=(nmlv->uNewState&4096)))
           {
            Tflag *f=(Tflag*)nmlv->lParam;
            if (nmlv->uNewState&8192)
             cfgSet(f->get<IDFF>(),cfgGet(f->get<IDFF>())|f->get<VAL>());
            else if (nmlv->uNewState&4096)
             cfgSet(f->get<IDFF>(),cfgGet(f->get<IDFF>())&~f->get<VAL>());
            if (f->get<REPAINT>())
             cfg2dlg();
           }
          return TRUE;
         }
        break;
       }
     break;
    }
  }
 return TconfPageEnc::msgProc(uMsg,wParam,lParam);
}

TgenericPage::TgenericPage(TffdshowPageEnc *Iparent):TconfPageEnc(Iparent)
{
 dialogId=IDD_GENERIC;
 static const int props[]={IDFF_enc_max_key_interval,IDFF_enc_min_key_interval,IDFF_enc_globalHeader,IDFF_enc_part,IDFF_enc_interlacing,IDFF_enc_interlacing_tff,IDFF_enc_gray,IDFF_enc_isBframes,IDFF_enc_max_b_frames,IDFF_enc_packedBitstream,IDFF_enc_dx50bvop,IDFF_enc_H263Pflags,IDFF_enc_b_dynamic,IDFF_enc_b_refine,IDFF_enc_svcd_scan_offset,IDFF_enc_x264_cabac,IDFF_enc_x264_interlaced,IDFF_numthreads,0};
 propsIDs=props;
 static const TbindCheckbox<TgenericPage> chb[]=
  {
   IDC_CHB_B,IDFF_enc_isBframes,&TgenericPage::b2dlg,
   IDC_CHB_PACKEDBITSTREAM,IDFF_enc_packedBitstream,NULL,
   IDC_CHB_DX50BVOP,IDFF_enc_dx50bvop,NULL,
   IDC_CHB_B_DYNAMIC,IDFF_enc_b_dynamic,NULL,
   IDC_CHB_B_REFINE,IDFF_enc_b_refine,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindEditInt<TgenericPage> edInt[]=
  {
   IDC_ED_B_MAX,1,8,IDFF_enc_max_b_frames,NULL,
   IDC_ED_NUMTHREADS,1,8,IDFF_numthreads,NULL,
   0
  };
 bindEditInts(edInt);
}

  void TgenericX264page::cfg2dlg(void)
  {
   TgenericPage::cfg2dlg();
   SetDlgItemInt(m_hwnd,IDC_ED_X264_DEBLOCK_ALPHA,cfgGet(IDFF_enc_x264_i_deblocking_filter_alphac0),TRUE);
   SetDlgItemInt(m_hwnd,IDC_ED_X264_DEBLOCK_BETA ,cfgGet(IDFF_enc_x264_i_deblocking_filter_beta   ),TRUE);
   static const int idDeblock[]={IDC_LBL_X264_DEBLOCK_ALPHA,IDC_ED_X264_DEBLOCK_ALPHA,IDC_LBL_X264_DEBLOCK_BETA,IDC_ED_X264_DEBLOCK_BETA,0};
   enable((cfgGet(IDFF_enc_H263Pflags)&CODEC_FLAG_LOOP_FILTER)!=0 && codecId!=CODEC_ID_X264_LOSSLESS,idDeblock);
  }
  void TgenericX264page::b2dlg(void)
  {
   setCheck(IDC_CHB_B_PYRAMID,cfgGet(IDFF_enc_x264_b_bframe_pyramid));
   TgenericPage::b2dlg();
  }
  TgenericX264page::TgenericX264page(TffdshowPageEnc *Iparent):TgenericPage(Iparent)
  {
   dialogId=IDD_GENERIC_X264;
   static const int props[]={IDFF_enc_max_key_interval,IDFF_enc_min_key_interval,IDFF_enc_isBframes,IDFF_enc_max_b_frames,IDFF_enc_H263Pflags,IDFF_enc_b_dynamic,IDFF_enc_x264_i_deblocking_filter_alphac0,IDFF_enc_x264_i_deblocking_filter_beta,IDFF_enc_b_dynamic,IDFF_enc_x264_b_bframe_pyramid,IDFF_enc_x264_b_aud,IDFF_numthreads,0};
   propsIDs=props;
   static const TbindCheckbox<TgenericX264page> chb[]=
    {
     IDC_CHB_B,IDFF_enc_isBframes,&TgenericX264page::b2dlg,
     IDC_CHB_B_DYNAMIC,IDFF_enc_b_dynamic,NULL,
     IDC_CHB_B_PYRAMID,IDFF_enc_x264_b_bframe_pyramid,NULL,
     0,NULL,NULL
    };
   bindCheckboxes(chb);
   static const TbindEditInt<TgenericX264page> edInt[]=
    {
     IDC_ED_B_MAX,1,8,IDFF_enc_max_b_frames,NULL,
     IDC_ED_NUMTHREADS,1,8,IDFF_numthreads,NULL,
     IDC_ED_X264_DEBLOCK_ALPHA,-6,6,IDFF_enc_x264_i_deblocking_filter_alphac0,NULL,
     IDC_ED_X264_DEBLOCK_BETA,-6,6,IDFF_enc_x264_i_deblocking_filter_beta,NULL,
     0
    };
   bindEditInts(edInt);
  }
