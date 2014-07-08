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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Cavisynth.h"
#include "TffdshowPageDec.h"
#include "imgfilters/avisynth/TimgFilterAvisynth.h"
#include "TavisynthSettings.h"

const char_t* TavisynthPage::avs_mask=_l("Avisynth scripts (*.avs)\0*.avs\0\0");

const char_t* TavisynthPage::pulldownModes[]=
 {
  _l("Ignore pulldown"),
  _l("Apply pulldown"),
  _l("Smooth timestamps"),
  0
 };

int bufferControlIDs[]=
 {
  IDC_ED_AVISYNTH_BUF_AHEAD,
  IDC_ED_AVISYNTH_BUF_BACK,
  IDC_BT_AVISYNTH_USECURRENT
 };

void TavisynthPage::init(void)
{
 helpURL=_l("http://ffdshow-tryout.wiki.sourceforge.net/AviSynth");

 static const TanchorInfo ainfo[]=
  {
   IDC_GRP_AVISYNTH,TanchorInfo::LEFT|TanchorInfo::RIGHT|TanchorInfo::BOTTOM|TanchorInfo::TOP,
   IDC_BT_AVISYNTH_LOAD,TanchorInfo::RIGHT|TanchorInfo::TOP,
   IDC_BT_AVISYNTH_SAVE,TanchorInfo::RIGHT|TanchorInfo::TOP,
   IDC_ED_AVISYNTH,TanchorInfo::LEFT|TanchorInfo::RIGHT|TanchorInfo::BOTTOM|TanchorInfo::TOP,
   0,0
  };

 anchors.init(ainfo,*this);

 edLimitText(IDC_ED_AVISYNTH_BUF_BACK,2);
 edLimitText(IDC_ED_AVISYNTH_BUF_AHEAD,2);
 edLimitText(IDC_ED_AVISYNTH,MAX_AVISYNTH_SCRIPT_LENGTH);

 addHint(IDC_ED_AVISYNTH_BUF_AHEAD,_l("Number of frames to buffer ahead"));
 addHint(IDC_ED_AVISYNTH_BUF_BACK,_l("Number of used frames to keep in buffer"));
}

void TavisynthPage::cfg2dlg(void)
{
 setCheck(IDC_CHB_AVISYNTH_FFDSHOW,cfgGet(IDFF_avisynthFfdshowSource));
 cbxSetCurSel(IDC_CBX_AVISYNTH_PULLDOWN,cfgGet(IDFF_avisynthApplyPulldown));
 setCheck(IDC_CHB_AVISYNTH_YV12 ,cfgGet(IDFF_avisynthInYV12 ));
 setCheck(IDC_CHB_AVISYNTH_YUY2 ,cfgGet(IDFF_avisynthInYUY2 ));
 setCheck(IDC_CHB_AVISYNTH_RGB32,cfgGet(IDFF_avisynthInRGB32));
 setCheck(IDC_CHB_AVISYNTH_RGB24,cfgGet(IDFF_avisynthInRGB24));
 setCheck(IDC_CHB_AVISYNTH_BUFFER,cfgGet(IDFF_avisynthEnableBuffering));
 SetDlgItemInt(m_hwnd,IDC_ED_AVISYNTH_BUF_BACK,cfgGet(IDFF_avisynthBufferBack),false);
 SetDlgItemInt(m_hwnd,IDC_ED_AVISYNTH_BUF_AHEAD,cfgGet(IDFF_avisynthBufferAhead),false);
 setDlgItemText(m_hwnd,IDC_ED_AVISYNTH,cfgGetStr(IDFF_avisynthScript));
 enable(getCheck(IDC_CHB_AVISYNTH_BUFFER),bufferControlIDs,false);
}

INT_PTR TavisynthPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 int c;
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_AVISYNTH:
       c=getCheck(IDC_CHB_AVISYNTH);
       cfgSet(IDFF_isAvisynth,c);
       if(!c)
        cfgSet(IDFF_OSDuser,_l(""));
       break;

      case IDC_CHB_AVISYNTH_BUFFER:
       enable(getCheck(IDC_CHB_AVISYNTH_BUFFER),bufferControlIDs,false);
       break;

      case IDC_ED_AVISYNTH_BUF_BACK:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         c=GetDlgItemInt(m_hwnd,IDC_ED_AVISYNTH_BUF_BACK,NULL,false);
         cfgSet(IDFF_avisynthBufferBack,c);
         return TRUE;
        }
       break;

      case IDC_ED_AVISYNTH_BUF_AHEAD:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         c=GetDlgItemInt(m_hwnd,IDC_ED_AVISYNTH_BUF_AHEAD,NULL,false);
         cfgSet(IDFF_avisynthBufferAhead,c);
         return TRUE;
        }
       break;

      case IDC_ED_AVISYNTH:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         parent->setChange();
         return TRUE;
        }
       break;
     }
    break;
  }
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}

