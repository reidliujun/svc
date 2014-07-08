//*@@@+++@@@@******************************************************************
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

//
//  Interlace encoding mode is disabled by default.
//  Interlace WMV 9 (4cc WMV3) bit streams should not
//  currently be created in AVI, because AVI does not
//  provide the ability to specify top-field-first or
//  bottom-field-first at frame level. However, the code
//  is provided in case the user may want to wrap interlace
//  WMV 9 streams in other file formats.
//

#include "stdafx.h"
#include <strmif.h>
#include "baseclasses/amvideo.h"
#include <uuids.h>
#include "baseclasses/dvdmedia.h"
#include <wmcodeciface.h>
#include "wmsbuffer.h"
#include "encappErr.h"
#include "videoenc.h"
#include "encode.h"

//////////////////////////////////////////////////////////////////////////////
HRESULT MakeVideoOutputType(IMediaObject   *pDMO,
                            AM_MEDIA_TYPE  *pmtIn,
                            VideoEncParams *pParams,
                            AM_MEDIA_TYPE  *pmt      )
{

 HRESULT             hr                   = S_OK;
 VIDEOINFOHEADER2    *pvih2               = NULL;
 VIDEOINFOHEADER     vih;
 IWMCodecPrivateData *pWMCodecPrivateData = NULL;
 DWORD               cbPrivateData        = 0;
 BYTE                *pbPrivateData       = NULL;
 BYTE                *pNewFormat          = NULL;

 if( NULL == pDMO    ||
     NULL == pmtIn   ||
     NULL == pParams ||
     NULL == pmt     )
  {
   return ( E_INVALIDARG );
  }

 if( NULL == pmtIn->pbFormat || pmtIn->cbFormat < sizeof( VIDEOINFOHEADER ) )
  {
   return ( E_INVALIDARG );
  }

 // make up a partial media type
 pmt->majortype            = MEDIATYPE_Video;
 pmt->formattype           = FORMAT_VideoInfo;
 pmt->bFixedSizeSamples    = FALSE;
 pmt->bTemporalCompression = TRUE;

 if( pmtIn->formattype == FORMAT_VideoInfo )
  {
   vih = *(VIDEOINFOHEADER*)pmtIn->pbFormat;
  }
 else if( pmtIn->formattype == FORMAT_VideoInfo2 )
  {
   pvih2 = (VIDEOINFOHEADER2*)pmtIn->pbFormat;
   vih.rcSource        = pvih2->rcSource;
   vih.rcTarget        = pvih2->rcTarget;
   vih.AvgTimePerFrame = pvih2->AvgTimePerFrame;
   vih.bmiHeader       = pvih2->bmiHeader;
  }
 else
  return ( E_VIDEO_INVALID_INPUT_TYPE );

 vih.dwBitRate            = (DWORD)pParams->nBitrate;
 vih.dwBitErrorRate       = 0;
 vih.bmiHeader.biPlanes   = 1;
 vih.bmiHeader.biBitCount = 24;
 pmt->subtype = WMCMEDIASUBTYPE_WMV1;
 pmt->subtype.Data1=pParams->dwTag;
 vih.bmiHeader.biCompression = pParams->dwTag;
 //
 //use the partial format above to get the private data
 //
 pmt->pbFormat = (BYTE*)CoTaskMemAlloc( sizeof( VIDEOINFOHEADER));
 memcpy( pmt->pbFormat , &vih, sizeof( VIDEOINFOHEADER));
 pmt->cbFormat = sizeof( vih );

 do
  {
   hr = pDMO->QueryInterface(IID_IWMCodecPrivateData, (void**)&pWMCodecPrivateData);
   if( FAILED( hr ) )
    {
     hr = E_NO_PRIVATE_DATA;
     break;
    }

   hr = pWMCodecPrivateData->SetPartialOutputType( pmt );
   if( FAILED( hr ) )
    {
     hr = E_PARTIAL_TYPE_REJECTED;
     break;
    }

   hr = pWMCodecPrivateData->GetPrivateData( NULL, &cbPrivateData );
   if( FAILED( hr ) )
    {
     hr = E_NO_PRIVATE_DATA_COUNT;
     break;
    }

   if( 0 == cbPrivateData )
    { // No private data
     hr = S_OK;
     break;
    }
   pbPrivateData = new BYTE[ cbPrivateData ];
   if( pbPrivateData == NULL )
    {
     hr = E_OUTOFMEMORY;
     break;
    }
   //
   // get the private data
   //
   hr = pWMCodecPrivateData->GetPrivateData( pbPrivateData, &cbPrivateData );
   if( FAILED( hr ) )
    {
     hr = E_PRIVATE_DATA_FAILED;
     break;
    }

   //
   //modify the media type accordingly
   //
   pNewFormat = (BYTE*)CoTaskMemAlloc( sizeof( VIDEOINFOHEADER) + cbPrivateData );
   if( NULL == pNewFormat )
    {
     hr = E_OUTOFMEMORY;
     break;
    }

   memcpy( pNewFormat, pmt->pbFormat, sizeof( VIDEOINFOHEADER));
   CoTaskMemFree(pmt->pbFormat);
   memcpy( pNewFormat + sizeof( VIDEOINFOHEADER), pbPrivateData, cbPrivateData);
   pmt->pbFormat = pNewFormat;
   pmt->cbFormat = sizeof( VIDEOINFOHEADER) + cbPrivateData;
   ((VIDEOINFOHEADER*)pmt->pbFormat)->bmiHeader.biSize         += cbPrivateData;
   ((VIDEOINFOHEADER*)pmt->pbFormat)->bmiHeader.biClrUsed       = 0;
   ((VIDEOINFOHEADER*)pmt->pbFormat)->bmiHeader.biClrImportant  = 0;
   ((VIDEOINFOHEADER*)pmt->pbFormat)->bmiHeader.biXPelsPerMeter = 0;
   ((VIDEOINFOHEADER*)pmt->pbFormat)->bmiHeader.biYPelsPerMeter = 0;
  }
 while( FALSE );

 SAFERELEASE( pWMCodecPrivateData );
 SAFEDELETE( pbPrivateData );

 return ( hr );
}

