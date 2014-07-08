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

/*
 * Modified by Haruhiko Yamagata in 2006-2007.
 */

#include "stdafx.h"
#include <windows.h>
#include "TffDecoderVideo.h"
#include "Tconfig.h"
#include "TglobalSettings.h"
#include "Tpresets.h"
#include "TpresetSettingsVideo.h"
#include "ToutputVideoSettings.h"
#include "TresizeAspectSettings.h"
#include "TdialogSettings.h"
#include "ffdebug.h"
#include "ffdshow_mediaguids.h"
#include "ffcodecs.h"

#include "TtrayIcon.h"
#include "TcpuUsage.h"
#include "TffPict.h"
#include "TimgFilters.h"
#include "TffdshowVideoInputPin.h"
#include "TtextInputPin.h"
#include "Tlibmplayer.h"
#include "TfontManager.h"
#include "TvideoCodec.h"
#include "dsutil.h"
#include "TkeyboardDirect.h"
#include "ffdshowRemoteAPIimpl.h"
#include "IffdshowEnc.h"
#include "ThwOverlayControlOverlay.h"
#include "ThwOverlayControlVMR9.h"
#include "TffdshowDecVideoOutputPin.h"
#include "Tinfo.h"
#include "D3d9.h"
#include "Vmr9.h"
#include "IVMRffdshow9.h"
#include "IffdshowDecAudio.h"
#include "qnetwork.h"

//#include "evr9.h"
//#include "dxva.h"

void TffdshowDecVideo::getMinMax(int id,int &min,int &max)
{
 if (id==IDFF_subCurLang)
  {
   min=0;
   max=getSubtitleLanguagesCount2()-1;
  }
}

// constructor
TffdshowDecVideo::TffdshowDecVideo(CLSID Iclsid,const char_t *className,const CLSID &Iproppageid,int IcfgDlgCaptionId,int IiconId,LPUNKNOWN punk,HRESULT *phr,int Imode,int IdefaultMerit,TintStrColl *Ioptions):
 TffdshowVideo(this,m_pOutput,this,this),
 TffdshowDec(Ioptions,
             className,punk,Iclsid,
             globalSettings=new TglobalSettingsDecVideo(&config,Imode,Ioptions),
             dialogSettings=new TdialogSettingsDecVideo(Imode&IDFF_FILTERMODE_VFW?true:false,Ioptions),
             presets=Imode&IDFF_FILTERMODE_PROC?(TpresetsVideo*)new TpresetsVideoProc(Imode):(Imode&IDFF_FILTERMODE_VFW?(TpresetsVideo*)new TpresetsVideoVFW(Imode):(TpresetsVideo*)new TpresetsVideoPlayer(Imode)),
             (Tpreset*&)presetSettings,
             this,
             (TinputPin*&)inpin,
             m_pOutput,
             m_pGraph,
             (Tfilters*&)imgFilters,
             Iproppageid,IcfgDlgCaptionId,IiconId,
             defaultMerit),
 decVideo_char(punk,this),
 outOldRenderer(false),
 m_IsOldVideoRenderer(false),
 m_IsOldVMR9RenderlessAndRGB(false),
 hReconnectEvent(NULL),
 m_aboutToFlash(false),
 OSD_time_on_ffdshowStart(0),
 OSD_time_on_ffdshowBeforeGetBuffer(0),
 OSD_time_on_ffdshowAfterGetBuffer(0),
 OSD_time_on_ffdshowEnd(0),
 OSD_time_on_ffdshowResult(0),
 OSD_time_on_ffdshowOldStart(0),
 OSD_time_on_ffdshowDuration(0),
 isOSD_time_on_ffdshow(false),
 OSD_time_on_ffdshowFirstRun(true),
 isQueue(-1),
 m_IsYV12andVMR9(false),
 m_IsQueueListedApp(-1),
 reconnectFirstError(true),
 m_NeedToAttachFormat(false),
 inReconnect(false),
 compatibleFilterConnected(false),
 inputConnectedPin(NULL),
 m_rtStart(0),
 inSampleEverField1Repeat(false),
 m_NeedToPauseRun(false),
 searchInterfaceInGraph(NULL),
 count_decoded_frames_for_framerate_calculation(0)
{
 DPRINTF(_l("TffdshowDecVideo::Constructor"));
#ifdef OSDTIMETABALE
 OSDtimeMax= 0;
#endif
 ASSERT(phr);

 setThreadName(DWORD(-1),"decVideo");

 static const TintOptionT<TffdshowDecVideo> iopts[]=
  {
   IDFF_currentFrame       ,&TffdshowDecVideo::currentFrame      , 1, 1,_l("Current frame"),0,NULL,0,
   IDFF_decodingFps        ,&TffdshowDecVideo::decodingFps       ,-1,-1,_l("Decoding Fps"),0,NULL,0,
   IDFF_AVIcolorspace      ,&TffdshowDecVideo::decodingCsp       ,-1,-1,_l("Input colorspace"),0,NULL,0,
   IDFF_dvdproc            ,&TffdshowDecVideo::dvdproc           ,-1,-1,_l(""),0,NULL,0,

   IDFF_currentq           ,&TffdshowDecVideo::currentq          ,0,6,_l(""),0,NULL,0,

   IDFF_subShowEmbedded    ,&TffdshowDecVideo::subShowEmbedded   ,0,40,_l(""),0,NULL,0,
   //IDFF_subFoundEmbedded   ,&TffdshowDecVideo::foundEmbedded     ,0,0,_l(""),0,NULL,0,
   IDFF_subCurLang         ,&TffdshowDecVideo::subCurLang        ,-3,-3,_l(""),0,NULL,0,
   0
  };
 addOptions(iopts);

 trayIconStart=&TtrayIconBase::start<TtrayIconDecVideo>;

 inpin=new TffdshowVideoInputPin(NAME("TffdshowVideoInputPin"),this,phr);
 if (!inpin) *phr=E_OUTOFMEMORY;
 m_pInput=inpin;
 if (FAILED(*phr)) return;
 //textpin=new TtextInputPin(this,phr);
 m_pOutputDecVideo= NULL;
 m_pOutputDecVideo=new TffdshowDecVideoOutputPin(NAME("TffdshowDecVideoOutputPin"),this,phr,L"Out");
 if(!m_pOutputDecVideo) *phr=E_OUTOFMEMORY;
 if (FAILED(*phr))
  {
   delete m_pInput;m_pInput=NULL;
   return;
  }
  m_pOutput= m_pOutputDecVideo;

 subShowEmbedded=0;
 imgFilters=NULL;
 wasSubtitleResetTime=false;fontManager=NULL;
 m_frame.dstColorspace=0;
 currentFrame=-1;decodingFps=0;decodingCsp=FF_CSP_NULL;
 segmentStart=-1;segmentFrameCnt=0;
 vc1frameCnt = 0;
 vc1rtStart=0;
 videoWindow=NULL;wasVideoWindow=false;
 basicVideo=NULL;wasBasicVideo=false;
 currentq=0;
 subCurLang=0;
 dvdproc=0;
 hwDeinterlace=false;
 waitForKeyframe=0;
 ppropActual.cBuffers=1;
}

TffdshowDecVideoRaw::TffdshowDecVideoRaw(LPUNKNOWN punk,HRESULT *phr):TffdshowDecVideo(CLSID_FFDSHOWRAW,NAME("TffdshowDecVideoRaw"),CLSID_TFFDSHOWPAGERAW,IDS_FFDSHOWDECVIDEORAW,IDI_FFDSHOW,punk,phr,IDFF_FILTERMODE_PLAYER|IDFF_FILTERMODE_VIDEORAW,defaultMerit,new TintStrColl)
{
}

TffdshowDecVideoSubtitles::TffdshowDecVideoSubtitles(LPUNKNOWN punk,HRESULT *phr):TffdshowDecVideo(CLSID_FFDSHOWSUBTITLES,NAME("TffdshowDecVideoSubtitles"), CLSID_TFFDSHOWPAGESUBTITLES, IDS_FFDSHOWDECVIDEOSUBTITLES, IDI_FFDSHOW,punk,phr, IDFF_FILTERMODE_PLAYER | IDFF_FILTERMODE_VIDEOSUBTITLES | IDFF_FILTERMODE_VIDEORAW, defaultMerit, new TintStrColl)
{
}

// destructor
TffdshowDecVideo::~TffdshowDecVideo()
{
 DPRINTF(_l("TffdshowDecVideo::Destructor"));
 if (searchInterfaceInGraph) delete searchInterfaceInGraph;
 for (size_t i=0;i<textpins.size();i++) delete textpins[i];
 if (fontManager) delete fontManager;
 if (inputConnectedPin != NULL)
 {
  inputConnectedPin->Release();
  inputConnectedPin = NULL;
 }
}

HRESULT TffdshowDecVideo::CheckConnect(PIN_DIRECTION dir,IPin *pPin)
{
 HRESULT res;
 switch (dir)
  {
   case PINDIR_INPUT:
    dvdproc=searchPrevNextFilter(PINDIR_INPUT,pPin,CLSID_DVDNavigator);
    return S_OK;
   case PINDIR_OUTPUT:
    {
     if (!presetSettings) initPreset();
     res = checkAllowOutChange(pPin);
     break;
    }
   default:
    return E_UNEXPECTED;
  }
 return res==S_OK?CTransformFilter::CheckConnect(dir,pPin):res;
}

HRESULT TffdshowDecVideo::checkAllowOutChange(IPin *pPin)
{
 HRESULT hr;
 CLSID clsid=GetCLSID(pPin);
 outOldRenderer=!!(clsid==CLSID_VideoRenderer);
 if (presetSettings->output->allowOutChange3 || dvdproc)
  {
   bool filterOk=clsid==CLSID_OverlayMixer ||
                 clsid==CLSID_VideoMixingRenderer ||
                 clsid==CLSID_VideoMixingRenderer9 ||
                 clsid==CLSID_EnhancedVideoRenderer ||
                 clsid==CLSID_DirectVobSubFilter ||
                 clsid==CLSID_DirectVobSubFilter2 ||
                 clsid==CLSID_HaaliVideoRenderer ||
                 clsid==CLSID_FFDSHOW || clsid==CLSID_FFDSHOWRAW || clsid==CLSID_FFDSHOWSUBTITLES;
   allowOutChange=dvdproc ||
                  presetSettings->output->allowOutChange3==1 ||
                  (presetSettings->output->allowOutChange3==2 && filterOk);

   if (presetSettings->output->allowOutChange3==1 && presetSettings->output->outChangeCompatOnly)
    hr=filterOk ? S_OK : E_FAIL;
   else
    hr=S_OK;
  }
 else
  {
   allowOutChange=false;
   hr=S_OK;
  }
 if (clsid==CLSID_DecklinkVideoRenderFilter || clsid==CLSID_InfTee || clsid==CLSID_SmartT || clsid==CLSID_WMP_VIDEODSP_DMO)
  allowOutChange=false;
 return hr;
}

