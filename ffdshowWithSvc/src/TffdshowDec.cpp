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

#include "stdafx.h"
#include "TffdshowDec.h"
#include "TdialogSettings.h"
#include "TglobalSettings.h"
#include "Tpresets.h"
#include "TpresetSettings.h"
#include "Ttranslate.h"
#include "TkeyboardDirect.h"
#include "ffdebug.h"
#include "ffdshowRemoteAPIimpl.h"
#include "Tfilters.h"

STDMETHODIMP_(int) TffdshowDec::getVersion2(void)
{
 return VERSION;
}

template<> interfaces<char_t>::IffdshowDec* TffdshowDec::getDecInterface(void)
{
 return this;
}
template<> interfaces<tchar_traits<char_t>::other_char_t>::IffdshowDec* TffdshowDec::getDecInterface(void)
{
 return &dec_char;
}

STDMETHODIMP TffdshowDec::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 if (globalSettings->isBlacklist && (riid==IID_IBaseFilter || riid==IID_IMediaFilter) && globalSettings->inBlacklist(getExeflnm()))
  return E_NOINTERFACE;
 if (globalSettings->isWhitelist && (riid==IID_IBaseFilter || riid==IID_IMediaFilter) && !globalSettings->inWhitelist(getExeflnm(),this))
  return E_NOINTERFACE;
 if (riid==IID_IffdshowBaseA)
  return GetInterface<IffdshowBaseA>(getBaseInterface<IffdshowBaseA>(),ppv);
 if (riid==IID_IffdshowBaseW)
  return GetInterface<IffdshowBaseW>(getBaseInterface<IffdshowBaseW>(),ppv);
 else if (riid==IID_IffdshowDecA)
  return GetInterface<IffdshowDecA>(getDecInterface<IffdshowDecA>(),ppv);
 else if (riid==IID_IffdshowDecW)
  return GetInterface<IffdshowDecW>(getDecInterface<IffdshowDecW>(),ppv);
 else if (riid==IID_ISpecifyPropertyPages)
  return GetInterface<ISpecifyPropertyPages>(this,ppv);
 else if (riid==IID_IAMStreamSelect && isStreamsMenu())
  return GetInterface<IAMStreamSelect>(this,ppv);
 else if (riid==IID_IffdshowParamsEnum)
  return GetInterface<IffdshowParamsEnum>(this,ppv);
 else
  return CTransformFilter::NonDelegatingQueryInterface(riid,ppv);
}

TffdshowDec::TffdshowDec(TintStrColl *Ioptions,const TCHAR *name,LPUNKNOWN punk,REFCLSID clsid,TglobalSettingsDec *IglobalSettings,TdialogSettingsDec *IdialogSettings,Tpresets *Ipresets,Tpreset* &IpresetSettings,CmyTransformFilter *Imfilter,TinputPin* &Iminput,CTransformOutputPin* &Imoutput,IFilterGraph* &Igraph,Tfilters* &Ifilters,const CLSID &Iproppageid,int IcfgDlgCaptionId,int IiconId,DWORD IdefaultMerit):
 presets(Ipresets),
 presetSettings(IpresetSettings),
 filters(Ifilters),
 CmyTransformFilter(name,punk,clsid),
 TffdshowBase(
  punk,
  Ioptions,
  globalSettings=IglobalSettings,
  dialogSettings=IdialogSettings,
  Imfilter,
  Iminput,
  Imoutput,
  Igraph,
  IcfgDlgCaptionId,IiconId,
  IdefaultMerit),
 proppageid(Iproppageid),
 dec_char(punk,this),
 m_dirtyStop(false)
{
 static const TintOptionT<TffdshowDec> iopts[]=
  {
   IDFF_movieDuration ,&TffdshowDec::moviesecs,-1,-1,_l(""),0,NULL,0,
   0
  };
 addOptions(iopts);

 keys=NULL;mouse=NULL;
 remote=NULL;
 hereSeek=false;
 presetSettings=NULL;
 firsttransform=true;discontinuity=true;
 filters=NULL;
 onNewFiltersWnd=NULL;onNewFiltersMsg=0;
}
TffdshowDec::~TffdshowDec()
{
 if (presets) delete presets;
 if (keys) delete keys;
 if (mouse) delete mouse;
 if (remote) delete remote;
}

STDMETHODIMP TffdshowDec::getGraph(IFilterGraph* *graphPtr)
{
 return TffdshowBase::getGraph(graphPtr);
}
STDMETHODIMP_(int) TffdshowDec::getState2(void)
{
 return TffdshowBase::getState2();
}

HRESULT TffdshowDec::onInitDialog(void)
{
 initPresets();
 if (!presetSettings) setActivePreset(globalSettings->defaultPreset,false);
 return TffdshowBase::onInitDialog();
}

