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
#include "COSD.h"
#include "TsubtitlesSettings.h"
#include "Tsubreader.h"
#include "TffdshowPageDec.h"
#include "IffdshowDecVideo.h"

//=================================== TOSDpageDec ==================================
void TOSDpageDec::init(void)
{
 tbrSetRange(IDC_TBR_OSD_POSX,0,100,10);
 tbrSetRange(IDC_TBR_OSD_POSY,0,100,10);

 setFont(IDC_BT_OSD_PRESETS  ,parent->arrowsFont);
 setFont(IDC_BT_OSD_LINE_UP  ,parent->arrowsFont);
 setFont(IDC_BT_OSD_LINE_DOWN,parent->arrowsFont);

 hlv=GetDlgItem(m_hwnd,IDC_LV_OSD_LINES);
 ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
 int ncol=0;
 RECT r;
 GetWindowRect(hlv,&r);
 ListView_AddCol(hlv,ncol,r.right-r.left-26,_l("OSD item"),false);

 edLimitText(IDC_ED_OSD_USER,240);
 dragitem=-1;
 CRect rp;GetWindowRect(m_hwnd,&rp);
 CRect rc;GetWindowRect(hlv,&rc);
 lvx=rc.left-rp.left;lvy=rc.top-rp.top;

 edLimitText(IDC_ED_OSD_SAVE,MAX_PATH);
 cbxAdd(IDC_CBX_OSD_USERFORMAT, _l("HTML"), Tsubreader::SUB_SUBVIEWER);
 cbxAdd(IDC_CBX_OSD_USERFORMAT, _l("SSA"), Tsubreader::SUB_SSA);
}

void TOSDpageDec::cfg2dlg(void)
{
 pos2dlg();
 osds2dlg();
 int format = cfgGet(IDFF_OSD_userformat);
 if (cbxSetDataCurSel(IDC_CBX_OSD_USERFORMAT,format)==CB_ERR)
  {
   format=(int)cbxGetItemData(IDC_CBX_OSD_USERFORMAT,0);
   cfgSet(IDFF_OSD_userformat,format);
   cbxSetCurSel(IDC_CBX_OSD_USERFORMAT,0);
  }
}

void TOSDpageDec::pos2dlg(void)
{
 char_t s[260];int x;

 x=cfgGet(IDFF_OSDposX);
 TsubtitlesSettings::getPosHoriz(x, s, this, IDC_LBL_OSD_POSX, countof(s));
 setDlgItemText(m_hwnd,IDC_LBL_OSD_POSX,s);
 tbrSet(IDC_TBR_OSD_POSX,x);

 x=cfgGet(IDFF_OSDposY);
 TsubtitlesSettings::getPosVert(x, s, this, IDC_LBL_OSD_POSY, countof(s));
 setDlgItemText(m_hwnd,IDC_LBL_OSD_POSY,s);
 tbrSet(IDC_TBR_OSD_POSY,x);
}

void TOSDpageDec::osds2dlg(void)
{
 nostate=true;
 osdslabels.clear();
 cbxClear(IDC_CBX_OSD_PRESETS);
 int cnt=deciD->getOSDpresetCount2();
 const char_t *startpresetname=cfgGetStr(IDFF_OSDstartPreset);
 for (int i=0;i<cnt;i++)
  {
   const char_t *presetname=deciD->getOSDpresetName2(i);
   if (startpresetname && strcmp(startpresetname,presetname)==0)
    osdslabels.push_back(ffstring(presetname)+ffstring(_(IDC_CBX_OSD_PRESETS,_l(" (show on startup)"))));
   else
    osdslabels.push_back(presetname);
   cbxAdd(IDC_CBX_OSD_PRESETS,osdslabels.back().c_str(),intptr_t(presetname));
  }
 const char_t *curpreset=cfgGetStr(IDFF_OSDcurPreset);
 for (int i=0;i<cnt;i++)
  if (strcmp(curpreset,(const char_t*)cbxGetItemData(IDC_CBX_OSD_PRESETS,i))==0)
   {
    cbxSetCurSel(IDC_CBX_OSD_PRESETS,i);
    break;
   }
 osd2dlg();
 nostate=false;
}

