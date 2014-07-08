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
#include "Tffvfw.h"
#include "ffdshow_constants.h"
#include "IffdshowBase.h"
#include "IffdshowEnc.h"
#include "Tconvert.h"
#include "TpageSite.h"
#include "TSpecifyPropertyPagesVE.h"
#include "TpresetSettingsVideo.h"
#include "ToutputVideoSettings.h"
#include "TglobalSettings.h"
#include "dsutil.h"
#include "autoptr.h"
#include "ffdebug.h"
#include "TffDecoderVideo.h"

class TimgFiltersVFW :public TimgFilters
{
public:
 TimgFiltersVFW(IffdshowBase *Ideci,IprocVideoSink *Isink):TimgFilters(Ideci,Isink) {}
};

class TffdshowDecVideoVFW :public TffdshowDecVideo
{
private:
 IprocVideoSink *procVideoSink;
protected:
 virtual TimgFilters* createImgFilters(void)
  {
   return new TimgFiltersVFW(this,procVideoSink);
  }
public:
 TffdshowDecVideoVFW(HRESULT *phr,IprocVideoSink* iprocVideoSink):
  TffdshowDecVideo(CLSID_FFDSHOWVFW,NAME("TffdshowDecVideoVFW"),CLSID_TFFDSHOWPAGEVFW,IDS_FFDSHOWDECVIDEOVFW,IDI_FFDSHOW,NULL,phr,IDFF_FILTERMODE_PLAYER|IDFF_FILTERMODE_VFW,MERIT_DO_NOT_USE,new TintStrColl),
  procVideoSink(iprocVideoSink)
  {
   AddRef();
  }
 void release(void)
  {
   if (imgFilters) delete imgFilters;
   Release();
  }
 HRESULT processPict(TffPict &pict)
  {
   if (!imgFilters) imgFilters=createImgFilters();
   return imgFilters->process(pict,presetSettings);
  }
 bool getFlip(void) const
  {
   return !!presetSettings->output->flip;
  }
};

//==================================================================

CUnknown* WINAPI Tffvfw::CreateInstance(LPUNKNOWN punk,HRESULT *phr)
{
 Tffvfw *pNewObject=new Tffvfw(punk,phr);
 if (pNewObject==NULL)
  *phr=E_OUTOFMEMORY;
 return pNewObject;
}
STDMETHODIMP Tffvfw::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
 CheckPointer(ppv, E_POINTER);
 if (riid==IID_Iffvfw)
  return GetInterface<Iffvfw>(this,ppv);
 else
  return CUnknown::NonDelegatingQueryInterface(riid,ppv);
}

Tffvfw::Tffvfw(LPUNKNOWN punk,HRESULT *phr):
 CUnknown(NAME("Tffvfw"),punk),
 convert(NULL),
 dec(NULL),
 graph(false),
 previous_out_biSizeImage(0),
 previouts_lpOutput(NULL)
{
 randomize();
}
Tffvfw::~Tffvfw()
{
 if (dec) delete dec;
 if (deciE) deciE->_release();
 if (convert) delete convert;
 if (decVFW) decVFW->release();
}

bool Tffvfw::initCo(void)
{
 if (deciEB) return true;
 if (SUCCEEDED(CoCreateInstance(CLSID_FFDSHOWENCVFW,NULL,CLSCTX_INPROC_SERVER,IID_IffdshowEncVFW,(void**)&deciE_VFW)))
  {
   deciE=deciE_VFW;
   deciEB=deciE;
  }
 else
  deciEB=NULL;
 return !!deciEB;
}
bool Tffvfw::initDec(void)
{
 if (decVFW) return true;
 HRESULT hr=S_OK;
 decVFW=new TffdshowDecVideoVFW(&hr,this);
 decVFW->initPresets();
 decVFW->setActivePreset(FFPRESET_DEFAULT,0);
 return !!decVFW;
}

STDMETHODIMP_(void) Tffvfw::aboutDlg(HWND hParent)
{
 if (initCo())
  {
   deciEB->putParam(IDFF_dlgEncAbout,1);
   deciEB->showCfgDlg(hParent);
  }
}
STDMETHODIMP_(void) Tffvfw::configDlg(HWND hParent)
{
 if (initCo() && initDec())
  TSpecifyPropertyPagesVE::show2configPages(IID_IffdshowEnc,deciE,getGUID<IffdshowDecVideo>(),(IffdshowDecVideo*)decVFW,IDS_FFDSHOWENC,IDI_FFVFW,IDFF_dlgEncCurrentPage);
}

