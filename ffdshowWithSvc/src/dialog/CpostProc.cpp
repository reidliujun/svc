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
#include "CpostProc.h"
#include "Tlibmplayer.h"
#include "Tconfig.h"

const int TpostProcPage::idMplayer[]={IDC_CHB_POSTPROC_MPLAYER,IDC_LBL_PP_LEVELFIX,IDC_CHB_LEVELFIX_LUM,IDC_CHB_FULLYRANGE,IDC_CHB_POSTPROC_MPLAYER_ACCURATE,0};
const int TpostProcPage::idNics[]={IDC_CHB_POSTPROC_NIC,IDC_LBL_POSTPROC_NIC_XTHRES,IDC_TBR_POSTPROC_NIC_XTHRES,IDC_LBL_POSTPROC_NIC_YTHRES,IDC_TBR_POSTPROC_NIC_YTHRES,0};
const int TpostProcPage::idSPP[]={IDC_CHB_POSTPROC_SPP,IDC_CHB_POSTPROC_SPP_SOFT,0};

void TpostProcPage::init(void)
{
 tbrSetRange(IDC_TBR_PPQUAL,0,6,1,false);
 tbrSetRange(IDC_TBR_DEBLOCKSTRENGTH,0,512,32);
 tbrSetRange(IDC_TBR_POSTPROC_NIC_XTHRES,0,255,16);
 tbrSetRange(IDC_TBR_POSTPROC_NIC_YTHRES,0,255,16);
}

void TpostProcPage::cfg2dlg(void)
{
 postProc2dlg();
}

void TpostProcPage::postProc2dlg(void)
{
 tbrSet(IDC_TBR_PPQUAL,cfgGet(IDFF_ppqual));
 setCheck(IDC_CHB_AUTOQ,cfgGet(IDFF_autoq));

 int method=deci->getParam2(IDFF_postprocMethod);
 int isMplayer=0,isNic=0,isSPP=0,isFSPP=0;
 switch (method)
  {
   case 0:isMplayer=1;break;
   case 1:isNic=1;break;
   case 2:isMplayer=isNic=1;break;
   case 4:isSPP=1;break;
   case 5:isFSPP=1;break;
  }
 setCheck(IDC_CHB_POSTPROC_MPLAYER  ,isMplayer==1);
 setCheck(IDC_CHB_POSTPROC_NIC      ,isNic==1);
 setCheck(IDC_CHB_POSTPROC_NIC_FIRST,cfgGet(IDFF_postprocMethodNicFirst));
 setCheck(IDC_CHB_POSTPROC_SPP      ,isSPP==1);enable(isSPP,IDC_CHB_POSTPROC_SPP_SOFT);setCheck(IDC_CHB_POSTPROC_SPP_SOFT,cfgGet(IDFF_postprocSPPmode)==1);
 setCheck(IDC_CHB_POSTPROC_FSPP     ,isFSPP==1);

 enable(isMplayer && isNic,IDC_CHB_POSTPROC_NIC_FIRST);
 strength2dlg();
 mplayer2dlg(isMplayer);
 nic2dlg(isNic);
 setPPchbs();
 setCheck(IDC_RBT_PPPRESETS,!cfgGet(IDFF_ppIsCustom));
 setCheck(IDC_RBT_PPCUSTOM , cfgGet(IDFF_ppIsCustom));
}
void TpostProcPage::strength2dlg(void)
{
 int strength=cfgGet(IDFF_deblockStrength);
 tbrSet(IDC_TBR_DEBLOCKSTRENGTH,strength);
 setText(IDC_LBL_DEBLOCKSTRENGTH,_l("%s %i%%"),_(IDC_LBL_DEBLOCKSTRENGTH),100*strength/256);
}
void TpostProcPage::mplayer2dlg(int is)
{
 if (is!=-1) enable(is,idMplayer+1);

 bool levelfixlum=!!cfgGet(IDFF_levelFixLum);
 setCheck(IDC_CHB_LEVELFIX_LUM,levelfixlum);
 if (is!=-1)
  enable(is && levelfixlum,IDC_CHB_FULLYRANGE);
 else
  enable(levelfixlum,IDC_CHB_FULLYRANGE);
 setCheck(IDC_CHB_FULLYRANGE,cfgGet(IDFF_fullYrange));

 setCheck(IDC_CHB_POSTPROC_MPLAYER_ACCURATE,cfgGet(IDFF_deblockMplayerAccurate));
}
void TpostProcPage::mplayer2dlg_1(void)
{
 mplayer2dlg(-1);
}