int CALLBACK TOSDpageDec::osdsSort(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)
{
 TOSDpageDec *self=(TOSDpageDec*)lParamSort;
 return std::find(self->osds.begin(),self->osds.end(),int(lParam1))>std::find(self->osds.begin(),self->osds.end(),int(lParam2));
}

void TOSDpageDec::osd2dlg(void)
{
 const char_t *osdsStr=deciD->getOSDpresetFormat2(NULL);
 if (strncmp(osdsStr,_l("user"),4)==0)
  {
   ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_CHECKBOXES,0);
   setCheck(IDC_CHB_OSD_USER,1);enable(1,IDC_ED_OSD_USER);
   setText(IDC_ED_OSD_USER,_l("%s"),osdsStr+4);
   user=true;
  }
 else
  {
   ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_CHECKBOXES,LVS_EX_CHECKBOXES);
   setCheck(IDC_CHB_OSD_USER,0);enable(0,IDC_ED_OSD_USER);
   strtok(osdsStr,_l(" "),osds);
   int cnt=ListView_GetItemCount(hlv);
   nostate=true;
   for (int j=0;j<cnt;j++)
    ListView_SetCheckState(hlv,j,FALSE);
   nostate=false;
   for (ints::const_iterator i=osds.begin();i!=osds.end();i++)
    checkOSDline(*i,true);
   ListView_SortItems(hlv,osdsSort,LPARAM(this));
   user=false;
  }
 save2dlg();
}

void TOSDpageDec::save2dlg(void)
{
 int is=cfgGet(IDFF_OSDisSave);
 setCheck(IDC_CHB_OSD_SAVE,is);
 int isUser=getCheck(IDC_CHB_OSD_USER);
 enable(!isUser,IDC_CHB_OSD_SAVE);
 setDlgItemText(m_hwnd,IDC_ED_OSD_SAVE,cfgGetStr(IDFF_OSDsaveFlnm));
 is&=!isUser;
 static const int idSave[]={IDC_ED_OSD_SAVE,IDC_BT_OSD_SAVE,0};
 enable(is,idSave);
}

void TOSDpageDec::checkOSDline(int idff,bool check)
{
 nostate=true;
 LVFINDINFO lvfi;memset(&lvfi,0,sizeof(lvfi));
 lvfi.flags=LVFI_PARAM;
 lvfi.lParam=idff;
 int i=ListView_FindItem(hlv,-1,&lvfi);
 if (i!=-1) ListView_SetCheckState(hlv,i,check);
 nostate=false;
}
void TOSDpageDec::lv2osdFormat(void)
{
 char_t format[256]=_l("");
 int cnt=ListView_GetItemCount(hlv);
 for (int i=0;i<cnt;i++)
  if (ListView_GetCheckState(hlv,i))
   strncatf(format, countof(format), _l("%i "), lvGetItemParam(IDC_LV_OSD_LINES,i));
 if (format[strlen(format)-1]==' ') format[strlen(format)-1]='\0';
 deciD->setOSDpresetFormat(NULL,format);
 parent->setChange();
}

int TOSDpageDec::findPreset(const char_t *presetname)
{
 int cnt=cbxGetItemCount(IDC_CBX_OSD_PRESETS);
 for (int i=0;i<cnt;i++)
  if (stricmp((const char_t*)cbxGetItemData(IDC_CBX_OSD_PRESETS,i),presetname)==0)
   return i;
 return CB_ERR;
}