HRESULT TffdshowDecVideo::CheckInputType(const CMediaType *mtIn)
{
 return S_OK;
}

// get list of supported output colorspaces
HRESULT TffdshowDecVideo::GetMediaType(int iPosition, CMediaType *mtOut)
{
 DPRINTF(_l("TffdshowDecVideo::GetMediaType"));
 if (m_pInput->IsConnected()==FALSE) return E_UNEXPECTED;

 if (!presetSettings) initPreset();

 int hwOverlay0,hwOverlay;
 hwOverlay0=hwOverlay=presetSettings->output->hwOverlay;
 if (hwOverlay==0 && presetSettings->output->hwDeinterlace)
  hwOverlay=2;
 if (hwOverlay==2 && m_pOutput->IsConnected())
  {
   const CLSID &ref=GetCLSID(m_pOutput->GetConnected());
   if (ref==CLSID_VideoMixingRenderer || ref==CLSID_VideoMixingRenderer9)
    hwOverlay=1;
  }

 bool isVIH2=!outdv && (hwOverlay==1 || (hwOverlay==2 && (iPosition&1)==0));

 if (hwOverlay==2) iPosition/=2;
 if (iPosition<0) return E_INVALIDARG;
 TcspInfos ocsps;size_t osize;
 if (outdv)
  osize=1;
 else
  {
   presetSettings->output->getOutputColorspaces(ocsps);
   osize=ocsps.size();
  }
 if ((size_t)iPosition>=osize) return VFW_S_NO_MORE_ITEMS;

 TffPictBase pictOut;
 if (inReconnect)
  pictOut=reconnectRect;
 else
  pictOut=inpin->pictIn;
 calcNewSize(pictOut);
 if (presetSettings->output->closest && !outdv) ocsps.sort(pictOut.csp);

 oldRect=pictOut.rectFull;

 static const TcspInfo dv=
  {
   0,_l("DVSD"),
   3,24, //Bpp
   1, //numplanes
   {0,0,0,0}, //shiftX
   {0,0,0,0}, //shiftY
   {0,0,0,0},  //black,
   FOURCC_DVSD, FOURCC_YV12, &MEDIASUBTYPE_DVSD
  };
 const TcspInfo *c=outdv?&dv:ocsps[iPosition];
 BITMAPINFOHEADER bih;memset(&bih,0,sizeof(bih));
 bih.biSize  =sizeof(BITMAPINFOHEADER);
 bih.biWidth =pictOut.rectFull.dx;
 if(c->id == FF_CSP_420P)    // YV12 and odd number lines.
  {
   pictOut.rectFull.dy=ODD2EVEN(pictOut.rectFull.dy);
  }
 bih.biHeight=pictOut.rectFull.dy;
 bih.biPlanes=WORD(c->numPlanes);
 bih.biCompression=c->fcc;
 bih.biBitCount=WORD(c->bpp);
 bih.biSizeImage=DIBSIZE(bih);// bih.biWidth*bih.biHeight*bih.biBitCount>>3;

 mtOut->majortype=MEDIATYPE_Video;
 mtOut->subtype=*c->subtype;
 mtOut->formattype=isVIH2?FORMAT_VideoInfo2:FORMAT_VideoInfo;
 mtOut->SetTemporalCompression(FALSE);
 mtOut->SetSampleSize(bih.biSizeImage);

 if (!isVIH2)
  {
   VIDEOINFOHEADER *vih=(VIDEOINFOHEADER*)mtOut->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER));
   if (!vih) return E_OUTOFMEMORY;
   ZeroMemory(vih,sizeof(VIDEOINFOHEADER));

   vih->rcSource.left=0;vih->rcSource.right=bih.biWidth;vih->rcSource.top=0;vih->rcSource.bottom=bih.biHeight;
   vih->rcTarget=vih->rcSource;
   vih->AvgTimePerFrame=inpin->avgTimePerFrame;
   vih->bmiHeader=bih;
  }
 else
  {
   VIDEOINFOHEADER2 *vih2=(VIDEOINFOHEADER2*)mtOut->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER2));
   if (!vih2) return E_OUTOFMEMORY;
   ZeroMemory(vih2,sizeof(VIDEOINFOHEADER2));
   if((presetSettings->resize && presetSettings->resize->is && presetSettings->resize->SARinternally && presetSettings->resize->mode==0) || hwOverlay0==0)
    {
     pictOut.rectFull.sar.num= 1;//pictOut.rectFull.dx; // VMR9 behaves better when this is set to 1(SAR). But in reconnectOutput, it is different(DAR) in my system.
     pictOut.rectFull.sar.den= 1;//pictOut.rectFull.dy;
    }
   setVIH2aspect(vih2,pictOut.rectFull,presetSettings->output->hwOverlayAspect);

   //DPRINTF(_l("AR getMediaType: %i:%i"),vih2->dwPictAspectRatioX,vih2->dwPictAspectRatioY);

   vih2->rcSource.left=0;vih2->rcSource.right=bih.biWidth;vih2->rcSource.top=0;vih2->rcSource.bottom=bih.biHeight;
   vih2->rcTarget=vih2->rcSource;
   vih2->AvgTimePerFrame=inpin->avgTimePerFrame;
   vih2->bmiHeader=bih;
   //vih2->dwControlFlags=AMCONTROL_USED | AMCONTROL_COLORINFO_PRESENT | (DXVA_NominalRange_Wide << DXVA_NominalRangeShift) | (DXVA_VideoTransferMatrix_BT601 << DXVA_VideoTransferMatrixShift);
   hwDeinterlace=presetSettings->output->hwDeinterlace;
   if (hwDeinterlace)
    vih2->dwInterlaceFlags=AMINTERLACE_IsInterlaced|AMINTERLACE_DisplayModeBobOrWeave;
  }
 return S_OK;
}

HRESULT TffdshowDecVideo::setOutputMediaType(const CMediaType &mt)
{
 DPRINTF(_l("TffdshowDecVideo::setOutputMediaType"));
/*  enable this to strictly require selected output colorspaces
 bool ok=false;
 TcspInfos ocsps;presetSettings->output->getOutputColorspaces(ocsps);
 for (TcspInfos::const_iterator oc=ocsps.begin();oc!=ocsps.end();oc++)
  if (mt.subtype==*(*oc)->subtype)
   {
    ok=true;
    break;
   }
 if (!ok) return S_FALSE;
*/
 for (int i=0;cspFccs[i].name;i++)
  {
   const TcspInfo *cspInfo;
   if (*(cspInfo=csp_getInfo(cspFccs[i].csp))->subtype==mt.subtype)
    {
     m_frame.dstColorspace=cspFccs[i].csp;
     if (m_frame.dstColorspace==FF_CSP_NV12) m_frame.dstColorspace=FF_CSP_NV12|FF_CSP_FLAGS_YUV_ORDER; // HACK
     int biWidth,outDy;
     BITMAPINFOHEADER *bih;
     if (mt.formattype==FORMAT_VideoInfo && mt.pbFormat) // && mt.pbFormat = work around other filter's bug.
      {
       VIDEOINFOHEADER *vih=(VIDEOINFOHEADER*)mt.pbFormat;
       m_frame.dstStride=calcBIstride(biWidth=vih->bmiHeader.biWidth,cspInfo->Bpp*8);
       outDy=vih->bmiHeader.biHeight;
       bih=&vih->bmiHeader;
      }
     else if (mt.formattype==FORMAT_VideoInfo2 && mt.pbFormat)
      {
       VIDEOINFOHEADER2 *vih2=(VIDEOINFOHEADER2*)mt.pbFormat;
       m_frame.dstStride=calcBIstride(biWidth=vih2->bmiHeader.biWidth,cspInfo->Bpp*8);
       outDy=vih2->bmiHeader.biHeight;
       bih=&vih2->bmiHeader;
      }
     else
      return VFW_E_TYPE_NOT_ACCEPTED; //S_FALSE;
     m_frame.dstSize=DIBSIZE(*bih);

     char_t s[256];
     DPRINTF(_l("TffdshowDecVideo::setOutputMediaType: colorspace:%s, biWidth:%i, dstStride:%i, Bpp:%i, dstSize:%i"),csp_getName(m_frame.dstColorspace,s,256),biWidth,m_frame.dstStride,cspInfo->Bpp,m_frame.dstSize);
     if (csp_isRGB(m_frame.dstColorspace) && outDy>0)
      m_frame.dstColorspace|=FF_CSP_FLAGS_VFLIP;
     //else if (biheight<0)
     // m_frame.colorspace|=FF_CSP_FLAGS_VFLIP;
     return S_OK;
    }
  }
 m_frame.dstColorspace=FF_CSP_NULL;
 return VFW_E_TYPE_NOT_ACCEPTED; //S_FALSE;
}

// set output colorspace
HRESULT TffdshowDecVideo::SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt)
{
 DPRINTF(_l("TffdshowDecVideo::SetMediaType"));
 switch (direction)
  {
   case PINDIR_INPUT:
    return S_OK;
   case PINDIR_OUTPUT:
    return setOutputMediaType(*pmt);
   default:
    return E_UNEXPECTED;
  }
}

// check input<->output compatibility
HRESULT TffdshowDecVideo::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
 DPRINTF(_l("TffdshowDecVideo::CheckTransform"));
#if 0
 BITMAPINFOHEADER *bmi=NULL;
 if (mtOut && mtOut->formattype==FORMAT_VideoInfo2
   && mtOut->pbFormat
   && mtOut->cbFormat>=sizeof(VIDEOINFOHEADER2))
  {
   VIDEOINFOHEADER2* vih = (VIDEOINFOHEADER2*)mtOut->pbFormat;
   bmi=&vih->bmiHeader;
  }
 if (mtOut && mtOut->formattype==FORMAT_VideoInfo
   && mtOut->pbFormat
   && mtOut->cbFormat>=sizeof(VIDEOINFOHEADER))
  {
   VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)mtOut->pbFormat;
   bmi=&vih->bmiHeader;
  }
 if (bmi && bmi->biCompression && !outdv)
  {
   TcspInfos ocsps;
   size_t osize;
   presetSettings->output->getOutputColorspaces(ocsps);
   osize=ocsps.size();
   bool outOk=false;
   for (size_t i=0;i<osize;i++)
    {
     if (ocsps[i]->fcc==bmi->biCompression)
      {
       outOk=true;
       break;
      }
    }
   if (!outOk)
    return VFW_E_TYPE_NOT_ACCEPTED;   
  }
