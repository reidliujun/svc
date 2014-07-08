#include "stdafx.h"
#include "ff_wmv9.h"
#include "GenProfile_lib.h"
#include "writerSink.h"
#include "Twmv9dll.h"

#include <strmif.h>
#include <mediaobj.h>
#include "encode.h"
#include "encappErr.h"
#include "videoenc.h"
#include <uuids.h>
#include "baseclasses/amvideo.h"

#include "imgFilters/ffImgfmt.h"
#include <initguid.h>
#include "ffdshow_mediaguids.h"
#include "../../compiler.h"
#ifdef __GNUC__
 #include "strmiids.cpp"
#else
 DEFINE_GUID(IID_IMediaObject,0xd8ad0f58,0x5494,0x4102,0x97,0xc5,0xec,0x79,0x8e,0x59,0xbc,0xf4);
 DEFINE_GUID(MEDIASUBTYPE_IYUV,0x56555949,0x0,0x10,0x80,0x0,0x0,0xAA,0x0,0x38,0x9B,0x71);
#endif

#if DEBUG
#define DEBUGS(s) OutputDebugString(s)
#else
#define DEBUGS(s)
#endif

using namespace std;

bool operator <(const Tff_wmv9codecInfo &c1,const Tff_wmv9codecInfo &c2)
{
 return stricmp(c1.name,c2.name)<0;
}