//////////////////////////////////////////////////////////////////////////////
HRESULT SetVideoTypes( IMediaObject   *pDMO,    \
                       AM_MEDIA_TYPE  *pmtIn,   \
                       VideoEncParams *pParams, \
                       AM_MEDIA_TYPE  *pmtOut,  \
                       DWORD *pcbIn,            \
                       DWORD *pcbOut )
{
 HRESULT hr;
 DWORD   dwDummy;

 hr = pDMO->SetInputType(0, pmtIn, 0);
 if( S_OK != hr )
  {
   return ( hr );
  }

 hr = MakeVideoOutputType( pDMO, pmtIn, pParams, pmtOut );
 if( FAILED( hr ) )
  {
   return ( hr );
  }

 hr = pDMO->SetOutputType(0, pmtOut, 0);
 if( S_OK != hr )
  {
   return ( hr );
  }

 hr = pDMO->GetInputSizeInfo( 0, pcbIn, &dwDummy, &dwDummy);
 if( FAILED(hr) )
  {
   return ( hr );
  }

 hr = pDMO->GetOutputSizeInfo( 0, pcbOut, &dwDummy);
 if( FAILED( hr ) )
  {
   return ( hr );
  }

 return ( hr == S_FALSE ? E_VIDEO_TYPE_NOT_SET: hr );
}