void TpostProcPage::nic2dlg(int is)
{
 if (is!=-1) enable(is,idNics+1);

 tbrSet(IDC_TBR_POSTPROC_NIC_XTHRES,cfgGet(IDFF_postprocNicXthresh),IDC_LBL_POSTPROC_NIC_XTHRES);
 tbrSet(IDC_TBR_POSTPROC_NIC_YTHRES,cfgGet(IDFF_postprocNicYthresh),IDC_LBL_POSTPROC_NIC_YTHRES);
}
void TpostProcPage::nic2dlg_1(void)
{
 nic2dlg(-1);
}
void TpostProcPage::setPPchbs(void)
{
 unsigned int ppmode;
 static const int idPPcustom[]={IDC_CHB_DEBLOCKV_LUM,IDC_CHB_DEBLOCKH_LUM,IDC_CHB_DEBLOCKV_CHROM,IDC_CHB_DEBLOCKH_CHROM,IDC_CHB_DERING_LUM,IDC_CHB_DERING_CHROM,0};
 static const int idPPpresets[]={IDC_TBR_PPQUAL,IDC_CHB_AUTOQ,0};
 int method=cfgGet(IDFF_postprocMethod);
 int isSPP=method==4 || method==5;
 enable(!isSPP,IDC_RBT_PPCUSTOM);
 if (isSPP) cfgSet(IDFF_ppIsCustom,0);
 if (!isSPP && cfgGet(IDFF_ppIsCustom))
  {
   enable(true,idPPcustom);
   enable(false,idPPpresets);
   ppmode=cfgGet(IDFF_ppcustom);
  }
 else
  {
   enable(false,idPPcustom);
   enable(method!=5,idPPpresets);
   enable(method!=5,IDC_RBT_PPPRESETS);
   deciV->getPPmode(&ppmode);
  }
 setCheck(IDC_CHB_DEBLOCKV_LUM  ,ppmode&(LUM_V_DEBLOCK|V_A_DEBLOCK));
 setCheck(IDC_CHB_DEBLOCKH_LUM  ,ppmode&(LUM_H_DEBLOCK|H_A_DEBLOCK));
 setCheck(IDC_CHB_DEBLOCKV_CHROM,ppmode&(CHROM_V_DEBLOCK|(V_A_DEBLOCK<<4)));
 setCheck(IDC_CHB_DEBLOCKH_CHROM,ppmode&(CHROM_H_DEBLOCK|(H_A_DEBLOCK<<4)));
 setCheck(IDC_CHB_DERING_LUM    ,ppmode&LUM_DERING);
 setCheck(IDC_CHB_DERING_CHROM  ,ppmode&CHROM_DERING);
}

void TpostProcPage::onAuto(void)
{
 cfgSet(IDFF_currentq,cfgGet(IDFF_ppqual));
 postProc2dlg();
}

