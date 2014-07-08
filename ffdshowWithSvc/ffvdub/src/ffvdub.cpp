/*
 * Copyright (c) 2002-2004 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include <new>
#include "Tffvdub.h"
#include "resource.h"

extern "C" void bz_internal_error ( int errcode )
{
}

//=================================== Tffvdub ==================================
static INT_PTR CALLBACK configProc(HWND m_hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
 switch (msg)
  {
   case WM_INITDIALOG:
    SetWindowLongPtr(m_hwnd,GWLP_USERDATA,lParam);
    break;
  };
 FilterActivation *fa=(FilterActivation*)GetWindowLongPtr(m_hwnd,GWLP_USERDATA);
 if (fa)
  {
   Tffvdub *self=(Tffvdub*)fa->filter_data;
   return self->config(fa,m_hwnd,msg,wParam,lParam);
  }
 else
  return FALSE;
}

static int __cdecl ffvdubInit(FilterActivation *fa, const FilterFunctions *ff)
{
 new(fa->filter_data) Tffvdub;
 return 0;
}
static void __cdecl ffvdubDeinit(FilterActivation *fa, const FilterFunctions *ff)
{
 ((Tffvdub*)fa->filter_data)->~Tffvdub();
}
static void __cdecl ffvdubCopy(FilterActivation *fa, const FilterFunctions *ff, void *dst)
{
 new(dst)Tffvdub((Tffvdub*)fa->filter_data);
}
static bool __cdecl ffvdubFss(FilterActivation *fa, const FilterFunctions *ff, char *buf, int buflen)
{
 Tffvdub *self=(Tffvdub*)fa->filter_data;
 return self->fss(buf,buflen);
}
static int __cdecl ffvdubRun(const FilterActivation *fa, const FilterFunctions *ff)
{
 Tffvdub *self=(Tffvdub*)fa->filter_data;
 return self->run(fa);
}
static long __cdecl ffvdubParam(FilterActivation *fa,const FilterFunctions *ff)
{
 Tffvdub *self=(Tffvdub*)fa->filter_data;
 return self->param(fa);
}
static int __cdecl ffvdubConfig(FilterActivation *fa, const FilterFunctions *ff, HWND hwnd)
{
 return (int)DialogBoxParam(fa->filter->module->hInstModule,MAKEINTRESOURCE(IDD_FFVDUB),hwnd,configProc,(LPARAM)fa);
}
static void ffvdubScriptConfig(IScriptInterpreter *isi, void *lpVoid, CScriptValue *argv, int argc)
{
 FilterActivation *fa=(FilterActivation*)lpVoid;
 Tffvdub *self=(Tffvdub*)fa->filter_data;
 self->scriptConfig(isi,argv,argc);
}

//=================================== ffvdub definitions ===================================
static ScriptFunctionDef ffvdub_func_defs[]=
{
 { (ScriptFunctionPtr)ffvdubScriptConfig, "Config2", "0is" },
 { NULL },
};

static CScriptObject ffvbuf_obj=
{
 NULL, ffvdub_func_defs
};

static FilterDefinition ffvdubDef=
{
 NULL, NULL, NULL,             // next, prev, module
 "ffvdub",                     // name
 "Uses ffdshow image filters", // desc
 "Milan Cutka",                // maker
 NULL,                         // private_data
 sizeof(Tffvdub),              // inst_data_size

 ffvdubInit,                   // initProc
 ffvdubDeinit,                 // deinitProc
 ffvdubRun,                    // runProc
 ffvdubParam,                  // paramProc
 ffvdubConfig,                 // configProc
 NULL,                         // stringProc
 NULL,                         // startProc
 NULL,                         // endProc

 &ffvbuf_obj,                  // script_obj
 ffvdubFss,                    // fssProc
 NULL,
 NULL,
 NULL,
 ffvdubCopy
};

static FilterDefinition *ffvdub=NULL;

extern "C" int __declspec(dllexport) __cdecl VirtualdubFilterModuleInit2(FilterModule *fm, const FilterFunctions *ff, int& vdfd_ver, int& vdfd_compat)
{
 CoInitialize(NULL);
 Tffvdub *test=new Tffvdub;
 bool ok=test->ok;
 delete test;
 if (!ok || !(ffvdub=ff->addFilter(fm,&ffvdubDef,sizeof(FilterDefinition)))) return 1;
 vdfd_ver   =VIRTUALDUB_FILTERDEF_VERSION;
 vdfd_compat=VIRTUALDUB_FILTERDEF_COMPATIBLE;
 return 0;
}
extern "C" void __declspec(dllexport) __cdecl VirtualdubFilterModuleDeinit(FilterModule *fm, const FilterFunctions *ff)
{
 ff->removeFilter(ffvdub);
 CoUninitialize();
}
