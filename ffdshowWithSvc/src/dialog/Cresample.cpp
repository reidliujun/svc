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
#include "Cresample.h"
#include "TresampleSettings.h"
#include "TffdshowPageDec.h"
#include "TflatButtons.h"

void TresamplePage::init(void)
{
 hedResample=GetDlgItem(m_hwnd,IDC_ED_RESAMPLE_FREQ);
 setFont(IDC_BT_RESAMPLE_CONDFREQ,parent->arrowsFont);
 setFont(IDC_BT_RESAMPLE_FREQ    ,parent->arrowsFont);
}

void TresamplePage::cfg2dlg(void)
{
 cbxSetDataCurSel(IDC_CBX_RESAMPLE_MODE,cfgGet(IDFF_resampleMode));
 setText(IDC_ED_RESAMPLE_FREQ,_l("%i"),cfgGet(IDFF_resampleFreq));
 if2dlg();
}
void TresamplePage::if2dlg(void)
{
 int _if=cfgGet(IDFF_resampleIf);
 setCheck(IDC_RBT_RESAMPLE_ALWAYS,_if==0);
 setCheck(IDC_RBT_RESAMPLE_IF    ,_if==1);
 setDlgItemText(m_hwnd,IDC_BT_RESAMPLE_COND,_(IDC_BT_RESAMPLE_COND,TresampleSettings::getCondStr(cfgGet(IDFF_resampleIfCond))));
 SetDlgItemInt(m_hwnd,IDC_ED_RESAMPLE_CONDFREQ,cfgGet(IDFF_resampleIfFreq),FALSE);
}

INT_PTR TresamplePage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_RESAMPLE_FREQ:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText && LOWORD(wParam)==IDC_ED_RESAMPLE_FREQ && GetFocus()==hedResample)
        parent->setChange();
       break;
      case IDC_BT_RESAMPLE_COND:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         cfgSet(IDFF_resampleIfCond,-1*cfgGet(IDFF_resampleIfCond));
         if2dlg();
         return TRUE;
        }
       break;
     }
    break;
   case WM_CTLCOLOREDIT:
    {
     HWND hwnd=HWND(lParam);
     bool ok;
     switch (getId(hwnd))
      {
       case IDC_ED_RESAMPLE_FREQ:
        ok=eval(hwnd,1,192000);break;
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
   case WM_DRAWITEM:
    switch (wParam)
     {
      case IDC_BT_RESAMPLE_COND:
       {
        DRAWITEMSTRUCT *dis=(LPDRAWITEMSTRUCT)lParam;
        ((TflatButton*)Twidget::getDlgItem(dis->hwndItem))->paint(dis);
        return TRUE;
       }
     }
    break;
  }
 return TconfPageDecAudio::msgProc(uMsg,wParam,lParam);
}
Twidget* TresamplePage::createDlgItem(int id,HWND h)
{
 if (id==IDC_BT_RESAMPLE_COND)
  return new TflatButton(h,this);
 else
  return TconfPageDecAudio::createDlgItem(id,h);
}

void TresamplePage::onResampleMenu(int idc,int idff)
{
 static const char_t *freqs[]=
  {
   _l("8000"),
   _l("11025"),
   _l("12000"),
   _l("16000"),
   _l("22100"),
   _l("24000"),
   _l("32000"),
   _l("44100"),
   _l("48000"),
   _l("96000"),
   NULL,
  };
 int cmd=selectFromMenu(freqs,idc,false);
 if (isIn(cmd,0,(int)countof(freqs)-2))
  cfgSet(idff,atoi(freqs[cmd]));
}
void TresamplePage::onResampleFreqMenu(void)
{
 onResampleMenu(IDC_BT_RESAMPLE_FREQ,IDFF_resampleFreq);
 cfg2dlg();
}
void TresamplePage::onResampleCondMenu(void)
{
 onResampleMenu(IDC_BT_RESAMPLE_CONDFREQ,IDFF_resampleIfFreq);
 if2dlg();
}

void TresamplePage::translate(void)
{
 TconfPageDecAudio::translate();

 int ii=cbxGetCurSel(IDC_CBX_RESAMPLE_MODE);
 cbxClear(IDC_CBX_RESAMPLE_MODE);
 for (int i=0;TresampleSettings::resamplers[i].name;i++)
  cbxAdd(IDC_CBX_RESAMPLE_MODE,_(IDC_CBX_RESAMPLE_MODE,TresampleSettings::resamplers[i].name),TresampleSettings::resamplers[i].id);
 cbxSetCurSel(IDC_CBX_RESAMPLE_MODE,ii);
}

void TresamplePage::applySettings(void)
{
 eval(hedResample,1,192000,IDFF_resampleFreq);
}

TresamplePage::TresamplePage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecAudio(Iparent,idff)
{
 resInter=IDC_CHB_RESAMPLE;
 static const TbindCombobox<TresamplePage> cbx[]=
  {
   IDC_CBX_RESAMPLE_MODE,IDFF_resampleMode,BINDCBX_DATA,NULL,
   0
  };
 bindComboboxes(cbx);
 static const TbindRadiobutton<TresamplePage> rbt[]=
  {
   IDC_RBT_RESAMPLE_ALWAYS,IDFF_resampleIf,0,NULL,
   IDC_RBT_RESAMPLE_IF    ,IDFF_resampleIf,1,NULL,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindButton<TresamplePage> bt[]=
  {
   IDC_BT_RESAMPLE_FREQ    ,&TresamplePage::onResampleFreqMenu,
   IDC_BT_RESAMPLE_CONDFREQ,&TresamplePage::onResampleCondMenu,
   0,NULL
  };
 bindButtons(bt);
 static const TbindEditInt<TresamplePage> edInt[]=
  {
   IDC_ED_RESAMPLE_CONDFREQ,1,192000,IDFF_resampleIfFreq,NULL,
   0
  };
 bindEditInts(edInt);
}
