#ifndef _TFFDECODERVIDEO_H_
#define _TFFDECODERVIDEO_H_

#include "IffDecoder.h"
#include "IffdshowDecVideo.h"
#include "TffdshowDec.h"
#include "TffdshowVideo.h"
#include "TimgFilters.h"
#include "TvideoCodec.h"
#include "TinputPin.h"
#include "Tinfo.h"

struct TpresetVideo;
struct ToutputVideoSettings;
struct TffPict;
struct IMixerPinConfig2;
struct TglobalSettingsDecVideo;
struct TdialogSettingsDecVideo;
class TtextInputPin;
class TpresetsVideo;
class TffdshowVideoInputPin;
class TffdshowDecVideoOutputPin;
class TsearchInterfaceInGraph;

class TffdshowDecVideo :public TffdshowDec,
                        public TffdshowVideo,
                        public interfaces<char_t>::IffdshowDecVideo,
                        public IffDecoder,
                        public IdecVideoSink,
                        public IprocVideoSink
{
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr);
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
 DECLARE_IUNKNOWN

 TffdshowDecVideo(CLSID Iclsid,const char_t *className,const CLSID &Iproppageid,int IcfgDlgCaptionId,int IiconId,LPUNKNOWN punk,HRESULT *phr,int Imode,int IdefaultMerit,TintStrColl *Ioptions);
 virtual ~TffdshowDecVideo();

 virtual HRESULT onGraphRemove(void);
 virtual CBasePin* GetPin(int n);
 virtual int GetPinCount(void);
 STDMETHODIMP FindPin(LPCWSTR Id, IPin **ppPin);

 HRESULT CheckConnect(PIN_DIRECTION dir,IPin *pPin);
 HRESULT CheckInputType(const CMediaType *mtIn);
 HRESULT CompleteConnect(PIN_DIRECTION direction,IPin *pReceivePin);
 HRESULT GetMediaType(int iPos,CMediaType *pmt);
 HRESULT SetMediaType(PIN_DIRECTION direction,const CMediaType *pmt);
 HRESULT CheckTransform(const CMediaType *mtIn,const CMediaType *mtOut);
 HRESULT DecideBufferSize(IMemAllocator *pima,ALLOCATOR_PROPERTIES *pProperties);

  // IAMExtendedSeeking
 STDMETHODIMP get_MarkerCount(long * pMarkerCount);
 STDMETHODIMP get_CurrentMarker(long * pCurrentMarker);
 STDMETHODIMP GetMarkerTime(long MarkerNum, double * pMarkerTime);
 STDMETHODIMP GetMarkerName(long MarkerNum, BSTR * pbstrMarkerName);
private:
 HRESULT DecideBufferSizeVMR(IMemAllocator *pima,ALLOCATOR_PROPERTIES *pProperties, const CLSID &ref);
 HRESULT DecideBufferSizeOld(IMemAllocator *pima,ALLOCATOR_PROPERTIES *pProperties, const CLSID &ref);
 virtual void getChapters(void);

