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
#include "Ceq.h"
#include "TeqSettings.h"
#include "TffdshowPageDec.h"
#include "IffdshowParamsEnum.h"

const int TeqPage::tbrs[10]=
{
 IDC_TBR_EQ0,IDC_TBR_EQ1,IDC_TBR_EQ2,IDC_TBR_EQ3,IDC_TBR_EQ4,IDC_TBR_EQ5,IDC_TBR_EQ6,IDC_TBR_EQ7,IDC_TBR_EQ8,IDC_TBR_EQ9
};
const int TeqPage::lblsdb[10]=
{
 IDC_LBL_EQDB0,IDC_LBL_EQDB1,IDC_LBL_EQDB2,IDC_LBL_EQDB3,IDC_LBL_EQDB4,IDC_LBL_EQDB5,IDC_LBL_EQDB6,IDC_LBL_EQDB7,IDC_LBL_EQDB8,IDC_LBL_EQDB9
};
const int TeqPage::eds[10]=
{
 IDC_ED_EQ0,IDC_ED_EQ1,IDC_ED_EQ2,IDC_ED_EQ3,IDC_ED_EQ4,IDC_ED_EQ5,IDC_ED_EQ6,IDC_ED_EQ7,IDC_ED_EQ8,IDC_ED_EQ9
};
const int TeqPage::idffsDb[10]=
{
 IDFF_eq0,IDFF_eq1,IDFF_eq2,IDFF_eq3,IDFF_eq4,IDFF_eq5,IDFF_eq6,IDFF_eq7,IDFF_eq8,IDFF_eq9
};
const int TeqPage::idffsFreq[10]=
{
 IDFF_eq0freq,IDFF_eq1freq,IDFF_eq2freq,IDFF_eq3freq,IDFF_eq4freq,IDFF_eq5freq,IDFF_eq6freq,IDFF_eq7freq,IDFF_eq8freq,IDFF_eq9freq
};

const TeqPage::TbindTrackbar<TeqPage> TeqPage::htbr[]=
{
 IDC_TBR_EQ0,IDFF_eq0,NULL,
 IDC_TBR_EQ1,IDFF_eq1,NULL,
 IDC_TBR_EQ2,IDFF_eq2,NULL,
 IDC_TBR_EQ3,IDFF_eq3,NULL,
 IDC_TBR_EQ4,IDFF_eq4,NULL,
 IDC_TBR_EQ5,IDFF_eq5,NULL,
 IDC_TBR_EQ6,IDFF_eq6,NULL,
 IDC_TBR_EQ7,IDFF_eq7,NULL,
 IDC_TBR_EQ8,IDFF_eq8,NULL,
 IDC_TBR_EQ9,IDFF_eq9,NULL,
 0,0,NULL
};

void TeqPage::init(void)
{
 RECT r;
 GetWindowRect(GetDlgItem(m_hwnd,lblsdb[0]),&r);
 int h=r.bottom-r.top+2;
 for (int i=0;i<10;i++)
  {
   tbrSetRange(tbrs[i],0,200,10);
   GetWindowRect(GetDlgItem(m_hwnd,eds[i]),&r);
   SetWindowPos(GetDlgItem(m_hwnd,eds[i]),NULL,0,0,r.right-r.left,h,SWP_NOMOVE|SWP_NOOWNERZORDER);
   SendDlgItemMessage(m_hwnd,eds[i],EM_SETMARGINS,EC_LEFTMARGIN|EC_RIGHTMARGIN,0);
  }
}

