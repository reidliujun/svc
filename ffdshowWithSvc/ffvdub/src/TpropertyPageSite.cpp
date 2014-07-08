/*
 * Copyright (c) 2002-2004 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "TpropertyPageSite.h"
#include "Tffvdub.h"

CUnknown* WINAPI TpropertyPageSite::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TpropertyPageSite *pNewObject=new TpropertyPageSite(punk,phr);
 if (pNewObject==NULL) *phr=E_OUTOFMEMORY;
 return pNewObject;
}
STDMETHODIMP TpropertyPageSite::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 CheckPointer(ppv, E_POINTER);
 if (riid==IID_IPropertyPageSite) return GetInterface((TpropertyPageSite*)this,ppv);
 else return CUnknown::NonDelegatingQueryInterface(riid,ppv);
}
HRESULT STDMETHODCALLTYPE TpropertyPageSite::OnStatusChange( /* [in] */ DWORD dwFlags)
{
 ffvdub->outchanged=false;
 ffvdub->param(fa);
 if (ffvdub->outchanged)
  {
   fa->ifp->UndoSystem(0);
   fa->ifp->RedoSystem(0);
  }
 else
  fa->ifp->RedoFrame(0);
 return S_OK;
}
HRESULT STDMETHODCALLTYPE TpropertyPageSite::GetLocaleID( /* [out] */ LCID *pLocaleID)
{
 return S_OK;
}
HRESULT STDMETHODCALLTYPE TpropertyPageSite::GetPageContainer( /* [out] */ IUnknown **ppUnk)
{
 return S_OK;
}
HRESULT STDMETHODCALLTYPE TpropertyPageSite::TranslateAccelerator( /* [in] */ MSG *pMsg)
{
 return S_OK;
}

TpropertyPageSite::TpropertyPageSite(LPUNKNOWN punk,HRESULT *phr):CUnknown(NAME("TpropertyPageSite"),punk)
{
 fa=NULL;
}
TpropertyPageSite::~TpropertyPageSite()
{
}
void TpropertyPageSite::setFilterActivation(FilterActivation *Ifa)
{
 fa=Ifa;
 ffvdub=(Tffvdub*)fa->filter_data;
}
