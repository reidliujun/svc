#include "config.h"
#include <string.h>
#include "../../../inttypes.h"
#include "mpeg2.h"
#include "../../compiler.h"

void __stdcall getVersion(char *ver,const char* *license)
{
 strcpy(ver,VERSION", build date "__DATE__" "__TIME__" ("COMPILER COMPILER_X64")");
 *license="(C) 2000-2003 Michel Lespinasse <walken@zoy.org>\n(C) 1999-2000 Aaron Holtzman <aholtzma@ess.engr.uvic.ca>";
}
