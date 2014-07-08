/*
 * Copyright (c) 2003-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 * Modified by Haruhiko Yamagata <h.yamagata@nifty.com> in 2006.
 * I modified this to support multi-thread related features.
 * I am compliant to GNU General Public License.
 */

#include "stdafx.h"
#include "TffdshowBase.h"
#include "ffdshow_constants.h"
#include "TglobalSettings.h"
#include "TdialogSettings.h"
#include "TcpuUsage.h"
#include "Ttranslate.h"
#include "Tlibmplayer.h"
#include "Tlibavcodec.h"
#include "ffdebug.h"
#include "dsutil.h"
#include "IffdshowParamsEnum.h"
#include "TpageSite.h"
#include "TinputPin.h"
#include "Tinfo.h"
#include "TtrayIcon.h"

STDMETHODIMP_(int) TffdshowBase::getVersion2(void)
{
 return VERSION;
}

template<> interfaces<char_t>::IffdshowBase* TffdshowBase::getBaseInterface(void)
{
 return this;
}
template<> interfaces<tchar_traits<char_t>::other_char_t>::IffdshowBase* TffdshowBase::getBaseInterface(void)
{
 return &base_char;
}

TffdshowBase::TffdshowBase(LPUNKNOWN pUnk,TintStrColl *Ioptions,TglobalSettingsBase *IglobalSettings,TdialogSettingsBase *IdialogSettings,CmyTransformFilter *Imfilter,TinputPin* &Iminput,CTransformOutputPin* &Imoutput,IFilterGraph* &Igraph,int IcfgDlgCaptionId,int IiconId,DWORD IdefaultMerit):
 options(Ioptions),
 cfgDlgCaptionId(IcfgDlgCaptionId),
 iconId(IiconId),
 mfilter(Imfilter),
 minput(Iminput),
 moutput(Imoutput),
 graph(Igraph),
 defaultMerit(IdefaultMerit),
 Toptions(Ioptions),
 globalSettings(IglobalSettings),
 dialogSettings(IdialogSettings),
 config(Ioptions),
 info(NULL),
 base_char(pUnk,this)
{
 DPRINTF(_l("TffdshowBase::Constructor"));
 randomize();

 Ioptions->setSendOnChange(fastdelegate::MakeDelegate(this,&TffdshowBase::sendOnChange));
 static const TintOptionT<TffdshowBase> iopts[]=
  {
   IDFF_alwaysOnTop  ,&TffdshowBase::cfgDlgAlwaysOnTop ,0,0,_l(""),0,NULL,0,
   IDFF_notreg       ,&TffdshowBase::notreg            ,0,0,_l(""),0,NULL,0,
   IDFF_defaultMerit ,&TffdshowBase::defaultMerit      ,-1,-1,_l(""),0,NULL,0,
   IDFF_applying     ,&TffdshowBase::applying          ,0,0,_l(""),0,NULL,0,
   0
  };
 addOptions(iopts);
 setOnChange(IDFF_lang,this,&TffdshowBase::onLangChange);
 setOnChange(IDFF_trayIcon,this,&TffdshowBase::onTrayIconChange);

 config.init1(g_hInst);
 globalSettings->load();
 config.initCPU(globalSettings->allowedCPUflags);
 config.init2();
 firstdialog=true;
 trans=NULL;
 cfgDlgHwnd=NULL;cfgDlgAlwaysOnTop=0;
 cpu=NULL;cpuUsageCounter=0;cpuUsage=0;cpus=-1;
 onChangeWnd=NULL;onChangeMsg=0;
 onFrameWnd=NULL;onFrameMsg=0;
 applying=0;
 trayHwnd=NULL;hTrayThread=NULL;
 libmplayer=NULL;libavcodec=NULL;
 dbgfile=NULL;
 notreg=false;
 moviesecs=-1;
 memset(locks,0,sizeof(locks));
 pdwROT=0;
 curEnumInfo=enumInfos.begin();
 m_CPUCount=Tconfig::getCPUcount();
}
TffdshowBase::~TffdshowBase()
{
 for (int i=0;i<LOCKS_COUNT;i++) if (locks[i]) delete locks[i];
 if (trans) trans->release();
 if (cpu) delete cpu;
 if (libavcodec) libavcodec->Release();
 delete globalSettings;
 delete dialogSettings;
 if (dbgfile) fclose(dbgfile);
 if (libmplayer) libmplayer->Release();
 if (info) delete info;
}

