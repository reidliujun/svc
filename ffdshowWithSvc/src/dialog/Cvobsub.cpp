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
#include "Cvobsub.h"
#include "TsubtitlesSettings.h"
#include "Ttranslate.h"

void TvobsubPage::init(void)
{
 tbrSetRange(IDC_TBR_VOBSUB_SCALE,0x10,0x300);
 tbrSetRange(IDC_TBR_VOBSUB_GAUSS,1,2000);
}

void TvobsubPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_VOBSUB,cfgGet(IDFF_subVobsub));
 cbxSetCurSel(IDC_CBX_VOBSUB_AA,cfgGet(IDFF_subVobsubAA));
 cbxSetDataCurSel(IDC_CBX_VOBSUB_DEFLANG,cfgGet(IDFF_subDefLang));
 cbxSetDataCurSel(IDC_CBX_VOBSUB_DEFLANG2,cfgGet(IDFF_subDefLang2));
 int lcnt=deciV->getSubtitleLanguagesCount2();
 static const int idCurlang[]={IDC_LBL_VOBSUB_CURLANG,IDC_CBX_VOBSUB_CURLANG,0};
 if (lcnt>0)
  {
   enable(1,idCurlang);
   cbxClear(IDC_CBX_VOBSUB_CURLANG);
   for (int i=0;i<lcnt;i++)
    {
     const char_t *desc;deciV->getSubtitleLanguageDesc(i,&desc);
     if (desc[0])
      cbxAdd(IDC_CBX_VOBSUB_CURLANG,desc,i);
    }
   cbxSetDataCurSel(IDC_CBX_VOBSUB_CURLANG,cfgGet(IDFF_subCurLang));
  }
 else
  enable(0,idCurlang);
 setCheck(IDC_CHB_VOBSUB_CHANGEPOSITION,cfgGet(IDFF_subVobsubChangePosition));
 scale2dlg();
 gauss2dlg();
}
void TvobsubPage::scale2dlg(void)
{
 int scale=cfgGet(IDFF_subVobsubScale);
 tbrSet(IDC_TBR_VOBSUB_SCALE,scale);
 setText(IDC_LBL_VOBSUB_SCALE,_l("%s %.3f"),_(IDC_LBL_VOBSUB_SCALE),scale/256.0f);
}
void TvobsubPage::gauss2dlg(void)
{
 tbrSet(IDC_TBR_VOBSUB_GAUSS,cfgGet(IDFF_subVobsubAAswgauss),IDC_LBL_VOBSUB_GAUSS,NULL,1000.0);
 static const int idGausss[]={IDC_LBL_VOBSUB_GAUSS,IDC_TBR_VOBSUB_GAUSS,0};
 enable(cfgGet(IDFF_subVobsubAA)==4,idGausss);
}

void TvobsubPage::translate(void)
{
 TconfPageBase::translate();

 cbxTranslate(IDC_CBX_VOBSUB_AA,TsubtitlesSettings::vobsubAAs);

 int sel1=cbxGetCurSel(IDC_CBX_VOBSUB_DEFLANG),sel2=cbxGetCurSel(IDC_CBX_VOBSUB_DEFLANG2);
 cbxClear(IDC_CBX_VOBSUB_DEFLANG);cbxClear(IDC_CBX_VOBSUB_DEFLANG2);
 const char_t *prev=NULL;
 for (int i=0;TsubtitlesSettings::langs[i].desc;i++)
  {
   const char_t *cur=TsubtitlesSettings::langs[i].desc;
   const char_t *lang=TsubtitlesSettings::langs[i].lang;
   if (lang && (!prev || strcmp(prev,cur)!=0))
    {
     const char_t *_cur=_(IDC_CBX_VOBSUB_DEFLANG,cur);
     cbxAdd(IDC_CBX_VOBSUB_DEFLANG ,_cur,Ttranslate::lang2int(lang));
     cbxAdd(IDC_CBX_VOBSUB_DEFLANG2,_cur,Ttranslate::lang2int(lang));
    }
   prev=cur;
  }
 cbxSetCurSel(IDC_CBX_VOBSUB_DEFLANG,sel1);cbxSetCurSel(IDC_CBX_VOBSUB_DEFLANG2,sel2);
}

TvobsubPage::TvobsubPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff,3)
{
 dialogId=IDD_VOBSUB;
 static const TbindCheckbox<TvobsubPage> chb[]=
  {
   IDC_CHB_VOBSUB,IDFF_subVobsub,NULL,
   IDC_CHB_VOBSUB_CHANGEPOSITION,IDFF_subVobsubChangePosition,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TvobsubPage> htbr[]=
  {
   IDC_TBR_VOBSUB_SCALE,IDFF_subVobsubScale,&TvobsubPage::scale2dlg,
   IDC_TBR_VOBSUB_GAUSS,IDFF_subVobsubAAswgauss,&TvobsubPage::gauss2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindCombobox<TvobsubPage> cbx[]=
  {
   IDC_CBX_VOBSUB_AA,IDFF_subVobsubAA,BINDCBX_SEL,&TvobsubPage::gauss2dlg,
   IDC_CBX_VOBSUB_DEFLANG,IDFF_subDefLang,BINDCBX_DATA,NULL,
   IDC_CBX_VOBSUB_DEFLANG2,IDFF_subDefLang2,BINDCBX_DATA,NULL,
   IDC_CBX_VOBSUB_CURLANG,IDFF_subCurLang,BINDCBX_DATA,NULL,
   0
  };
 bindComboboxes(cbx);
}
