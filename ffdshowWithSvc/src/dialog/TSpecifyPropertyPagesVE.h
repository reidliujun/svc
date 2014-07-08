#ifndef _TSPECIFYPROPERTYPAGESVE_H_
#define _TSPECIFYPROPERTYPAGESVE_H_

#include "ISpecifyPropertyPagesVE.h"

class TSpecifyPropertyPagesVE :public CUnknown, public ISpecifyPropertyPagesVE
{
private:
 typedef std::pair<TpropertyPagesPair,IffdshowBase*> TpageI;
 TpageI first,second;
 TpageI& page(const IID &iid);
 static HRESULT commonOptionChanged(int idff,IffdshowBase *deciSrc,IffdshowBase *deciDst);
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr);
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
 DECLARE_IUNKNOWN

 TSpecifyPropertyPagesVE(LPUNKNOWN punk,HRESULT *phr);
 virtual ~TSpecifyPropertyPagesVE();

 STDMETHODIMP set(const TpropertyPagesPair &first,const TpropertyPagesPair& second);
 STDMETHODIMP_(ISpecifyPropertyPages*) get(const IID &iid);
 STDMETHODIMP setIffdshowBase(const IID &iid,IffdshowBase *Ideci);
 STDMETHODIMP commonOptionChanged(const IID &src,int idff);

 static void show2configPages(const IID &iidD,IUnknown *deciD,const IID &iidE,IUnknown *deciE,int idCaption,int icon,int idff_multiplePages);
};

#endif