STDMETHODIMP_(int) Tffvfw::saveMem(void *buf,int len)
{
 if (initCo())
  return deciE->saveEncodingSettingsMem(buf,len);
 else
  return 0;
}
STDMETHODIMP_(int) Tffvfw::loadMem(const void *buf,int len)
{
 if (initCo())
  return deciE->loadEncodingSettingsMem(buf,len);
 else
  return 0;
}
STDMETHODIMP_(void) Tffvfw::loadReg(void)
{
 if (initCo())
  deciE->loadEncodingSettings();
}

//------------------------------ encoding ------------------------------
STDMETHODIMP_(LRESULT) Tffvfw::coQuery(BITMAPINFO *lpbiInput,BITMAPINFO *lpbiOutput)
{
 if (initCo())
  return deciE_VFW->query(&lpbiInput->bmiHeader,lpbiOutput?&lpbiOutput->bmiHeader:NULL);
 else
  return VFW_E_RUNTIME_ERROR;
}
STDMETHODIMP_(LRESULT) Tffvfw::coGetFormat(BITMAPINFO *lpbiInput,BITMAPINFO *lpbiOutput)
{
 if (initCo())
  return deciE_VFW->getFormat(&lpbiInput->bmiHeader,lpbiOutput);
 else
  return VFW_E_RUNTIME_ERROR;
}
STDMETHODIMP_(LRESULT) Tffvfw::coGetSize(BITMAPINFO *lpbiInput,BITMAPINFO*)
{
 if (initCo())
  return deciE_VFW->getSize(lpbiInput);
 else
  return VFW_E_RUNTIME_ERROR;
}
STDMETHODIMP_(LRESULT) Tffvfw::coFramesInfo(ICCOMPRESSFRAMES *icf)
{
 if (initCo())
  {
   int r=icf->dwRate;
   int s=icf->dwScale;
   if (r==1000000)
    {
     switch (s)
      {
       case 41708: // 23.976 fps
        r=24000;s=1001;
        break;
       case 33367: // 29.97 fps
        r=30000;s=1001;
        break;
       case 41667: // 24 fps
        r=24;s=1;
        break;
       case 33333: // 30 fps
        r=30;s=1;
        break;
       case 16683: // 59.941 fps
        r=60000;s=1001;
        break;
       case 16667: // 60 fps
        r=60;s=1;
        break;
       case 66667: // 15 fps
        r=15;s=1;
        break;
       case 83333: // 12 fps
        r=12;s=1;
        break;
      }
    }
   deciEB->putParam(IDFF_enc_fpsRate ,r);
   deciEB->putParam(IDFF_enc_fpsScale,s);
   icf->dwRate=r;
   icf->dwScale=s;
   return ICERR_OK;
  }
 else
  return VFW_E_RUNTIME_ERROR;
}
STDMETHODIMP_(LRESULT) Tffvfw::coBegin(BITMAPINFO *lpbiInput,BITMAPINFO *lpbiOutput)
{
 if (initCo())
  {
   previous_out_biSizeImage = 0;
   previouts_lpOutput = NULL;
   return deciE_VFW->begin(&lpbiInput->bmiHeader);
  }
 else
  return VFW_E_RUNTIME_ERROR;
}
STDMETHODIMP_(LRESULT) Tffvfw::coEnd(void)
{
 if (initCo())
  {
   previous_out_biSizeImage = 0;
   previouts_lpOutput = NULL;
   return deciE_VFW->end();
  }
 else
  return VFW_E_RUNTIME_ERROR;
}
STDMETHODIMP_(LRESULT) Tffvfw::coRun(void *icc0)
{
 if (initCo())
  {
   ICCOMPRESS *icc=(ICCOMPRESS*)icc0;
   // Work around applications' bugs
   // Because Microsoft's document is incomplete, many applications are buggy in my opinion.
   //  lpbiOutput->biSizeImage is used to return value, the applications should update lpbiOutput->biSizeImage on every call.
   //  But some applications doesn't do this, thus lpbiOutput->biSizeImage smaller and smaller.
   //  The size of the buffer isn't likely to change during encoding.
   if (previouts_lpOutput == icc->lpOutput)
    icc->lpbiOutput->biSizeImage = std::max(icc->lpbiOutput->biSizeImage, previous_out_biSizeImage); // looks like very bad code, but I have no choice. Not one application need this.
   previous_out_biSizeImage = icc->lpbiOutput->biSizeImage;
   previouts_lpOutput = icc->lpOutput;
   // End of work around applications' bugs.
   deciE_VFW->setICC(icc0);
   return deciE_VFW->compress(icc->lpbiInput,(const uint8_t*)icc->lpInput,icc->lpbiInput->biSizeImage,0,0);
  }
 else
  return VFW_E_RUNTIME_ERROR;
}