INT_PTR TpostProcPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_PPQUAL:
       {
        int ppqual=tbrGet(IDC_TBR_PPQUAL);
        cfgSet(IDFF_ppqual,ppqual);
        cfgSet(IDFF_currentq,ppqual);
        setPPchbs();
        return TRUE;
       }
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_DEBLOCKV_LUM:
      case IDC_CHB_DEBLOCKH_LUM:
      case IDC_CHB_DEBLOCKV_CHROM:
      case IDC_CHB_DEBLOCKH_CHROM:
      case IDC_CHB_DERING_LUM:
      case IDC_CHB_DERING_CHROM:
       {
        int ppcustom=0;
        ppcustom|=getCheck(IDC_CHB_DEBLOCKV_LUM  )?LUM_V_DEBLOCK:0;
        ppcustom|=getCheck(IDC_CHB_DEBLOCKH_LUM  )?LUM_H_DEBLOCK:0;
        ppcustom|=getCheck(IDC_CHB_DEBLOCKV_CHROM)?CHROM_V_DEBLOCK:0;
        ppcustom|=getCheck(IDC_CHB_DEBLOCKH_CHROM)?CHROM_H_DEBLOCK:0;
        ppcustom|=getCheck(IDC_CHB_DERING_LUM    )?LUM_DERING:0;
        ppcustom|=getCheck(IDC_CHB_DERING_CHROM  )?CHROM_DERING:0;
        cfgSet(IDFF_ppcustom,ppcustom);
        setPPchbs();
        return TRUE;
       }
      case IDC_CHB_POSTPROC_MPLAYER:
      case IDC_CHB_POSTPROC_NIC:
       setCheck(IDC_CHB_POSTPROC_SPP,0);
       setCheck(IDC_CHB_POSTPROC_FSPP,0);
      case IDC_CHB_POSTPROC_SPP:
       setCheck(IDC_CHB_POSTPROC_FSPP,0);
      case IDC_CHB_POSTPROC_FSPP:
       {
        int isMplayer=getCheck(IDC_CHB_POSTPROC_MPLAYER),isNic=getCheck(IDC_CHB_POSTPROC_NIC),isSPP=getCheck(IDC_CHB_POSTPROC_SPP),isFSPP=getCheck(IDC_CHB_POSTPROC_FSPP);
        if      ( isFSPP)                        cfgSet(IDFF_postprocMethod,5);
        else if ( isSPP )                        cfgSet(IDFF_postprocMethod,4);
        else if (!isMplayer && !isNic && !isSPP) cfgSet(IDFF_postprocMethod,3);
        else if ( isMplayer && !isNic && !isSPP) cfgSet(IDFF_postprocMethod,0);
        else if (!isMplayer &&  isNic && !isSPP) cfgSet(IDFF_postprocMethod,1);
        else if ( isMplayer &&  isNic && !isSPP) cfgSet(IDFF_postprocMethod,2);
        cfg2dlg();
        return TRUE;
       }
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}
void TpostProcPage::onFrame(void)
{
 SendDlgItemMessage(m_hwnd,IDC_TBR_PPQUAL,TBM_SETSELEND,TRUE,cfgGet(IDFF_currentq));
 setPPchbs();
}

void TpostProcPage::translate(void)
{
 TconfPageDecVideo::translate();
}

TpostProcPage::TpostProcPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_POSTPROC;
 static const TbindCheckbox<TpostProcPage> chb[]=
  {
   IDC_CHB_LEVELFIX_LUM,IDFF_levelFixLum,&TpostProcPage::mplayer2dlg_1,
   //IDC_CHB_LEVELFIX_CHROM,IDFF_levelFixChrom,&TpostProcPage::mplayer2dlg_1,
   IDC_CHB_FULLYRANGE,IDFF_fullYrange,NULL,
   IDC_CHB_POSTPROC_MPLAYER_ACCURATE,IDFF_deblockMplayerAccurate,NULL,
   IDC_CHB_POSTPROC_NIC_FIRST,IDFF_postprocMethodNicFirst,NULL,
   IDC_CHB_POSTPROC_SPP_SOFT,IDFF_postprocSPPmode,NULL,
   IDC_CHB_AUTOQ,IDFF_autoq,&TpostProcPage::onAuto,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TpostProcPage> htbr[]=
  {
   IDC_TBR_DEBLOCKSTRENGTH,IDFF_deblockStrength,&TpostProcPage::strength2dlg,
   IDC_TBR_POSTPROC_NIC_XTHRES,IDFF_postprocNicXthresh,&TpostProcPage::nic2dlg_1,
   IDC_TBR_POSTPROC_NIC_YTHRES,IDFF_postprocNicYthresh,&TpostProcPage::nic2dlg_1,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindRadiobutton<TpostProcPage> rbt[]=
  {
   IDC_RBT_PPPRESETS,IDFF_ppIsCustom,0,&TpostProcPage::postProc2dlg,
   IDC_RBT_PPCUSTOM,IDFF_ppIsCustom,1,&TpostProcPage::postProc2dlg,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindCombobox<TpostProcPage> cbx[]=
  {
   0
  };
 bindComboboxes(cbx);
}