public:
 virtual HRESULT NewSegment(REFERENCE_TIME tStart,REFERENCE_TIME tStop,double dRate);
 virtual STDMETHODIMP Run(REFERENCE_TIME tStart);
 virtual STDMETHODIMP Stop(void);
 virtual HRESULT EndOfStream(void);
 HRESULT AlterQuality(Quality q);

 STDMETHODIMP_(int) getVersion2(void);
 STDMETHODIMP getAVIdimensions(unsigned int *x,unsigned int *y);
 STDMETHODIMP getAVIfps(unsigned int *fps1000);
 STDMETHODIMP loadActivePresetFromFile(const char_t *flnm);
 STDMETHODIMP getAVcodecVersion(char_t *buf,size_t len) {return E_NOTIMPL;}
 STDMETHODIMP getPPmode(unsigned int *ppmode);
 STDMETHODIMP getRealCrop(unsigned int *left,unsigned int *top,unsigned int *right,unsigned int *bottom) {return E_NOTIMPL;}
 STDMETHODIMP setPresetPtr(Tpreset *preset);
 STDMETHODIMP getXvidVersion(char_t *buf,size_t len) {return E_NOTIMPL;}
 STDMETHODIMP getMovieSource(const TvideoCodecDec* *moviePtr);
 STDMETHODIMP getOutputDimensions(unsigned int *x,unsigned int *y);
 STDMETHODIMP getOutputFourcc(char_t *buf,size_t len);
 STDMETHODIMP_(int) getInputBitrate2(void);
 STDMETHODIMP getHistogram_(unsigned int dst[256]) {return E_NOTIMPL;}
 STDMETHODIMP setFilterOrder(unsigned int filterID,unsigned int newOrder) {return TffdshowDec::setFilterOrder(filterID,newOrder);}
 STDMETHODIMP buildHistogram_(const TffPict *pict,int full) {return E_NOTIMPL;}
 STDMETHODIMP_(int) getAVIfps1000_2(void);
 STDMETHODIMP getCurrentFrameTime(unsigned int *sec) {return getFrameTime(currentFrame,sec);}
 STDMETHODIMP getImgFilters_(void* *imgFiltersPtr) {*imgFiltersPtr=(void*)imgFilters; return S_OK;}
 STDMETHODIMP getQuant(int* *quantPtr) {return E_NOTIMPL;}
 STDMETHODIMP calcNewSize(unsigned int inDx,unsigned int inDy,unsigned int *outDx,unsigned int *outDy);
 STDMETHODIMP calcNewSize(Trect inRect,unsigned int *outDx,unsigned int *outDy);
 STDMETHODIMP grabNow(void);
 STDMETHODIMP getOverlayControlCapability(int idff) {return E_NOTIMPL;}
 STDMETHODIMP drawOSD(int px,int py,const char_t *text);
 STDMETHODIMP lock(int lockId) {return TffdshowBase::lock(lockId);}
 STDMETHODIMP unlock(int lockId) {return TffdshowBase::unlock(lockId);}
 STDMETHODIMP calcMeanQuant(float *quant);
 STDMETHODIMP_(int) findAutoSubflnm2(void) {return E_NOTIMPL;}
 STDMETHODIMP getFrameTime(unsigned int framenum,unsigned int *sec);
 STDMETHODIMP shortOSDmessage(const char_t *msg,unsigned int duration);
 STDMETHODIMP setImgFilters_(void *imgFiltersPtr) {return E_NOTIMPL;}
 STDMETHODIMP registerSelectedMediaTypes(void) {return E_NOTIMPL;}
 STDMETHODIMP getFrameTimes(int64_t *start,int64_t *stop) {return E_NOTIMPL;}
 STDMETHODIMP getSubtitleTimes(int64_t *start,int64_t *stop) {return E_NOTIMPL;}
 STDMETHODIMP resetSubtitleTimes(void);
 STDMETHODIMP setFrameTimes(int64_t start,int64_t stop) {return E_NOTIMPL;}
 STDMETHODIMP_(int) getOSDpresetCount2(void) {return TffdshowDec::getOSDpresetCount2();}
 STDMETHODIMP_(const char_t*) getOSDpresetName2(unsigned int i) {return TffdshowDec::getOSDpresetName2(i);}
 STDMETHODIMP_(const char_t*) getOSDpresetFormat2(const char_t *presetName) {return TffdshowDec::getOSDpresetFormat2(presetName);}
 STDMETHODIMP setOSDpresetFormat(const char_t *presetName,const char_t *format) {return TffdshowDec::setOSDpresetFormat(presetName,format);}
 STDMETHODIMP setOSDpresetName(unsigned int i,const char_t *name) {return TffdshowDec::setOSDpresetName(i,name);}
 STDMETHODIMP addOSDpreset(const char_t *presetName,const char_t *format) {return TffdshowDec::addOSDpreset(presetName,format);}
 STDMETHODIMP deleteOSDpreset(const char_t *presetName) {return TffdshowDec::deleteOSDpreset(presetName);}
 STDMETHODIMP cycleOSDpresets(void) {return TffdshowDec::cycleOSDpresets();}
 STDMETHODIMP_(int) getCodecId(const BITMAPINFOHEADER *hdr,const GUID *subtype,FOURCC *AVIfourcc);
 STDMETHODIMP getFontManager(TfontManager* *fontManagerPtr);
 STDMETHODIMP getInCodecString(char_t *buf,size_t buflen);
 STDMETHODIMP getOutCodecString(char_t *buf,size_t buflen);
 STDMETHODIMP_(int) getInIsSync(void) {return E_NOTIMPL;}
 STDMETHODIMP getVideoWindowPos(int *left,int *top,unsigned int *width,unsigned int *height);
 STDMETHODIMP_(unsigned int) getSubtitleLanguagesCount2(void);
 STDMETHODIMP getSubtitleLanguageDesc(unsigned int num,const char_t* *descPtr);
 STDMETHODIMP fillSubtitleLanguages(const char_t **langs);
 STDMETHODIMP resetOSD(void);
 STDMETHODIMP getFrameTimeMS(unsigned int framenum,unsigned int *msec);
 STDMETHODIMP getCurrentFrameTimeMS(unsigned int *msec) {return getFrameTimeMS(currentFrame,msec);}
 STDMETHODIMP frameStep(int diff) {return TffdshowBase::frameStep(diff);}
 STDMETHODIMP_(const TfilterIDFF*) getNextFilterIDFF(void) {return nextFilters;}
 STDMETHODIMP textPinConnected_(unsigned int num) {return E_NOTIMPL;}
 STDMETHODIMP cycleSubLanguages(int diff);
 STDMETHODIMP getLevelsMap(unsigned int map[256]);
 STDMETHODIMP_(const char_t*) findAutoSubflnm3(void);
 STDMETHODIMP setAverageTimePerFrame(int64_t *avg,int useDef);
 STDMETHODIMP getLate(int64_t *latePtr);
 STDMETHODIMP getAverageTimePerFrame(int64_t *avg);
 STDMETHODIMP_(const char_t*) getEmbeddedSubtitleName2_(unsigned int num) {return NULL;}
 STDMETHODIMP getEncoderInfo(char_t *buf,size_t buflen);
 STDMETHODIMP_(const char_t*) getDecoderName(void);
 STDMETHODIMP putHistogram_(unsigned int Ihistogram[256]) {return E_NOTIMPL;}
 STDMETHODIMP_(const char_t*) getCurrentSubFlnm(void);
 STDMETHODIMP quantsAvailable(void);
 STDMETHODIMP isNeroAVC_(void) {return E_NOTIMPL;}
 STDMETHODIMP findOverlayControl(IMixerPinConfig2* *overlayPtr);
 STDMETHODIMP getVideoDestRect(RECT *r);
 STDMETHODIMP_(FOURCC) getMovieFOURCC(void);
 STDMETHODIMP getRemainingFrameTime(unsigned int *sec);
 STDMETHODIMP getInputSAR(unsigned int *a1,unsigned int *a2);
 STDMETHODIMP getInputDAR(unsigned int *a1,unsigned int *a2);
 STDMETHODIMP getQuantMatrices(unsigned char intra8[64],unsigned char inter8[64]);
 STDMETHODIMP_(const char_t*) findAutoSubflnms(IcheckSubtitle *checkSubtitle);
 STDMETHODIMP addClosedCaption(const wchar_t* line);
 STDMETHODIMP hideClosedCaptions(void);
 STDMETHODIMP_(int) getConnectedTextPinCnt(void);
 STDMETHODIMP getConnectedTextPinInfo(int i,const char_t* *name,int *id,int *found);
 STDMETHODIMP registerOSDprovider(IOSDprovider *provider,const char *name);
 STDMETHODIMP unregisterOSDprovider(IOSDprovider *provider);
 STDMETHODIMP findOverlayControl2(IhwOverlayControl* *overlayPtr);
 STDMETHODIMP getChaptersList(void **ppChaptersList);

 //compatibility
 STDMETHODIMP compat_getIffDecoderVersion2(void);
 STDMETHODIMP compat_getParam(unsigned int paramID, int* value) {return getParam(paramID,value);}
 STDMETHODIMP compat_getParam2(unsigned int paramID) {return getParam2(paramID);}
 STDMETHODIMP compat_putParam(unsigned int paramID, int  value) {return putParam(paramID,value);}
 STDMETHODIMP compat_getNumPresets(unsigned int *value) {return getNumPresets(value);}
 STDMETHODIMP compat_getPresetName(unsigned int i,char *buf,unsigned int len) {return getDecInterface<IffdshowDecA>()->getPresetName(i,buf,len);}
 STDMETHODIMP compat_getActivePresetName(char *buf,unsigned int len) {return getDecInterface<IffdshowDecA>()->getActivePresetName(buf,len);}
 STDMETHODIMP compat_setActivePreset(const char *name,int create) {return setActivePreset(text<char_t>(name),create);}
 STDMETHODIMP compat_getAVIdimensions(unsigned int *x,unsigned int *y) {return getAVIdimensions(x,y);}
 STDMETHODIMP compat_getAVIfps(unsigned int *fps1000) {return getAVIfps(fps1000);}
 STDMETHODIMP compat_saveActivePreset(const char *name) {return saveActivePreset(text<char_t>(name));}
 STDMETHODIMP compat_saveActivePresetToFile(const char *flnm) {return  saveActivePresetToFile(text<char_t>(flnm));}
 STDMETHODIMP compat_loadActivePresetFromFile(const char *flnm) {return loadActivePresetFromFile(text<char_t>(flnm));}
 STDMETHODIMP compat_removePreset(const char *name) {return removePreset(text<char_t>(name));}
 STDMETHODIMP compat_notifyParamsChanged(void) {return notifyParamsChanged();}
 STDMETHODIMP compat_getAVcodecVersion(char *buf,unsigned int len) {return E_NOTIMPL;}
 STDMETHODIMP compat_getPPmode(unsigned int *ppmode) {return getPPmode(ppmode);}
 STDMETHODIMP compat_getRealCrop(unsigned int *left,unsigned int *top,unsigned int *right,unsigned int *bottom) {return getRealCrop(left,top,right,bottom);}
 STDMETHODIMP compat_getMinOrder2(void) {return getMinOrder2();}
 STDMETHODIMP compat_getMaxOrder2(void) {return getMaxOrder2();}
 STDMETHODIMP compat_saveGlobalSettings(void) {return saveGlobalSettings();}
 STDMETHODIMP compat_loadGlobalSettings(void) {return loadGlobalSettings();}
 STDMETHODIMP compat_saveDialogSettings(void) {return saveDialogSettings();}
 STDMETHODIMP compat_loadDialogSettings(void) {return loadDialogSettings();}
 STDMETHODIMP compat_getPresets(Tpresets *presets2) {return getPresets(presets2);}
 STDMETHODIMP compat_setPresets(const Tpresets *presets2) {return setPresets(presets2);}
 STDMETHODIMP compat_savePresets(void) {return savePresets();}
 STDMETHODIMP compat_getPresetPtr(Tpreset**preset) {return getPresetPtr(preset);}
 STDMETHODIMP compat_setPresetPtr(Tpreset *preset) {return setPresetPtr(preset);}
 STDMETHODIMP compat_renameActivePreset(const char *newName) {return renameActivePreset(text<char_t>(newName));}
 STDMETHODIMP compat_setOnChangeMsg(HWND wnd,unsigned int msg) {return setOnChangeMsg(wnd,msg);}
 STDMETHODIMP compat_setOnFrameMsg(HWND wnd,unsigned int msg) {return setOnFrameMsg(wnd,msg);}
 STDMETHODIMP compat_isDefaultPreset(const char *presetName) {return isDefaultPreset(text<char_t>(presetName));}
 STDMETHODIMP compat_showCfgDlg(HWND owner) {return showCfgDlg(owner);}
 STDMETHODIMP compat_getXvidVersion(char *buf,unsigned int len) {return E_NOTIMPL;}
 STDMETHODIMP compat_getMovieSource(const TvideoCodecDec* *moviePtr) {return getMovieSource(moviePtr);}
 STDMETHODIMP compat_getOutputDimensions(unsigned int *x,unsigned int *y) {return getOutputDimensions(x,y);}
 STDMETHODIMP compat_getCpuUsage2(void) {return getCpuUsage2();}
 STDMETHODIMP compat_getCpuUsageForPP(void) {return getCpuUsageForPP();}
 STDMETHODIMP compat_getOutputFourcc(char *buf,unsigned int len);//
 STDMETHODIMP compat_getInputBitrate2(void) {return getInputBitrate2();}
 STDMETHODIMP compat_getHistogram(unsigned int dst[256]) {return getHistogram_(dst);}
 STDMETHODIMP compat_setFilterOrder(unsigned int filterID,unsigned int newOrder) {return setFilterOrder(filterID,newOrder);}
 STDMETHODIMP compat_buildHistogram(const TffPict *pict,int full) {return buildHistogram_(pict,full);}
 STDMETHODIMP compat_cpuSupportsMMX(void) {return cpuSupportsMMX();}
 STDMETHODIMP compat_cpuSupportsMMXEXT(void) {return cpuSupportsMMXEXT();}
 STDMETHODIMP compat_cpuSupportsSSE(void) {return cpuSupportsSSE();}
 STDMETHODIMP compat_cpuSupportsSSE2(void) {return cpuSupportsSSE2();}
 STDMETHODIMP compat_cpuSupportsSSE3(void) {return cpuSupportsSSE3();}
 STDMETHODIMP compat_cpuSupportsSSSE3(void) {return cpuSupportsSSSE3();}
 STDMETHODIMP compat_cpuSupports3DNOW(void)    {return cpuSupports3DNOW();}
 STDMETHODIMP compat_cpuSupports3DNOWEXT(void) {return cpuSupports3DNOWEXT();}
 STDMETHODIMP compat_getAVIfps1000_2(void) {return getAVIfps1000_2();}
 STDMETHODIMP compat_getParamStr(unsigned int paramID,char *buf,unsigned int buflen) {return getBaseInterface<IffdshowBaseA>()->getParamStr(paramID,buf,buflen);}
 STDMETHODIMP compat_putParamStr(unsigned int paramID,const char *buf) {return putParamStr(paramID,text<char_t>(buf));}
 STDMETHODIMP compat_invParam(unsigned int paramID) {return invParam(paramID);}
 STDMETHODIMP compat_getInstance(HINSTANCE *hi) {return getInstance(hi);}
 STDMETHODIMP compat_saveKeysSettings(void) {return saveKeysSettings();}
 STDMETHODIMP compat_loadKeysSettings(void) {return loadKeysSettings();}
 STDMETHODIMP compat_seek(int seconds) {return seek(seconds);}
 STDMETHODIMP compat_tell(int*seconds) {return tell(seconds);}
 STDMETHODIMP compat_getDuration(int*seconds) {return getDuration(seconds);}
 STDMETHODIMP compat_getKeyParamCount2(void) {return getKeyParamCount2();}
 STDMETHODIMP compat_getKeyParamDescr(unsigned int i,const char **descr) {return getDecInterface<IffdshowDecA>()->getKeyParamDescr(i,descr);}
 STDMETHODIMP compat_getKeyParamKey2(unsigned int i) {return getKeyParamKey2(i);}
 STDMETHODIMP compat_setKeyParamKey(unsigned int i,int key) {return setKeyParamKey(i,key);}
 STDMETHODIMP compat_getImgFilters(TimgFilters* *imgFiltersPtr) {return E_NOTIMPL;}
 STDMETHODIMP compat_getQuant(int* *quantPtr) {return getQuant(quantPtr);}
 STDMETHODIMP compat_calcNewSize(Trect inRect,unsigned int *outDx,unsigned int *outDy) {return calcNewSize(inRect,outDx,outDy);}
 STDMETHODIMP compat_calcNewSize(unsigned int inDx,unsigned int inDy,unsigned int *outDx,unsigned int *outDy) {return calcNewSize(inDx,inDy,outDx,outDy);}
 STDMETHODIMP compat_grabNow(void) {return grabNow();}
 STDMETHODIMP compat_getOverlayControlCapability(int idff) {return getOverlayControlCapability(idff);}
 STDMETHODIMP compat_getParamName(unsigned int i,char *buf,unsigned int len) {return getBaseInterface<IffdshowBaseA>()->getParamName(i,buf,len);}
 STDMETHODIMP compat_getTranslator(Ttranslate* *trans) {return getTranslator(trans);}
 STDMETHODIMP compat_drawOSD(int px,int py,const char *txt) {return drawOSD(px,py,text<char_t>(txt));}
 STDMETHODIMP compat_lock(int lockId) {return TffdshowBase::lock(lockId);}
 STDMETHODIMP compat_unlock(int lockId) {return TffdshowBase::unlock(lockId);}
 STDMETHODIMP compat_getInstance2(void)
  {
  #ifdef WIN64
   return E_NOTIMPL;
  #else
   return (HRESULT)getInstance2();
  #endif
  }
 STDMETHODIMP compat_getGraph(IFilterGraph* *graphPtr) {return getGraph(graphPtr);}
 STDMETHODIMP compat_getConfig(Tconfig* *configPtr) {return getConfig((const Tconfig**)configPtr);}
 STDMETHODIMP compat_initDialog(void) {return initDialog();}
 STDMETHODIMP compat_initPresets(void) {return initPresets();}
 STDMETHODIMP compat_calcMeanQuant(float *quant) {return calcMeanQuant(quant);}
 STDMETHODIMP compat_initKeys(void) {return initKeys();}
 STDMETHODIMP compat_savePresetMem(void *buf,unsigned int len) {return savePresetMem(buf,len);}
 STDMETHODIMP compat_loadPresetMem(const void *buf,unsigned int len) {return loadPresetMem(buf,len);}
 STDMETHODIMP compat_getGlobalSettings(TglobalSettingsDecVideo* *globalSettingsPtr) {return getGlobalSettings((TglobalSettingsBase**)globalSettingsPtr);}
 STDMETHODIMP compat_createTempPreset(const char *presetName) {return createTempPreset(text<char_t>(presetName));}
 STDMETHODIMP compat_getParamStr2(unsigned int paramID)
  {
  #ifdef WIN64
   return E_NOTIMPL;
  #else
   return (HRESULT)getParamStr2(paramID);
  #endif
  }
 STDMETHODIMP compat_findAutoSubflnm2(void) {return findAutoSubflnm2();}
 STDMETHODIMP compat_getCurrentFrameTime(unsigned int *sec) {return getCurrentFrameTime(sec);}
 STDMETHODIMP compat_getFrameTime(unsigned int framenum,unsigned int *sec) {return getFrameTime(framenum,sec);}
 STDMETHODIMP compat_getCurTime2(void) {return getCurTime2();}
 STDMETHODIMP compat_getPostproc(Tlibmplayer* *postprocPtr) {return getPostproc(postprocPtr);}
 STDMETHODIMP compat_stop(void) {return stop();}
 STDMETHODIMP compat_run(void) {return run();}
 STDMETHODIMP compat_getState2(void) {return getState2();}
 STDMETHODIMP compat_resetFilter(unsigned int filterID) {return resetFilter(filterID);}
 STDMETHODIMP compat_resetFilterEx(unsigned int filterID,unsigned int filterPageId) {return resetFilterEx(filterID,filterPageId);}
 STDMETHODIMP compat_getFilterTip(unsigned int filterID,char *buf,unsigned int buflen) {return getDecInterface<IffdshowDecA>()->getFilterTip(filterID,buf,buflen);}
 STDMETHODIMP compat_getFilterTipEx(unsigned int filterID,unsigned int filterPageId,char *buf,unsigned int buflen) {return getDecInterface<IffdshowDecA>()->getFilterTipEx(filterID,filterPageId,buf,buflen);}
 STDMETHODIMP compat_filterHasReset(unsigned int filterID) {return filterHasReset(filterID);}
 STDMETHODIMP compat_filterHasResetEx(unsigned int filterID,unsigned int filterPageId) {return filterHasResetEx(filterID,filterPageId);}
 STDMETHODIMP compat_shortOSDmessage(const char *msg,unsigned int duration) {return shortOSDmessage(text<char_t>(msg),duration);}
 STDMETHODIMP compat_setImgFilters(TimgFilters *imgFiltersPtr) {return E_NOTIMPL;}
 STDMETHODIMP compat_registerSelectedMediaTypes(void) {return registerSelectedMediaTypes();}
 STDMETHODIMP compat_getFrameTimes(int64_t *start,int64_t *stop) {return getFrameTimes(start,stop);}
 STDMETHODIMP compat_getSubtitleTimes(int64_t *start,int64_t *stop) {return getSubtitleTimes(start,stop);}
 STDMETHODIMP compat_resetSubtitleTimes(void) {return resetSubtitleTimes();}
 STDMETHODIMP compat_setFrameTimes(int64_t start,int64_t stop) {return setFrameTimes(start,stop);}

 // IdecVideoSink
 STDMETHODIMP deliverDecodedSample(TffPict &pict);
 STDMETHODIMP deliverPreroll(int frametype);
 STDMETHODIMP acceptsManyFrames(void) {return S_OK;}
 STDMETHODIMP flushDecodedSamples(void);

 // IprocVideoSink
 STDMETHODIMP deliverProcessedSample(TffPict &pict);
 HRESULT initializeOutputSample(IMediaSample **ppOutSample);

 //TffdshowVideo
 virtual int getVideoCodecId(const BITMAPINFOHEADER *hdr,const GUID *subtype,FOURCC *AVIfourcc);
 virtual void initCodecSettings(void);
 virtual void lockReceive(void),unlockReceive(void);

 bool initSubtitles(int id,int type,const unsigned char *extradata,unsigned int extradatalen);
 void addSubtitle(int id,REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,bool utf8);
 void resetSubtitles(int id);
 bool ctlSubtitles(int id,int type,unsigned int ctl_id,const void *ctl_data,unsigned int ctl_datalen);

 void OSDtimeStartSampling(void);
 void OSDtimeEndSampling(void);
 void OSDtimeEndSamplingMax(void);
 STDMETHODIMP_(int) getQueuedCount(void);
 STDMETHODIMP_(__int64) getLate(void);
 STDMETHODIMP_(const char*)get_current_idct(void);
 STDMETHODIMP_(int) get_time_on_ffdshow(void);
 STDMETHODIMP_(int) get_time_on_ffdshow_percent(void);
 STDMETHODIMP_(bool) shouldSkipH264loopFilter(void);
 STDMETHODIMP_(int) get_downstreamID(void);
 STDMETHODIMP_(const char*) getAviSynthInfo(void);
 STDMETHODIMP lockCSReceive(void) { lockReceive(); return S_OK; }
 STDMETHODIMP unlockCSReceive(void) { unlockReceive(); return S_OK; }
 STDMETHODIMP_(ToutputVideoSettings*) getToutputVideoSettings(void);
 STDMETHODIMP_(int) getBordersBrightness(void);
