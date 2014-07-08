#ifndef _IIMGFILTERGRAB_H_
#define _IIMGFILTERGRAB_H_

// {6D188C14-3C3D-4fad-B726-31076C974A83}
static const GUID IID_IimgFilterGrab = { 0x6d188c14, 0x3c3d, 0x4fad, { 0xb7, 0x26, 0x31, 0x7, 0x6c, 0x97, 0x4a, 0x83 } };

DECLARE_INTERFACE(IimgFilterGrab)
{
 STDMETHOD (grabNow)(void) PURE;
};

#endif
