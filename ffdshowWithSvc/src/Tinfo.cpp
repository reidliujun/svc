/*
 * Copyright (c) 2005,2006 Milan Cutka
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
 * I modified this to support multi-thread related features, added OSD item Time table for debug.
 */

#include "stdafx.h"
#include "Tinfo.h"
#include "ffdshow_constants.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecVideo.h"
#include "IffdshowDecAudio.h"
#include "TsampleFormat.h"
#include "ffImgfmt.h"
#include "resource.h"

//============================= TinfoBase::TinfoValueBase =============================
const char_t* TinfoBase::TinfoValueBase::getVal0(bool &wasChange,bool &splitline)
{
 switch (item->type)
  {
   case IDFF_OSDtype_cpuUsage:
    {
     int usage=deci->getCpuUsage2();
     if(usage<0)
      usage=0;
     tsprintf(s,_l("%i%%"),usage);
     wasChange=true;
     return s;
    }
   case IDFF_OSDtype_systemTime:
    {
     char_t news[50];
     char_t t[20];
     tsnprintf_s(news, countof(news), _TRUNCATE, _l("%s"), _strtime(t));
     if (strcmp(news,s)!=0)
      {
       ff_strncpy(s, news, countof(s));
       wasChange=true;
      }
     return s;
    }
   case IDFF_OSDtype_movieTime:
    {
     int val=deci->getParam2(IDFF_movieDuration);
     char_t news[256];
     tsprintf(news,_l("%02i:%02i:%02i"),val/3600,(val/60)%60,val%60);
     if (strcmp(news,s)!=0)
      {
       ff_strncpy(s, news, countof(s));
       wasChange=true;
      }
     return s;
    }
   case IDFF_OSDtype_sourceFlnm:
    {
     ff_strncpy(s, deci->getSourceName(), countof(s));
     wasChange=strcmp(s,olds)!=0;
     return s;
    }
   case IDFF_OSDtype_sourceFlnmWOpath:
    {
     const char_t *flnm=deci->getSourceName();
     const char_t *pathend=strrchr(flnm,_l('\\'));
     if (!pathend) pathend=strrchr(flnm,_l('/'));
     ff_strncpy(s, pathend ? pathend + 1 : flnm, countof(s));
     wasChange=strcmp(s,olds)!=0;
     return s;
    }
   case IDFF_OSDtype_inCodecString:
    {
     deci->getInCodecString(s,countof(s));
     wasChange=strcmp(s,olds)!=0;
     return s;
    }
   case IDFF_OSDtype_outCodecString:
    {
     deci->getOutCodecString(s,countof(s));
     wasChange=strcmp(s,olds)!=0;
     return s;
    }
   case IDFF_OSDtype_exeflnm:
    {
     ff_strncpy(s, deci->getExeflnm(), countof(s));
     wasChange=strcmp(s,olds)!=0;
     return s;
    }
   default:
    {
     int val;
     HRESULT res=deci->getParam(item->type,&val);
     if (res==S_OK && (olds[0]=='\0' || val!=oldVal))
      {
       oldVal=val;
       switch (item->type)
        {
         case IDFF_decodingFps:
          tsprintf(s,_l("%5.2f"),float(val/1000.0));
          break;
         case IDFF_AVIcolorspace:
          csp_getName(val,s,countof(s));
          break;
         default:
          tsnprintf_s(s, countof(s), _TRUNCATE, _l("%i"), val);
        }
       wasChange=true;
      }
     return s;
    }
  }
}
const char_t* TinfoBase::TinfoValueBase::getVal(bool &wasChange,bool &splitline)
{
 splitline=false;
 const char_t *s=getVal0(wasChange,splitline);
 ff_strncpy(olds, s, countof(olds));
 return s;
}

//==================================== TinfoBase ======================================
TinfoBase::TinfoBase(IffdshowBase *Ideci):deci(Ideci)
{
 static const TinfoItem items[]=
  {
   IDFF_OSDtype_cpuUsage,_l("CPU load"),_l("cl"),
   IDFF_OSDtype_systemTime,_l("System time"),_l("stime"),
   IDFF_OSDtype_movieTime,_l("Movie length"),_l("mt"),
   IDFF_OSDtype_sourceFlnm,_l("Source file"),_l("sfile"),
   IDFF_OSDtype_sourceFlnmWOpath,_l("Source file name"),_l("sflnm"),
   IDFF_OSDtype_inCodecString,_l("Input description"),_l("ind"),
   IDFF_OSDtype_outCodecString,_l("Output description"),_l("outd"),
   0
  };
 addItems(items);
}

