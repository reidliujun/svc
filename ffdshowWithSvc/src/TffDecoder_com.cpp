/*
 * Copyright (c) 2002-2006 Milan Cutka
 * based on CXvidDecoder.cpp from Xvid DirectShow filter
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
#include "TffDecoderVideo.h"
#include "TpresetSettingsVideo.h"
#include "TglobalSettings.h"
#include "TvideoCodec.h"
#include "TkeyboardDirect.h"
#include "Ttranslate.h"
#include "ffdebug.h"
#include "Tlibmplayer.h"
#include "TimgFilters.h"
#include "TfontManager.h"
#include "Tpresets.h"
#include "TtextInputPin.h"
#include "TffdshowVideoInputPin.h"
#include "TffdshowDecVideoOutputPin.h"
#include "resource.h"
#include "Tinfo.h"
#include "TlevelsSettings.h"
#include "ToutputVideoSettings.h"
#include "imgFilters/avisynth/TimgFilterAvisynth.h"

const TfilterIDFF TffdshowDecVideo::nextFilters[]=
{
 {
  /*name*/  _l("Flip"),
  /*id*/    0,
  /*is*/    IDFF_flip,
  /*order*/ 0,
  /*show*/  0,
  /*full*/  0,
  /*half*/  0,
  /*dlgId*/ IDD_FLIP,
 },
 {
  /*name*/  _l("OSD"),
  /*id*/    0,
  /*is*/    IDFF_isOSD,
  /*order*/ 0,
  /*show*/  0,
  /*full*/  0,
  /*half*/  0,
  /*dlgId*/ IDD_OSD,
 },
 {
  /*name*/  _l("Keyboard & remote"),
  /*id*/    0,
  /*is*/    IDFF_isKeys,
  /*order*/ 0,
  /*show*/  0,
  /*full*/  0,
  /*half*/  0,
  /*dlgId*/ IDD_KEYS,
 },
#if 0
 {
  /*name*/  _l("Mouse"),
  /*id*/    0,
  /*is*/    IDFF_isMouse,
  /*order*/ 0,
  /*show*/  0,
  /*full*/  0,
  /*half*/  0,
  /*dlgId*/ IDD_MOUSE,
 },
#endif
 NULL,0
};

STDMETHODIMP_(int) TffdshowDecVideo::getVersion2(void)
{
 return VERSION;
}
STDMETHODIMP TffdshowDecVideo::compat_getIffDecoderVersion2(void)
{
 return COMPAT_VERSION;
}

// create instance
CUnknown* WINAPI TffdshowDecVideo::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
#ifdef DEBUG
 DbgSetModuleLevel(0xffffffff,0);
#endif
 TffdshowDecVideo *pNewObject=new TffdshowDecVideo(CLSID_FFDSHOW,NAME("TffDecoder"),CLSID_TFFDSHOWPAGE,IDS_FFDSHOWDECVIDEO,IDI_FFDSHOW,punk,phr,IDFF_FILTERMODE_VIDEO|IDFF_FILTERMODE_PLAYER,defaultMerit,new TintStrColl);
 if (pNewObject==NULL) *phr=E_OUTOFMEMORY;
 return pNewObject;
}

CUnknown* WINAPI TffdshowDecVideoRaw::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffdshowDecVideoRaw *pNewObject=new TffdshowDecVideoRaw(punk,phr);
 if (pNewObject==NULL) *phr=E_OUTOFMEMORY;
 return pNewObject;
}

CUnknown* WINAPI TffdshowDecVideoSubtitles::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffdshowDecVideoSubtitles *pNewObject=new TffdshowDecVideoSubtitles(punk,phr);
 if (pNewObject==NULL) *phr=E_OUTOFMEMORY;
 return pNewObject;
}

template<> interfaces<char_t>::IffdshowDecVideo* TffdshowDecVideo::getDecVideoInterface(void)
{
 return this;
}
template<> interfaces<tchar_traits<char_t>::other_char_t>::IffdshowDecVideo* TffdshowDecVideo::getDecVideoInterface(void)
{
 return &decVideo_char;
}

