/*
 * Copyright (c) 2005,2006 Milan Cutka
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
#include "resource.h"
#include "TpageSite.h"
#include "IffdshowBase.h"
#include "Ttranslate.h"
#include "Cabout.h"

STDMETHODIMP TpageSite::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 if (riid==IID_IPropertyPageSite)
  return GetInterface<IPropertyPageSite>(this,ppv);
 else if (riid==IID_IPropertyPageSiteFF)
  return GetInterface<IPropertyPageSiteFF>(this,ppv);
 else
  return CUnknown::NonDelegatingQueryInterface(riid,ppv);
}

TpageSite::TpageSite(int Iidff_multiplePages):
 CUnknown(NAME("TpageSite"),NULL),TdlgWindow(IDD_PAGESITE),prevrect(0,0,0,0),
 idff_multiplePages(Iidff_multiplePages)
{
 AddRef();
 activepage=NULL;
 strcpy(regflnm,_l("ffdshow.reg"));
 hicon=NULL;
}
TpageSite::~TpageSite()
{
}

INT_PTR TpageSite::show(IffdshowBase *deci,HWND parent,int Iidcaption,int Iicon)
{
 if (comptrQ<ISpecifyPropertyPages> ppages=deci)
  {
   CAUUID pages;
   ppages->GetPages(&pages);
   comptr<IUnknown> deciU;
   deci->QueryInterface(IID_IUnknown,(void**)&deciU);
   return show(deci,parent,Iidcaption,Iicon,pages,deciU);
  }
 else
  return E_FAIL;
}
INT_PTR TpageSite::show(IffdshowBase *deci,HWND parent,int Iidcaption,int Iicon,CAUUID &pages,IUnknown *unk)
{
 for (ULONG i=0;i<pages.cElems;i++)
  {
   IPropertyPage *page;
   if (CoCreateInstance(pages.pElems[i],NULL,CLSCTX_INPROC_SERVER,IID_IPropertyPage,(void**)&page)==S_OK)
    {
     page->SetPageSite(this);
     page->SetObjects(1,&unk);
     propertypages.push_back(page);
    }
  }
 CoTaskMemFree(pages.pElems);
 if (!propertypages.empty())
  {
   HINSTANCE hi=deci->getInstance2();
   setDeci(deci);
   idcaption=Iidcaption;
   if (Iicon)
    hicon=LoadIcon(hi,MAKEINTRESOURCE(Iicon));
   INT_PTR res=dialogBox(dialogId,parent);
   for (TpropertyPages::iterator p=propertypages.begin();p!=propertypages.end();p++)
    (*p)->Release();
   if (hicon) DestroyIcon(hicon);
   return res;
  }
 else
  return E_FAIL;
}

INT_PTR TpageSite::msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
 switch (uMsg)
  {
   case WM_INITDIALOG:
    {
     WINDOWPLACEMENT wpl; wpl.length=sizeof(WINDOWPLACEMENT);

     GetWindowPlacement(m_hwnd,&wpl);

     if (idcaption)
      {
       LoadString(hi,idcaption,caption,256);
       strncat_s(caption, countof(caption), _l(" configuration"), _TRUNCATE);
      }
     SendMessage(m_hwnd,WM_SETICON,ICON_BIG,(LPARAM)hicon);
     int sx=0,sy=0;
     for (TpropertyPages::iterator p=propertypages.begin();p!=propertypages.end();p++)
      {
       PROPPAGEINFO ppi;memset(&ppi,0,sizeof(ppi));
       (*p)->GetPageInfo(&ppi);
       if (ppi.size.cx>sx) sx=ppi.size.cx;
       if (ppi.size.cy>sy) sy=ppi.size.cy;
       if (ppi.pszTitle) CoTaskMemFree(ppi.pszTitle);
       if (ppi.pszHelpFile) CoTaskMemFree(ppi.pszHelpFile);
       if (ppi.pszDocString) CoTaskMemFree(ppi.pszDocString);
      }
     pagerect=CRect(0,0,sx,sy);
     CRect siterect;GetWindowRect(GetDlgItem(m_hwnd,IDC_PLACE_PAGE),&siterect);
     htab=GetDlgItem(m_hwnd,IDC_TAB);
     CRect tabrect;GetWindowRect(htab,&tabrect);

     static const TanchorInfo ainfo[]=
      {
       IDC_BT_APPLY,TanchorInfo::BOTTOM|TanchorInfo::RIGHT,
       IDOK,TanchorInfo::BOTTOM|TanchorInfo::RIGHT,
       IDCANCEL,TanchorInfo::BOTTOM|TanchorInfo::RIGHT,
       IDC_BT_EXPORT,TanchorInfo::BOTTOM|TanchorInfo::RIGHT,
       IDC_BT_DONATE,TanchorInfo::BOTTOM|TanchorInfo::LEFT,
       IDC_LINE,TanchorInfo::BOTTOM|TanchorInfo::LEFT|TanchorInfo::RIGHT,
       IDC_TAB,TanchorInfo::LEFT|TanchorInfo::TOP|TanchorInfo::RIGHT|TanchorInfo::BOTTOM,
       0,0
      };
     anchors.add(ainfo,*this);
     if (propertypages.size()==1)
      {
       Twindow::show(0,IDC_TAB);
       CPoint dif(tabrect.left-siterect.left,tabrect.top-siterect.top);
       siterect+=dif;
       setSizeD(dif.x,dif.y);
      }
     else
      {
       for (size_t i=0;i<propertypages.size();i++)
        {
         TCITEM tci;
         tci.mask=TCIF_TEXT;
         PROPPAGEINFO ppi;
         propertypages[i]->GetPageInfo(&ppi);
         char_t title[512];
         text<char_t>(ppi.pszTitle, -1, title, countof(title));//unicode16toAnsi(ppi.pszTitle,-1,title);
         tci.pszText=title;
         TabCtrl_InsertItem(htab,i,&tci);
        }
       setSizeD(4,0);
      }
     static const TanchorInfo ainfo2[]=
      {
       IDC_PLACE_PAGE,TanchorInfo::LEFT|TanchorInfo::TOP|TanchorInfo::RIGHT|TanchorInfo::BOTTOM,
       0,0
      };
     anchors.add(ainfo2,*this);

     CPoint sitetl=siterect.TopLeft();
     siterect-=siterect.TopLeft();
     int dx=pagerect.right-siterect.right,dy=pagerect.bottom-siterect.bottom;
     setSizeD(dx,dy);

     ScreenToClient(m_hwnd,&sitetl);
     pagerect+=sitetl;
     setActivePage(propertypages.front());
     if (idff_multiplePages)
      {
       int pg=deci->getParam2(idff_multiplePages);
       PostMessage(htab,TCM_SETCURSEL,pg,0);
       setActivePage(propertypages[pg]);
      }
     addHint(IDC_BT_DONATE,_l("Thank you!"));

     // Re-center the %&#@*! pagesite dialog

     if (!cfgGet(IDFF_dlgRestorePos))
      {
       int xDiff=((wpl.rcNormalPosition.right-wpl.rcNormalPosition.left)-pagerect.Width())/2;

       GetWindowPlacement(m_hwnd,&wpl);

       wpl.rcNormalPosition.left+=xDiff;
       wpl.rcNormalPosition.right+=xDiff;

       SetWindowPlacement(m_hwnd,&wpl);
      }

     return TRUE;
    }
   case WM_SIZE:
    {
     CRect r;GetWindowRect(m_hwnd,&r);
     resize(r);
     return TRUE;
    }
   case WM_COMMAND:
    switch (LOWORD(wParam))
     {
      case IDOK:
      case IDCANCEL:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         if (idff_multiplePages)
          {
           deci->putParam(idff_multiplePages,(int)SendMessage(htab,TCM_GETCURSEL,0,0));
           deci->saveDialogSettings();
          }
         if (LOWORD(wParam)==IDOK)
          for (TpropertyPages::iterator p=propertypages.begin();p!=propertypages.end();p++)
           if ((*p)->IsPageDirty()==S_OK)
            (*p)->Apply();
         activepage->Deactivate();
         EndDialog(m_hwnd,LOWORD(wParam));
         return TRUE;
        }
       break;
      case IDC_BT_APPLY:
       if (HIWORD(wParam)==BN_CLICKED)
        applyChanges();
       return TRUE;
      case IDC_BT_DONATE:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         ShellExecute(m_hwnd,_l("open"),_l("http://order.kagi.com/?6FAEY"),NULL,_l("."),SW_SHOWNORMAL);
/*
         static const char_t *donations[]={_l("PayPal"),_l("Kagi"),NULL};
         int cmd=selectFromMenu(donations,IDC_BT_DONATE,false);
         if (isIn(cmd,0,1))
          {
           static const char_t *urls[]=
            {
             _l("https://www.paypal.com/cgi-bin/webscr?cmd=_xclick&business=milan_cutka%40yahoo%2ecom&item_name=ffdshow&page_style=PayPal&no_shipping=1&no_note=1&tax=0&currency_code=EUR&bn=PP%2dDonationsBF&charset=UTF%2d8"),
             _l("https://order.kagi.com/?6FAEY")
            };
           ShellExecute(m_hwnd,_l("open"),urls[cmd],NULL,_l("."),SW_SHOWNORMAL);
          }
*/
        }
       return TRUE;
      case IDC_BT_EXPORT:
       if (HIWORD(wParam)==BN_CLICKED)
        {
         applyChanges();
         TaboutPage::exportReg(this,regflnm);
        }
       return TRUE;
     };
    break;
   case WM_NOTIFY:
    {
     NMHDR *nmhdr=LPNMHDR(lParam);
     if (nmhdr->hwndFrom==htab && nmhdr->idFrom==IDC_TAB)
      switch (nmhdr->code)
       {
        case TCN_SELCHANGE:
         setActivePage(propertypages[TabCtrl_GetCurSel(htab)]);
         return TRUE;
       }
     break;
    }
  }
 return TdlgWindow::msgProc(uMsg,wParam,lParam);
}