void TeqPage::cfg2dlg(void)
{
 for (int i=0;i<10;i++)
  {
   int db=cfgGet(idffsDb[i]);
   tbrSet(tbrs[i],200-db);
   setText(eds[i],_l("%g"),cfgGet(idffsFreq[i])/100.0f);
  }
 setText(IDC_ED_EQ_HIGHDB,_l("%g"),cfgGet(IDFF_eqHighdb)/100.0);
 setText(IDC_ED_EQ_LOWDB ,_l("%g"),cfgGet(IDFF_eqLowdb) /100.0);
 setCheck(IDC_CHB_EQ_SUPER,cfgGet(IDFF_eqSuper));
 lblsdb2dlg();
}
float TeqPage::getEqDb(int lowdb,int highdb,int i)
{
 return ((highdb-lowdb)*cfgGet(idffsDb[i])/200+lowdb)/100.0f;
}
void TeqPage::lblsdb2dlg(void)
{
 int lowdb=cfgGet(IDFF_eqLowdb),highdb=cfgGet(IDFF_eqHighdb);
 for (int i=0;i<10;i++)
  setText(lblsdb[i],_l("%5.2f"),getEqDb(lowdb,highdb,i));
}

bool TeqPage::presetsSort(const TwinampQ1preset &p1,const TwinampQ1preset &p2)
{
 return DwStrcasecmp(p1.name,p2.name)<0;
}

INT_PTR TeqPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_VSCROLL:
    switch (getId(HWND(lParam)))
     {
      case IDC_TBR_EQ0:
      case IDC_TBR_EQ1:
      case IDC_TBR_EQ2:
      case IDC_TBR_EQ3:
      case IDC_TBR_EQ4:
      case IDC_TBR_EQ5:
      case IDC_TBR_EQ6:
      case IDC_TBR_EQ7:
      case IDC_TBR_EQ8:
      case IDC_TBR_EQ9:
       for (int i=0;i<10;i++)
        cfgSet(idffsDb[i],200-tbrGet(tbrs[i]));
       cfg2dlg();
       return TRUE;
     }
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_ED_EQ_HIGHDB:
      case IDC_ED_EQ_LOWDB:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam));
         if (hed!=GetFocus()) return FALSE;
         repaint(hed);
         switch (LOWORD(wParam))
          {
           case IDC_ED_EQ_HIGHDB:eval(hed,cfgGet(IDFF_eqLowdb)/100.0+0.1,100.0,IDFF_eqHighdb,100.0);lblsdb2dlg();break;
           case IDC_ED_EQ_LOWDB :eval(hed,-100.0,cfgGet(IDFF_eqHighdb)/100.0-0.1,IDFF_eqLowdb,100.0);lblsdb2dlg();break;
          }
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
       case IDC_ED_EQ_HIGHDB:ok=eval(hwnd,cfgGet(IDFF_eqLowdb)/100.0+0.1,100.0);break;
       case IDC_ED_EQ_LOWDB:ok=eval(hwnd,-100.0,cfgGet(IDFF_eqHighdb)/100.0-0.1);break;
       default:goto endColor;
      }
     if (!ok)
      {
       HDC dc=HDC(wParam);
       SetBkColor(dc,RGB(255,0,0));
       return INT_PTR(getRed());
      }
     else return FALSE;
     endColor:;
    }
  }
 return TconfPageDecAudio::msgProc(uMsg,wParam,lParam);
}

