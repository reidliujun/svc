/*
 * Copyright (c) 2002-2006 Milan Cutka
 * based on Cabout.cpp from XviD DirectShow filter
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
#include "TffdshowPageDec.h"
#include "TconfPageDec.h"
#include "ChideShow.h"
#include "Tpresets.h"
#include "TpresetSettingsVideo.h"
#include "IffdshowDecVideo.h"
#include "IffdshowDecAudio.h"
#include "TSpecifyPropertyPagesVE.h"

TffdshowPageDec::TffdshowPageDec(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr,bool IhasFull):
 TffdshowPageBase(pUnk,phr,ItitleW,name,dialogId,resstr),
 hasFull(IhasFull),
 dragpage(NULL)
{
 page=NULL;

 typedef void(Twindow::*TonClick)(void);
 static const TbindButton<Twindow> bt[]=
  {
   IDC_BT_RESET,(TonClick)&TffdshowPageDec::onReset, // HACK
   IDC_BT_RESETORDER,(TonClick)&TffdshowPageDec::onResetOrder,
   0,NULL
  };
 bindButtons(bt);
}
TffdshowPageDec::~TffdshowPageDec()
{
}

void TffdshowPageDec::selectPage(TconfPageBase *Ipage)
{
 TffdshowPageBase::selectPage(Ipage);
 page->interDlg();
 enable(page->helpURL!=NULL,IDC_BT_HELP);
 enable(page->hasReset(),IDC_BT_RESET);
 enable(page->inPreset && (filterMode&IDFF_FILTERMODE_PROC)==0,IDC_CBX_PRESETS);
 setFullHalf();
}

void TffdshowPageDec::setFullHalf(void)
{
 if (!page || !hasFull) return;
 int processFull=page->getProcessFull();
 enable(processFull!=-1,IDC_CHB_PROCESSFULL);
 setCheck(IDC_CHB_PROCESSFULL,processFull>0);
 int processHalf=page->getProcessHalf();
 enable(processHalf!=-1,IDC_CHB_PROCESSHALF);
 setCheck(IDC_CHB_PROCESSHALF,processHalf>0);
}

void TffdshowPageDec::drawInter(void)
{
 InvalidateRect(htv,NULL,FALSE);
}

bool TffdshowPageDec::invInter(TconfPageBase *page,RECT *r)
{
 if (!page) page=this->page;
 if (page->invInter())
  {
   InvalidateRect(htv,r,FALSE);
   setDlgResult(TRUE);
   return true;
  }
 else
  return false;
}

HRESULT TffdshowPageDec::OnConnect(IUnknown *pUnk)
{
 deciD=pUnk;
 HRESULT hr=TffdshowPageBase::OnConnect(pUnk);
 return FAILED(hr)?hr:S_OK;
}
HRESULT TffdshowPageDec::OnDisconnect(void)
{
 deci->loadGlobalSettings();
 deciD->loadKeysSettings();
 deciD->loadRemoteSettings();
 deciD=NULL;
 return TffdshowPageBase::OnDisconnect();
}
void TffdshowPageDec::onActivate(void)
{
 cbxSetDroppedWidth(IDC_CBX_PRESETS,340);

 Tpresets *presets;
 deciD->getPresetsPtr(&presets);
 localPresets=presets->newPresets();
 deciD->getPresets(localPresets);
 deciD->getActivePresetName(oldActivePresetName,MAX_PATH);

 TreeView_SetIndent(htv,24);
 //TreeView_SetItemHeight(htv,26);
 TreeView_Expand(htv,htiPresets,TVE_EXPAND);
 fillPresetsCbx();
 cbxSetCurSel(IDC_CBX_PRESETS,cbxFindItem(IDC_CBX_PRESETS,oldActivePresetName,true));
 selectPreset(oldActivePresetName);
 CRect rp;GetWindowRect(m_hwnd,&rp);
 CRect rc;GetWindowRect(htv,&rc);
 tvx=rc.left-rp.left;tvy=rc.top-rp.top;
 deciD->setOnNewFiltersMsg(m_hwnd,WM_FFONNEWFILTERS);
}
STDMETHODIMP TffdshowPageDec::Deactivate(void)
{
 deciD->setOnNewFiltersMsg(NULL,0);
 filterPages.clear();
 deciD->setActivePreset(oldActivePresetName,false);
 delete localPresets;
 return TffdshowPageBase::Deactivate();
}

void TffdshowPageDec::onApplyChanges(void)
{
 deciD->saveKeysSettings();
 deciD->saveRemoteSettings();
 if (localPresets)
  {
   deciD->setPresets(localPresets);
   deciD->savePresets();
   ff_strncpy(oldActivePresetName, cbxGetCurText(IDC_CBX_PRESETS), countof(oldActivePresetName));
  }
}

int CALLBACK TffdshowPageDec::orderCompareFunc(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)
{
 int o1=((ThtiPage*)lParam1)->getPage()->getOrder(),o2=((ThtiPage*)lParam2)->getPage()->getOrder();
 return o1-o2;
}
void TffdshowPageDec::sortOrder(void)
{
 if (!htiPresets) return;
 TVSORTCB tvs;
 tvs.hParent=htiPresets;
 tvs.lpfnCompare=orderCompareFunc;
 tvs.lParam=0;
 TreeView_SortChildrenCB(htv,&tvs,0);
}

void TffdshowPageDec::swap(int direction)
{
 int pageorder=page->getOrder();
 int minOrder=deciD->getMinOrder2();
 if (pageorder<minOrder) return;
 HTREEITEM hti0;
 int maxOrder=deciD->getMaxOrder2();
 switch (direction)
  {
   case -1:if (pageorder<=minOrder) return;
           hti0=TreeView_GetPrevSibling(htv,page->hti);
           break;
   case  1:if (pageorder>=maxOrder) return;
           hti0=TreeView_GetNextSibling(htv,page->hti);
           break;
   default:return;
  }
 TconfPageBase *page1=hti2page(hti0),*page2=hti2page(page->hti);
 if (isIn(page1->getOrder(),minOrder,maxOrder) && isIn(page2->getOrder(),minOrder,maxOrder))
  swap(page1,page2);
}
void TffdshowPageDec::swap(TconfPageBase *page1,TconfPageBase *page2)
{
 int o1=page1->getOrder(),o2=page2->getOrder();
 page1->setOrder(o2);page2->setOrder(o1);
 sortOrder();
}

void TffdshowPageDec::fillPresetsCbx(void)
{
 int sel=cbxGetCurSel(IDC_CBX_PRESETS);
 cbxClear(IDC_CBX_PRESETS);
 for (Tpresets::iterator i=localPresets->begin();i!=localPresets->end();i++)
  cbxAdd(IDC_CBX_PRESETS,(*i)->presetName);
 char_t presetName[MAX_PATH];
 deciD->getActivePresetName(presetName,MAX_PATH);
 cbxSetCurSel(IDC_CBX_PRESETS,cbxFindItem(IDC_CBX_PRESETS,presetName,true));
}
void TffdshowPageDec::selectPreset(const char_t *presetName)
{
 Tpreset *oldPreset;
 deciD->getPresetPtr(&oldPreset);
 for (Tpresets::iterator i=localPresets->begin();i!=localPresets->end();i++)
  if (stricmp(presetName,(*i)->presetName)==0 && *i!=oldPreset)
   {
    deciD->setPresetPtr(*i);
    presetChanged(presetName);
    cbxSetCurSel(IDC_CBX_PRESETS,int(i-localPresets->begin()));
    if (page) page->cfg2dlg();
    return;
   }
}

void TffdshowPageDec::presetChanged(const char_t *presetName)
{
 fillPresetsCbx();
 int previd=page?page->uniqueID():0,prevVisId=hti2page(TreeView_GetFirstVisible(htv))->uniqueID();
 for (ThtiPages::iterator p=filterPages.begin();p!=filterPages.end();p++)
  deletePage(*p);
 filterPages.clear();
 deciD->createPresetPages(presetName,this);
 showShowHide(previd);
 InvalidateRect(htv,NULL,FALSE);
 sortOrder();
 setFullHalf();
}
void TffdshowPageDec::showShowHide(int previd,int prevVisId)
{
 if (previd==0 && page) previd=page->uniqueID();
 if (prevVisId==0) prevVisId=hti2page(TreeView_GetFirstVisible(htv))->uniqueID();
 for (ThtiPages::reverse_iterator rp=filterPages.rbegin();rp!=filterPages.rend();rp++)
  if ((*rp)->hti)
   {
    TreeView_DeleteItem(htv,(*rp)->hti);
    (*rp)->hti=NULL;
    for (TconfPages::iterator i=(*rp)->begin();i!=(*rp)->end();i++)
     (*i)->hti=NULL;
   }
 TVINSERTSTRUCT tvis;
 tvis.hParent=htiPresets;
 tvis.hInsertAfter=htiBeforeShowHide;
 tvis.item.mask=TVIF_PARAM|TVIF_TEXT;
 HTREEITEM htiLast0=NULL;
 for (ThtiPages::const_iterator p=filterPages.begin();p!=filterPages.end();p++)
  if ((*p)->getPage()->getShow())
   {
    tvis.item.lParam=(LPARAM)*p;
    tvis.item.pszText=LPTSTR((*p)->front()->dialogName());
    tvis.item.iImage=tvis.item.iSelectedImage=0;
    tvis.hParent=((*p)->getPage()->filterPageID>1)?htiLast0:htiPresets;
    HTREEITEM hti=TreeView_InsertItem(htv,&tvis);
    (*p)->hti=hti;
    for (TconfPages::iterator i=(*p)->begin();i!=(*p)->end();i++)
     (*i)->hti=hti;
    if ((*p)->getPage()->filterPageID<=1)
     {
      if (htiLast0)
       TreeView_Expand(htv,htiLast0,TVE_EXPAND);
      htiLast0=hti;
     }
   }
 if (htiLast0) TreeView_Expand(htv,htiLast0,TVE_EXPAND);
 sortOrder();
 if (previd)
  treeSelectItem(previd,prevVisId);
}

INT_PTR TffdshowPageDec::msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_CBX_PRESETS:
       if (HIWORD(wParam)==CBN_SELCHANGE)
        {
         char_t presetName[256],actPresetName[256];
         GetDlgItemText(m_hwnd,IDC_CBX_PRESETS,presetName,255);
         deciD->getActivePresetName(actPresetName,255);
         if (stricmp(presetName,actPresetName)!=0)
          selectPreset(presetName);
         return TRUE;
        }
       break;
     }
    break;
   case WM_FFONNEWFILTERS:
    {
     for (ThtiPages::const_iterator hp=filterPages.begin();hp!=filterPages.end();hp++)
      for (TconfPages::const_iterator i=(*hp)->begin();i!=(*hp)->end();i++)
       ((TconfPageDec*)(*i))->onNewFilter();
     return TRUE;
    }
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (nmhdr->hwndFrom==htv && nmhdr->idFrom==IDC_TV_TREE)
      switch (nmhdr->code)
       {
        case TVN_GETINFOTIP:
         {
          if (!cfgGet(IDFF_showHints)) return FALSE;
          NMTVGETINFOTIP *nmtvit=LPNMTVGETINFOTIP(lParam);
          TconfPageBase *page=hti2page(nmtvit->hItem);
          char_t tipS[1024]=_l("");
          if (deciD->getFilterTipEx(page->filterID,page->filterPageID,tipS,1023)!=S_OK)
           page->getTip(tipS,1023);
          if (tipS[0]=='\0') return FALSE;
          memset(nmtvit->pszText,0,nmtvit->cchTextMax);
          ff_strncpy(nmtvit->pszText,tipS,nmtvit->cchTextMax-1);
          return TRUE;
         }
        case NM_CUSTOMDRAW:
         {
          NMTVCUSTOMDRAW *tvcd=LPNMTVCUSTOMDRAW(lParam);
          int rcDy=tvcd->nmcd.rc.bottom-tvcd->nmcd.rc.top;
          if (tvcd->nmcd.dwDrawStage==CDDS_PREPAINT)
           {
            setDlgResult(CDRF_NOTIFYITEMDRAW);
            return TRUE;
           }
          else if (tvcd->nmcd.dwDrawStage==CDDS_ITEMPREPAINT)
           {
            TconfPageBase *page=((ThtiPage*)tvcd->nmcd.lItemlParam)->getPage();
            if (page->getInter()==-1 && page->getOrder()==-1) return FALSE;
            setDlgResult(CDRF_NOTIFYPOSTPAINT);
            return TRUE;
           }
          else if (tvcd->nmcd.dwDrawStage==CDDS_ITEMPOSTPAINT)
           {
            RECT &tr=tvcd->nmcd.rc;
            if (tr.right>tr.left && tr.bottom>tr.top)
             {
              RECT rr;
              TreeView_GetItemRect(htv,HTREEITEM(tvcd->nmcd.dwItemSpec),&rr,true);
              rr.left-=24;
              TconfPageBase *page=((ThtiPage*)tvcd->nmcd.lItemlParam)->getPage();
              if (page->getInter()!=-1) ImageList_Draw(hil,page->getInter()?ilChecked:ilClear,tvcd->nmcd.hdc,tr.left+8+rr.left,tr.top+(rcDy-16)/2,ILD_TRANSPARENT);
              if (isIn(page->getOrder(),deciD->getMinOrder2(),deciD->getMaxOrder2()) && (tvcd->nmcd.uItemState&CDIS_SELECTED))
               {
                int img;
                HTREEITEM hti=page->hti;
                HTREEITEM htiPrev=TreeView_GetPrevVisible(htv,hti),htiNext=TreeView_GetNextVisible(htv,hti);
                int order=page->getOrder(),orderPrev=hti2page(htiPrev)->getOrder(),orderNext=hti2page(htiNext)->getOrder();
                if (order==deciD->getMinOrder2()) img=ilArrowD;
                else if (order==deciD->getMaxOrder2()) img=ilArrowU;
                else img=ilArrowUD;
                ImageList_DrawEx(hil,img,tvcd->nmcd.hdc,tr.left+2+rr.left,tr.top+(rcDy-16)/2,5,16,CLR_DEFAULT,CLR_DEFAULT,ILD_TRANSPARENT);
               }
             }
            return TRUE;
           }
          return FALSE;
         }
        case NM_CLICK:
         {
          POINT ps;
          GetCursorPos(&ps);
          ScreenToClient(htv,&ps);
          TVHITTESTINFO tvhti;
          tvhti.pt=ps;
          HTREEITEM hti=TreeView_HitTest(htv,&tvhti);
          if (!hti) return FALSE;
          RECT rr;
          TreeView_GetItemRect(htv,hti,&rr,TRUE);
          RECT r;
          TreeView_GetItemRect(htv,hti,&r,FALSE);
          ps.x-=rr.left-24;
          int iconTop=((r.bottom-r.top)-16)/2;
          ps.y-=r.top;
          if (ps.x>=8 && ps.x<=16+8 && ps.y>=iconTop+2 && ps.y<=iconTop+13)
           {
            if (invInter(hti2page(hti),&r))
             return TRUE;
           }
          else if (ps.x>=2 && ps.x<=7 && TreeView_GetSelection(htv)==tvhti.hItem)
           {
            int center=(r.bottom-r.top)/2;
            if (ps.y>center-6 && ps.y<center-1 && page->getOrder()>deciD->getMinOrder2())
             {
              swap(-1);
              return TRUE;
             }
            else if (ps.y>center+1 && ps.y<center+6 && page->getOrder()<deciD->getMaxOrder2())
             {
              swap(1);
              return TRUE;
             }
           }
          return FALSE;
         }
        case TVN_BEGINDRAG:
         {
          NMTREEVIEW *nmtv=LPNMTREEVIEW(lParam);
          dragpage=hti2page(nmtv->itemNew.hItem);
          int order=dragpage->getOrder();
          TreeView_SelectItem(htv,nmtv->itemNew.hItem);
          if (isIn(order,deciD->getMinOrder2(),deciD->getMaxOrder2()))
           SetCapture(m_hwnd);
          else
           dragpage=NULL;
          break;
         }
       }
     break;
    }
   case WM_MOUSEMOVE:
    if (dragpage)
     {
      TVHITTESTINFO tvhti;
      tvhti.pt.x=LOWORD(lParam)-tvx;
      tvhti.pt.y=HIWORD(lParam)-tvy;
      if (HTREEITEM htiTarget=TreeView_HitTest(htv,&tvhti))
       {
        //TreeView_SelectDropTarget(htv,htiTarget);
        TconfPageBase *targetpage=hti2page(htiTarget);
        if (isIn(targetpage->getOrder(),deciD->getMinOrder2(),deciD->getMaxOrder2()))
         swap(dragpage,targetpage);
       }
      return TRUE;
     }
    break;
   case WM_LBUTTONUP:
    if (dragpage)
     {
      dragpage=NULL;
      ReleaseCapture();
      return TRUE;
     }
    break;
  }
 return TffdshowPageBase::msgProc(uMsg,wParam,lParam);
}

Twidget* TffdshowPageDec::createDlgItem(int id,HWND h)
{
 if (id==IDC_TV_TREE)
  return new TwidgetTv(h,this);
 else
  return TffdshowPageBase::createDlgItem(id,h);
}

void TffdshowPageDec::onReset(void)
{
 bool wasreset=false;
 if (page->filterID)
  if (page->filterPageID)
   wasreset|=deciD->resetFilterEx(page->filterID,page->filterPageID)==S_OK;
  else
   wasreset|=deciD->resetFilter(page->filterID)==S_OK;
 wasreset|=page->reset();
 page->cfg2dlg();
 setChange();
}
void TffdshowPageDec::onResetOrder(void)
{
 if (SUCCEEDED(deciD->resetOrder()))
  sortOrder();
}

LRESULT TffdshowPageDec::TwidgetTv::onKeyDown(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (wParam)
  {
   case VK_SHIFT:
    self->isShift=true;
    break;
   case VK_UP:
   case VK_DOWN:
    if (self->isShift)
     {
      self->swap(wParam==VK_UP?-1:1);
      return 0;
     }
    else break;
   case VK_SPACE:
    self->invInter();
    return 0;
  }
 return Twidget::onKeyDown(hwnd,uMsg,wParam,lParam);
}
LRESULT TffdshowPageDec::TwidgetTv::onKeyUp(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (wParam)
  {
   case VK_SHIFT:
    self->isShift=false;
    break;
   case VK_SPACE:
    return 0;
   case VK_UP:
   case VK_DOWN:
    if (self->isShift) return 0;
    else break;
  }
 return Twidget::onKeyUp(hwnd,uMsg,wParam,lParam);
}

// -------------------- configure ----------------------
extern "C" void CALLBACK configureDec(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow);
void CALLBACK configureDec(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow)
{
 if (CoInitialize(NULL)!=S_OK) return;
 comptr<IffdshowDecVideo> deciV;comptr<IffdshowDecAudio> deciA;
 const GUID &IID_IffdshowDecVideo=getGUID<IffdshowDecVideo>(),&IID_IffdshowDecAudio=getGUID<IffdshowDecAudio>();
 if (CoCreateInstance(CLSID_FFDSHOW,NULL,CLSCTX_INPROC_SERVER,IID_IffdshowDecVideo,(void**)&deciV)==S_OK &&
     CoCreateInstance(CLSID_FFDSHOWAUDIO,NULL,CLSCTX_INPROC_SERVER,IID_IffdshowDecAudio,(void**)&deciA)==S_OK)
  {
   comptrQ<IffdshowBase>(deciV)->putParam(IDFF_filterMode,IDFF_FILTERMODE_VIDEO|IDFF_FILTERMODE_CONFIG);
   comptrQ<IffdshowBase>(deciA)->putParam(IDFF_filterMode,IDFF_FILTERMODE_AUDIO|IDFF_FILTERMODE_CONFIG);
   if (strstr(lpCmdLine,_l("top")))
    {
     comptrQ<IffdshowBase>(deciV)->putParam(IDFF_alwaysOnTop,1);
     comptrQ<IffdshowBase>(deciA)->putParam(IDFF_alwaysOnTop,1);
    }
   TSpecifyPropertyPagesVE::show2configPages(IID_IffdshowDecVideo,deciV,IID_IffdshowDecAudio,deciA,IDS_FFDSHOWDEC,IDI_FFDSHOW,IDFF_dlgDecCurrentPage);
  }
 CoUninitialize();
}
