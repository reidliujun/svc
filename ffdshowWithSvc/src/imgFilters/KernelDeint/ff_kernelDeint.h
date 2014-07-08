#ifndef _FF_KERNELDEINT_H_
#define _FF_KERNELDEINT_H_

#define FF_CSP_ONLY
#include "../ffImgfmt.h"
#undef FF_CSP_ONLY

DECLARE_INTERFACE(IkernelDeint)
{
 STDMETHOD_(void,getFrame)(const unsigned char *cur[3],stride_t srcStride[3],unsigned char *dst[3],stride_t dstStride[3],int bobframe) PURE;
 STDMETHOD_(void,setOrder)(int order) PURE;
 STDMETHOD_(void,destroy)(void) PURE;
};

#endif