//////////////////////////////////////////////////////////////////////////////
HRESULT SetVideoParams( IMediaObject *pDMO, VideoEncParams *pParams )
{
 HRESULT      hr;
 VARIANT      varg;
 IPropertyBag *pPropertyBag = NULL;

 if( NULL == pDMO || NULL == pParams )
  {
   return ( E_INVALIDARG );
  }

 do
  {
   //
   // Get the IPropertyBag IF and set the appropriate params
   //
   hr = pDMO->QueryInterface(IID_IPropertyBag, (void**)&pPropertyBag);
   if( FAILED( hr ) )
    {
     break;
    }
   //
   //set the encoder in VBR mode if required
   //
   if( pParams->fIsVBR == TRUE )
    {
     ::VariantInit(&varg);
     varg.vt      = VT_BOOL;
     varg.boolVal = TRUE;

     hr = pPropertyBag->Write( g_wszWMVCVBREnabled, &varg );
     if( FAILED( hr ) )
      {
       hr = E_VIDEO_VBR_NOT_SUPPORTED;
       break;
      }
     //
     // if it is 1 pass VBR set the Quality param
     //
     if( pParams->nPasses == 1 )
      {
       ::VariantInit(&varg);
       varg.vt = VT_I4;
       varg.lVal = pParams->nVBRQuality;
       hr = pPropertyBag->Write( g_wszWMVCVBRQuality, &varg );
       if( FAILED( hr ) )
        {
         hr = E_VBR_QUALITY_REJECTED;
         break;
        }
      }
    }
   //
   //set the number of passes
   //
   if( pParams->nPasses > 1 )
    {
     ::VariantInit(&varg);
     varg.vt   = VT_I4;
     varg.lVal = pParams->nPasses;

     hr = pPropertyBag->Write( g_wszWMVCPassesUsed, &varg );
     if( FAILED( hr ) )
      {
       hr = E_VIDEO_NPASS_NOT_SUPPORTED;
       break;
      }
    }
   //
   // set the bitrate for all the modes except 1 pass VBR
   //
   if( !( pParams->fIsVBR == TRUE && pParams->nPasses == 1 ) )
    {
     ::VariantInit(&varg);
     varg.vt = VT_I4;
     varg.lVal = pParams->nBitrate;
     hr = pPropertyBag->Write( g_wszWMVCAvgBitrate, &varg );
     if( FAILED( hr ) )
      {
       hr = E_VIDEO_BITRATE_REJECTED;
       break;
      }
    }
   //
   // set the buffer window
   //
   ::VariantInit(&varg);
   varg.vt = VT_I4;
   varg.lVal = pParams->nBufferDelay;
   hr = pPropertyBag->Write( g_wszWMVCVideoWindow, &varg );
   if( FAILED( hr ) )
    {
     hr = E_VIDEO_BUFFER_REJECTED;
     break;
    }

   if( pParams->fIsConstrained == TRUE )
    {
     ::VariantInit(&varg);
     varg.vt   = VT_I4;
     varg.lVal = pParams->nPeakBitrate;

     hr = pPropertyBag->Write( g_wszWMVCMaxBitrate, &varg );
     if( FAILED( hr ) )
      {
       hr = E_VIDEO_PEAK_BITRATE_REJECTED;
       break;
      }

     ::VariantInit(&varg);
     varg.vt   = VT_I4;
     varg.lVal = pParams->nPeakBuffer;

     hr = pPropertyBag->Write( g_wszWMVCBMax, &varg );
     if( FAILED( hr ) )
      {
       hr = E_VIDEO_PEAK_BUFFER_REJECTED;
       break;
      }
    }

   //
   // set the profile for WMV# only
   //
   hr = S_OK;
   BSTR bstrIn=NULL;
   if( pParams->dwTag == WMCFOURCC_WMV3 )
    {
     switch( pParams->nProfile )
      {
      case P_MAIN:
       bstrIn = ::SysAllocString(L"MP");
       break;
      case P_SIMPLE:
       bstrIn = ::SysAllocString(L"SP");
       break;
      case P_COMPLEX:
       bstrIn = ::SysAllocString(L"CP");
       break;
      default:
       hr = E_VIDEO_INVALID_PROFILE;
       break;
      }
     if( FAILED( hr ) ) break;

     ::VariantInit(&varg);
     varg.vt      = VT_BSTR;
     varg.bstrVal = bstrIn;
     hr = pPropertyBag->Write( g_wszWMVCDecoderComplexityRequested, &varg );
     ::SysFreeString( bstrIn );
     if( FAILED( hr ) )
      {
       hr = E_VIDEO_PROFILE_REJECTED;
       break;
      }
    }

   //
   // set the complexity param
   //
   ::VariantInit(&varg);
   varg.vt = VT_I4;
   varg.lVal = pParams->nComplexity;
   hr = pPropertyBag->Write( g_wszWMVCComplexityEx, &varg );
   if( FAILED( hr ) )
    {
     hr = E_VIDEO_KEYDIST_REJECTED;
     break;
    }

   //
   // set the max distance between the key frames
   //
   ::VariantInit(&varg);
   varg.vt = VT_I4;
   varg.lVal = pParams->nKeyDist;
   hr = pPropertyBag->Write( g_wszWMVCKeyframeDistance, &varg );
   if( FAILED( hr ) )
    {
     hr = E_VIDEO_KEYDIST_REJECTED;
     break;
    }

   //
   // set the crispness params for WMV# only
   //
   if( pParams->dwTag == WMCFOURCC_WMV3 || pParams->dwTag == WMCFOURCC_WMV2 || pParams->dwTag == WMCFOURCC_WMV1 )
    {
     ::VariantInit(&varg);
     varg.vt = VT_I4;
     varg.lVal = pParams->nCrisp;
     hr = pPropertyBag->Write( g_wszWMVCCrisp, &varg );
     if( FAILED( hr ) )
      {
       hr = E_VIDEO_CRISPNESS_REJECTED;
       break;
      }
    }

   if ( pParams->fIsVBR)
    {
     ::VariantInit(&varg);
     varg.vt = VT_I4;
     varg.lVal = pParams->nQuality;
     hr = pPropertyBag->Write( g_wszWMVCVBRQuality, &varg );
     if( FAILED( hr ) )
      {
       hr = E_VIDEO_QUALITY_REJECTED;
       break;
      }
    }

#ifdef SUPPORT_INTERLACE
   //
   // set the interlace mode for WMV# only
   //
   if( pParams->dwTag == WMCFOURCC_WMV3 && pParams->fIsInterlaced)
    {
     ::VariantInit(&varg);
     varg.vt = VT_BOOL;
     varg.boolVal = TRUE;
     hr = pPropertyBag->Write( g_wszWMVCInterlacedCodingEnabled, &varg );
     if( FAILED( hr ) )
      {
       hr = E_VIDEO_INTERLACE_REJECTED;
       break;
      }
    }
#endif //SUPPORT_INTERLACE

  }
 while( FALSE );
 SAFERELEASE( pPropertyBag );
 return ( hr );
}