// query interfaces
STDMETHODIMP TffdshowDecVideo::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 CheckPointer(ppv,E_POINTER);
 if (riid==IID_IffdshowDecVideoA)
  return GetInterface<IffdshowDecVideoA>(getDecVideoInterface<IffdshowDecVideoA>(),ppv);
 else if (riid==IID_IffdshowDecVideoW)
  return GetInterface<IffdshowDecVideoW>(getDecVideoInterface<IffdshowDecVideoW>(),ppv);
 else if (riid==IID_IffDecoder)
  return GetInterface<IffDecoder>(this,ppv);
/*
 else if (riid==IID_IAMExtendedSeeking)
  return GetInterface((IAMExtendedSeeking*)this,ppv);*/
 else
  return TffdshowDec::NonDelegatingQueryInterface(riid,ppv);
}

STDMETHODIMP TffdshowDecVideo::setPresetPtr(Tpreset *preset)
{
 HRESULT res=TffdshowDec::setPresetPtr(preset);
 if (res==S_OK && presetSettings->postproc) currentq=presetSettings->postproc->qual;
 return S_OK;
}
STDMETHODIMP TffdshowDecVideo::getOutputFourcc(char_t *buf,size_t len)
{
 if (!buf) return E_POINTER;
 if (outdv)
  strcpy(buf,_l("DV"));
 else if (m_frame.dstColorspace==0)
  buf[0]='\0';
 else
  csp_getName(m_frame.dstColorspace,buf,len);
 return S_OK;
}
STDMETHODIMP TffdshowDecVideo::getAVIfps(unsigned int *fps1000)
{
 HRESULT hr = inpin->getAVIfps(fps1000);
 if (hr != S_OK) return hr;

 if (fps1000
     // After seeking, a few frames may have incorrect timestamps.
     && count_decoded_frames_for_framerate_calculation > 9)
  {
   int pos_current_even = (count_decoded_frames_for_framerate_calculation - 1) & ~1;
   REFERENCE_TIME avg = (decoded_rtStarts[pos_current_even & 3] - eighth_decoded_rtStart) / (pos_current_even - 7);
   if (avg > 0)
    {
     unsigned int fps1000_tmp = (unsigned int)(REF_SECOND_MULT*1000/avg);
     if (ff_abs((int)fps1000_tmp - (int)*fps1000) > 300) // Some splitters (matroska, flv) round the timestamps to milli-seconds. Thus there is some error in this calculation. Container's averave duration may be more reliable in this case.
      *fps1000 = fps1000_tmp;
     return S_OK;
    }
  }

 return S_OK;
}
STDMETHODIMP_(int) TffdshowDecVideo::getAVIfps1000_2(void)
{
 return inpin->getAVIfps1000_2();
}

STDMETHODIMP TffdshowDecVideo::loadActivePresetFromFile(const char_t *flnm)
{
 HRESULT res=TffdshowDec::loadActivePresetFromFile(flnm);
 if (res==S_OK) currentq=presetSettings->postproc->qual;
 return res;
}
STDMETHODIMP TffdshowDecVideo::getAVIdimensions(unsigned int *x,unsigned int *y)
{
 return inpin->getAVIdimensions(x,y);
}
STDMETHODIMP TffdshowDecVideo::getOutputDimensions(unsigned int *x,unsigned int *y)
{
 return calcNewSize(inpin->pictIn.rectFull/*.dx,inpin->pictIn.rectFull.dy*/,x,y);
}
STDMETHODIMP_(FOURCC) TffdshowDecVideo::getMovieFOURCC(void)
{
 return inpin->getMovieFOURCC();
}
STDMETHODIMP TffdshowDecVideo::getInputSAR(unsigned int *a1,unsigned int *a2)
{
 return inpin->getInputSAR(a1,a2);
}
STDMETHODIMP TffdshowDecVideo::getInputDAR(unsigned int *a1,unsigned int *a2)
{
 return inpin->getInputDAR(a1,a2);
}


STDMETHODIMP TffdshowDecVideo::getPPmode(unsigned int *ppmode)
{
 *ppmode=Tlibmplayer::getPPmode(presetSettings->postproc,currentq);
 return S_OK;
}
void TffdshowDecVideo::sendOnChange(int paramID,int val)
{
 if (paramID!=IDFF_currentq)
  TffdshowDec::sendOnChange(paramID,val);
}
STDMETHODIMP TffdshowDecVideo::getMovieSource(const TvideoCodecDec* *moviePtr)
{
 return inpin->getMovieSource(moviePtr);
}
STDMETHODIMP_(int) TffdshowDecVideo::getInputBitrate2(void)
{
 if (inpin->avgTimePerFrame>0 && frameCnt>0)
  {
   unsigned int Bps=(unsigned int)(bytesCnt/(frameCnt/(10000000.0/inpin->avgTimePerFrame)));
   return 8*Bps/1000;
  }
 else return 0;
}