STDMETHODIMP TffdshowBase::getParam(unsigned int paramID,int* value)
{
 if (!value) return S_FALSE;
 if (options->get(paramID,value))
  return S_OK;
 else
  {
   *value=0;
   return S_FALSE;
  }
}
STDMETHODIMP_(int) TffdshowBase::getParam2(unsigned int paramID)
{
 int val;
 return getParam(paramID,&val)==S_OK?val:0;
}
STDMETHODIMP TffdshowBase::putParam(unsigned int paramID,int val)
{
 return options->set(paramID,val)?S_OK:S_FALSE;
}
STDMETHODIMP TffdshowBase::invParam(unsigned int paramID)
{
 return options->inv(paramID)?S_OK:S_FALSE;
}
STDMETHODIMP TffdshowBase::resetParam(unsigned int paramID)
{
 return options->reset(paramID)?S_OK:S_FALSE;
}

STDMETHODIMP TffdshowBase::getParamStr(unsigned int paramID,char_t *buf,size_t buflen)
{
 if (!buf) return S_FALSE;
 const char_t *bufPtr;
 if (getParamStr3(paramID,&bufPtr)==S_OK)
  {
   ff_strncpy(buf,bufPtr,buflen);
   buf[buflen-1]='\0';
   return S_OK;
  }
 else
  {
   buf[0]='\0';
   return S_FALSE;
  }
}
STDMETHODIMP TffdshowBase::getParamStr3(unsigned int paramID,const char_t* *value)
{
 if (!value) return S_FALSE;
 if (options->get(paramID,value))
  return S_OK;
 else
  {
   *value=NULL;
   return S_FALSE;
  }
}
STDMETHODIMP_(const char_t*) TffdshowBase::getParamStr2(unsigned int paramID)
{
 const char_t *bufPtr=NULL;
 return getParamStr3(paramID,&bufPtr)==S_OK?bufPtr:NULL;
}
STDMETHODIMP TffdshowBase::putParamStr(unsigned int paramID,const char_t *buf)
{
 if (!buf) return S_FALSE;
 return options->set(paramID,buf)?S_OK:S_FALSE;
}

STDMETHODIMP TffdshowBase::getParamName(unsigned int paramID,char_t *buf,size_t buflen)
{
 if (!buf) return E_POINTER;
 const char_t *namePtr;
 if (getParamName3(paramID,&namePtr)==S_OK)
  {
   ff_strncpy(buf,namePtr,buflen);
   buf[buflen-1]='\0';
   return S_OK;
  }
 else
  {
   buf[0]='\0';
   return S_FALSE;
  }
}
STDMETHODIMP TffdshowBase::getParamName3(unsigned int paramID,const char_t* *namePtr)
{
 if (!namePtr) return E_POINTER;
 TffdshowParamInfo param;memset(&param,0,sizeof(param));
 if (getParamInfo(paramID,&param)!=S_OK)
  {
   *namePtr=_l("");
   return S_FALSE;
  }
 else
  {
   *namePtr=param.namePtr;
   return S_OK;
  }
}
STDMETHODIMP TffdshowBase::getParamInfo(unsigned int paramID,TffdshowParamInfo *paramPtr)
{
 if (!paramPtr) return E_POINTER;
 return options->getInfo(paramID,paramPtr)?S_OK:S_FALSE;
}

STDMETHODIMP TffdshowBase::notifyParam(int id,int val)
{
 return options->notifyParam(id,val)?S_OK:S_FALSE;
}
STDMETHODIMP TffdshowBase::notifyParamStr(int id,const char_t *val)
{
 return options->notifyParam(id,val)?S_OK:S_FALSE;
}
void TffdshowBase::sendOnChange(int paramID,int val)
{
 if (!applying && onChangeWnd && onChangeMsg &&
     paramID!=IDFF_lastPage &&
     paramID!=IDFF_applying
    )
  PostMessage(onChangeWnd,onChangeMsg,paramID,val);
}