void TeqPage::onWinampLoad(void)
{
 char_t q1flnm[MAX_PATH]=_l("");
 if (dlgGetFile(false,m_hwnd,_(-IDD_EQ,_l("Select Winamp preset file")),_l("Winamp equalizer presets (*.q1)\0*.q1\0"),_l("q1"),q1flnm,cfgGetStr(IDFF_winamp2dir),0))
  {
   FILE *file=fopen(q1flnm,_l("rb"));
   if (!file) return;
   char header[31];
   fread(header,1,31,file);
   typedef std::vector<TwinampQ1preset> TwinampQ1presets;
   TwinampQ1presets presets;
   if (strncmp(header,"Winamp EQ library file v1.1",27)==0)
    {
     char name[257];
     while (fread(name,1,257,file))
      {
       TwinampQ1preset preset;
       preset.name=name;
       char bands[11];
       fread(bands,1,11,file);
       preset.preamp=20.0-((bands[10]*40.0)/64);
       for (int i=0;i<10;i++)
        preset.db[i]=20.0-((bands[i]*40.0)/64);
       presets.push_back(preset);
      }
    }
   fclose(file);
   if (!presets.empty())
    {
     std::sort(presets.begin(),presets.end(),presetsSort);
     HMENU hm=CreatePopupMenu();
     int ord=0;
     for (TwinampQ1presets::const_iterator p=presets.begin();p!=presets.end();p++)
      insertMenuItem(hm,ord,ord+1,stringreplace(p->name,"&","&&",rfReplaceAll).c_str(),false);
     RECT r;
     GetWindowRect(GetDlgItem(m_hwnd,IDC_BT_EQ_PRESET_LOAD),&r);
     POINT pt={0,r.bottom-r.top};
     ClientToScreen(GetDlgItem(m_hwnd,IDC_BT_EQ_PRESET_LOAD),&pt);
     int cmd=TrackPopupMenu(_(hm),TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,pt.x,pt.y,0,m_hwnd,NULL);
     PostMessage(m_hwnd,WM_NULL,0,0);
     if (cmd>0)
      {
       const TwinampQ1preset &p=presets[cmd-1];
       cfgSet(IDFF_eqLowdb,-20*100);cfgSet(IDFF_eqHighdb,20*100);
       for (int i=0;i<10;i++)
        {
         cfgSet(idffsFreq[i],int(TeqSettings::Fwinamp[i]*100));
         cfgSet(idffsDb[i],int(((p.preamp/20.0+1)*p.db[i]+20)*5));
        }
       cfg2dlg();
       parent->setChange();
      }
     DestroyMenu(hm);
    }
  }
}

Twidget* TeqPage::createDlgItem(int id,HWND h)
{
 for (int ii=0;ii<countof(tbrs);ii++)
  if (id==tbrs[ii])
   return new TeqTbr(ii,h,this,TbindTrackbars(htbr+ii));
 return TconfPageDecAudio::createDlgItem(id,h);
}
void TeqPage::TeqTbr::getEditMinMaxVal(const TffdshowParamInfo &info,int *min,int *max,int *val)
{
 int imin,imax;
 *min=ff_round((imin=self->deci->getParam2(IDFF_eqLowdb))/100.0f);
 *max=ff_round((imax=self->deci->getParam2(IDFF_eqHighdb))/100.0f);
 *val=ff_round(self->getEqDb(imin,imax,index));
}
void TeqPage::TeqTbr::storeEditValue(const TffdshowParamInfo &info,int min,int max,const char_t *valS)
{
 int db;
 self->eval(valS,min,max,&db,1,true);
 db=200*(db-min)/(max-min);
 self->deci->putParam(info.id,db);
}

TeqPage::TeqPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecAudio(Iparent,idff)
{
 resInter=IDC_CHB_EQ;
 static const TbindEditReal<TeqPage> edReal[]=
  {
   IDC_ED_EQ0,1.0,192000.0,IDFF_eq0freq,100.0,NULL,
   IDC_ED_EQ1,1.0,192000.0,IDFF_eq1freq,100.0,NULL,
   IDC_ED_EQ2,1.0,192000.0,IDFF_eq2freq,100.0,NULL,
   IDC_ED_EQ3,1.0,192000.0,IDFF_eq3freq,100.0,NULL,
   IDC_ED_EQ4,1.0,192000.0,IDFF_eq4freq,100.0,NULL,
   IDC_ED_EQ5,1.0,192000.0,IDFF_eq5freq,100.0,NULL,
   IDC_ED_EQ6,1.0,192000.0,IDFF_eq6freq,100.0,NULL,
   IDC_ED_EQ7,1.0,192000.0,IDFF_eq7freq,100.0,NULL,
   IDC_ED_EQ8,1.0,192000.0,IDFF_eq8freq,100.0,NULL,
   IDC_ED_EQ9,1.0,192000.0,IDFF_eq9freq,100.0,NULL,
   0
  };
 bindEditReals(edReal);
 static const TbindCheckbox<TeqPage> chb[]=
  {
   IDC_CHB_EQ_SUPER,IDFF_eqSuper,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindButton<TeqPage> bt[]=
  {
   IDC_BT_EQ_PRESET_LOAD,&TeqPage::onWinampLoad,
   0,NULL
  };
 bindButtons(bt);

 bindHtracks(htbr);
}
