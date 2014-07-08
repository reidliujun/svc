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
#include "Ckeys.h"
#include "TkeyboardDirect.h"
#include "TffdshowPageDec.h"

LRESULT TkeysPage::TwidgetKeys::onSysKeyDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 return onKeyDown(hwnd,uMsg,wParam,lParam); //HACK
}
LRESULT TkeysPage::TwidgetKeys::onKeyDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (keysPage->keyChanging!=-1)
  {
   keysPage->keyChange((int)wParam);
   keysPage->endKeyChange();
   keysPage->ignoreNextKey=true;
   return 0;
  }
 return TwindowWidget::onKeyDown(hwnd,uMsg,wParam,lParam);
}
LRESULT TkeysPage::TwidgetKeys::onGetDlgCode(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (keysPage->keyChanging!=-1)
  {
   MSG *msg=(MSG*)lParam;
   if (msg && (msg->message==WM_KEYDOWN || msg->message==WM_KEYUP))
    switch (msg->wParam)
     {
      case VK_TAB:
      case VK_RETURN:
       keysPage->keyChange((int)msg->wParam);
       keysPage->ignoreNextKey=true;
       // fall
      case VK_ESCAPE:
       keysPage->endKeyChange();
       msg->message=WM_NULL;
       return DLGC_WANTALLKEYS;
     }
  }
 return TwindowWidget::onGetDlgCode(hwnd,uMsg,wParam,lParam);
}
LRESULT TkeysPage::TwidgetKeys::onKeyUp(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 if (keysPage->ignoreNextKey)
  {
   keysPage->ignoreNextKey=false;
   return 0;
  }
 else
  switch (wParam)
   {
    case VK_DELETE:
     keysPage->beginKeyChange();
     keysPage->keyChange(0);
     keysPage->endKeyChange();
     return 0;
    case VK_SPACE:
     keysPage->beginKeyChange();
     return 0;
   }
 return TwindowWidget::onKeyUp(hwnd,uMsg,wParam,lParam);
}
LRESULT TkeysPage::TwidgetKeys::onChar(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 return onKeyUp(hwnd,uMsg,wParam,lParam);
}

void TkeysPage::init(void)
{
 keyChanging=-1;ignoreNextKey=false;

 hlv=GetDlgItem(m_hwnd,IDC_LV_KEYS);

 int ncol=0;
 ListView_AddCol(hlv,ncol,cfgGet(IDFF_lvKeysWidth0),_l("Action"),false);
 ListView_AddCol(hlv,ncol,cfgGet(IDFF_lvKeysWidth1),_l("Key"),false);
 ListView_SetExtendedListViewStyle(hlv,ListView_GetExtendedListViewStyle(hlv)|LVS_EX_FULLROWSELECT);
 ListView_SetItemCount(hlv,deciD->getKeyParamCount2());
}

void TkeysPage::cfg2dlg(void)
{
 keys2dlg();
 remote2dlg();
}

void TkeysPage::keys2dlg(void)
{
 setCheck(IDC_CHB_KEYS,cfgGet(IDFF_isKeys));
 setCheck(IDC_CHB_KEYS_ALWAYS,cfgGet(IDFF_keysAlways));
 setCheck(IDC_CHB_KEYS_SHORTOSD,cfgGet(IDFF_keysShortOsd));
 SetDlgItemInt(m_hwnd,IDC_ED_KEYS_SEEK1,cfgGet(IDFF_keysSeek1),FALSE);
 SetDlgItemInt(m_hwnd,IDC_ED_KEYS_SEEK2,cfgGet(IDFF_keysSeek2),FALSE);
}
void TkeysPage::remote2dlg(void)
{
 setCheck(IDC_CHB_REMOTE,cfgGet(IDFF_isRemote));
 int messageMode=cfgGet(IDFF_remoteMessageMode);
 if (messageMode==0) setCheck(IDC_RBT_REMOTE_FFDSHOW,1);
 else if (messageMode==1) setCheck(IDC_RBT_REMOTE_USER,1);
 SetDlgItemInt(m_hwnd,IDC_ED_REMOTE_USER,cfgGet(IDFF_remoteMessageUser),FALSE);
 setCheck(IDC_CHB_REMOTE_ACCEPTKEYS,cfgGet(IDFF_remoteAcceptKeys));
}

INT_PTR TkeysPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    cfgSet(IDFF_lvKeysWidth0,ListView_GetColumnWidth(hlv,0));
    cfgSet(IDFF_lvKeysWidth1,ListView_GetColumnWidth(hlv,1));
    break;
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CHB_KEYS:
       cfgSet(IDFF_isKeys,getCheck(IDC_CHB_KEYS));
       keys2dlg();
       parent->drawInter();
       return TRUE;
     }
    break;
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (nmhdr->hwndFrom==hlv && nmhdr->idFrom==IDC_LV_KEYS)
      switch (nmhdr->code)
       {
        case NM_DBLCLK:
         if (keyChanging!=-1) endKeyChange();
         else beginKeyChange();
         return TRUE;
        case NM_CLICK:
         if (keyChanging!=-1) endKeyChange();
         return TRUE;
        case LVN_GETDISPINFO:
         {
          NMLVDISPINFO *nmdi=(NMLVDISPINFO*)lParam;
          int i=nmdi->item.iItem;
          if (i==-1) break;
          if (nmdi->item.mask&LVIF_TEXT)
           switch (nmdi->item.iSubItem)
            {
             case 0:
              {
               const char_t *descr;
               deciD->getKeyParamDescr(i,&descr);
               strcpy(nmdi->item.pszText,_(IDC_LV_KEYS,descr));
               break;
              }
             case 1:
              strcpy(nmdi->item.pszText,_(IDC_LV_KEYS,keyChanging==-1?Tkeyboard::getKeyName(deciD->getKeyParamKey2(i)):_l("press key")));
              break;
            }
          return TRUE;
         }
       }
     break;
    }
  }
 return TconfPageDec::msgProc(uMsg,wParam,lParam);
}
Twidget* TkeysPage::createDlgItem(int id,HWND h)
{
 if (id==IDC_LV_KEYS)
  return new TwidgetKeys(h,this);
 else
  return TconfPageDec::createDlgItem(id,h);
}

