/*
 * Copyright (c) 2003-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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
#include "TffdshowEnc.h"
#include "Tconfig.h"
#include "TffdshowVideoInputPin.h"
#include "ffcodecs.h"
#include "ffdebug.h"
#include "TglobalSettings.h"
#include "lzo/minilzo.h"
#include "TffProc.h"
#include "TtrayIcon.h"
#include "TdialogSettings.h"
#include "reg.h"
#include "dsutil.h"
#include "resource.h"
#include "Tmuxer.h"
#include "Tinfo.h"
#include "2pass.h"
#include "Tconvert.h"
#include <mmintrin.h>
#include "TfakeImediaSample.h"

STDMETHODIMP_(int) TffdshowEnc::getVersion2(void)
{
 return VERSION;
}

CUnknown* WINAPI TffdshowEncDshow::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffdshowEnc *pNewObject=new TffdshowEncDshow(punk,phr,new TintStrColl);
 if (pNewObject==NULL) *phr=E_OUTOFMEMORY;
 return pNewObject;
}

CUnknown* WINAPI TffdshowEncVFW::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 TffdshowEncVFW *pNewObject=new TffdshowEncVFW(punk,phr,new TintStrColl);
 if (pNewObject==NULL) *phr=E_OUTOFMEMORY;
 return pNewObject;
}

STDMETHODIMP TffdshowEnc::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 CheckPointer(ppv,E_POINTER);
 if (riid==IID_IffdshowEnc)
  return GetInterface<IffdshowEnc>(this,ppv);
 else if (riid==IID_IffdshowBaseA)
  return GetInterface<IffdshowBaseA>(getBaseInterface<IffdshowBaseA>(),ppv);
 else if (riid==IID_IffdshowBaseW)
  return GetInterface<IffdshowBaseW>(getBaseInterface<IffdshowBaseW>(),ppv);
 else if (riid==IID_ISpecifyPropertyPages)
  return GetInterface<ISpecifyPropertyPages>(this,ppv);
 else if (riid==IID_IPersistStream)
  return GetInterface<IPersistStream>(this,ppv);
 else if (riid==IID_IffdshowParamsEnum)
  return GetInterface<IffdshowParamsEnum>(this,ppv);
 else
  return CTransformFilter::NonDelegatingQueryInterface(riid,ppv);
}

STDMETHODIMP TffdshowEncVFW::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 CheckPointer(ppv,E_POINTER);
 if (riid==IID_IffdshowEncVFW)
  return GetInterface<IffdshowEncVFW>(this,ppv);
 else
  return TffdshowEnc::NonDelegatingQueryInterface(riid,ppv);
}

TffdshowEnc::TffdshowEnc(LPUNKNOWN punk, HRESULT *phr,TintStrColl *Ioptions,const char_t *Iname,REFCLSID Iclsid,int Imode):
 TffdshowBase(
  punk,
  Ioptions,
  globalSettings=new TglobalSettingsEnc(&config,Imode,Ioptions),
  dialogSettings=new TdialogSettingsEnc(Ioptions),
  this,
  (TinputPin*&)inpin,
  m_pOutput,
  m_pGraph,
  IDS_FFDSHOWENC,IDI_FFVFW,
  defaultMerit
 ),
 CmyTransformFilter(Iname,punk,Iclsid),
 TffdshowVideo(this,m_pOutput,this,NULL),
 CPersistStream(punk,phr),
 inColorspace(FF_CSP_NULL)
{
 DPRINTF(_l("TffdshowEnc::Constructor"));
 setThreadName(DWORD(-1),"enc");
 SetDirty(TRUE);

 static const TintOptionT<TffdshowEnc> iopts[]=
  {
   IDFF_enc_fpsRate     ,&TffdshowEnc::fpsRate    , 1, 1,_l(""),0,NULL,0,
   IDFF_enc_fpsScale    ,&TffdshowEnc::fpsScale   , 1, 1,_l(""),0,NULL,0,
   IDFF_enc_ownStoreExt ,&TffdshowEnc::ownStoreExt, 0, 0,_l(""),0,NULL,0,
   IDFF_dlgEncGraph     ,&TffdshowEnc::dlgEncGraph, 0, 0,_l(""),0,NULL,0,
   IDFF_dlgEncAbout     ,&TffdshowEnc::dlgEncAbout, 0, 0,_l(""),0,NULL,0,
   IDFF_enc_working     ,&TffdshowEnc::working    ,-1,-1,_l(""),0,NULL,0,
   0
  };
 addOptions(iopts);

 m_pInput=inpin=new TffdshowVideoEncInputPin(this,phr);
 if (!m_pInput) *phr=E_OUTOFMEMORY;
 if (FAILED(*phr)) return;

 m_pOutput=new CTransformOutputPin(NAME("Transform output pin"),this,phr,L"Out");
 if (!m_pOutput) *phr=E_OUTOFMEMORY;
 if (FAILED(*phr))
  {
   delete m_pInput;m_pInput=NULL;
   return;
  }

 trayIconStart=&TtrayIconBase::start<TtrayIconEnc>;

 started=false;
 coSettings=NULL;
 fpsRate=25;fpsScale=1;
 ownStoreExt=false;
 dlgEncGraph=dlgEncAbout=0;

 memset(&ownpict,0,sizeof(ownpict));
 enc=NULL;mux=NULL;
 pass=NULL;
 working=false;firstrun=false;
 h_graph=NULL;
 convert=NULL;
 ffproc=NULL;
 oldCodecId=CODEC_ID_NONE;
 options=Ioptions;
}
TffdshowEnc::~TffdshowEnc()
{
 _release();
}

void TffdshowEnc::initCo(void)
{
 if (!coSettings)
  {
   coSettings=new TcoSettings(options);
   enclibs.init(this,this);
   coSettings->loadReg();
  }
}

STDMETHODIMP TffdshowEnc::_release(void)
{
 for (TvideoCodecs::iterator i=enclibs.begin();i!=enclibs.end();i++)
  delete *i;
 enclibs.clear();
 if (ffproc) delete ffproc;ffproc=NULL;
 if (coSettings) delete coSettings;coSettings=NULL;
 return S_OK;
}

STDMETHODIMP TffdshowEnc::GetPages(CAUUID * pPages)
{
 DPRINTF(_l("TffdshowEnc::GetPages"));

 initDialog();loadEncodingSettings();
 onTrayIconChange(0,0);

 pPages->cElems=1;
 pPages->pElems=(GUID *)CoTaskMemAlloc(pPages->cElems*sizeof(GUID));
 if (pPages->pElems==NULL) return E_OUTOFMEMORY;
 pPages->pElems[0]=(globalSettings->filtermode&IDFF_FILTERMODE_VFW)?CLSID_TFFDSHOWENCPAGEVFW:CLSID_TFFDSHOWENCPAGE;

 return S_OK;
}
STDMETHODIMP TffdshowEnc::GetClassID(CLSID *pClsid)
{
 return CBaseFilter::GetClassID(pClsid);
}
int TffdshowEnc::SizeMax(void)
{
 return 4096;
}
HRESULT TffdshowEnc::WriteToStream(IStream *pStream)
{
 int32_t len=saveEncodingSettingsMem(NULL,0);
 void *buf=malloc(len+4);
 *(int32_t*)buf=len;
 saveEncodingSettingsMem((char*)buf+4,len);
 pStream->Write(buf,len+4,NULL);
 free(buf);
 return S_OK;
}
HRESULT TffdshowEnc::ReadFromStream(IStream *pStream)
{
 int32_t len;
 pStream->Read(&len,4,NULL);
 void *buf=malloc(len);
 pStream->Read(buf,len,NULL);
 notreg=true;
 loadEncodingSettingsMem(buf,len);
 free(buf);
 return S_OK;
}

HRESULT TffdshowEnc::CheckInputType(const CMediaType * mtIn)
{
 return S_OK;
}
int TffdshowEnc::getVideoCodecId(const BITMAPINFOHEADER *hdr,const GUID *subtype,FOURCC *AVIfourcc)
{
 initCo();
 return query(hdr,NULL)==ICERR_OK?CODEC_ID_RAW:CODEC_ID_NONE;
}

HRESULT TffdshowEnc::CheckConnect(PIN_DIRECTION dir,IPin *pPin)
{
 if (dir==PINDIR_INPUT)
  return S_OK;
 else
  return CTransformFilter::CheckConnect(dir,pPin);
}

HRESULT TffdshowEnc::SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt)
{
 DPRINTF(_l("TffdshowEnc::SetMediaType"));
 if (direction==PINDIR_INPUT)
  initCo();
 return S_OK;
}
HRESULT TffdshowEnc::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
 DPRINTF(_l("TffdshowEnc::CheckTransform"));
 fileout=!!(*mtOut->Type()==MEDIATYPE_Stream);
 return S_OK;
}
HRESULT TffdshowEnc::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest)
{
 DPRINTF(_l("TffdshowEnc::DecideBufferSize"));
 if (m_pInput->IsConnected()==FALSE) return E_UNEXPECTED;

 ppropInputRequest->cBuffers=1;
 ppropInputRequest->cbBuffer=(long)getSize(&inpin->biIn);
 ppropInputRequest->cbPrefix=0;
 if (fileout) ppropInputRequest->cbAlign=1;

 ALLOCATOR_PROPERTIES ppropActual;
 HRESULT result=pAlloc->SetProperties(ppropInputRequest,&ppropActual);
 if (result!=S_OK) return result;
 if (ppropActual.cbBuffer<ppropInputRequest->cbBuffer) return E_FAIL;
 return S_OK;
}

HRESULT TffdshowEnc::GetMediaType(int iPosition, CMediaType *mtOut)
{
 DPRINTF(_l("TffdshowEnc::GetMediaType"));

 if (m_pInput->IsConnected()==FALSE) return E_UNEXPECTED;

 if (iPosition<0) return E_INVALIDARG;
 if (iPosition>/*(mpeg_codec(ffvfw->cfg.co.codecId)?2:1)*/1) return VFW_S_NO_MORE_ITEMS;

 switch (iPosition)
  {
   case 0:
    {
     size_t elen=getFormat(&inpin->biIn.bmiHeader,NULL)-sizeof(BITMAPINFOHEADER);
     VIDEOINFOHEADER *vih=(VIDEOINFOHEADER*)mtOut->ReallocFormatBuffer(ULONG(sizeof(VIDEOINFOHEADER)+elen));
     if (!vih) return E_OUTOFMEMORY;

     ZeroMemory(vih,sizeof(VIDEOINFOHEADER)+elen);
     getFormat(&inpin->biIn.bmiHeader,(BITMAPINFO*)&vih->bmiHeader);
     vih->rcSource.left=0;vih->rcSource.top=0;vih->rcSource.right=vih->bmiHeader.biWidth;vih->rcSource.bottom=vih->bmiHeader.biHeight;
     vih->rcTarget=vih->rcSource;
     vih->AvgTimePerFrame=inpin->avgTimePerFrame;

     GUID subtype=MEDIASUBTYPE_YV12;
     subtype.Data1=vih->bmiHeader.biCompression;
     mtOut->SetSubtype(&subtype);

     mtOut->SetType(&MEDIATYPE_Video);
     mtOut->SetFormatType(&FORMAT_VideoInfo);
     mtOut->SetTemporalCompression(TRUE);
     mtOut->SetSampleSize(vih->bmiHeader.biSizeImage);
     break;
    }
   case 1:
    {
     mtOut->SetType(&MEDIATYPE_Stream);
     mtOut->SetSubtype(&MEDIASUBTYPE_None);
     mtOut->SetFormatType(&FORMAT_None);
     unsigned int outdx,outdy;
     getOut(inpin->biIn.bmiHeader.biWidth,inpin->biIn.bmiHeader.biHeight,&outdx,&outdy);
     mtOut->SetSampleSize(outdx*outdy*4);
     mtOut->SetVariableSize();
     mtOut->SetTemporalCompression(FALSE);
     break;
    }
   case 2:
    {
     unsigned int outdx,outdy;
     getOut(inpin->biIn.bmiHeader.biWidth,inpin->biIn.bmiHeader.biHeight,&outdx,&outdy);
     mtOut->SetType(&MEDIATYPE_Video);
     mtOut->SetTemporalCompression(TRUE);
     mtOut->SetSampleSize(outdx*outdy*4);
     if (coSettings->codecId==CODEC_ID_MPEG1VIDEO) //mpeg1 (including VideoCD)
      {
       mtOut->SetSubtype(&MEDIASUBTYPE_MPEG1Video);
       mtOut->SetFormatType(&FORMAT_MPEGVideo);
       MPEG1VIDEOINFO *mvi=(MPEG1VIDEOINFO*)mtOut->ReallocFormatBuffer(sizeof(MPEG1VIDEOINFO));
       mvi->dwStartTimeCode=0;
       ZeroMemory(&mvi->hdr,sizeof(VIDEOINFOHEADER));
       mvi->hdr.rcSource.left=0;mvi->hdr.rcSource.top=0;mvi->hdr.rcSource.right=outdx;mvi->hdr.rcSource.bottom=outdy;
       mvi->hdr.rcTarget=mvi->hdr.rcSource;
       mvi->hdr.AvgTimePerFrame=inpin->avgTimePerFrame;
       //mvi->hdr.bmiHeader=biOut.bmiHeader;
      }
     else //mpeg2
      {
      }
     break;
    }
  }
 return S_OK;
}

