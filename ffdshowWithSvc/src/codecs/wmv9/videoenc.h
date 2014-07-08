//*@@@+++@@@@******************************************************************
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

#ifndef VIDEOENC_H
#define VIDEOENC_H

#include <dmo.h>
#include <tchar.h>

enum        { S_RESOURCE, S_MC };

#define   SAFERELEASE(x)            if((x)){ (x)->Release(); (x) = NULL;}
#define   SAFEDELETE(x)             if((x)){ delete [] (x);  (x) = NULL;}
#define   SAFEDELETES(x)            if((x)){ delete    (x);  (x) = NULL;}
#define   SAFEFREE(x)               if((x)){ free(x);        (x) = NULL;}

class CHandlingMediaBuffer;

//////////////////////////////////////////////////////////////////////////////
//
// Video FOURCCs
//
//////////////////////////////////////////////////////////////////////////////

enum { P_MAIN = 0, P_SIMPLE, P_COMPLEX };

//////////////////////////////////////////////////////////////////////////////
//
// Video params struct
//
//////////////////////////////////////////////////////////////////////////////
struct VideoEncParams
 {
  int    nBitrate;
  int    nWidth;
  int    nHeight;
  double dFrameRate;
  int    nKeyDist;
  int    nBufferDelay;
  int    nQuality;
  int    nCrisp;
  int    nVBRQuality;
  int    nPeakBuffer;
  int    nPeakBitrate;
  TCHAR  *pszProfileString;
  int    nComplexity;

  int    nProfile;
#ifdef SUPPORT_INTERLACE

  BOOL   fIsInterlaced;
#endif //SUPPORT_INTERLACE

  BOOL   fIsVBR;
  BOOL   fIsConstrained;
  int    nPasses;
  DWORD  dwTag;

  VideoEncParams()
  {
   memset(  this, 0, sizeof( VideoEncParams ) );
  }
 };

// some prototypes
HRESULT InitializeVideoEncoder( AM_MEDIA_TYPE *pmtInput, \
                                VideoEncParams *pParams, \
                                IMediaObject   **ppDMO,  \
                                AM_MEDIA_TYPE  *pmtOutPut,
                                CHandlingMediaBuffer *pMBIn);

#endif /*VIDEOENC_H*/