#endif
 if (!outOldRenderer)
  return S_OK;
 else
  {
   BITMAPINFOHEADER biOut;
   ExtractBIH(*mtOut,&biOut);
   if (ff_abs(biOut.biWidth)>=oldRect.dx && ff_abs(biOut.biHeight)==oldRect.dy)
    return S_OK;
   else
    return VFW_E_TYPE_NOT_ACCEPTED;
  }
}

HRESULT TffdshowDecVideo::CompleteConnect(PIN_DIRECTION direction,IPin *pReceivePin)
{
 if (direction==PINDIR_INPUT)
  {
  }
 else if (direction==PINDIR_OUTPUT)
  {
   const CLSID &out=GetCLSID(m_pOutput->GetConnected());
   outOverlayMixer=!!(out==CLSID_OverlayMixer);
  }
 return CTransformFilter::CompleteConnect(direction,pReceivePin);
}

// alloc output buffer
HRESULT TffdshowDecVideo::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest)
{
 DPRINTF(_l("TffdshowDecVideo::DecideBufferSize"));
 if (m_pInput->IsConnected()==FALSE)
  return E_UNEXPECTED;

 if (!presetSettings) initPreset();
 if (m_IsQueueListedApp==-1) // Not initialized
  m_IsQueueListedApp= IsQueueListedApp(getExeflnm());

 m_IsOldVideoRenderer= IsOldRenderer();
 const CLSID &ref=GetCLSID(m_pOutput->GetConnected());
 if (isQueue==-1)
  isQueue=presetSettings->multiThread && m_IsQueueListedApp;
 // Queue and Overlay Mixer works only in MPC and
 // when Overlay Mixer is not connected to old video renderer(rare, usually RGB out).
 // If queue can't work with Overlay Mixer, IsOldRenderer() returns true.
 isQueue=isQueue && !m_IsOldVideoRenderer &&
   (ref==CLSID_OverlayMixer || ref==CLSID_VideoMixingRenderer || ref==CLSID_VideoMixingRenderer9);
 isQueue=isQueue && !(m_IsOldVMR9RenderlessAndRGB=IsOldVMR9RenderlessAndRGB()); // inform MPC about queue only when queue is effective.
 // DPRINTF(_l("CLSID 0x%x,0x%x,0x%x"),ref.Data1,ref.Data2,ref.Data3);for(int i=0;i<8;i++) {DPRINTF(_l(",0x%2x"),ref.Data4[i]);}
 if (ref==CLSID_VideoRenderer || ref==CLSID_OverlayMixer)
  return DecideBufferSizeOld(pAlloc, ppropInputRequest,ref);
 else
  return DecideBufferSizeVMR(pAlloc, ppropInputRequest,ref);
}

HRESULT TffdshowDecVideo::DecideBufferSizeVMR(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest, const CLSID &ref)
{
 HRESULT result;
 if (downstreamID==VMR9 && !presetSettings->queueVMR9YV12)
  {
   CMediaType &mt=m_pOutput->CurrentMediaType();
   if (mt.subtype==MEDIASUBTYPE_YV12)
    {
       m_IsYV12andVMR9= true; // to let OSD getQueuedCount know the reason.
       isQueue= false; // queue off internaly.
    }
  }
 int cBuffersMax;
 if (isQueue==1)
  cBuffersMax= presetSettings->queueCount+1;
 else
  cBuffersMax= 1;
 TffPictBase pictOut=inpin->pictIn;calcNewSize(pictOut);
 ppropInputRequest->cbBuffer=pictOut.rectFull.dx*pictOut.rectFull.dy*4;
 // cbAlign 16 causes problems with the resize filter
 //ppropInputRequest->cbAlign =1;
 ppropInputRequest->cbPrefix=0;
 ppropInputRequest->cBuffers=1;

 // first try cBuffers=1, if succeeded try cBuffers=cBuffersMax. Faster than before(older than 20060730).

 // retry because VMR9 does not release current MediaSample that VMR9 itself
 // is holding and waiting for the presentation time,
 // even when it is asked to Reconnect.
 // if frame rate > 10, and 10 frames are queued in VMR9's internal queue, 1000ms would be enough.
 int retry=ppropActual.cBuffers*10;
 bool isretry;
 do {
   result=pAlloc->SetProperties(ppropInputRequest,&ppropActual);
   isretry=result==VFW_E_BUFFERS_OUTSTANDING && retry-->0 && !firsttransform && downstreamID!=VMR7;
   if (isretry) Sleep(10);
  } while (isretry);
 if (result!=S_OK)
  return result;
 if (cBuffersMax>1)
  {
   ppropInputRequest->cBuffers= cBuffersMax;
   while(ppropInputRequest->cBuffers>=1)
   {
    result=pAlloc->SetProperties(ppropInputRequest,&ppropActual);
    if (result==S_OK)
     break;
    ppropInputRequest->cBuffers--;
    DPRINTF(_l("cBuffsers= %d failed. About to try cBuffers= %d"), ppropInputRequest->cBuffers+1, ppropInputRequest->cBuffers);
   }
   if (result==S_OK && ppropInputRequest->cBuffers>=3)
    {
     ppropInputRequest->cBuffers--; // avoiding to keep all the memory.
     result=pAlloc->SetProperties(ppropInputRequest,&ppropActual);
    }
  }
 if (ppropActual.cbBuffer<ppropInputRequest->cbBuffer)
  return E_FAIL;
 if (downstreamID==VMR9)
  isQueue=0; // Use VMR9's internal queueing.
 return result;
}

HRESULT TffdshowDecVideo::DecideBufferSizeOld(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest, const CLSID &ref)
{
/*
 * Overlay mixer doesn't want SetPropoerties called twice. After successfull call of SetPropoerties, it never allow us change the properties.
 * Old renderer doesn't support multithreading, so cBuffers should be 1.
 */
 int cBuffersMax;
 if (isQueue==1)
  cBuffersMax= presetSettings->queueCount;
 else
  cBuffersMax= 1;

 if (ref==CLSID_OverlayMixer)
  ppropInputRequest->cBuffers= cBuffersMax;
 else
  ppropInputRequest->cBuffers= 1;

 TffPictBase pictOut=inpin->pictIn;calcNewSize(pictOut);
 ppropInputRequest->cbBuffer=pictOut.rectFull.dx*pictOut.rectFull.dy*4;
 // cbAlign 16 causes problems with the resize filter */
 //ppropInputRequest->cbAlign =1;
 ppropInputRequest->cbPrefix=0;

 HRESULT result=pAlloc->SetProperties(ppropInputRequest,&ppropActual);
 if (result!=S_OK)
  {
   while(ppropInputRequest->cBuffers>1 && result!=S_OK)
   {
    // retry
    ppropInputRequest->cBuffers--;
    DPRINTF(_l("cBuffsers= %d failed. About to try cBuffers= %d"), ppropInputRequest->cBuffers+1, ppropInputRequest->cBuffers);
    result=pAlloc->SetProperties(ppropInputRequest,&ppropActual);
   }
   if (result!=S_OK)
    return result;
  }
 if (ppropActual.cbBuffer<ppropInputRequest->cbBuffer)
  return E_FAIL;
 return S_OK;
}

void TffdshowDecVideo::ConnectCompatibleFilter(void)
{
	if (compatibleFilterConnected || inpin->pCompatibleFilter == NULL) return;
	HRESULT hr;
	if (inputConnectedPin == NULL) // Reuse of inputConnectedPin is possible
	{
		hr=inpin->ConnectedTo(&inputConnectedPin);
		if(FAILED(hr))
			return;
	}
	IFilterGraph *pGraph=NULL;
	getGraph(&pGraph);
	IGraphBuilder *pGraphBuilder = NULL;
	hr = pGraph->QueryInterface(IID_IGraphBuilder, (void **)&pGraphBuilder);
	if (hr!=S_OK)
		return;
	
	AM_MEDIA_TYPE connectedPinMediaType;
	inpin->ConnectionMediaType(&connectedPinMediaType);

	hr = inputConnectedPin->Disconnect();
	hr = inpin->Disconnect();

	// Browse pins compatible codec
	IEnumPins *enumPins = NULL;
	inpin->pCompatibleFilter->EnumPins(&enumPins);
	IPin *outPin=NULL, *filterPin;
	bool inPinConnected=false, outPinFound=false;
	unsigned long fetched;
	while (1)
	{
		enumPins->Next(1, &filterPin, &fetched);
		if (fetched < 1) break;
		
		PIN_DIRECTION pinDirection;
		filterPin->QueryDirection(&pinDirection);
		if (pinDirection == PINDIR_INPUT && !inPinConnected)
		{
			// Input filter -> FFDShow ==> Input filter -> Compatible filter
			hr = inputConnectedPin->Connect(filterPin, &connectedPinMediaType);
			inPinConnected=true;
		}
		else if (pinDirection == PINDIR_OUTPUT && outPin==NULL)
		{
			outPin=filterPin;
			continue;
		}
		filterPin->Release();
	}
	enumPins->Release();

	// Input filter -> Compatible filter ==> Input filter -> Compatible filter -> FFDShow
	if (outPin==NULL) // Oops... problem, should not happen
	{
		pGraph->Reconnect(inputConnectedPin);
		return;
	}

	AM_MEDIA_TYPE connectedPinMediaTypeOut;
	outPin->ConnectionMediaType(&connectedPinMediaTypeOut);
	//hr=outPin->Connect(inpin, &connectedPinMediaTypeOut);
	hr = pGraphBuilder->Connect(outPin, inpin);

	outPin->Release();
	pGraphBuilder->Release();	

	compatibleFilterConnected=true;
}

void TffdshowDecVideo::DisconnectFromCompatibleFilter(void)
{
	if (!compatibleFilterConnected) return;

	HRESULT hr;
	IPin *connectedPin = NULL;
	hr=inpin->ConnectedTo(&connectedPin);
	if(FAILED(hr))
		return;

	IFilterGraph *pGraph=NULL;
	getGraph(&pGraph);
	IGraphBuilder *pGraphBuilder = NULL;
	hr = pGraph->QueryInterface(IID_IGraphBuilder, (void **)&pGraphBuilder);
	if (hr!=S_OK)
	{
		connectedPin->Release();
		return;
	}

	hr = connectedPin->Disconnect();
	hr = inpin->Disconnect();

	// Browse pins compatible codec
	IEnumPins *enumPins = NULL;
	inpin->pCompatibleFilter->EnumPins(&enumPins);
	IPin *outPin=NULL, *filterPin;
	bool inPinDisConnected=false, outPinFound=false;
	unsigned long fetched;
	while (1)
	{
		enumPins->Next(1, &filterPin, &fetched);
		if (fetched < 1) break;
		
		PIN_DIRECTION pinDirection;
		filterPin->QueryDirection(&pinDirection);
		if (pinDirection == PINDIR_INPUT && !inPinDisConnected)
		{
			IPin *pPin=NULL;
			filterPin->ConnectedTo(&pPin);
			if (pPin!=NULL)
				pPin->Disconnect();
			pPin->Release();
			filterPin->Disconnect();
			inPinDisConnected=true;
		}
		filterPin->Release();
	}
	enumPins->Release();
	connectedPin->Release();
	pGraphBuilder->Release();	

	compatibleFilterConnected=false;
}


