/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2002             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: #ifdef jail to whip a few platforms into the UNIX ideal.

 ********************************************************************/
#ifndef _OS_TYPES_H
#define _OS_TYPES_H

#ifdef _LOW_ACCURACY_
#  define X(n) (((((n)>>22)+1)>>1) - ((((n)>>22)+1)>>9))
#  define LOOKUP_T const unsigned char
#else
#  define X(n) (n)
#  define LOOKUP_T const ogg_int32_t
#endif

/* make it easy on the folks that want to compile the libs with a
   different malloc than stdlib */
#define _ogg_malloc  malloc
#define _ogg_calloc  calloc
#define _ogg_realloc realloc
#define _ogg_free    free

#if defined (__MINGW32__)
  #include "inttypes.h"
  typedef int64_t ogg_int64_t;
  typedef int32_t ogg_int32_t;
  typedef uint32_t ogg_uint32_t;
  typedef int16_t ogg_int16_t;
  typedef uint16_t ogg_uint16_t;

#else
   /* MSVC */
   typedef __int64 ogg_int64_t;
   typedef __int32 ogg_int32_t;
   typedef unsigned __int32 ogg_uint32_t;
   typedef __int16 ogg_int16_t;
   typedef unsigned __int16 ogg_uint16_t;
#endif

#endif  /* _OS_TYPES_H */