//------------------------------ decoding ------------------------------
TcspInfos Tffvfw::TautoForcedCsps::decGetForcedCsp(IffdshowDec *deciD)
{
 if (!empty())
  return *this;
 else
  {
   TpresetVideo *presetSettings;deciD->getPresetPtr((Tpreset**)&presetSettings);
   TcspInfos ocsps;presetSettings->output->getOutputColorspaces(ocsps);
   return ocsps;
  }
}

TvideoCodecDec* Tffvfw::initDecoder(const BITMAPINFO *lpbiInput,CodecID *codecId)
{
 BITMAPINFOHEADER inhdr=lpbiInput->bmiHeader;
 fixMPEGinAVI(inhdr.biCompression);
 FOURCC fcc;
 CodecID owncodecId;if (!codecId) codecId=&owncodecId;
 *codecId=(CodecID)decVFW->getCodecId(&inhdr,NULL,&fcc);
 if (*codecId==CODEC_ID_NONE) return NULL;
 CMediaType mt;
 bih2mediatype(lpbiInput->bmiHeader,&mt);
 return TvideoCodecDec::initDec(decVFW,this,*codecId,fcc,mt);
}

STDMETHODIMP_(LRESULT) Tffvfw::decQuery(BITMAPINFO *lpbiInput,BITMAPINFO *lpbiOutput)
{
 if (initDec())
  {
   if (lpbiInput==NULL) return ICERR_ERROR;
   CodecID codecId;
   autoptr<TvideoCodecDec> dec=initDecoder(lpbiInput,&codecId);
   if (codecId==CODEC_ID_NONE) return ICERR_UNSUPPORTED;
   if (lpbiOutput!=NULL)
    {
     if (!dec) return ICERR_UNSUPPORTED;
     dec->forceOutputColorspace(&lpbiInput->bmiHeader,&autoforcedilace,autoforcedcolorspaces);
     const BITMAPINFOHEADER *outhdr=&lpbiOutput->bmiHeader;
     char_t pomS[60];
     DPRINTF(_l("Tffvfw::decQuery: %s"),fourcc2str(hdr2fourcc(outhdr,NULL),pomS,60));
     if (lpbiInput->bmiHeader.biWidth!=outhdr->biWidth || abs(lpbiInput->bmiHeader.biHeight)!=abs(outhdr->biHeight) || getBMPcolorspace(outhdr,autoforcedcolorspaces.decGetForcedCsp(decVFW))==FF_CSP_NULL)
      return ICERR_BADFORMAT;
    }
   return ICERR_OK;
  }
 else
  return VFW_E_RUNTIME_ERROR;
}
STDMETHODIMP_(LRESULT) Tffvfw::decGetFormat(BITMAPINFO *lpbiInput,BITMAPINFO *lpbiOutput)
{
 if (lpbiOutput==NULL) return sizeof(BITMAPINFOHEADER);

 BITMAPINFOHEADER *inhdr =&lpbiInput ->bmiHeader;
 BITMAPINFOHEADER *outhdr=&lpbiOutput->bmiHeader;

 memcpy(outhdr,inhdr,sizeof(BITMAPINFOHEADER));
 outhdr->biSize=sizeof(BITMAPINFOHEADER);
 outhdr->biBitCount=24;
 outhdr->biCompression=BI_RGB;
 outhdr->biSizeImage=outhdr->biWidth*outhdr->biHeight*(outhdr->biBitCount>>3);
 outhdr->biXPelsPerMeter=0;
 outhdr->biYPelsPerMeter=0;
 outhdr->biClrUsed=0;
 outhdr->biClrImportant=0;

 return decQuery(lpbiInput,lpbiOutput);
}
STDMETHODIMP_(LRESULT) Tffvfw::decBegin(BITMAPINFO *lpbiInput,BITMAPINFO *lpbiOutput)
{
 if (!decVFW) return VFW_E_RUNTIME_ERROR;
 if (dec) delete dec;
 dec=initDecoder(lpbiInput,NULL);
 FOURCC infcc=lpbiInput->bmiHeader.biCompression;
 fixMPEGinAVI(infcc);
 CMediaType mt;
 bih2mediatype(lpbiInput->bmiHeader,&mt);
 TffPictBase p(lpbiInput->bmiHeader.biWidth,lpbiInput->bmiHeader.biHeight);
 LRESULT res=dec->beginDecompress(p,infcc,mt,0);
 if (!res) return ICERR_UNSUPPORTED;
 decVFW->dbgInit();
 return ICERR_OK;
}
STDMETHODIMP_(LRESULT) Tffvfw::decEnd(void)
{
 if (!decVFW) return VFW_E_RUNTIME_ERROR;
 if (dec) dec->end();
 decVFW->dbgDone();
 return ICERR_OK;
}
STDMETHODIMP_(LRESULT) Tffvfw::decRun(void *icd0)
{
 if (!decVFW) return VFW_E_RUNTIME_ERROR;
 icd=(ICDECOMPRESS*)icd0;
 if (~((icd->dwFlags&ICDECOMPRESS_HURRYUP) | (icd->dwFlags&ICDECOMPRESS_UPDATE) | (icd->dwFlags&ICDECOMPRESS_PREROLL)))
  {
   if ((colorspace=getBMPcolorspace(icd->lpbiOutput,autoforcedcolorspaces.decGetForcedCsp(decVFW)))==FF_CSP_NULL)
    return ICERR_UNSUPPORTED;
   if (colorspace&FF_CSP_RGB16) colorspace=(colorspace&~FF_CSP_RGB16)|FF_CSP_RGB15;
   if (sign(icd->lpbiInput->biHeight)!=sign(icd->lpbiOutput->biHeight) && !csp_isYUVpacked(colorspace))
    colorspace^=FF_CSP_FLAGS_VFLIP;
  }
 else
  colorspace=FF_CSP_NULL;

 unsigned char *inData=(unsigned char*)icd->lpInput;
 unsigned int inLen=icd->lpbiInput->biSizeImage;

 isSyncPoint=(icd->dwFlags&ICDECOMPRESS_NOTKEYFRAME)==0;
 return dec->decompress(inData,inLen,this);
}

