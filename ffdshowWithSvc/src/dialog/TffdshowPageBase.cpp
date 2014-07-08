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
#include "TffdshowPageBase.h"
#include "IffdshowBase.h"
#include "TconfPageBase.h"
#include "Ttranslate.h"
#include "ffdebug.h"
#include "Tconfig.h"

//===================================== ThtiPage ===================================
TconfPageBase* ThtiPage::getPage(void)
{
 if (id)
  for (TconfPages::iterator page=begin();page!=end();page++)
   if ((*page)->forCodec(*id)) return *page;
 return *begin();
}
ThtiPage::~ThtiPage()
{
 for (TconfPages::iterator page=begin();page!=end();page++)
  {
   if ((*page)->m_hwnd) DestroyWindow((*page)->m_hwnd);
   delete *page;
  }
}

//================================= TffdshowPageBase ===============================
TffdshowPageBase::TffdshowPageBase(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int IdialogId,int Iresstr):
 CUnknown(name,pUnk),
 titleW(ItitleW),
 resstr(Iresstr),
 pageSite(NULL),
 dirty(false),
 wasobjectset(false),
 Twindow(NULL),
 hil(NULL),
 page(NULL),
 isShift(false),
 arrowsFont(NULL),
 resorePos(true)
{
//CBasePropertyPage(NAME(name),pUnk,dialogId,resstr)
 ASSERT(phr);
 dialogId=IdialogId;
 m_hwnd=NULL;
}

STDMETHODIMP TffdshowPageBase::GetPageInfo(LPPROPPAGEINFO pPageInfo)
{
 CheckPointer(pPageInfo,E_POINTER);

 pPageInfo->cb               = sizeof(PROPPAGEINFO);
 pPageInfo->pszDocString     = NULL;
 pPageInfo->pszHelpFile      = NULL;
 pPageInfo->dwHelpContext    = 0;

 LPOLESTR pszTitle;
 AMGetWideString(titleW,&pszTitle);
 pPageInfo->pszTitle=pszTitle;

 if (HWND hwnd=createDialog(dialogId,GetDesktopWindow(),0))
  {
   RECT rc;
   GetWindowRect(hwnd,&rc);
   pPageInfo->size.cx=rc.right-rc.left;
   pPageInfo->size.cy=rc.bottom-rc.top;
   DestroyWindow(hwnd);
  }
 else
  {
   pPageInfo->size.cx=340;
   pPageInfo->size.cy=150;
  }
 return S_OK;
}

void TffdshowPageBase::selectPage(TconfPageBase *Ipage)
{
 if (!Ipage->m_hwnd) Ipage->createWindow();
 bool pageChanged=page!=Ipage;
 if (page && pageChanged)
  if (Tconfig::winNT)
   ShowWindow(page->m_hwnd,SW_HIDE);
  else
   page->destroyWindow();
 page=Ipage;
 CRect rp=getChildRect(IDC_PLACE_PAGE);
 SetWindowPos(page->m_hwnd,htv,rp.left,rp.top,0,0,SWP_NOSIZE);
 page->resize(rp);
 pageIs=page->enabled();
 if (page->wndEnabled!=pageIs) page->enable();
 if (pageIs) page->cfg2dlg();
 if (pageChanged) ShowWindow(page->m_hwnd,SW_SHOW);
 enable(page->helpURL!=NULL && pageIs,IDC_BT_HELP);
 InvalidateRect(m_hwnd,NULL,FALSE);
 int lastpage=page->uniqueID();
 cfgSet(IDFF_lastPage,lastpage);
}

