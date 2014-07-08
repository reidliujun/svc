#ifndef _TFFDSHOWDEC_H_
#define _TFFDSHOWDEC_H_

#include "IffdshowDec.h"
#include "TffdshowBase.h"

class Tkeyboard;
class Tmouse;
struct TglobalSettingsDec;
struct TdialogSettingsDec;
class Tremote;
class Tfilters;
class TffdshowDec :public TffdshowBase,
                   public interfaces<char_t>::IffdshowDec,
                   public CmyTransformFilter,
                   public ISpecifyPropertyPages,
                   public IAMExtendedSeeking,
                   public IAMStreamSelect
{
public:
 TffdshowDec(TintStrColl *Ioptions,const TCHAR *name,LPUNKNOWN punk,REFCLSID clsid,TglobalSettingsDec *IglobalSettings,TdialogSettingsDec *IdialogSettings,Tpresets *Ipresets,Tpreset* &IpresetSettings,CmyTransformFilter *Imfilter,TinputPin* &Iminput,CTransformOutputPin* &Imoutput,IFilterGraph* &Igraph,Tfilters* &Ifilters,const CLSID &Iproppageid,int IcfgDlgCaptionId,int IiconId,DWORD IdefaultMerit);
 virtual ~TffdshowDec();

 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
 HRESULT BreakConnect(PIN_DIRECTION dir);

 STDMETHODIMP JoinFilterGraph(IFilterGraph *pGraph,LPCWSTR pName);
 HRESULT StopStreaming(void);

 STDMETHODIMP_(int) getVersion2(void);
 STDMETHODIMP saveKeysSettings(void);
 STDMETHODIMP loadKeysSettings(void);
 STDMETHODIMP getGraph(IFilterGraph* *graphPtr);
 STDMETHODIMP seek(int seconds);
 STDMETHODIMP tell(int*seconds);
 STDMETHODIMP stop(void);
 STDMETHODIMP run(void);
 STDMETHODIMP_(int) getState2(void);
 STDMETHODIMP getDuration(int*seconds);
 STDMETHODIMP_(int) getCurTime2(void);
 STDMETHODIMP initKeys(void);
 STDMETHODIMP_(int) getKeyParamCount2(void);
 STDMETHODIMP getKeyParamDescr(unsigned int i,const char_t **descr);
 STDMETHODIMP_(int) getKeyParamKey2(unsigned int i);
 STDMETHODIMP setKeyParamKey(unsigned int i,int key);
 STDMETHODIMP initPresets(void);
 STDMETHODIMP getNumPresets(unsigned int *value);
 STDMETHODIMP getPresetName(unsigned int i,char_t *buf,size_t len);
 STDMETHODIMP getActivePresetName(char_t *buf,size_t len);
 STDMETHODIMP setActivePreset(const char_t *name,int create);
 STDMETHODIMP saveActivePreset(const char_t *name);
 STDMETHODIMP saveActivePresetToFile(const char_t *flnm);
 STDMETHODIMP loadActivePresetFromFile(const char_t *flnm);
 STDMETHODIMP removePreset(const char_t *name);
 STDMETHODIMP getPresets(Tpresets *presets2);
 STDMETHODIMP setPresets(const Tpresets *presets2);
 STDMETHODIMP savePresets(void);
 STDMETHODIMP getPresetPtr(Tpreset**preset);
 STDMETHODIMP setPresetPtr(Tpreset *preset);
 STDMETHODIMP renameActivePreset(const char_t *newName);
 STDMETHODIMP isDefaultPreset(const char_t *presetName);
 STDMETHODIMP createTempPreset(const char_t *presetName);
 STDMETHODIMP_(int) getMinOrder2(void);
 STDMETHODIMP_(int) getMaxOrder2(void);
 STDMETHODIMP resetFilter(unsigned int filterID);
 STDMETHODIMP resetFilterEx(unsigned int filterID,unsigned int filterPageId);
 STDMETHODIMP getFilterTip(unsigned int filterID,char_t *buf,size_t buflen);
 STDMETHODIMP getFilterTipEx(unsigned int filterID,unsigned int filterPageId,char_t *buf,size_t buflen);
 STDMETHODIMP filterHasReset(unsigned int filterID);
 STDMETHODIMP filterHasResetEx(unsigned int filterID,unsigned int filterPageId);
 STDMETHODIMP getPresetsPtr(Tpresets* *presetsPtr);
 STDMETHODIMP newSample(IMediaSample* *samplePtr);
 STDMETHODIMP deliverSample_unused(IMediaSample *sample) {return E_NOTIMPL;}
 STDMETHODIMP resetOrder(void);
 STDMETHODIMP resetKeys(void);
 STDMETHODIMP putStringParams(const char_t *params,char_t sep,int loaddef);
 STDMETHODIMP_(const TfilterIDFF*) getNextFilterIDFF(void) {return NULL;}
 STDMETHODIMP cyclePresets(int step);
 STDMETHODIMP exportKeysToGML(const char_t *flnm);
 STDMETHODIMP getShortDescription(char_t *buf,int buflen);
 STDMETHODIMP_(const char_t*) getActivePresetName2(void);
 STDMETHODIMP createPresetPages(const char_t *presetname,TffdshowPageDec *pages);
 STDMETHODIMP_(const TfilterIDFF*) getFilterIDFF_notimpl(void) {return NULL;}
 STDMETHODIMP getFilterIDFFs(const char_t *presetname,const TfilterIDFFs* *filters);
 STDMETHODIMP notifyParamsChanged(void);
 STDMETHODIMP getParam(unsigned int paramID,int* value);
 STDMETHODIMP putParam(unsigned int paramID,int val);
 STDMETHODIMP invParam(unsigned int paramID);
 STDMETHODIMP getParamStr3(unsigned int paramID,const char_t* *value);
 STDMETHODIMP putParamStr(unsigned int paramID,const char_t *buf);
 STDMETHODIMP getParamInfo(unsigned int paramID,TffdshowParamInfo *paramPtr);
 STDMETHODIMP notifyParam(int id,int val);
 STDMETHODIMP notifyParamStr(int id,const char_t *val);
 STDMETHODIMP initRemote(void);
 STDMETHODIMP saveRemoteSettings(void);
 STDMETHODIMP loadRemoteSettings(void);
 STDMETHODIMP setFilterOrder(unsigned int filterID,unsigned int newOrder);
 STDMETHODIMP savePresetMem(void *buf,size_t len);
 STDMETHODIMP loadPresetMem(const void *buf,size_t len);
 STDMETHODIMP resetParam(unsigned int paramID);
 STDMETHODIMP initMouse(void);
 STDMETHODIMP_(unsigned int) getPresetAutoloadItemsCount2(void);
 STDMETHODIMP getPresetAutoloadItemInfo(unsigned int index,const char_t* *name,const char_t* *hint,int *allowWildcard,int *is,int *isVal,char_t *val,size_t vallen,int *isList,int *isHelp);
 STDMETHODIMP setPresetAutoloadItem(unsigned int index,int is,const char_t *val);
 STDMETHODIMP_(const char_t*) getPresetAutoloadItemList(unsigned int paramIndex,unsigned int listIndex);
 STDMETHODIMP_(const char_t**) getSupportedFOURCCs(void);
 STDMETHODIMP_(const Tstrptrs*) getCodecsList(void);
 STDMETHODIMP queryFilterInterface(const IID &iid,void **ptr);
 STDMETHODIMP setOnNewFiltersMsg(HWND wnd,unsigned int msg);
 STDMETHODIMP sendOnNewFiltersMsg(void);
 STDMETHODIMP setKeyParamKeyCheck(unsigned int i,int key,int *prev,const char_t* *prevDescr);
 STDMETHODIMP getPresetAutoloadItemHelp(unsigned int index,const char_t* *helpPtr);
 STDMETHODIMP_(int) getOSDpresetCount2(void);
 STDMETHODIMP_(const char_t*) getOSDpresetName2(unsigned int i);
 STDMETHODIMP_(const char_t*) getOSDpresetFormat2(const char_t *presetName);
 STDMETHODIMP setOSDpresetFormat(const char_t *presetName,const char_t *format);
 STDMETHODIMP setOSDpresetName(unsigned int i,const char_t *name);
 STDMETHODIMP addOSDpreset(const char_t *presetName,const char_t *format);
 STDMETHODIMP deleteOSDpreset(const char_t *presetName);
 STDMETHODIMP cycleOSDpresets(void);
 STDMETHODIMP_(const char_t*) getOSDstartupFormat(int *duration);
 STDMETHODIMP_(TinputPin*)getInputPin(void);

 // IDispatch
 STDMETHODIMP GetTypeInfoCount(UINT FAR* pctinfo) {return E_NOTIMPL;}
 STDMETHODIMP GetTypeInfo(UINT itinfo,  LCID lcid, ITypeInfo FAR* FAR* pptinfo) {return E_NOTIMPL;}
 STDMETHODIMP GetIDsOfNames(REFIID riid,OLECHAR FAR* FAR* rgszNames,UINT cNames,LCID lcid,DISPID FAR* rgdispid) {return E_NOTIMPL;}
 STDMETHODIMP Invoke(DISPID dispidMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS FAR* pdispparams,VARIANT FAR* pvarResult,EXCEPINFO FAR* pexcepinfo,UINT FAR* puArgErr) {return E_NOTIMPL;}

 // IAMExtendedSeeking
 STDMETHODIMP get_ExSeekCapabilities(long * pExCapabilities);
 STDMETHODIMP get_MarkerCount(long * pMarkerCount);
 STDMETHODIMP get_CurrentMarker(long * pCurrentMarker);
 STDMETHODIMP GetMarkerTime(long MarkerNum, double * pMarkerTime);
 STDMETHODIMP GetMarkerName(long MarkerNum, BSTR * pbstrMarkerName);
 STDMETHODIMP put_PlaybackSpeed(double Speed);
 STDMETHODIMP get_PlaybackSpeed(double * pSpeed);

 // ISpecifyPropertyPages
 STDMETHODIMP GetPages(CAUUID *pPages);

 // CTransformFilter
 HRESULT NewSegment(REFERENCE_TIME tStart,REFERENCE_TIME tStop,double dRate);
protected:
 struct TffdshowDec_char : interfaces<tchar_traits<char_t>::other_char_t>::IffdshowDec, CUnknown
  {
  protected:
   typedef tchar_traits<char_t>::other_char_t tchar;
  private:
   IffdshowDecT<char_t> *deciD;
  public:
   TffdshowDec_char(LPUNKNOWN punk,IffdshowDecT<char_t> *IdeciD):deciD(IdeciD),CUnknown(NAME("TffdshowDec_char"),punk) {}

   STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {return deciD->QueryInterface(riid,ppv);}
   STDMETHODIMP_(ULONG) AddRef() {return deciD->AddRef();}
   STDMETHODIMP_(ULONG) Release() {return deciD->Release();}

   STDMETHODIMP_(int) getVersion2(void) {return deciD->getVersion2();}
   STDMETHODIMP saveKeysSettings(void) {return deciD->saveKeysSettings();}
   STDMETHODIMP loadKeysSettings(void) {return deciD->loadKeysSettings();}
   STDMETHODIMP getGraph(IFilterGraph* *graphPtr) {return deciD->getGraph(graphPtr);}
   STDMETHODIMP seek(int seconds) {return deciD->seek(seconds);}
   STDMETHODIMP tell(int*seconds) {return deciD->tell(seconds);}
   STDMETHODIMP stop(void) {return deciD->stop();}
   STDMETHODIMP run(void) {return deciD->run();}
   STDMETHODIMP_(int) getState2(void) {return deciD->getState2();}
   STDMETHODIMP getDuration(int*seconds) {return deciD->getDuration(seconds);}
   STDMETHODIMP_(int) getCurTime2(void) {return deciD->getCurTime2();}
   STDMETHODIMP initKeys(void) {return deciD->initKeys();}
   STDMETHODIMP_(int) getKeyParamCount2(void) {return deciD->getKeyParamCount2();}
   STDMETHODIMP getKeyParamDescr(unsigned int i,const tchar* *descr)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP_(int) getKeyParamKey2(unsigned int i) {return deciD->getKeyParamKey2(i);}
   STDMETHODIMP setKeyParamKey(unsigned int i,int key) {return deciD->setKeyParamKey(i,key);}
   STDMETHODIMP initPresets(void) {return deciD->initPresets();}
   STDMETHODIMP getNumPresets(unsigned int *value) {return deciD->getNumPresets(value);}
   STDMETHODIMP getPresetName(unsigned int i,tchar *buf,size_t buflen)
    {
     char_t *buft=(char_t*)alloca(buflen*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deciD->getPresetName(i,buft,buflen)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP getActivePresetName(tchar *buf,size_t buflen)
    {
     char_t *buft=(char_t*)alloca(buflen*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deciD->getActivePresetName(buft,buflen)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP setActivePreset(const tchar *name,int create)
    {
     return deciD->setActivePreset(text<char_t>(name),create);
    }
   STDMETHODIMP saveActivePreset(const tchar *name)
    {
     return deciD->saveActivePreset(text<char_t>(name));
    }
   STDMETHODIMP saveActivePresetToFile(const tchar *flnm)
    {
     return deciD->saveActivePresetToFile(text<char_t>(flnm));
    }
   STDMETHODIMP loadActivePresetFromFile(const tchar *flnm)
    {
     return deciD->loadActivePresetFromFile(text<char_t>(flnm));
    }
   STDMETHODIMP removePreset(const tchar *name)
    {
     return deciD->removePreset(text<char_t>(name));
    }
   STDMETHODIMP getPresets(Tpresets *presets2)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP setPresets(const Tpresets *presets2)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP savePresets(void) {return deciD->savePresets();}
   STDMETHODIMP getPresetPtr(Tpreset**preset)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP setPresetPtr(Tpreset *preset)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP renameActivePreset(const tchar *newName)
    {
     return deciD->renameActivePreset(text<char_t>(newName));
    }
   STDMETHODIMP isDefaultPreset(const tchar *presetName)
    {
     return deciD->isDefaultPreset(text<char_t>(presetName));
    }
   STDMETHODIMP createTempPreset(const tchar *presetName)
    {
     return deciD->createTempPreset(text<char_t>(presetName));
    }
   STDMETHODIMP_(int) getMinOrder2(void) {return deciD->getMinOrder2();}
   STDMETHODIMP_(int) getMaxOrder2(void) {return deciD->getMaxOrder2();}
   STDMETHODIMP resetFilter(unsigned int filterID) {return deciD->resetFilter(filterID);}
   STDMETHODIMP resetFilterEx(unsigned int filterID,unsigned int filterPageId) {return deciD->resetFilterEx(filterID,filterPageId);}
   STDMETHODIMP getFilterTip(unsigned int filterID,tchar *buf,size_t buflen)
    {
     char_t *buft=(char_t*)alloca(buflen*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deciD->getFilterTip(filterID,buft,buflen)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP getFilterTipEx(unsigned int filterID,unsigned int filterPageId,tchar *buf,size_t buflen)
    {
     char_t *buft=(char_t*)alloca(buflen*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deciD->getFilterTipEx(filterID,filterPageId,buft,buflen)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP filterHasReset(unsigned int filterID) {return deciD->filterHasReset(filterID);}
   STDMETHODIMP filterHasResetEx(unsigned int filterID,unsigned int filterPageId) {return deciD->filterHasResetEx(filterID,filterPageId);}
   STDMETHODIMP getPresetsPtr(Tpresets* *presetsPtr)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP newSample(IMediaSample* *samplePtr) {return deciD->newSample(samplePtr);}
   STDMETHODIMP deliverSample_unused(IMediaSample *sample) {return deciD->deliverSample_unused(sample);}
   STDMETHODIMP resetOrder(void) {return deciD->resetOrder();}
   STDMETHODIMP resetKeys(void) {return deciD->resetKeys();}
   STDMETHODIMP putStringParams(const tchar *params,tchar sep,int loaddef)
    {
     return deciD->putStringParams(text<char_t>(params),(char_t)sep,loaddef);
    }
   STDMETHODIMP_(const TfilterIDFF*) getNextFilterIDFF(void) {return deciD->getNextFilterIDFF();}
   STDMETHODIMP cyclePresets(int step) {return deciD->cyclePresets(step);}
   STDMETHODIMP exportKeysToGML(const tchar *flnm)
    {
     return deciD->exportKeysToGML(text<char_t>(flnm));
    }
   STDMETHODIMP getShortDescription(tchar *buf,int buflen)
    {
     char_t *buft=(char_t*)alloca(buflen*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deciD->getShortDescription(buft,buflen)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP_(const tchar*) getActivePresetName2(void)
    {
     return NULL;
    }
   STDMETHODIMP createPresetPages(const tchar *presetname,TffdshowPageDec *pages)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP getEncoderInfo(tchar *buf,size_t buflen)
    {
     char_t *buft=(char_t*)alloca(buflen*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deciD->getEncoderInfo(buft,buflen)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP_(const tchar*) getDecoderName(void)
    {
     return NULL;
    }
   STDMETHODIMP_(const TfilterIDFF*) getFilterIDFF_notimpl(void) {return deciD->getFilterIDFF_notimpl();}
   STDMETHODIMP getFilterIDFFs(const tchar *presetname,const TfilterIDFFs* *filters)
    {
     return E_NOTIMPL;
    }
   /*
   STDMETHODIMP getParam(unsigned int paramID,int* value) {return deciD->getParam(paramID,value);}
   STDMETHODIMP putParam(unsigned int paramID,int val) {return deciD->putParam(paramID,val);}
   STDMETHODIMP invParam(unsigned int paramID) {return deciD->invParam(paramID);}
   STDMETHODIMP getParamStr3(unsigned int paramID,const tchar* *value)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP putParamStr(unsigned int paramID,const tchar *buf)
    {
     return deciD->putParamStr(paramID,text<char_t>(buf));
    }
   STDMETHODIMP getParamInfo(unsigned int paramID,TffdshowParamInfo *paramPtr)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP notifyParam(int id,int val) {return deciD->notifyParam(id,val);}
   STDMETHODIMP notifyParamStr(int id,const tchar *val)
    {
     return deciD->notifyParamStr(id,text<char_t>(val));
    }
   */
   STDMETHODIMP initRemote(void) {return deciD->initRemote();}
   STDMETHODIMP saveRemoteSettings(void) {return deciD->saveRemoteSettings();}
   STDMETHODIMP loadRemoteSettings(void) {return deciD->loadRemoteSettings();}
   STDMETHODIMP setFilterOrder(unsigned int filterID,unsigned int newOrder) {return deciD->setFilterOrder(filterID,newOrder);}
   //STDMETHODIMP savePresetMem(void *buf,unsigned int len) {return deciD->savePresetMem(buf,len);}
   //STDMETHODIMP loadPresetMem(const void *buf,unsigned int len) {return deciD->loadPresetMem(buf,len);}
   //STDMETHODIMP resetParam(unsigned int paramID) {return deciD->resetParam(paramID);}
   //STDMETHODIMP initMouse(void) {return deciD->initMouse();}
   STDMETHODIMP_(unsigned int) getPresetAutoloadItemsCount2(void) {return deciD->getPresetAutoloadItemsCount2();}
   STDMETHODIMP getPresetAutoloadItemInfo(unsigned int index,const tchar* *name,const tchar* *hint,int *allowWildcard,int *is,int *isVal,tchar *val,size_t vallen,int *isList,int *isHelp)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP setPresetAutoloadItem(unsigned int index,int is,const tchar *val)
    {
     return deciD->setPresetAutoloadItem(index,is,text<char_t>(val));
    }
   STDMETHODIMP_(const tchar*) getPresetAutoloadItemList(unsigned int paramIndex,unsigned int listIndex)
    {
     return NULL;
    }
   STDMETHODIMP_(const tchar**) getSupportedFOURCCs(void)
    {
     return NULL;
    }
   STDMETHODIMP_(const Tstrptrs*) getCodecsList(void)
    {
     return NULL;
    }
   STDMETHODIMP queryFilterInterface(const IID &iid,void **ptr) {return deciD->queryFilterInterface(iid,ptr);}
   STDMETHODIMP setOnNewFiltersMsg(HWND wnd,unsigned int msg) {return deciD->setOnNewFiltersMsg(wnd,msg);}
   STDMETHODIMP sendOnNewFiltersMsg(void) {return deciD->sendOnNewFiltersMsg();}
   STDMETHODIMP setKeyParamKeyCheck(unsigned int i,int key,int *prev,const tchar* *prevDescr)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP_(int) getInputBitrate2(void)
    {
     return deciD->getInputBitrate2();
    }
   STDMETHODIMP getPresetAutoloadItemHelp(unsigned int index,const tchar* *helpPtr)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP_(int) getOSDpresetCount2(void) {return deciD->getOSDpresetCount2();}
   STDMETHODIMP_(const tchar*) getOSDpresetName2(unsigned int i)
    {
     return NULL;
    }
   STDMETHODIMP_(const tchar*) getOSDpresetFormat2(const tchar *presetName)
    {
     return NULL;
    }
   STDMETHODIMP setOSDpresetFormat(const tchar *presetName,const tchar *format) {return deciD->setOSDpresetFormat(text<char_t>(presetName),text<char_t>(format));}
   STDMETHODIMP setOSDpresetName(unsigned int i,const tchar *name) {return deciD->setOSDpresetName(i,text<char_t>(name));}
   STDMETHODIMP addOSDpreset(const tchar *presetName,const tchar *format) {return deciD->addOSDpreset(text<char_t>(presetName),text<char_t>(format));}
   STDMETHODIMP deleteOSDpreset(const tchar *presetName) {return deciD->deleteOSDpreset(text<char_t>(presetName));}
   STDMETHODIMP cycleOSDpresets(void) {return deciD->cycleOSDpresets();}
   STDMETHODIMP_(const tchar*) getOSDstartupFormat(int *duration)
    {
     return NULL;
    }

   STDMETHODIMP_(TinputPin*) getInputPin(void){ return(deciD->getInputPin());};
  } dec_char;
 template<class Tinterface> Tinterface* getDecInterface(void);
private:
 static const int VERSION=26;
 Tfilters* &filters;
 Tstrptrs codecs;
 HWND onNewFiltersWnd;unsigned int onNewFiltersMsg;
protected:
 Tpresets *presets;
 Tpreset* &presetSettings;
 virtual void getColls(TintStrColls &colls);
 void initPreset(void);
 TglobalSettingsDec *globalSettings;
 TdialogSettingsDec *dialogSettings;

 virtual HRESULT onGraphRemove(void);

 const CLSID &proppageid;
 virtual HRESULT onInitDialog(void);

 Tkeyboard *keys;
 Tmouse *mouse;
 Tremote *remote;

 virtual void onTrayIconChange(int id,int newval);
 virtual void sendOnChange(int paramID,int val) {TffdshowBase::sendOnChange(paramID,val);}

 bool firsttransform,discontinuity;

 // IAMStreamSelect
 STDMETHODIMP Count(DWORD* pcStreams);
 STDMETHODIMP Info(long lIndex, AM_MEDIA_TYPE** ppmt, DWORD* pdwFlags, LCID* plcid, DWORD* pdwGroup, WCHAR** ppszName, IUnknown** ppObject, IUnknown** ppUnk);
 STDMETHODIMP Enable(long lIndex, DWORD dwFlags);
 virtual bool isStreamsMenu(void) const =0;
 struct Tstream
  {
   Tstream(TffdshowDec *Iself,int Iorder,int Igroup):self(Iself),order(Iorder),group(Igroup) {}
   virtual ~Tstream() {}
   TffdshowDec *self;
   int group;
   int order;
   virtual DWORD getFlags(void)=0;
   virtual const char_t* getName(void)=0;
   virtual bool action(void)=0;
  };
 struct TstreamFilter :Tstream
  {
  private:
   Ttranslate *tr;
  public:
   TstreamFilter(TffdshowDec *Iself,int Iorder,int Igroup,const TfilterIDFF *If,Ttranslate *Itr);
   virtual ~TstreamFilter();
   const TfilterIDFF *f;
   virtual DWORD getFlags(void);
   virtual const char_t* getName(void);
   virtual bool action(void);
  };
 struct TstreamPreset :Tstream
  {
   TstreamPreset(TffdshowDec *Iself,int Iorder,int Igroup,const char_t *Ipreset):Tstream(Iself,Iorder,Igroup),preset(Ipreset) {}
   const char_t *preset;
   virtual DWORD getFlags(void);
   virtual const char_t* getName(void);
   virtual bool action(void);
  };

 struct Tstreams :array_vector<Tstream*,255>
  {
   ~Tstreams()
    {
     for (iterator s=begin();s!=end();s++)
      delete *s;
    }
  };
 Tstreams streams;
 static bool streamsSort(const Tstream *s1,const Tstream *s2);
 virtual AM_MEDIA_TYPE* getInputMediaType(int lIndex);
 virtual void addOwnStreams(void) {}
public:
 virtual STDMETHODIMP Stop(void)
  {
   m_dirtyStop=true;
   return CmyTransformFilter::Stop();
  }
protected:
 bool m_dirtyStop; // Work around DVBViewer compatibility issue. Old DVBViewer doesn't call NewSegment after Stop and before next play.
};

#endif
