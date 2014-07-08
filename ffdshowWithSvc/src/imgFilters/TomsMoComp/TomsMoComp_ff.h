#ifndef _TOMSMOCOMP_FF_H_
#define _TOMSMOCOMP_FF_H_

#define FF_CSP_ONLY
#include "../ffImgfmt.h"
#undef FF_CSP_ONLY

DECLARE_INTERFACE(ItomsMoComp)
{
 STDMETHOD_(bool,create)(bool isSSE2,bool isMMX2,bool is3dnow,bool _IsYUY2, int _TopFirst, int _Search_Effort, bool _Use_Vertical_Filter,int &width,int &height,stride_t srcStride[4],stride_t dstStride[4]) PURE;
 STDMETHOD_(void,GetFrame)(int inFrame,int tff,
                           const unsigned char *srcs[3],
                           unsigned char *dsts[3],
                           const unsigned char *psrcs[3]) PURE;
 STDMETHOD_(void,destroy)(void) PURE;
};

#endif