//tell and seek
STDMETHODIMP TffdshowDec::tell(int *seconds)
{
 return TffdshowBase::tell(seconds);
}
STDMETHODIMP TffdshowDec::seek(int seconds)
{
 return TffdshowBase::seek(seconds);
}
STDMETHODIMP TffdshowDec::stop(void)
{
 return TffdshowBase::stop();
}
STDMETHODIMP_(int) TffdshowDec::getCurTime2(void)
{
 return TffdshowBase::getCurTime2();
}
STDMETHODIMP TffdshowDec::run(void)
{
 return TffdshowBase::run();
}
STDMETHODIMP TffdshowDec::getDuration(int *duration)
{
 return E_NOTIMPL;
}

STDMETHODIMP TffdshowDec::initKeys(void)
{
 if (!keys) keys=new Tkeyboard(options,this);
 return S_OK;
}
STDMETHODIMP TffdshowDec::saveKeysSettings(void)
{
 if (!keys) return S_FALSE;
 keys->save();
 return S_OK;
}
STDMETHODIMP TffdshowDec::loadKeysSettings(void)
{
 if (!keys) return S_FALSE;
 keys->load();
 return S_OK;
}
STDMETHODIMP_(int) TffdshowDec::getKeyParamCount2(void)
{
 return keys?(int)keys->keysParams.size():0;
}
STDMETHODIMP TffdshowDec::getKeyParamDescr(unsigned int i,const char_t **descr)
{
 if (!keys || i>=keys->keysParams.size())
  {
   *descr=_l("");
   return S_FALSE;
  };
 *descr=keys->keysParams[i].descr;
 return S_OK;
}
STDMETHODIMP_(int) TffdshowDec::getKeyParamKey2(unsigned int i)
{
 if (!keys || i>=keys->keysParams.size()) return 0;
 return keys->keysParams[i].key;
}
STDMETHODIMP TffdshowDec::setKeyParamKey(unsigned int i,int key)
{
 if (!keys || i>=keys->keysParams.size()) return S_FALSE;
 keys->keysParams[i].key=key;
 sendOnChange(IDFF_keysSeek1/*just not to send 0*/,key);
 return S_OK;
}
STDMETHODIMP TffdshowDec::setKeyParamKeyCheck(unsigned int i,int key,int *prev,const char_t* *prevDescr)
{
 if (key!=0 && (prev || prevDescr))
  for (unsigned int previ=0;previ<keys->keysParams.size();previ++)
   if (previ!=i && keys->keysParams[previ].key==key)
    {
     if (prev) *prev=previ;
     if (prevDescr) *prevDescr=keys->keysParams[previ].descr;
     return E_INVALIDARG;
    }
 return setKeyParamKey(i,key);
}

STDMETHODIMP TffdshowDec::resetKeys(void)
{
 if (!keys) return S_FALSE;
 keys->reset();
 return S_OK;
}
STDMETHODIMP TffdshowDec::exportKeysToGML(const char_t *flnm)
{
 return keys->exportToGML(flnm)?S_OK:E_FAIL;
}

STDMETHODIMP TffdshowDec::initMouse(void)
{
 if (!mouse) mouse=new Tmouse(options,this);
 return S_OK;
}

STDMETHODIMP TffdshowDec::initRemote(void)
{
 if (!remote) remote=new Tremote(options,this);
 return S_OK;
}
STDMETHODIMP TffdshowDec::saveRemoteSettings(void)
{
 if (!remote) return S_FALSE;
 remote->save();
 return S_OK;
}
STDMETHODIMP TffdshowDec::loadRemoteSettings(void)
{
 if (!remote) return S_FALSE;
 remote->load();
 return S_OK;
}

void TffdshowDec::onTrayIconChange(int id,int newval)
{
 if (!firsttransform && (globalSettings->filtermode&IDFF_FILTERMODE_PROC)==0)
  TffdshowBase::onTrayIconChange(id,newval);
}

STDMETHODIMP TffdshowDec::getNumPresets(unsigned int *value)
{
 if (!value) return E_POINTER;
 *value=(unsigned int)presets->size();
 return S_OK;
}
STDMETHODIMP TffdshowDec::getPresetName(unsigned int i,char_t *buf,size_t len)
{
 if (!buf) return E_POINTER;
 if (i>=presets->size()) return S_FALSE;
 if (len<strlen((*presets)[i]->presetName)+1) return E_OUTOFMEMORY;
 strcpy(buf,(*presets)[i]->presetName);
 return S_OK;
}
STDMETHODIMP TffdshowDec::getActivePresetName(char_t *buf,size_t len)
{
 if (!buf) return E_POINTER;
 if (!presetSettings) return S_FALSE;
 if (len<strlen(presetSettings->presetName)+1) return E_OUTOFMEMORY;
 strcpy(buf,presetSettings->presetName);
 return S_OK;
}
STDMETHODIMP_(const char_t*) TffdshowDec::getActivePresetName2(void)
{
 return presetSettings?presetSettings->presetName:NULL;
}