STDMETHODIMP TffdshowBase::notifyParamsChanged(void)
{
 options->notifyParamsChanged();
 return S_OK;
}

STDMETHODIMP TffdshowBase::setOnChangeMsg(HWND wnd,unsigned int msg)
{
 onChangeWnd=wnd;onChangeMsg=msg;
 return S_OK;
}
STDMETHODIMP TffdshowBase::setOnFrameMsg(HWND wnd,unsigned int msg)
{
 onFrameWnd=wnd;onFrameMsg=msg;
 return S_OK;
}
void TffdshowBase::sendOnFrameMsg(void)
{
 if (onFrameMsg)
  PostMessage(onFrameWnd,onFrameMsg,0,0);
}
STDMETHODIMP TffdshowBase::getGlobalSettings(TglobalSettingsBase* *globalSettingsPtr)
{
 if (!globalSettingsPtr) return E_POINTER;
 *globalSettingsPtr=globalSettings;
 return S_OK;
}
STDMETHODIMP TffdshowBase::saveGlobalSettings(void)
{
 globalSettings->save();
 config.save();
 return S_OK;
}
STDMETHODIMP TffdshowBase::loadGlobalSettings(void)
{
 globalSettings->load();
 return S_OK;
}
STDMETHODIMP TffdshowBase::saveDialogSettings(void)
{
 dialogSettings->save();
 return S_OK;
}
STDMETHODIMP TffdshowBase::loadDialogSettings(void)
{
 dialogSettings->load(&config);
 return S_OK;
}

STDMETHODIMP TffdshowBase::getConfig(const Tconfig* *configPtr)
{
 if (!configPtr) return E_POINTER;
 *configPtr=&config;
 return S_OK;
}
STDMETHODIMP TffdshowBase::getInstance(HINSTANCE *hi)
{
 if (!hi) return S_FALSE;
 *hi=g_hInst;
 return S_OK;
}
STDMETHODIMP_(HINSTANCE) TffdshowBase::getInstance2(void)
{
 return g_hInst;
}

void TffdshowBase::onLangChange(int id,const char_t *newval)
{
 if (trans)
  trans->init(dialogSettings->lang,dialogSettings->translate);
}
STDMETHODIMP TffdshowBase::getTranslator(Ttranslate* *transPtr)
{
 if (!transPtr) return E_POINTER;
 if (!trans || !dialogSettings->loaded)
  {
   dialogSettings->load(&config);
   trans=new Ttranslate(g_hInst,config.pth);
   trans->init(dialogSettings->lang,dialogSettings->translate);
  }
 else
  onLangChange(0,NULL);
 trans->addref();
 *transPtr=trans;
 return S_OK;
}

STDMETHODIMP TffdshowBase::initDialog(void)
{
 if (firstdialog)
  {
   firstdialog=false;
   //loadDialogSettings();
   InitCommonControls();
   return onInitDialog();
  }
 else
  return E_UNEXPECTED;
}
STDMETHODIMP TffdshowBase::doneDialog(void)
{
 if (!firstdialog)
  {
   firstdialog=true;
   return onDoneDialog();
  }
 else
  return E_UNEXPECTED;
}

STDMETHODIMP TffdshowBase::showCfgDlg(HWND owner)
{
 if (cfgDlgHwnd) return S_FALSE;
 CoInitialize(NULL);
 TpageSite site;
 site.show(this,owner,cfgDlgCaptionId,get_trayIconType());
 CoUninitialize();
 return S_OK;
}
STDMETHODIMP_(int) TffdshowBase::getCpuUsage2(void)
{
 if (!cpu && cpus==-1)
  {
   cpu=new TcpuUsage;
   cpus=cpu->GetCPUCount();
   if (cpus==0) {delete cpu;cpu=NULL;};
  }
 if (!cpu)
  return 0;
 if(cpuUsageCounter++==0)
  {
   cpu->CollectCPUData();
   if(cpus>2)                                 // If you have 2 cpus, cpus==3. 0,1 for each core, 2 for Total
    return cpuUsage=cpu->GetCPUUsage(cpus-1); // Total
   else
    return cpuUsage=cpu->GetCPUUsage(0);
  }
 if(cpuUsageCounter==6)
  cpuUsageCounter=0;
 return cpuUsage;
}