ThtiPage* TffdshowPageBase::addTI(TVINSERTSTRUCT *tvis,TconfPageBase *page)
{
 ThtiPage *htiPage=new ThtiPage(NULL,page);htiPages.push_back(htiPage);
 if (tvis)
  {
   tvis->item.lParam=(LPARAM)htiPage;
   tvis->item.pszText=LPTSTR(page->dialogName());
   HTREEITEM hti=TreeView_InsertItem(htv,tvis);
   page->hti=hti;
   htiPage->hti=hti;
  }
 else
  {
   page->hti=NULL;
   htiPage->hti=NULL;
  }
 return htiPage;
}
ThtiPage* TffdshowPageBase::addTI(TVINSERTSTRUCT *tvis,const TconfPages &pages,int *Iid)
{
 ThtiPage *htiPage=new ThtiPage(Iid,pages);htiPages.push_back(htiPage);
 HTREEITEM hti;
 if (tvis)
  {
   tvis->item.lParam=(LPARAM)htiPage;
   tvis->item.pszText=LPTSTR(pages[0]->dialogName());
   tvis->item.iImage=tvis->item.iSelectedImage=0;
   hti=TreeView_InsertItem(htv,tvis);
  }
 else
  hti=NULL;
 for (TconfPages::iterator i=htiPage->begin();i!=htiPage->end();i++)
  (*i)->hti=hti;
 htiPage->hti=hti;
 return htiPage;
}

ThtiPage* TffdshowPageBase::hti2htiPage(HTREEITEM hti)
{
 TVITEM tvi;
 tvi.hItem=hti;
 tvi.mask=TVIF_PARAM;
 TreeView_GetItem(htv,&tvi);
 return (ThtiPage*)tvi.lParam;
}
TconfPageBase* TffdshowPageBase::hti2page(HTREEITEM hti)
{
 return hti2htiPage(hti)->getPage();
}

HWND TffdshowPageBase::findParentDlg(void)
{
 for (HWND dlg=GetParent(m_hwnd);dlg;dlg=GetParent(dlg))
  if (GetWindowStyle(dlg)&WS_DLGFRAME)
   return dlg;
 return NULL;
}

void TffdshowPageBase::translate(void)
{
 Twindow::translate();
 for (ThtiPages::const_iterator hp=htiPages.begin();hp!=htiPages.end();hp++)
  for (TconfPages::const_iterator i=(*hp)->begin();i!=(*hp)->end();i++)
   {
    if ((*i)->m_hwnd) (*i)->translate();
    TVITEM ti;
    ti.hItem=(*i)->hti;
    ti.mask=TVIF_TEXT;
    ti.pszText=LPTSTR((*i)->dialogName());
    TreeView_SetItem(htv,&ti);
    if ((*i)->m_hwnd) (*i)->cfg2dlg();
   }
 if (comptrQ<IPropertyPageSiteFF> siteFF=pageSite)
  siteFF->onTranslate();
}
bool TffdshowPageBase::translateTV()
{
 if (page && tr->translateMode)
  {
   HTREEITEM hti=TreeView_GetDropHilight(htv);
   if (!hti) hti=TreeView_GetSelection(htv);
   ThtiPage *pages=hti2htiPage(hti);
   char_t newtrans[1024]=_l("");
   for (TconfPages::const_iterator page=pages->begin();page!=pages->end();page++)
    {
     const char_t *oldtrans=tr->translate((*page)->dialogId);
     if (oldtrans)
      {
       if (newtrans[0]=='\0')
        {
         ff_strncpy(newtrans, oldtrans, countof(newtrans));
         if (!inputString(_l("Translate"),oldtrans,newtrans,1024))
          break;
        }
       tr->newTranslation((*page)->dialogId,0,newtrans);
      }
    }
   translate();
   return true;
  }
 return false;
}

void TffdshowPageBase::enableHints(bool is)
{
 cfgSet(IDFF_showHints,is);
 for (ThtiPages::const_iterator hp=htiPages.begin();hp!=htiPages.end();hp++)
  for (TconfPages::const_iterator i=(*hp)->begin();i!=(*hp)->end();i++)
   (*i)->enableHints(is);
}
void TffdshowPageBase::setChange(void)
{
 dirty=true;
 if (pageSite) pageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
}