STDMETHODIMP TffdshowEnc::Stop(void)
{
 DPRINTF(_l("TffdshowEnc::Stop"));
 HRESULT res=CTransformFilter::Stop();
 end();
 started=false;
 return res;
}

bool TffdshowEnc::start(void)
{
 started=true;
 if (coSettings->isFPSoverride)
  {
   inpin->avgTimePerFrame=REF_SECOND_MULT*coSettings->fpsOverrideDen/coSettings->fpsOverrideNum;
   fpsRate=coSettings->fpsOverrideNum;fpsScale=coSettings->fpsOverrideDen;
  }
 else if (inpin->avgTimePerFrame>10000) //if sane value is available
  lavc_reduce(&fpsRate,&fpsScale,REF_SECOND_MULT,inpin->avgTimePerFrame,32768);
 else // default
  {
   fpsRate=25;fpsScale=1;
  }
 if (begin(&inpin->biIn.bmiHeader)!=ICERR_OK) return false;
 ft1=ft2=0;
 return true;
}

int TffdshowEnc::getQuantFirst(void)
{
 if (coSettings->credits_mode==CREDITS_MODE::QUANT && coSettings->isInCredits(params.framenum))
  return coSettings->credits_quant_p;
 else
  return 2;
}
int TffdshowEnc::getQuantQuant(void)
{
 if (coSettings->isInCredits(params.framenum))
  switch (coSettings->credits_mode)
   {
    case CREDITS_MODE::PERCENT:
     return coSettings->q_p_max-((coSettings->q_p_max-coSettings->quant)*coSettings->credits_percent/100);
    case CREDITS_MODE::QUANT:
     return coSettings->credits_quant_p;
    default:
     return 0;
   }
 else
  return coSettings->quant;
}