HRESULT TffdshowDecVideo::Receive(IMediaSample *pSample)
{
 m_aboutToFlash= false;
 HRESULT hr= ReceiveI(pSample);
 m_aboutToFlash= false;
 return hr;
}

HRESULT TffdshowDecVideo::ReceiveI(IMediaSample *pSample)
{
 // If the next filter downstream is the video renderer, then it may
 // be able to operate in DirectDraw mode which saves copying the data
 // and gives higher performance.  In that case the buffer which we
 // get from GetDeliveryBuffer will be a DirectDraw buffer, and
 // drawing into this buffer draws directly onto the display surface.
 // This means that any waiting for the correct time to draw occurs
 // during GetDeliveryBuffer, and that once the buffer is given to us
 // the video renderer will count it in its statistics as a frame drawn.
 // This means that any decision to drop the frame must be taken before
 // calling GetDeliveryBuffer.
 ASSERT(pSample);
 ASSERT(m_pOutput!=NULL);

 inSampleTypeSpecificFlags = 0;
 AM_SAMPLE2_PROPERTIES inProp2;
 if (comptrQ<IMediaSample2> pIn2=pSample)
  {
   pIn2->GetProperties(sizeof(AM_SAMPLE2_PROPERTIES),(PBYTE)&inProp2);
   inSampleTypeSpecificFlags = inProp2.dwTypeSpecificFlags;
  }
 // If no output pin to deliver to then no point sending us data

 AM_MEDIA_TYPE *pmt;
 // The source filter may dynamically ask us to start transforming from a
 // different media type than the one we're using now.  If we don't, we'll
 // draw garbage. (typically, this is a palette change in the movie,
 // but could be something more sinister like the compression type changing,
 // or even the video size changing)
 if(isOSD_time_on_ffdshow && m_pClock)
  m_pClock->GetTime(&OSD_time_on_ffdshowStart);
 pSample->GetMediaType(&pmt);
 if (pmt!=NULL && pmt->pbFormat!=NULL)
  {
   // spew some debug output
   ASSERT(!IsEqualGUID(pmt->majortype, GUID_NULL));
   // now switch to using the new format.  I am assuming that the
   // derived filter will do the right thing when its media type is
   // switched and streaming is restarted.
   StopStreaming();
   m_pInput->CurrentMediaType() = *pmt;
   DeleteMediaType(pmt);
   // if this fails, playback will stop, so signal an error
   HRESULT hr=StartStreaming();
   if (FAILED(hr)) return abortPlayback(hr);
  }
 // Now that we have noticed any format changes on the input sample, it's OK to discard it.
 REFERENCE_TIME rtStart,rtStop;
 if (pSample->GetTime(&rtStart,&rtStop)==S_OK) // memo: If VFW_S_NO_STOP_TIME is returned, rtStart is supoosed to be valid.
  {
   late-=ff_abs(rtStart-lastrtStart);
   lastrtStart=rtStart;
   insample_rtStart = rtStart;
    insample_rtStop = rtStop;
  }
 else
  {
   late -= inpin->avgTimePerFrame;
   insample_rtStart = REFTIME_INVALID;
   insample_rtStop = REFTIME_INVALID;
  }

 if (m_bSampleSkipped && late>0) return S_OK;
 int codecId = inpin->getInCodecId2();
 if (presetSettings->dropOnDelay && !mpeg12_codec(codecId) && !vc1_codec(codecId) && late>presetSettings->dropDelayTime*10000)
  {
   //MSR_NOTE(m_idSkip);
   setSampleSkipped(true);
   DPRINTF_SAMPLE_TIME(pSample);
   //late=0;
   waitForKeyframe=1000;
   count_decoded_frames_for_framerate_calculation = 0;
   return S_OK;
  }

 // After a discontinuity, we need to wait for the next key frame
 if (pSample->IsDiscontinuity()==S_OK && inpin->waitForKeyframes())
  {
   DPRINTF(_l("Non-key discontinuity - wait for keyframe"));
   waitForKeyframe=100;
  }

 if (firsttransform)
  {
   firsttransform=false;
   initKeys();
   initMouse();
   initRemote();
   onTrayIconChange(0,0);
   options->notifyParam(IDFF_isKeys,0);
   options->notifyParam(IDFF_isMouse,0);
   remote->onChange(0,0);
   lastTime=clock();
   m_IsOldVideoRenderer= IsOldRenderer();
   isQueue=isQueue && !m_IsOldVideoRenderer;
   checkAllowOutChange(m_pOutput->GetConnected());
  }

 if (m_dirtyStop)
  {
   m_dirtyStop = false;
   inpin->onSeek(0);
   imgFilters->onSeek();
  }

 long srcLength;
 HRESULT hr=inpin->decompress(pSample,&srcLength);
 if (srcLength<0)
  return S_FALSE;
 bytesCnt+=srcLength;

 if (waitForKeyframe)
  waitForKeyframe--;

 if (hr==S_FALSE)
  {
   setSampleSkipped(false);
   DPRINTF_SAMPLE_TIME(pSample);
   if (!m_bQualityChanged)
    {
     m_bQualityChanged=TRUE;
     NotifyEvent(EC_QUALITY_CHANGE,0,0);
    }
   hr=S_OK;
  }
 return hr;
}

bool TffdshowDecVideo::IsOldRenderer(void)
{
 // Check downstream filter
 // Does Video Renderer support multithreading?

 const char_t *fileName= getExeflnm();
 bool isOld = false;
 const CLSID &ref=GetCLSID(m_pOutput->GetConnected());
 if(_strnicmp(_l("mplayerc"),fileName,8)!=0)
  {
   return (ref == CLSID_VideoRenderer || ref == CLSID_OverlayMixer) ? true : false;
  }
 else
  {
   if (ref == CLSID_VideoRenderer)
    return true;
   if (ref != CLSID_OverlayMixer)
    return false;
   PIN_INFO pinInfo; 
   m_pOutput->GetConnected()->QueryPinInfo(&pinInfo);
   if (pinInfo.pFilter)
    {
     IEnumPins *iEnum = NULL;
     pinInfo.pFilter->EnumPins(&iEnum);
     if (iEnum)
      {
       ULONG cFetched;
       IPin *overlayPin = NULL;
       iEnum->Reset();
       while (!isOld)
        {
         if (iEnum->Next(1, &overlayPin, &cFetched) != S_OK || !overlayPin)
          break;
         PIN_INFO pinInfo1;
         overlayPin->QueryPinInfo(&pinInfo1);
         if (pinInfo1.pFilter)
          {
           if (pinInfo1.dir == PINDIR_OUTPUT)
            {
             IPin *ipinRenderer;
             if (overlayPin->ConnectedTo(&ipinRenderer) == S_OK && ipinRenderer)
              {
               CLSID clsid = GetCLSID(ipinRenderer);
               if (clsid == CLSID_VideoRenderer)
                isOld = true;
               ipinRenderer->Release();
              }
            }
           pinInfo1.pFilter->Release();
          }
         overlayPin->Release();
        }
       iEnum->Release();
      }
     pinInfo.pFilter->Release();
    }
  }
 return isOld;
}

bool TffdshowDecVideo::IsOldVMR9RenderlessAndRGB(void)
{
 if(!csp_isRGB(m_frame.dstColorspace))
  return false;

 const char_t *fileName= getExeflnm();
 if (_strnicmp(_l("mplayerc.exe"),fileName,13)!=0)
  return false;

 // Check downstream filter
 IBaseFilter* pBaseFilter;

 bool isVMR9rs= false;

 if(graph)
  {
   if(graph->FindFilterByName(L"Video Mixing Render 9 (Renderless)", &pBaseFilter)==S_OK)
    {
     IVMRffdshow9* ivmrffdshow9;
     pBaseFilter->QueryInterface(IID_IVMRffdshow9,(void**)&ivmrffdshow9);
     pBaseFilter->Release();
     if(ivmrffdshow9)
      {
       ivmrffdshow9->support_ffdshow();
       ivmrffdshow9->Release();
       isVMR9rs= false; // patched VMR9 Renderless
      }
     else
      {
       isVMR9rs= true;
      }
    }
  }
 return isVMR9rs;
}

bool TffdshowDecVideo::IsVMR9Renderless(IPin *downstream_input_pin)
{
 bool result=false;
 PIN_INFO pininfo;
 FILTER_INFO filterinfo;
 downstream_input_pin->QueryPinInfo(&pininfo);
 if (pininfo.pFilter)
  {
   pininfo.pFilter->QueryFilterInfo(&filterinfo);
   if (wcsncmp(L"Video Mixing Render 9 (Renderless)",filterinfo.achName,34)==0)
    result=true;
   if (filterinfo.pGraph)
    filterinfo.pGraph->Release();
   pininfo.pFilter->Release();
  }
 return result;
}

STDMETHODIMP TffdshowDecVideo::deliverPreroll(int frametype)
{
 // Maybe we're waiting for a keyframe still?
 if (waitForKeyframe && (frametype&FRAME_TYPE::typemask)==FRAME_TYPE::I)
  waitForKeyframe=FALSE;
 // if so, then we don't want to pass this on to the renderer
 //if (waitForKeyframe)
 // DPRINTF(_l("still waiting for a keyframe"));
 return S_FALSE;
}

STDMETHODIMP TffdshowDecVideo::flushDecodedSamples(void)
{
 TffPict pict;
 return deliverDecodedSample(pict);
}

