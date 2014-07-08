/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecVideo.h"
#include "TpresetSettingsVideo.h"
#include "ToutputVideoSettings.h"
#include "TglobalSettings.h"
#include "TimgFilters.h"
#include "TimgFilter.h"
#include "TffPict.h"
#include "TimgFilterOSD.h"
#include "TimgFilterSubtitles.h"
#include "TimgFilterExpand.h"
#include "TimgFilterHWoverlay.h"
#include "IimgFilterGrab.h"
#include "TimgFilterResize.h"
#include "TimgFilterOutput.h"
#include "TresizeAspectSettings.h"

//================================================= TimgFilters =================================================
TimgFilters::TimgFilters(IffdshowBase *Ideci,IprocVideoSink *Isink):
 Tfilters(Ideci),
 sink(Isink),
 deciV(Ideci),
 dirtyBorder(2),
 firstprocess(true),
 subtitleResetTime(0),
 grab(NULL),
 stopAtSubtitles(false)
{
 subtitles=getFilter<TimgFilterSubtitles>();
 getFilter<TimgFilterHWoverlay>();
 output=getFilter<TimgFilterOutput>();
}
TimgFilters::~TimgFilters()
{
}

HRESULT TimgFilters::process(TffPict &pict,const TpresetVideo *cfg)
{
 deciV->lockCSReceive();

 csp_vflip(pict.csp,&pict.cspInfo,pict.data,pict.stride,pict.rectFull.dy);
 if (firstprocess)
  {
   firstprocess=false;
   onFirstProcess(cfg);
  }

 deci->lock(IDFF_lockPresetPtr);
 if (queueChanged)
  {
   makeQueue(cfg,queue);
   if (queryFilterInterface(IID_IimgFilterGrab,(void**)&grab)!=S_OK)
    grab=NULL;
   InterlockedDecrement((LONG*)&queueChanged);
   deciD->sendOnNewFiltersMsg();
  }
 queue.copyCfg(cfg);
 deci->unlock(IDFF_lockPresetPtr);

 if (cfg->isDyInterlaced && pict.rectFull.dy>(unsigned int)cfg->dyInterlaced)
  pict.csp|=FF_CSP_FLAGS_INTERLACED;

 HRESULT ret=deliverSample(queue.begin(),pict);

 deciV->unlockCSReceive();

 return ret;
 
}

HRESULT TimgFilters::deliverSample(TfilterQueue::iterator it,TffPict &pict)
{
 if (it==queue.end())
  {
   if (dirtyBorder==1)
    {
     //static int cnt=0;DEBUGS1("clearBorder",cnt++);
     int brightness = deciV->getBordersBrightness();
     pict.clearBorder(brightness, deciV->getToutputVideoSettings()->brightness2luma(brightness));
     dirtyBorder=2;
    }
   return sink->deliverProcessedSample(pict);
  }
 else
  {
   TimgFilter *filter=(TimgFilter*)it->filter;
   const TfilterSettingsVideo *cfg=(const TfilterSettingsVideo*)it->getCfg();
   if (pict.csp!=FF_CSP_NULL)
    return filter->process(it,pict,cfg);
   else
    return filter->flush(it,pict,cfg);
  }
}

// draw DVD subtitles and menu before resize, if it is not done.
void TimgFilters::adhocDVDsub(TfilterQueue::iterator it0,TffPict &pict)
{
 TfilterQueue::iterator it = it0;
 while (it!=queue.end())
  {
   TimgFilter *filter=(TimgFilter*)it->filter;
   if (filter->getImgFilterID() == TimgFilter::IMGFILTER_SUBTITLES)
    {
     const TfilterSettingsVideo *cfg=(const TfilterSettingsVideo*)it->getCfg();
     if (((TimgFilterSubtitles*)filter)->enterAdhocMode())
      filter->process(it0,pict,cfg);
     break;
    }
   it++;
  }
}

void TimgFilters::onEndOfStream(void)
{
 for (iterator f=begin();f!=end();f++)
  {
   TimgFilter *filter = (TimgFilter*)f->second;
   filter->onEndOfStream();
  }
}

bool TimgFilters::pullImageFromSubtitlesFilter(TfilterQueue::iterator from)
{
 bool result = false;
 for (TfilterQueue::iterator f = queue.begin() ; f != from ; f++)
  {
   TimgFilter *filter = (TimgFilter*)f->filter;
   result |= filter->onPullImageFromSubtitlesFilter();
  }
 return result;
}

bool TimgFilters::isAnyActiveDownstreamFilter(TfilterQueue::iterator it)
{
 it++;
 while (it!=queue.end())
  {
   TimgFilter *filter=(TimgFilter*)it->filter;
   const TfilterSettingsVideo *cfg=(const TfilterSettingsVideo*)it->getCfg();
   if (cfg->is)
    {
     if (!filter->acceptRandomYV12andRGB32())
      return true;
    }
   it++;
  }
 return false;
}

HRESULT TimgFilters::convertOutputSample(const TffPict &pict,int dstcsp,unsigned char *dst[4],int dstStride[4],LONG &dstSize,const ToutputVideoSettings *cfg)
{
 return output->process(pict,dstcsp,dst,dstStride,dstSize,cfg);
}