TimgFilters* TffdshowDecVideo::createImgFilters(void)
{
 return new TimgFiltersPlayer(this,this,globalSettings->osd.font,allowOutChange);
}

STDMETHODIMP TffdshowDecVideo::calcNewSize(unsigned int inDx,unsigned int inDy ,unsigned int *outDx,unsigned int *outDy)
{
 if (!presetSettings) return E_UNEXPECTED;
 if (!outDx || !outDy) return E_POINTER;
 if (!imgFilters) imgFilters=createImgFilters();
 TffPictBase p(inDx,inDy);
 calcNewSize(p);
 *outDx=p.rectFull.dx;*outDy=p.rectFull.dy;
 return S_OK;
}
STDMETHODIMP TffdshowDecVideo::calcNewSize(Trect inRect,unsigned int *outDx,unsigned int *outDy)
{
 if (!presetSettings) return E_UNEXPECTED;
 if (!outDx || !outDy) return E_POINTER;
 if (!imgFilters) imgFilters=createImgFilters();
 TffPictBase p(inRect.dx,inRect.dy);
 p.setSar(inRect.sar);
 calcNewSize(p);
 *outDx=p.rectFull.dx;*outDy=p.rectFull.dy;
 return S_OK;
}
void TffdshowDecVideo::calcNewSize(TffPictBase &p)
{
 if (!presetSettings || (p.rectFull.dx==0 && p.rectFull.dy==0)) return;
 if (!imgFilters) imgFilters=createImgFilters();
 imgFilters->getOutputFmt(p,presetSettings);
}

STDMETHODIMP TffdshowDecVideo::getFrameTime(unsigned int framenum,unsigned int *sec)
{
 return inpin->getFrameTime(framenum,sec);
}
STDMETHODIMP TffdshowDecVideo::getFrameTimeMS(unsigned int framenum,unsigned int *msec)
{
 return inpin->getFrameTimeMS(framenum,msec);
}

STDMETHODIMP TffdshowDecVideo::getRemainingFrameTime(unsigned int *sec)
{
 if (!sec) return E_POINTER;
 unsigned int val;
 HRESULT hr;
 if (FAILED(hr=getCurrentFrameTime(&val)))
  return hr;
 *sec=std::max(0,moviesecs-(int)val);
 return S_OK;
}

STDMETHODIMP TffdshowDecVideo::grabNow(void)
{
 return imgFilters?(imgFilters->grabNow(),S_OK):E_UNEXPECTED;
}

STDMETHODIMP TffdshowDecVideo::drawOSD(int px,int py,const char_t *text)
{
 if ((globalSettings->filtermode&IDFF_FILTERMODE_PLAYER)==0) return E_UNEXPECTED;
 if (px<0 || px>100 || py<0 || py>100) return E_INVALIDARG;
 if (!text) return E_POINTER;
 globalSettings->osd.userPx=px;globalSettings->osd.userPy=py;
 if (strlen(text)>2048)
  ff_strncpy(globalSettings->osd.user, text, countof(globalSettings->osd.user));
 else
  ff_strncpy(globalSettings->osd.user, text, countof(globalSettings->osd.user));
 return S_OK;
}
STDMETHODIMP TffdshowDecVideo::calcMeanQuant(float *quant)
{
 return inpin->calcMeanQuant(quant);
}
STDMETHODIMP TffdshowDecVideo::quantsAvailable(void)
{
 return inpin->quantsAvailable();
}
STDMETHODIMP TffdshowDecVideo::getQuantMatrices(unsigned char intra8[64],unsigned char inter8[64])
{
 return inpin->getQuantMatrices(intra8,inter8);
}

