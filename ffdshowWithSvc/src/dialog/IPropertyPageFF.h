#ifndef _IPROPERTYPAGEFF_H_
#define _IPROPERTYPAGEFF_H_

struct CRect;
DECLARE_INTERFACE_(IPropertyPageFF,IUnknown)
{
 STDMETHOD (toTop)(void) PURE;
 STDMETHOD (resize)(const CRect &newrect) PURE;
};
// {EADD2349-7089-4cb9-A899-4A60BBA1CA8B}
DEFINE_TGUID(IID,IPropertyPageFF, 0xeadd2349, 0x7089, 0x4cb9, 0xa8, 0x99, 0x4a, 0x60, 0xbb, 0xa1, 0xca, 0x8b)

DECLARE_INTERFACE_(IPropertyPageSiteFF,IUnknown)
{
 STDMETHOD (onTranslate)(void) PURE;
};
// {1C9D145F-3655-4732-BF66-A051290D9CFB}
DEFINE_TGUID(IID,IPropertyPageSiteFF, 0x1c9d145f, 0x3655, 0x4732, 0xbf, 0x66, 0xa0, 0x51, 0x29, 0xd, 0x9c, 0xfb)

#endif