//////////////////////////////////////////////////////////////////////////////
// a decent guess would be 0.4 bits per pixel
HRESULT DefaultVideoBitrate( AM_MEDIA_TYPE *pmt, double dFramesPerSec, int *pBitrate )
{
 if( NULL == pmt || NULL == pBitrate )
  {
   return ( E_INVALIDARG );
  }

 if( NULL == pmt->pbFormat || pmt->cbFormat <= 0 )
  {
   return ( E_INVALIDARG );
  }

 if( FORMAT_VideoInfo == pmt->formattype )
  {
   VIDEOINFOHEADER *pvih = (VIDEOINFOHEADER*)pmt->pbFormat;

   *pBitrate = ( pvih->bmiHeader.biWidth * pvih->bmiHeader.biHeight * 2 ) / 5;
   if( dFramesPerSec == 0 )
    { // use the input rate
     if( pvih->AvgTimePerFrame > 0 )
      {
       dFramesPerSec = 10000000.0 / pvih->AvgTimePerFrame;
      }
     else
      {
       dFramesPerSec = 29.97;   // 30 frames per sec
      }
    }
  }
 else if( FORMAT_VideoInfo2 == pmt->formattype  )
  {
   VIDEOINFOHEADER2 *pvih = (VIDEOINFOHEADER2*)pmt->pbFormat;

   *pBitrate = ( pvih->bmiHeader.biWidth * pvih->bmiHeader.biHeight * 2 ) / 5;

   if( dFramesPerSec == 0 )
    { // use the input rate
     if( pvih->AvgTimePerFrame > 0 )
      {
       dFramesPerSec = 10000000.0 / pvih->AvgTimePerFrame;
      }
     else
      {
       dFramesPerSec = 29.97;   // 30 frames per sec
      }
    }
  }
 else
  {
   return ( E_VIDEO_INVALID_INPUT_TYPE );
  }

 *pBitrate = (int)( *pBitrate * dFramesPerSec );
 return ( S_OK );
}

