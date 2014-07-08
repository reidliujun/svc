#ifndef _TFFDSHOWBASE_H_
#define _TFFDSHOWBASE_H_

#include "IffdshowBase.h"
#include "IffdshowParamsEnum.h"
#include "Tconfig.h"
#include "interfaces.h"

struct TdialogSettingsBase;
class TcpuUsage;
struct Tlibmplayer;
struct Tconfig;
class TinputPin;
class TinfoBase;
struct TtrayIconStartData;
class CmyTransformFilter;
class TffdshowBase :public Toptions,
                    public IffdshowParamsEnum,
                    public interfaces<char_t>::IffdshowBase
{
public:
 TffdshowBase(LPUNKNOWN pUnk,TintStrColl *Ioptions,TglobalSettingsBase *IglobalSettings,TdialogSettingsBase *IdialogSettings,CmyTransformFilter *Imfilter,TinputPin* &Iminput,CTransformOutputPin* &Imoutput,IFilterGraph* &Igraph,int IcfgDlgCaptionId,int IiconId,DWORD IdefaultMerit);
 virtual ~TffdshowBase();

 STDMETHODIMP_(int) getVersion2(void);
 STDMETHODIMP getParam(unsigned int paramID, int* value);
 STDMETHODIMP_(int) getParam2(unsigned int paramID);
 STDMETHODIMP putParam(unsigned int paramID, int  value);
 STDMETHODIMP invParam(unsigned int paramID);
 STDMETHODIMP getParamStr(unsigned int paramID,char_t *buf,size_t buflen);
 STDMETHODIMP_(const char_t*) getParamStr2(unsigned int paramID); //returns const pointer to string, NULL if fail
 STDMETHODIMP getParamStr3(unsigned int paramID,const char_t* *bufPtr);
 STDMETHODIMP putParamStr(unsigned int paramID,const char_t *buf);
 STDMETHODIMP getParamName(unsigned int i,char_t *buf,size_t len);
 STDMETHODIMP getParamName3(unsigned int i,const char_t* *namePtr);
 STDMETHODIMP notifyParamsChanged(void);
 STDMETHODIMP setOnChangeMsg(HWND wnd,unsigned int msg);
 STDMETHODIMP setOnFrameMsg(HWND wnd,unsigned int msg);
 STDMETHODIMP getGlobalSettings(TglobalSettingsBase* *globalSettingsPtr);
 STDMETHODIMP saveGlobalSettings(void);
 STDMETHODIMP loadGlobalSettings(void);
 STDMETHODIMP saveDialogSettings(void);
 STDMETHODIMP loadDialogSettings(void);
 STDMETHODIMP savePresetMem(void *buf,size_t len) {return E_NOTIMPL;}
 STDMETHODIMP loadPresetMem(const void *buf,size_t len) {return E_NOTIMPL;}
 STDMETHODIMP getConfig(const Tconfig* *configPtr);
 STDMETHODIMP getInstance(HINSTANCE *hi);
 STDMETHODIMP_(HINSTANCE) getInstance2(void);
 STDMETHODIMP getPostproc(Tlibmplayer* *postprocPtr);
 STDMETHODIMP getTranslator(Ttranslate* *trans);
 STDMETHODIMP initDialog(void);
 STDMETHODIMP showCfgDlg(HWND owner);
 STDMETHODIMP_(int) getCpuUsage2(void);
 STDMETHODIMP_(int) getCpuUsageForPP(void);
 STDMETHODIMP cpuSupportsMMX(void);
 STDMETHODIMP cpuSupportsMMXEXT(void);
 STDMETHODIMP cpuSupportsSSE(void);
 STDMETHODIMP cpuSupportsSSE2(void);
 STDMETHODIMP cpuSupportsSSE3(void);
 STDMETHODIMP cpuSupportsSSSE3(void);
 STDMETHODIMP cpuSupports3DNOW(void);
 STDMETHODIMP cpuSupports3DNOWEXT(void);
 STDMETHODIMP dbgInit(void);
 STDMETHODIMP dbgError(const char_t *fmt,...);
 STDMETHODIMP dbgWrite(const char_t *fmt,...);
 STDMETHODIMP dbgDone(void);
 STDMETHODIMP_(const char_t*) getExeflnm(void);
 STDMETHODIMP getLibavcodec(Tlibavcodec* *libavcodecPtr);
 STDMETHODIMP getLibavcodecSVC(Tlibavcodec* *libavcodecPtr);
 STDMETHODIMP showTrayIcon(void);
 STDMETHODIMP hideTrayIcon(void);
 STDMETHODIMP_(const char_t*) getSourceName(void);
 STDMETHODIMP getGraph(IFilterGraph* *graphPtr);
 STDMETHODIMP seek(int seconds);
 STDMETHODIMP tell(int*seconds);
 STDMETHODIMP stop(void);
 STDMETHODIMP run(void);
 STDMETHODIMP_(int) getState2(void);
 STDMETHODIMP_(int) getCurTime2(void);
 STDMETHODIMP getInCodecString(char_t *buf,size_t buflen);
 STDMETHODIMP getOutCodecString(char_t *buf,size_t buflen);
 STDMETHODIMP getMerit(DWORD *merit);
 STDMETHODIMP setMerit(DWORD  merit);
 STDMETHODIMP lock(int lockId);
 STDMETHODIMP unlock(int lockId);
 STDMETHODIMP getParamInfo(unsigned int i,TffdshowParamInfo *paramPtr);
 STDMETHODIMP exportRegSettings(int all,const char_t *regflnm,int unicode);
 STDMETHODIMP checkInputConnect(IPin *pin);
 STDMETHODIMP getParamListItem(int paramId,int index,const char_t* *ptr);
 STDMETHODIMP abortPlayback(HRESULT hr);
 STDMETHODIMP notifyParam(int id,int val);
 STDMETHODIMP notifyParamStr(int id,const char_t *val);
 STDMETHODIMP doneDialog(void);
 STDMETHODIMP resetParam(unsigned int paramID);
 STDMETHODIMP_(int) getCurrentCodecId2(void);
 STDMETHODIMP frameStep(int diff);
 STDMETHODIMP getInfoItem(unsigned int index,int *id,const char_t* *name);
 STDMETHODIMP getInfoItemValue(int id,const char_t* *value,int *wasChange,int *splitline);
 STDMETHODIMP inExplorer(void);
 STDMETHODIMP_(const char_t*) getInfoItemName(int id);
 STDMETHODIMP_(HWND) getCfgDlgHwnd(void) {return cfgDlgHwnd;}
 STDMETHODIMP_(void) setCfgDlgHwnd(HWND hwnd) {cfgDlgHwnd=hwnd;}
 STDMETHODIMP_(HWND) getTrayHwnd_(void) {return NULL;}
 STDMETHODIMP_(void) setTrayHwnd_(HWND hwnd) {}
 STDMETHODIMP_(const char_t*) getInfoItemShortcut(int id);
 STDMETHODIMP_(int) getInfoShortcutItem(const char_t *s,int *toklen);
 STDMETHODIMP_(DWORD) CPUcount(void);
 STDMETHODIMP_(int) get_trayIconType(void) PURE;


 //IffdshowParamsEnum
 STDMETHODIMP resetEnum(void);
 STDMETHODIMP nextEnum(TffdshowParamInfo *paramPtr);

protected:
 struct TffdshowBase_char : interfaces<tchar_traits<char_t>::other_char_t>::IffdshowBase, CUnknown
  {
  protected:
   typedef tchar_traits<char_t>::other_char_t tchar;
  private:
   IffdshowBaseT<char_t> *deci;
  public:
   TffdshowBase_char(LPUNKNOWN punk,IffdshowBaseT<char_t> *Ideci):deci(Ideci),CUnknown(NAME("TffdshowBase_char"),punk) {}

   STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {return deci->QueryInterface(riid,ppv);}
   STDMETHODIMP_(ULONG) AddRef() {return deci->AddRef();}
   STDMETHODIMP_(ULONG) Release() {return deci->Release();}

   STDMETHODIMP_(int) getVersion2(void) {return deci->getVersion2();}
   STDMETHODIMP getParam(unsigned int paramID, int* value) {return deci->getParam(paramID,value);}
   STDMETHODIMP_(int) getParam2(unsigned int paramID) {return deci->getParam2(paramID);}
   STDMETHODIMP putParam(unsigned int paramID, int  value) {return deci->putParam(paramID,value);}
   STDMETHODIMP invParam(unsigned int paramID) {return deci->invParam(paramID);}
   STDMETHODIMP getParamStr(unsigned int paramID,tchar *buf,size_t buflen)
    {
     char_t *buft=(char_t*)alloca(buflen*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deci->getParamStr(paramID,buft,buflen)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP_(const tchar*) getParamStr2(unsigned int paramID)
    {
     return NULL;
    }
   STDMETHODIMP getParamStr3(unsigned int paramID,const tchar* *bufPtr)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP putParamStr(unsigned int paramID,const tchar *buf)
    {
     return deci->putParamStr(paramID,text<char_t>(buf));
    }
   STDMETHODIMP getParamName(unsigned int i,tchar *buf,size_t len)
    {
     char_t *buft=(char_t*)alloca(len*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deci->getParamName(i,buft,len)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP getParamName3(unsigned int i,const tchar* *namePtr)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP notifyParamsChanged(void) {return deci->notifyParamsChanged();}
   STDMETHODIMP setOnChangeMsg(HWND wnd,unsigned int msg) {return deci->setOnChangeMsg(wnd,msg);}
   STDMETHODIMP setOnFrameMsg(HWND wnd,unsigned int msg) {return deci->setOnFrameMsg(wnd,msg);}
   STDMETHODIMP getGlobalSettings(TglobalSettingsBase* *globalSettingsPtr) {return deci->getGlobalSettings(globalSettingsPtr);}
   STDMETHODIMP saveGlobalSettings(void) {return deci->saveGlobalSettings();}
   STDMETHODIMP loadGlobalSettings(void) {return deci->loadGlobalSettings();}
   STDMETHODIMP saveDialogSettings(void) {return deci->saveDialogSettings();}
   STDMETHODIMP loadDialogSettings(void) {return deci->loadDialogSettings();}
   STDMETHODIMP savePresetMem(void *buf,size_t len) {return deci->savePresetMem(buf,len);}
   STDMETHODIMP loadPresetMem(const void *buf,size_t len) {return deci->loadPresetMem(buf,len);}
   STDMETHODIMP getConfig(const Tconfig* *configPtr) {return deci->getConfig(configPtr);}
   STDMETHODIMP getInstance(HINSTANCE *hi) {return deci->getInstance(hi);}
   STDMETHODIMP_(HINSTANCE) getInstance2(void) {return deci->getInstance2();}
   STDMETHODIMP getPostproc(Tlibmplayer* *postprocPtr) {return deci->getPostproc(postprocPtr);}
   STDMETHODIMP getTranslator(Ttranslate* *trans) {return deci->getTranslator(trans);}
   STDMETHODIMP initDialog(void) {return deci->initDialog();}
   STDMETHODIMP showCfgDlg(HWND owner) {return deci->showCfgDlg(owner);}
   STDMETHODIMP_(int) getCpuUsage2(void) {return deci->getCpuUsage2();}
   STDMETHODIMP_(int) getCpuUsageForPP(void) {return deci->getCpuUsageForPP();}
   STDMETHODIMP cpuSupportsMMX(void) {return deci->cpuSupportsMMX();}
   STDMETHODIMP cpuSupportsMMXEXT(void) {return deci->cpuSupportsMMXEXT();}
   STDMETHODIMP cpuSupportsSSE(void) {return deci->cpuSupportsSSE();}
   STDMETHODIMP cpuSupportsSSE2(void) {return deci->cpuSupportsSSE2();}
   STDMETHODIMP cpuSupportsSSE3(void) {return deci->cpuSupportsSSE3();}
   STDMETHODIMP cpuSupportsSSSE3(void) {return deci->cpuSupportsSSSE3();}
   STDMETHODIMP cpuSupports3DNOW(void) {return deci->cpuSupports3DNOW();}
   STDMETHODIMP cpuSupports3DNOWEXT(void) {return deci->cpuSupports3DNOWEXT();}
   STDMETHODIMP dbgInit(void) {return deci->dbgInit();}
   STDMETHODIMP dbgError(const tchar *fmt,...)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP dbgWrite(const tchar *fmt,...)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP dbgDone(void) {return deci->dbgDone();}
   STDMETHODIMP_(const tchar*) getExeflnm(void)
    {
     return NULL;
    }
   STDMETHODIMP getLibavcodec(Tlibavcodec* *libavcodecPtr) {return deci->getLibavcodec(libavcodecPtr);}
   STDMETHODIMP getLibavcodecSVC(Tlibavcodec* *libavcodecPtr) {return deci->getLibavcodec(libavcodecPtr);}
   STDMETHODIMP showTrayIcon(void) {return deci->showTrayIcon();}
   STDMETHODIMP hideTrayIcon(void) {return deci->hideTrayIcon();}
   STDMETHODIMP_(const tchar*) getSourceName(void)
    {
     return NULL;
    }
   STDMETHODIMP getGraph(IFilterGraph* *graphPtr) {return deci->getGraph(graphPtr);}
   STDMETHODIMP seek(int seconds) {return deci->seek(seconds);}
   STDMETHODIMP tell(int*seconds) {return deci->tell(seconds);}
   STDMETHODIMP stop(void) {return deci->stop();}
   STDMETHODIMP run(void) {return deci->run();}
   STDMETHODIMP_(int) getState2(void) {return deci->getState2();}
   STDMETHODIMP_(int) getCurTime2(void) {return deci->getCurTime2();}
   STDMETHODIMP getInCodecString(tchar *buf,size_t len)
    {
     char_t *buft=(char_t*)alloca(len*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deci->getInCodecString(buft,len)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP getOutCodecString(tchar *buf,size_t len)
    {
     char_t *buft=(char_t*)alloca(len*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deci->getOutCodecString(buft,len)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP getMerit(DWORD *merit) {return deci->getMerit(merit);}
   STDMETHODIMP setMerit(DWORD  merit) {return deci->setMerit(merit);}
   STDMETHODIMP lock(int lockId) {return deci->lock(lockId);}
   STDMETHODIMP unlock(int lockId) {return deci->unlock(lockId);}
   STDMETHODIMP getParamInfo(unsigned int i,TffdshowParamInfo *paramPtr) {return deci->getParamInfo(i,paramPtr);}
   STDMETHODIMP exportRegSettings(int all,const tchar *regflnm,int unicode)
    {
     return deci->exportRegSettings(all,text<char_t>(regflnm),unicode);
    }
   STDMETHODIMP checkInputConnect(IPin *pin) {return deci->checkInputConnect(pin);}
   STDMETHODIMP getParamListItem(int paramId,int index,const tchar* *ptr)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP abortPlayback(HRESULT hr) {return deci->abortPlayback(hr);}
   STDMETHODIMP notifyParam(int id,int val) {return deci->notifyParam(id,val);}
   STDMETHODIMP notifyParamStr(int id,const tchar *val)
    {
     return deci->notifyParamStr(id,text<char_t>(val));
    }
   STDMETHODIMP doneDialog(void) {return deci->doneDialog();}
   STDMETHODIMP resetParam(unsigned int paramID) {return deci->resetParam(paramID);}
   STDMETHODIMP_(int) getCurrentCodecId2(void) {return deci->getCurrentCodecId2();}
   STDMETHODIMP frameStep(int diff) {return deci->frameStep(diff);}
   STDMETHODIMP getInfoItem(unsigned int index,int *id,const tchar* *name)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP getInfoItemValue(int id,const tchar* *value,int *wasChange,int *splitline)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP inExplorer(void) {return deci->inExplorer();}
   STDMETHODIMP_(const tchar*) getInfoItemName(int id)
    {
     return NULL;
    }
   STDMETHODIMP_(HWND) getCfgDlgHwnd(void) {return deci->getCfgDlgHwnd();}
   STDMETHODIMP_(void) setCfgDlgHwnd(HWND hwnd) {deci->setCfgDlgHwnd(hwnd);}
   STDMETHODIMP_(HWND) getTrayHwnd_(void) {return NULL;}
   STDMETHODIMP_(void) setTrayHwnd_(HWND hwnd) {}
   STDMETHODIMP_(const tchar*) getInfoItemShortcut(int id) {return NULL;}
   STDMETHODIMP_(int) getInfoShortcutItem(const tchar *s,int *toklen)
    {
     return deci->getInfoShortcutItem(text<char_t>(s),toklen);
    }
   STDMETHODIMP_(DWORD) CPUcount(void) {return deci->CPUcount();}
   STDMETHODIMP_(int) get_trayIconType(void) {return deci->get_trayIconType();}
  } base_char;
 template<class Tinterface> Tinterface* getBaseInterface(void);
private:
 DWORD m_CPUCount;

 static const int VERSION=23;

 double tell(void);
 static HRESULT changeFilterMerit(const CLSID &clsid,DWORD merit);
 int defaultMerit;
 CmyTransformFilter *mfilter;
 CTransformOutputPin* &moutput;
 int cfgDlgAlwaysOnTop;
 int cfgDlgCaptionId,iconId;
 TglobalSettingsBase *globalSettings;
 TdialogSettingsBase *dialogSettings;
 HWND onFrameWnd;unsigned int onFrameMsg;
 ints enumInfos;ints::const_iterator curEnumInfo;
 TinfoBase *info;
 void initInfo(void);

 CCritSec* locks[LOCKS_COUNT];
 int cpuUsage,cpuUsageCounter;
protected:
 TinputPin* &minput;
 TintStrColl *options;
 typedef std::vector<TintStrColl*> TintStrColls;
 virtual void getColls(TintStrColls &colls) {colls.push_back(options);}
 volatile bool hereSeek;
 Tconfig config;
 bool firstdialog;
 HWND cfgDlgHwnd;
 virtual HRESULT onInitDialog(void) {return S_OK;}
 virtual HRESULT onDoneDialog(void) {return S_OK;}
 Ttranslate *trans;
 void onLangChange(int id,const char_t *newval);
 Tlibmplayer *libmplayer;
 Tlibavcodec *libavcodec;
 FILE *dbgfile;
 int notreg;

 IFilterGraph* &graph;
 DWORD pdwROT;
 HRESULT onJoinFilterGraph(IFilterGraph *pGraph,LPCWSTR pName);
 virtual HRESULT onGraphJoin(IFilterGraph *pGraph);
 virtual HRESULT onGraphRemove(void);
 virtual TinfoBase* createInfo(void) =0;
 static void setPropsTime(IMediaSample *sample,REFERENCE_TIME t1,REFERENCE_TIME t2,AM_SAMPLE2_PROPERTIES* const pProps,BOOL *m_bSampleSkipped);
 HRESULT getDeliveryBuffer(IMediaSample **pSample,BYTE **pData);

 HWND onChangeWnd;unsigned int onChangeMsg;
 void sendOnFrameMsg(void);

 TcpuUsage *cpu;;int cpus;

 int moviesecs;
 int getDuration(void);

 int applying;
 virtual void sendOnChange(int paramID,int val);

 HWND trayHwnd;HANDLE hTrayThread;
 unsigned (__stdcall *trayIconStart)(TtrayIconStartData*);
 virtual void onTrayIconChange(int id,int newval);

 void onMeritChange(int id,int newval);
};

#endif