STDMETHODIMP_(const char_t*) TffdshowDecVideo::findAutoSubflnm3(void)
{
 return findAutoSubflnms(NULL);
}
STDMETHODIMP_(const char_t*) TffdshowDecVideo::findAutoSubflnms(IcheckSubtitle *checkSubtitle)
{
 const char_t *tempflnm=getParamStr2(IDFF_subTempFilename);
 if (tempflnm[0])
  return tempflnm;
 else
  return inpin->findAutoSubflnm(checkSubtitle,getParamStr2(IDFF_subSearchDir),getParamStr2(IDFF_subSearchExt),!!getParam2(IDFF_subSearchHeuristic));
}

STDMETHODIMP TffdshowDecVideo::shortOSDmessage(const char_t *msg,unsigned int duration)
{
 if (!msg || duration==0) return E_INVALIDARG;
 if (!imgFilters || !presetSettings) return E_UNEXPECTED;
 return imgFilters->shortOSDmessage(msg,duration)?S_OK:S_FALSE;
}
STDMETHODIMP TffdshowDecVideo::registerOSDprovider(IOSDprovider *provider,const char *name)
{
 if (!imgFilters) return E_UNEXPECTED;
 return imgFilters->registerOSDprovider(provider,name);
}
STDMETHODIMP TffdshowDecVideo::unregisterOSDprovider(IOSDprovider *provider)
{
 if (!imgFilters) return E_UNEXPECTED;
 return imgFilters->unregisterOSDprovider(provider);
}

STDMETHODIMP TffdshowDecVideo::resetSubtitleTimes(void)
{
 wasSubtitleResetTime=true;
 return S_OK;
}

STDMETHODIMP TffdshowDecVideo::resetOSD(void)
{
 globalSettings->osd.resetLook();
 return S_OK;
}

STDMETHODIMP_(int) TffdshowDecVideo::getCodecId(const BITMAPINFOHEADER *hdr,const GUID *subtype,FOURCC *AVIfourcc)
{
 return globalSettings->getCodecId(hdr2fourcc(hdr,subtype),AVIfourcc);
}

int TffdshowDecVideo::getVideoCodecId(const BITMAPINFOHEADER *hdr,const GUID *subtype,FOURCC *AVIfourcc)
{
 return getCodecId(hdr,subtype,AVIfourcc);
}

STDMETHODIMP TffdshowDecVideo::getFontManager(TfontManager* *fontManagerPtr)
{
 if (!fontManagerPtr) return E_POINTER;
 if (!fontManager) fontManager=new TfontManager;
 *fontManagerPtr=fontManager;
 return S_OK;
}

STDMETHODIMP TffdshowDecVideo::getInCodecString(char_t *buf,size_t buflen)
{
 return inpin->getInCodecString(buf,buflen);
}
STDMETHODIMP TffdshowDecVideo::getOutCodecString(char_t *buf,size_t buflen)
{
 return getOutputFourcc(buf,buflen);
}

STDMETHODIMP TffdshowDecVideo::getVideoWindowPos(int *left,int *top,unsigned int *width,unsigned int *height)
{
 if (!videoWindow) return E_FAIL;
 long left1,top1,width1,height1;
 HRESULT hr;
 if (FAILED(hr=videoWindow->GetWindowPosition(&left1,&top1,&width1,&height1)))
  return hr;
 if (left  ) *left  =left1  ;
 if (top   ) *top   =top1   ;
 if (width ) *width =width1 ;
 if (height) *height=height1;
 return S_OK;
}
STDMETHODIMP TffdshowDecVideo::getVideoDestRect(RECT *r)
{
 if (!basicVideo) return E_FAIL;
 if (!r) return E_POINTER;
 HRESULT hr;
 long x,y,dx,dy;
 if (FAILED(hr=basicVideo->GetDestinationPosition(&x,&y,&dx,&dy)))
  return hr;
 *r=CRect(CPoint(x,y),CSize(dx,dy));
 return S_OK;
}