STDMETHODIMP TffdshowDec::setActivePreset(const char_t *name,int create)
{
 if (!name) return S_FALSE;
 Tpreset *preset=presets->getPreset(name,!!create);
 if (preset)
  {
   setPresetPtr(preset);
   return S_OK;
  }
 else
  return S_FALSE;
}
STDMETHODIMP TffdshowDec::saveActivePreset(const char_t *name)
{
 if (!presetSettings) return E_INVALIDARG;
 presets->savePreset(presetSettings,name);
 return S_OK;
}
STDMETHODIMP TffdshowDec::saveActivePresetToFile(const char_t *flnm)
{
 if (!flnm || !presetSettings) return E_INVALIDARG;
 return presets->savePresetFile(presetSettings,flnm)?S_OK:E_FAIL;
}
STDMETHODIMP TffdshowDec::loadActivePresetFromFile(const char_t *flnm)
{
 if (!flnm) return E_INVALIDARG;
 if (!presetSettings)
  presetSettings=presets->newPreset();
 if (presetSettings->loadFile(flnm))
  {
   presets->storePreset(presetSettings);
   notifyParamsChanged();
   return S_OK;
  }
 else return E_FAIL;
}
STDMETHODIMP TffdshowDec::savePresetMem(void *buf,size_t len)
{
 if (!presetSettings) return E_UNEXPECTED;
 if (!buf) return E_POINTER;
 TregOpIDstreamWrite t;
 presetSettings->reg_op(t);

 if (len==0)
  *(int*)buf=(int)t.size();
 else
  memcpy(buf,&*t.begin(),std::min(size_t(len),t.size()));
 return S_OK;
}
STDMETHODIMP TffdshowDec::loadPresetMem(const void *buf,size_t len)
{
 if (!presetSettings) return E_UNEXPECTED;
 TregOpIDstreamRead t(buf,len);
 presetSettings->reg_op(t);
 return 0;
}

STDMETHODIMP TffdshowDec::removePreset(const char_t *name)
{
 if (stricmp(presetSettings->presetName,globalSettings->defaultPreset)==0)
  putParamStr(IDFF_defaultPreset,(*presets)[0]->presetName);
 return presets->removePreset(name)?S_OK:S_FALSE;
}
STDMETHODIMP TffdshowDec::getPresets(Tpresets *presets2)
{
 if (!presets2) return E_INVALIDARG;
 presets2->done();
 for (Tpresets::iterator i=presets->begin();i!=presets->end();i++)
  presets2->push_back((*i)->copy());
 return S_OK;
}
STDMETHODIMP TffdshowDec::setPresets(const Tpresets *presets2)
{
 if (!presets2) return E_POINTER;
 presets->done();
 for (Tpresets::const_iterator i=presets2->begin();i!=presets2->end();i++)
  presets->push_back((*i)->copy());
 return S_OK;
}
STDMETHODIMP TffdshowDec::savePresets(void)
{
 presets->saveRegAll();
 return S_OK;
}
STDMETHODIMP TffdshowDec::setPresetPtr(Tpreset *preset)
{
 if (!preset) return E_POINTER;
 if (presetSettings!=preset)
  {
   lock(IDFF_lockPresetPtr);
   presetSettings=preset;
   presetSettings->options->setSendOnChange(fastdelegate::MakeDelegate(this,&TffdshowDec::sendOnChange));
   notifyParamsChanged();
   sendOnChange(0,0);
   unlock(IDFF_lockPresetPtr);
  }
 return S_OK;
}
STDMETHODIMP TffdshowDec::getPresetPtr(Tpreset**preset)
{
 if (!preset) return E_POINTER;
 *preset=presetSettings;
 return S_OK;
}
void TffdshowDec::getColls(TintStrColls &colls)
{
 if (presetSettings)
  colls.push_back(presetSettings->options);
 TffdshowBase::getColls(colls);
}

STDMETHODIMP TffdshowDec::getParam(unsigned int paramID,int* value)
{
 if (!value) return S_FALSE;
 if (presetSettings && presetSettings->options->get(paramID,value))
  return S_OK;
 else
  return TffdshowBase::getParam(paramID,value);
}
STDMETHODIMP TffdshowDec::putParam(unsigned int paramID,int val)
{
 return presetSettings && presetSettings->options->set(paramID,val)?S_OK:TffdshowBase::putParam(paramID,val);
}
STDMETHODIMP TffdshowDec::invParam(unsigned int paramID)
{
 return presetSettings && presetSettings->options->inv(paramID)?S_OK:TffdshowBase::invParam(paramID);
}
STDMETHODIMP TffdshowDec::resetParam(unsigned int paramID)
{
 return presetSettings && presetSettings->options->reset(paramID)?S_OK:TffdshowBase::resetParam(paramID);
}

