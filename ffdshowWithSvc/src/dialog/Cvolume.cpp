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
#include "Cvolume.h"
#include "IaudioFilterVolume.h"
#include "TaudioFilter.h"
#include "ffdebug.h"
#include "IffdshowParamsEnum.h"

const int MAX_AMPLIFICATION=1000;

const int TvolumePage::tbrs[8]={IDC_TBR_VOLUME_L,IDC_TBR_VOLUME_C,IDC_TBR_VOLUME_R,IDC_TBR_VOLUME_SL,IDC_TBR_VOLUME_SR,IDC_TBR_VOLUME_AL,IDC_TBR_VOLUME_AR,IDC_TBR_VOLUME_LFE};
const int TvolumePage::lbls[8]={IDC_LBL_VOLUME_L2,IDC_LBL_VOLUME_C2,IDC_LBL_VOLUME_R2,IDC_LBL_VOLUME_SL2,IDC_LBL_VOLUME_SR2,IDC_LBL_VOLUME_AL2,IDC_LBL_VOLUME_AR2,IDC_LBL_VOLUME_LFE2};
const int TvolumePage::pbrs[8]={IDC_PBR_VOLUME_L,IDC_PBR_VOLUME_C,IDC_PBR_VOLUME_R,IDC_PBR_VOLUME_SL,IDC_PBR_VOLUME_SR,IDC_PBR_VOLUME_AL,IDC_PBR_VOLUME_AR,IDC_PBR_VOLUME_LFE};
const int TvolumePage::idffs[8]={IDFF_volumeL,IDFF_volumeC,IDFF_volumeR,IDFF_volumeSL,IDFF_volumeSR,IDFF_volumeAL,IDFF_volumeAR,IDFF_volumeLFE};
const int TvolumePage::idffsMS[8]={IDFF_volumeLmute,IDFF_volumeCmute,IDFF_volumeRmute,IDFF_volumeSLmute,IDFF_volumeSRmute,IDFF_volumeALmute,IDFF_volumeARmute,IDFF_volumeLFEmute};
const int TvolumePage::mutes[8]={IDC_CHB_VOLUME_L_MUTE,IDC_CHB_VOLUME_C_MUTE,IDC_CHB_VOLUME_R_MUTE,IDC_CHB_VOLUME_SL_MUTE,IDC_CHB_VOLUME_SR_MUTE,IDC_CHB_VOLUME_AL_MUTE,IDC_CHB_VOLUME_AR_MUTE,IDC_CHB_VOLUME_LFE_MUTE};
const int TvolumePage::solos[8]={IDC_CHB_VOLUME_L_SOLO,IDC_CHB_VOLUME_C_SOLO,IDC_CHB_VOLUME_R_SOLO,IDC_CHB_VOLUME_SL_SOLO,IDC_CHB_VOLUME_SR_SOLO,IDC_CHB_VOLUME_AL_SOLO,IDC_CHB_VOLUME_AR_SOLO,IDC_CHB_VOLUME_LFE_SOLO};

const TvolumePage::TbindTrackbar<TvolumePage> TvolumePage::htbr[]=
{
 IDC_TBR_VOLUME_L,IDFF_volumeL,NULL,
 IDC_TBR_VOLUME_C,IDFF_volumeC,NULL,
 IDC_TBR_VOLUME_R,IDFF_volumeR,NULL,
 IDC_TBR_VOLUME_SL,IDFF_volumeSL,NULL,
 IDC_TBR_VOLUME_SR,IDFF_volumeSR,NULL,
 IDC_TBR_VOLUME_AL,IDFF_volumeAL,NULL,
 IDC_TBR_VOLUME_AR,IDFF_volumeAR,NULL,
 IDC_TBR_VOLUME_LFE,IDFF_volumeLFE,NULL,
 IDC_TBR_VOLUME_MASTER,IDFF_volume,NULL,
 0,0,NULL
};

void TvolumePage::init(void)
{
 deciD->queryFilterInterface(IID_IaudioFilterVolume,(void**)&filter);
 for (int i=0;i<countof(mutes);i++)
  {
   addHint(mutes[i],_l("mute"));
   addHint(solos[i],_l("solo"));
   SendDlgItemMessage(m_hwnd,pbrs[i],PBM_SETRANGE,0,MAKELPARAM(0,50));
  }
 addHint(IDC_CHB_NORMALIZE_REGAINVOLUME,_l("Unchecked: one-pass normalization will be performed.\n(this mode is used to reach the highest possible loudness\nwith minimum of possible artifacts)\n\nChecked: dynamic range compression will be performed.\n(quieter parts will be amplified more than louder parts,\nalso known as night mode)"));
 switchDb();
}

