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
#include "Csubtitles.h"
#include "TsubtitlesFile.h"
#include "TsubtitlesSettings.h"
#include "TffdshowPageDec.h"
#include "Ttranslate.h"

void TsubtitlesPage::init(void)
{
 SendDlgItemMessage(m_hwnd,IDC_CBX_SUB_FLNM,CB_LIMITTEXT,MAX_PATH,0);
 edLimitText(IDC_ED_SUB_SEARCH_DIR,MAX_PATH);
 edLimitText(IDC_ED_SUB_SEARCH_EXT,MAX_PATH);

 tbrSetRange(IDC_TBR_SUB_POSX,0,100,10);
 tbrSetRange(IDC_TBR_SUB_POSY,0,100,10);
 tbrSetRange(IDC_TBR_SUB_STEREOSCOPICPAR,-100,100);

 autosubfirsttime=true;
 addHint(IDC_CHB_SUBTEXTPIN_SSA,_l("Very incomplete and experimental.\nSend me samples which don't work."));
 addHint(IDC_ED_SUB_SEARCH_EXT,_l("ffdshow searches subtitle files in the folders which are configured in the edit box above.\nFor video.avi, ffdshow searches video.utf, video.idx, video.sub,... and use the file which is found at the first time.\nEnumerate extensions in the order you like and separate them by semicolons.\n\nutf;idx;sub;srt;smi;rt;txt;ssa;aqt;mpl;usf is the default settings."));
 addHint(IDC_CHB_SUBEXTENDEDTAGS,_l("Experimental, may give garbled subtitles."));
 setFont(IDC_BT_SUBTITLES_EXPAND,parent->arrowsFont);
}

