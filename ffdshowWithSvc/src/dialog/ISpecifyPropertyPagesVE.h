#ifndef _ISPECIFYPROPERTYPAGESVE_H_
#define _ISPECIFYPROPERTYPAGESVE_H_

#include "interfaces.h"

struct ISpecifyPropertyPages;
typedef std::pair<IID,ISpecifyPropertyPages*> TpropertyPagesPair;
DECLARE_INTERFACE_(ISpecifyPropertyPagesVE,IUnknown)
{
 STDMETHOD (set)(const TpropertyPagesPair &first,const TpropertyPagesPair& second) PURE;
 STDMETHOD_(ISpecifyPropertyPages*,get)(const IID &iid) PURE;
 STDMETHOD (setIffdshowBase)(const IID &iid,IffdshowBase *Ideci) PURE;
 STDMETHOD (commonOptionChanged)(const IID &src,int idff) PURE;
};
// {6ECF7699-3026-4896-8F22-CEB373C2D63D}
DEFINE_TGUID(IID,ISpecifyPropertyPagesVE, 0x6ecf7699, 0x3026, 0x4896, 0x8f, 0x22, 0xce, 0xb3, 0x73, 0xc2, 0xd6, 0x3d)

#endif