void TvolumePage::switchDb(void)
{
 isdb=!!cfgGet(IDFF_dlgVolumeDb);
 if (isdb)
  tbrSetRange(IDC_TBR_VOLUME_MASTER,-30,30,5);
 else
  tbrSetRange(IDC_TBR_VOLUME_MASTER,0,300,10);
 for (int i=0;i<countof(tbrs);i++)
  {
   if (isdb)
    tbrSetRange(tbrs[i],-30,30,5);
   else
    tbrSetRange(tbrs[i],-300,0,10);
  }
 setDlgItemText(m_hwnd,IDC_LBL_VOLUME_NORMALIZE_MAX2,isdb?_l("db"):_l("%"));
 master2dlg();
 speakers2dlg();
 normalize2dlg();
}

void TvolumePage::cfg2dlg(void)
{
 setCheck(IDC_CHB_VOL_DB,isdb);
 setCheck(IDC_CHB_VOLUME_SHOWCURRENT,cfgGet(IDFF_showCurrentVolume));
 master2dlg();
 speakers2dlg();
 normalize2dlg();
}
void TvolumePage::normalize2dlg(void)
{
 setCheck(IDC_CHB_VOLUME_NORMALIZE,cfgGet(IDFF_volumeNormalize));
 int val=cfgGet(IDFF_maxNormalization);
 if (isdb)
  val=limit(ff_round(value2db(val/100.0)),0,30);
 SetDlgItemInt(m_hwnd,IDC_ED_VOLUME_NORMALIZE_MAX,val,FALSE);
 setCheck(IDC_CHB_NORMALIZE_RESETONSEEK,cfgGet(IDFF_volumeNormalizeResetOnSeek));
 setCheck(IDC_CHB_NORMALIZE_REGAINVOLUME,cfgGet(IDFF_volumeNormalizeRegainVolume));
}

void TvolumePage::master2dlg(void)
{
 cfg2volTbr(IDC_TBR_VOLUME_MASTER,IDC_LBL_VOLUME_MASTER,IDFF_volume,IDC_LBL_VOLUME_MASTER,1);
}
void TvolumePage::speakers2dlg(void)
{
 for (int i=0;i<countof(tbrs);i++)
  cfg2volTbr(tbrs[i],lbls[i],idffs[i],0,-1);
 mute2dlg();
}

void TvolumePage::mute2dlg(void)
{
 for (int i=0;i<countof(idffsMS);i++)
  {
   int v=cfgGet(idffsMS[i]);
   setCheck(mutes[i],v==1);
   setCheck(solos[i],v==2);
  }
}

void TvolumePage::volTbr2cfg(int idc,int idff,int mult)
{
 int x=tbrGet(idc);
 if (isdb)
  x=mult*ff_round(db2value((double)x*mult)*100);
 cfgSet(idff,mult*x);
}
void TvolumePage::cfg2volTbr(int tbr,int lbl,int idff,int lbltran,int mult)
{
 int x=cfgGet(idff);
 if (isdb)
  {
   int db=ff_round(value2db(x/100.0));
   tbrSet(tbr,mult*db);
   if (lbltran)
    setText(lbl,_l("%s %i db"),_(lbltran),db);
   else
    setText(lbl,_l("%i db"),db);
  }
 else
  {
   if (!isIn(x,0,300))
    cfgSet(idff,x=limit(x,0,300));
   tbrSet(tbr,mult*x);
   if (lbltran)
    setText(lbl,_l("%s %i%%"),_(lbltran),x);
   else
    setText(lbl,_l("%i%%"),x);
  }
}