TvideoCodecEnc* TffdshowEnc::findEncLib(void)
{
 if (!enc || oldCodecId!=coSettings->codecId)
  {
   oldCodecId=(CodecID)coSettings->codecId;
   enc=enclibs.getEncLib(oldCodecId);
  }
 return enc;
}
void TffdshowEnc::getOut(unsigned int AVIdx,unsigned int AVIdy,unsigned int *outDx,unsigned int *outDy)
{
 initCo();
 if (!ffproc) getFFproc();
 if (coSettings->isProc && ffproc)
  ffproc->getNewSize(AVIdx,AVIdy,outDx,outDy);
 else
  {
   *outDx=AVIdx;
   *outDy=AVIdy;
  }
}

STDMETHODIMP_(LRESULT) TffdshowEnc::query(const BITMAPINFOHEADER *inhdr,BITMAPINFOHEADER *outhdr)
{
 initCo();
 if (getBMPcolorspace(inhdr,coSettings->incsps)==FF_CSP_NULL)
  return ICERR_BADFORMAT;
 if (outhdr==NULL)
  {
   unsigned int outDx,outDy;
   getOut(inhdr->biWidth,inhdr->biHeight,&outDx,&outDy);
   if (outDx&1 || outDy&1) return ICERR_BADFORMAT;
   return ICERR_OK;
  }
 if (inhdr->biWidth!=outhdr->biWidth || inhdr->biHeight!=outhdr->biHeight) return ICERR_BADFORMAT;
 if (outhdr->biCompression==coSettings->fourcc) return ICERR_OK; //FIX ?
 return ICERR_BADFORMAT;
}

