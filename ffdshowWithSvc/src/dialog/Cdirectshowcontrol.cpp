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
#include "Cdirectshowcontrol.h"
#include "TvideoCodec.h"
#include "TaudioCodec.h"
#include "ffdshow_mediaguids.h"
#include "Tconfig.h"
#include "TffdshowPageDec.h"
#include "Tinfo.h"
#include "Ttranslate.h"
#include "TcompatibilityList.h"
#include "TglobalSettings.h"

//=================================== TdirectshowControlPageDec =======================================
void TdirectshowControlPageDec::init(void)
{
 static const int insts[]={IDC_LBL_MULTIPLE_INSTANCES,IDC_CBX_MULTIPLE_INSTANCES,0};
 enable((filterMode&IDFF_FILTERMODE_VFW)==0,insts);

 merits.clear();
 static const int idmerits[]={IDC_LBL_MERIT,IDC_TBR_MERIT,0};
 isMerit=(filterMode&(IDFF_FILTERMODE_VFW|IDFF_FILTERMODE_ENC|IDFF_FILTERMODE_VIDEORAW|IDFF_FILTERMODE_AUDIORAW))==0;
 if (isMerit)
  {
   //merits.push_back(std::make_pair(MERIT_SW_COMPRESSOR /*0x100000*/,_("SW compressor")));
   //merits.push_back(std::make_pair(MERIT_HW_COMPRESSOR /*0x100050*/,_("HW compressor")));
   merits.push_back(std::make_pair(MERIT_DO_NOT_USE    /*0x200000*/,(const char_t*)_l("do not use")     ));
   merits.push_back(std::make_pair(MERIT_UNLIKELY      /*0x400000*/,(const char_t*)_l("unlikely")       ));
   merits.push_back(std::make_pair(MERIT_NORMAL        /*0x600000*/,(const char_t*)_l("normal")         ));
   merits.push_back(std::make_pair(MERIT_PREFERRED     /*0x800000*/,(const char_t*)_l("preferred")      ));
   merits.push_back(std::make_pair(cfgGet(IDFF_defaultMerit)       ,(const char_t*)_l("ffdshow default")));
   merits.push_back(std::make_pair(0xffffff00                      ,(const char_t*)_l("very high")      ));
   std::sort(merits.begin(),merits.end(),sortMerits);
   tbrSetRange(IDC_TBR_MERIT,0,(int)merits.size()-1,1);
   enable(1,idmerits);
  }
 else
  enable(0,idmerits);
 
 edLimitText(IDC_ED_BLACKLIST,MAX_COMPATIBILITYLIST_LENGTH);
 edLimitText(IDC_ED_COMPATIBILITYLIST,MAX_COMPATIBILITYLIST_LENGTH);
 addHint(IDC_ED_BLACKLIST,_l("List of applications for which ffdshow should not load, separated by semicolons"));
 addHint(IDC_ED_COMPATIBILITYLIST,_l("List of applications which are compatible with ffdshow, separated by semicolons."));

 blacklist2dlg();
}

void TdirectshowControlPageDec::cfg2dlg(void)
{
 setCheck(IDC_CHB_ADDTOROT,cfgGet(IDFF_addToROT));
 cbxSetDataCurSel(IDC_CBX_MULTIPLE_INSTANCES,cfgGet(IDFF_multipleInstances));
 if (isMerit) merit2dlg();
 blacklist2dlg();
}

bool TdirectshowControlPageDec::sortMerits(const Tmerit &m1,const Tmerit &m2)
{
 return m1.first<m2.first;
}

void TdirectshowControlPageDec::merit2dlg(void)
{
 DWORD merit=0;deci->getMerit(&merit);
 meritset=true;
 for (int ii=0;ii<2;ii++,merit=cfgGet(IDFF_defaultMerit))
  {
   int i=0;
   for (Tmerits::const_iterator m=merits.begin();m!=merits.end();m++,i++)
    if (m->first==merit)
     {
      tbrSet(IDC_TBR_MERIT,i);
      setText(IDC_LBL_MERIT,_l("%s %s"),_(IDC_LBL_MERIT),_(IDC_LBL_MERIT,m->second));
      return;
     }
  }
}

void TdirectshowControlPageDec::blacklist2dlg(void)
{
 int is=cfgGet(IDFF_isBlacklist);
 setCheck(IDC_CHB_BLACKLIST,is);
 const char_t *blacklist0=cfgGetStr(IDFF_blacklist);
 ffstring blacklistResult;
 convertDelimit(blacklist0,_l(";"),blacklistResult,_l(";"));
 SetDlgItemText(m_hwnd,IDC_ED_BLACKLIST,blacklistResult.c_str());
 enable(is,IDC_ED_BLACKLIST);
 enable(is,IDC_BT_BLACKLIST);

 is=cfgGet(IDFF_isWhitelist);
 setCheck(IDC_CHB_COMPATIBILITYLIST,is);
 const char_t *complist0=cfgGetStr(IDFF_whitelist);
 ffstring complistResult;
 convertDelimit(complist0,_l(";"),complistResult,_l(";"));
 SetDlgItemText(m_hwnd,IDC_ED_COMPATIBILITYLIST,complistResult.c_str());
 setCheck(IDC_CHB_IS_COMPMGR,cfgGet(IDFF_isCompMgr));
 enable(is,IDC_ED_COMPATIBILITYLIST);
 enable(is,IDC_BT_COMPATIBILITYLIST);
 enable(is,IDC_CHB_IS_COMPMGR);
}