STDMETHODIMP TffdshowDec::getParamStr3(unsigned int paramID,const char_t* *value)
{
 if (!value) return S_FALSE;
 if (presetSettings && presetSettings->options->get(paramID,value))
  return S_OK;
 else
  return TffdshowBase::getParamStr3(paramID,value);
}
STDMETHODIMP TffdshowDec::putParamStr(unsigned int paramID,const char_t *buf)
{
 if (!buf) return S_FALSE;
 return presetSettings && presetSettings->options->set(paramID,buf)?S_OK:TffdshowBase::putParamStr(paramID,buf);
}
STDMETHODIMP TffdshowDec::getParamInfo(unsigned int paramID,TffdshowParamInfo *paramPtr)
{
 if (!paramPtr) return E_POINTER;
 return presetSettings && presetSettings->options->getInfo(paramID,paramPtr)?S_OK:TffdshowBase::getParamInfo(paramID,paramPtr);
}
STDMETHODIMP TffdshowDec::notifyParam(int id,int val)
{
 return presetSettings && presetSettings->options->notifyParam(id,val)?S_OK:TffdshowBase::notifyParam(id,val);
}
STDMETHODIMP TffdshowDec::notifyParamStr(int id,const char_t *val)
{
 return presetSettings && presetSettings->options->notifyParam(id,val)?S_OK:TffdshowBase::notifyParamStr(id,val);
}

STDMETHODIMP TffdshowDec::notifyParamsChanged(void)
{
 if (filters) filters->onQueueChange(0,0);
 return TffdshowBase::notifyParamsChanged();
}

STDMETHODIMP TffdshowDec::renameActivePreset(const char_t *newName)
{
 if (!newName) return E_POINTER;
 if (strlen(newName)>MAX_PATH) return E_OUTOFMEMORY;
 int res=stricmp(presetSettings->presetName,globalSettings->defaultPreset);
 strcpy(presetSettings->presetName,newName);
 if (res==0) putParamStr(IDFF_defaultPreset,newName);
 sendOnChange(0,0);
 return S_OK;
}
STDMETHODIMP TffdshowDec::isDefaultPreset(const char_t *presetName)
{
 return stricmp(globalSettings->defaultPreset,presetName)==0?1:0;
}
STDMETHODIMP TffdshowDec::initPresets(void)
{
 if (!presets->empty()) return E_UNEXPECTED;
 presets->init();
 return S_OK;
}

void TffdshowDec::initPreset(void)
{
 DPRINTF(_l("initPreset"));

 if (presets->empty()) initPresets();

 getSourceName();

 if (globalSettings->autoPreset)
  if (Tpreset *preset=presets->getAutoPreset(this,!!globalSettings->autoPresetFileFirst))
   {
    setPresetPtr(preset);
    return;
   }
 setActivePreset(globalSettings->defaultPreset,false);
}

STDMETHODIMP TffdshowDec::createTempPreset(const char_t *presetName)
{
 if (presets->empty()) return E_UNEXPECTED;
 Tpreset *preset=presets->newPreset(presetName);
 preset->autoLoadedFromFile=true;
 preset->loadReg();
 presets->push_back(preset);
 return S_OK;
}

STDMETHODIMP TffdshowDec::putStringParams(const char_t *params,char_t sep,int loaddef)
{
 if (!presetSettings) return E_UNEXPECTED;
 if (!params) return E_POINTER;
 TregOpStreamRead t(params,strlen(params),sep,!!loaddef);
 presetSettings->reg_op(t);
 notifyParamsChanged();
 return S_OK;
}

STDMETHODIMP TffdshowDec::resetFilter(unsigned int filterID)
{
 return resetFilterEx(filterID,0);
}
STDMETHODIMP TffdshowDec::resetFilterEx(unsigned int filterID,unsigned int filterPageId)
{
 if (!presetSettings) return E_UNEXPECTED;
 TfilterSettings *fs=presetSettings->getSettings(filterID);
 if (!fs) return E_INVALIDARG;
 return fs->reset(filterPageId)?S_OK:S_FALSE;
}
STDMETHODIMP TffdshowDec::filterHasReset(unsigned int filterID)
{
 return filterHasResetEx(filterID,0);
}
STDMETHODIMP TffdshowDec::filterHasResetEx(unsigned int filterID,unsigned int filterPageId)
{
 if (!presetSettings) return E_UNEXPECTED;
 TfilterSettings *fs=presetSettings->getSettings(filterID);
 if (!fs) return E_INVALIDARG;
 return fs->hasReset(filterPageId)?S_OK:S_FALSE;
}

