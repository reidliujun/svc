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
#include "Cpresets.h"
#include "TpresetSettings.h"
#include "TffdshowPageBase.h"
#include "Tpresets.h"
#include "TpresetAutoload.h"
#include "TffdshowPageDec.h"

//=========================== TpresetsPage ============================
bool TpresetsPage::enabled(void)
{
 return (filterMode&IDFF_FILTERMODE_PROC)==0;
}

void TpresetsPage::init(void)
{
 nochange=true;
 enable();
 hlv=GetDlgItem(m_hwnd,IDC_LV_PRESETS);

 italicFont=boldFont=NULL;
 int ncol=0;

 ListView_SetExtendedListViewStyleEx(hlv,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
 ListView_AddCol(hlv,ncol,cfgGet(IDFF_lvWidth0),_l("Preset name"),false);

 setFont(IDC_BT_PRESET_NEW_MENU   ,parent->arrowsFont);
 setFont(IDC_BT_PRESET_RENAME_MENU,parent->arrowsFont);

 nochange=false;
}

void TpresetsPage::cfg2dlg(void)
{
 ListView_SetItemCountEx(hlv,parent->localPresets->size(),0);
 int ii=(int)SendMessage(GetDlgItem(parent->m_hwnd,IDC_CBX_PRESETS),CB_GETCURSEL,0,0);
 lvSetSelItem(IDC_LV_PRESETS,ii);
 enable(ii!=0 && wndEnabled,IDC_BT_PRESET_REMOVE);
 ListView_EnsureVisible(hlv,ii,FALSE);
 setCheck(IDC_CHB_AUTOPRESET,cfgGet(IDFF_autoPreset));
 setCheck(IDC_CHB_AUTOPRESET_FILEFIRST,cfgGet(IDFF_autoPresetFileFirst));
 autopreset2dlg();
}
void TpresetsPage::autopreset2dlg(void)
{
 enable(getCheck(IDC_CHB_AUTOPRESET),IDC_CHB_AUTOPRESET_FILEFIRST);
}

INT_PTR TpresetsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    cfgSet(IDFF_lvWidth0,ListView_GetColumnWidth(hlv,0));
    if (italicFont) DeleteObject(italicFont);italicFont=NULL;
    if (boldFont) DeleteObject(boldFont);boldFont=NULL;
    break;
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (nmhdr->hwndFrom==hlv && nmhdr->idFrom==IDC_LV_PRESETS)
      switch (nmhdr->code)
       {
        case LVN_GETDISPINFO:
         {
          NMLVDISPINFO *nmdi=(NMLVDISPINFO*)lParam;
          int i=nmdi->item.iItem;
          if (i==-1) break;
          if (nmdi->item.mask&LVIF_TEXT)
           strcpy(nmdi->item.pszText,(*parent->localPresets)[i]->presetName);
          return TRUE;
         }
        case LVN_ITEMCHANGED:
         {
          if (nochange) break;
          NMLISTVIEW *nmlv=LPNMLISTVIEW(lParam);
          if (nmlv->iItem==-1) return TRUE;
          char_t presetName[1024];
          ListView_GetItemText(hlv,nmlv->iItem,0,presetName,1023);
          applying=true;
          parent->selectPreset(presetName);
          cfg2dlg();
          applying=false;
          return TRUE;
         }
        case LVN_KEYDOWN:
         {
          NMLVKEYDOWN *nmkd=(NMLVKEYDOWN*)lParam;
          switch (nmkd->wVKey)
           {
            case VK_F2:
             ListView_EditLabel(hlv,ListView_GetNextItem(hlv,-1,LVNI_SELECTED));
             return TRUE;
            case VK_SPACE:
             changePresetState();
             return TRUE;
           }
          break;
         }
        case NM_DBLCLK:
        case NM_CLICK:
         {
          NMITEMACTIVATE *nmia=LPNMITEMACTIVATE(lParam);
          if (nmia->iItem==-1)
           {
            char_t activePresetName[MAX_PATH];
            deciD->getActivePresetName(activePresetName, countof(activePresetName));
            applying=true;
            parent->selectPreset(activePresetName);
            cfg2dlg();
            applying=false;
           }
          else if (nmhdr->code==NM_DBLCLK)
           changePresetState();
          return TRUE;
         }
        case LVN_ENDLABELEDIT:
         {
          NMLVDISPINFO *nmdi=(NMLVDISPINFO*)lParam;
          if (!nmdi->item.pszText) return FALSE;
          if (parent->localPresets->getPreset(nmdi->item.pszText,false)==NULL && Tpreset::isValidPresetName(nmdi->item.pszText))
           {
            setDlgResult(TRUE);
            ffstring presetName;
            Tpreset::normalizePresetName(presetName,nmdi->item.pszText);
            renamePreset(presetName.c_str());
           }
          else
           setDlgResult(FALSE);
         }
        case NM_CUSTOMDRAW:
         {
          NMLVCUSTOMDRAW *lvcd=LPNMLVCUSTOMDRAW(lParam);
          if (lvcd->nmcd.dwDrawStage==CDDS_PREPAINT)
           {
            setDlgResult(CDRF_NOTIFYITEMDRAW);
            return TRUE;
           }
          if (lvcd->nmcd.dwDrawStage==CDDS_ITEMPREPAINT)
           {
            if (!italicFont)
             {
              LOGFONT oldFont;
              HFONT hf=(HFONT)GetCurrentObject(lvcd->nmcd.hdc,OBJ_FONT);
              GetObject(hf,sizeof(LOGFONT),&oldFont);
              oldFont.lfItalic=TRUE;
              italicFont=CreateFontIndirect(&oldFont);
              oldFont.lfItalic=FALSE;oldFont.lfWeight=FW_BLACK;
              boldFont=CreateFontIndirect(&oldFont);
             }
            Tpreset *preset=(*parent->localPresets)[lvcd->nmcd.dwItemSpec];
            if (preset->autoLoadedFromFile)
             SelectObject(lvcd->nmcd.hdc,italicFont);
            const char_t *defaultPreset=cfgGetStr(IDFF_defaultPreset);
            if (stricmp(defaultPreset,preset->presetName)==0)
             SelectObject(lvcd->nmcd.hdc,boldFont);
            setDlgResult(/*CDRF_NOTIFYPOSTPAINT*/CDRF_NEWFONT);
            return TRUE;
           }
          return TRUE;
         }
       }
     break;
    }
  }
 return TconfPageDec::msgProc(uMsg,wParam,lParam);
}