STDMETHODIMP_(LRESULT) TffdshowEnc::getFormat(const BITMAPINFOHEADER *inhdr,BITMAPINFO *lpbiOutput)
{
 initCo();
 if (getBMPcolorspace(inhdr,coSettings->incsps)==FF_CSP_NULL)
  return ICERR_BADFORMAT;

 unsigned int outDx,outDy;getOut(inhdr->biWidth,inhdr->biHeight,&outDx,&outDy);
 if (!findEncLib()) return 0;
 extradata.clear();
 if (enc->supExtradata())
  {
   if (enccsps.empty()) enc->getCompressColorspaces(enccsps,outDx,outDy);
   enccsp=enccsps[0];
   enc->beginCompress(coSettings->mode,enccsp,Trect(0,0,outDx,outDy));
   const void *edata0;size_t elen;
   enc->getExtradata(&edata0,&elen);
   if (elen)
    extradata.set(edata0,elen,0,true);
   enc->end();
  }

 if (lpbiOutput==NULL)
  return sizeof(BITMAPINFOHEADER)+extradata.size;
 BITMAPINFOHEADER *outhdr=&lpbiOutput->bmiHeader;

 memcpy(outhdr,inhdr,sizeof(BITMAPINFOHEADER));
 outhdr->biSize=DWORD(sizeof(BITMAPINFOHEADER)+extradata.size);
 findEncLib();
 outhdr->biWidth=outDx;
 outhdr->biHeight=outDy;
 if (!enc || !enc->prepareHeader(outhdr))
  {
   outhdr->biCompression=coSettings->fourcc;
   outhdr->biBitCount=24; // or 16
   outhdr->biSizeImage=outDx*outDy*3;
  }
 //TODO: maybe encoders should be allowed to modify other outhdr properties
 outhdr->biPlanes=1;
 outhdr->biXPelsPerMeter=0;
 outhdr->biYPelsPerMeter=0;
 outhdr->biClrUsed=0;
 outhdr->biClrImportant=0;
 if (extradata.data) memcpy((unsigned char*)outhdr+sizeof(BITMAPINFOHEADER),extradata.data,extradata.size);
 biOutput.bmiHeader=lpbiOutput->bmiHeader;
 return ICERR_OK;
}

STDMETHODIMP_(LRESULT) TffdshowEnc::getSize(const BITMAPINFO *lpbiInput)
{
 findEncLib();
 unsigned int outDx,outDy;
 getOut(lpbiInput->bmiHeader.biWidth,lpbiInput->bmiHeader.biHeight,&outDx,&outDy);
 BITMAPINFOHEADER outhdr;
 outhdr.biWidth=outDx;
 outhdr.biHeight=outDy;
 if (!enc || !enc->prepareHeader(&outhdr))
  return outDx*outDy*3;
 else
  return outhdr.biSizeImage;
}

