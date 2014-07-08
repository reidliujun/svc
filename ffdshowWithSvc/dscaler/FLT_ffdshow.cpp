#pragma warning (disable:4100 4786 4512)

#define WIN32_LEAN_AND_MEAN
#define _STLP_NO_OWN_IOSTREAMS 1
#define _STLP_NEW_PLATFORM_SDK
#include <windows.h>
#include <mmsystem.h>
#include <initguid.h>
#include <limits.h>
#undef max
#include "PODtypes.h"
#if defined(UCLIBCPP) && (defined(__INTEL_COMPILER) || defined(__GNUC__) || (_MSC_VER>=1300))
 #include "uClibc++/algorithm"
 #include "uClibc++/vector"
#else
 #include <algorithm>
 #include <vector>
#endif
#include "imgFilters/avisynth/avisynth.h"
#include "inttypes.h"
#include "imgFilters/IffProc.h"
#include "imgFilters/ffImgfmt.h"
#include "imgFilters/DScaler/DS_Filter.h"
#include "IffdshowParamsEnum.h"
//#include "..\help\helpids.h"
#include "ffdebug.h"

bool begun=false;
int refcount=0;
extern FILTER_METHOD ffdshowMethod;
IffProc *proc=NULL;
int prevdx=0,prevdy=0;
typedef std::vector< std::pair<char*,char*> > Tnames;Tnames *names=NULL;

long process(TDeinterlaceInfo* pInfo)
{
 if (!proc && pInfo->PictureHistory[0]==NULL)
  return 1000;
 if (!begun || prevdx!=pInfo->FrameWidth || prevdy!=pInfo->FieldHeight)
  {
   begun=true;
   if (proc) proc->end();
   proc->begin(prevdx=pInfo->FrameWidth,prevdy=pInfo->FieldHeight,25,1);
  }
 proc->process(pInfo->CurrentFrame,FF_CSP_YUY2,(const unsigned char**)&pInfo->PictureHistory[0]->pData,&pInfo->InputPitch,0,FF_CSP_YUY2,&pInfo->PictureHistory[0]->pData,&pInfo->InputPitch);
 return 1000;
}

///////////////////////////////////////////////////////////////////////////////
//                       Start of Settings related code                      //
///////////////////////////////////////////////////////////////////////////////

SETTING FLT_ffdshowSettings=
{
 "ffdshow", ONOFF, 0, (long*)&ffdshowMethod.bActive,
 FALSE, 0, 1, 1, 1,
 NULL,
 "ffdshow", "UseFfdshow", NULL,
};

BOOL __cdecl onChange(long NewValue)
{
 proc->notifyParamsChanged();
 return FALSE;
}

void __cdecl init(void)
{
 refcount++;
 if (refcount>1) return;
 if (CoCreateInstance(CLSID_TFFPROC,NULL,CLSCTX_INPROC_SERVER,IID_IffProc8,(void**)&proc)==S_OK)
  {
   proc->loadPreset("ffdscaler");
   ffdshowMethod.pSettings=(SETTING*)malloc(sizeof(SETTING));
   ffdshowMethod.pSettings[0]=FLT_ffdshowSettings;
   ffdshowMethod.nSettings=1;
   IffdshowParamsEnum *e=NULL;
   proc->getParamsEnum(&e);
   if (e)
    {
     e->resetEnum();
     names=new Tnames;
     TffdshowParamInfo param;
     names->push_back(std::make_pair(param.valname=(char*)calloc(256,1),param.name=(char*)calloc(256,1)));
     while (e->nextEnum(&param)==S_OK)
      {
       if (!param.inPreset || param.valname[0]=='\0' || (param.type==FFDSHOW_PARAM_INT && param.min==param.max && param.min<0) || param.type==FFDSHOW_PARAM_STRING)
        continue;
       ffdshowMethod.pSettings=(SETTING*)realloc(ffdshowMethod.pSettings,sizeof(SETTING)*(ffdshowMethod.nSettings+1));
       ffdshowMethod.pSettings[ffdshowMethod.nSettings].szDisplayName=param.name[0]?param.name:param.valname;
       ffdshowMethod.pSettings[ffdshowMethod.nSettings].LastSavedValue=0;
       ffdshowMethod.pSettings[ffdshowMethod.nSettings].pValue=(long*)param.ptr;
       ffdshowMethod.pSettings[ffdshowMethod.nSettings].Default=*(int*)param.ptr;
       if (param.min==0 && param.max==0)
        {
         ffdshowMethod.pSettings[ffdshowMethod.nSettings].Type=YESNO;
         ffdshowMethod.pSettings[ffdshowMethod.nSettings].MinValue=0;
         ffdshowMethod.pSettings[ffdshowMethod.nSettings].MaxValue=1;
        }
       else
        {
         ffdshowMethod.pSettings[ffdshowMethod.nSettings].Type=SLIDER;
         if (param.min==1 && param.max==1)
          {
           ffdshowMethod.pSettings[ffdshowMethod.nSettings].MinValue=LONG_MIN/2;
           ffdshowMethod.pSettings[ffdshowMethod.nSettings].MaxValue=LONG_MAX/2;
          }
         else
          {
           ffdshowMethod.pSettings[ffdshowMethod.nSettings].MinValue=param.min;
           ffdshowMethod.pSettings[ffdshowMethod.nSettings].MaxValue=param.max;
          }
         ffdshowMethod.pSettings[ffdshowMethod.nSettings].StepValue=std::max((ffdshowMethod.pSettings[ffdshowMethod.nSettings].MaxValue-ffdshowMethod.pSettings[ffdshowMethod.nSettings].MinValue)/100L,1L);
        }
       ffdshowMethod.pSettings[ffdshowMethod.nSettings].OSDDivider=1;
       ffdshowMethod.pSettings[ffdshowMethod.nSettings].pszList=NULL;
       ffdshowMethod.pSettings[ffdshowMethod.nSettings].szIniSection="ffdshow";
       ffdshowMethod.pSettings[ffdshowMethod.nSettings].szIniEntry=param.name[0]?param.name:param.valname;
       ffdshowMethod.pSettings[ffdshowMethod.nSettings].pfnOnChange=onChange;//param.isNotify?onChange:NULL;
       ffdshowMethod.nSettings++;
       names->push_back(std::make_pair(param.valname=(char*)calloc(256,1),param.name=(char*)calloc(256,1)));
      }
     e->Release();
    }
  }
}
void __cdecl done(void)
{
 refcount--;
 if (refcount==0 && proc)
  {
   if (names)
    {
     for (Tnames::iterator n=names->begin();n!=names->end();n++)
      {
       free(n->first);
       free(n->second);
      }
     delete names;
    }
   proc->saveActivePreset(NULL);
   proc->end();
   proc->Release();
   proc=NULL;
  }
}

FILTER_METHOD ffdshowMethod=
{
 sizeof(FILTER_METHOD),
 FILTER_CURRENT_VERSION,
 DEINTERLACE_INFO_CURRENT_VERSION,
 "ffdshow",
 "&ffdshow",
 FALSE,
 TRUE,
 process,
 0,
 TRUE,
 init,
 done,
 NULL,
 1, //number of settings
 &FLT_ffdshowSettings, //FLT_ffdshowSettings,
 WM_FLT_INVERSION_GETVALUE - WM_APP,
 TRUE,
 1,
 0//IDH_INVERSION,
};

extern "C" __declspec(dllexport) FILTER_METHOD* GetFilterPluginInfo(long CpuFeatureFlags)
{
 return &ffdshowMethod;
}
