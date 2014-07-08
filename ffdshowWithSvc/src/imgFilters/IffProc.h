#ifndef _IFFPROC_H_
#define _IFFPROC_H_

#ifdef __cplusplus
extern "C" {
#endif

DEFINE_GUID(IID_IffProc  , 0xff23a288, 0x1025, 0x46b1, 0xb7, 0xa0, 0x78, 0x99, 0x17, 0xcc, 0x40, 0x75); // {FF23A288-1025-46b1-B7A0-789917CC4075}
DEFINE_GUID(IID_IffProc2 , 0x51c8e9df, 0x0285, 0x475a, 0xb1, 0x1d, 0xc3, 0xa8, 0x2a, 0xb2, 0x27, 0x38); // {51C8E9DF-0285-475a-B11D-C3A82AB22738}
DEFINE_GUID(IID_IffProc3 , 0x73d95667, 0x0cb4, 0x4ae6, 0x80, 0x08, 0xeb, 0xa3, 0xaf, 0x10, 0x66, 0x13); // {73D95667-0CB4-4ae6-8008-EBA3AF106613}
DEFINE_GUID(IID_IffProc4 , 0xbb3f4bc1, 0x5601, 0x4fdd, 0xa0, 0xc5, 0x4b, 0xb1, 0xd1, 0xae, 0xed, 0xab); // {BB3F4BC1-5601-4fdd-A0C5-4BB1D1AEEDAB}
DEFINE_GUID(IID_IffProc5 , 0x1eddd55e, 0xc4fe, 0x43cf, 0x94, 0xb8, 0xf7, 0xf6, 0xa0, 0x28, 0xc5, 0xa3); // {1EDDD55E-C4FE-43cf-94B8-F7F6A028C5A3}
DEFINE_GUID(IID_IffProc6 , 0xa3dac012, 0xd711, 0x4da3, 0x96, 0xc3, 0xc9, 0x6f, 0x67, 0xaa, 0xa4, 0x4e); // {A3DAC012-D711-4da3-96C3-C96F67AAA44E}
DEFINE_GUID(IID_IffProc7 , 0x2c9f2569, 0xb35c, 0x4140, 0x93, 0x98, 0x26, 0xec, 0xae, 0x55, 0x5f, 0xf2); // {2C9F2569-B35C-4140-9398-26ECAE555FF2}
DEFINE_GUID(IID_IffProc8 , 0x13cd13b6, 0x80b9, 0x483f, 0x9a, 0x17, 0x5d, 0xda, 0xf7, 0xdb, 0x53, 0x20); // {13CD13B6-80B9-483f-9A17-5DDAF7DB5320}
DEFINE_GUID(CLSID_TFFPROC, 0xfffcc670, 0x5cd4, 0x4c09, 0x95, 0x2c, 0xf5, 0x3f, 0x46, 0xc2, 0xb1, 0xa7); // {FFFCC670-5CD4-4c09-952C-F53F46C2B1A7}

struct TcspInfo;
struct IffdshowParamsEnum;
struct IPropertyPage;
DECLARE_INTERFACE_(IffProc, IUnknown)
{
 STDMETHOD (setTempPreset)(const char *tempPresetName) PURE;
 STDMETHOD (setActivePreset)(const char *presetName) PURE;
 STDMETHOD (loadPreset)(const char *presetName) PURE;
 STDMETHOD (saveBytestreamConfig)(void *buf,size_t len) PURE; //if len=0, then buf should point to int variable which will be filled with required buffer length
 STDMETHOD (loadBytestreamConfig)(const void *buf,size_t len) PURE;
 STDMETHOD (config)(HWND owner) PURE;
 STDMETHOD (getIffDecoder_)(/*IffdshowBase*/void* *deciPtr) PURE;
 STDMETHOD (getNewSize)(unsigned int srcDx,unsigned int srcDy,unsigned int *outDx,unsigned int *outDy) PURE;
 STDMETHOD (begin)(unsigned int srcDx,unsigned int srcDy,int FpsNum,int FpsDen) PURE;
 STDMETHOD (process)(unsigned int framenum,int incsp,const unsigned char *src[4],ptrdiff_t srcStride[4],int ro,int outcsp,unsigned char *dst[4],ptrdiff_t dstStride[4]) PURE;
 STDMETHOD (processTime)(unsigned int framenum,int64_t ref_start,int64_t ref_stop,int incsp,const unsigned char *src[4],ptrdiff_t srcStride[4],int ro,int outcsp,unsigned char *dst[4],ptrdiff_t dstStride[4]) PURE;
 STDMETHOD (end)(void) PURE;
 STDMETHOD_(const TcspInfo*,getCspInfo)(int csp) PURE;
 STDMETHOD (putStringParam)(const char *param,char sep) PURE;
 STDMETHOD (getParamsEnum)(IffdshowParamsEnum* *enumPtr) PURE;
 STDMETHOD (notifyParamsChanged)(void) PURE;
 STDMETHOD (saveActivePreset)(const char *name) PURE;
 STDMETHOD (putParam)(unsigned int paramID,int val) PURE;
 STDMETHOD (setBasePageSite)(IPropertyPage *page) PURE;
};

#ifdef __cplusplus
}
#endif

#endif