void TffdshowPageBase::onHelp(void)
{
 if (!page || !page->helpURL) return;
 char_t fullflnm[MAX_PATH];
 bool internet=false;
 if(strlen(page->helpURL)>6 && strncmp(page->helpURL,_l("http"),4)==0)
  {
   internet=true;
   const char_t *translatedURL=_(-1,page->helpURL);
   ff_strncpy(fullflnm, translatedURL, countof(fullflnm));
  }
 else
  {
   _makepath_s(fullflnm,MAX_PATH,NULL,config->pth,_l("help\\"),NULL);
   strncat_s(fullflnm, countof(fullflnm), page->helpURL, _TRUNCATE);
  }
 if (internet || fileexists(fullflnm))
  ShellExecute(m_hwnd,_l("open"),fullflnm,NULL,_l("."),SW_SHOWNORMAL);
}

STDMETHODIMP TffdshowPageBase::SetPageSite(LPPROPERTYPAGESITE IpageSite)
{
 if (IpageSite)
  {
   if (pageSite)
    return E_UNEXPECTED;
   pageSite=IpageSite;
   pageSite->AddRef();
  }
 else
  {
   if (!pageSite)
    return E_UNEXPECTED;
   pageSite->Release();
   pageSite=NULL;
  }
 return S_OK;
}

STDMETHODIMP TffdshowPageBase::SetObjects(ULONG cObjects,LPUNKNOWN *ppUnk)
{
 if (cObjects==1)
  {
   if (!ppUnk || !*ppUnk) return E_POINTER;
   wasobjectset=true;
   return OnConnect(*ppUnk);
  }
 else if (cObjects==0)
  {
   wasobjectset=false;
   return OnDisconnect();
  }
 else
  return E_UNEXPECTED;
}

HRESULT TffdshowPageBase::OnConnect(IUnknown *pUnk)
{
 DPRINTF(_l("On connect 1"));
 if (!(deci=pUnk)) return E_NOINTERFACE;
 ASSERT(deci);
 deci->initDialog();
 deci->loadDialogSettings();
 setDeci(deci);
 DPRINTF(_l("On connect 2"));
 return S_OK;
}
HRESULT TffdshowPageBase::OnDisconnect(void)
{
 if (deci) deci->doneDialog();
 deci=NULL;
 return S_OK;
}