STDMETHODIMP_(int) TffdshowBase::getCpuUsageForPP(void)
{
 if (!cpu)
  return 0;
 cpu->CollectCPUData();
 return cpu->GetCPUUsageForPP();
}

STDMETHODIMP TffdshowBase::cpuSupportsMMX(void)
{
 return (Tconfig::cpu_flags&FF_CPU_MMX)?1:0;
}
STDMETHODIMP TffdshowBase::cpuSupportsMMXEXT(void)
{
 return (Tconfig::cpu_flags&FF_CPU_MMXEXT)?1:0;
}
STDMETHODIMP TffdshowBase::cpuSupportsSSE(void)
{
 return (Tconfig::cpu_flags&FF_CPU_SSE)?1:0;
}
STDMETHODIMP TffdshowBase::cpuSupportsSSE2(void)
{
 return (Tconfig::cpu_flags&FF_CPU_SSE2)?1:0;
}
STDMETHODIMP TffdshowBase::cpuSupportsSSE3(void)
{
 return (Tconfig::cpu_flags&FF_CPU_SSE3)?1:0;
}
STDMETHODIMP TffdshowBase::cpuSupportsSSSE3(void)
{
 return (Tconfig::cpu_flags&FF_CPU_SSSE3)?1:0;
}
STDMETHODIMP TffdshowBase::cpuSupports3DNOW(void)
{
 return (Tconfig::cpu_flags&FF_CPU_3DNOW)?1:0;
}
STDMETHODIMP TffdshowBase::cpuSupports3DNOWEXT(void)
{
 return (Tconfig::cpu_flags&FF_CPU_3DNOWEXT)?1:0;
}

STDMETHODIMP TffdshowBase::getPostproc(Tlibmplayer* *postprocPtr)
{
 if (!postprocPtr) return E_POINTER;
 if (!libmplayer)
  libmplayer=new Tlibmplayer(&config);
 libmplayer->AddRef();
 *postprocPtr=libmplayer;
 return S_OK;
}

STDMETHODIMP TffdshowBase::dbgInit(void)
{
 dbgDone();
 if (globalSettings->outputdebugfile && globalSettings->debugfile)
  dbgfile=fopen(globalSettings->debugfile,_l("wt"));
 return S_OK;
}
STDMETHODIMP TffdshowBase::dbgError(const char_t *fmt,...)
{
 char_t msg[260];
 va_list args;
 va_start(args,fmt);
 int len=_vsnprintf(msg,260,fmt,args);
 va_end(args);
 dbgWrite(msg);
 if (globalSettings->errorbox) MessageBox(NULL,msg,_l("ffdshow error"),MB_ICONERROR|MB_OK);
 return S_OK;
}
STDMETHODIMP TffdshowBase::dbgWrite(const char_t *fmt,...)
{
 if (!globalSettings->outputdebug && !globalSettings->outputdebugfile) return S_FALSE;
 char_t s[260];
 va_list args;
 va_start(args,fmt);
 int len=_vsnprintf(s,256,fmt,args);
 va_end(args);
 if (len<-1) return E_INVALIDARG;
 s[256]='\0';
 if (s[len-1]!='\n') {s[len]='\n';s[len+1]='\0';}
 if (globalSettings->outputdebug) OutputDebugString(s);
 if (dbgfile && globalSettings->outputdebugfile)
  {
   fputs(s,dbgfile);
   fflush(dbgfile);
  }
 return S_OK;
}
STDMETHODIMP TffdshowBase::dbgDone(void)
{
 if (dbgfile)
  {
   fflush(dbgfile);fclose(dbgfile);
   dbgfile=NULL;
  }
 return S_OK;
}

STDMETHODIMP_(const char_t*) TffdshowBase::getExeflnm(void)
{
 return config.getExeflnm();
}
STDMETHODIMP TffdshowBase::inExplorer(void)
{
 return stricmp(getExeflnm(),_l("explorer.exe"))==0?S_OK:S_FALSE;
}