STDMETHODIMP_(LRESULT) TffdshowEnc::begin(const BITMAPINFOHEADER *inhdr)
{
 if (!findEncLib()) return ICERR_ERROR;

 int nom,den;
 if (lavc_reduce(&nom,&den,fpsRate,fpsScale,10000))
  {
   fpsRate =nom;
   fpsScale=den;
  }

 dbgInit();
 getOut(inhdr->biWidth,inhdr->biHeight,&outDx,&outDy);
 ownStoreExt=false;

 if (enccsps.empty())
  {
   enc->getCompressColorspaces(enccsps,outDx,outDy);
   if (enccsps.empty())
    return ICERR_BADFORMAT;
  }
 enccsp=enccsps[0];

 cfgcomode=coSettings->mode;
 switch (cfgcomode)
  {
   case ENC_MODE::CBR:
    if (!sup_CBR(coSettings->codecId)) cfgcomode=ENC_MODE::UNKNOWN;
    break;
   case ENC_MODE::VBR_QUAL:
    if (!sup_VBR_QUAL(coSettings->codecId)) cfgcomode=ENC_MODE::UNKNOWN;
    break;
   case ENC_MODE::VBR_QUANT:
    if (!sup_VBR_QUANT(coSettings->codecId)) cfgcomode=ENC_MODE::UNKNOWN;
    break;
   case ENC_MODE::PASS2_1:
   case ENC_MODE::PASS2_2_EXT:
   case ENC_MODE::PASS2_2_INT:
    if (!sup_XVID2PASS(coSettings->codecId)) cfgcomode=ENC_MODE::UNKNOWN;
    break;
  }

 if (coSettings->storeExt && !ownStoreExt && coSettings->storeExtFlnm[0])
  mux=Tmuxer::getMuxer(coSettings->muxer,this);
 else
  mux=NULL;
 enc->setCoSettings(oldCodecId);
 LRESULT res=enc->beginCompress(cfgcomode,enccsp,Trect(0,0,outDx,outDy));
 if (res!=ICERR_OK) return res;
 switch (cfgcomode)
  {
   case ENC_MODE::PASS2_1:
    pass=new T2passFirst(*coSettings);
    break;
   case ENC_MODE::PASS2_2_EXT:
    pass=new T2passSecond(this);
   case ENC_MODE::PASS2_2_INT:
    pass=new T2passSecond(this);
    break;
  }

 dx=inhdr->biWidth;dy=inhdr->biHeight;
 totalsize=keyspacing=0;
 if (coSettings->isProc && ffproc)
  ffproc->begin(dx,dy,fpsRate,fpsScale);

 if (mux)
  mux->writeHeader(extradata.data,extradata.size,1,biOutput.bmiHeader);

 memset(&params,0,sizeof(params));
 encStats.init(outDx,outDy,enccsp);
 outputdebug=globalSettings->outputdebug;outputdebugfile=globalSettings->outputdebugfile;

 working=true;firstrun=true;
 return ICERR_OK;
}
HRESULT TffdshowEnc::EndOfStream(void)
{
 HRESULT hr=compress(&inpin->biIn.bmiHeader,NULL,0,_I64_MIN,_I64_MIN);
 return FAILED(hr)?hr:CTransformFilter::EndOfStream();
}
STDMETHODIMP_(LRESULT) TffdshowEnc::end(void)
{
 if (working && (outputdebug || outputdebugfile) && encStats.count>0)
  {
   if (int(encStats.sumQuants)>0) dbgWrite(_l("Average quantizer: %-5.2f\n"),float(encStats.sumQuants)/encStats.count);
   if (coSettings->mode!=ENC_MODE::VBR_QUANT)
    {
     dbgWrite(_l("Quantizer distribution:"));
     for (int i=1;i<countof(encStats.quantCount);i++)
      if (encStats.quantCount[i])
       dbgWrite(_l("Q:%i:%i"),i,encStats.quantCount[i]);
    }
   dbgWrite(_l("Bitrate: %i kbps"),int((8ULL*encStats.sumFramesize*fpsRate)/(1000ULL*encStats.count*fpsScale)));
   if (globalSettings->psnr)
    {
     double psnrY,psnrU,psnrV;encStats.calcPSNR(&psnrY,&psnrU,&psnrV);
     if (psnrY!=-1)
      dbgWrite(_l("Average PSNR: %6.2f,%6.2f,%6.2f\n"),psnrY,psnrU,psnrV);
    }
  }
 working=false;
 if (h_graph) PostMessage(h_graph,TencStats::MSG_FF_CLEAR,0,0);
 hideTrayIcon();
 if (coSettings->isProc && ffproc) ffproc->end();
 if (pass) delete pass;pass=NULL;
 if (enc) enc->end();
 if (mux) delete mux;mux=NULL;
 if (convert) delete convert;convert=NULL;
 if (ownpictbuf.size()) {ownpictbuf.clear();memset(&ownpict,0,sizeof(ownpict));}
 enccsps.clear();
 dbgDone();
 return ICERR_OK;
}

