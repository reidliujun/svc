#ifndef _STDAFX_H_
#define _STDAFX_H_

#pragma warning (disable:4511 4512)

#define _STLP_NEW_PLATFORM_SDK
#define _STLP_NO_OWN_IOSTREAMS 1

// Windows Header Files:
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// DirectX/VFW/ACM
#include <mmsystem.h>
#include <mmreg.h>
// STL
#include "../../PODtypes.h"
 #include "../../uClibc++/algorithm"
 #include "../../uClibc++/vector"
// WMV9
#ifdef __GNUC__
 #define AM_NOVTABLE
#endif
#ifndef EXTERN_GUID
 #define EXTERN_GUID(g,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8) DEFINE_GUID(g,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8)
#endif
#include <wmsdk.h>
#include <wmcodecconst.h>
// ffdshow
#include "../../inttypes.h"
#include "char_t.h"

#endif