STDMETHODIMP TffdshowDec::getFilterTip(unsigned int filterID,char_t *buf,size_t buflen)
{
 return getFilterTipEx(filterID,0,buf,buflen);
}
STDMETHODIMP TffdshowDec::getFilterTipEx(unsigned int filterID,unsigned int filterPageId,char_t *buf,size_t buflen)
{
 if (!presetSettings) return E_UNEXPECTED;
 if (!buf) return E_POINTER;
 TfilterSettings *fs=presetSettings->getSettings(filterID);
 if (!fs) return E_INVALIDARG;
 return fs->getTip(filterPageId,buf,buflen)?S_OK:S_FALSE;
}

STDMETHODIMP TffdshowDec::getPresetsPtr(Tpresets* *presetsPtr)
{
 if (!presetsPtr) return E_POINTER;
 *presetsPtr=presets;
 return S_OK;
}
STDMETHODIMP TffdshowDec::cyclePresets(int step)
{
 if (!presets || !presetSettings) return E_UNEXPECTED;
 Tpreset* preset=presets->getPreset(presetSettings->presetName,false);
 Tpresets::const_iterator pi=std::find(presets->begin(),presets->end(),preset);
 if (pi==presets->end()) return E_FAIL;
 for (int i=0;i!=step;i+=sign(step))
  {
   if (sign(step)==-1 && pi==presets->begin())
    pi=presets->end()-1;
   else if (sign(step)==1 && pi==presets->end()-1)
    pi=presets->begin();
   else
    pi+=sign(step);
  }
 setActivePreset((*pi)->presetName,false);
 return S_OK;
}

STDMETHODIMP TffdshowDec::newSample(IMediaSample* *samplePtr)
{
 return E_NOTIMPL;
}


STDMETHODIMP TffdshowDec::JoinFilterGraph(IFilterGraph *pGraph,LPCWSTR pName)
{
 return onJoinFilterGraph(pGraph,pName);
}

HRESULT TffdshowDec::BreakConnect(PIN_DIRECTION dir)
{
/*
 if (dir==PINDIR_OUTPUT && filters)
  filters->onDisconnect(dir);
*/
 return CTransformFilter::BreakConnect(dir);
}

HRESULT TffdshowDec::onGraphRemove(void)
{
 if (keys) delete keys;keys=NULL;
 if (mouse) delete mouse;mouse=NULL;
 if (remote) delete remote;remote=NULL;
 return TffdshowBase::onGraphRemove();
}

HRESULT TffdshowDec::StopStreaming(void)
{
 if (filters) filters->onStop();
 return CTransformFilter::StopStreaming();
}

STDMETHODIMP_(int) TffdshowDec::getMinOrder2(void)
{
 if (!presetSettings) return INT_MIN;
 return presetSettings->getMinOrder();
}
STDMETHODIMP_(int) TffdshowDec::getMaxOrder2(void)
{
 if (!presetSettings) return INT_MIN;
 return presetSettings->getMaxOrder();
}
STDMETHODIMP TffdshowDec::resetOrder(void)
{
 if (!presetSettings) return E_FAIL;
 if (presetSettings->resetOrder())
  sendOnChange(0,0);
 if (filters) filters->onQueueChange(0,0);
 return S_OK;
}
STDMETHODIMP TffdshowDec::setFilterOrder(unsigned int filterID,unsigned int newOrder)
{
 if (!presetSettings) return S_FALSE;
 return presetSettings->setFilterOrder(filterID,newOrder)?S_OK:S_FALSE;
}

STDMETHODIMP TffdshowDec::get_ExSeekCapabilities(long * pExCapabilities)
{
 *pExCapabilities=AM_EXSEEK_BUFFERING|AM_EXSEEK_NOSTANDARDREPAINT|AM_EXSEEK_CANSEEK|AM_EXSEEK_CANSCAN;//|AM_EXSEEK_SCANWITHOUTCLOCK;
 return S_OK;
}
STDMETHODIMP TffdshowDec::get_MarkerCount(long * pMarkerCount)
{
 *pMarkerCount=TffdshowBase::getDuration();
 return S_OK;
}
STDMETHODIMP TffdshowDec::get_CurrentMarker(long * pCurrentMarker)
{
 *pCurrentMarker=getCurTime2();
 return S_OK;
}
STDMETHODIMP TffdshowDec::GetMarkerTime(long MarkerNum, double * pMarkerTime)
{
 DPRINTF(_l("GetMarkerTime :%i"),MarkerNum);
 *pMarkerTime=MarkerNum;
 return S_OK;
}
STDMETHODIMP TffdshowDec::GetMarkerName(long MarkerNum, BSTR * pbstrMarkerName)
{
 return E_NOTIMPL;
}
STDMETHODIMP TffdshowDec::put_PlaybackSpeed(double Speed)
{
 if (comptrQ<IMediaSeeking> seek=graph)
  return seek->SetRate(Speed);
 else
  return E_NOINTERFACE;
}
STDMETHODIMP TffdshowDec::get_PlaybackSpeed(double * pSpeed)
{
 if (comptrQ<IMediaSeeking> seek=graph)
  return seek->GetRate(pSpeed);
 else
  return E_NOINTERFACE;
}