void TpresetsPage::onAutopreset(void)
{
 TpresetAutoloadDlgBase *dlg=autoDlgCreate(deci,m_hwnd,10,10);
 if (dlg->show())
  parent->setChange();
 delete dlg;
}
void TpresetsPage::onNewBt(void)
{
 onNew(ID_MNI_PRESET_NEWFROMDEFAULT);
}
void TpresetsPage::onNewMenu(void)
{
 onNew(0);
}
void TpresetsPage::addNewPreset(Tpreset *newPreset)
{
 parent->localPresets->nextUniqueName(newPreset);
 parent->localPresets->storePreset(newPreset);
 deciD->setPresets(parent->localPresets);
 ListView_SetItemCountEx(hlv,parent->localPresets->size(),0);
 parent->selectPreset(newPreset->presetName);
 cfg2dlg();
 parent->setChange();
 //SetActiveWindow(hlv);
 //PostMessage(hlv,LVM_EDITLABEL,ListView_GetNextItem(hlv,-1,LVNI_SELECTED),0);
}
void TpresetsPage::onNew(int menuCmd)
{
 if (menuCmd==0)
  {
   HMENU hmn=LoadMenu(hi,MAKEINTRESOURCE(IDR_MENU_PRESET)),hmn2=_(GetSubMenu(hmn,0)) ;
   RECT r;
   GetWindowRect(GetDlgItem(m_hwnd,IDC_BT_PRESET_NEW_MENU),&r);
   menuCmd=TrackPopupMenu(hmn2,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,r.left-1,r.bottom,0,m_hwnd,0);
   DestroyMenu(hmn);
  }
 switch (menuCmd)
  {
   case ID_MNI_PRESET_NEWFROMDEFAULT:
    {
     addNewPreset((*parent->localPresets)[0]->copy());
     break;
    }
   case ID_MNI_PRESET_NEWFROMSELECTED:
    {
     addNewPreset((*parent->localPresets)[lvGetSelItem(IDC_LV_PRESETS)]->copy());
     break;
    }
   case ID_MNI_PRESET_NEW_FROMFILE:
    {
     strings files;
     if (dlgOpenFiles(m_hwnd,_(-IDD_PRESETS,_l("Load ffdshow preset")),presetfilter,presetext,files,_l("."),0))
      for (strings::const_iterator fileDlgFlnm=files.begin();fileDlgFlnm!=files.end();fileDlgFlnm++)
       {
        Tpreset *newPreset=parent->localPresets->newPreset(NULL);
        if (!newPreset->loadFile(fileDlgFlnm->c_str()))
         delete newPreset;
        else
         addNewPreset(newPreset);
       }
     }
    break;
  }
}
void TpresetsPage::onReadFromFile(void)
{
 int i=lvGetSelItem(IDC_LV_PRESETS);
 char_t presetFlnm[MAX_PATH];
 _splitpath_s((*parent->localPresets)[i]->presetName, NULL, 0, NULL, 0, presetFlnm, countof(presetFlnm), NULL, 0);
 if (dlgGetFile(false,m_hwnd,_(-IDD_PRESETS,_l("Load ffdshow preset")),presetfilter,presetext,presetFlnm,_l("."),0))
  {
   char_t presetName[MAX_PATH];
   ff_strncpy(presetName,(*parent->localPresets)[i]->presetName,MAX_PATH);
   (*parent->localPresets)[i]->loadFile(presetFlnm);
   ff_strncpy((*parent->localPresets)[i]->presetName, presetName, countof((*parent->localPresets)[i]->presetName));
   parent->selectPreset(presetName);
   cfg2dlg();
   parent->setChange();
  }
}
void TpresetsPage::onSaveToFile(void)
{
 int i=lvGetSelItem(IDC_LV_PRESETS);
 char_t presetFlnm[MAX_PATH];
 _splitpath_s((*parent->localPresets)[i]->presetName, NULL, 0, NULL, 0, presetFlnm, countof(presetFlnm), NULL, 0);
 if (dlgGetFile(true,m_hwnd,_(-IDD_PRESETS,_l("Save ffdshow preset")),presetfilter,presetext,presetFlnm,_l("."),0))
  (*parent->localPresets)[i]->saveFile(presetFlnm);
}
void TpresetsPage::onRemove(void)
{
 int i=lvGetSelItem(IDC_LV_PRESETS);
 if (i!=0 && MessageBox(m_hwnd,_(-IDD_PRESETS,_l("Do you really want to remove selected preset?")),_(-IDD_PRESETS,_l("Removing preset")),MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDYES)
  {
   char_t presetName[1024];
   ListView_GetItemText(hlv,i,0,presetName,1023);
   if (deciD->isDefaultPreset(presetName))
    cfgSet(IDFF_defaultPreset,(*parent->localPresets)[0]->presetName); // [0] is protected(Remove bottun is inactivated, thus cannot be removed), it's safe to assume default preset is not [0].
   parent->localPresets->removePreset(presetName);
   ListView_SetItemCountEx(hlv,parent->localPresets->size(),0);
   ListView_GetItemText(hlv,0,0,presetName,1023);
   parent->selectPreset(presetName);
   cfg2dlg();
   parent->setChange();
  }
}

void TpresetsPage::renamePreset(const char_t *presetName)
{
 deciD->renameActivePreset(presetName);
 deciD->setPresets(parent->localPresets);
 parent->presetChanged(presetName);
 parent->fillPresetsCbx();
 parent->setChange();
}
void TpresetsPage::onRenameBt(void)
{
 onRename(ID_MNI_PRESET_RENAME);
}
void TpresetsPage::onRenameMenu(void)
{
 onRename(0);
}
void TpresetsPage::onRename(int menuCmd)
{
 const char_t *AVIname=deci->getSourceName();
 if (menuCmd==0)
  {
   HMENU hmn=LoadMenu(hi,MAKEINTRESOURCE(IDR_MENU_PRESET)),hmn2=_(GetSubMenu(hmn,1));
   RECT r;
   GetWindowRect(GetDlgItem(m_hwnd,IDC_BT_PRESET_RENAME_MENU),&r);
   if (AVIname[0]=='\0')
    EnableMenuItem(hmn2,1,MF_BYPOSITION|MF_GRAYED);
   menuCmd=TrackPopupMenu(hmn2,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,r.left-1,r.bottom,0,m_hwnd,0);
   DestroyMenu(hmn);
  }
 int i=lvGetSelItem(IDC_LV_PRESETS);
 if (menuCmd==ID_MNI_PRESET_RENAMETOFILE)
  {
   char_t presetName[MAX_PATH];
   Tpreset::normalizePresetName(presetName, AVIname, countof(presetName));
   parent->localPresets->nextUniqueName(presetName, countof(presetName));
   renamePreset(presetName);
   InvalidateRect(hlv,NULL,FALSE);
  }
 else if (menuCmd==ID_MNI_PRESET_RENAMETOEXE)
  {
   char_t presetName[MAX_PATH];
   Tpreset::normalizePresetName(presetName, deci->getExeflnm(), countof(presetName));
   parent->localPresets->nextUniqueName(presetName, countof(presetName));
   renamePreset(presetName);
   InvalidateRect(hlv,NULL,FALSE);
  }
 else if (menuCmd==ID_MNI_PRESET_RENAME)
  {
   SetFocus(hlv);
   ListView_EditLabel(hlv,i);
  }
}

void TpresetsPage::changePresetState(void)
{
 if (cfgGet(IDFF_autoLoadedFromFile)==1)
  {
   cfgSet(IDFF_autoLoadedFromFile,0);
   InvalidateRect(hlv,NULL,FALSE);
  }
 else
  {
   char_t presetName[MAX_PATH];
   deciD->getActivePresetName(presetName, countof(presetName));
   cfgSet(IDFF_defaultPreset,presetName);
   InvalidateRect(hlv,NULL,FALSE);
  }
}
void TpresetsPage::translate(void)
{
 TconfPageBase::translate();
 ListView_SetColumnText(hlv,0,_(IDC_LV_PRESETS,_l("Preset name")));
}

TpresetsPage::TpresetsPage(TffdshowPageDec *Iparent,const char_t *Ipresetfilter,const char_t *Ipresetext):TconfPageDec(Iparent,NULL,0),presetfilter(Ipresetfilter),presetext(Ipresetext)
{
 dialogId=IDD_PRESETS;
 static const TbindCheckbox<TpresetsPage> chb[]=
  {
   IDC_CHB_AUTOPRESET,IDFF_autoPreset,&TpresetsPage::autopreset2dlg,
   IDC_CHB_AUTOPRESET_FILEFIRST,IDFF_autoPresetFileFirst,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindButton<TpresetsPage> bt[]=
  {
   IDC_BT_AUTOPRESET,&TpresetsPage::onAutopreset,
   IDC_BT_PRESET_NEW,&TpresetsPage::onNewBt,
   IDC_BT_PRESET_NEW_MENU,&TpresetsPage::onNewMenu,
   IDC_BT_PRESET_READFROMFILE,&TpresetsPage::onReadFromFile,
   IDC_BT_PRESET_SAVETOFILE,&TpresetsPage::onSaveToFile,
   IDC_BT_PRESET_REMOVE,&TpresetsPage::onRemove,
   IDC_BT_PRESET_RENAME,&TpresetsPage::onRenameBt,
   IDC_BT_PRESET_RENAME_MENU,&TpresetsPage::onRenameMenu,
   0,NULL
  };
 bindButtons(bt);
}

//========================= TpresetsPageVideo ============================
TpresetsPageVideo::TpresetsPageVideo(TffdshowPageDec *Iparent):TpresetsPage(Iparent,_l("ffdshow video preset (*.ffpreset)\0*.ffpreset\0All files (*.*)\0*.*\0\0"),_l("ffpreset"))
{
 autoDlgCreate=TpresetAutoloadDlgVideo::create;
}

//========================= TpresetsPageAudio ============================
TpresetsPageAudio::TpresetsPageAudio(TffdshowPageDec *Iparent):TpresetsPage(Iparent,_l("ffdshow audio preset (*.ffApreset)\0*.ffApreset\0All files (*.*)\0*.*\0\0"),_l("ffApreset"))
{
 autoDlgCreate=TpresetAutoloadDlgAudio::create;
}
