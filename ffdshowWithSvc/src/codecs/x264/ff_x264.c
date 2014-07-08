#include <string.h>
#include <stdarg.h>
#ifdef HAVE_STDINT
#  include <stdint.h>
#else
#  include "inttypes.h"
#endif
#include "x264.h"
#include "common/common.h"
#include "encoder/ratecontrol.h"
#include "../../compiler.h"

void __stdcall getVersion(char *ver,const char* *license)
{
 strcpy(ver, X264_BUILD", build date "__DATE__" "__TIME__" ("COMPILER COMPILER_X64")");
 *license="";
}
