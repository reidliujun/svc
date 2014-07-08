#ifndef _IIMGFILTERLEVELS_H_
#define _IIMGFILTERLEVELS_H_

// {42EE9A7E-CD2E-48ef-BC90-983BA899160B}
static const GUID IID_IimgFilterLevels = { 0x42ee9a7e, 0xcd2e, 0x48ef, { 0xbc, 0x90, 0x98, 0x3b, 0xa8, 0x99, 0x16, 0xb } };

DECLARE_INTERFACE(IimgFilterLevels)
{
 STDMETHOD (getHistogram)(unsigned int dst[256]) PURE;
 STDMETHOD (getInAuto)(int *min,int *max) PURE;
};

#endif