void TinfoBase::addItems(const TinfoItem *Iitems)
{
 for (;Iitems->type;Iitems++)
  values.insert(std::make_pair(Iitems->type,createNew(Iitems)));
}

bool TinfoBase::getInfo(unsigned int index,int *id,const char_t* *name)
{
 if (index>=values.size()) return false;
 Tvalues::const_iterator i=values.begin();
 for (;index;index--)
  i++;
 *id=i->second->item->type;
 *name=i->second->item->name;
 return true;
}
const char_t* TinfoBase::getName(int id)
{
 Tvalues::const_iterator v=values.find(id);
 return v==values.end()?_l(""):v->second->item->name;
}
const char_t* TinfoBase::getShortcut(int id)
{
 Tvalues::const_iterator v=values.find(id);
 return v==values.end()?_l(""):v->second->item->shortcut;
}
int TinfoBase::getInfoShortcutItem(const char_t *s,int *toklen)
{
 for (Tvalues::const_iterator v=values.begin();v!=values.end();v++)
  if (v->second->item->shortcut)
   if (_strnicmp(s,v->second->item->shortcut,strlen(v->second->item->shortcut))==0)
    {
     if (toklen) *toklen=(int)strlen(v->second->item->shortcut);
     return v->second->item->type;
    }
 return 0;
}
const char_t* TinfoBase::getVal(int id,int *wasChangePtr,int *splitlinePtr)
{
 bool wasChange=false,splitline;
 Tvalues::iterator v=values.find(id);
 if (v==values.end()) return NULL;
 const char_t *value=v->second->getVal(wasChange,splitline);
 if (wasChangePtr) *wasChangePtr=wasChange;
 if (splitlinePtr) *splitlinePtr=splitline;
 return value;
}

//==================================== TinfoDec ======================================
TinfoDec::TinfoDec(IffdshowBase *Ideci):TinfoBase(Ideci),deciD(comptrQ<IffdshowDec>(deci))
{
 static const TinfoItem items[]=
  {
   IDFF_OSDtype_encoderInfo,_l("Encoder info"),_l("enc"),
   IDFF_OSDtype_shortInfo,_l("Short configuration description"),_l("info"),
   IDFF_OSDtype_movieSource,_l("Decoder"),_l("dec"),
   IDFF_OSDtype_exeflnm,_l("Host application"),_l("host"),
   IDFF_OSDtype_activePresetName,_l("Active preset name"),_l("preset"),
   0
  };
 addItems(items);
}

const char_t* TinfoDec::TinfoValueDec::getVal0(bool &wasChange,bool &splitline)
{
 switch (item->type)
  {
   case IDFF_OSDtype_encoderInfo:
    if (olds[0]=='\0')
     {
      char_t encoder[100]=_l("unknown");
      deciD->getEncoderInfo(encoder,100);
      tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s"), encoder);
      wasChange=true;
     }
    return s;
   case IDFF_OSDtype_shortInfo:
    deciD->getShortDescription(s,512);
    wasChange=strcmp(s,olds)!=0;
    splitline=true;
    return s;
   case IDFF_OSDtype_movieSource:
    ff_strncpy(s, deciD->getDecoderName(), countof(s));
    wasChange=strcmp(s,olds)!=0;
    return s;
   case IDFF_OSDtype_bps:
    {
     int bps=deciD->getInputBitrate2();
     tsprintf(s,bps==-1?_l("N/A"):_l("%i kbps"),bps);
     wasChange=true;
     return s;
    }
   case IDFF_OSDtype_activePresetName:
    {
     deciD->getActivePresetName(s,countof(s));
     wasChange=strcmp(s,olds)!=0;
     return s;
    }
   default:
    return TinfoValueBase::getVal0(wasChange,splitline);
  }
}