STDMETHODIMP TffdshowBase::getLibavcodec(Tlibavcodec* *libavcodecPtr)
{
 if (!libavcodecPtr) return E_POINTER;
 if (!libavcodec) libavcodec=new Tlibavcodec(&config, 0);
 if (libavcodec->ok)
  {
   libavcodec->AddRef();
   *libavcodecPtr=libavcodec;
   return S_OK;
  }
 else
  {
   *libavcodecPtr=NULL;
   return E_FAIL;
  }
}

STDMETHODIMP TffdshowBase::getLibavcodecSVC(Tlibavcodec* *libavcodecPtr)
{
 if (!libavcodecPtr) return E_POINTER;
 if (!libavcodec) libavcodec=new Tlibavcodec(&config, 1);
 if (libavcodec->ok)
  {
   libavcodec->AddRef();
   *libavcodecPtr=libavcodec;
   return S_OK;
  }
 else
  {
   *libavcodecPtr=NULL;
   return E_FAIL;
  }
}

void TffdshowBase::onTrayIconChange(int id,int newval)
{
 if (globalSettings->trayIcon && inExplorer()!=S_OK)
  if (!trayHwnd)
   {
    unsigned threadID;
    CAMEvent ev;
    hTrayThread=(HANDLE)_beginthreadex(NULL,128*1024,(unsigned(__stdcall *)(void*))trayIconStart,&std::make_tuple((IffdshowBase*)this,&ev,&trayHwnd),0,&threadID);
    ev.Wait();
   }
}

STDMETHODIMP TffdshowBase::showTrayIcon(void)
{
 onTrayIconChange(0,0);
 return S_OK;
}
STDMETHODIMP TffdshowBase::hideTrayIcon(void)
{
 if (hTrayThread && trayHwnd)
  {
   SendMessage(trayHwnd,WM_CLOSE,0,0);
   //MsgWaitForMultipleObjects(1,&hTrayThread,TRUE,INFINITE,QS_ALLEVENTS);
   WaitForSingleObject(hTrayThread,INFINITE);
   trayHwnd=NULL;
   CloseHandle(hTrayThread); // Thanks, hartlerl.
   hTrayThread= NULL;
  }
 return S_OK;
}

HRESULT TffdshowBase::onJoinFilterGraph(IFilterGraph *pGraph,LPCWSTR pName)
{
 HRESULT res;
 if (pGraph)
  {
   res=onGraphJoin(pGraph);
   if (res==S_OK)
    res=mfilter->TransformJoinFilterGraph(pGraph,pName);
  }
 else
  {
   res=mfilter->TransformJoinFilterGraph(pGraph,pName);
   if (res==S_OK)
    res=onGraphRemove();
  }
 return res;
}

HRESULT TffdshowBase::onGraphJoin(IFilterGraph *pGraph)
{
 DPRINTF(_l("Join filter graph"));
 if (!config.isDecoder[IDFF_MOVIE_MPLAYER])
  return VFW_E_CANNOT_CONNECT;

 if (globalSettings->multipleInstances==4)
  return E_FAIL;
 else if (globalSettings->multipleInstances==2)
  {
   CLSID clsid;
   mfilter->GetClassID(&clsid);
   if (searchFilter(pGraph,clsid,mfilter))
    return E_FAIL;
  }
 if (globalSettings->addToROT && !pdwROT)
  {
   comptr<IRunningObjectTable> pROT;
   if (SUCCEEDED(GetRunningObjectTable(0,&pROT)))
    {
     WCHAR wsz[256];
     wsprintfW(wsz, L"FilterGraph %08p pid %08x (ffdshow)", (DWORD_PTR)pGraph,GetCurrentProcessId());
     comptr<IMoniker> pMoniker;
     if (SUCCEEDED(CreateItemMoniker(L"!",wsz,&pMoniker)))
      pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE,(IUnknown*)pGraph,pMoniker,&pdwROT);
    }
  }
 return S_OK;
}
HRESULT TffdshowBase::onGraphRemove(void)
{
 DPRINTF(_l("Removed from filter graph"));
 if (cfgDlgHwnd) SendMessage(cfgDlgHwnd,WM_CLOSE,0,0);
 if (pdwROT)
  {
   comptr<IRunningObjectTable> pROT;
   if (SUCCEEDED(GetRunningObjectTable(0,&pROT)))
    pROT->Revoke(pdwROT);
   pdwROT=0;
  }
 hideTrayIcon();
 return S_OK;
}