void TavisynthPage::onLoad(void)
{
 char_t scriptflnm[MAX_PATH]=_l("");
 if (dlgGetFile(false,m_hwnd,_(-IDD_AVISYNTH,_l("Load Avisynth script")),avs_mask,_l("avs"),scriptflnm,_l("."),0))
  {
   FILE *f=fopen(scriptflnm,_l("rb"));
   if (f)
    {
     char script[MAX_AVISYNTH_SCRIPT_LENGTH];
     size_t len=fread(script,1,MAX_AVISYNTH_SCRIPT_LENGTH,f);
     fclose(f);
     script[len]='\0';
     setDlgItemText(m_hwnd,IDC_ED_AVISYNTH,text<char_t>(script));
    }
  }
}

void TavisynthPage::onSave(void)
{
 char_t scriptflnm[MAX_PATH]=_l("");
 if (dlgGetFile(true,m_hwnd,_(-IDD_AVISYNTH,_l("Save Avisynth script")),avs_mask,_l("avs"),scriptflnm,_l("."),0))
  {
   FILE *f=fopen(scriptflnm,_l("wt"));
   if (f)
    {
     int linescnt=(int)SendDlgItemMessage(m_hwnd,IDC_ED_AVISYNTH,EM_GETLINECOUNT,0,0);
     for (int i=0;i<linescnt;i++)
      {
       char_t line[MAX_AVISYNTH_SCRIPT_LENGTH];
       WORD *lparam=(WORD*)line;
       *lparam=MAX_AVISYNTH_SCRIPT_LENGTH;
       LRESULT len=SendDlgItemMessage(m_hwnd,IDC_ED_AVISYNTH,EM_GETLINE,i,LPARAM(line));
       line[len]='\0';
       if (i>0) fputs("\n",f);
       fputs(text<char>(line),f);
      }
     fclose(f);
    }
  }
}

void TavisynthPage::onUseCurrent(void)
{
 // Using static methods of TimgFilterAvisynth was the best I could come up
 // with here - but I couldn't find a way to get to the current Tfilters
 // object... :( (Of course, since there currently can't be more than one
 // instance of TimgFilterAvisynth this works, but it's a bit fugly...)
 // -- Leak

 int maxBufferAhead=TimgFilterAvisynth::getMaxBufferAhead();
 int maxBufferBack=TimgFilterAvisynth::getMaxBufferBack();

 SetDlgItemInt(m_hwnd,IDC_ED_AVISYNTH_BUF_AHEAD,maxBufferAhead,false);
 SetDlgItemInt(m_hwnd,IDC_ED_AVISYNTH_BUF_BACK,maxBufferBack,false);
}

void TavisynthPage::applySettings(void)
{
 char_t script[MAX_AVISYNTH_SCRIPT_LENGTH];
 GetDlgItemText(m_hwnd,IDC_ED_AVISYNTH,script,MAX_AVISYNTH_SCRIPT_LENGTH);
 cfgSet(IDFF_avisynthScript,script);
}

void TavisynthPage::translate(void)
{
 TconfPageDecVideo::translate();

 cbxTranslate(IDC_CBX_AVISYNTH_PULLDOWN,pulldownModes);
}

TavisynthPage::TavisynthPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff)
{
 resInter=IDC_CHB_AVISYNTH;
 static const TbindCheckbox<TavisynthPage> chb[]=
  {
   IDC_CHB_AVISYNTH_FFDSHOW,IDFF_avisynthFfdshowSource,NULL,
   IDC_CHB_AVISYNTH_YV12,IDFF_avisynthInYV12,NULL,
   IDC_CHB_AVISYNTH_YUY2,IDFF_avisynthInYUY2,NULL,
   IDC_CHB_AVISYNTH_RGB32,IDFF_avisynthInRGB32,NULL,
   IDC_CHB_AVISYNTH_RGB24,IDFF_avisynthInRGB24,NULL,
   IDC_CHB_AVISYNTH_BUFFER,IDFF_avisynthEnableBuffering,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindButton<TavisynthPage> bt[]=
  {
   IDC_BT_AVISYNTH_LOAD,&TavisynthPage::onLoad,
   IDC_BT_AVISYNTH_SAVE,&TavisynthPage::onSave,
   IDC_BT_AVISYNTH_USECURRENT,&TavisynthPage::onUseCurrent,
   0,NULL
  };
 bindButtons(bt);
 static const TbindCombobox<TavisynthPage> cbx[]=
  {
   IDC_CBX_AVISYNTH_PULLDOWN,IDFF_avisynthApplyPulldown,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
}