INT_PTR TvolumePage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_VOLUME_MASTER:volTbr2cfg(IDC_TBR_VOLUME_MASTER,IDFF_volume,1);master2dlg();return TRUE;
     }
    break;
   case WM_VSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_VOLUME_L:volTbr2cfg(IDC_TBR_VOLUME_L,IDFF_volumeL,-1);speakers2dlg();return TRUE;
      case IDC_TBR_VOLUME_C:volTbr2cfg(IDC_TBR_VOLUME_C,IDFF_volumeC,-1);speakers2dlg();return TRUE;
      case IDC_TBR_VOLUME_R:volTbr2cfg(IDC_TBR_VOLUME_R,IDFF_volumeR,-1);speakers2dlg();return TRUE;
      case IDC_TBR_VOLUME_SL:volTbr2cfg(IDC_TBR_VOLUME_SL,IDFF_volumeSL,-1);speakers2dlg();return TRUE;
      case IDC_TBR_VOLUME_SR:volTbr2cfg(IDC_TBR_VOLUME_SR,IDFF_volumeSR,-1);speakers2dlg();return TRUE;
      case IDC_TBR_VOLUME_AL:volTbr2cfg(IDC_TBR_VOLUME_AL,IDFF_volumeAL,-1);speakers2dlg();return TRUE;
      case IDC_TBR_VOLUME_AR:volTbr2cfg(IDC_TBR_VOLUME_AR,IDFF_volumeAR,-1);speakers2dlg();return TRUE;
      case IDC_TBR_VOLUME_LFE:volTbr2cfg(IDC_TBR_VOLUME_LFE,IDFF_volumeLFE,-1);speakers2dlg();return TRUE;
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_VOLUME_NORMALIZE_MAX:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam));
         if (hed!=GetFocus()) return FALSE;
         repaint(hed);
         switch (LOWORD(wParam))
          {
           case IDC_ED_VOLUME_NORMALIZE_MAX:
            {
             if (isdb)
              {
               int db;
               if (eval(hed,0,30,&db))
                {
                 db=ff_round(db2value((double)db)*100);
                 cfgSet(IDFF_maxNormalization,db);
                }
              }
             else
              eval(hed,0,MAX_AMPLIFICATION,IDFF_maxNormalization);
             break;
            }
          }
         return TRUE;
        }
       break;
      case IDC_CHB_VOLUME_L_MUTE:
      case IDC_CHB_VOLUME_C_MUTE:
      case IDC_CHB_VOLUME_R_MUTE:
      case IDC_CHB_VOLUME_SL_MUTE:
      case IDC_CHB_VOLUME_SR_MUTE:
      case IDC_CHB_VOLUME_AL_MUTE:
      case IDC_CHB_VOLUME_AR_MUTE:
      case IDC_CHB_VOLUME_LFE_MUTE:
       {
        int idff=((TwidgetMS*)Twidget::getDlgItem((HWND)lParam))->idff;
        cfgSet(idff,getCheck(LOWORD(wParam)));
        mute2dlg();
        break;
       }
      case IDC_CHB_VOLUME_L_SOLO:
      case IDC_CHB_VOLUME_C_SOLO:
      case IDC_CHB_VOLUME_R_SOLO:
      case IDC_CHB_VOLUME_SL_SOLO:
      case IDC_CHB_VOLUME_SR_SOLO:
      case IDC_CHB_VOLUME_AL_SOLO:
      case IDC_CHB_VOLUME_AR_SOLO:
      case IDC_CHB_VOLUME_LFE_SOLO:
       {
        int idff=((TwidgetMS*)Twidget::getDlgItem((HWND)lParam))->idff;
        for (int i=0;i<countof(idffsMS);i++)
         if (idffsMS[i]==idff)
          cfgSet(idff,getCheck(LOWORD(wParam))?2:0);
         else
          cfgSet(idffsMS[i],0);
        mute2dlg();
        break;
       }
     }
    break;
   case WM_CTLCOLOREDIT:
    {
     HWND hwnd=HWND(lParam);
     bool ok;
     switch (getId(hwnd))
      {
       case IDC_ED_VOLUME_NORMALIZE_MAX:
        ok=eval(hwnd,0,isdb?30:MAX_AMPLIFICATION);break;
       default:return FALSE;
      }
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
Twidget* TvolumePage::createDlgItem(int id,HWND h)
{
 for (int ii=0;ii<countof(idffsMS);ii++)
  {
   if (id==mutes[ii] || id==solos[ii])
    return new TwidgetMS(h,this,idffsMS[ii]);
   else if (id==tbrs[ii])
    return new TvolumeTbr(isdb,h,this,TbindTrackbars(htbr+ii));
   else if (id==IDC_TBR_VOLUME_MASTER)
    return new TvolumeTbr(isdb,h,this,TbindTrackbars(htbr+6));
  }
 return TconfPageDecAudio::createDlgItem(id,h);
}

void TvolumePage::TvolumeTbr::getEditMinMaxVal(const TffdshowParamInfo &info,int *min,int *max,int *val)
{
 if (isdb)
  {
   *min=-30;*max=30;
   *val=ff_round(value2db(self->deci->getParam2(info.id)/100.0));
  }
 else
  TwidgetSubclassTbr::getEditMinMaxVal(info,min,max,val);
}
void TvolumePage::TvolumeTbr::storeEditValue(const TffdshowParamInfo &info,int min,int max,const char_t *valS)
{
 int x;
 self->eval(valS,min,max,&x,1,true);
 if (isdb)
  x=ff_round(db2value((double)x)*100);
 self->deci->putParam(info.id,x);
}

void TvolumePage::onFrame(void)
{
 int amp=(filter && getCheck(IDC_CHB_VOLUME_NORMALIZE)&& getCheck(IDC_CHB_VOLUME))?filter->getCurrentNormalization():100;
 if (isdb)
  setText(IDC_LBL_VOLUME_NORMALIZE_CURRENT,_l("%s %4.1f db"),_(IDC_LBL_VOLUME_NORMALIZE_CURRENT),value2db(amp/100.0));
 else
  setText(IDC_LBL_VOLUME_NORMALIZE_CURRENT,_l("%s %i%%"),_(IDC_LBL_VOLUME_NORMALIZE_CURRENT),amp);
 if (getCheck(IDC_CHB_VOLUME_SHOWCURRENT))
  {
   unsigned int numchannels;int channels[8],volumes[8];
   if (filter && filter->getVolumeData(&numchannels,channels,volumes)==S_OK)
    {
     bool isPbr[8];memset(isPbr,0,sizeof(isPbr));
     for (unsigned int i=0;i<numchannels;i++)
      {
       int pbr;
       if (channels[i]&SPEAKER_FRONT_LEFT)
        {
         pbr=IDC_PBR_VOLUME_L;
         isPbr[0]=true;
        }
       else if (channels[i]&SPEAKER_FRONT_RIGHT)
        {
         pbr=IDC_PBR_VOLUME_R;
         isPbr[2]=true;
        }
       else if (channels[i]&SPEAKER_FRONT_CENTER)
        {
         pbr=IDC_PBR_VOLUME_C;
         isPbr[1]=true;
        }
       else if (channels[i]&(SPEAKER_BACK_LEFT|SPEAKER_BACK_CENTER))
        {
         pbr=IDC_PBR_VOLUME_SL;
         isPbr[3]=true;
        }
       else if (channels[i]&SPEAKER_BACK_RIGHT)
        {
         pbr=IDC_PBR_VOLUME_SR;
         isPbr[4]=true;
        }
       else if (channels[i]&SPEAKER_SIDE_LEFT)
        {
         pbr=IDC_PBR_VOLUME_AL;
         isPbr[5]=true;
        }
       else if (channels[i]&SPEAKER_SIDE_RIGHT)
        {
         pbr=IDC_PBR_VOLUME_AR;
         isPbr[6]=true;
        }
       else if (channels[i]&SPEAKER_LOW_FREQUENCY)
        {
         pbr=IDC_PBR_VOLUME_LFE;
         isPbr[7]=true;
        }
       else
        continue;
       SendDlgItemMessage(m_hwnd,pbr,PBM_SETPOS,volumes[i]?50+ff_round(value2db(volumes[i]/65536.0)):0,0);
       //SendDlgItemMessage(m_hwnd,pbr,PBM_SETBARCOLOR,0,LPARAM(CLR_DEFAULT));
      }
     for (unsigned int i=0;i<countof(isPbr);i++)
      if (!isPbr[i])
       {
        SendDlgItemMessage(m_hwnd,pbrs[i],PBM_SETPOS,0,0);
        //SendDlgItemMessage(m_hwnd,pbrs[i],PBM_SETBARCOLOR,0,LPARAM(CLR_NONE));
       }
     return;
    }
  }
 for (int i=0;i<6;i++)
  SendDlgItemMessage(m_hwnd,pbrs[i],PBM_SETPOS,0,0);
}

TvolumePage::TvolumePage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecAudio(Iparent,idff),filter(NULL)
{
 resInter=IDC_CHB_VOLUME;
 static const TbindCheckbox<TvolumePage> chb[]=
  {
   IDC_CHB_VOLUME_NORMALIZE,IDFF_volumeNormalize,NULL,
   IDC_CHB_VOLUME_SHOWCURRENT,IDFF_showCurrentVolume,NULL,
   IDC_CHB_VOL_DB,IDFF_dlgVolumeDb,&TvolumePage::switchDb,
   IDC_CHB_NORMALIZE_RESETONSEEK,IDFF_volumeNormalizeResetOnSeek,NULL,
   IDC_CHB_NORMALIZE_REGAINVOLUME,IDFF_volumeNormalizeRegainVolume,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);

 bindHtracks(htbr);
}