STDMETHODIMP_(const char_t*) TffdshowBase::getSourceName(void)
{
 return minput?minput->getFileSourceName():_l("");
}
STDMETHODIMP_(int) TffdshowBase::getCurrentCodecId2(void)
{
 return minput?minput->getInCodecId2():CODEC_ID_NONE;
}

HRESULT TffdshowBase::checkInputConnect(IPin *pin)
{
 HRESULT res=S_OK;
 if (globalSettings->multipleInstances==3)
  {
   CLSID clsid;
   mfilter->GetClassID(&clsid);
   if (searchPrevNextFilter(PINDIR_INPUT,pin,clsid))
    res=VFW_E_INVALID_DIRECTION;
  }
 else
  {
   PIN_INFO pi;
   if (globalSettings->multipleInstances==1 && SUCCEEDED(pin->QueryPinInfo(&pi)))
    {
     CLSID inclsid,clsid;
     if (SUCCEEDED(pi.pFilter->GetClassID(&inclsid)) && SUCCEEDED(mfilter->GetClassID(&clsid)) && inclsid==clsid)
      res=VFW_E_INVALID_DIRECTION;
     pi.pFilter->Release();
    }
  }
 return res;
}

STDMETHODIMP TffdshowBase::getGraph(IFilterGraph* *graphPtr)
{
 if (!graphPtr) return E_POINTER;
 *graphPtr=graph;
 return S_OK;
}

//tell and seek
double TffdshowBase::tell(void)
{
 if (!graph) return E_UNEXPECTED;
 if (comptrQ<IMediaPosition> mpos=graph)
  {
   REFTIME pos;
   return (mpos->get_CurrentPosition(&pos)==S_OK)?pos:-1;
  }
 else
  return S_FALSE;
}
STDMETHODIMP TffdshowBase::tell(int *seconds)
{
 if (!graph) return E_UNEXPECTED;
 if (!seconds) return E_POINTER;
 *seconds=-1;
 return ((*seconds=(int)(tell()+0.5))!=-1)?S_OK:S_FALSE;
}
STDMETHODIMP TffdshowBase::seek(int seconds)
{
 if (!graph) return E_UNEXPECTED;
 if (hereSeek) return S_FALSE;
 hereSeek=true;
 if (comptrQ<IMediaPosition> mpos=graph)
  {
   HRESULT res;
   REFTIME duration;
   res=mpos->get_Duration(&duration);
   if (res==S_OK)
    res=mpos->put_CurrentPosition(limit(REFTIME(seconds),0.0,duration));
   hereSeek=false;
   return res;
  }
 else
  return S_FALSE;
}
int TffdshowBase::getDuration(void)
{
 if (!graph) return E_UNEXPECTED;
 if (comptrQ<IMediaPosition> mpos=graph)
  {
   REFTIME duration;
   mpos->get_Duration(&duration);
   return (int)duration;
  }
 else
  return -1;
}
STDMETHODIMP TffdshowBase::stop(void)
{
 if (!graph) return E_UNEXPECTED;
 if (comptrQ<IMediaControl> mc=graph)
  return mc->Stop();
 else
  return S_FALSE;
}
STDMETHODIMP_(int) TffdshowBase::getCurTime2(void)
{
 if (!graph) return E_UNEXPECTED;
 if (comptrQ<IMediaPosition> mpos=graph)
  {
   REFTIME curtime;
   return FAILED(mpos->get_CurrentPosition(&curtime))?-1:int(curtime);
  }
 else
  return -1;
}
STDMETHODIMP TffdshowBase::run(void)
{
 if (!graph) return E_UNEXPECTED;
 if (comptrQ<IMediaControl> mc=graph)
  return mc->Run();
 else
  return S_FALSE;
}
STDMETHODIMP_(int) TffdshowBase::getState2(void)
{
 if (!mfilter) return E_UNEXPECTED;
 FILTER_STATE fs;
 return FAILED(mfilter->GetState(1000,&fs))?-1:fs;
}