void TpageSite::setActivePage(IPropertyPage *page)
{
 if (activepage)
  {
   applyChanges();
   activepage->Deactivate();
   activepageFF=NULL;
  }
 activepage=page;
 activepage->Activate(m_hwnd,&pagerect,FALSE);
 activepage->Show(SW_SHOW);
 if (activepageFF=activepage)
  activepageFF->toTop();

 enable(activepage->IsPageDirty()==S_OK,IDC_BT_APPLY);
}

HRESULT STDMETHODCALLTYPE TpageSite::OnStatusChange(DWORD dwFlags)
{
 if (dwFlags&PROPPAGESTATUS_DIRTY) setDirty(1);
 if (dwFlags&PROPPAGESTATUS_CLEAN) setDirty(0);
 return S_OK;
}
void TpageSite::setDirty(int d)
{
 enable(d,IDC_BT_APPLY);
}
void TpageSite::applyChanges(void)
{
 activepage->Apply();
 setDirty(activepage->IsPageDirty()==S_OK);
}
HRESULT STDMETHODCALLTYPE TpageSite::GetLocaleID(LCID *pLocaleID)
{
 if (!pLocaleID) return E_POINTER;
 *pLocaleID=GetUserDefaultLCID();
 return S_OK;
}
HRESULT STDMETHODCALLTYPE TpageSite::GetPageContainer(IUnknown **ppUnk)
{
 return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE TpageSite::TranslateAccelerator(MSG *pMsg)
{
 return E_NOTIMPL;
}

void TpageSite::resize(CRect &newrect)
{
 if (prevrect.Width()!=0)
  {
   anchors.resize(*this,newrect);
   if (activepage)
    {
     CRect r;GetWindowRect(GetDlgItem(m_hwnd,IDC_PLACE_PAGE),&r);
     SetWindowPos(GetDlgItem(m_hwnd,IDC_PLACE_PAGE),NULL,0,0,r.Width(),r.Height(),SWP_NOMOVE|SWP_NOZORDER);
     if (activepageFF) activepageFF->resize(r);
    }
  }
 prevrect=newrect;
}

STDMETHODIMP TpageSite::onTranslate(void)
{
 Twindow::translate();
 if (idcaption)
  setWindowText(m_hwnd,_(-dialogId,caption));
 return S_OK;
}