INT_PTR TOSDpageDec::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_OSD:
       cfgSet(IDFF_isOSD,getCheck(IDC_CHB_OSD));
       parent->drawInter();
       return TRUE;
      case IDC_CHB_OSD_USER:
       if (!getCheck(IDC_CHB_OSD_USER))
        lv2osdFormat();
       else
        deciD->setOSDpresetFormat(NULL,_l("user"));
       osd2dlg();
       parent->setChange();
       break;
      case IDC_ED_OSD_USER:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t ed[250];
         GetDlgItemText(m_hwnd,IDC_ED_OSD_USER,ed,250);
         char_t format[256];
         tsnprintf_s(format, 256, _TRUNCATE, _l("user%s"),ed);
         deciD->setOSDpresetFormat(NULL,format);
         parent->setChange();
        };
       break;
      case IDC_ED_OSD_SAVE:
       if (HIWORD(wParam)==EN_CHANGE && !isSetWindowText)
        {
         char_t saveflnm[MAX_PATH];
         GetDlgItemText(m_hwnd,IDC_ED_OSD_SAVE,saveflnm,MAX_PATH);
         cfgSet(IDFF_OSDsaveFlnm, saveflnm);
         return TRUE;
        }
       return TRUE;
     }
    break;
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (!nostate && nmhdr->hwndFrom==hlv && nmhdr->idFrom==IDC_LV_OSD_LINES)
      switch (nmhdr->code)
       {
        case LVN_ITEMCHANGED:
         {
          LPNMLISTVIEW nmlv=LPNMLISTVIEW(lParam);
          if (nmlv->uChanged&LVIF_STATE && ((nmlv->uOldState&4096)!=(nmlv->uNewState&4096)))
           lv2osdFormat();
          return TRUE;
         }
        case LVN_BEGINDRAG:
         {
          if (!user)
           {
            LPNMLISTVIEW nmlv=LPNMLISTVIEW(lParam);
            if (nmlv->iItem!=-1)
             {
              dragitem=nmlv->iItem;
              SetCapture(m_hwnd);
             }
           }
          break;
         }
        case NM_DBLCLK:
         {
          if (user)
           {
            LPNMITEMACTIVATE nmia=LPNMITEMACTIVATE(lParam);
            if (nmia->iItem!=-1)
             {
              const char_t *shortcut=deci->getInfoItemShortcut((int)lvGetItemParam(IDC_LV_OSD_LINES,nmia->iItem));
              if (shortcut && shortcut[0])
               {
                char_t osd[240];
                tsnprintf_s(osd, countof(osd), _TRUNCATE, _l("%%%s"), shortcut);
                SendDlgItemMessage(m_hwnd,IDC_ED_OSD_USER,EM_REPLACESEL,TRUE,LPARAM(osd));
               }
             }
           }
          break;
         }
       }
     break;
    }
   case WM_MOUSEMOVE:
    if (dragitem!=-1)
     {
      LVHITTESTINFO lvhti;
      lvhti.pt.x=LOWORD(lParam)-lvx;
      lvhti.pt.y=HIWORD(lParam)-lvy;
      int target=ListView_HitTest(hlv,&lvhti);
      if (target!=-1)
       {
        lvSwapItems(IDC_LV_OSD_LINES,target,dragitem);
        lv2osdFormat();
        dragitem=target;
       }
      return TRUE;
     }
    break;
   case WM_LBUTTONUP:
    if (dragitem!=-1)
     {
      dragitem=-1;
      ReleaseCapture();
      return TRUE;
     }
    break;
  }
 return TconfPageDec::msgProc(uMsg,wParam,lParam);
}
void TOSDpageDec::onLineUp(void)
{
 int ii=lvGetSelItem(IDC_LV_OSD_LINES);
 if (ii>=1)
  {
   lvSwapItems(IDC_LV_OSD_LINES,ii,ii-1);
   lv2osdFormat();
  }
}
void TOSDpageDec::onLineDown(void)
{
 int ii=lvGetSelItem(IDC_LV_OSD_LINES);
 if (ii!=-1 && ii<ListView_GetItemCount(hlv)-1)
  {
   lvSwapItems(IDC_LV_OSD_LINES,ii,ii+1);
   lv2osdFormat();
  }
}
void TOSDpageDec::onPresets(void)
{
 int ii=cbxGetCurSel(IDC_CBX_OSD_PRESETS);
 const char_t *curPresetName=(const char_t*)cbxGetCurItemData(IDC_CBX_OSD_PRESETS);
 const char_t *startupPresetName=cfgGetStr(IDFF_OSDstartPreset);
 enum
  {
   IDC_MNI_OSD_PRESETS_NEW       =6000,
   IDC_MNI_OSD_PRESETS_SAVEAS_NEW,
   IDC_MNI_OSD_PRESETS_RENAME,
   IDC_MNI_OSD_PRESETS_DELETE,
   IDC_MNI_OSD_PRESETS_ONSTARTUP,
   IDC_MNI_OSD_PRESETS_SAVEAS,
   IDC_MNI_OSD_PRESETS_SAVEONLY
  };
 HMENU hm=CreatePopupMenu(),hmSaveAs=NULL;
 int ord=0;
 insertMenuItem(hm,ord,IDC_MNI_OSD_PRESETS_NEW,_l("New"),false);
 int size=deciD->getOSDpresetCount2();
 if (size>1)
  {
   hmSaveAs=CreatePopupMenu();
   int ord2=0;
   for (int i=0;i<size;i++)
    {
     const char_t *name2=deciD->getOSDpresetName2(i);
     if (stricmp(curPresetName,name2)!=0)
      insertMenuItem(hmSaveAs,ord2,IDC_MNI_OSD_PRESETS_SAVEAS+i,name2,false);
    }
   insertSeparator(hmSaveAs,ord2);
   insertMenuItem(hmSaveAs,ord2,IDC_MNI_OSD_PRESETS_SAVEAS_NEW,_l("New preset..."),false);
   insertSubmenu(hm,ord,_l("Save preset to..."),hmSaveAs);
  }
 else
  insertMenuItem(hm,ord,IDC_MNI_OSD_PRESETS_SAVEAS_NEW,_l("Save as..."),false);
 insertMenuItem(hm,ord,IDC_MNI_OSD_PRESETS_RENAME,_l("Rename preset..."),false);enable(hm,2,ii!=0);
 insertMenuItem(hm,ord,IDC_MNI_OSD_PRESETS_DELETE,_l("Delete preset"),false);enable(hm,3,ii!=0);
 insertSeparator(hm,ord);
 insertMenuItem(hm,ord,IDC_MNI_OSD_PRESETS_ONSTARTUP,_l("Show on startup"),stricmp(startupPresetName,curPresetName)==0);
 if (cfgGet(IDFF_OSDisSave))
  {
   insertSeparator(hm,ord);
   insertMenuItem(hm,ord,IDC_MNI_OSD_PRESETS_SAVEONLY,_l("Don't display, only save data to file"),!!cfgGet(IDFF_OSDsaveOnly));
  }
 RECT r;
 GetWindowRect(GetDlgItem(m_hwnd,IDC_BT_OSD_PRESETS),&r);
 POINT p={0,r.bottom-r.top};
 ClientToScreen(GetDlgItem(m_hwnd,IDC_BT_OSD_PRESETS),&p);
 int cmd=TrackPopupMenu(_(hm),TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,p.x,p.y,0,m_hwnd,NULL);
 PostMessage(m_hwnd,WM_NULL,0,0);
 if (cmd!=0)
  if (cmd==IDC_MNI_OSD_PRESETS_SAVEONLY)
   cfgSet(IDFF_OSDsaveOnly,1-cfgGet(IDFF_OSDsaveOnly));
  else if (cmd>=IDC_MNI_OSD_PRESETS_SAVEAS)
   {
    char_t format[256];
    ff_strncpy(format, deciD->getOSDpresetFormat2(curPresetName), countof(format));
    char_t newPreset[40];GetMenuString(hmSaveAs,cmd,newPreset,40,MF_BYCOMMAND);
    deciD->setOSDpresetFormat(newPreset,format);
   }
  else
   switch (cmd)
    {
     case IDC_MNI_OSD_PRESETS_RENAME:
      {
       char_t newPresetName[40];
       ff_strncpy(newPresetName, curPresetName, countof(newPresetName));
       if (inputString(_(-IDD_OSD,_l("New preset name")),_(-IDD_OSD,_l("Enter new preset name")),newPresetName,40))
        if (newPresetName[0] && findPreset(newPresetName)==CB_ERR)
         {
          deciD->setOSDpresetName(ii,newPresetName);
          cfgSet(IDFF_OSDcurPreset,newPresetName);
          if (strcmp(startupPresetName,curPresetName)==0)
           cfgSet(IDFF_OSDstartPreset,newPresetName);
          osds2dlg();
         }
        else
         err(_(-IDD_OSD,_l("Preset already exists")));
       break;
      }
     case IDC_MNI_OSD_PRESETS_SAVEAS_NEW:
      {
       char_t newPresetName[40];
       ff_strncpy(newPresetName,curPresetName,countof(newPresetName));
       if (inputString(_(-IDD_OSD,_l("New preset name")),_(-IDD_OSD,_l("Enter new preset name")),newPresetName,40))
        if (newPresetName[0] && findPreset(newPresetName)==CB_ERR)
         {
          char_t format[256];
          ff_strncpy(format,deciD->getOSDpresetFormat2(curPresetName),countof(format));
          deciD->addOSDpreset(newPresetName,format);
          cfgSet(IDFF_OSDcurPreset,newPresetName);
          osds2dlg();
         }
        else
         err(_(-IDD_OSD,_l("Preset already exists")));
       break;
      }
     case IDC_MNI_OSD_PRESETS_DELETE:
      if (MessageBox(m_hwnd,_(-IDD_OSD,_l("Do you really want to delete current OSD preset?")),_(-IDD_OSD,_l("Delete preset")),MB_ICONQUESTION|MB_YESNO)==IDYES)
       {
        deciD->deleteOSDpreset(curPresetName);
        cfgSet(IDFF_OSDcurPreset,_l("default"));
        if (strcmp(curPresetName,startupPresetName)==0)
         cfgSet(IDFF_OSDstartPreset,_l(""));
        osds2dlg();
       }
      break;
     case IDC_MNI_OSD_PRESETS_NEW:
      {
       char_t newPresetName[40]=_l("");
       if (inputString(_(-IDD_OSD,_l("New preset name")),_(-IDD_OSD,_l("Enter new preset name")),newPresetName,40))
        if (newPresetName[0] && findPreset(newPresetName)==CB_ERR)
         {
          deciD->addOSDpreset(newPresetName,_l(""));
          cfgSet(IDFF_OSDcurPreset,newPresetName);
          osds2dlg();
         }
        else
         err(_(-IDD_OSD,_l("Preset already exists")));
       break;
      }
     case IDC_MNI_OSD_PRESETS_ONSTARTUP:
      {
       if (stricmp(startupPresetName,curPresetName)==0)
        {
         cfgSet(IDFF_OSDstartPreset,_l(""));
         osds2dlg();
        }
       else
        {
         char_t durationS[20];_itoa(cfgGet(IDFF_OSDstartDuration),durationS,10);
         if (inputString(_(-IDD_OSD,_l("Startup OSD preset")),_(-IDD_OSD,_l("Number of frames to show preset")),durationS,20))
          {
           cfgSet(IDFF_OSDstartPreset,curPresetName);
           deci->putParam(IDFF_OSDstartDuration,atoi(durationS));
           osds2dlg();
          }
        }
       break;
      }
    }
 DestroyMenu(hm);
}
void TOSDpageDec::onSave(void)
{
 char_t flnm[MAX_PATH];cfgGet(IDFF_OSDsaveFlnm,flnm,MAX_PATH);
 if (dlgGetFile(true,m_hwnd,_(-IDD_OSD,_l("File where to write statistics")),_l("Comma delimited (*.csv)\0*.csv\0"),_l("csv"),flnm,_l("."),0))
  {
   setDlgItemText(m_hwnd,IDC_ED_OSD_SAVE,flnm);
   cfgSet(IDFF_OSDsaveFlnm,flnm);
  }
}