#ifdef OSDTIMETABALE
 STDMETHODIMP_(int) getOSDtime(void){return (int)OSDtime3;}
#else
 STDMETHODIMP_(int) getOSDtime(void){return -1;}
#endif

private:
#ifdef OSDTIMETABALE
 REFERENCE_TIME OSDtime1;
 REFERENCE_TIME OSDtime2;
 REFERENCE_TIME OSDtime3;
 REFERENCE_TIME OSDtime4;
 REFERENCE_TIME OSDtime5;
 REFERENCE_TIME OSDtimeMax;
 REFERENCE_TIME OSDlastdisplayed;
#endif
 REFERENCE_TIME OSD_time_on_ffdshowStart;
 REFERENCE_TIME OSD_time_on_ffdshowBeforeGetBuffer;
 REFERENCE_TIME OSD_time_on_ffdshowAfterGetBuffer;
 REFERENCE_TIME OSD_time_on_ffdshowEnd;
 REFERENCE_TIME OSD_time_on_ffdshowResult;
 REFERENCE_TIME OSD_time_on_ffdshowOldStart;
 REFERENCE_TIME OSD_time_on_ffdshowDuration;
 bool isOSD_time_on_ffdshow;
 bool OSD_time_on_ffdshowFirstRun;

//#ifdef DEBUG
//#define DPRINTF_SAMPLE_TIME(x) DPRINTF_SampleTime(x)
//#else
#define DPRINTF_SAMPLE_TIME(x)
//#endif

 void DPRINTF_SampleTime(IMediaSample* pSample);

 struct TffdshowDecVideo_char : interfaces<tchar_traits<char_t>::other_char_t>::IffdshowDecVideo, CUnknown
 {
  protected:
   typedef tchar_traits<char_t>::other_char_t tchar;
  private:
   IffdshowDecVideoT<char_t> *deciV;
  public:
   TffdshowDecVideo_char(LPUNKNOWN punk,IffdshowDecVideoT<char_t> *IdeciV):deciV(IdeciV),CUnknown(NAME("TffdshowDecVideo_char"),punk) {}

   STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {return deciV->QueryInterface(riid,ppv);}
   STDMETHODIMP_(ULONG) AddRef() {return deciV->AddRef();}
   STDMETHODIMP_(ULONG) Release() {return deciV->Release();}

   STDMETHODIMP_(int) getVersion2(void) {return deciV->getVersion2();}
   STDMETHODIMP getAVIdimensions(unsigned int *x,unsigned int *y) {return deciV->getAVIdimensions(x,y);}
   STDMETHODIMP getAVIfps(unsigned int *fps1000) {return deciV->getAVIfps(fps1000);}
   STDMETHODIMP getAVcodecVersion(tchar *buf,size_t len) {return E_NOTIMPL;}
   STDMETHODIMP getPPmode(unsigned int *ppmode) {return deciV->getPPmode(ppmode);}
   STDMETHODIMP getRealCrop(unsigned int *left,unsigned int *top,unsigned int *right,unsigned int *bottom) {return deciV->getRealCrop(left,top,right,bottom);}
   STDMETHODIMP getXvidVersion(tchar *buf,size_t len) {return E_NOTIMPL;}
   STDMETHODIMP getMovieSource(const TvideoCodecDec* *moviePtr) {return deciV->getMovieSource(moviePtr);}
   STDMETHODIMP getOutputDimensions(unsigned int *x,unsigned int *y) {return deciV->getOutputDimensions(x,y);}
   STDMETHODIMP getOutputFourcc(tchar *buf,size_t buflen)
    {
     char_t *buft=(char_t*)alloca(buflen*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deciV->getOutputFourcc(buft,buflen)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP_(int) getInputBitrate2(void) {return deciV->getInputBitrate2();}
   STDMETHODIMP getHistogram_(unsigned int dst[256]) {return deciV->getHistogram_(dst);}
   STDMETHODIMP setFilterOrder(unsigned int filterID,unsigned int newOrder) {return deciV->setFilterOrder(filterID,newOrder);}
   STDMETHODIMP buildHistogram_(const TffPict *pict,int full) {return deciV->buildHistogram_(pict,full);}
   STDMETHODIMP_(int) getAVIfps1000_2(void) {return deciV->getAVIfps1000_2();}
   STDMETHODIMP getCurrentFrameTime(unsigned int *sec) {return deciV->getCurrentFrameTime(sec);}
   STDMETHODIMP getImgFilters_(void* *imgFiltersPtr) {return E_NOTIMPL;}
   STDMETHODIMP getQuant(int* *quantPtr) {return deciV->getQuant(quantPtr);}
   STDMETHODIMP calcNewSize(unsigned int inDx,unsigned int inDy,unsigned int *outDx,unsigned int *outDy) {return deciV->calcNewSize(inDx,inDy,outDx,outDy);}
   STDMETHODIMP grabNow(void) {return deciV->grabNow();}
   STDMETHODIMP getOverlayControlCapability(int idff) {return deciV->getOverlayControlCapability(idff);}
   STDMETHODIMP drawOSD(int px,int py,const tchar *txt) {return deciV->drawOSD(px,py,text<char_t>(txt));}
   STDMETHODIMP lock(int lockId) {return deciV->lock(lockId);}
   STDMETHODIMP unlock(int lockId) {return deciV->unlock(lockId);}
   STDMETHODIMP calcMeanQuant(float *quant) {return deciV->calcMeanQuant(quant);}
   STDMETHODIMP_(int) findAutoSubflnm2(void) {return deciV->findAutoSubflnm2();}
   STDMETHODIMP getFrameTime(unsigned int framenum,unsigned int *sec) {return deciV->getFrameTime(framenum,sec);}
   STDMETHODIMP shortOSDmessage(const tchar *msg,unsigned int duration) {return deciV->shortOSDmessage(text<char_t>(msg),duration);}
   STDMETHODIMP setImgFilters_(void *imgFiltersPtr) {return E_NOTIMPL;}
   STDMETHODIMP registerSelectedMediaTypes(void) {return deciV->registerSelectedMediaTypes();}
   STDMETHODIMP getFrameTimes(int64_t *start,int64_t *stop) {return deciV->getFrameTimes(start,stop);}
   STDMETHODIMP getSubtitleTimes(int64_t *start,int64_t *stop) {return deciV->getSubtitleTimes(start,stop);}
   STDMETHODIMP resetSubtitleTimes(void) {return deciV->resetSubtitleTimes();}
   STDMETHODIMP setFrameTimes(int64_t start,int64_t stop) {return deciV->setFrameTimes(start,stop);}
   STDMETHODIMP_(int) getOSDpresetCount2(void) {return deciV->getOSDpresetCount2();}
   STDMETHODIMP_(const tchar*) getOSDpresetName2(unsigned int i)
    {
     return NULL;
    }
   STDMETHODIMP_(const tchar*) getOSDpresetFormat2(const tchar *presetName)
    {
     return NULL;
    }
   STDMETHODIMP setOSDpresetFormat(const tchar *presetName,const tchar *format) {return deciV->setOSDpresetFormat(text<char_t>(presetName),text<char_t>(format));}
   STDMETHODIMP setOSDpresetName(unsigned int i,const tchar *name) {return deciV->setOSDpresetName(i,text<char_t>(name));}
   STDMETHODIMP addOSDpreset(const tchar *presetName,const tchar *format) {return deciV->addOSDpreset(text<char_t>(presetName),text<char_t>(format));}
   STDMETHODIMP deleteOSDpreset(const tchar *presetName) {return deciV->deleteOSDpreset(text<char_t>(presetName));}
   STDMETHODIMP cycleOSDpresets(void) {return deciV->cycleOSDpresets();}
   STDMETHODIMP_(int) getCodecId(const BITMAPINFOHEADER *hdr,const GUID *subtype,FOURCC *AVIfourcc) {return deciV->getCodecId(hdr,subtype,AVIfourcc);}
   STDMETHODIMP getFontManager(TfontManager* *fontManagerPtr) {return deciV->getFontManager(fontManagerPtr);}
   STDMETHODIMP_(int) getInIsSync(void) {return deciV->getInIsSync();}
   STDMETHODIMP getVideoWindowPos(int *left,int *top,unsigned int *width,unsigned int *height) {return deciV->getVideoWindowPos(left,top,width,height);}
   STDMETHODIMP_(unsigned int) getSubtitleLanguagesCount2(void) {return deciV->getSubtitleLanguagesCount2();}
   STDMETHODIMP getSubtitleLanguageDesc(unsigned int num,const tchar* *descPtr)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP fillSubtitleLanguages(const tchar **langs)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP resetOSD(void) {return deciV->resetOSD();}
   STDMETHODIMP getFrameTimeMS(unsigned int framenum,unsigned int *msec) {return deciV->getFrameTimeMS(framenum,msec);}
   STDMETHODIMP getCurrentFrameTimeMS(unsigned int *msec) {return deciV->getCurrentFrameTimeMS(msec);}
   STDMETHODIMP frameStep(int diff) {return deciV->frameStep(diff);}
   STDMETHODIMP textPinConnected_(unsigned int num) {return E_NOTIMPL;}
   STDMETHODIMP cycleSubLanguages(int diff) {return deciV->cycleSubLanguages(diff);}
   STDMETHODIMP getLevelsMap(unsigned int map[256]) {return deciV->getLevelsMap(map);}
   STDMETHODIMP_(const tchar*) findAutoSubflnm3(void)
    {
     return NULL;
    }
   STDMETHODIMP setAverageTimePerFrame(int64_t *avg,int useDef) {return deciV->setAverageTimePerFrame(avg,useDef);}
   STDMETHODIMP getLate(int64_t *latePtr) {return deciV->getLate(latePtr);}
   STDMETHODIMP getAverageTimePerFrame(int64_t *avg) {return deciV->getAverageTimePerFrame(avg);}
   STDMETHODIMP_(const tchar*) getEmbeddedSubtitleName2_(unsigned int num) {return NULL;}
   STDMETHODIMP putHistogram_(unsigned int Ihistogram[256]) {return deciV->putHistogram_(Ihistogram);}
   STDMETHODIMP_(const tchar*) getCurrentSubFlnm(void)
    {
     return NULL;
    }
   STDMETHODIMP quantsAvailable(void) {return deciV->quantsAvailable();}
   STDMETHODIMP isNeroAVC_(void) {return deciV->isNeroAVC_();}
   STDMETHODIMP findOverlayControl(IMixerPinConfig2* *overlayPtr) {return deciV->findOverlayControl(overlayPtr);}
   STDMETHODIMP getVideoDestRect(RECT *r) {return deciV->getVideoDestRect(r);}
   STDMETHODIMP_(FOURCC) getMovieFOURCC(void) {return deciV->getMovieFOURCC();}
   STDMETHODIMP getRemainingFrameTime(unsigned int *sec) {return deciV->getRemainingFrameTime(sec);}
   STDMETHODIMP getInputSAR(unsigned int *a1,unsigned int *a2) {return deciV->getInputSAR(a1,a2);}
   STDMETHODIMP getInputDAR(unsigned int *a1,unsigned int *a2) {return deciV->getInputDAR(a1,a2);}
   STDMETHODIMP getQuantMatrices(unsigned char intra8[64],unsigned char inter8[64]) {return deciV->getQuantMatrices(intra8,inter8);}
   STDMETHODIMP_(const tchar*) findAutoSubflnms(IcheckSubtitle *checkSubtitle)
    {
     return NULL;
    }
   STDMETHODIMP addClosedCaption(const wchar_t* line) {return deciV->addClosedCaption(line);}
   STDMETHODIMP hideClosedCaptions(void) {return deciV->hideClosedCaptions();}
   STDMETHODIMP_(int) getConnectedTextPinCnt(void) {return deciV->getConnectedTextPinCnt();}
   STDMETHODIMP getConnectedTextPinInfo(int i,const tchar* *name,int *id,int *found)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP registerOSDprovider(IOSDprovider *provider,const char *name) {return deciV->registerOSDprovider(provider,name);}
   STDMETHODIMP unregisterOSDprovider(IOSDprovider *provider) {return deciV->unregisterOSDprovider(provider);}
   STDMETHODIMP_(int) getOSDtime(void){return deciV->getOSDtime();}
   STDMETHODIMP_(int) getQueuedCount(void){return deciV->getQueuedCount();}
   STDMETHODIMP_(__int64) getLate(void){return deciV->getLate();}
   STDMETHODIMP_(const char*)get_current_idct(void){return deciV->get_current_idct();}
   STDMETHODIMP_(int) get_time_on_ffdshow(void){return deciV->get_time_on_ffdshow();}
   STDMETHODIMP_(int) get_time_on_ffdshow_percent(void){return deciV->get_time_on_ffdshow_percent();}
   STDMETHODIMP_(bool) shouldSkipH264loopFilter(void){return deciV->shouldSkipH264loopFilter();}
   STDMETHODIMP findOverlayControl2(IhwOverlayControl* *overlayPtr) {return deciV->findOverlayControl2(overlayPtr);}
   STDMETHODIMP_(int) get_downstreamID(void) {return deciV->get_downstreamID();}
   STDMETHODIMP_(const char*)getAviSynthInfo(void){return deciV->getAviSynthInfo();}
   STDMETHODIMP lockCSReceive(void) { return deciV->lockCSReceive(); }
   STDMETHODIMP unlockCSReceive(void) { return deciV->unlockCSReceive(); }
   STDMETHODIMP_(ToutputVideoSettings*) getToutputVideoSettings(void) {return deciV->getToutputVideoSettings();}
   STDMETHODIMP_(int) getBordersBrightness(void){return deciV->getBordersBrightness();}
   STDMETHODIMP getChaptersList(void **ppChaptersList) { return E_NOTIMPL; };
 } decVideo_char;
 template<class Tinterface> Tinterface* getDecVideoInterface(void);
 void ConnectCompatibleFilter(void);
 void DisconnectFromCompatibleFilter(void);
protected:
 TffdshowDecVideoOutputPin *m_pOutputDecVideo;
 int IsQueueListedApp(const char_t *exe);
 HRESULT Receive(IMediaSample *pSample);
 virtual void getMinMax(int id,int &min,int &max);
 virtual bool isStreamsMenu(void) const;
 virtual TinfoBase* createInfo(void);
 virtual TimgFilters* createImgFilters(void);

 TimgFilters *imgFilters;
 TpresetVideo *presetSettings;
 TglobalSettingsDecVideo *globalSettings;
 bool m_aboutToFlash;
 int m_IsQueueListedApp;        // -1: first run, 0: false, 1: true
 IPin *inputConnectedPin;
private:
 HRESULT ReceiveI(IMediaSample *pSample);
 static const int VERSION=38;
 static const int COMPAT_VERSION=28;
 TffdshowVideoInputPin *inpin;

 TdialogSettingsDecVideo *dialogSettings;
 TpresetsVideo *presets;
 int currentq;

 virtual void sendOnChange(int paramID,int val);

 int subShowEmbedded;//,foundEmbedded;
 TfontManager *fontManager;
 bool wasSubtitleResetTime;
 TpinsVector<TtextInputPin> textpins;
 strings subtitleLanguages;
 int subCurLang;
 int isQueue;
 bool m_IsOldVideoRenderer;    // Note: This flag may be true when ffdshow is connected to overlaymixer, because overlaymixer often connects to old video renderer.
 bool m_IsOldVMR9RenderlessAndRGB;
 bool m_IsYV12andVMR9;
 bool reconnectFirstError;
 bool m_NeedToAttachFormat,m_NeedToPauseRun;
 HANDLE hReconnectEvent;
 ALLOCATOR_PROPERTIES ppropActual;

 bool IsOldRenderer(void);
 bool IsOldVMR9RenderlessAndRGB(void);
 bool IsVMR9Renderless(IPin *downstream_input_pin);
 HRESULT setOutputMediaType(const CMediaType &mt);
 struct
  {
   int dstColorspace;
   int dstStride;
   LONG dstSize;
  } m_frame;
 Trect oldRect;
 TffPictBase reconnectRect;
 bool inReconnect;
 void calcNewSize(TffPictBase &p);
 unsigned int frameCnt;uint64_t bytesCnt;
 REFERENCE_TIME segmentStart;unsigned int segmentFrameCnt;
 REFERENCE_TIME vc1rtStart;int vc1frameCnt;
 int currentFrame;
 int decodingFps,decodingCsp;
 HRESULT reconnectOutput(const TffPict &newpict);

 clock_t lastTime;
 int waitForKeyframe;
 void setSampleSkipped(bool sendSkip);
 REFERENCE_TIME late,lastrtStart;
 unsigned int count_decoded_frames_for_framerate_calculation;
 REFERENCE_TIME decoded_rtStarts[4];
 REFERENCE_TIME eighth_decoded_rtStart; // for frame rate calculation. After seeking, a few frames may have incorrect timestamps.

 IVideoWindow *videoWindow;bool wasVideoWindow;
 IBasicVideo *basicVideo;bool wasBasicVideo;

 static const TfilterIDFF nextFilters[];

 int dvdproc;bool allowOutChange;
 bool outOverlayMixer,outOldRenderer,outdv;
 int hwDeinterlace;
 STDMETHODIMP_(int) get_trayIconType(void);
 bool compatibleFilterConnected;
 DWORD inSampleTypeSpecificFlags;
 bool inSampleEverField1Repeat;
 REFERENCE_TIME m_rtStart;
 REFERENCE_TIME insample_rtStart,insample_rtStop;
 HRESULT checkAllowOutChange(IPin *pPin);
 TsearchInterfaceInGraph *searchInterfaceInGraph;
 void update_time_on_ffdshow1(void);
 void update_time_on_ffdshow2(void);
 void update_time_on_ffdshow3(REFERENCE_TIME rtStart, REFERENCE_TIME rtStop);
 void update_time_on_ffdshow4(void);
public:
 enum DOWNSTREAM_FILTER_TYPE
  {
   UNKNOWN,
   OLD_RENDERER,
   OVERLAY_MIXER,
   VMR7,
   VMR9,
   VMR9RENDERLESS_MPC,
   DVOBSUB,
   HAALI_RENDERER
  } downstreamID;
 void set_downstreamID(IPin *downstream_input_pin);
 static const AMOVIESETUP_MEDIATYPE inputMediaTypes[],outputMediaTypes[];
 static AMOVIESETUP_PIN pins[];
 static const AMOVIESETUP_FILTER filter;
 static const DWORD defaultMerit;
 friend class TffdshowDecVideoOutputPin;
 std::vector<std::pair<long, ffstring> > chaptersList;
};

class TffdshowDecVideoRaw :public TffdshowDecVideo
{
 static const AMOVIESETUP_MEDIATYPE inputMediaTypes[];
public:
 static AMOVIESETUP_PIN pins[];
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr);
 TffdshowDecVideoRaw(LPUNKNOWN punk,HRESULT *phr);
 static const AMOVIESETUP_FILTER filter;
 static const DWORD defaultMerit;
};

class TffdshowDecVideoSubtitles :public TffdshowDecVideo
{
 static const AMOVIESETUP_MEDIATYPE inputMediaTypes[];
public:
 static AMOVIESETUP_PIN pins[];
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr);
 TffdshowDecVideoSubtitles(LPUNKNOWN punk,HRESULT *phr);
 static const AMOVIESETUP_FILTER filter;
 static const DWORD defaultMerit;
};

#endif