struct Tff_wmv9 :public Iff_wmv9
{
private:
 Tff_wmv9cfg cfg;
 IWMProfile *pProfile;
 IWMCodecInfo3 *m_pCodecInfo;
 IWMWriter *pWMWriter;
 IWMWriterAdvanced *pWMWriterA;
 IWMWriterFileSink *filesink;
 TmyWriterSink *mysink;
 int wrinputnum;
 bool ok,writing;
 std::vector<Tff_wmv9codecInfo> codecs;
 Twmv9dll *wmv9dll;
public:
 Tff_wmv9(void)
  {
   ok=false;
   pProfile=NULL;m_pCodecInfo=NULL;
   pWMWriter=NULL;pWMWriterA=NULL;
   mysink=NULL;filesink=NULL;
   memset(&cfg,0,sizeof(cfg));

   wmv9dll=new Twmv9dll;
   if (!wmv9dll->ok) return;

   #define JIF(x) if (FAILED(hr=(x))) {DEBUGS(_l("FAILED in ")_l(#x)_l("\n"));return;}
   HRESULT hr;
   JIF(EnsureIWMCodecInfo3(wmv9dll,&m_pCodecInfo));
   DWORD codecCount;
   m_pCodecInfo->GetCodecInfoCount(WMMEDIATYPE_Video,&codecCount);
   for (DWORD i=0;i<codecCount;i++)
    {
     WCHAR codecName[1024];
     DWORD codecNameLength=1024;
     if (SUCCEEDED(m_pCodecInfo->GetCodecName(WMMEDIATYPE_Video,i,codecName,&codecNameLength)))
      {
       codecName[codecNameLength]=L'\0'; // terminate the codec name, just in case
       Tff_wmv9codecInfo info;memset(&info,0,sizeof(info));
       info.index=i;
       //WideCharToMultiByte(CP_ACP,0,codecName,-1,info.name,256,NULL,NULL);
       strcpy(info.name,text<char_t>(codecName));
       WMT_ATTR_DATATYPE type;
       BOOL isVBR;DWORD isVBRsize=sizeof(isVBR);
       if (SUCCEEDED(m_pCodecInfo->GetCodecProp(WMMEDIATYPE_Video,i,g_wszIsVBRSupported,&type,(BYTE*)&isVBR,&isVBRsize)))
        info.vbr=isVBR;
       DWORD cmplxMax,cmplxMaxSize=sizeof(cmplxMax);
       if (SUCCEEDED(m_pCodecInfo->GetCodecProp(WMMEDIATYPE_Video,i,g_wszComplexityMax,&type,(BYTE*)&cmplxMax,&cmplxMaxSize)))
        info.cmplxMax=cmplxMax;
       DWORD codecformatcount=0;
       m_pCodecInfo->GetCodecFormatCount(WMMEDIATYPE_Video,i,&codecformatcount);
       for (unsigned int j=0;j<min(DWORD(1),codecformatcount);j++)
        {
         IWMStreamConfig *wsc=NULL;
         WCHAR desc[256];DWORD desclen=256;
         if (SUCCEEDED(m_pCodecInfo->GetCodecFormatDesc(WMMEDIATYPE_Video,i,j,&wsc,desc,&desclen)))
          {
           IWMMediaProps *mp=NULL;
           if (SUCCEEDED(wsc->QueryInterface(IID_IWMMediaProps,(void**)&mp)))
            {
             DWORD typelen=0;
             if (SUCCEEDED(mp->GetMediaType(NULL,&typelen)) && typelen!=0)
              {
               WM_MEDIA_TYPE *type=(WM_MEDIA_TYPE*)malloc(typelen);
               mp->GetMediaType(type,&typelen);
               info.mediatype=type->subtype;
               info.fcc=type->subtype.Data1;
               codecs.push_back(info);
               free(type);
              }
             mp->Release();
            }
           wsc->Release();
          }
        }
      }
    }
   sort(codecs.begin(),codecs.end());
   ok=true;
  }
 virtual ~Tff_wmv9()
  {
   end();
   delete wmv9dll;
  }
 virtual bool __stdcall getOk(void) {return ok;}
 virtual size_t  __stdcall getCodecCount(void) {return codecs.size();}
 virtual bool __stdcall getCodecInfo(size_t i,const Tff_wmv9codecInfo* *info)
  {
   if (i>=codecs.size() || !info) return false;
   *info=&codecs[i];
   return true;
  }
 virtual bool __stdcall start(const Tff_wmv9cfg &Icfg)
  {
   cfg=Icfg;
   if (cfg.avioutput) return startAVI();
   writing=false;
   #undef JIF
   #define JIF(x) if (FAILED(hr=(x))) {DEBUGS(_l("FAILED in ")_l(#x)_l("\n"));return false;}
   HRESULT hr;
   IWMProfileManager *pProfileManager=NULL;
   JIF(wmv9dll->WMCreateProfileManager(&pProfileManager));
   JIF(pProfileManager->CreateEmptyProfile(WMT_VER_9_0,&pProfile));
   IWMProfile3 *pProfile3=NULL;
   JIF(pProfile->QueryInterface(IID_IWMProfile3,(void**)&pProfile3));

   JIF(pProfile->SetName(L"ff_wmv9 profile"));
   JIF(pProfile->SetDescription(L"ff_wmv9 description"));

   IWMStreamConfig *pNewStreamConfig=NULL;
   JIF(CreateVideoStream(wmv9dll,
                         &pNewStreamConfig,
                         m_pCodecInfo,
                         pProfile,
                         cfg.codecIndex,  // codec index
                         cfg.bitrate,       // bitrate
                         3000, // buffer window
                         cfg.width, // width
                         cfg.height, // height
                         cfg.fps,  // fps
                         cfg.quality, // video quality
                         cfg.seckf,  // seconds per keyframes
                         cfg.bUseVBR, // video is VBR
                         cfg.vbr_mode, // VBR mode ( VIDEO_VBR_MODE )
                         cfg.vbrquality, // VBR Quality
                         cfg.maxbitrate, // Max Bitrate (VBR)
                         10000,      // Max VBR Video Buffer
                         GetSystemDefaultLCID()));

   JIF(pNewStreamConfig->SetStreamNumber(1));
   JIF(pNewStreamConfig->SetStreamName(L"Video"));
   #define CONN_NAME L"VideoConnect"
   JIF(pNewStreamConfig->SetConnectionName(CONN_NAME));
   JIF(pNewStreamConfig->SetBufferWindow(3000));

   IWMPropertyVault *strprops=NULL;
   if (SUCCEEDED(pNewStreamConfig->QueryInterface(IID_IWMPropertyVault,(void**)&strprops)))
    {
     WORD cplx=(WORD)cfg.cplx;if (cplx>codecs[cfg.codecIndex].cmplxMax) cplx=WORD(codecs[cfg.codecIndex].cmplxMax);
     strprops->SetProperty(g_wszComplexity,WMT_TYPE_WORD,(BYTE*)&cplx,sizeof(cplx));
     strprops->Release();
    }

   JIF(pProfile->AddStream(pNewStreamConfig));
   pNewStreamConfig->Release();pNewStreamConfig=NULL;
   pProfile3->Release();pProfile3=NULL;
   pProfileManager->Release();pProfileManager=NULL;

   JIF(wmv9dll->WMCreateWriter(NULL,&pWMWriter));
   JIF(pWMWriter->QueryInterface(IID_IWMWriterAdvanced,(void**)&pWMWriterA));
   JIF(pWMWriter->SetProfile(pProfile));
   DWORD inpcnt=0;
   pWMWriter->GetInputCount(&inpcnt);
   wrinputnum=-1;
   for (DWORD i=0;i<inpcnt && wrinputnum==-1;i++)
    {
     IWMInputMediaProps *props=NULL;
     JIF(pWMWriter->GetInputProps(i,&props));
     WCHAR connNameW[256];WORD connNameLen=256;
     props->GetConnectionName(connNameW,&connNameLen);
     if (wcscmp(connNameW,CONN_NAME)==0)
      wrinputnum=i;
     props->Release();
    }
   if (wrinputnum==-1) return false;
   DWORD fmtcnt=0;
   pWMWriter->GetInputFormatCount(wrinputnum,&fmtcnt);
   WM_MEDIA_TYPE *mediatype=NULL;
   IWMInputMediaProps *props=NULL;
   for (DWORD fmti=0;fmti<fmtcnt && !mediatype;fmti++)
    {
     JIF(pWMWriter->GetInputFormat(wrinputnum,fmti,&props));
     DWORD mtsize;
     JIF(props->GetMediaType(NULL,&mtsize));
     mediatype=(WM_MEDIA_TYPE*)malloc(mtsize);
     JIF(props->GetMediaType(mediatype,&mtsize));
     if (mediatype->subtype==WMMEDIASUBTYPE_YV12) break;
     free(mediatype);mediatype=NULL;
     props->Release();props=NULL;
    }
   if (!mediatype) return false;
   //here we could modify input type settings, but they seem to be OK already
   //WMVIDEOINFOHEADER *wvih=(WMVIDEOINFOHEADER*)mediatype->pbFormat;
   // BITMAPINFOHEADER &bih=wvih->bmiHeader;
   pWMWriter->SetInputProps(wrinputnum,props);
   free(mediatype);
   props->Release();
   IWMWriterAdvanced2 *writerA2=NULL;
   if (SUCCEEDED(pWMWriter->QueryInterface(IID_IWMWriterAdvanced2,(void**)&writerA2)))
    {
     if (cfg.ivtc)
      {
       DWORD a=WM_DM_DEINTERLACE_INVERSETELECINE;
       hr=writerA2->SetInputSetting(wrinputnum,g_wszDeinterlaceMode,WMT_TYPE_DWORD,(const unsigned char *)&a,sizeof(DWORD));
      }
     if (cfg.deint)
      {
       DWORD a=WM_DM_DEINTERLACE_NORMAL;
       hr=writerA2->SetInputSetting(wrinputnum,g_wszDeinterlaceMode,WMT_TYPE_DWORD,(const unsigned char *)&a,sizeof(DWORD));
      }
     writerA2->Release();
    }

   if (0)
    {
     mysink=new TmyWriterSink(NULL,&hr);
     pWMWriterA->AddSink((IWMWriterSink*)mysink);
    }
   if (cfg.flnm)
    {
     wmv9dll->WMCreateWriterFileSink(&filesink);
     //WCHAR flnmW[MAX_PATH];
     //MultiByteToWideChar(CP_ACP,0,cfg.flnm,(int)strlen(cfg.flnm)+1,flnmW,MAX_PATH);
     filesink->Open(text<wchar_t>(cfg.flnm));
     pWMWriterA->AddSink(filesink);
    }
   JIF(pWMWriter->BeginWriting());
   writing=true;
   return true;
  }
 virtual int __stdcall write(unsigned int framenum,int csp,const unsigned char * const src[4],const stride_t srcStride[4],void *dst)
  {
   if (!writing) return 0;
   if (cfg.avioutput) return writeAVI(framenum,csp,src,srcStride,dst);
   INSSBuffer *buf=NULL;
   if (SUCCEEDED(pWMWriter->AllocateSample(3*cfg.width*cfg.height/2,&buf)))
    {
     BYTE *bufptr=NULL;
     buf->GetBuffer(&bufptr);
     DWORD buflen=3*cfg.width*cfg.height/2;
     memcpy(bufptr,src[0],buflen); //always XVID_CSP_YV12
     buf->SetLength(buflen);
     HRESULT res=pWMWriter->WriteSample(wrinputnum,QWORD(framenum*int64_t(10000000)/cfg.fps),0,buf);
     buf->Release();
     return 1;
    }
   else
    return 0;
  }
 virtual void __stdcall end(void)
  {
   if (cfg.avioutput)
    {
     endAVI();
     return;
    }
   if (pWMWriter)
    {
     if (writing) pWMWriter->EndWriting();writing=false;
     if (filesink) filesink->Release();filesink=NULL;
     if (mysink) mysink->Release();mysink=NULL;
     if (pWMWriterA) pWMWriterA->Release();pWMWriterA=NULL;
     if (pWMWriter) pWMWriter->Release();pWMWriter=NULL;
    }
   if (pProfile) pProfile->Release();pProfile=NULL;
   if (m_pCodecInfo) m_pCodecInfo->Release();m_pCodecInfo=NULL;
  }
private:
 struct Tcontext
  {
   Tcontext(void)
    {
     memset(this,0,sizeof(*this));
    }
   IMediaObject *pDMO;
   AM_MEDIA_TYPE mtIn;
   AM_MEDIA_TYPE mtOut;
   CHandlingMediaBuffer *pInputBuffer;
   CHandlingMediaBuffer *pOutputBuffer;
   REFERENCE_TIME       rtDecimatorTimeStamp;
   REFERENCE_TIME       rtDecimatorDuration;
   REFERENCE_TIME       rtMaxJitter;
   REFERENCE_TIME       rtFrameDuration;
   REFERENCE_TIME       rtTimeStamp;
   unsigned char *input;unsigned int inputSize;
   unsigned char *output;unsigned int outputSize;
  } pContext;
 VideoEncParams pParams;
 int nOutFrames;
 DWORD cbVideoOut;
 HRESULT InitializeEncoder(IMediaObject  **ppDMO,
                           AM_MEDIA_TYPE *pmtIn,
                           AM_MEDIA_TYPE *pmtOut,
                           CHandlingMediaBuffer *pMediaBuffer)
  {
   HRESULT hr=S_OK;
   if (!ppDMO ||!pmtIn || !pmtOut) return E_INVALIDARG;
   hr=InitializeVideoEncoder(pmtIn,
                             &pParams,
                             ppDMO,
                             pmtOut,
                             pMediaBuffer);
   return ( hr );
  }
 bool startAVI(void)
  {
   cbVideoOut = max(128 * 128 * 2,
                    (((cfg.width  + 15) & ~15) *
                     ((cfg.height + 15) & ~15) * 3));
   HRESULT  hr=S_OK;
   DWORD dwFlags;
   VIDEOINFOHEADER *vih=(VIDEOINFOHEADER*)CoTaskMemAlloc(sizeof(VIDEOINFOHEADER));
   vih->rcSource.left=0;vih->rcSource.top=0;vih->rcSource.right=cfg.width;vih->rcSource.bottom=cfg.height;
   vih->rcTarget=vih->rcSource;
   vih->dwBitRate=DWORD(3*cfg.fps*cfg.width*cfg.height/2);
   vih->dwBitErrorRate=0;
   vih->AvgTimePerFrame=REFERENCE_TIME(10000000.0/cfg.fps);
   vih->bmiHeader.biSize=sizeof(vih->bmiHeader);
   vih->bmiHeader.biWidth=cfg.width;
   vih->bmiHeader.biHeight=cfg.height;
   vih->bmiHeader.biPlanes=1;
   const TcspInfo *cspInfo=csp_getInfo(cfg.csp);
   vih->bmiHeader.biBitCount=WORD(cspInfo->bpp);
   vih->bmiHeader.biCompression=cspInfo->fcc;
   vih->bmiHeader.biSizeImage=cspInfo->bpp*cfg.width*cfg.height/8;
   pContext.mtIn.subtype=*cspInfo->subtype;
   vih->bmiHeader.biXPelsPerMeter=0;
   vih->bmiHeader.biYPelsPerMeter=0;
   vih->bmiHeader.biClrUsed=0;
   vih->bmiHeader.biClrImportant=0;
   pContext.pDMO=NULL;
   pContext.pInputBuffer=new CHandlingMediaBuffer;
   pContext.mtIn.majortype=MEDIATYPE_Video;
   pContext.mtIn.bFixedSizeSamples=TRUE;
   pContext.mtIn.bTemporalCompression=FALSE;
   pContext.mtIn.formattype=FORMAT_VideoInfo;
   pContext.mtIn.lSampleSize=vih->bmiHeader.biSizeImage;
   pContext.mtIn.pUnk=NULL;
   pContext.mtIn.cbFormat=sizeof(VIDEOINFOHEADER);
   pContext.mtIn.pbFormat=(unsigned char*)vih;
   pContext.input=(unsigned char*)malloc(pContext.inputSize=pContext.mtIn.lSampleSize);

   pContext.pOutputBuffer=new CHandlingMediaBuffer;
   memset(&pContext.mtOut,0,sizeof(pContext.mtOut));

   pParams.nBitrate=cfg.bitrate;
   pParams.nWidth=cfg.width;
   pParams.nHeight=cfg.height;
   pParams.dFrameRate=cfg.fps;
   pParams.nKeyDist=cfg.seckf*1000;
   pParams.nBufferDelay=5000;
   pParams.nCrisp=cfg.crisp;
   pParams.nQuality=cfg.quality;
   pParams.nPeakBuffer=5000;
   pParams.nPeakBitrate=cfg.maxbitrate;
   pParams.nComplexity=cfg.cplx;
   pParams.nProfile=P_MAIN;
   pParams.fIsVBR=cfg.bUseVBR;
   pParams.fIsConstrained=FALSE;
   pParams.fIsInterlaced=cfg.deint;
   pParams.nPasses=1;
   for (vector<Tff_wmv9codecInfo>::const_iterator c=codecs.begin();c!=codecs.end();c++)
    if (c->index==cfg.codecIndex)
     {
      pParams.dwTag=c->fcc;
      break;
     }
   hr=InitializeEncoder(&pContext.pDMO,
                        &pContext.mtIn,
                        &pContext.mtOut,
                        pContext.pInputBuffer);
   if (FAILED(hr)) return false;

   //
   // The codecs used in this sample don't perform lookahead on the incoming data.
   // If this changes the scheme used here may not work.
   // FAIL is the encoder uses lookahead.
   //
   hr = pContext.pDMO->GetInputStreamInfo( 0, &dwFlags );
   if( FAILED( hr ) ) return false;
   if( dwFlags & DMO_INPUT_STREAMF_HOLDS_BUFFERS ) return false;
   //
   //copy AvgTimePerFrame from mtIn to the context
   //
   if( FORMAT_VideoInfo == pContext.mtIn.formattype )
    {
     if( 0 == pContext.rtFrameDuration )
      {
       pContext.rtFrameDuration = ((VIDEOINFOHEADER*)pContext.mtIn.pbFormat)->AvgTimePerFrame;
      }
     if( pParams.dFrameRate > 0 )
      {
       pContext.rtDecimatorDuration = (REFERENCE_TIME)(10000000.0 / pParams.dFrameRate );
      }
    }
#define   MAX_JITTER_PERCENT       20
#define   JITTER_OFFSET            50000

   pContext.rtMaxJitter = ( pContext.rtFrameDuration * MAX_JITTER_PERCENT ) / 100;
   pContext.rtMaxJitter += pContext.rtFrameDuration;
   pContext.rtDecimatorTimeStamp=0;

   nOutFrames=0;
   writing=true;
   return true;
  }
 void __stdcall getExtradata(const void* *ptr,size_t *len)
  {
   unsigned int dif;
   if (pContext.mtOut.cbFormat==0 || (dif=pContext.mtOut.cbFormat-sizeof(VIDEOINFOHEADER))==0)
    {
     *ptr=NULL;
     *len=NULL;
    }
   else
    {
     if (ptr) *ptr=pContext.mtOut.pbFormat+sizeof(VIDEOINFOHEADER);
     *len=dif;
    }
  };
 HRESULT FeedData( IMediaObject         *pDMO,
                   BYTE                 *pbData,
                   LONG                 cbData,
                   REFERENCE_TIME       rtStart,
                   REFERENCE_TIME       rtLength,
                   CHandlingMediaBuffer *pMediaBuffer )
  {
   HRESULT hr  = S_OK;
   DWORD   dwFlags = DMO_INPUT_DATA_BUFFERF_SYNCPOINT  |
                     DMO_INPUT_DATA_BUFFERF_TIME       |
                     DMO_INPUT_DATA_BUFFERF_TIMELENGTH;
   DWORD   dwStatus;

   if( NULL == pDMO         ||
       NULL == pbData       ||
       NULL == pMediaBuffer ||
       0    >= cbData   )
    {
     return ( E_INVALIDARG );
    }

   hr = pDMO->GetInputStatus(0, &dwStatus);
   if (FAILED(hr))
    {
     return ( hr );
    }

   if (!(dwStatus & DMO_INPUT_STATUSF_ACCEPT_DATA))
    {
     return ( E_DMO_NOTACCEPTING );
    }
   //
   //set the params for the input buffer
   //
   pMediaBuffer->set_Buffer( pbData,
                             cbData,
                             cbData );

   return pDMO->ProcessInput(0, pMediaBuffer, dwFlags, rtStart, rtLength );
  }
  HRESULT FeedFrame( LONG nFrame)
  {

   REFERENCE_TIME  rtStartIn;
   REFERENCE_TIME  rtLenIn;

   //DWORD   dwStatus    = 0;
   HRESULT hr          = S_OK;

   //
   //Compute the time stamps and decide if the frame should be decimated
   //
   rtLenIn = REFERENCE_TIME(10000000.0 * (1/cfg.fps));
   rtStartIn = REFERENCE_TIME(nFrame * 10000000.0 * (1/cfg.fps));
   if( pContext.rtDecimatorDuration > 0 )
    {
     if( rtStartIn >= pContext.rtDecimatorTimeStamp - ( JITTER_OFFSET ) )
      {
       pContext.rtDecimatorTimeStamp = rtStartIn + pContext.rtDecimatorDuration;
      }
     else
      {
       return ( S_FALSE );   // this frame is dropped
      }
    }

   hr = FeedData( pContext.pDMO,
                  pContext.input,
                  pContext.inputSize,
                  rtStartIn,
                  rtLenIn,
                  pContext.pInputBuffer );

   return ( hr );
  }
 HRESULT PullData( IMediaObject         *pDMO,
                   BYTE                 *pbData,
                   LONG                 cbData,
                   DWORD                *pdwStatus,
                   REFERENCE_TIME       *prtStart,
                   REFERENCE_TIME       *prtLength,
                   CHandlingMediaBuffer *pMediaBuffer )
  {
   DMO_OUTPUT_DATA_BUFFER OutputBufferStructs[1];
   HRESULT                hr             = S_OK;
   DWORD                  dwStatus       = 0;
   //DWORD                  cbProduced     = NULL;

   if( NULL == pDMO         ||
       NULL == pMediaBuffer ||
       NULL == pdwStatus    ||
       NULL == pbData       ||
       0    >= cbData       )
    {
     return ( E_INVALIDARG );
    }

   *pdwStatus = 0;

   pMediaBuffer->set_Buffer( pbData, 0, cbData );

   OutputBufferStructs[0].pBuffer = pMediaBuffer;
   OutputBufferStructs[0].dwStatus = 0;

   hr = pDMO->ProcessOutput(0, 1, OutputBufferStructs, &dwStatus);
   if( S_OK != hr )
    {
     return ( hr );
    }

   *pdwStatus = OutputBufferStructs[0].dwStatus;

   if( NULL != prtStart )
    {
     *prtStart = OutputBufferStructs[0].rtTimestamp;
    }
   if( NULL != prtLength )
    {
     *prtLength = OutputBufferStructs[0].rtTimelength;
    }

   return ( hr );
  }
 int writeAVI(unsigned int framenum,int csp,const unsigned char * const src[4],const stride_t srcStride[4],void *dst)
  {
   if (csp&FF_CSP_FLAGS_YUV_ADJ)
    memcpy(pContext.input,src[0],(csp&FF_CSP_420P)?3*cfg.width*cfg.height/2:srcStride[0]*cfg.height);
   else
    {
     int Bpp=csp_getInfo(csp)->Bpp;
     for (int y=0;y<cfg.height;y++)
      memcpy(pContext.input+cfg.width*Bpp*y,src[0]+srcStride[0]*y,cfg.width*Bpp);
    }

   HRESULT hr=FeedFrame(framenum);
   if (S_FALSE==hr || FAILED( hr )) return 0;
   DWORD           dwVideoStatus;
   REFERENCE_TIME  rtStartOut;
   REFERENCE_TIME  rtLenOut;

   hr = PullData( pContext.pDMO,
                  (BYTE*)dst,
                  cbVideoOut,
                  &dwVideoStatus,
                  &rtStartOut,
                  &rtLenOut,
                  pContext.pOutputBuffer );
   if( S_OK != hr )
    return 0;
   DWORD ulLength;
   hr=pContext.pOutputBuffer->GetBufferAndLength( NULL, &ulLength );
   if( S_OK != hr || 0 == ulLength )
    return 0;
   /*
   while( nOutFrames > 0 && rtStartOut - pContext.rtTimeStamp >= pContext.rtMaxJitter )
    {
     hr = AVIStreamWrite( pVideoContext->pAviOutStream,
                          (*pnOutFrames)++,
                          1,
                          NULL,
                          0,
                          0,
                          NULL,
                          NULL );
     if( FAILED( hr ) )
      {
       break;
      }
     pVideoContext->rtTimeStamp += pVideoContext->rtFrameDuration;
    }
   if( FAILED( hr ) )
    {
     break;
    }*/

   return (dwVideoStatus&DMO_OUTPUT_DATA_BUFFERF_SYNCPOINT)?-ulLength:ulLength;
  }
 void endAVI(void)
  {
   if( NULL != pContext.pDMO )
    {
     SAFERELEASE( pContext.pDMO );
    }
   if( NULL != pContext.mtOut.pbFormat )
    {
     CoTaskMemFree( pContext.mtOut.pbFormat );
     memset( &pContext.mtOut, 0, sizeof( AM_MEDIA_TYPE ) );
    }
   if( NULL != pContext.mtIn.pbFormat )
    {
     CoTaskMemFree( pContext.mtIn.pbFormat );
     memset( &pContext.mtIn, 0, sizeof( AM_MEDIA_TYPE ) );
    }
   if (pContext.input ) free(pContext.input );pContext.input =NULL;//SAFEDELETE ( pContext.input );
   if (pContext.output) free(pContext.output);pContext.output=NULL;//SAFEDELETE ( pContext.output);
   SAFEDELETES( pContext.pInputBuffer );
   SAFEDELETES( pContext.pOutputBuffer );
  };
private:
 REFERENCE_TIME decRtLength;
 int decmult;
public:
 virtual const Tff_wmv9codecInfo* __stdcall findCodec(FOURCC fcc)
  {
   for (vector<Tff_wmv9codecInfo>::const_iterator c=codecs.begin();c!=codecs.end();c++)
    if (c->fcc==fcc)
     return &*c;
   return NULL;
  }
 virtual bool __stdcall decStart(FOURCC fcc,double fps,unsigned int dx,unsigned int dy,const void *extradata,size_t extradata_len,int *csp)
  {
   cfg.avioutput=true;
   cfg.width=dx;cfg.height=dy;cfg.fps=fps;
   decRtLength=REFERENCE_TIME(10000000.0/cfg.fps);
   HRESULT  hr=S_OK;
   vector<Tff_wmv9codecInfo>::const_iterator c;
   for (c=codecs.begin();c!=codecs.end();c++)
    if (c->fcc==fcc)
     break;
   pContext.pDMO=NULL;
   VIDEOINFOHEADER *vih=(VIDEOINFOHEADER*)CoTaskMemAlloc(sizeof(VIDEOINFOHEADER)+extradata_len);
   vih->rcSource.left=0;vih->rcSource.top=0;vih->rcSource.right=dx;vih->rcSource.bottom=dy;
   vih->rcTarget=vih->rcSource;
   vih->dwBitRate=DWORD(cfg.fps*dx*dy);
   vih->dwBitErrorRate=0;
   vih->AvgTimePerFrame=decRtLength;
   vih->bmiHeader.biSize=DWORD(sizeof(vih->bmiHeader)+extradata_len);
   vih->bmiHeader.biWidth=dx;
   vih->bmiHeader.biHeight=dy;
   vih->bmiHeader.biPlanes=1;
   vih->bmiHeader.biBitCount=24;
   vih->bmiHeader.biCompression=c->fcc;
   vih->bmiHeader.biSizeImage=3*dx*dy;
   vih->bmiHeader.biXPelsPerMeter=0;
   vih->bmiHeader.biYPelsPerMeter=0;
   vih->bmiHeader.biClrUsed=0;
   vih->bmiHeader.biClrImportant=0;
   memcpy((unsigned char*)(vih+1),extradata,extradata_len);
   pContext.pInputBuffer=new CHandlingMediaBuffer;
   pContext.mtIn.majortype=MEDIATYPE_Video;
   pContext.mtIn.subtype=c->mediatype;
   pContext.mtIn.bFixedSizeSamples=FALSE;
   pContext.mtIn.bTemporalCompression=TRUE;
   pContext.mtIn.lSampleSize=0;
   pContext.mtIn.formattype=FORMAT_VideoInfo;
   pContext.mtIn.pUnk=NULL;
   pContext.mtIn.cbFormat=DWORD(sizeof(VIDEOINFOHEADER)+extradata_len);
   pContext.mtIn.pbFormat=(unsigned char*)vih;

   vih=(VIDEOINFOHEADER*)CoTaskMemAlloc(sizeof(VIDEOINFOHEADER));
   vih->rcSource.left=0;vih->rcSource.top=0;vih->rcSource.right=dx;vih->rcSource.bottom=dy;
   vih->rcTarget=vih->rcSource;
   vih->dwBitRate=DWORD(cfg.fps*3*dx*dy/2);
   vih->dwBitErrorRate=0;
   vih->AvgTimePerFrame=decRtLength;
   vih->bmiHeader.biSize=sizeof(vih->bmiHeader);
   vih->bmiHeader.biWidth=dx;
   vih->bmiHeader.biHeight=dy;
   vih->bmiHeader.biPlanes=1;
   switch (fcc)
    {
     case WMCFOURCC_MSS1:
     case WMCFOURCC_MSS2:
      vih->bmiHeader.biBitCount=24;
      vih->bmiHeader.biCompression=0;
      pContext.mtOut.subtype=MEDIASUBTYPE_RGB24;
      vih->bmiHeader.biSizeImage=3*dx*dy;
      *csp=FF_CSP_RGB24|FF_CSP_FLAGS_VFLIP;
      decmult=3;
      break;
     default:
      vih->bmiHeader.biBitCount=12;
      vih->bmiHeader.biCompression=WMCFOURCC_YV12;
      pContext.mtOut.subtype=MEDIASUBTYPE_YV12;
      vih->bmiHeader.biSizeImage=3*dx*dy/2;
      *csp=FF_CSP_420P|FF_CSP_FLAGS_YUV_ADJ;
      decmult=1;
      break;
    }
   vih->bmiHeader.biXPelsPerMeter=0;
   vih->bmiHeader.biYPelsPerMeter=0;
   vih->bmiHeader.biClrUsed=0;
   vih->bmiHeader.biClrImportant=0;
   pContext.pOutputBuffer=new CHandlingMediaBuffer;
   pContext.mtOut.majortype=MEDIATYPE_Video;
   pContext.mtOut.bFixedSizeSamples=TRUE;
   pContext.mtOut.bTemporalCompression=FALSE;
   pContext.mtOut.lSampleSize=vih->bmiHeader.biSizeImage;
   pContext.mtOut.formattype=FORMAT_VideoInfo;
   pContext.mtOut.pUnk=NULL;
   pContext.mtOut.cbFormat=sizeof(VIDEOINFOHEADER);
   pContext.mtOut.pbFormat=(unsigned char*)vih;

   CLSID decid;
   switch (fcc)
    {
     case WMCFOURCC_MSS1:
     case WMCFOURCC_MSS2:
      decid=CLSID_CMSSCDecMediaObject;
      break;
     case WMCFOURCC_WMVA:
      decid=CLSID_CWMVDecMediaObject2;
      break;
     default:
     case WMCFOURCC_WMV1:
     case WMCFOURCC_WMV2:
     case WMCFOURCC_WMV3:
     case WMCFOURCC_WVC1:
     case WMCFOURCC_WMVP:
     case WMCFOURCC_WVP2:
      decid=CLSID_CWMVDecMediaObject;
      break;
    }
   hr = CoCreateInstance( decid,
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          IID_IMediaObject,
                          (void**)&pContext.pDMO);
   if (FAILED(hr)) return false;
   hr = pContext.pDMO->SetInputType(0, &pContext.mtIn, 0);
   if (FAILED(hr)) return false;
   hr = pContext.pDMO->SetOutputType(0, &pContext.mtOut, 0);
   if (FAILED(hr)) return false;
   pContext.output=(unsigned char*)malloc(pContext.outputSize=vih->bmiHeader.biSizeImage);
   return true;
  }
 virtual size_t __stdcall decompress(const unsigned char *src,size_t srcLen,unsigned char* *dst,stride_t *dstStride)
  {
   *dst=NULL;
   HRESULT hr;
   DWORD dwStatus;
   hr = pContext.pDMO->GetInputStatus(0, &dwStatus);
   if (FAILED(hr)) return 0;
   if (!(dwStatus & DMO_INPUT_STATUSF_ACCEPT_DATA)) return 0;
   pContext.pInputBuffer->set_Buffer((BYTE*)src, (DWORD)srcLen, (DWORD)srcLen );
   DWORD dwFlags = DMO_INPUT_DATA_BUFFERF_SYNCPOINT| DMO_INPUT_DATA_BUFFERF_TIME|DMO_INPUT_DATA_BUFFERF_TIMELENGTH;
   REFERENCE_TIME rtStart=0;
   hr=pContext.pDMO->ProcessInput(0, pContext.pInputBuffer, dwFlags, rtStart, decRtLength );
   if (S_FALSE==hr || FAILED( hr )) return srcLen;

   pContext.pOutputBuffer->set_Buffer( pContext.output, 0, pContext.outputSize );
   DMO_OUTPUT_DATA_BUFFER OutputBufferStructs[1];
   OutputBufferStructs[0].pBuffer = pContext.pOutputBuffer;
   OutputBufferStructs[0].dwStatus = 0;

   hr = pContext.pDMO->ProcessOutput(0, 1, OutputBufferStructs, &dwStatus);
   *dst=pContext.output;
   *dstStride=cfg.width*decmult;
   return srcLen;
  }
};

//===========================================================================================
extern "C" Iff_wmv9* __stdcall createWmv9(void)
{
 return new Tff_wmv9;
}
extern "C" void __stdcall destroyWmv9(Iff_wmv9 *self)
{
 if (self) delete (Tff_wmv9*)self;
}
extern "C" void __stdcall getVersion(char *ver,const char* *license)
{
 strcpy(ver,"2.7a, build date "__DATE__" "__TIME__" ("COMPILER COMPILER_X64")");
 *license="";
}