void TOSDpageDec::applySettings(void)
{
 char_t flnm[MAX_PATH];
 GetDlgItemText(m_hwnd,IDC_ED_OSD_SAVE,flnm,MAX_PATH);
 cfgSet(IDFF_OSDsaveFlnm,flnm);
}

void TOSDpageDec::translate(void)
{
 TconfPageDec::translate();

 nostate=true;
 ListView_DeleteAllItems(hlv);
 for (int i=0;;i++)
  {
   LVITEM lvi;memset(&lvi,0,sizeof(lvi));
   lvi.mask=LVIF_TEXT|LVIF_PARAM;
   lvi.iItem=i+1;
   const char_t *text;
   if (deci->getInfoItem(i,(int*)&lvi.lParam,&text)!=S_OK)
    break;
   lvi.pszText=const_cast<char_t*>(_(IDC_LV_OSD_LINES,text));
   ListView_InsertItem(hlv,&lvi);
  }
 nostate=false;
}

TOSDpageDec::TOSDpageDec(TffdshowPageDec *Iparent):TconfPageDec(Iparent,NULL,0)
{
 dialogId=IDD_OSD;
 idffInter=IDFF_isOSD;resInter=IDC_CHB_OSD;
 static const TbindCheckbox<TOSDpageDec> chb[]=
  {
   IDC_CHB_OSD_SAVE,IDFF_OSDisSave,&TOSDpageDec::save2dlg,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindTrackbar<TOSDpageDec> htbr[]=
  {
   IDC_TBR_OSD_POSX,IDFF_OSDposX,&TOSDpageDec::pos2dlg,
   IDC_TBR_OSD_POSY,IDFF_OSDposY,&TOSDpageDec::pos2dlg,
   0,0,NULL
  };
 bindHtracks(htbr);
 static const TbindCombobox<TOSDpageDec> cbx[]=
  {
   IDC_CBX_OSD_PRESETS,IDFF_OSDcurPreset,BINDCBX_DATATEXT,&TOSDpageDec::osd2dlg,
   IDC_CBX_OSD_USERFORMAT, IDFF_OSD_userformat, BINDCBX_DATA, &TOSDpageDec::cfg2dlg,
   0
  };
 bindComboboxes(cbx);
 static const TbindButton<TOSDpageDec> bt[]=
  {
   IDC_BT_OSD_LINE_UP,&TOSDpageDec::onLineUp,
   IDC_BT_OSD_LINE_DOWN,&TOSDpageDec::onLineDown,
   IDC_BT_OSD_PRESETS,&TOSDpageDec::onPresets,
   IDC_BT_OSD_SAVE,&TOSDpageDec::onSave,
   0,NULL
  };
 bindButtons(bt);
}

//================================ TOSDpageDecVideo ================================
bool TOSDpageVideo::reset(bool testonly)
{
 if (!testonly)
  comptrQ<IffdshowDecVideo>(deciD)->resetOSD();
 return true;
}

//================================ TOSDpageDecAudio ================================
void TOSDpageAudio::init(void)
{
 TOSDpageDec::init();

 static const int idPos[]={IDC_LBL_OSD_POSX,IDC_TBR_OSD_POSX,IDC_LBL_OSD_POSY,IDC_TBR_OSD_POSY,0};
 enable(0,idPos);
}