STDMETHODIMP TffdshowBase::frameStep(int diff)
{
 if (!graph) return E_UNEXPECTED;
 if (comptrQ<IMediaSeeking> ms=graph)
  {
   HRESULT hr;
   GUID oldtf;
   if (FAILED(hr=ms->GetTimeFormat(&oldtf)))
    return hr;
   if (oldtf!=TIME_FORMAT_FRAME)
    if (FAILED(hr=ms->SetTimeFormat(&TIME_FORMAT_FRAME)))
     return hr;
   LONGLONG curpos;
   hr=ms->GetCurrentPosition(&curpos);
   if (SUCCEEDED(hr))
    {
     curpos+=diff;
     hr=ms->SetPositions(&curpos,AM_SEEKING_AbsolutePositioning,NULL,AM_SEEKING_NoPositioning);
    }
   if (oldtf!=TIME_FORMAT_FRAME)
    ms->SetTimeFormat(&oldtf);
   return hr;
  }
 else
  return E_NOINTERFACE;
}

STDMETHODIMP TffdshowBase::getInCodecString(char_t *buf,size_t buflen)
{
 if (!buf) return E_POINTER;
 *buf='\0';
 return S_OK;
}
STDMETHODIMP TffdshowBase::getOutCodecString(char_t *buf,size_t buflen)
{
 if (!buf) return E_POINTER;
 *buf='\0';
 return S_OK;
}

STDMETHODIMP TffdshowBase::getMerit(DWORD *merit)
{
 if (!mfilter) return E_UNEXPECTED;
 CLSID clsid;mfilter->GetClassID(&clsid);
 return getFilterMerit(clsid,merit);
}
STDMETHODIMP TffdshowBase::setMerit(DWORD merit)
{
 if (!mfilter) return E_UNEXPECTED;
 CLSID clsid;mfilter->GetClassID(&clsid);
 return setFilterMerit(clsid,merit);
}
void TffdshowBase::onMeritChange(int id,int newval)
{
 setMerit(newval);
}

STDMETHODIMP TffdshowBase::lock(int lockId)
{
 if (!locks[lockId]) locks[lockId]=new CCritSec;
 locks[lockId]->Lock();
 return S_OK;
}
STDMETHODIMP TffdshowBase::unlock(int lockId)
{
 locks[lockId]->Unlock();
 return S_OK;
}

STDMETHODIMP TffdshowBase::exportRegSettings(int all,const char_t *regflnm,int unicode)
{
 return globalSettings->exportReg(!!all,regflnm,!!unicode)?S_OK:E_FAIL;
}

STDMETHODIMP TffdshowBase::getParamListItem(int paramId,int index,const char_t* *ptr)
{
 if (index<0) return S_FALSE;
 TparamListItems *items=options->getParamList(paramId);
 if (!items) return E_FAIL;
 if (index>=(int)items->size()) return S_FALSE;
 *ptr=items->at(index).second;
 delete items;
 return S_OK;
}

STDMETHODIMP TffdshowBase::abortPlayback(HRESULT hr)
{
 mfilter->NotifyEvent(EC_ERRORABORT,hr,0);
 if (moutput) moutput->DeliverEndOfStream();
 return hr;
}

STDMETHODIMP TffdshowBase::resetEnum(void)
{
 TintStrColls colls;getColls(colls);
 enumInfos.clear();
 for (TintStrColls::iterator options=colls.begin();options!=colls.end();options++)
  (*options)->getInfoIDs(enumInfos);
 curEnumInfo=enumInfos.begin();
 return S_OK;
}
STDMETHODIMP TffdshowBase::nextEnum(TffdshowParamInfo *paramPtr)
{
 if (!paramPtr) return E_POINTER;
 if (curEnumInfo==enumInfos.end()) return S_FALSE;
 TintStrColls colls;getColls(colls);
 for (TintStrColls::iterator options=colls.begin();options!=colls.end();options++)
  if ((*options)->getInfo(*curEnumInfo,paramPtr))
   break;
 curEnumInfo++;
 return S_OK;
}