STDMETHODIMP TffdshowDecVideo::fillSubtitleLanguages(const char_t **langs)
{
 lock(IDFF_lockSublangs);
 subtitleLanguages.clear();
 while (*langs)
  {
   subtitleLanguages.push_back(*langs);
   langs++;
  }
 unlock(IDFF_lockSublangs);
 return S_OK;
}
STDMETHODIMP_(unsigned int) TffdshowDecVideo::getSubtitleLanguagesCount2(void)
{
 lock(IDFF_lockSublangs);
 int num=(unsigned int)subtitleLanguages.size();
 unlock(IDFF_lockSublangs);
 return num;
}
STDMETHODIMP TffdshowDecVideo::getSubtitleLanguageDesc(unsigned int num,const char_t* *descPtr)
{
 if (!descPtr) return E_POINTER;
 if (num>=subtitleLanguages.size()) return E_INVALIDARG;
 lock(IDFF_lockSublangs);
 *descPtr=subtitleLanguages[num].c_str();
 unlock(IDFF_lockSublangs);
 return S_OK;
}
STDMETHODIMP TffdshowDecVideo::cycleSubLanguages(int step)
{
 if (subtitleLanguages.size()<2) return S_FALSE;
 lock(IDFF_lockSublangs);
 strings::const_iterator curlang=subtitleLanguages.begin()+getParam2(IDFF_subCurLang);
 for (int i=0;i!=step;)
  {
   if (sign(step)==-1 && curlang==subtitleLanguages.begin())
    curlang=subtitleLanguages.end()-1;
   else if (sign(step)==1 && curlang==subtitleLanguages.end()-1)
    curlang=subtitleLanguages.begin();
   else
    curlang+=sign(step);
   if (*curlang!=_l(""))
    i+=sign(step);
  }
 putParam(IDFF_subCurLang,int(curlang-subtitleLanguages.begin()));
 unlock(IDFF_lockSublangs);
 return S_OK;
}

bool TffdshowDecVideo::initSubtitles(int id,int type,const unsigned char *extradata,unsigned int extradatalen)
{
 return imgFilters?imgFilters->initSubtitles(id,type,extradata,extradatalen):false;
}
void TffdshowDecVideo::addSubtitle(int id,REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,bool utf8)
{
 if (imgFilters && presetSettings)
  imgFilters->addSubtitle(id,start,stop,data,datalen,presetSettings->subtitles,utf8);
}
void TffdshowDecVideo::resetSubtitles(int id)
{
 if (imgFilters)
  imgFilters->resetSubtitles(id);
}
bool TffdshowDecVideo::ctlSubtitles(int id,int type,unsigned int ctl_id,const void *ctl_data,unsigned int ctl_datalen)
{
 return imgFilters?imgFilters->ctlSubtitles(id,type,ctl_id,ctl_data,ctl_datalen):false;
}
STDMETHODIMP_(const char_t*) TffdshowDecVideo::getCurrentSubFlnm(void)
{
 return imgFilters?imgFilters->getCurrentSubFlnm():_l("");
}

STDMETHODIMP TffdshowDecVideo::addClosedCaption(const wchar_t* line)
{
 return imgFilters?imgFilters->addClosedCaption(line):E_UNEXPECTED;
}
STDMETHODIMP TffdshowDecVideo::hideClosedCaptions(void)
{
 return imgFilters?imgFilters->hideClosedCaptions():E_UNEXPECTED;
}

STDMETHODIMP_(int) TffdshowDecVideo::getConnectedTextPinCnt(void)
{
 return textpins.getNumConnectedInpins();
}
STDMETHODIMP TffdshowDecVideo::getConnectedTextPinInfo(int i,const char_t* *name,int *id,int *found)
{
 if (!name && !found) return E_POINTER;
 if (i>=(int)textpins.getNumConnectedInpins()) return E_INVALIDARG;
 TtextInputPin *pin=textpins.getConnectedInpin(i);
 return pin->getInfo(name,id,found);
}

STDMETHODIMP TffdshowDecVideo::getLevelsMap(unsigned int map[256])
{
 if (!map) return E_POINTER;
 if (!presetSettings || !presetSettings->levels) return E_FAIL;
 int divisor;
 presetSettings->levels->calcMap(map,&divisor);
 return S_OK;
}

STDMETHODIMP TffdshowDecVideo::setAverageTimePerFrame(int64_t *avg,int useDef)
{
 if (!avg) return E_POINTER;
 if (!inpin) return E_UNEXPECTED;
 if (*avg==0 && useDef && m_pInput->IsConnected())
  *avg=((VIDEOINFOHEADER*)m_pInput->CurrentMediaType().Format())->AvgTimePerFrame;
 inpin->avgTimePerFrame=*avg;
 return S_OK;
}
STDMETHODIMP TffdshowDecVideo::getAverageTimePerFrame(int64_t *avg)
{
 if (!avg) return E_POINTER;
 if (!inpin) return E_UNEXPECTED;
 return inpin->getAverageTimePerFrame(avg);
}