STDMETHODIMP TffdshowDecVideo::deliverDecodedSample(TffPict &pict)
{
 // Maybe we're waiting for a keyframe still?
 if (waitForKeyframe && (pict.frametype&FRAME_TYPE::typemask)==FRAME_TYPE::I)
  waitForKeyframe=FALSE;
 // if so, then we don't want to pass this on to the renderer
 if (waitForKeyframe)
  {
   //DPRINTF(_l("still waiting for a keyframe"));
   m_rtStart += inpin->avgTimePerFrame;
   return S_OK;
  }

 //if (m_frame.srcLength==0) return S_FALSE;

 HRESULT frameTimeOk=S_FALSE;
 int codecId = inpin->getInCodecId2();
 if (mpeg12_codec(codecId) && inpin->biIn.bmiHeader.biCompression!=FOURCC_MPEG)
  {
   frameTimeOk=S_OK;
  }
 else
  if (inpin->sourceFlags&TvideoCodecDec::SOURCE_NEROAVC && pict.rtStart!=REFTIME_INVALID && pict.rtStop==REFTIME_INVALID)
   {
    pict.rtStop=pict.rtStart+inpin->avgTimePerFrame;
    frameTimeOk=S_OK;
   }

 if (frameTimeOk != S_OK)
  frameTimeOk=(pict.rtStart!=REFTIME_INVALID) ? S_OK : S_FALSE;

 if (globalSettings->autodetect24P && vc1_codec(codecId) && inSampleEverField1Repeat && inpin->avgTimePerFrame == 333666)
  {
   if (insample_rtStart != REFTIME_INVALID)
    {
     vc1rtStart = insample_rtStart;
     pict.rtStop = pict.rtStart + 417082;
     m_rtStart = pict.rtStop + 1;
     vc1frameCnt = 0;
    }
   else
    {
     pict.rtStart = vc1rtStart + vc1frameCnt * 417083;
     pict.rtStop = pict.rtStart + 417082;
     m_rtStart = pict.rtStop + 1;
    }
  }
 else
  {
   if (frameTimeOk==S_OK && pict.rtStop-pict.rtStart!=0)
    {
     if (inpin->avgTimePerFrame==0)
      inpin->avgTimePerFrame=pict.rtStop-pict.rtStart;
     m_rtStart = pict.rtStart + inpin->avgTimePerFrame;
    }
   else
    {
     frameTimeOk=S_OK;
     pict.rtStart = m_rtStart;
     if (inSampleTypeSpecificFlags & AM_VIDEO_FLAG_REPEAT_FIELD)
      {
       // field 1 repeat flag. Haali's splitter use this for interlaced VC1 in EVO.
       pict.rtStop = pict.rtStart + inpin->avgTimePerFrame + (inpin->avgTimePerFrame >> 1) - 1;
       m_rtStart += inpin->avgTimePerFrame + (inpin->avgTimePerFrame >> 1);
       inSampleEverField1Repeat = true;
      }
     else
      {
       pict.rtStop = pict.rtStart + inpin->avgTimePerFrame - 1;
       m_rtStart += inpin->avgTimePerFrame;
      }
    }
  }

 if ((inSampleEverField1Repeat || inpin->isInterlacedRawVideo) && !(inSampleTypeSpecificFlags & AM_VIDEO_FLAG_WEAVE))
  {
   if (inSampleTypeSpecificFlags & AM_VIDEO_FLAG_FIELD1FIRST)
    pict.fieldtype = FIELD_TYPE::INT_TFF;
   else
    pict.fieldtype = FIELD_TYPE::INT_BFF;
  }

 //LONGLONG mediaTime1=-1,mediaTime2=-1;
 //HRESULT mediaTimeOk=pIn->GetMediaTime(&mediaTime1,&mediaTime2);
 if (pict.mediatimeStart!=REFTIME_INVALID)
  currentFrame=(unsigned long)pict.mediatimeStart;
 else if (frameTimeOk==S_OK && inpin->avgTimePerFrame)
  currentFrame=long((pict.rtStart+segmentStart)/(inpin->avgTimePerFrame*1.0)+0.5);
 else
  currentFrame++;

 vc1frameCnt++;

 int videoDelay;
 if (moviesecs>0 && presetSettings->isVideoDelayEnd && presetSettings->videoDelay!=presetSettings->videoDelayEnd)
  {
   unsigned int msecs;
   if (SUCCEEDED(getCurrentFrameTimeMS(&msecs)))
    videoDelay=msecs*(presetSettings->videoDelayEnd-presetSettings->videoDelay)/(moviesecs*1000)+presetSettings->videoDelay;
   else
    videoDelay=presetSettings->videoDelay;
  }
 else
  videoDelay=presetSettings->videoDelay;

 if (videoDelay)
  {
   REFERENCE_TIME delay100ns=videoDelay*10000LL;
   pict.rtStart+=delay100ns;
   pict.rtStop +=delay100ns;
  }
 pict.rtStart+=segmentStart;
 pict.rtStop+=segmentStart;

 clock_t t=clock();
 decodingFps=(t!=lastTime)?1000*CLOCKS_PER_SEC/(t-lastTime):0;
 lastTime=t;

 if (pict.csp!=FF_CSP_NULL) decodingCsp=pict.csp;

 if (!cpu && cpus==-1)
  {
   cpu=new TcpuUsage;
   cpus=cpu->GetCPUCount();
   if (cpus==0) {delete cpu;cpu=NULL;};
  }

 if (!imgFilters) imgFilters=createImgFilters();
 if (wasSubtitleResetTime) imgFilters->subtitleResetTime=pict.rtStart;
 // buffer recent four timestamps for frame rate calculation
 decoded_rtStarts[count_decoded_frames_for_framerate_calculation & 3] = pict.rtStart;
 if (count_decoded_frames_for_framerate_calculation == 7)
  eighth_decoded_rtStart = pict.rtStart;
 count_decoded_frames_for_framerate_calculation++;
 return imgFilters->process(pict,presetSettings);
}

STDMETHODIMP TffdshowDecVideo::deliverProcessedSample(TffPict &pict)
{
 if (pict.csp==FF_CSP_NULL)
  return S_OK;

 REFERENCE_TIME rtStart=pict.rtStart-segmentStart;
 REFERENCE_TIME rtStop=rtStart+1;

 int codecId = inpin->getInCodecId2();
 if (mpeg12_codec(codecId) && inpin->biIn.bmiHeader.biCompression!=FOURCC_MPEG && rtStart < 0)
  return S_OK; // work around compatibility issue with splitter?

 sendOnFrameMsg();

 if (presetSettings->output->hwOverlayAspect)
  pict.setDar(Rational(presetSettings->output->hwOverlayAspect>>8,256));
 if (!outdv && allowOutChange)
  {
   HRESULT hr=reconnectOutput(pict);
   if (FAILED(hr))
    return S_FALSE;//hr;
  }

 segmentFrameCnt++;
 frameCnt++;

 comptr<IMediaSample> pOut=NULL;
 HRESULT hr=initializeOutputSample(&pOut);
 if (FAILED(hr))
  return hr;

if (!outdv && hwDeinterlace)
  if (comptrQ<IMediaSample2> pOut2=pOut)
   {
    AM_SAMPLE2_PROPERTIES outProp2;
    if (SUCCEEDED(pOut2->GetProperties(FIELD_OFFSET(AM_SAMPLE2_PROPERTIES,tStart),(PBYTE)&outProp2)))
     {
      // Set interlace information (every sample)
      outProp2.dwTypeSpecificFlags=AM_VIDEO_FLAG_INTERLEAVED_FRAME;

      // Force weave
      if (presetSettings->output->hwDeintMethod == 1)
       {
        outProp2.dwTypeSpecificFlags|=AM_VIDEO_FLAG_WEAVE;
       }
      // Force bob
      else if (presetSettings->output->hwDeintMethod == 2)
       {
        // force TFF
        if (presetSettings->output->hwDeintFieldOrder == 1)
         {
          outProp2.dwTypeSpecificFlags|=AM_VIDEO_FLAG_FIELD1FIRST;
         }
        // auto field order
        else if (presetSettings->output->hwDeintFieldOrder == 0)
         {
          if (pict.fieldtype&FIELD_TYPE::INT_TFF 
             && !(pict.fieldtype & FIELD_TYPE::PROGRESSIVE_FRAME) // avoid sending rapidly alternating TFF and BFF if telecine is detected.
             )
           {
            outProp2.dwTypeSpecificFlags|=AM_VIDEO_FLAG_FIELD1FIRST;
           }
         }
       }
      // Auto
      else
       {
        if (pict.fieldtype & FIELD_TYPE::PROGRESSIVE_FRAME)
         {
          outProp2.dwTypeSpecificFlags|=AM_VIDEO_FLAG_WEAVE;
         }
        else
         {
          // force TFF
          if (presetSettings->output->hwDeintFieldOrder == 1)
           {
            outProp2.dwTypeSpecificFlags|=AM_VIDEO_FLAG_FIELD1FIRST;
           }
          // auto field order
          else if (presetSettings->output->hwDeintFieldOrder == 0)
           {
            if (pict.fieldtype&FIELD_TYPE::INT_TFF)
             {
              outProp2.dwTypeSpecificFlags|=AM_VIDEO_FLAG_FIELD1FIRST;
             }
           }
         }
       }
      pOut2->SetProperties(FIELD_OFFSET(AM_SAMPLE2_PROPERTIES,dwStreamId),(PBYTE)&outProp2);
     }
   }

 m_bSampleSkipped=FALSE;
 // The renderer may ask us to on-the-fly to start transforming to a
 // different format.  If we don't obey it, we'll draw garbage
 AM_MEDIA_TYPE *pmtOut;
 pOut->GetMediaType(&pmtOut);
 if (pmtOut!=NULL && pmtOut->pbFormat!=NULL)
  {
   // spew some debug output
   ASSERT(!IsEqualGUID(pmtOut->majortype, GUID_NULL));
   // now switch to using the new format.  I am assuming that the
   // derived filter will do the right thing when its media type is
   // switched and streaming is restarted.
   StopStreaming();
   m_pOutput->CurrentMediaType() = *pmtOut;
   DeleteMediaType(pmtOut);
   hr = StartStreaming();
   if (SUCCEEDED(hr))
    {
     // a new format, means a new empty buffer, so wait for a keyframe
     // before passing anything on to the renderer.
     // !!! a keyframe may never come, so give up after 30 frames
     DPRINTF(_l("Output format change /*means we must wait for a keyframe*/"));
     //waitForKeyframe = 30;
    }
   else // if this fails, playback will stop, so signal an error
    {
     //  Must release the sample before calling AbortPlayback
     //  because we might be holding the win16 lock or
     //  ddraw lock
     abortPlayback(hr);
     return hr;
    }
  }

 AM_MEDIA_TYPE *mtOut=NULL;
 pOut->GetMediaType(&mtOut);
 if (mtOut!=NULL)
  {
   hr=setOutputMediaType(*mtOut);
   DeleteMediaType(mtOut);
   if (hr!=S_OK)
    return hr;
  }

 if (m_NeedToAttachFormat) // code imported from DScaler. Copyright (c) 2004 John Adcock
  {
   m_NeedToAttachFormat = false;
   AM_MEDIA_TYPE omt = m_pOutput->CurrentMediaType();
   if (omt.formattype==FORMAT_VideoInfo2)
    {
     VIDEOINFOHEADER2* vih = (VIDEOINFOHEADER2*)omt.pbFormat;
     BITMAPINFOHEADER *bmi=&vih->bmiHeader;
     setVIH2aspect(vih, pict.rectFull, 0);
     SetRect(&vih->rcTarget, 0, 0, 0, 0);
     SetRect(&vih->rcSource, 0, 0, pict.rectFull.dx, pict.rectFull.dy);
     bmi->biXPelsPerMeter = pict.rectFull.dx * vih->dwPictAspectRatioX;
     bmi->biYPelsPerMeter = pict.rectFull.dy * vih->dwPictAspectRatioY;
     pOut->SetMediaType(&omt);
     comptrQ<IMediaEventSink> pMES=graph;
     NotifyEvent(EC_VIDEO_SIZE_CHANGED, MAKELPARAM(pict.rectFull.dx, pict.rectFull.dy), 0);
    }
  }

 int sync=(pict.frametype&FRAME_TYPE::typemask)==FRAME_TYPE::I?TRUE:FALSE;
 pOut->SetSyncPoint(sync);
 if (outOverlayMixer)
  pOut->SetDiscontinuity(TRUE);

 if (rtStart!=REFTIME_INVALID)
  {
   rtStop=pict.rtStop-segmentStart;
   pOut->SetTime(&rtStart,&rtStop);
  }
 if (pict.mediatimeStart!=REFTIME_INVALID)
  pOut->SetMediaTime(&pict.mediatimeStart,&pict.mediatimeStop);

 unsigned char *dst;
 if (pOut->GetPointer(&dst)!=S_OK)
  return S_FALSE;
 LONG dstSize=pOut->GetSize();
 HRESULT cr=imgFilters->convertOutputSample(pict,m_frame.dstColorspace,&dst,&m_frame.dstStride,dstSize,presetSettings->output);
 pOut->SetActualDataLength(cr==S_FALSE?dstSize:m_frame.dstSize);
 update_time_on_ffdshow3(rtStart, rtStop);

 hr= m_pOutput->Deliver(pOut);

 update_time_on_ffdshow4();

 if (m_NeedToPauseRun && graph) // Work around Windows Media Center
  {
   DPRINTF(_l("Work around Windows Media Center. After reconnecting Pause and Run."));
   m_NeedToPauseRun = false;
   IMediaFilter *imediafilter;
   graph->QueryInterface(IID_IMediaFilter, (void**)(&imediafilter));
   if (imediafilter)
    {
     imediafilter->Pause(); // Why would this be required? Microsoft should fix their application.
     imediafilter->Run(0);
     imediafilter->Release();
    }
  }

 return hr;
}

