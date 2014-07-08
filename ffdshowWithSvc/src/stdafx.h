#ifndef _STDAFX_H_
#define _STDAFX_H_

#pragma warning (disable:4100 4786 4512 4097)

#define _STLP_NEW_PLATFORM_SDK
#define _STLP_NO_OWN_IOSTREAMS 1

#define _WIN32_DCOM

#ifndef __GNUC__
 #define __attribute__(x)
#endif

#ifdef __GNUC__
 #define __forceinline __attribute__((__always_inline__)) inline
#endif

#define always_inline __forceinline
#define av_always_inline __forceinline

// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <vfw.h>
// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include <time.h>
#include <assert.h>
#include <io.h>
#include <process.h>
#include <limits.h>
#include <ctype.h>
#include <wchar.h>
// STL
#include "PODtypes.h"
 #include "uClibc++/vector"
 #include "uClibc++/algorithm"
 #include "uClibc++/map"
 #include "uClibc++/list"
 #include "uClibc++/hash_map"
 #include "uClibc++/utility"
 #include "uClibc++/limits"
#include "tuple.h"
// baseclasses
#include <streams.h>
// DirectX/VFW/ACM
#include <mmreg.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <dvdmedia.h>
#include <mpconfig.h>
#include <ks.h>
#include <ksmedia.h>
#include <qnetwork.h>
#include <d3d9.h>
#include <vmr9.h>
#include "msacmdrv.h"

#ifdef __GNUC__
 #undef VISTA_SPDIF
#else
 #if defined(WINVER) && WINVER<0x0600
  #undef VISTA_SPDIF
 #endif
#endif

// ffdshow
#include "inttypes.h"
#include "dwstring.h"
#include "mem_align.h"
#include "array_allocator.h"
#include "ffglobals.h"
#include "comptr.h"

#if defined(UNICODE) && defined(__GNUC__)
 #undef TEXT
 #define TEXT(q) L##q
#endif

#endif