//================================== TinfoDecVideo ===================================
TinfoDecVideo::TinfoDecVideo(IffdshowBase *Ideci):
 TinfoDec(Ideci),
 deciV(comptrQ<IffdshowDecVideo>(deci))
{
 static const TinfoItem items[]=
  {
   IDFF_OSDtype_inputSize,_l("Input size"),_l("idim"),
   IDFF_OSDtype_inputAspect,_l("Input aspect ratio"),_l("iar"),
   IDFF_OSDtype_inputSizeAspect,_l("Input size and aspect ratio"),_l("idimar"),
   IDFF_OSDtype_inputFPS,_l("Movie FPS"),_l("fps"),
   IDFF_decodingFps,_l("Decoder FPS"),_l("dfps"),
   IDFF_OSDtype_bps,_l("Input bitrate"),_l("ib"),
   IDFF_OSDtype_meanQuant,_l("Frame mean quantizer"),_l("mq"),
   IDFF_OSDtype_currentFrameTime,_l("Current playing time"),_l("ct"),
   IDFF_OSDtype_remainingFrameTime,_l("Remaining time"),_l("rtime"),
   IDFF_OSDtype_accurDeblock,_l("Accurate deblocking"),_l("ad"),
   IDFF_OSDtype_inputFOURCC,_l("Input FOURCC"),_l("ifcc"),
   IDFF_AVIcolorspace,_l("Input colorspace"),_l("icsp"),
   IDFF_OSDtype_outputFOURCC,_l("Output colorspace"),_l("ocsp"),
   IDFF_currentFrame,_l("Current frame"),_l("fnum"),
   IDFF_subDelay,_l("Subtitles delay"),_l("subd"),
   IDFF_OSDtype_QueueCount,_l("Queued samples"),_l("queue"),
   IDFF_OSDtype_Late,_l("Video delay"),_l("late"),
   IDFF_OSDtype_TimeOnffdshow,_l("Time on ffdshow"),_l("timeon"),
   IDFF_OSDtype_idct,_l("IDCT(libavcodec)"),_l("idct"),
   IDFF_OSDtype_AviSynth_Info,_l("AviSynth info"),_l("avisynth"),
   IDFF_OpenSVCDecoderInfo, _l("This codec has been developped by the IETR. \n For further information, please contact mblestel@insa-rennes.fr"),
#ifdef OSDTIMETABALE
   IDFF_OSDtype_timetable,_l("Time table"),_l("timet"),
#endif
   0
  };
 addItems(items);
}

