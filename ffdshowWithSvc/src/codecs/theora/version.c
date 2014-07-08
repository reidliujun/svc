#include "../../compiler.h"
#include "toplevel.h"
#include <string.h>

void __stdcall getVersion(char *ver,const char* *license)
{
 strcpy(ver,VENDOR_STRING", build date "__DATE__" "__TIME__" ("COMPILER COMPILER_X64")");
 *license="Copyright (C) 2002-2006 Xiph.org Foundation";
}