STDMETHODIMP_(LRESULT) TffdshowEnc::compress(const BITMAPINFOHEADER *inhdr,const uint8_t *src,size_t /*srclen*/,REFERENCE_TIME rtStart,REFERENCE_TIME rtStop)
{
 if (firstrun)
  {
   firstrun=false;
   showTrayIcon();
  }

 _mm_empty();
 inColorspace=getBMPcolorspace(inhdr,coSettings->incsps);
 if (inColorspace==FF_CSP_NULL) return ICERR_BADFORMAT;
 if (coSettings->flip) inColorspace^=FF_CSP_FLAGS_VFLIP;
 if (globalSettings->isDyInterlaced && dy>(unsigned int)globalSettings->dyInterlaced) inColorspace|=FF_CSP_FLAGS_INTERLACED;
 params.quant=-1;
 params.frametype=FRAME_TYPE::UNKNOWN;params.keyframe=false;
 switch (cfgcomode)
  {
   case ENC_MODE::CBR:
   case ENC_MODE::VBR_QUAL:
    break;
   case ENC_MODE::VBR_QUANT:
    params.quant=getQuantQuant();
    break;
   case ENC_MODE::PASS2_1:
    params.quant=getQuantFirst();
    break;
   case ENC_MODE::PASS2_2_EXT:
   case ENC_MODE::PASS2_2_INT:
    if (!pass->getQuantSecond(params)) return ICERR_ERROR;
    if (params.quant!=-1) params.quant&=Txvid_2pass::NNSTATS_QUANTMASK;
    break;
   case ENC_MODE::UNKNOWN:
    break;
   default:
    DPRINTF(_l("Invalid encoding mode"));
    return ICERR_ERROR;
  }

 params.quanttype=coSettings->getQuantType(params.quant);
 params.gray=coSettings->gray || (coSettings->graycredits && coSettings->isInCredits(params.framenum));

 if (keyspacing<coSettings->min_key_interval && params.framenum)
  params.frametype=FRAME_TYPE::P;

 //if (!src || !srclen) return ICERR_ERROR;

 if (pict.csp!=inColorspace)
  {
   pict.csp=inColorspace;
   pict.cspInfo=*csp_getInfo(pict.csp);
  }
 pict.setSize(dx,dy);
 pict.data[0]=(unsigned char*)src;
 pict.stride[0]=dx*pict.cspInfo.Bpp;
 if (src)
  {
   csp_yuv_adj_to_plane(pict.csp,&pict.cspInfo,dy,pict.data,pict.stride);
   csp_yuv_order(pict.csp,pict.data,pict.stride);
   csp_vflip(pict.csp,&pict.cspInfo,pict.data,pict.stride,dy);
  }
 if (csp_isPAL(pict.csp))
  {
   pict.palette.pal=(unsigned char*)(inhdr+1);
   pict.palette.numcolors=inhdr->biClrUsed;
  }

 if (!coSettings->isProc || !ffproc)
  {
   bool directYV12=false;
   for (Tcsps::const_iterator c=enccsps.begin();c!=enccsps.end();c++)
    if (*c==pict.csp)
     {
      directYV12=true;
      break;
     }
   if (!directYV12)
    {
     if (!convert) convert=new Tconvert(this,dx,dy);
     if (!ownpict.data[0]) ownpict.alloc(dx,dy,enccsp,ownpictbuf);
     if (src)
      convert->convert(pict,ownpict.csp,ownpict.data,ownpict.stride);
       //srclen=ownpict.cspInfo.bpp*ownpict.rectFull.dx*ownpict.rectFull.dy/8;
     pict=ownpict;
     if (!src)
      pict.data[0]=NULL;
    }
  }

 params.length=0;
 params.priv=NULL;
 pict.rtStart=rtStart;
 pict.rtStop=rtStop;
 if (src && coSettings->isProc && ffproc)
  return ffproc->processPict(params.framenum,pict,enccsp);
 else
  return src?enc->compress(pict,params):enc->flushEnc(pict,params);
}

STDMETHODIMP TffdshowEnc::TffProcVideoEnc::deliverProcessedSample(TffPict &pict)
{
 return self->enc->compress(pict,self->params);
}

STDMETHODIMP TffdshowEnc::deliverEncodedSample(const TmediaSample &sample,TencFrameParams &params)
{
 _mm_empty();

 params.outlength=coSettings->storeAVI?params.length:0;

 if (mux)
  mux->writeFrame(sample,params.length,params);

 if (params.keyframe)
  keyspacing=0;

 totalsize+=params.length;

 if (outputdebug || outputdebugfile)
  if (cfgcomode==ENC_MODE::PASS2_2_INT || cfgcomode==ENC_MODE::PASS2_2_EXT)
   pass->writeInfo(params);
  else
   dbgWrite(_l("1st-pass: size:%d total-kbytes:%d %s quant:%d %s kblocks:%d mblocks:%d\n"),params.length,int(totalsize/1024),FRAME_TYPE::name(params.frametype),params.quant,encQuantTypes[params.quanttype],params.kblks,params.mblks);

 if (pass)
  if (pass->update(params)==false)
   return ICERR_ERROR;

 params.framenum++;keyspacing++;

 encStats.add(params);
 if (h_graph) PostMessage(h_graph,TencStats::MSG_FF_FRAME,params.length,(params.quant<<20)|params.frametype);

 return ICERR_OK;
}

STDMETHODIMP TffdshowEnc::JoinFilterGraph(IFilterGraph *pGraph,LPCWSTR pName)
{
 return onJoinFilterGraph(pGraph,pName);
}

HRESULT TffdshowEnc::onGraphRemove(void)
{
 _release();
 return TffdshowBase::onGraphRemove();
}

void TffdshowEnc::sendOnChange(int paramID,int val)
{
 if (paramID!=IDFF_enc_fpsRate && paramID!=IDFF_enc_fpsScale &&
     paramID!=IDFF_enc_fpsScale
    )
  TffdshowBase::sendOnChange(paramID,val);
}

STDMETHODIMP TffdshowEnc::setCoSettingsPtr(TcoSettings *coSettingsPtr)
{
 if (!coSettingsPtr) return E_POINTER;
 coSettings=coSettingsPtr;
 new(coSettings)TcoSettings(options);
 notifyParamsChanged();
 return S_OK;
}
STDMETHODIMP TffdshowEnc::getCoSettingsPtr(const TcoSettings* *coSettingsPtr)
{
 if (!coSettingsPtr) return E_POINTER;
 *coSettingsPtr=coSettings;
 return S_OK;
}

STDMETHODIMP TffdshowEnc::loadEncodingSettings(void)
{
 initCo();
 return S_OK;
}
STDMETHODIMP TffdshowEnc::saveEncodingSettings(void)
{
 initCo();
 if (!notreg) coSettings->saveReg();
 return S_OK;
}

HRESULT TffdshowEnc::onInitDialog(void)
{
 loadEncodingSettings();
 return S_OK;
}

void TffdshowEnc::onTrayIconChange(int id,int newval)
{
 if (working)
  TffdshowBase::onTrayIconChange(id,newval);
}

void TffdshowEnc::lockReceive(void)
{
 m_csReceive.Lock();
}
void TffdshowEnc::unlockReceive(void)
{
 m_csReceive.Unlock();
}