AM_MEDIA_TYPE* TffdshowDec::getInputMediaType(int lIndex)
{
 return CreateMediaType(&m_pInput->CurrentMediaType());
}

bool TffdshowDec::streamsSort(const Tstream *s1,const Tstream *s2)
{
 return s2->order>s1->order;
}

STDMETHODIMP TffdshowDec::Count(DWORD* pcStreams)
{
 if (!pcStreams) return E_POINTER;
 if (cfgDlgHwnd || !presetSettings)
  {
   *pcStreams=0;
   return S_OK;
  }
 for (Tstreams::iterator s=streams.begin();s!=streams.end();s++) delete *s;streams.clear();
 Ttranslate *tr;getTranslator(&tr);
 if (globalSettings->streamsMenu)
  {
   for (unsigned int i=0;i<presets->size();i++)
    streams.push_back(new TstreamPreset(this,-200+i,0,(*presets)[i]->presetName));
   const char_t *activepresetname=getActivePresetName2();
   if (activepresetname)
    {
     const TfilterIDFFs *filters;getFilterIDFFs(activepresetname,&filters);
     for (TfilterIDFFs::const_iterator f=filters->begin();f!=filters->end();f++)
      if (f->idff->is && (f->idff->show==0 || getParam2(f->idff->show)))
	   // 10 is the group of the stream. 1 is for audio stream so should not be used
       streams.push_back(new TstreamFilter(this,getParam2(f->idff->order),10,f->idff,tr)); 	 
     std::sort(streams.begin(),streams.end(),streamsSort);
    }
   for (const TfilterIDFF *f=getNextFilterIDFF();f && f->name;f++)
    if (f->show==0 || getParam2(f->show))
	    // Group 1 or 2 should not be used : for audio (1) and subtitle (2) streams
		streams.push_back(new TstreamFilter(this,f->order?getParam2(f->order):1000,f->order?10:20,f,tr));
  }
 tr->release();
 addOwnStreams();
 *pcStreams=(DWORD)streams.size();
 return S_OK;
}
STDMETHODIMP TffdshowDec::Info(long lIndex, AM_MEDIA_TYPE** ppmt, DWORD* pdwFlags, LCID* plcid, DWORD* pdwGroup, WCHAR** ppszName, IUnknown** ppObject, IUnknown** ppUnk)
{
 if (lIndex<0 || lIndex>=(long)streams.size() || !presetSettings) return E_INVALIDARG;
 if (ppmt) *ppmt=getInputMediaType(lIndex);
 if (pdwFlags)
  *pdwFlags=streams[lIndex]->getFlags();
 if (plcid) *plcid=0;
 if (pdwGroup) *pdwGroup=streams[lIndex]->group;
 if (ppszName)
  {
   ffstring name=streams[lIndex]->getName();//stringreplace(ffstring(streams[lIndex]->getName()),"&","&&",rfReplaceAll);
   size_t wlen=(name.size()+1)*sizeof(WCHAR);
   *ppszName=(WCHAR*)CoTaskMemAlloc(wlen);memset(*ppszName,0,wlen);
   nCopyAnsiToWideChar(*ppszName,name.c_str());
  }
 if (ppObject) *ppObject=NULL;
 if (ppUnk) *ppUnk=NULL;
 return S_OK;
}
STDMETHODIMP TffdshowDec::Enable(long lIndex, DWORD dwFlags)
{
 if (firsttransform) return S_OK;
 if (/*!(dwFlags&AMSTREAMSELECTENABLE_ENABLE)*/dwFlags!=AMSTREAMSELECTENABLE_ENABLE) return E_NOTIMPL;
 if (lIndex<0 || lIndex>=(long)streams.size()) return E_INVALIDARG;
 if (streams[lIndex]->action())
  {
   saveGlobalSettings();
   saveKeysSettings();
   saveRemoteSettings();
   saveActivePreset(NULL);
  }
 return S_OK;
}