void TkeysPage::onGirder(void)
{
 if (deciD->getKeyParamKey2(0)!=VK_CONTROL || deciD->getKeyParamKey2(1)!=VK_MENU || deciD->getKeyParamKey2(2)!=VK_SHIFT)
  err(_(-IDD_KEYS,_l("GML file can be exported only if:\nActivation key 1 is 'ctrl'\nActivation key 2 is 'alt' and\nSecond function key is 'shift'")),_(-IDD_KEYS,_l("Girder export")));
 else
  {
   char_t flnm[MAX_PATH]=_l("ffdshow.gml");
   if (dlgGetFile(true,m_hwnd,_(-IDD_KEYS,_l("Select name of exported Girder file")),_l("Girder Files (*.gml)\0*.gml\0"),_l("gml"),flnm,_l("."),0))
    if (deciD->exportKeysToGML(flnm)==S_OK)
     msg(_(-IDD_KEYS,_l("GML file exported successfully")),_(-IDD_KEYS,_l("Girder export")));
    else
     err(_(-IDD_KEYS,_l("GML file export failed")),_(-IDD_KEYS,_l("Girder export")));
  }
}
void TkeysPage::beginKeyChange(void)
{
 if (keyChanging!=-1) return;
 int i=lvGetSelItem(IDC_LV_KEYS);
 if (i==-1) return;
 keyChanging=i;
 ListView_RedrawItems(hlv,i,i);
}
void TkeysPage::endKeyChange(void)
{
 if (keyChanging==-1) return;
 ListView_RedrawItems(hlv,keyChanging,keyChanging);
 keyChanging=-1;
}
void TkeysPage::keyChange(int newkey)
{
 if (keyChanging==-1) return;
 int prev;const char_t *prevDescr;
 if (deciD->setKeyParamKeyCheck(keyChanging,newkey,&prev,&prevDescr)!=S_OK)
  {
   endKeyChange();
   ignoreNextKey=true;
   char_t s[256];
   tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s %s"), _(-IDD_KEYS, _l("This key is already assigned to")), _(IDC_LV_KEYS,prevDescr));
   msg(s);
  }
}
bool TkeysPage::reset(bool testonly)
{
 if (!testonly)
  {
   deciD->resetKeys();
   repaint(hlv);
  }
 return true;
}

void TkeysPage::translate(void)
{
 TconfPageDec::translate();
 ListView_SetColumnText(hlv,0,_(IDC_LV_KEYS,_l("Action")));
 ListView_SetColumnText(hlv,1,_(IDC_LV_KEYS,_l("Key")));
}

TkeysPage::TkeysPage(TffdshowPageDec *Iparent):TconfPageDec(Iparent,NULL,0)
{
 dialogId=IDD_KEYS;
 idffInter=IDFF_isKeys;resInter=IDC_CHB_KEYS;
 helpURL=_l("Keyboard+and+remote+control.html");
 static const TbindCheckbox<TkeysPage> chb[]=
  {
   IDC_CHB_KEYS_ALWAYS,IDFF_keysAlways,NULL,
   IDC_CHB_KEYS_SHORTOSD,IDFF_keysShortOsd,NULL,
   IDC_CHB_REMOTE,IDFF_isRemote,NULL,
   IDC_CHB_REMOTE_ACCEPTKEYS,IDFF_remoteAcceptKeys,NULL,
   0,NULL,NULL
  };
 bindCheckboxes(chb);
 static const TbindRadiobutton<TkeysPage> rbt[]=
  {
   IDC_RBT_REMOTE_FFDSHOW,IDFF_remoteMessageMode,0,NULL,
   IDC_RBT_REMOTE_USER,IDFF_remoteMessageMode,1,NULL,
   0,0,0,NULL
  };
 bindRadioButtons(rbt);
 static const TbindEditInt<TkeysPage> edInt[]=
  {
   IDC_ED_KEYS_SEEK1,1,10000,IDFF_keysSeek1,NULL,
   IDC_ED_KEYS_SEEK2,1,10000,IDFF_keysSeek2,NULL,
   IDC_ED_REMOTE_USER,1,int(1<<30),IDFF_remoteMessageUser,NULL,
   0
  };
 bindEditInts(edInt);
 static const TbindButton<TkeysPage> bt[]=
  {
   IDC_BT_REMOTE_GIRDER,&TkeysPage::onGirder,
   0,NULL
  };
 bindButtons(bt);
}