HRESULT TffdshowDecVideo::onGraphRemove(void)
{
 if (videoWindow) {videoWindow=NULL;wasVideoWindow=false;}
 if (basicVideo) {basicVideo=NULL;wasBasicVideo=false;}
 if (imgFilters) delete imgFilters;imgFilters=NULL;
 if (inputConnectedPin != NULL)
 {
	 inputConnectedPin->Release();
	 inputConnectedPin = NULL;
 }
 return TffdshowDec::onGraphRemove();
}

STDMETHODIMP TffdshowDecVideo::Run(REFERENCE_TIME tStart)
{
 DPRINTF(_l("TffdshowDecVideo::Run thread=%d"),GetCurrentThreadId());
 if (!wasVideoWindow)
  {
   wasVideoWindow=true;
   if (SUCCEEDED(m_pGraph->QueryInterface(IID_IVideoWindow,(void**)&videoWindow)))
    videoWindow->Release();
  }
 if (!wasBasicVideo)
  {
   wasBasicVideo=true;
   if (SUCCEEDED(m_pGraph->QueryInterface(IID_IBasicVideo,(void**)&basicVideo)))
    basicVideo->Release();
  }
 return CTransformFilter::Run(tStart);
}
STDMETHODIMP TffdshowDecVideo::Stop(void)
{
 DPRINTF(_l("TffdshowDecVideo::Stop thread=%d"),GetCurrentThreadId());
 m_dirtyStop = true;
 if (hReconnectEvent)
  SetEvent(hReconnectEvent);
 if (videoWindow) {videoWindow=NULL;wasVideoWindow=false;}
 if (basicVideo) {basicVideo=NULL;wasBasicVideo=false;}
 return TffdshowDec::Stop();
}

void TffdshowDecVideo::lockReceive(void)
{
 //DPRINTF(_l("TffdshowDecVideo::lockReceive thread=%d"),GetCurrentThreadId());
 m_csReceive.Lock();
}
void TffdshowDecVideo::unlockReceive(void)
{
 //DPRINTF(_l("TffdshowDecVideo::unlockReceive thread=%d"),GetCurrentThreadId());
 m_csReceive.Unlock();
}

HRESULT TffdshowDecVideo::NewSegment(REFERENCE_TIME tStart,REFERENCE_TIME tStop,double dRate)
{
 DPRINTF(_l("TffdshowDecVideo::NewSegment thread=%d"),GetCurrentThreadId());
 OSD_time_on_ffdshowStart=0;
 OSD_time_on_ffdshowBeforeGetBuffer=0;
 OSD_time_on_ffdshowAfterGetBuffer=0;
 OSD_time_on_ffdshowEnd=0;
 OSD_time_on_ffdshowResult=0;
 OSD_time_on_ffdshowOldStart=0;
 OSD_time_on_ffdshowDuration=0;
 segmentStart=tStart;
 segmentFrameCnt=0;
 vc1frameCnt = 0;
 vc1rtStart=0;
 m_rtStart = 0;
 count_decoded_frames_for_framerate_calculation = 0;
 for (size_t i=0;i<textpins.size();i++)
  if (textpins[i]->needSegment)
   textpins[i]->NewSegment(tStart,tStop,dRate);
 late=lastrtStart=0;
 frameCnt=0;bytesCnt=0;
 return TffdshowDec::NewSegment(tStart,tStop,dRate);
}

HRESULT TffdshowDecVideo::EndOfStream(void)
{
 if (imgFilters)
  imgFilters->onEndOfStream();
 return TffdshowDec::EndOfStream();
}

// not used. compatibitliy for IffdshowDecVideo
STDMETHODIMP TffdshowDecVideo::findOverlayControl(IMixerPinConfig2* *overlayPtr)
{
 if (!overlayPtr) return E_POINTER;
 *overlayPtr=NULL;
 if (!m_pGraph) return E_UNEXPECTED;
 return searchPinInterface(m_pGraph,IID_IMixerPinConfig2,(IUnknown**)overlayPtr)?S_OK:S_FALSE;
}

struct TvmrInterfaceCmp
{
private:
 const IID &iid;
public:
 mutable comptr<IVMRMixerControl9> vmr9;mutable int id;
 TvmrInterfaceCmp(const IID &Iiid):iid(Iiid),vmr9(NULL),id(-1) {}
 bool operator()(IBaseFilter *f,IPin *ipin) const
  {
   if (FAILED(f->QueryInterface(iid,(void**)&vmr9)) || vmr9==NULL)
    return false;
   comptr<IEnumPins> epi;
   if (f->EnumPins(&epi)==S_OK)
    {
     epi->Reset();id=0;
     for (comptr<IPin> bpi;epi->Next(1,&bpi,NULL)==S_OK;bpi=NULL,id++)
      {
       comptr<IPin> cpin;bpi->ConnectedTo(&cpin);
       if (ipin==cpin)
        return true;
      }
    }
   return false;
  }
};

STDMETHODIMP TffdshowDecVideo::findOverlayControl2(IhwOverlayControl* *overlayPtr)
{
 if (!overlayPtr) return E_POINTER;
 *overlayPtr=NULL;
 if (m_pGraph)
  {
   comptr<IMixerPinConfig2> overlay;
   if (!searchInterfaceInGraph)
    searchInterfaceInGraph = new TsearchInterfaceInGraph(m_pGraph, IID_IMixerPinConfig2, searchPinInterface);
   if (searchInterfaceInGraph && searchInterfaceInGraph->getResult((IUnknown**)&overlay) && overlay)
    {
     (*overlayPtr=new ThwOverlayControlOverlay(overlay))->AddRef();
     return S_OK;
    }
   else
    {
     TvmrInterfaceCmp vmr9comp(IID_IVMRMixerControl9);
     if (searchPrevNextFilter(PINDIR_OUTPUT,m_pOutput,m_pOutput,NULL,vmr9comp))
      {
       (*overlayPtr=new ThwOverlayControlVMR9(vmr9comp.vmr9,vmr9comp.id))->AddRef();
       return S_OK;
      }
    }
   if (searchInterfaceInGraph && !searchInterfaceInGraph->waitSucceeded())
    {
     return E_FAIL;
    }
  }
 (*overlayPtr=new ThwOverlayControlBase)->AddRef();
 return S_FALSE;
}