void TffdshowEnc::getColls(TintStrColls &colls)
{
 if (coSettings)
  colls.push_back(coSettings->options);
 TffdshowBase::getColls(colls);
}

TinfoBase* TffdshowEnc::createInfo(void)
{
 return new TinfoBase(this);
}

HRESULT TffdshowEnc::muxHeader(const void *data,size_t datalen,int flush)
{
 if (!mux) return E_UNEXPECTED;
 return mux->writeHeader(data,datalen,!!flush,biOutput.bmiHeader)==0?S_FALSE:S_OK;
}

STDMETHODIMP TffdshowEnc::getEncStats(TencStats* *encStatsPtr)
{
 if (!encStatsPtr) return E_POINTER;
 *encStatsPtr=&encStats;
 return S_OK;
}

STDMETHODIMP TffdshowEnc::isLAVCadaptiveQuant(void)
{
 return coSettings->isLAVCadaptiveQuant();
}
STDMETHODIMP TffdshowEnc::isQuantControlActive(void)
{
 return coSettings->isQuantControlActive();
}
STDMETHODIMP_(int) TffdshowEnc::getQuantType2(int quant)
{
 return coSettings->getQuantType(quant);
}

STDMETHODIMP TffdshowEnc::getCustomQuantMatrixes(unsigned char* *intra8,unsigned char* *inter8,unsigned char* *intra4Y,unsigned char* *inter4Y,unsigned char* *intra4C,unsigned char* *inter4C)
{
 if (intra8) *intra8=(unsigned char*)&coSettings->qmatrix_intra_custom0;
 if (inter8) *inter8=(unsigned char*)&coSettings->qmatrix_inter_custom0;
 if (intra4Y) *intra4Y=(unsigned char*)&coSettings->qmatrix_intra4x4Y_custom0;
 if (inter4Y) *inter4Y=(unsigned char*)&coSettings->qmatrix_inter4x4Y_custom0;
 if (intra4C) *intra4C=(unsigned char*)&coSettings->qmatrix_intra4x4C_custom0;
 if (inter4C) *inter4C=(unsigned char*)&coSettings->qmatrix_inter4x4C_custom0;
 return S_OK;
}

STDMETHODIMP TffdshowEnc::getEncoder(int codecId,const Tencoder* *encPtr)
{
 if (!encPtr) return E_POINTER;
 *encPtr=enclibs.getEncoder(codecId);
 return S_OK;
}
STDMETHODIMP TffdshowEnc::getVideoCodecs(const TvideoCodecs* *codecs)
{
 if (!codecs) return E_POINTER;
 *codecs=&enclibs;
 return S_OK;
}

void TffdshowEnc::getFFproc(void)
{
 if (!ffproc)
  {
   HRESULT hr;
   ffproc=new TffProcVideoEnc(NULL,&hr,this);
   ffproc->setTempPreset("ffdshowenc temporary preset");
   ffproc->loadPreset("ffdshowenc");
  }
}
STDMETHODIMP TffdshowEnc::getFFproc(TffProcVideo* *procPtr)
{
 if (!procPtr) return E_POINTER;
 getFFproc();
 *procPtr=ffproc;
 return S_OK;
}


STDMETHODIMP_(int) TffdshowEnc::loadEncodingSettingsMem(const void *buf,int len)
{
 initCo();

 lzo_uint lenD=len*10;
 unsigned char *bufD=(unsigned char*)malloc(lenD);
 if (lzo1x_decompress((unsigned char*)buf,len,bufD,&lenD,NULL)<0) return 0;
 const unsigned char *last;
 TregOpIDstreamRead t(bufD,lenD,(const void**)&last);
 coSettings->reg_op(t);
 getFFproc();
 if (ffproc && last!=bufD+lenD && *(short*)last==0)
  {
   last+=3;
   ffproc->loadConfig(!!notreg,last,lenD-(last-bufD));
  }
 free(bufD);
 coSettings->fillIncsps();
 return len;
}
STDMETHODIMP_(int) TffdshowEnc::saveEncodingSettingsMem(void *buf,int len)
{
 initCo();
 TregOpIDstreamWrite t;
 coSettings->reg_op(t);
 getFFproc();
 if (ffproc) ffproc->saveConfig(t);
 lzo_uint lenC=(lzo_uint)t.size()*2;
 unsigned char *bufC=(unsigned char*)malloc(lenC);
 unsigned char work[LZO1X_MEM_COMPRESS];
 lzo1x_1_compress(&*t.begin(),(lzo_uint)t.size(),bufC,&lenC,work);
 if (buf) memcpy(buf,bufC,lenC);
 free(bufC);
 return (int)lenC;
}

STDMETHODIMP TffdshowEnc::getInCodecString(char_t *buf,size_t buflen)
{
 if (!buf) return E_POINTER;
 if (inColorspace==0)
  buf[0]='\0';
 else
  csp_getName(inColorspace,buf,buflen);
 return S_OK;
}
STDMETHODIMP TffdshowEnc::getOutCodecString(char_t *buf,size_t buflen)
{
 if (!buf) return E_POINTER;
 if (!enc) return E_UNEXPECTED;
 ff_strncpy(buf,enc->getName(),buflen);
 buf[buflen-1]='\0';

 return S_OK;
}

int TffdshowEnc::get_trayIconType(void)
{
 switch (globalSettings->trayIconType)
  {
   case 2: return IDI_FFVFW;
   case 0:
   case 1:
   default:
   return IDI_MODERN_ICON_E;
  }
}