const char_t* TinfoDecVideo::TinfoValueDecVideo::getInputSize(char_t *s,bool &wasChange)
{
 unsigned int dx=0,dy=0;
 deciV->getAVIdimensions(&dx,&dy);
 if (oldDx!=dx || oldDy!=dy)
  {
   tsprintf(s,_l("%ux%u"),dx,dy);
   wasChange=true;
  }
 oldDx=dx;oldDy=dy;
 return s;
}
const char_t* TinfoDecVideo::TinfoValueDecVideo::getInputAspect(char_t *s, bool &wasChange, size_t buflen)
{
 unsigned int sar1=0,sar2=0,dar1=0,dar2=0;
 bool sarOk=deciV->getInputSAR(&sar1,&sar2)==S_OK;
 bool darOk=deciV->getInputDAR(&dar1,&dar2)==S_OK;
 if (oldSar1!=sar1 || oldSar2!=sar2 || oldDar1!=dar1 || oldDar2!=dar2)
  {
   if (sarOk)
    tsprintf(s,_l("SAR: %u/%u, "),sar1,sar2);
   else
    tsprintf(s,_l("SAR: N/A, "));

   if (darOk)
    strncatf(s, buflen,_l("DAR: %u/%u"),dar1,dar2);
   else
    strncatf(s, buflen,_l("DAR: N/A"));
   wasChange=true;
  }
 oldSar1=sar1;oldSar2=sar2;oldDar1=dar1;oldDar2=dar2;
 return s;
}
const char_t* TinfoDecVideo::TinfoValueDecVideo::getVal0(bool &wasChange,bool &splitline)
{
 int percent;
 switch (item->type)
  {
#ifdef OSDTIMETABALE
   case IDFF_OSDtype_timetable:
    tsprintf(s,_l("%3.2fms"),deciV->getOSDtime()/10000.0);
    wasChange=true;
    return s;
#endif
   case IDFF_OSDtype_TimeOnffdshow:
    percent=deciV->get_time_on_ffdshow_percent();
    if(percent<0 || percent>300)
     tsprintf(s,_l("%3dms (N/A )"),deciV->get_time_on_ffdshow());
    else
     tsprintf(s,_l("%3dms (%3d%%)"),deciV->get_time_on_ffdshow(),percent);
    wasChange=true;
    return s;
   case IDFF_OSDtype_inputSize:
    return getInputSize(s,wasChange);
   case IDFF_OSDtype_inputAspect:
    return getInputAspect(s, wasChange, countof(s));
   case IDFF_OSDtype_inputSizeAspect:
    {
     bool wasChangeSize=false;
     getInputSize(sizeStr,wasChangeSize);
     bool wasChangeAspect=false;
     getInputAspect(aspectStr, wasChangeAspect, countof(aspectStr));
     if (wasChange=(wasChangeSize || wasChangeAspect))
      tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s, %s"), sizeStr, aspectStr);
     return s;
    }
   case IDFF_OSDtype_meanQuant:
    {
     char_t news[60];
     float q;
     if (deciV->calcMeanQuant(&q)==S_OK && q>0)
      tsprintf(news,_l("%-5.2f"),q);
     else
      ff_strncpy(news, _l("not available"), countof(news));
     if (strcmp(news,olds)!=0)
      {
       ff_strncpy(s, news, countof(s));
       wasChange=true;
      }
     return s;
    }
   case IDFF_OSDtype_outputFOURCC:
    deciV->getOutputFourcc(s,50);
    wasChange=strcmp(s,olds)!=0;
    return s;
   case IDFF_OSDtype_currentFrameTime:
    {
     int val;
     if (SUCCEEDED(deciV->getCurrentFrameTime((unsigned int*)&val)))
      {
       tsprintf(s,_l("%02i:%02i:%02i"),val/3600,(val/60)%60,val%60);
       wasChange=true;
      }
     else
      {
       strcpy(s, _l("failed"));
       wasChange=false;
      }
     return s;
    }
   case IDFF_OSDtype_remainingFrameTime:
    {
     int val;
     if (SUCCEEDED(deciV->getRemainingFrameTime((unsigned int*)&val)))
      {
       tsprintf(s,_l("%02i:%02i:%02i"),val/3600,(val/60)%60,val%60);
       wasChange=true;
      }
     else
      {
       strcpy(s, _l("failed"));
       wasChange=false;
      }
     return s;
    }
   case IDFF_OSDtype_accurDeblock:
    if (olds[0]=='\0')
     {
      tsprintf(s,deciV->quantsAvailable()==S_OK?_l("yes"):_l("no"));
      wasChange=true;
     }
    return s;
   case IDFF_OSDtype_inputFPS:
    {
     unsigned int fps1000;
     if (deciV->getAVIfps(&fps1000)!=S_OK)
      s[0]='\0';
     else
      tsprintf(s,_l("%-7.3f"),float(fps1000/1000.0));
     wasChange=strcmp(s,olds)!=0;
     return s;
    }
   case IDFF_OSDtype_inputFOURCC:
    {
     fourcc2str(deciV->getMovieFOURCC(),s,countof(s));
     wasChange=strcmp(s,olds)!=0;
     return s;
    }
   case IDFF_OSDtype_QueueCount:
    {
     int val= deciV->getQueuedCount();
     wasChange=true;
     if(val>=0)
      tsprintf(s,_l("%2d"),val);
     else
      {
       val=-1*val;
       switch(val)
        {
          case IDD_QUEUEMSG_1:
          case IDD_QUEUEMSG_2:
          case IDD_QUEUEMSG_3:
          case IDD_QUEUEMSG_4:
          case IDD_QUEUEMSG_5:
          case IDD_QUEUEMSG_6:
          case IDD_QUEUEMSG_7:
           ff_strncpy(s, trans->translate(val), countof(s));
           break;
          case IDD_QUEUEMSG_8:
           {
            int late=(int)((-1)*deciV->getLate()/10000);
            tsnprintf_s(s, countof(s), _TRUNCATE, _l("%s %4dms"), trans->translate(val), late > 0 ? late : 0);
           }
        }
      }
     return s;
    }
   case IDFF_OSDtype_Late:
    {
     int late=(int)deciV->getLate()/10000;
     tsprintf(s,_l("%7dms"),late>0?late:0);
     wasChange=true;
     return s;
    }
   case IDFF_OSDtype_idct:
    {
     const char *idct0=deciV->get_current_idct();
     if (idct0)
      {
       text<char_t> idct(idct0);
       ff_strncpy(s, (const char_t*)idct, countof(s));
      }
     else
      tsprintf(s,_l("unknown"));
    }
    return s;
   case IDFF_OSDtype_AviSynth_Info:
    {
     const char *info0=deciV->getAviSynthInfo();
     if (info0)
      {
       text<char_t> info(info0);
       ff_strncpy(s, (const char_t*)info, countof(s));
      }
     else
      tsprintf(s,_l("unavailable"));
     wasChange=true;
     return s;
    }
   default:
    return TinfoValueDec::getVal0(wasChange,splitline);
  }
}