TffdshowDec::TstreamFilter::TstreamFilter(TffdshowDec *Iself,int Iorder,int Igroup,const TfilterIDFF *If,Ttranslate *Itr):Tstream(Iself,Iorder,Igroup),f(If)
{
 tr=Itr;
 tr->addref();
}
TffdshowDec::TstreamFilter::~TstreamFilter()
{
 tr->release();
}
DWORD TffdshowDec::TstreamFilter::getFlags(void)
{
 return ((f->is==IDFF_isKeys && !self->keys) || self->getParam2(f->is)==0)?0:AMSTREAMSELECTINFO_ENABLED;
}
const char_t* TffdshowDec::TstreamFilter::getName(void)
{
 const char_t *ret=tr->translate(NULL,f->dlgId,0,f->name);
 if (strcmp(ret,_l("Subtitles"))==0)
  return _l("Subtitles ");
 else
  return ret;
}
bool TffdshowDec::TstreamFilter::action(void)
{
 self->invParam(f->is);
 return true;
}

DWORD TffdshowDec::TstreamPreset::getFlags(void)
{
 return stricmp(preset,self->presetSettings->presetName)==0?AMSTREAMSELECTINFO_ENABLED|AMSTREAMSELECTINFO_EXCLUSIVE:0;
}
const char_t* TffdshowDec::TstreamPreset::getName(void)
{
 return preset;
}
bool TffdshowDec::TstreamPreset::action(void)
{
 self->setActivePreset(preset,0);
 return true;
}

STDMETHODIMP TffdshowDec::getShortDescription(char_t *buf,int buflen)
{
 if (!buf) return E_POINTER;
 const char_t *activepresetname=getActivePresetName2();
 if (!activepresetname)
  {
   buf[0]='\0';
   return E_UNEXPECTED;
  }
 int len = tsnprintf_s(buf, buflen, _TRUNCATE, _l("ffdshow %s: "),FFDSHOW_VER);
 buf+=len;buflen-=len;
 const TfilterIDFFs *filters;getFilterIDFFs(activepresetname,&filters);
 for (TfilterIDFFs::const_iterator f=filters->begin();f!=filters->end() && buflen>0;f++)
  if (f->idff->is && getParam2(f->idff->is))
   {
    len=tsnprintf_s(buf, buflen, _TRUNCATE, _l("%s "),f->idff->name);
    buf+=len;buflen-=len;
   }
 for (const TfilterIDFF *f=getNextFilterIDFF();f && f->name && buflen>0;f++)
  if (f->is && getParam2(f->is))
   {
    len=tsnprintf_s(buf, buflen, _TRUNCATE,_l("%s "),f->name);
    buf+=len;buflen-=len;
   }
 buf[-1]='\0';
 return true;
}

STDMETHODIMP TffdshowDec::createPresetPages(const char_t *presetname,TffdshowPageDec *pages)
{
 if (!presetname) return S_FALSE;
 Tpreset *preset=presets->getPreset(presetname,false);
 if (preset)
  {
   preset->createPages(pages);
   return S_OK;
  }
 else
  return S_FALSE;
}

STDMETHODIMP TffdshowDec::getFilterIDFFs(const char_t *presetname,const TfilterIDFFs* *filters)
{
 if (!filters) return E_POINTER;
 Tpreset *preset=presets->getPreset(presetname,false);
 if (preset)
  {
   *filters=preset->getFilters();
   return S_OK;
  }
 else
  return S_FALSE;
}

HRESULT TffdshowDec::NewSegment(REFERENCE_TIME tStart,REFERENCE_TIME tStop,double dRate)
{
 if ((tStop&0xfffffffffffffffLL)==0xfffffffffffffffLL)
  moviesecs=1; //tStop is most probably wrong, but don't risk getDuration
 else
  moviesecs=int(tStop/REF_SECOND_MULT);
 if (filters) filters->onSeek();
 m_dirtyStop = false;
 return CTransformFilter::NewSegment(tStart,tStop,dRate);
}

STDMETHODIMP TffdshowDec::GetPages(CAUUID *pPages)
{
 DPRINTF(_l("TffdshowDec::GetPages"));
 initDialog();
 onTrayIconChange(0,0);

 pPages->cElems=1;
 pPages->pElems=(GUID*)CoTaskMemAlloc(pPages->cElems*sizeof(GUID));
 if (pPages->pElems==NULL) return E_OUTOFMEMORY;
 pPages->pElems[0]=proppageid;
 return S_OK;
}

