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
#include "IffdshowBase.h"
#include "TpageSite.h"
#include <initguid.h>
#include "TSpecifyPropertyPagesVE.h"

CUnknown* WINAPI TSpecifyPropertyPagesVE::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TSpecifyPropertyPagesVE *pNewObject=new TSpecifyPropertyPagesVE(punk,phr);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
STDMETHODIMP TSpecifyPropertyPagesVE::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 CheckPointer(ppv, E_POINTER);
 if (riid==IID_ISpecifyPropertyPagesVE)
  return GetInterface<ISpecifyPropertyPagesVE>(this,ppv);
 else
  return CUnknown::NonDelegatingQueryInterface(riid,ppv);
}

TSpecifyPropertyPagesVE::TSpecifyPropertyPagesVE(LPUNKNOWN punk,HRESULT *phr):CUnknown(NAME("TSpecifyPropertyPagesVE"),punk)
{
}
TSpecifyPropertyPagesVE::~TSpecifyPropertyPagesVE()
{
}

STDMETHODIMP TSpecifyPropertyPagesVE::set(const TpropertyPagesPair &Ifirst,const TpropertyPagesPair& Isecond)
{
 first.first=Ifirst;second.first=Isecond;
 return S_OK;
}

TSpecifyPropertyPagesVE::TpageI& TSpecifyPropertyPagesVE::page(const IID &iid)
{
 return iid==first.first.first?first:second;
}

STDMETHODIMP_(ISpecifyPropertyPages*) TSpecifyPropertyPagesVE::get(const IID &iid)
{
 return page(iid).first.second;
}

STDMETHODIMP TSpecifyPropertyPagesVE::setIffdshowBase(const IID &iid,IffdshowBase *Ideci)
{
 page(iid).second=Ideci;
 return S_OK;
}
STDMETHODIMP TSpecifyPropertyPagesVE::commonOptionChanged(const IID &src,int idff)
{
 return commonOptionChanged(idff,src==first.first.first?first.second:second.second,src==first.first.first?second.second:first.second);
}

HRESULT TSpecifyPropertyPagesVE::commonOptionChanged(int idff,IffdshowBase *deciSrc,IffdshowBase *deciDst)
{
 int valI;
 if (deciSrc->getParam(idff,&valI)==S_OK)
  return deciDst->putParam(idff,valI);
 else
  {
   const char_t *valS;
   if (deciSrc->getParamStr3(idff,&valS)==S_OK)
    return deciDst->putParamStr(idff,valS);
  }
 return S_FALSE;
}

void TSpecifyPropertyPagesVE::show2configPages(const IID &iidD,IUnknown *deciD,const IID &iidE,IUnknown *deciE,int idCaption,int icon,int idff_multiplePages)
{
 comptrQ<ISpecifyPropertyPages> isppD=deciD;
 CAUUID pagesD;
 isppD->GetPages(&pagesD);

 comptrQ<ISpecifyPropertyPages> isppE=deciE;
 CAUUID pagesE;
 isppE->GetPages(&pagesE);

 comptrQ<IffdshowBase> deci=deciD;
 if (deci)
  icon=deci->get_trayIconType();

 HRESULT hr;
 TSpecifyPropertyPagesVE *isppVE=new TSpecifyPropertyPagesVE(NULL,&hr);isppVE->AddRef();
 isppVE->set(TpropertyPagesPair(iidD,isppD),TpropertyPagesPair(iidE,isppE));
 IUnknown *ifflist[]={isppVE};
 CAUUID pages;
 pages.cElems=2;
 pages.pElems=(GUID*)CoTaskMemAlloc(2*sizeof(GUID));
 pages.pElems[0]=pagesD.pElems[0];pages.pElems[1]=pagesE.pElems[0];
 TpageSite site(idff_multiplePages);
 comptrQ<IffdshowBase> deciDB=deciD;
 site.show(deciDB,NULL,idCaption,icon,pages,isppVE);
 isppVE->Release();
 CoTaskMemFree(pagesD.pElems);CoTaskMemFree(pagesE.pElems);
}
