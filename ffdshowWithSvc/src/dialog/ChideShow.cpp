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
#include "ChideShow.h"
#include "TffdshowPageDec.h"

void ThideShowPage::init(void)
{
 setFont(IDC_BT_HIDESHOW_HIDE,parent->arrowsFont);
 setFont(IDC_BT_HIDESHOW_SHOW,parent->arrowsFont);
 setFont(IDC_BT_HIDESHOW_HIDE_ALL,parent->arrowsFont);
 setFont(IDC_BT_HIDESHOW_SHOW_ALL,parent->arrowsFont);
}

void ThideShowPage::cfg2dlg(void)
{
 char_t ii1[256];lbxGetCurText(IDC_LBX_HIDESHOW_AVAILABLE,ii1);lbxClear(IDC_LBX_HIDESHOW_AVAILABLE);
 char_t ii2[256];lbxGetCurText(IDC_LBX_HIDESHOW_VISIBLE  ,ii2);lbxClear(IDC_LBX_HIDESHOW_VISIBLE  );
 lbxClear(IDC_LBX_HIDESHOW_VISIBLE);
 for (ThtiPages::const_iterator pages=filterPages.begin();pages!=filterPages.end();pages++)
  {
   TconfPageBase *page=(*pages)->getPage();
   int show=page->getShow();
   if (show>=0 && page->filterPageID<=1)
    if (show)
     lbxAdd(IDC_LBX_HIDESHOW_VISIBLE,page->dialogName(),intptr_t(page));
    else
     lbxAdd(IDC_LBX_HIDESHOW_AVAILABLE,page->dialogName(),intptr_t(page));
  }
 lbxSetCurSel(IDC_LBX_HIDESHOW_AVAILABLE,lbxFindItem(IDC_LBX_HIDESHOW_AVAILABLE,ii1,true));
 lbxSetCurSel(IDC_LBX_HIDESHOW_VISIBLE  ,lbxFindItem(IDC_LBX_HIDESHOW_VISIBLE  ,ii2,true));
}

void ThideShowPage::hideSelected(void)
{
 int ii=lbxGetCurSel(IDC_LBX_HIDESHOW_VISIBLE);
 if (ii==LB_ERR) return;
 TconfPageBase *page=(TconfPageBase*)lbxGetItemData(IDC_LBX_HIDESHOW_VISIBLE,ii);
 page->setShow(0);
 lbxSetCurSel(IDC_LBX_HIDESHOW_VISIBLE,ii+1);
 parent->showShowHide();
 parent->drawInter();
 cfg2dlg();
}
void ThideShowPage::showSelected(void)
{
 int ii=lbxGetCurSel(IDC_LBX_HIDESHOW_AVAILABLE);
 if (ii==LB_ERR) return;
 TconfPageBase *page=(TconfPageBase*)lbxGetItemData(IDC_LBX_HIDESHOW_AVAILABLE,ii);
 page->setShow(1);
 lbxSetCurSel(IDC_LBX_HIDESHOW_AVAILABLE,ii+1);
 parent->showShowHide();
 parent->drawInter();
 cfg2dlg();
}

void ThideShowPage::hideAll(void)
{
 int itCount=lbxGetItemCount(IDC_LBX_HIDESHOW_VISIBLE);
 for (int i=0;i<itCount;i++)
  {
   TconfPageBase *page=(TconfPageBase*)lbxGetItemData(IDC_LBX_HIDESHOW_VISIBLE,i);
   page->setShow(0);
  }
 parent->showShowHide();
 parent->drawInter();
 cfg2dlg();
}
void ThideShowPage::showAll(void)
{
 int itCount=lbxGetItemCount(IDC_LBX_HIDESHOW_AVAILABLE);
 for (int i=0;i<itCount;i++)
  {
   TconfPageBase *page=(TconfPageBase*)lbxGetItemData(IDC_LBX_HIDESHOW_AVAILABLE,i);
   page->setShow(1);
  }
 parent->showShowHide();
 parent->drawInter();
 cfg2dlg();
}

INT_PTR ThideShowPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_LBX_HIDESHOW_AVAILABLE:
       if (HIWORD(wParam)==LBN_DBLCLK)
        {
         showSelected();
         return TRUE;
        }
       break;
      case IDC_LBX_HIDESHOW_VISIBLE:
       if (HIWORD(wParam)==LBN_DBLCLK)
        {
         hideSelected();
         return TRUE;
        }
       break;
     }
    break;
  }
 return TconfPageDec::msgProc(uMsg,wParam,lParam);
}

ThideShowPage::ThideShowPage(TffdshowPageDec *Iparent,ThtiPages &IfilterPages):TconfPageDec(Iparent,NULL,0),filterPages(IfilterPages)
{
 dialogId=IDD_HIDESHOW;
 idffOrder=-3;
 inPreset=1;
 static const TbindButton<ThideShowPage> bt[]=
  {
   IDC_BT_HIDESHOW_HIDE,&ThideShowPage::hideSelected,
   IDC_BT_HIDESHOW_SHOW,&ThideShowPage::showSelected,
   IDC_BT_HIDESHOW_HIDE_ALL,&ThideShowPage::hideAll,
   IDC_BT_HIDESHOW_SHOW_ALL,&ThideShowPage::showAll,
   0,NULL
  };
 bindButtons(bt);
}