void TsubtitlesPage::cfg2dlg(void)
{
 sub2dlg();
 enable(filterMode&IDFF_FILTERMODE_PLAYER,IDC_BT_SUBTITLES_RESET);
 setCheck(IDC_CHB_SUB_WATCH,cfgGet(IDFF_subWatch));
 static const int idEmbedd[]={IDC_CHB_SUBTEXTPIN,IDC_CHB_SUBCC,IDC_CHB_SUBTEXTPIN_SSA,0};
 setCheck(IDC_CHB_SUBTEXTPIN,cfgGet(IDFF_subTextpin));
 setCheck(IDC_CHB_SUBCC,cfgGet(IDFF_subCC));
 setCheck(IDC_CHB_SUBTEXTPIN_SSA,cfgGet(IDFF_subTextpinSSA));
 setCheck(IDC_CHB_SUBEXTENDEDTAGS,cfgGet(IDFF_subExtendedTags));
 enable((filterMode&IDFF_FILTERMODE_VFW)==0,idEmbedd);
 stereo2dlg();
}
void TsubtitlesPage::sub2dlg(void)
{
 char_t s[260];int x;

 x=cfgGet(IDFF_subPosX);
 TsubtitlesSettings::getPosHoriz(x, s, this, IDC_LBL_SUB_POSX, countof(s));
 setDlgItemText(m_hwnd,IDC_LBL_SUB_POSX,s);
 tbrSet(IDC_TBR_SUB_POSX,x);

 x=cfgGet(IDFF_subPosY);
 TsubtitlesSettings::getPosVert(x, s, this, IDC_LBL_SUB_POSY, countof(s));
 setDlgItemText(m_hwnd,IDC_LBL_SUB_POSY,s);
 tbrSet(IDC_TBR_SUB_POSY,x);

 expand2dlg();
 cbxSetCurSel(IDC_CBX_SUBTITLES_ALIGN,cfgGet(IDFF_subAlign));

 const char_t *subfilename=cfgGetStr(IDFF_subFilename);
 if (SendDlgItemMessage(m_hwnd,IDC_CBX_SUB_FLNM,CB_SELECTSTRING,WPARAM(-1),LPARAM(subfilename))==CB_ERR)
  {
   SendDlgItemMessage(m_hwnd,IDC_CBX_SUB_FLNM,CB_INSERTSTRING,0,LPARAM(subfilename));
   cbxSetCurSel(IDC_CBX_SUB_FLNM,0);
  }
 int autoflnm=cfgGet(IDFF_subAutoFlnm);
 setCheck(IDC_RBT_SUB_SEARCHDIR,autoflnm);
 setCheck(IDC_RBT_SUB_FLNM,!autoflnm);

 setDlgItemText(m_hwnd,IDC_ED_SUB_SEARCH_DIR,cfgGetStr(IDFF_subSearchDir));
 setDlgItemText(m_hwnd,IDC_ED_SUB_SEARCH_EXT,cfgGetStr(IDFF_subSearchExt));

 SetDlgItemInt(m_hwnd,IDC_ED_SUB_DELAY ,cfgGet(IDFF_subDelay ),TRUE );
 SetDlgItemInt(m_hwnd,IDC_ED_SUB_SPEED ,cfgGet(IDFF_subSpeed ),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_SUB_SPEED2,cfgGet(IDFF_subSpeed2),FALSE);

 auto2dlg();
}
void TsubtitlesPage::auto2dlg(void)
{
 int a=getCheck(IDC_RBT_SUB_SEARCHDIR);
 static const int idFile[]={IDC_CBX_SUB_FLNM,IDC_BT_SUB_LOADFILE,0};
 enable(!a,idFile);
 setCheck(IDC_CHB_SUB_SEARCHHEURISTIC,cfgGet(IDFF_subSearchHeuristic));
 static const int idSearch[]={IDC_ED_SUB_SEARCH_DIR,IDC_BT_SUB_SEARCHDIR,IDC_CHB_SUB_SEARCHHEURISTIC,0};
 static const int idSearchExt[]={IDC_ED_SUB_SEARCH_EXT,IDC_TXT_SERCH_ORDER,0};
 enable(a,idSearch);
 int b=getCheck(IDC_CHB_SUB_SEARCHHEURISTIC);
 enable(a && !b,idSearchExt);
 if (a && (filterMode&IDFF_FILTERMODE_PLAYER))
  {
   const char_t *autosubflnm=deciV->findAutoSubflnm3();
/*
   if (autosubfirsttime)
    {
     autosubfirsttime=false;
     Tsubtitles::findPossibleSubtitles(deci->getSourceName(),autosubfiles);
    }
   if (!autosubfiles.empty())
    {
     //enable(1,IDC_CBX_SUB_FLNM);
     //SendDlgItemMessage(m_hwnd,IDC_CBX_SUB_FLNM,CB_RESETCONTENT,0,0);
     //for (strings::const_iterator f=autosubfiles.begin();f!=autosubfiles.end();f++)
     // cbxAdd(IDC_CBX_SUB_FLNM,f->c_str());
     */
     setDlgItemText(m_hwnd,IDC_CBX_SUB_FLNM,autosubflnm);
    //}
  }
}
void TsubtitlesPage::stereo2dlg(void)
{
 static const int idStereo[]={IDC_LBL_SUB_STEREOSCOPICPAR,IDC_TBR_SUB_STEREOSCOPICPAR,0};
 int is=cfgGet(IDFF_subStereoscopic);
 setCheck(IDC_CHB_SUB_STEREOSCOPIC,is);
 tbrSet(IDC_TBR_SUB_STEREOSCOPICPAR,cfgGet(IDFF_subStereoscopicPar),IDC_LBL_SUB_STEREOSCOPICPAR,NULL,10.0f);
 enable(is,idStereo);
}
void TsubtitlesPage::expand2dlg(void)
{
 int isExpand=cfgGet(IDFF_subIsExpand);
 setCheck(IDC_CHB_SUBTITLES_EXPAND,isExpand);
 static const int idExpand[]={IDC_ED_SUBTITLES_EXPAND_X,IDC_LBL_SUBTITLES_EXPAND2,IDC_ED_SUBTITLES_EXPAND_Y,IDC_BT_SUBTITLES_EXPAND,0};
 enable(isExpand,idExpand);
 int e1,e2;TsubtitlesSettings::getExpand(1,cfgGet(IDFF_subExpand),&e1,&e2);
 if (e1==0 && e2==0)
  {
   e1=4;e2=3;
   cfgSet(IDFF_subExpand,1);
  }
 SetDlgItemInt(m_hwnd,IDC_ED_SUBTITLES_EXPAND_X,e1,FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_SUBTITLES_EXPAND_Y,e2,FALSE);
}

