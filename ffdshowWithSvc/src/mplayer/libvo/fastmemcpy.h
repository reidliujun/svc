#ifndef __MPLAYER_MEMCPY
#define __MPLAYER_MEMCPY

#include <stddef.h>
#include "../config.h"

#if defined(USE_FASTMEMCPY) && (defined(HAVE_MMX) || defined(HAVE_MMX2) || defined(HAVE_3DNOW) || defined(HAVE_SSE) || defined(HAVE_SSE2))
extern void* (*fast_memcpy)(void * to, const void * from, size_t len);
#define memcpy(a,b,c) fast_memcpy(a,b,c)
#endif
void init_fast_memcpy(void);

static inline void * memcpy_pic(unsigned char * dst, unsigned char * src, int bytesPerLine, int height, int dstStride, int srcStride)
{
	int i;
	void *retval=dst;

	if(dstStride == srcStride)
	{
		if (srcStride < 0) {
	    		src += (height-1)*srcStride;
	    		dst += (height-1)*dstStride;
	    		srcStride = -srcStride;
		}

		memcpy(dst, src, srcStride*height);
	}
	else
	{
		for(i=0; i<height; i++)
		{
			memcpy(dst, src, bytesPerLine);
			src+= srcStride;
			dst+= dstStride;
		}
	}

	return retval;
}

#endif