//================================ TinfoDecVideoPict =================================
TinfoDecVideoPict::TinfoDecVideoPict(IffdshowBase *Ideci):TinfoDecVideo(Ideci)
{
 static const TinfoItem items[]=
  {
   IDFF_OSDtype_outputSize,_l("Output size"),_l("odim"),
   IDFF_OSDtype_outputAspect,_l("Output aspect ratio"),_l("oar"),
   IDFF_frameType,_l("Frame type"),_l("ft"),
   IDFF_frameSize,_l("Coded frame size"),_l("cfsz"),
   IDFF_OSDtype_gmcWarpPoints,_l("GMC warp points"),_l("gmc"),
   IDFF_OSDtype_frameMD5,_l("MD5 sum"),_l("md5"),
   IDFF_OSDtype_frameTimestamps,_l("Frame timestamps"),_l("fpts"),
   IDFF_OSDtype_frameDuration,_l("Frame duration"),_l("fdur"),
   0
  };
 addItems(items);
}

//================================== TinfoDecAudio ===================================
TinfoDecAudio::TinfoDecAudio(IffdshowBase *Ideci):TinfoDec(Ideci),deciA(comptrQ<IffdshowDecAudio>(deci))
{
 static const TinfoItem items[]=
  {
   IDFF_OSDtype_nchannels,_l("Number of channels"),_l("nch"),
   IDFF_OSDtype_sampleFrequency,_l("Sample frequency"),_l("freq"),
   IDFF_OSDtype_audioSampleFormat,_l("Sample format"),_l("sfmt"),
   IDFF_OSDtype_outSpeakersConfig,_l("Output speakers"),_l("ospk"),
   IDFF_OSDtype_bps,_l("Current input bitrate"),_l("cibr"),
   IDFF_OSDtype_audioJitter,_l("Jitter"),_l("jitt"),
   0
  };
 addItems(items);
}

const char_t* TinfoDecAudio::TinfoValueDecAudio::getVal0(bool &wasChange,bool &splitline)
{
 switch (item->type)
  {
   case IDFF_OSDtype_outSpeakersConfig:
    deciA->getOutSpeakersDescr(s,512,true);
    wasChange=strcmp(s,olds)!=0;
    return s;
   case IDFF_OSDtype_audioSampleFormat:
    {
     int sf;
     const char_t *sampleFormat=SUCCEEDED(deciA->currentSampleFormat(NULL,NULL,&sf))?TsampleFormat::descriptionPCM(sf):_l("unknown");
     ff_strncpy(s, sampleFormat, countof(s));
     wasChange=oldSampleFormat!=sampleFormat;
     oldSampleFormat=sampleFormat;
     return s;
    }
   case IDFF_OSDtype_sampleFrequency:
    {
     unsigned int freq;
     if (FAILED(deciA->currentSampleFormat(NULL,&freq,NULL)))
      freq=0;
     wasChange=oldfreq!=freq;
     oldfreq=freq;
     return _itoa(freq,s,10);
    }
   case IDFF_OSDtype_nchannels:
    {
     unsigned int nchannels;
     if (FAILED(deciA->currentSampleFormat(&nchannels,NULL,NULL)))
      nchannels=0;
     wasChange=oldnchannels!=nchannels;
     oldnchannels=nchannels;
     return _itoa(nchannels,s,10);
    }
   case IDFF_OSDtype_audioJitter:
    tsprintf(s,_l("%5i ms"),deciA->getJitter());
    wasChange=true;
    return s;
   default:
    return TinfoValueDec::getVal0(wasChange,splitline);
  }
}