void TsubtitlesPage::applySettings(void)
{
 loadSubtitles();
}

void TsubtitlesPage::loadSubtitles(void)
{
 if (!cfgGet(IDFF_subAutoFlnm))
  {
   char_t subflnm[1024];
   GetDlgItemText(m_hwnd,IDC_CBX_SUB_FLNM,subflnm,1023);
   cfgSet(IDFF_subFilename,subflnm);
  }
 sub2dlg();
}

INT_PTR TsubtitlesPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CBX_SUB_FLNM:
       if (HIWORD(wParam)==CBN_SELCHANGE || HIWORD(wParam)==CBN_EDITUPDATE)
        parent->setChange();
       return TRUE;
      case IDC_BT_SUBTITLES_RESET:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         deciV->resetSubtitleTimes();
         return TRUE;
        }
       break;
      case IDC_ED_SUB_SEARCH_DIR:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t sdir[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_SUB_SEARCH_DIR,sdir,MAX_PATH);
         cfgSet(IDFF_subSearchDir,sdir);
         return TRUE;
        }
       break;
      case IDC_ED_SUB_SEARCH_EXT:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t sext[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_SUB_SEARCH_EXT,sext,MAX_PATH);
         cfgSet(IDFF_subSearchExt,sext);
         return TRUE;
        }
       break;
      case IDC_BT_SUB_LOAD:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         deci->notifyParamStr(IDFF_subFilename,_l(""));
         return TRUE;
        }
       break;
      case IDC_ED_SUBTITLES_EXPAND_X:
      case IDC_ED_SUBTITLES_EXPAND_Y:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         HWND hed=GetDlgItem(m_hwnd,LOWORD(wParam));
         if (hed!=GetFocus()) return FALSE;
         repaint(hed);
         int x,y;
         if (eval(GetDlgItem(m_hwnd,IDC_ED_SUBTITLES_EXPAND_X),1,10000,&x) && eval(GetDlgItem(m_hwnd,IDC_ED_SUBTITLES_EXPAND_Y),1,10000,&y))
          {
           cfgSet(IDFF_subExpand,MAKELONG(y,x));
           return S_OK;
          }
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
       case IDC_ED_SUBTITLES_EXPAND_X:
       case IDC_ED_SUBTITLES_EXPAND_Y:ok=eval(hwnd,1,10000);break;
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
 return TconfPageDecVideo::msgProc(uMsg,wParam,lParam);
}
void TsubtitlesPage::onLoadfile(void)
{
 char_t subflnm[MAX_PATH]=_l("");
 if (dlgGetFile(false,m_hwnd,_(-IDD_SUBTITLES,_l("Load subtitles file")),TsubtitlesFile::mask,_l("txt"),subflnm,_l("."),0))
  {
   setDlgItemText(m_hwnd,IDC_CBX_SUB_FLNM,subflnm);
   parent->setChange();
  }
}
void TsubtitlesPage::onSearchdir(void)
{
 char_t dir[MAX_PATH]=_l("");
 if (dlgGetDir(m_hwnd,dir,_(-IDD_SUBTITLES,_l("Select directory where subtitles are stored"))))
  {
   char_t sdir[2*MAX_PATH];cfgGet(IDFF_subSearchDir,sdir,2*MAX_PATH);
   strncat_s(sdir, countof(sdir), _l(";"), _TRUNCATE);
   strncat_s(sdir, countof(sdir), dir, _TRUNCATE);
   cfgSet(IDFF_subSearchDir,sdir);
   sub2dlg();
  }
}
void TsubtitlesPage::onExpandClick(void)
{
 static const char_t *letterboxes[]=
  {
   _l("4:3"),
   _l("5:4"),
   _l("16:9"),
   NULL
  };
 int cmd=selectFromMenu(letterboxes,IDC_BT_SUBTITLES_EXPAND,false);
 int x,y;
 switch (cmd)
  {
   case 0:x=4;y=3;break;
   case 1:x=5;y=4;break;
   case 2:x=16;y=9;break;
   default:return;
  }
 cfgSet(IDFF_subExpand,MAKELONG(y,x));
 expand2dlg();
}