void TffdshowBase::initInfo(void)
{
 lock(IDFF_lockInfo);
 if (!info) info=createInfo();
 unlock(IDFF_lockInfo);
}
STDMETHODIMP TffdshowBase::getInfoItem(unsigned int index,int *id,const char_t* *name)
{
 if (!id || !name) return E_POINTER;
 initInfo();
 return info->getInfo(index,id,name)?S_OK:S_FALSE;
}
STDMETHODIMP_(const char_t*) TffdshowBase::getInfoItemName(int id)
{
 initInfo();
 return info->getName(id);
}
STDMETHODIMP TffdshowBase::getInfoItemValue(int id,const char_t* *valuePtr,int *wasChange,int *splitline)
{
 if (!valuePtr) return E_POINTER;
 initInfo();
 const char_t *value=info->getVal(id,wasChange,splitline);
 return value?(*valuePtr=value,S_OK):S_FALSE;
}
STDMETHODIMP_(const char_t*) TffdshowBase::getInfoItemShortcut(int id)
{
 initInfo();
 return info->getShortcut(id);
}
STDMETHODIMP_(int)TffdshowBase::getInfoShortcutItem(const char_t *s,int *toklen)
{
 if (!s) return E_POINTER;
 initInfo();
 return info->getInfoShortcutItem(s,toklen);
}

void TffdshowBase::setPropsTime(IMediaSample *sample,REFERENCE_TIME t1,REFERENCE_TIME t2,AM_SAMPLE2_PROPERTIES* const pProps,BOOL *m_bSampleSkipped)
{
 if (comptrQ<IMediaSample2> pOutSample2=sample)
  {
   AM_SAMPLE2_PROPERTIES OutProps;
   EXECUTE_ASSERT(SUCCEEDED(pOutSample2->GetProperties(FIELD_OFFSET(AM_SAMPLE2_PROPERTIES,tStart),(PBYTE)&OutProps)));
   OutProps.dwTypeSpecificFlags=pProps->dwTypeSpecificFlags;
   OutProps.dwSampleFlags=(OutProps.dwSampleFlags&AM_SAMPLE_TYPECHANGED)|(pProps->dwSampleFlags&~AM_SAMPLE_TYPECHANGED);
   OutProps.tStart=t1;
   OutProps.tStop =t2;
   OutProps.cbData=FIELD_OFFSET(AM_SAMPLE2_PROPERTIES,dwStreamId);
   pOutSample2->SetProperties(FIELD_OFFSET(AM_SAMPLE2_PROPERTIES,dwStreamId),(PBYTE)&OutProps);
   if (pProps->dwSampleFlags&AM_SAMPLE_DATADISCONTINUITY)
    *m_bSampleSkipped=FALSE;
  }
 else
  {
   if (pProps->dwSampleFlags&AM_SAMPLE_TIMEVALID)
    sample->SetTime(&t1,&t2);
   if (pProps->dwSampleFlags&AM_SAMPLE_SPLICEPOINT)
    sample->SetSyncPoint(TRUE);
   if (pProps->dwSampleFlags&AM_SAMPLE_DATADISCONTINUITY)
    {
     sample->SetDiscontinuity(TRUE);
     *m_bSampleSkipped=FALSE;
    }
   /* Copy the media times
   LONGLONG MediaStart, MediaEnd;
   if (pSample->GetMediaTime(&MediaStart,&MediaEnd)==NOERROR)
    pOutSample->SetMediaTime(&MediaStart,&MediaEnd);
   */
  }
}

HRESULT TffdshowBase::getDeliveryBuffer(IMediaSample **pSample,BYTE **pData)
{
 HRESULT hr;

 *pData=NULL;
 if (FAILED(hr=moutput->GetDeliveryBuffer(pSample,NULL,NULL,0)) ||
     FAILED(hr=(*pSample)->GetPointer(pData)))
  return hr;

 AM_MEDIA_TYPE *pmt=NULL;
 if (SUCCEEDED((*pSample)->GetMediaType(&pmt)) && pmt)
  {
   CMediaType mt=*pmt;
   moutput->SetMediaType(&mt);
   DeleteMediaType(pmt);
   pmt=NULL;
  }
 return S_OK;
}

STDMETHODIMP_(DWORD) TffdshowBase::CPUcount(void)
{
 return m_CPUCount;
}
