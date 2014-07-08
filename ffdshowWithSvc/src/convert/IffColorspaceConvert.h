#ifndef _IFFCOLORSPACECONVERT_H_
#define _IFFCOLORSPACECONVERT_H_

#define FFCOLORSPACECONVERT_NAME_L  L"ffdshow colospace converter"

// {59E1BFB0-8EA8-4e0e-8C22-EEC7A8100A12}
DEFINE_GUID(IID_IffColorspaceConvert , 0x59e1bfb0, 0x8ea8, 0x4e0e, 0x8c, 0x22, 0xee, 0xc7, 0xa8, 0x10, 0xa, 0x12);
// {87271B4E-1726-4ced-AF0D-BE675621FD29}
DEFINE_GUID(CLSID_FFCOLORSPACECONVERT, 0x87271b4e, 0x1726, 0x4ced, 0xaf, 0xd, 0xbe, 0x67, 0x56, 0x21, 0xfd, 0x29);

DECLARE_INTERFACE_(IffColorspaceConvert,IUnknown)
{
 STDMETHOD (allocPicture)(int csp,unsigned int dx,unsigned int dy,uint8_t *data[],stride_t stride[]) PURE;
 STDMETHOD (freePicture)(uint8_t *data[]) PURE;
 STDMETHOD (convert)(unsigned int dx,unsigned int dy,int incsp,uint8_t *src[],const stride_t srcStride[],int outcsp,uint8_t *dst[],stride_t dstStride[]) PURE;
 STDMETHOD (convertPalette)(unsigned int dx,unsigned int dy,int incsp,uint8_t *src[],const stride_t srcStride[],int outcsp,uint8_t *dst[],stride_t dstStride[],const unsigned char *pal,unsigned int numcolors) PURE;
};

#endif