void TsubtitlesPage::translate(void)
{
 TconfPageBase::translate();

 cbxTranslate(IDC_CBX_SUBTITLES_ALIGN,TsubtitlesSettings::alignments);
}

TsubtitlesPage::TsubtitlesPage(TffdshowPageDec *Iparent,const TfilterIDFF *idff):TconfPageDecVideo(Iparent,idff,1)
{
 resInter=IDC_CHB_SUBTITLES;
 helpURL=_l("subtitles.html");
 static const TbindCheckbox<TsubtitlesPage> chb[]=
  {
   IDC_CHB_SUB_WATCH,IDFF_subWatch,NULL,
   IDC_CHB_SUB_STEREOSCOPIC,IDFF_subStereoscopic,&TsubtitlesPage::stereo2dlg,
   IDC_CHB_SUBTEXTPIN,IDFF_subTextpin,NULL,
   IDC_CHB_SUB_SEARCHHEURISTIC,IDFF_subSearchHeuristic,&TsubtitlesPage::auto2dlg,
   IDC_CHB_SUBTITLES_EXPAND,IDFF_subIsExpand,&TsubtitlesPage::expand2dlg,
   IDC_CHB_SUBCC,IDFF_subCC,&TsubtitlesPage::cfg2dlg,
   IDC_CHB_SUBTEXTPIN_SSA,IDFF_subTextpinSSA,NULL,
   IDC_CHB_SUBEXTENDEDTAGS,IDFF_subExtendedTags,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TsubtitlesPage> htbr[]=
  {
   IDC_TBR_SUB_POSX,IDFF_subPosX,&TsubtitlesPage::sub2dlg,
   IDC_TBR_SUB_POSY,IDFF_subPosY,&TsubtitlesPage::sub2dlg,
   IDC_TBR_SUB_STEREOSCOPICPAR,IDFF_subStereoscopicPar,&TsubtitlesPage::stereo2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindRadiobutton<TsubtitlesPage> rbt[]=
  {
   IDC_RBT_SUB_SEARCHDIR,IDFF_subAutoFlnm,1,&TsubtitlesPage::sub2dlg,
   IDC_RBT_SUB_FLNM,IDFF_subAutoFlnm,0,&TsubtitlesPage::sub2dlg,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindEditInt<TsubtitlesPage> edInt[]=
  {
   IDC_ED_SUB_DELAY ,-INT_MAX/2,INT_MAX/2,IDFF_subDelay,NULL,
   IDC_ED_SUB_SPEED ,1,INT_MAX/2,IDFF_subSpeed ,NULL,
   IDC_ED_SUB_SPEED2,1,INT_MAX/2,IDFF_subSpeed2,NULL,
   0
  };
 bindEditInts(edInt);
 static const TbindCombobox<TsubtitlesPage> cbx[]=
  {
   IDC_CBX_SUBTITLES_ALIGN,IDFF_subAlign,BINDCBX_SEL,NULL,
   0
  };
 bindComboboxes(cbx);
 static const TbindButton<TsubtitlesPage> bt[]=
  {
   IDC_BT_SUB_LOADFILE,&TsubtitlesPage::onLoadfile,
   IDC_BT_SUB_SEARCHDIR,&TsubtitlesPage::onSearchdir,
   IDC_BT_SUBTITLES_EXPAND,&TsubtitlesPage::onExpandClick,
   0,NULL
  };
 bindButtons(bt);
}