void TdirectshowControlPageDec::applySettings(void)
{
 if (isMerit && meritset)
  deci->setMerit(merits[tbrGet(IDC_TBR_MERIT)].first);
}

bool TdirectshowControlPageDec::strCaseCmp(const ffstring &s1,const ffstring &s2)
{
 return DwStrcasecmp(s1,s2)==-1;
}

INT_PTR TdirectshowControlPageDec::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_HSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_MERIT:
       setText(IDC_LBL_MERIT,_l("%s %s"),_(IDC_LBL_MERIT),_(IDC_LBL_MERIT,merits[tbrGet(IDC_TBR_MERIT)].second));
       parent->setChange();
       return TRUE;
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))  
     {
      case IDC_ED_BLACKLIST:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        comp_dlg2cfg(IDC_ED_BLACKLIST,IDFF_blacklist,_l(";"));
       return TRUE;
      case IDC_ED_COMPATIBILITYLIST:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        comp_dlg2cfg(IDC_ED_COMPATIBILITYLIST,IDFF_whitelist,_l(";"));
       return TRUE;
     }    
    break;
  }
 return TconfPageDec::msgProc(uMsg,wParam,lParam);
}
void TdirectshowControlPageDec::comp_dlg2cfg(int IDC_ED_target,int IDFF_target,const char_t *delimitResult)
{
 char_t complist0[MAX_COMPATIBILITYLIST_LENGTH];
 GetDlgItemText(m_hwnd,IDC_ED_target,complist0,MAX_COMPATIBILITYLIST_LENGTH);
 ffstring complistResult;
 convertDelimit(complist0,_l(";"),complistResult,delimitResult);
 cfgSet(IDFF_target,complistResult.c_str());
}

void TdirectshowControlPageDec::convertDelimit(const char_t* instr,const char_t *inDelimit,ffstring &outstr,const char_t *outDelimit,const char_t *newFileName)
{
 strings complist;
 strtok(instr,inDelimit,complist);

 std::list<ffstring> complistList;
 complistList.insert(complistList.begin(),complist.begin(),complist.end());
 if (newFileName)
  complistList.push_back(newFileName);
 complistList.sort(strCaseCmp);

 for (std::list<ffstring>::iterator i=complistList.begin();i!=complistList.end();i++)
  outstr+=*i+outDelimit;
}

void TdirectshowControlPageDec::translate(void)
{
 TconfPageDec::translate();

 int ii=cbxGetCurSel(IDC_CBX_MULTIPLE_INSTANCES);
 cbxClear(IDC_CBX_MULTIPLE_INSTANCES);
 for (int i=0;multipleInstances[i].name;i++)
  cbxAdd(IDC_CBX_MULTIPLE_INSTANCES,_(IDC_CBX_MULTIPLE_INSTANCES,multipleInstances[i].name),multipleInstances[i].id);
 cbxSetCurSel(IDC_CBX_MULTIPLE_INSTANCES,ii);
}

void TdirectshowControlPageDec::onChbIsCompMgr(void)
{
 cfgSet(IDFF_isCompMgrChanged,1);
}

TdirectshowControlPageDec::TdirectshowControlPageDec(TffdshowPageDec *Iparent):TconfPageDec(Iparent,NULL,0)
{
 dialogId=IDD_DIRECTSHOWCONTROL;
 meritset=false;
 static const TbindCheckbox<TdirectshowControlPageDec> chb[]=
  {
   IDC_CHB_ADDTOROT,IDFF_addToROT,NULL,
   IDC_CHB_BLACKLIST,IDFF_isBlacklist,&TdirectshowControlPageDec::blacklist2dlg,
   IDC_CHB_COMPATIBILITYLIST,IDFF_isWhitelist,&TdirectshowControlPageDec::blacklist2dlg,
   IDC_CHB_IS_COMPMGR,IDFF_isCompMgr,&TdirectshowControlPageDec::onChbIsCompMgr,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindCombobox<TdirectshowControlPageDec> cbx[]=
  {
   IDC_CBX_MULTIPLE_INSTANCES,IDFF_multipleInstances,BINDCBX_DATA,NULL,
   0
  };
 bindComboboxes(cbx);
 static const TbindButton<TdirectshowControlPageDec> bt[]=
  {
   IDC_BT_COMPATIBILITYLIST,&TdirectshowControlPageDec::onEditCompatibilitylist,
   IDC_BT_BLACKLIST,&TdirectshowControlPageDec::onEditBlacklist,
   0,NULL
  };
 bindButtons(bt); 
}
TdirectshowControlPageDec::~TdirectshowControlPageDec()
{
}
void TdirectshowControlPageDec::onEditCompatibilitylist(void)
{
 onEditCompI(IDFF_whitelist);
}
void TdirectshowControlPageDec::onEditBlacklist(void)
{
 onEditCompI(IDFF_blacklist);
}
void TdirectshowControlPageDec::onEditCompI(int IDFF_target)
{
 TcompatibilityList *dlg=new TcompatibilityList(deci,m_hwnd,10,10,IDFF_target);
 if (dlg->show())
  {
   blacklist2dlg();
   parent->setChange();
  }
 delete dlg;
}