STDMETHODIMP TffdshowPageBase::Activate(HWND hwndParent,LPCRECT rect,BOOL fModal)
{
 CheckPointer(rect,E_POINTER);

 if (!wasobjectset) return E_UNEXPECTED;
 if (m_hwnd) return E_UNEXPECTED;

 HWND hwnd=createDialog(dialogId,hwndParent);
 if (hwnd==NULL) return E_OUTOFMEMORY;
 //setHWND(hwnd);
 translate();

 Move(rect);
 Show(SW_SHOWNORMAL);

 //LOGFONT lf={-13L,-13L,0L,0L,FW_NORMAL,0,0,0,SYMBOL_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FF_ROMAN+VARIABLE_PITCH,"Webdings"};
 LOGFONT lf={18L,0L,0L,0L,FW_NORMAL,0,0,0,SYMBOL_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,FF_DONTCARE+DEFAULT_PITCH,_l("Webdings")};
 arrowsFont=CreateFontIndirect(&lf);

 dlg=findParentDlg();
 if (IsWindow(dlg))
  {
   if (resorePos && cfgGet(IDFF_dlgRestorePos))
    {
     int x,y;
     x=cfgGet(IDFF_dlgPosX);
     y=cfgGet(IDFF_dlgPosY);
     WINDOWPLACEMENT wpl;
     wpl.length=sizeof(wpl);
     GetWindowPlacement(dlg,&wpl);
     OffsetRect(&wpl.rcNormalPosition,x-wpl.rcNormalPosition.left,y-wpl.rcNormalPosition.top);
     SetWindowPlacement(dlg,&wpl);
    }
   if (cfgGet(IDFF_alwaysOnTop))
    SetWindowPos(dlg,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
  }

 htv=GetDlgItem(m_hwnd,IDC_TV_TREE);
 SetWindowLong(htv,GWL_STYLE,TVS_NOHSCROLL|GetWindowLong(htv,GWL_STYLE));

 hil=ImageList_Create(16,16,ILC_COLOR|ILC_MASK,2,2);
 ilClear  =ImageList_Add(hil,bmp1[0]=LoadBitmap(hi,MAKEINTRESOURCE(IDB_CLEAR  )),bmp2[0]=LoadBitmap(hi,MAKEINTRESOURCE(IDB_CHB_MASK      )));
 ilChecked=ImageList_Add(hil,bmp1[1]=LoadBitmap(hi,MAKEINTRESOURCE(IDB_CHECKED)),bmp2[1]=LoadBitmap(hi,MAKEINTRESOURCE(IDB_CHB_MASK      )));
 ilArrowUD=ImageList_Add(hil,bmp1[2]=LoadBitmap(hi,MAKEINTRESOURCE(IDB_ARROWS )),bmp2[2]=LoadBitmap(hi,MAKEINTRESOURCE(IDB_ARROWS_MASK_UD)));
 ilArrowU =ImageList_Add(hil,bmp1[3]=LoadBitmap(hi,MAKEINTRESOURCE(IDB_ARROWS )),bmp2[3]=LoadBitmap(hi,MAKEINTRESOURCE(IDB_ARROWS_MASK_U )));
 ilArrowD =ImageList_Add(hil,bmp1[4]=LoadBitmap(hi,MAKEINTRESOURCE(IDB_ARROWS )),bmp2[4]=LoadBitmap(hi,MAKEINTRESOURCE(IDB_ARROWS_MASK_D )));

 onActivate();
 static const TanchorInfo ainfo[]=
  {
   IDC_PLACE_PAGE,TanchorInfo::LEFT|TanchorInfo::RIGHT|TanchorInfo::TOP|TanchorInfo::BOTTOM,
   0,0
  };
 anchors.add(ainfo,*this);
 TreeView_SetIndent(htv,24);
 //TreeView_SetItemHeight(htv,26);

 showShowHide();

 int lastPage=cfgGet(IDFF_lastPage);
 treeSelectItem(lastPage);
 enableHints(!!cfgGet(IDFF_showHints));
 deci->setCfgDlgHwnd(dlg);
 deci->setOnChangeMsg(m_hwnd,WM_FFONCHANGE);
 deci->setOnFrameMsg(m_hwnd,WM_FFONFRAME);
 return NOERROR;
}

void TffdshowPageBase::treeSelectItem(int uniqueId,int prevVisId)
{
 for (HTREEITEM hti=TreeView_GetRoot(htv);hti;hti=TreeView_GetNextVisible(htv,hti))
  {
   TconfPageBase *page=hti2page(hti);
   if (page->uniqueID()==uniqueId)
    {
     TreeView_SelectItem(htv,hti);
     if (prevVisId)
      for (HTREEITEM htivis=TreeView_GetRoot(htv);htivis;htivis=TreeView_GetNextVisible(htv,htivis))
       {
        TconfPageBase *pagevis=hti2page(htivis);
        if (pagevis->uniqueID()==prevVisId)
         TreeView_EnsureVisible(htv,htivis);
       }
     return;
    }
  }
}

void TffdshowPageBase::saveDialogSettings(void)
{
 if (IsWindow(dlg))
  {
   WINDOWPLACEMENT wpl;
   wpl.length=sizeof(wpl);
   GetWindowPlacement(dlg,&wpl);
   cfgSet(IDFF_dlgPosX,wpl.rcNormalPosition.left);
   cfgSet(IDFF_dlgPosY,wpl.rcNormalPosition.top);
  }
 deci->saveDialogSettings();
}

STDMETHODIMP TffdshowPageBase::Deactivate(void)
{
 deci->setOnChangeMsg(NULL,0);
 deci->setOnFrameMsg(NULL,0);

 if (m_hwnd==NULL) return E_UNEXPECTED;

 // Remove WS_EX_CONTROLPARENT before DestroyWindow call
 DWORD dwStyle = GetWindowLong(m_hwnd,GWL_EXSTYLE);
 dwStyle=dwStyle&~WS_EX_CONTROLPARENT;
 // Set m_hwnd to be NULL temporarily so the message handler
 // for WM_STYLECHANGING doesn't add the WS_EX_CONTROLPARENT
 // style back in
 HWND hwnd = m_hwnd;
 m_hwnd=NULL;
 SetWindowLong(hwnd,GWL_EXSTYLE,dwStyle);
 m_hwnd=hwnd;
 // Destroy the dialog window
 DestroyWindow(m_hwnd);
 m_hwnd=NULL;

 saveDialogSettings();
 deci->setCfgDlgHwnd(NULL);
 return S_OK;
}

STDMETHODIMP TffdshowPageBase::Apply(void)
{
 if (wasobjectset==FALSE) return E_UNEXPECTED;
 if (pageSite==NULL) return E_UNEXPECTED;
 if (!dirty) return NOERROR;
 HRESULT hr=OnApplyChanges();
 if (SUCCEEDED(hr))
  dirty=false;
 return hr;
}

HRESULT TffdshowPageBase::OnApplyChanges(void)
{
 cfgSet(IDFF_applying,1);
 for (ThtiPages::const_iterator hp=htiPages.begin();hp!=htiPages.end();hp++)
  for (TconfPages::const_iterator i=(*hp)->begin();i!=(*hp)->end();i++)
   if ((*i)->m_hwnd)
    (*i)->applySettings();
 deci->saveGlobalSettings();
 onApplyChanges();
 if (page && tr->translateMode && page->dialogId!=IDD_ABOUT)
  {
   HWND hed=GetDlgItem(page->m_hwnd,IDC_ED_HELP);
   if (hed && SendMessage(hed,EM_GETMODIFY,0,0))
    {
     int len=GetWindowTextLength(hed);
     char_t *text=(char_t*)_alloca((len+2)*sizeof(char_t));
     GetWindowText(hed,text,len+1);
     tr->newTranslation(page->dialogId,IDC_ED_HELP,text);
    }
  }
 deci->putParam(IDFF_applying,0);
 return S_OK;
}

void TffdshowPageBase::deletePage(ThtiPage *htiPage)
{
 ThtiPages::iterator p=std::find(htiPages.begin(),htiPages.end(),htiPage);
 if (p!=htiPages.end())
  {
   if ((*p)->hti)
    TreeView_DeleteItem(htv,(*p)->hti);
   delete *p;
   htiPages.erase(p);
  }
}

INT_PTR TffdshowPageBase::msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_DESTROY:
    {
     for (ThtiPages::const_iterator htiPage=htiPages.begin();htiPage!=htiPages.end();htiPage++)
      delete *htiPage;
     htiPages.clear();
     page=NULL;
     if (arrowsFont) DeleteObject(arrowsFont);arrowsFont=NULL;
     if (hil) ImageList_Destroy(hil);hil=NULL;
     for (int i=0;i<countof(bmp1);i++)
      {
       DeleteObject(bmp1[i]);
       DeleteObject(bmp2[i]);
      }
     break;
    }
   case WM_FFONCHANGE:
    DPRINTF(_l("onChangeParam"));
    if (wParam==IDFF_alwaysOnTop && IsWindow(dlg))
     SetWindowPos(dlg,lParam?HWND_TOPMOST:HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
    if (wParam!=0)
     setChange();
    return TRUE;
   case WM_FFONFRAME:
    {
     MSG msg;
     if (PeekMessage(&msg,m_hwnd,WM_FFONFRAME,WM_FFONFRAME,PM_NOREMOVE))
      return TRUE; // drop msg when next WM_FFONFRAME is already in the message queue.
     for (ThtiPages::const_iterator hp=htiPages.begin();hp!=htiPages.end();hp++)
      for (TconfPages::const_iterator i=(*hp)->begin();i!=(*hp)->end();i++)
       if ((*i)->m_hwnd)
        (*i)->onFrame();
     return TRUE;
    }
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDC_BT_HELP:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         onHelp();
         return TRUE;
        }
       break;
     }
    break;
   case WM_CONTEXTMENU:
    if (translateTV())
     return TRUE;
    break;
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (nmhdr->hwndFrom==htv && nmhdr->idFrom==IDC_TV_TREE)
      switch (nmhdr->code)
       {
        case TVN_SELCHANGED:
         {
          NMTREEVIEW *nmtv=LPNMTREEVIEW(lParam);
          if (nmtv->itemNew.hItem)
           {
            selectPage(hti2page(nmtv->itemNew.hItem));
            return TRUE;
           }
          break;
         }
        case TVN_ITEMEXPANDING:
         {
          NMTREEVIEW *nmtv=LPNMTREEVIEW(lParam);
          if (nmtv->action==TVE_COLLAPSE) setDlgResult(TRUE);
          return TRUE;
         }
       }
     break;
    }
  }
 return Twindow::msgProc(uMsg,wParam,lParam);
}
INT_PTR TffdshowPageBase::defMsgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 // we would like the TAB key to move around the tab stops in our property
 // page, but for some reason OleCreatePropertyFrame clears the CONTROLPARENT
 // style behind our back, so we need to switch it back on now behind its
 // back.  Otherwise the tab key will be useless in every page.

 if (m_hwnd==NULL)
  return 0;
 switch (uMsg)
  {
   case WM_STYLECHANGING:
    if (wParam==GWL_EXSTYLE)
     {
      LPSTYLESTRUCT lpss=(LPSTYLESTRUCT)lParam;
      lpss->styleNew|=WS_EX_CONTROLPARENT;
      return 0;
     }
  }
 return DefWindowProc(m_hwnd,uMsg,wParam,lParam);
}