STDMETHODIMP_(unsigned int) TffdshowDec::getPresetAutoloadItemsCount2(void)
{
 return presetSettings?(unsigned int)presetSettings->getAutoPresetItemsCount():0;
}
STDMETHODIMP TffdshowDec::getPresetAutoloadItemInfo(unsigned int index,const char_t* *name,const char_t* *hint,int *allowWildcard,int *is,int *isVal,char_t *val,size_t vallen,int *isList,int *isHelp)
{
 if (!presetSettings) return E_UNEXPECTED;
 if (!name || !allowWildcard || !is || !val || !vallen) return E_POINTER;
 if (index>=presetSettings->getAutoPresetItemsCount()) return E_INVALIDARG;
 presetSettings->getAutoPresetItemInfo(index,name,hint,allowWildcard,is,isVal,val,vallen,isList,isHelp);
 return S_OK;
}
STDMETHODIMP TffdshowDec::setPresetAutoloadItem(unsigned int index,int is,const char_t *val)
{
 if (!presetSettings) return E_UNEXPECTED;
 if (!val) return E_POINTER;
 if (index>=presetSettings->getAutoPresetItemsCount()) return E_INVALIDARG;
 presetSettings->setAutoPresetItem(index,is,val);
 return S_OK;
}
STDMETHODIMP TffdshowDec::getPresetAutoloadItemHelp(unsigned int index,const char_t* *helpPtr)
{
 if (!presetSettings) return E_UNEXPECTED;
 if (!helpPtr) return E_POINTER;
 if (index>=presetSettings->getAutoPresetItemsCount()) return E_INVALIDARG;
 presetSettings->getAutoPresetItemHelp(index,helpPtr);
 return *helpPtr?S_OK:S_FALSE;
}

STDMETHODIMP_(const char_t*) TffdshowDec::getPresetAutoloadItemList(unsigned int paramIndex,unsigned int listIndex)
{
 return presetSettings && paramIndex<presetSettings->getAutoPresetItemsCount()?presetSettings->getAutoPresetItemList(this,paramIndex,listIndex):NULL;
}
STDMETHODIMP_(const char_t**) TffdshowDec::getSupportedFOURCCs(void)
{
 return globalSettings->getFOURCClist();
}
STDMETHODIMP_(const Tstrptrs*) TffdshowDec::getCodecsList(void)
{
 if (codecs.empty())
  globalSettings->getCodecsList(codecs);
 return &codecs;
}

STDMETHODIMP TffdshowDec::queryFilterInterface(const IID &iid,void **ptr)
{
 if (!ptr) return E_POINTER;
 if (!filters) return E_UNEXPECTED;
 lock(IDFF_lockPresetPtr);
 HRESULT res=filters->queryFilterInterface(iid,ptr);
 unlock(IDFF_lockPresetPtr);
 return res;
}
STDMETHODIMP TffdshowDec::setOnNewFiltersMsg(HWND wnd,unsigned int msg)
{
 onNewFiltersWnd=wnd;onNewFiltersMsg=msg;
 return S_OK;
}
STDMETHODIMP TffdshowDec::sendOnNewFiltersMsg(void)
{
 if (onNewFiltersMsg)
  {
   PostMessage(onNewFiltersWnd,onNewFiltersMsg,0,0);
   return S_OK;
  }
 else
  return S_FALSE;
}

STDMETHODIMP_(int) TffdshowDec::getOSDpresetCount2(void)
{
 return (int)globalSettings->osd->getSize();
}
STDMETHODIMP_(const char_t*) TffdshowDec::getOSDpresetName2(unsigned int i)
{
 return globalSettings->osd->getPresetName(i);
}
STDMETHODIMP_(const char_t*) TffdshowDec::getOSDpresetFormat2(const char_t *presetName)
{
 return globalSettings->osd->getFormat(presetName);
}
STDMETHODIMP TffdshowDec::setOSDpresetFormat(const char_t *presetName,const char_t *format)
{
 return globalSettings->osd->setFormat(presetName,format)?S_OK:S_FALSE;
}
STDMETHODIMP TffdshowDec::setOSDpresetName(unsigned int i,const char_t *name)
{
 return globalSettings->osd->setPresetName(i,name);
}
STDMETHODIMP TffdshowDec::addOSDpreset(const char_t *presetName,const char_t *format)
{
 if (!presetName || presetName[0]=='\0' || !format) return E_INVALIDARG;
 globalSettings->osd->addPreset(presetName,format);
 return S_OK;
}
STDMETHODIMP TffdshowDec::deleteOSDpreset(const char_t *presetName)
{
 return globalSettings->osd->erase(presetName)?S_OK:S_FALSE;
}
STDMETHODIMP TffdshowDec::cycleOSDpresets(void)
{
 return globalSettings->osd->cycle()?S_OK:S_FALSE;
}
STDMETHODIMP_(const char_t*) TffdshowDec::getOSDstartupFormat(int *duration)
{
 return globalSettings->osd->getStartupFormat(duration);
}

STDMETHODIMP_(TinputPin*) TffdshowDec::getInputPin()
{
 return minput;
}