//////////////////////////////////////////////////////////////////////////////
HRESULT InitializeVideoEncoder(  AM_MEDIA_TYPE      *pmtInput,
                                 VideoEncParams       *pParams,
                                 IMediaObject         **ppDMO,
                                 AM_MEDIA_TYPE        *pmtOutput,
                                 CHandlingMediaBuffer *pMediaBuffer )
{
 HRESULT       hr             = S_OK;
 DWORD         cbInputBuffer  = 0;
 DWORD         cbOutputBuffer = 0;

 if( NULL == pmtInput || NULL == pParams || NULL == ppDMO )
  {
   return ( E_INVALIDARG );
  }

#ifdef SUPPORT_INTERLACE
 //
 // Interlace encoding using IYUV or I420 at the input is not supported
 //
 if( NULL == pmtInput->pbFormat )
  {
   return ( E_INVALIDARG );
  }
 if( WMCFOURCC_IYUV == ((VIDEOINFOHEADER*)pmtInput->pbFormat)->bmiHeader.biCompression ||
     WMCFOURCC_I420 == ((VIDEOINFOHEADER*)pmtInput->pbFormat)->bmiHeader.biCompression )
  {
   if( TRUE == pParams->fIsInterlaced )
    {
     return ( E_VIDEO_INTERLACE_REJECTED );
    }
  }
#endif //SUPPORT_INTERLACE

 *ppDMO = NULL;

 //
 //check to see if the video bitrate has been set; if not calculate the default
 //
 if( pParams->nBitrate <= 0 )
  {
   hr = DefaultVideoBitrate( pmtInput, pParams->dFrameRate, &pParams->nBitrate );
   if( FAILED( hr ) )
    {
     return ( hr );
    }
   if( pParams->nPeakBitrate <= 0 )
    {
     pParams->nPeakBitrate = 3 * pParams->nBitrate;
    }
  }
 do
  {
   //
   // Create the DMO Encoder
   //
   const CLSID *clsid=&CLSID_NULL;
   char fcc[]="1234";memcpy(fcc,&pParams->dwTag,4);
   switch (pParams->dwTag)
    {
     case WMCFOURCC_MSS1:
     case WMCFOURCC_MSS2:
      {
       static const CLSID clsids[]={CLSID_CMSSCEncMediaObject2,CLSID_NULL};
       clsid=clsids;
       break;
      }
     case WMCFOURCC_WMV3:
     case WMCFOURCC_WMVA:
     case WMCFOURCC_WVC1:
     case WMCFOURCC_WVP2:
      {
       static const CLSID clsids[]={CLSID_CWMV9EncMediaObject,CLSID_NULL};
       clsid=clsids;
       break;
      }
     case WMCFOURCC_MP43:
     case WMCFOURCC_MP4S:
     case WMCFOURCC_WMV1:
     case WMCFOURCC_WMV2:
     case WMCFOURCC_WMVP:
     default:
      {
       static const CLSID clsids[]={CLSID_CWMVEncMediaObject2,CLSID_CWMVEncMediaObject2A,CLSID_NULL};
       clsid=clsids;
       break;
      }
    }
  nextclsid:
   hr = CoCreateInstance( *clsid,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_IMediaObject,
                           (void**)ppDMO);
   clsid++;

   if( FAILED( hr ) )
    if (*clsid==CLSID_NULL)
      break;
    else
      goto nextclsid;

   hr = SetVideoParams( *ppDMO, pParams );
   if( FAILED( hr ) )
    {
     break;
    }

   hr = SetVideoTypes( *ppDMO, pmtInput, pParams, pmtOutput, &cbInputBuffer, &cbOutputBuffer );
   if( FAILED( hr ) )
    {
     break;
    }

#ifdef SUPPORT_INTERLACE
   if( pParams->dwTag == WMCFOURCC_WMV3 && pParams->fIsInterlaced)
    {
     INSSBuffer3 *pINSSBuffer3 = NULL;
     BYTE        bType         = WM_CT_INTERLACED;

     hr = pMediaBuffer->QueryInterface( IID_INSSBuffer3, (void**)&pINSSBuffer3 );
     if( FAILED( hr ) )
      {
       break;
      }
     pINSSBuffer3->SetProperty( WM_SampleExtension_ContentType,
                                &bType,
                                1 );
     SAFERELEASE( pINSSBuffer3 );
    }
#endif //SUPPORT_INTERLACE

  }
 while ( FALSE );

 if( S_OK != hr )
  {
   SAFERELEASE( *ppDMO );
  }

 return ( hr );
}
