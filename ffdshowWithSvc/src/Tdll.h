#ifndef _TDLL_H_
#define _TDLL_H_

#include "Tconfig.h"

class Tdll
{
public:
 bool ok;
 Tdll(const char_t *dllName1,const Tconfig *config,bool explicitFullPath=false)
  {
   char_t name[MAX_PATH],ext[MAX_PATH];
   _splitpath_s(dllName1, NULL, 0, NULL, 0, name, countof(name), ext, countof(ext));
   if (config && !explicitFullPath)
    {
     char_t dllName2[MAX_PATH]; //installdir+filename+ext
     _makepath_s(dllName2, countof(dllName2), NULL, config->pth, name, ext);
     hdll=LoadLibrary(dllName2);
    }
   else
    hdll=NULL;
   if (!hdll)
    {
     hdll=LoadLibrary(dllName1);
     if (!hdll && !explicitFullPath)
      {
       if (config)
        {
         char_t dllName3[MAX_PATH]; //ffdshow.ax_path+filename+ext
         _makepath_s(dllName3, countof(dllName3), NULL, config->epth, name, ext);
         hdll=LoadLibrary(dllName3);
        }
       if (!hdll)
        {
         char_t dllName0[MAX_PATH]; //only filename+ext - let Windows find it
         _makepath_s(dllName0, countof(dllName0), NULL, NULL, name, ext);
         hdll=LoadLibrary(dllName0);
        }
      }
    }
   ok=(hdll!=NULL);
  }
 ~Tdll()
  {
   if (hdll) FreeLibrary(hdll);
  }
 HMODULE hdll;
 template<class T> __forceinline void loadFunction(T &fnc,const char *name)
  {
   fnc=hdll?(T)GetProcAddress(hdll,name):NULL;
   ok&=(fnc!=NULL);
  }
 static bool check(const char_t *dllName1,const Tconfig *config)
  {
   char_t name[MAX_PATH],ext[MAX_PATH];
   _splitpath_s(dllName1, NULL, 0, NULL, 0, name, countof(name), ext, countof(ext));
   if (config)
    {
     char_t dllName2[MAX_PATH]; //installdir+filename+ext
     _makepath_s(dllName2, countof(dllName2), NULL, config->pth, name, ext);
     if (fileexists(dllName2)) return true;
    }
   if (fileexists(dllName1)) return true;
   if (config)
    {
     char_t dllName3[MAX_PATH]; //ffdshow.ax_path+filename+ext
     _makepath_s(dllName3,MAX_PATH,NULL,config->epth,name,ext);
     if (fileexists(dllName3)) return true;
    }
   char_t dllName0[MAX_PATH]; //only filename+ext - let Windows find it
   _makepath_s(dllName0, countof(dllName0), NULL, NULL, name, ext);
   char_t dir0[MAX_PATH],*dir0flnm;
   if (SearchPath(NULL,dllName0,NULL,MAX_PATH,dir0,&dir0flnm)) return true;
   return false;
  }
};

#endif
