#include "cpudetect.h"
#include "config.h"
#include <string.h>
#include "fastmemcpy.h"
#include "compiler.h"

CpuCaps gCpuCaps;

void init_mplayer(int mmx,int mmx2,int _3dnow,int _3dnowExt,int sse,int sse2,int ssse3)
{
 gCpuCaps.hasMMX=mmx;
 gCpuCaps.hasMMX2=mmx2;
 gCpuCaps.has3DNow=_3dnow;
 gCpuCaps.has3DNowExt=_3dnowExt;
 gCpuCaps.hasSSE=sse;
 gCpuCaps.hasSSE2=sse2;
 gCpuCaps.hasSSSE3=ssse3;
 init_fast_memcpy();
 // Avoid using multithread if the CPU is Pentium4-HT
 // because it is not faster at all and uses more CPU.
 // (Swscaler depends much on MMX and P4HT have only one MMX unit.)
}

void __stdcall getVersion(char *ver,const char* *license)
{
 strcpy(ver,"3.8, build date "__DATE__" "__TIME__" ("COMPILER COMPILER_X64")");
 *license="";
}