void TimgFilters::getOutputFmt(TffPictBase &pict,const TpresetVideo *cfg)
{
 TfilterQueue queue(true);
 makeQueue(cfg,queue);
 queue.copyCfg(cfg);
 for (TfilterQueue::iterator f=queue.begin();f!=queue.end();f++)
  ((TimgFilter*)f->filter)->getOutputFmt(pict,(const TfilterSettingsVideo*)f->getCfg());
}

bool TimgFilters::initSubtitles(int id,int type,const unsigned char *extradata,unsigned int extradatalen)
{
 return subtitles->initSubtitles(id,type,extradata,extradatalen);
}
void TimgFilters::addSubtitle(int id,REFERENCE_TIME start,REFERENCE_TIME stop,const unsigned char *data,unsigned int datalen,const TsubtitlesSettings *cfg,bool utf8)
{
 subtitles->addSubtitle(id,start,stop,data,datalen,cfg,utf8);
}
void TimgFilters::resetSubtitles(int id)
{
 subtitles->resetSubtitles(id);
}
bool TimgFilters::ctlSubtitles(int id,int type,unsigned int ctl_id,const void *ctl_data,unsigned int ctl_datalen)
{
 return subtitles->ctlSubtitles(id,type,ctl_id,ctl_data,ctl_datalen);
}

const char_t* TimgFilters::getCurrentSubFlnm(void) const
{
 return subtitles->getCurrentFlnm();
}

void TimgFilters::grabNow(void)
{
 if (grab) grab->grabNow();
}

//============================================== TimgFiltersPlayer ==============================================
TimgFiltersPlayer::TimgFiltersPlayer(IffdshowBase *Ideci,IprocVideoSink *Isink,const TfontSettingsOSD &IglobalFontSettingsOSD,bool &IallowOutChange):
 TimgFilters(Ideci,Isink),
 resizeSettingsDV(NULL),resizeDV(NULL),
 expand(NULL),
 globalFontSettingsOSD(IglobalFontSettingsOSD),fontSettingsOSD((TfontSettingsOSD*)calloc(1,sizeof(TfontSettingsOSD))),
 allowOutChange(IallowOutChange)
{
 deci->getGlobalSettings((TglobalSettingsBase**)&globalCfg);
 osd=getFilter<TimgFilterOSD>();
}
TimgFiltersPlayer::~TimgFiltersPlayer()
{
 if (resizeSettingsDV) delete resizeSettingsDV;
 if (resizeDV) delete resizeDV;
 free(fontSettingsOSD);
}

void TimgFiltersPlayer::makeQueue(const Tpreset *cfg,TfilterQueue &queue)
{
 TimgFilters::makeQueue(cfg,queue);
 if (expand) queue.add(expand,&expandSettings);
 if (resizeDV) queue.add(resizeDV,resizeSettingsDV);
 queue.add(osd,&globalCfg->osd);
}

void TimgFiltersPlayer::onFirstProcess(const TpresetVideo *cfg)
{
 if (cfg->output->dv)
  {
   resizeSettingsDV=new TresizeAspectSettings(new TintStrColl);
   resizeSettingsDV->reset();
   resizeSettingsDV->is=1;resizeSettingsDV->show=1;resizeSettingsDV->full=1;
   deciV->getOutputDimensions(&resizeSettingsDV->dx,&resizeSettingsDV->dy);
   cfg->output->getDVsize(&resizeSettingsDV->dx,&resizeSettingsDV->dy);
   resizeDV=new TimgFilterResize(deci,this);
  }
 else if (!allowOutChange && !isdvdproc)
  {
   expandSettings.full=true;
   deciV->getOutputDimensions(&expandSettings.newrect.dx,&expandSettings.newrect.dy);
   expand=getFilter<TimgFilterExpand>();
  }
}

void TimgFiltersPlayer::getOutputFmt(TffPictBase &pict,const TpresetVideo *cfg)
{
 TimgFilters::getOutputFmt(pict,cfg);
 if (cfg->output->dv)
  cfg->output->getDVsize(&pict.rectFull.dx,&pict.rectFull.dy);
}

bool TimgFiltersPlayer::shortOSDmessage(const char_t *msg,unsigned int duration)
{
 return osd->shortOSDmessage(msg,duration);
}
HRESULT TimgFiltersPlayer::registerOSDprovider(IOSDprovider *provider,const char *name)
{
 return osd->registerOSDprovider(provider,name);
}
HRESULT TimgFiltersPlayer::unregisterOSDprovider(IOSDprovider *provider)
{
 return osd->unregisterOSDprovider(provider);
}

HRESULT TimgFiltersPlayer::process(TffPict &pict,const TpresetVideo *cfg)
{
 memcpy(fontSettingsOSD,&globalFontSettingsOSD,sizeof(*fontSettingsOSD));
 return TimgFilters::process(pict,cfg);
}

HRESULT TimgFiltersPlayer::addClosedCaption(const wchar_t *line)
{
 subtitles->addClosedCaption(line);
 return S_OK;
}
HRESULT TimgFiltersPlayer::hideClosedCaptions(void)
{
 subtitles->hideClosedCaptions();
 return S_OK;
}