//================================== DirectShow ====================================
STDMETHODIMP_(LRESULT) TffdshowEncDshow::begin(const BITMAPINFOHEADER *inhdr)
{
 HRESULT hr=TffdshowEnc::begin(inhdr);
 if (SUCCEEDED(hr))
  times.clear();
 return hr;
}

HRESULT TffdshowEncDshow::Receive(IMediaSample *pIn)
{
 if (!started)
  if (!start()) return E_FAIL;

 BYTE *src=NULL;
 if (pIn->GetPointer(&src)!=S_OK) return S_FALSE;
 size_t srcLen=pIn->GetActualDataLength();
 REFERENCE_TIME rtStart,rtStop;
 if (FAILED(pIn->GetTime(&rtStart,&rtStop)))
  rtStart=rtStop=REFTIME_INVALID;
 return (HRESULT)compress(&inpin->biIn.bmiHeader,src,srcLen,rtStart,rtStop);
}

STDMETHODIMP TffdshowEncDshow::getDstBuffer(IMediaSample* *pOut,const TffPict &pict)
{
 *pOut=NULL;BYTE *data;
 HRESULT hr;
 if (FAILED(hr=getDeliveryBuffer(pOut,&data)))
  return hr;

 if (coSettings->isFPSoverride || pict.rtStart==REFTIME_INVALID || pict.rtStop==REFTIME_INVALID)
  {
   REFERENCE_TIME tStart=(params.framenum  )*inpin->avgTimePerFrame;
   REFERENCE_TIME tStop =(params.framenum+1)*inpin->avgTimePerFrame;
   setPropsTime(*pOut,tStart,tStop,m_pInput->SampleProps(),&m_bSampleSkipped);
   //DPRINTF("tStart:%i, tStop:%i, AVIfps:%i",int(tStart),int(tStop),timePerFrame);
  }
 else
  (*pOut)->SetTime((REFERENCE_TIME*)&pict.rtStart,(REFERENCE_TIME*)&pict.rtStop);

 AM_MEDIA_TYPE *mtOut;
 (*pOut)->GetMediaType(&mtOut);
 if (mtOut!=NULL)
  {
   HRESULT result=S_OK;
   DeleteMediaType(mtOut);
   if (result!=S_OK) return result;
  }
 return S_OK;
}
STDMETHODIMP TffdshowEncDshow::deliverError(void)
{
 return E_FAIL;
}
STDMETHODIMP TffdshowEncDshow::deliverEncodedSample(const TmediaSample &sample,TencFrameParams &params)
{
 REFERENCE_TIME rtStart,rtStop;
 if (sample->GetTime(&rtStart,&rtStop)==S_OK)
  times.push_back(std::make_pair(rtStart,rtStop));
 if (params.frametype==FRAME_TYPE::DELAY)
  {
   sample->Release();
   return S_OK;
  }
 HRESULT hr;
 if (FAILED(hr=TffdshowEnc::deliverEncodedSample(sample,params)))
  return hr;

 if (!times.empty())
  {
   rtStart=times.front().first;
   rtStop=times.front().second;
   sample->SetTime(&rtStart,&rtStop);
   times.pop_front();
  }
 sample->SetSyncPoint(params.keyframe?TRUE:FALSE);
 sample->SetActualDataLength(params.outlength);

 if (fileout)
  {
   ft2+=params.outlength;
   sample->SetTime(&ft1,&ft2);
   ft1+=params.outlength;
  }
 IMediaSample **s=(IMediaSample**)&sample;
 return m_pOutput->Deliver(*s);
}

//===================================== VFW ========================================
STDMETHODIMP TffdshowEncVFW::getDstBuffer(IMediaSample* *samplePtr,const TffPict &pict)
{
 if (!samplePtr) return E_POINTER;
 struct TfakeEncMediaSample : TfakeMediaSample,CUnknown
  {
  private:
   void *buf;long bufsize;
  public:
   TfakeEncMediaSample(void *Ibuf,long Ibufsize):buf(Ibuf),bufsize(Ibufsize),CUnknown("TfakeEncMediaSample",NULL) {}
   DECLARE_IUNKNOWN
   virtual HRESULT STDMETHODCALLTYPE GetPointer(BYTE **ppBuffer)
    {
     if (!ppBuffer) return E_POINTER;
     return (*ppBuffer=(uint8_t*)buf)!=NULL?S_OK:E_FAIL;
    }
   virtual long STDMETHODCALLTYPE GetSize(void) {return bufsize;}
  };
 *samplePtr=new TfakeEncMediaSample(icc->lpOutput,icc->lpbiOutput->biSizeImage);
 (*samplePtr)->AddRef();
 return S_OK;
}

STDMETHODIMP TffdshowEncVFW::deliverError(void)
{
 return ICERR_ERROR;
}

STDMETHODIMP TffdshowEncVFW::deliverEncodedSample(const TmediaSample &sample,TencFrameParams &params)
{
 HRESULT hr;
 if (FAILED(hr=TffdshowEnc::deliverEncodedSample(sample,params)))
  return hr;

 icc->lpbiOutput->biSizeImage=params.outlength;
 if (icc->lpdwFlags) *icc->lpdwFlags=params.keyframe?AVIIF_KEYFRAME:0;

 return hr;
}