int TffdshowDecVideo::GetPinCount(void)
{
 return int(2+textpins.size()+(textpins.size()==textpins.getNumConnectedInpins()?1:0));
}
CBasePin* TffdshowDecVideo::GetPin(int n)
{
 if (n==0)
  return m_pInput;
 else if (n==1)
  return m_pOutput;
 else
  {
   n-=2;
   if (n<(int)textpins.size())
    return textpins[n];
   else
    {
     wchar_t name[50];
     if (n==0)
      swprintf(name,L"In Text");
     else
      swprintf(name,L"In Text %i",n+1);
     HRESULT phr=0;
     TtextInputPin *textpin=new TtextInputPin(this,&phr,name,n+1);
     if (FAILED(phr)) return NULL;
     textpins.push_back(textpin);
     return textpin;
    }
  }
}
STDMETHODIMP TffdshowDecVideo::FindPin(LPCWSTR Id,IPin **ppPin)
{
 CheckPointer(ppPin,E_POINTER);

 if (lstrcmpW(Id,m_pInput->Name())==0)
  *ppPin=m_pInput;
 else if (lstrcmpW(Id,m_pOutput->Name())==0)
  *ppPin=m_pOutput;
 else
  *ppPin=textpins.find(Id);

 if (*ppPin)
  {
   (*ppPin)->AddRef();
   return S_OK;
  }
 else
  return VFW_E_NOT_FOUND;
}
HRESULT TffdshowDecVideo::reconnectOutput(const TffPict &newpict)
{
 HRESULT hr=S_OK;
 if ((newpict.rectFull==oldRect && newpict.rectFull.sar!=oldRect.sar)
      && presetSettings->output->hwOverlay==0)
  {
   // Only aspect ratio is different and "Set pixel aspect ratio in output media type" is unchecked.
   oldRect=newpict.rectFull;
   return S_OK;
  }
 if ((newpict.rectFull==oldRect && newpict.rectFull.sar!=oldRect.sar)
      && _strnicmp(_l("wmplayer.exe"),getExeflnm(),13)!=0
      && (downstreamID == OVERLAY_MIXER || (dvdproc &&  (downstreamID ==  VMR7 || downstreamID == VMR9 || downstreamID == VMR9RENDERLESS_MPC))))
  {
   m_NeedToAttachFormat = true;
   oldRect=newpict.rectFull;
   return S_OK;
  }

 if (newpict.rectFull!=oldRect || newpict.rectFull.sar!=oldRect.sar)
  {
   DPRINTF(_l("TffdshowDecVideo::reconnectOutput"));

   if (!m_pOutput->IsConnected())
    return VFW_E_NOT_CONNECTED;

   inReconnect=true;
   int newdy=newpict.rectFull.dy;
   if (newpict.cspInfo.id==FF_CSP_420P)
    newdy=ODD2EVEN(newdy);
   reconnectRect=newpict;
   reconnectRect.rectFull.dy=newdy;
   CMediaType &mt=m_pOutput->CurrentMediaType();
   BITMAPINFOHEADER *bmi=NULL;
   if (mt.formattype==FORMAT_VideoInfo)
    {
     VIDEOINFOHEADER *vih=(VIDEOINFOHEADER*)mt.Format();
     SetRect(&vih->rcSource,0,0,newpict.rectFull.dx,newdy);
     SetRect(&vih->rcTarget,0,0,newpict.rectFull.dx,newdy);
     bmi=&vih->bmiHeader;
     //bmi->biXPelsPerMeter = m_win * m_aryin;
     //bmi->biYPelsPerMeter = m_hin * m_arxin;
    }
   else if (mt.formattype==FORMAT_VideoInfo2)
    {
     VIDEOINFOHEADER2* vih=(VIDEOINFOHEADER2*)mt.Format();
     //DPRINTF(_l("AR in: %i:%i"),vih->dwPictAspectRatioX,vih->dwPictAspectRatioY);

     SetRect(&vih->rcSource,0,0,newpict.rectFull.dx,newdy);
     SetRect(&vih->rcTarget,0,0,newpict.rectFull.dx,newdy);
     bmi=&vih->bmiHeader;

     //DPRINTF(_l("AR pict: %i:%i"),newpict.rectFull.dar().num,newpict.rectFull.dar().den);

     //vih->dwControlFlags=AMCONTROL_USED | AMCONTROL_COLORINFO_PRESENT | (DXVA_NominalRange_Wide << DXVA_NominalRangeShift);
     setVIH2aspect(vih,newpict.rectFull,presetSettings->output->hwOverlayAspect);
     if (presetSettings->output->hwOverlay==0)
      {
       vih->dwPictAspectRatioX=newpict.rectFull.dx;
       vih->dwPictAspectRatioY=newdy;
       vih->dwControlFlags=0;
      }
     if(presetSettings->resize && presetSettings->resize->is && presetSettings->resize->SARinternally && presetSettings->resize->mode==0)
      {
       vih->dwPictAspectRatioX= newpict.rectFull.dx;
       vih->dwPictAspectRatioY= newpict.rectFull.dy;
      }

     //DPRINTF(_l("AR set: %i:%i"),vih->dwPictAspectRatioX,vih->dwPictAspectRatioY);
    }

   bmi->biWidth=newpict.rectFull.dx;
   bmi->biHeight=newdy;
   bmi->biSizeImage=newpict.rectFull.dx*newdy*bmi->biBitCount>>3;

   FILTER_INFO filtInfo;
   IPinConnection* ipinConnection= NULL;  // if this is not supported, dynamic reconnect fails.
   IGraphConfig* igraphConfig= NULL;      // To reconnect dynamicly. We need to re-negotiate cBuffers.
   IVMRVideoStreamControl9* iVmrSC9= NULL;
   BOOL isVMR9Active= true;
   m_pOutputDecVideo->DeliverBeginFlush();
   m_pOutputDecVideo->DeliverEndFlush();
   m_pOutputDecVideo->SendAnyway();

   QueryFilterInfo(&filtInfo);
   if (filtInfo.pGraph)
    {
     filtInfo.pGraph->QueryInterface(IID_IGraphConfig, (void **)(&igraphConfig));
     filtInfo.pGraph->Release();
    }
   m_pOutput->GetConnected()->QueryInterface(IID_IPinConnection, (void**)(&ipinConnection));
   m_pOutput->GetConnected()->QueryInterface(IID_IVMRVideoStreamControl9, (void**)(&iVmrSC9));
   if (iVmrSC9)
    {
     iVmrSC9->GetStreamActiveState(&isVMR9Active);
     iVmrSC9->Release();
    }
   bool overlayYUY2=downstreamID==OVERLAY_MIXER && m_frame.dstColorspace==FF_CSP_YUY2;
   if (!overlayYUY2)
    {
     hr= m_pOutput->GetConnected()->QueryAccept(&mt);
     if (SUCCEEDED(hr))
      {
       int retry=ppropActual.cBuffers*10; // Read comments in DecideBufferSizeVMR for the reason to retry.
       do {
        hr= m_pOutput->GetConnected()->ReceiveConnection(m_pOutput, &mt);
        if (FAILED(hr)) Sleep(10);
       } while (FAILED(hr) && retry-->0);
      }
    }

   int isDynamicTried=false;
   if (overlayYUY2 || (FAILED(hr) && isQueue))  // try dynamic reconnect to re-negotiate cBuffers.
    {
     DPRINTF(_l("try dynamic reconnect."));
     if (ipinConnection && igraphConfig)
      {
       hr= ipinConnection->DynamicQueryAccept(&mt);
       if (SUCCEEDED(hr))
        {
         isDynamicTried=true;
         hReconnectEvent= CreateEvent(NULL, false, false, NULL);
         hr= igraphConfig->Reconnect((IPin *)m_pOutput, m_pOutput->GetConnected(),&mt,NULL,hReconnectEvent ,AM_GRAPH_CONFIG_RECONNECT_DIRECTCONNECT);
         if (SUCCEEDED(hr))
          m_pOutputDecVideo->GetAllocator()->Commit();
         else
          DPRINTF(_l("Reconnect failed."));
         CloseHandle(hReconnectEvent);
         hReconnectEvent= NULL;
        }
      }
    }

   if (ipinConnection)
    ipinConnection->Release();
   if (igraphConfig)
    igraphConfig->Release();

   if (FAILED(hr))
    {
     if (reconnectFirstError && isDynamicTried)
      {
       MessageBox(NULL,_l("Reconnect failed.\nPlease restart the video application."),_l("ffdshow error"),MB_ICONERROR|MB_OK);
       reconnectFirstError=false;
      }
     inReconnect=false;
     return hr;
    }

   comptr<IMediaSample> pOut;
   if (SUCCEEDED(m_pOutput->GetDeliveryBuffer(&pOut,NULL,NULL,0)))
    {
     AM_MEDIA_TYPE *opmt;
     if (SUCCEEDED(pOut->GetMediaType(&opmt)) && opmt)
      {
       CMediaType omt=*opmt;

       //VIDEOINFOHEADER2* vih=(VIDEOINFOHEADER2*)omt.Format();
       //DPRINTF(_l("AR out: %i:%i"),vih->dwPictAspectRatioX,vih->dwPictAspectRatioY);

       m_pOutput->SetMediaType(&omt);
       DeleteMediaType(opmt);
      }
     else // stupid overlay mixer won't let us know the new pitch...
      {
       long size=pOut->GetSize();
       m_frame.dstStride=bmi->biWidth=size/bmi->biHeight*8/bmi->biBitCount;
      }
    }

#if 0
   comptrQ<IMixerPinConfig> imixer=m_pOutput->GetConnected();
   AM_ASPECT_RATIO_MODE arMode;
   if (imixer)
    {
     HRESULT hr1=imixer->GetAspectRatioMode(&arMode);
    }

   if (downstreamID==OVERLAY_MIXER)
    {
     IBasicVideo2 *ibv=NULL;
     graph->QueryInterface(IID_IBasicVideo2, (void **)(&ibv));
     if (ibv)
      {
       HRESULT hr2=ibv->SetSourcePosition(newpict.rectFull.x,newpict.rectFull.y,newpict.rectFull.dx,newdy);
       ibv->Release();
      }
    }
#endif

   if (iVmrSC9) // without this, the old renderer(tested with i82865G) sometimes hang up for unknown reason.
    {
     m_pOutput->GetConnected()->QueryInterface(IID_IVMRVideoStreamControl9, (void**)(&iVmrSC9));
     if (iVmrSC9)
      {
       iVmrSC9->SetStreamActiveState(isVMR9Active);  // without this VMR9(windowed)/MPC black out.
       iVmrSC9->Release();
      }
    }

   // some renderers don't send this
   NotifyEvent(EC_VIDEO_SIZE_CHANGED,MAKELPARAM(newpict.rectFull.dx,newdy),0);
   oldRect=newpict.rectFull;
   inReconnect=false;
   if(_strnicmp(_l("ehshell.exe"),getExeflnm(),12) == 0)
    m_NeedToPauseRun=true;
   return S_OK;
  }
 return S_FALSE;
}


HRESULT TffdshowDecVideo::AlterQuality(Quality q)
{
 late=q.Late;
 return S_FALSE;
}

HRESULT TffdshowDecVideo::initializeOutputSample(IMediaSample **ppOutSample)
{
 if(!m_pOutput->IsConnected())
  return VFW_E_NOT_CONNECTED;
 // default - times are the same
 AM_SAMPLE2_PROPERTIES * const pProps=m_pInput->SampleProps();
 DWORD dwFlags=m_bSampleSkipped?AM_GBF_PREVFRAMESKIPPED:0;

 // This will prevent the image renderer from switching us to DirectDraw
 // when we can't do it without skipping frames because we're not on a
 // keyframe.  If it really has to switch us, it still will, but then we
 // will have to wait for the next keyframe

 //if (!(pProps->dwSampleFlags&AM_SAMPLE_SPLICEPOINT))
 // dwFlags|=AM_GBF_NOTASYNCPOINT;

 //ASSERT(m_pOutput->m_pAllocator != NULL);
 IMediaSample *pOutSample;
 HRESULT hr;
 update_time_on_ffdshow1();

 hr=m_pOutputDecVideo->GetDeliveryBuffer(&pOutSample,
                                pProps->dwSampleFlags&AM_SAMPLE_TIMEVALID?&pProps->tStart:NULL,
                                pProps->dwSampleFlags&AM_SAMPLE_STOPVALID?&pProps->tStop :NULL,
                                dwFlags);

 update_time_on_ffdshow2();

 if (FAILED(hr))
  {
   DPRINTF(_l(" TffdshowDecVideo::initializeOutputSample GetDeliveryBuffer failed %x"),hr);
   return hr;
  }
 *ppOutSample=pOutSample;

 ASSERT(pOutSample);
 setPropsTime(pOutSample,pProps->tStart,pProps->tStop,pProps,&m_bSampleSkipped);
 return S_OK;
}

void TffdshowDecVideo::setSampleSkipped(bool sendSkip)
{
 //DPRINTF(_l("dropframe"));
 m_bSampleSkipped=TRUE;
 if (sendSkip && inpin) inpin->setSampleSkipped();
}