STDMETHODIMP TffdshowDecVideo::getLate(int64_t *latePtr)
{
 if (!latePtr) return E_POINTER;
 *latePtr=late;
 return S_OK;
}

void TffdshowDecVideo::initCodecSettings(void)
{
 initPreset();
 outdv=!!presetSettings->output->dv;
}

STDMETHODIMP TffdshowDecVideo::getEncoderInfo(char_t *buf,size_t buflen)
{
 return inpin?inpin->getEncoderInfo(buf,buflen):E_FAIL;
}
STDMETHODIMP_(const char_t*) TffdshowDecVideo::getDecoderName(void)
{
 return inpin?inpin->getDecoderName():_l("");
}

bool TffdshowDecVideo::isStreamsMenu(void) const
{
 return !!globalSettings->streamsMenu;
}

TinfoBase* TffdshowDecVideo::createInfo(void)
{
 return new TinfoDecVideoPict(this);
}

STDMETHODIMP TffdshowDecVideo::compat_getOutputFourcc(char *buf,unsigned int len)
{
 return getDecVideoInterface<IffdshowDecVideoA>()->getOutputFourcc(buf,len);
}

STDMETHODIMP_(int) TffdshowDecVideo::getQueuedCount(void)
{
 int qCount= m_pOutputDecVideo->queue->GetCount();
 if(qCount)
  return qCount;
 if(!presetSettings->multiThread)
  return -1*IDD_QUEUEMSG_1;
 if(m_IsOldVideoRenderer)
  return -1*IDD_QUEUEMSG_5;
 if(m_IsYV12andVMR9)
  return -1*IDD_QUEUEMSG_7;
 const char_t *fileName=getExeflnm();
 if(_strnicmp(_l("wmplayer.exe"),fileName,13)==0)
  return -1*IDD_QUEUEMSG_6;
 if(!m_IsQueueListedApp)
  return -1*IDD_QUEUEMSG_3;
 if(m_IsOldVMR9RenderlessAndRGB)
  return -1*IDD_QUEUEMSG_4;
 if(downstreamID==VMR9)
  return -1*IDD_QUEUEMSG_8;
 if(!isQueue)
  return -1*IDD_QUEUEMSG_5;
 return 0;
}

STDMETHODIMP_(const char*) TffdshowDecVideo::getAviSynthInfo(void)
{
 Tfilters* filters=0;

 if (this->getImgFilters_((void**)&filters) == S_OK && filters)
  {
   TimgFilterAvisynth* avisynth=filters->getFilter<TimgFilterAvisynth>();

   if (avisynth)
    return avisynth->getInfoBuffer();
  }

 return 0;
}

STDMETHODIMP_(REFERENCE_TIME) TffdshowDecVideo::getLate(void)
{
 return late;
}

STDMETHODIMP_(bool) TffdshowDecVideo::shouldSkipH264loopFilter(void)
{
 return (presetSettings->h264skipOnDelay && late>presetSettings->h264skipDelayTime*10000);
}

STDMETHODIMP_(int) TffdshowDecVideo::get_time_on_ffdshow(void)
{
  isOSD_time_on_ffdshow= true;
  return int(OSD_time_on_ffdshowResult/10000);
}

STDMETHODIMP_(int) TffdshowDecVideo::get_time_on_ffdshow_percent(void)
{
 if(OSD_time_on_ffdshowDuration>0)
  return int(100*OSD_time_on_ffdshowResult/OSD_time_on_ffdshowDuration);
 else
  return -1;
}

STDMETHODIMP_(const char*) TffdshowDecVideo::get_current_idct(void)
{
 return inpin->get_current_idct();
}

STDMETHODIMP_(int) TffdshowDecVideo::get_downstreamID(void)
{
 return downstreamID;
}

STDMETHODIMP_(ToutputVideoSettings*) TffdshowDecVideo::getToutputVideoSettings(void)
{
 return presetSettings->output;
}

STDMETHODIMP_(int) TffdshowDecVideo::getBordersBrightness(void)
{
 return presetSettings->bordersBrightness;
}