STDMETHODIMP Tffvfw::deliverPreroll(int frametype)
{
 return S_FALSE;
}

STDMETHODIMP Tffvfw::deliverDecodedSample(TffPict &pict)
{
 if (!pict.data[0])
  return /*used_bytes==inLen?ICERR_OK:*/ICERR_ERROR; //TODO
 if (colorspace==FF_CSP_NULL) return ICERR_OK;

 if (autoforcedilace==1)
  pict.csp&=~FF_CSP_FLAGS_INTERLACED;
 else if (autoforcedilace==2)
  pict.csp|=FF_CSP_FLAGS_INTERLACED;

 return decVFW->processPict(pict);
}

STDMETHODIMP Tffvfw::deliverProcessedSample(TffPict &pict)
{
 if (!convert || (LONG)convert->dx!=icd->lpbiInput->biWidth || (LONG)convert->dy!=icd->lpbiInput->biHeight)
  {
   if (convert) delete convert;
   convert=new Tconvert(decVFW,icd->lpbiInput->biWidth,abs(icd->lpbiInput->biHeight));
  }
 unsigned char *dst[4]={(unsigned char*)icd->lpOutput,NULL,NULL};
 stride_t dstStride[4]={icd->lpbiInput->biWidth*csp_getInfo(colorspace)->Bpp,0,0};
 return convert->convert(pict,colorspace^(decVFW->getFlip()?FF_CSP_FLAGS_VFLIP:0),dst,dstStride)?ICERR_OK:ICERR_BADFORMAT;
}

/*
void Tffvfw::dbgError(const char *msg)
{
 initCo();
 deciEB->dbgError(msg);
}
*/
