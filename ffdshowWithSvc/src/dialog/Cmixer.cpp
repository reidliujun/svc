/*
 * Copyright (c) 2003-2006 Milan Cutka
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
#include "Cmixer.h"
#include "TmixerSettings.h"
#include "TffdshowPageDec.h"
#include "IaudioFilterMixer.h"

const int TmixerPage::idffs[9*9]=
{
 IDFF_mixerMatrix00,IDFF_mixerMatrix01,IDFF_mixerMatrix02,IDFF_mixerMatrix03,IDFF_mixerMatrix04,IDFF_mixerMatrix05,IDFF_mixerMatrix08,IDFF_mixerMatrix06,IDFF_mixerMatrix07,
 IDFF_mixerMatrix10,IDFF_mixerMatrix11,IDFF_mixerMatrix12,IDFF_mixerMatrix13,IDFF_mixerMatrix14,IDFF_mixerMatrix15,IDFF_mixerMatrix18,IDFF_mixerMatrix16,IDFF_mixerMatrix17,
 IDFF_mixerMatrix20,IDFF_mixerMatrix21,IDFF_mixerMatrix22,IDFF_mixerMatrix23,IDFF_mixerMatrix24,IDFF_mixerMatrix25,IDFF_mixerMatrix28,IDFF_mixerMatrix26,IDFF_mixerMatrix27,
 IDFF_mixerMatrix30,IDFF_mixerMatrix31,IDFF_mixerMatrix32,IDFF_mixerMatrix33,IDFF_mixerMatrix34,IDFF_mixerMatrix35,IDFF_mixerMatrix38,IDFF_mixerMatrix36,IDFF_mixerMatrix37,
 IDFF_mixerMatrix40,IDFF_mixerMatrix41,IDFF_mixerMatrix42,IDFF_mixerMatrix43,IDFF_mixerMatrix44,IDFF_mixerMatrix45,IDFF_mixerMatrix48,IDFF_mixerMatrix46,IDFF_mixerMatrix47,
 IDFF_mixerMatrix50,IDFF_mixerMatrix51,IDFF_mixerMatrix52,IDFF_mixerMatrix53,IDFF_mixerMatrix54,IDFF_mixerMatrix55,IDFF_mixerMatrix58,IDFF_mixerMatrix56,IDFF_mixerMatrix57,
 IDFF_mixerMatrix80,IDFF_mixerMatrix81,IDFF_mixerMatrix82,IDFF_mixerMatrix83,IDFF_mixerMatrix84,IDFF_mixerMatrix85,IDFF_mixerMatrix88,IDFF_mixerMatrix86,IDFF_mixerMatrix87,
 IDFF_mixerMatrix60,IDFF_mixerMatrix61,IDFF_mixerMatrix62,IDFF_mixerMatrix63,IDFF_mixerMatrix64,IDFF_mixerMatrix65,IDFF_mixerMatrix68,IDFF_mixerMatrix66,IDFF_mixerMatrix67,
 IDFF_mixerMatrix70,IDFF_mixerMatrix71,IDFF_mixerMatrix72,IDFF_mixerMatrix73,IDFF_mixerMatrix74,IDFF_mixerMatrix75,IDFF_mixerMatrix78,IDFF_mixerMatrix76,IDFF_mixerMatrix77
};
const int TmixerPage::idcs[9*9+1]=
{
 IDC_ED_MIXERMATRIX00,IDC_ED_MIXERMATRIX20,IDC_ED_MIXERMATRIX10,IDC_ED_MIXERMATRIX50,IDC_ED_MIXERMATRIX30,IDC_ED_MIXERMATRIX40,IDC_ED_MIXERMATRIX80,IDC_ED_MIXERMATRIX60,IDC_ED_MIXERMATRIX70,
 IDC_ED_MIXERMATRIX02,IDC_ED_MIXERMATRIX22,IDC_ED_MIXERMATRIX12,IDC_ED_MIXERMATRIX52,IDC_ED_MIXERMATRIX32,IDC_ED_MIXERMATRIX42,IDC_ED_MIXERMATRIX82,IDC_ED_MIXERMATRIX62,IDC_ED_MIXERMATRIX72,
 IDC_ED_MIXERMATRIX01,IDC_ED_MIXERMATRIX21,IDC_ED_MIXERMATRIX11,IDC_ED_MIXERMATRIX51,IDC_ED_MIXERMATRIX31,IDC_ED_MIXERMATRIX41,IDC_ED_MIXERMATRIX81,IDC_ED_MIXERMATRIX61,IDC_ED_MIXERMATRIX71,
 IDC_ED_MIXERMATRIX05,IDC_ED_MIXERMATRIX25,IDC_ED_MIXERMATRIX15,IDC_ED_MIXERMATRIX55,IDC_ED_MIXERMATRIX35,IDC_ED_MIXERMATRIX45,IDC_ED_MIXERMATRIX85,IDC_ED_MIXERMATRIX65,IDC_ED_MIXERMATRIX75,
 IDC_ED_MIXERMATRIX03,IDC_ED_MIXERMATRIX23,IDC_ED_MIXERMATRIX13,IDC_ED_MIXERMATRIX53,IDC_ED_MIXERMATRIX33,IDC_ED_MIXERMATRIX43,IDC_ED_MIXERMATRIX83,IDC_ED_MIXERMATRIX63,IDC_ED_MIXERMATRIX73,
 IDC_ED_MIXERMATRIX04,IDC_ED_MIXERMATRIX24,IDC_ED_MIXERMATRIX14,IDC_ED_MIXERMATRIX54,IDC_ED_MIXERMATRIX34,IDC_ED_MIXERMATRIX44,IDC_ED_MIXERMATRIX84,IDC_ED_MIXERMATRIX64,IDC_ED_MIXERMATRIX74,
 IDC_ED_MIXERMATRIX08,IDC_ED_MIXERMATRIX28,IDC_ED_MIXERMATRIX18,IDC_ED_MIXERMATRIX58,IDC_ED_MIXERMATRIX38,IDC_ED_MIXERMATRIX48,IDC_ED_MIXERMATRIX88,IDC_ED_MIXERMATRIX68,IDC_ED_MIXERMATRIX78,
 IDC_ED_MIXERMATRIX06,IDC_ED_MIXERMATRIX26,IDC_ED_MIXERMATRIX16,IDC_ED_MIXERMATRIX56,IDC_ED_MIXERMATRIX36,IDC_ED_MIXERMATRIX46,IDC_ED_MIXERMATRIX86,IDC_ED_MIXERMATRIX66,IDC_ED_MIXERMATRIX76,
 IDC_ED_MIXERMATRIX07,IDC_ED_MIXERMATRIX27,IDC_ED_MIXERMATRIX17,IDC_ED_MIXERMATRIX57,IDC_ED_MIXERMATRIX37,IDC_ED_MIXERMATRIX47,IDC_ED_MIXERMATRIX87,IDC_ED_MIXERMATRIX67,IDC_ED_MIXERMATRIX77,
 0
};

void TmixerPage::init(void)
{
 onNewFilter();
 for (int i=0;i<9*9;i++)
  ((TwidgetMixer*)Twidget::getDlgItem(GetDlgItem(m_hwnd,idcs[i])))->idff=idffs[i];
 tbrSetRange(IDC_TBR_HEADPHONE_DIM,0,100);
 tbrSetRange(IDC_TBR_MIXER_CLEV  ,0,300);
 tbrSetRange(IDC_TBR_MIXER_SLEV  ,0,300);
 tbrSetRange(IDC_TBR_MIXER_LFELEV,0,300);
 cbxSetDroppedWidth(IDC_CBX_MIXER_OUT,200);
}

void TmixerPage::cfg2dlg(void)
{
 int out=cfgGet(IDFF_mixerOut);
 int nameIndex=TmixerSettings::chConfigs[out].nameIndex;
 if (nameIndex>=0)
  {
   int lfe=TmixerSettings::chConfigs[out].channelmask&SPEAKER_LOW_FREQUENCY;
   cbxSetDataCurSel(IDC_CBX_MIXER_OUT,lfe?nameIndex:out);
   setCheck(IDC_CHB_MIXER_LFE,lfe);
   enable(true,IDC_CHB_MIXER_LFE);
  }
 else
  {
   cbxSetDataCurSel(IDC_CBX_MIXER_OUT,out);
   enable(false,IDC_CHB_MIXER_LFE,false);
  }
 static const int mixer[]={IDC_CHB_MIXER_CUSTOM,IDC_CHB_MIXER_NORMALIZE_MATRIX,IDC_CHB_MIXER_VOICE_CONTROL,IDC_CHB_MIXER_EXPAND_STEREO,IDC_LBL_MIXERMATRIX_L,IDC_LBL_MIXERMATRIX_R,IDC_LBL_MIXERMATRIX_C,IDC_LBL_MIXERMATRIX_SL,IDC_LBL_MIXERMATRIX_SR,IDC_LBL_MIXERMATRIX_LFE,IDC_LBL_MIXERMATRIX_AL,IDC_LBL_MIXERMATRIX_AR,IDC_LBL_MIXERMATRIX_BC,IDC_LBL_MIXERMATRIX_L1,IDC_LBL_MIXERMATRIX_R1,IDC_LBL_MIXERMATRIX_C1,IDC_LBL_MIXERMATRIX_SL1,IDC_LBL_MIXERMATRIX_SR1,IDC_LBL_MIXERMATRIX_LFE1,IDC_LBL_MIXERMATRIX_AL1,IDC_LBL_MIXERMATRIX_AR1,IDC_LBL_MIXERMATRIX_BC1,IDC_LBL_MIXER_CLEV,IDC_TBR_MIXER_CLEV,IDC_LBL_MIXER_SLEV,IDC_TBR_MIXER_SLEV,IDC_LBL_MIXER_LFELEV,IDC_TBR_MIXER_LFELEV,IDC_LBL_MIXER_CLEV2,IDC_LBL_MIXER_SLEV2,IDC_LBL_MIXER_LFELEV2,0};
 static const int headphone[]={IDC_LBL_HEADPHONE_DIM,IDC_TBR_HEADPHONE_DIM,0};
 bool head=out==17 || out==18;
 enable(out==17,headphone);
 enable(!head,mixer);enable(!head,idcs);
 if (head)
  {
   if (out==17)
    tbrSet(IDC_TBR_HEADPHONE_DIM,cfgGet(IDFF_headphone_dim));
  }
 else
  {
   setCheck(IDC_CHB_MIXER_NORMALIZE_MATRIX,cfgGet(IDFF_normalizeMatrix));
   matrix2dlg();
  }
}
void TmixerPage::matrix2dlg(void)
{
 int custom=cfgGet(IDFF_customMatrix);
 setCheck(IDC_CHB_MIXER_CUSTOM,custom);
 enable(custom,idcs);
 static const int voexp[]={IDC_CHB_MIXER_VOICE_CONTROL,IDC_CHB_MIXER_EXPAND_STEREO,IDC_LBL_MIXER_CLEV,IDC_TBR_MIXER_CLEV,IDC_LBL_MIXER_SLEV,IDC_TBR_MIXER_SLEV,IDC_LBL_MIXER_LFELEV,IDC_TBR_MIXER_LFELEV,IDC_LBL_MIXER_CLEV2,IDC_LBL_MIXER_SLEV2,IDC_LBL_MIXER_LFELEV2,0};
 enable(!custom,voexp);
 if (custom)
  for (int i=0;i<9*9;i++)
   setText(idcs[i],_l("%.3g"),float(cfgGet(idffs[i])/100000.0f));
 clev2dlg();slev2dlg(),lfe2dlg();
}
void TmixerPage::clev2dlg(void)
{
 int x=cfgGet(IDFF_mixerClev);
 tbrSet(IDC_TBR_MIXER_CLEV,x);
 setText(IDC_LBL_MIXER_CLEV2,_l("%i%%"),x);
 int is=cfgGet(IDFF_mixerVoiceControl) && !cfgGet(IDFF_customMatrix);
 //static const int clevs[]={IDC_LBL_MIXER_CLEV,IDC_TBR_MIXER_CLEV,IDC_LBL_MIXER_CLEV2,0};
 //enable(is,clevs);
 setCheck(IDC_CHB_MIXER_VOICE_CONTROL,is);
}
void TmixerPage::slev2dlg(void)
{
 int x=cfgGet(IDFF_mixerSlev);
 tbrSet(IDC_TBR_MIXER_SLEV,x);
 setText(IDC_LBL_MIXER_SLEV2,_l("%i%%"),x);
 int is=cfgGet(IDFF_mixerExpandStereo) && !cfgGet(IDFF_customMatrix);
 //static const int slevs[]={IDC_LBL_MIXER_SLEV,IDC_TBR_MIXER_SLEV,IDC_LBL_MIXER_SLEV2,0};
 //enable(is,slevs);
 setCheck(IDC_CHB_MIXER_EXPAND_STEREO,is);
}
void TmixerPage::lfe2dlg(void)
{
 int x=cfgGet(IDFF_mixerLFElev);
 tbrSet(IDC_TBR_MIXER_LFELEV,x);
 setText(IDC_LBL_MIXER_LFELEV2,_l("%i%%"),x);
}

bool TmixerPage::isEdit(int idc)
{
 for (int i=0;i<9*9;i++)
  if (idc==idcs[i])
   return true;
 return false;
}

void TmixerPage::onNewFilter(void)
{
 deciD->queryFilterInterface(IID_IaudioFilterMixer,(void**)&filter);
}
void TmixerPage::onFrame(void)
{
 if (!cfgGet(IDFF_customMatrix) && filter)
  {
   double matrix[9][9];
   int inmask,outmask;
   if (filter->getMixerMatrixData2(matrix,&inmask,&outmask)==S_OK)
    {
     static const int NCHANNELS=9;
     static const int mspeakers[]={SPEAKER_FRONT_LEFT,SPEAKER_FRONT_RIGHT,SPEAKER_FRONT_CENTER,SPEAKER_LOW_FREQUENCY,SPEAKER_BACK_LEFT,SPEAKER_BACK_RIGHT,SPEAKER_BACK_CENTER,SPEAKER_SIDE_LEFT,SPEAKER_SIDE_RIGHT};
     for (int i=0;i<NCHANNELS;i++)
      if (inmask&mspeakers[i])
       {
        for (int j=0;j<NCHANNELS;j++)
         if (outmask&mspeakers[j])
          setText(idcs[i*9+j],_l("%.3g"),float(matrix[i][j]));
         else
          setText(idcs[i*9+j],_l(""));
       }
      else
       {
        for (int j=0;j<NCHANNELS;j++)
         setText(idcs[i*9+j],_l(""));
       }
    }
  }
}

void TmixerPage::dlg2out(void)
{
 int out=(int)cbxGetCurItemData(IDC_CBX_MIXER_OUT);
 //                       {0,1,2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
 static const int lfes[]= {7,8,9,10,11,12,13,-1,-1,-1,-1,-1,-1,-1,19,20,-1,-1,-1,-1,-1,22,-1,24,-1};
 if (TmixerSettings::chConfigs[out].nameIndex>=0 && getCheck(IDC_CHB_MIXER_LFE))
  out=lfes[out];
 cfgSet(IDFF_mixerOut,out);
 cfg2dlg();
}

INT_PTR TmixerPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
     {
      HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam));
      if (!isEdit(getId(hed)) || hed!=GetFocus()) return FALSE;
      repaint(hed);
      int idff=((TwidgetMixer*)Twidget::getDlgItem(hed))->idff;
      if (idff)
       {
        int valI;
        eval(hed,-4.0f,4.0f,&valI,100000.0f);
        cfgSet(idff,valI);
       }
      return TRUE;
     }
    else
     switch (LOWORD(wParam))
      {
       case IDC_CHB_MIXER_LFE:
        dlg2out();
        return TRUE;
       case IDC_CBX_MIXER_OUT:
        if (HIWORD(wParam)==CBN_SELCHANGE)
         {
          dlg2out();
          return TRUE;
         }
        break;
      };
    break;
   case WM_CTLCOLOREDIT:
    {
     HWND hwnd=HWND(lParam);
     if (!isEdit(getId(hwnd))) break;
     bool ok=eval(hwnd,-4.0f,4.0f);
     if (!ok)
      {
       HDC dc=HDC(wParam);
       SetBkColor(dc,RGB(255,0,0));
       return INT_PTR(getRed());
      }
     else return FALSE;
    }
  }
 return TconfPageDecAudio::msgProc(uMsg,wParam,lParam);
}
Twidget* TmixerPage::createDlgItem(int id,HWND h)
{
 char_t cls[256]=_l("");
 GetClassName(h,cls,256);
 if (stricmp(cls,_l("EDIT"))==0)
  return new TwidgetMixer(h,this);
 else
  return TconfPageDecAudio::createDlgItem(id,h);
}

void TmixerPage::translate(void)
{
 TconfPageDecAudio::translate();

 static const int order[]={0,1,2,3,4,5,6,21,23, 7, 8, 9,10,11,12,13,22,24,14,15,16,17,18,19,20};
 static const int names[]={0,1,2,3,4,5,6,21,23,-1,-1,-1,-1,-1,-1,-1,-1,-1,14,15,16,17,18,-1,-1};
 int sel=cbxGetCurSel(IDC_CBX_MIXER_OUT);
 cbxClear(IDC_CBX_MIXER_OUT);
 for (int i=0;i<countof(order);i++)
  if (names[i]>=0)
   cbxAdd(IDC_CBX_MIXER_OUT,_(IDC_CBX_MIXER_OUT,TmixerSettings::chConfigs[order[i]].name),TmixerSettings::chConfigs[order[i]].id);
 cbxSetCurSel(IDC_CBX_MIXER_OUT,sel);
}

TmixerPage::TmixerPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecAudio(Iparent,idff),filter(NULL)
{
 resInter=IDC_CHB_MIXER;
 static const TbindCheckbox<TmixerPage> chb[]=
  {
   IDC_CHB_MIXER_NORMALIZE_MATRIX,IDFF_normalizeMatrix,NULL,
   IDC_CHB_MIXER_CUSTOM,IDFF_customMatrix,&TmixerPage::matrix2dlg,
   IDC_CHB_MIXER_VOICE_CONTROL,IDFF_mixerVoiceControl,&TmixerPage::clev2dlg,
   IDC_CHB_MIXER_EXPAND_STEREO,IDFF_mixerExpandStereo,&TmixerPage::slev2dlg,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TmixerPage> htbr[]=
  {
   IDC_TBR_HEADPHONE_DIM,IDFF_headphone_dim,NULL,
   IDC_TBR_MIXER_CLEV,IDFF_mixerClev,&TmixerPage::clev2dlg,
   IDC_TBR_MIXER_SLEV,IDFF_mixerSlev,&TmixerPage::slev2dlg,
   IDC_TBR_MIXER_LFELEV,IDFF_mixerLFElev,&TmixerPage::lfe2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindCombobox<TmixerPage> cbx[]=
  {
   IDC_CBX_MIXER_OUT,IDFF_mixerOut,BINDCBX_DATA,&TmixerPage::cfg2dlg,
   0
  };
 bindComboboxes(cbx);
}