STDMETHODIMP TffdshowPageBase::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 CheckPointer(ppv,E_POINTER);
 if (riid==IID_IPropertyPageFF)
  return GetInterface<IPropertyPageFF>(this,ppv);
 else if (riid==IID_IPropertyPage)
  return GetInterface<IPropertyPage>(this,ppv);
 else
  return CUnknown::NonDelegatingQueryInterface(riid,ppv);
}

STDMETHODIMP_(ULONG) TffdshowPageBase::NonDelegatingAddRef(void)
{
 LONG lRef = InterlockedIncrement((LONG*)&m_cRef);
 ASSERT(lRef > 0);
 return std::max(ULONG(m_cRef),1ul);
}
// Release a reference count and protect against reentrancy
STDMETHODIMP_(ULONG) TffdshowPageBase::NonDelegatingRelease(void)
{
 // If the reference count drops to zero delete ourselves
 if (InterlockedDecrement((LONG*)&m_cRef) == 0)
  {
   m_cRef++;
   SetPageSite(NULL);
   SetObjects(0,NULL);
   delete this;
   return ULONG(0);
  }
 else
  return std::max(ULONG(m_cRef),1ul);
}

STDMETHODIMP TffdshowPageBase::toTop(void)
{
 if (!m_hwnd) return E_UNEXPECTED;
 SetWindowPos(m_hwnd,HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
 return S_OK;
}
STDMETHODIMP TffdshowPageBase::resize(const CRect &newrect)
{
 setSize(newrect.Width(),newrect.Height());
 anchors.resize(*this,newrect);
 if (page)
  page->resize(getChildRect(IDC_PLACE_PAGE));
 return S_OK;
}

HRESULT STDMETHODCALLTYPE TffdshowPageBase::TranslateAccelerator(MSG *pMsg)
{
 return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE TffdshowPageBase::Help(LPCOLESTR pszHelpDir)
{
 return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE TffdshowPageBase::IsPageDirty(void)
{
 return dirty?S_OK:S_FALSE;
}
HRESULT STDMETHODCALLTYPE TffdshowPageBase::Move(LPCRECT pRect)
{
 CheckPointer(pRect,E_POINTER);
 if (m_hwnd==NULL) return E_UNEXPECTED;
 MoveWindow(m_hwnd,pRect->left,pRect->top,pRect->right-pRect->left,pRect->bottom-pRect->top,TRUE);
 return S_OK;
}
HRESULT STDMETHODCALLTYPE TffdshowPageBase::Show(UINT nCmdShow)
{
 if (m_hwnd==NULL) return E_UNEXPECTED;
 // Ignore wrong show flags
 if (nCmdShow!=SW_SHOW && nCmdShow!=SW_SHOWNORMAL && nCmdShow!=SW_HIDE)
  return E_INVALIDARG;
 ShowWindow(m_hwnd,nCmdShow);
 repaint(m_hwnd);
 return NOERROR;
}