int TffdshowDecVideo::IsQueueListedApp(const char_t *exe)
{
 if (!presetSettings) initPreset();
 if(presetSettings->useQueueOnlyIn)
  {
   strings queuelistList;
   strtok(presetSettings->useQueueOnlyInList,_l(";"),queuelistList);
   for (strings::const_iterator b=queuelistList.begin();b!=queuelistList.end();b++)
    if (DwStrcasecmp(*b,exe)==0)
     return 1;
   return 0;
  }
 else
  {
   if (_strnicmp(_l("wmplayer.exe"),exe,13)==0)
    return 0;
   return 1;
  }
}

void TffdshowDecVideo::DPRINTF_SampleTime(IMediaSample* pSample)
{
 REFERENCE_TIME TimeStart;
 REFERENCE_TIME TimeEnd;
 pSample->GetTime(&TimeStart, &TimeEnd);
 DPRINTF(_l(" tStart %7.0f, tEnd %7.0f"), TimeStart/10000.0, TimeEnd/10000.0);
}

void TffdshowDecVideo::set_downstreamID(IPin *downstream_input_pin)
{
 downstreamID=UNKNOWN;
 const CLSID &ref=GetCLSID(downstream_input_pin);
 if (ref==CLSID_VideoRenderer)        downstreamID=OLD_RENDERER;
 if (ref==CLSID_OverlayMixer)         downstreamID=OVERLAY_MIXER;
 if (ref==CLSID_VideoMixingRenderer)  downstreamID=VMR7;
 if (ref==CLSID_HaaliVideoRenderer)   downstreamID=HAALI_RENDERER;
 if (ref==CLSID_VideoMixingRenderer9)
  if (IsVMR9Renderless(downstream_input_pin))
   downstreamID=VMR9RENDERLESS_MPC;
  else
   downstreamID=VMR9;
 if (ref==CLSID_DirectVobSubFilter || ref==CLSID_DirectVobSubFilter2) downstreamID=DVOBSUB;
}

int TffdshowDecVideo::get_trayIconType(void)
{
 switch (globalSettings->trayIconType)
  {
   case 2: return IDI_FFDSHOW;
   case 0:
   case 1:
   default:
   return IDI_MODERN_ICON_V;
  }
}

void TffdshowDecVideo::getChapters(void)
{
	comptr<IEnumFilters> eff;
	if (graph->EnumFilters(&eff)==S_OK)
	{
		eff->Reset();
		for (comptr<IBaseFilter> bff;eff->Next(1,&bff,NULL)==S_OK;bff=NULL)
		{
			char_t name[MAX_PATH],filtername[MAX_PATH];
			getFilterName(bff,name,filtername,countof(filtername));
			if (!strcmp(_l("ffdshow video decoder"), filtername)
				|| !strcmp(_l("ffdshow raw video filter"), filtername)
				|| !strcmp(_l("ffdshow VFW decoder helper"), filtername)
				|| !strcmp(_l("ffdshow subtitles filter"), filtername)
				|| !strcmp(_l("ffdshow Audio Decoder"), filtername))
				continue;
			IAMExtendedSeeking *pAMExtendedSeeking = NULL;
			bff->QueryInterface(IID_IAMExtendedSeeking, (void**) &pAMExtendedSeeking);
			if (pAMExtendedSeeking == NULL)
				continue;
			long markerCount = 0;
			pAMExtendedSeeking->get_MarkerCount(&markerCount);
			if (markerCount == 0)
			{
				pAMExtendedSeeking->Release();
				continue;
			}
			chaptersList.clear();
			
			// Retrieve the list of chapters and add them to the chaptersList
			for (int i=1; i<= markerCount; i++)
			{
				double markerTime = 0;
				BSTR markerName = NULL;
				pAMExtendedSeeking->GetMarkerTime(i, &markerTime);
				pAMExtendedSeeking->GetMarkerName(i, &markerName);
				if (markerName != NULL)
				{
					//char_t fMarkerName[MAX_PATH];
					//ff_strncpy(fMarkerName, markerName, strlen(fMarkerName));
 					
					//strcpy(fMarkerName, markerName);
					long markerTimeL = (long)markerTime;
					//chaptersList[markerTimeL]=fMarkerName;
					std::pair<long, ffstring> pair = std::make_pair<long, ffstring>(markerTimeL, markerName);
					chaptersList.push_back(pair);
					SysFreeString(markerName);
				}
				else
				{
					int hh, mm, ss;
					char_t time_str[30];
					hh = (int)(markerTime/3600);
					mm = (int)((markerTime - hh*3600)/60);
					ss = (int)(markerTime - hh*3600 - mm*60);
					tsprintf(time_str,_l("%02i:%02i:%02"), hh, mm, ss);
					long markerTimeL = (long)markerTime;
					std::pair<long, ffstring> pair = std::make_pair<long, ffstring>(markerTimeL, time_str);
					chaptersList.push_back(pair);
					//chaptersList[markerTimeL] = time_str;
				}			
				markerName = NULL;
			}
			pAMExtendedSeeking->Release();
			break;
		}
	}
}

STDMETHODIMP TffdshowDecVideo::get_MarkerCount(long * pMarkerCount)
{
	if (chaptersList.size() <= 1)
		getChapters();
	*pMarkerCount = (long)chaptersList.size();
	return S_OK;
}
STDMETHODIMP TffdshowDecVideo::get_CurrentMarker(long * pCurrentMarker)
{
	if (chaptersList.size() <= 1)
		getChapters();
	DWORD currentTime = GetTickCount();
	unsigned int i = 0;
	for (i=0; i<chaptersList.size(); i++)
	{
		if ((double)currentTime < chaptersList[i].first) break;
	}
	*pCurrentMarker = i;
	return S_OK;
}

// Markers start at position 1
STDMETHODIMP TffdshowDecVideo::GetMarkerTime(long MarkerNum, double * pMarkerTime)
{
	if (chaptersList.size() <= 1)
		getChapters();
	long l = 1;
	if (MarkerNum > (long)chaptersList.size())
		return S_FALSE;

	for (long l=0; (unsigned long)l<chaptersList.size(); l++)
	{
		if (l+1 == MarkerNum)
		{
			*pMarkerTime=(double)chaptersList[l].first;
			break;
		}
	}
	return S_OK;
}

// Markers start at position 1
STDMETHODIMP TffdshowDecVideo::GetMarkerName(long MarkerNum, BSTR * pbstrMarkerName)
{
	if (chaptersList.size() <= 1)
		getChapters();
	if (MarkerNum > (long)chaptersList.size())
		return S_FALSE;

	for (unsigned int i = 0;i<chaptersList.size();i++)
	{
		if (i+1 == (unsigned int)MarkerNum)
		{
			const char_t* markerName = chaptersList[i].second.c_str();
			//wcrtomb(
			size_t wlen = (chaptersList[i].second.size()+1)*sizeof(WCHAR);
			*pbstrMarkerName=(WCHAR*)CoTaskMemAlloc(wlen);memset(*pbstrMarkerName,0,wlen);
			nCopyAnsiToWideChar(*pbstrMarkerName,markerName);
			break;
		}
		i++;
	}
	return S_OK;
}

STDMETHODIMP TffdshowDecVideo::getChaptersList(void **ppChaptersList)
{
	if (chaptersList.size() <= 1)
		getChapters();
	*ppChaptersList = (void**) &chaptersList;
	return S_OK;
}

void TffdshowDecVideo::update_time_on_ffdshow1(void)
{
 if(isOSD_time_on_ffdshow && m_pClock)
  m_pClock->GetTime(&OSD_time_on_ffdshowBeforeGetBuffer);
}

void TffdshowDecVideo::update_time_on_ffdshow2(void)
{
 if(isOSD_time_on_ffdshow && m_pClock)
  m_pClock->GetTime(&OSD_time_on_ffdshowAfterGetBuffer);
}

void TffdshowDecVideo::update_time_on_ffdshow3(REFERENCE_TIME rtStart, REFERENCE_TIME rtStop)
{
 if(isOSD_time_on_ffdshow && m_pClock && rtStart!=REFTIME_INVALID)
  {
   m_pClock->GetTime(&OSD_time_on_ffdshowEnd);
   if(OSD_time_on_ffdshowFirstRun)
    {
     OSD_time_on_ffdshowFirstRun= false;
     OSD_time_on_ffdshowOldStart= rtStart;
     OSD_time_on_ffdshowDuration= 0;
     OSD_time_on_ffdshowResult=0;
    }
   else
    {
     OSD_time_on_ffdshowResult= OSD_time_on_ffdshowEnd
                               -OSD_time_on_ffdshowAfterGetBuffer
                               +OSD_time_on_ffdshowBeforeGetBuffer
                               -OSD_time_on_ffdshowStart;
     if(rtStop>rtStart+1)
      OSD_time_on_ffdshowDuration= rtStop-rtStart;
     else
      OSD_time_on_ffdshowDuration= rtStart-OSD_time_on_ffdshowOldStart;
     OSD_time_on_ffdshowOldStart= rtStart;
    }
  }
}

void TffdshowDecVideo::update_time_on_ffdshow4(void)
{
 if(isOSD_time_on_ffdshow && m_pClock)
   m_pClock->GetTime(&OSD_time_on_ffdshowStart);
}

#ifdef OSDTIMETABALE
/* Usage
 *
 * OSDTIMETABALE is defined in Tinfo.h.
 * Enable OSDTIMETABALE.
 *
 * This item is used to reserch time table on multithreading.
 *
 * Call OSDtimeStartSampling to start sampling.
 * Call OSDtimeEndSampling or OSDtimeEndSamplingMax at the end of sampling.
 * Open ffdshow dialog box and see Info & debug.
 *
 */

void TffdshowDecVideo::OSDtimeStartSampling(void)
{
 m_pClock->GetTime(&OSDtime1);
}

void TffdshowDecVideo::OSDtimeEndSampling(void)
{
 m_pClock->GetTime(&OSDtime2);
 if(OSDtime2>OSDlastdisplayed+1000000 && OSDtime2!=OSDtime1)
 {
  OSDtime3=OSDtime2-OSDtime1;
  OSDlastdisplayed=OSDtime2;
 }
}

void TffdshowDecVideo::OSDtimeEndSamplingMax(void)
{
 m_pClock->GetTime(&OSDtime2);
 if(OSDtime2-OSDtime1>OSDtimeMax)
  OSDtimeMax= OSDtime2-OSDtime1;
 if(OSDtime2>OSDlastdisplayed+5000000 && OSDtime2!=OSDtime1)
 {
  OSDtime3=OSDtimeMax;
  OSDtimeMax=0;
  OSDlastdisplayed=OSDtime2;
 }
}
#endif
