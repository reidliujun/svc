#include <string.h>
#include "neaacdec.h"
#include "../../compiler.h"

void __stdcall getVersion(char *ver,const char* *license)
{
 strcpy(ver, FAAD2_VERSION", build date "__DATE__" "__TIME__" ("COMPILER COMPILER_X64")");
 *license="FAAD2 AAC/HE-AAC/HE-AACv2/DRM decoder (c) Nero AG, www.nero.com";
}
