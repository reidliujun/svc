//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation. All rights reserved.
//
//  File:        wmcodecs.h
//
//  Description: Consolidated source for the definitions needed to use
//               the Windows Media codec DMO class identifiers.
//               These values are all defined elsewhere, but
//               are given different names here to avoid ambiguity.
//
//--------------------------------------------------------------------------

#ifndef __WMCODECS_H_
#define __WMCODECS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


////////////////////////////////////////////////////////////////////////////
// Definitions of the Windows Media Audio and Video
// codec DMO class identifiers.
////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
// Windows Media Audio encoder and decoder.
// Support for the following codecs:

// * Windows Media Audio 7 |
// * Windows Media Audio 8 |-> These three use the same format tag and
// * Windows Media Audio 9 |   subtype GUID.

// * Windows Media Audio 9 Professional
// * Windows Media Audio 9 Lossless

EXTERN_GUID(CLSID_CWMAEncMediaObject,
    0x70f598e9, 0xf4ab, 0x495a, 0x99, 0xe2, 0xa7, 0xc4, 0xd3, 0xd8, 0x9a, 0xbf);
EXTERN_GUID(CLSID_CWMADecMediaObject,
    0x2eeb4adf, 0x4578, 0x4d10, 0xbc, 0xa7, 0xbb, 0x95, 0x5f, 0x56, 0x32, 0x0a);

//////////////////////////////////////////////////////
// Windows Media Video encoder and decoder.
// Support for the following codecs:
// * Windows Media Video 7 (WMV1)
// * Windows Media Video 8 (WMV2)
// * Windows Media Video 9 (WMV3)
// * Windows Media Video 9 Image (WMVP)
EXTERN_GUID(CLSID_CWMVEncMediaObject2,
    0x96b57cdd, 0x8966, 0x410c, 0xbb, 0x1f, 0xc9, 0x7e, 0xea, 0x76, 0x5c, 0x04);
EXTERN_GUID(CLSID_CWMVEncMediaObject2A,
    0x179D356D, 0xA7AF, 0x4464, 0x8A, 0xD8, 0xD7, 0xA4, 0x39, 0x45, 0x5E, 0x35);
EXTERN_GUID(CLSID_CWMVDecMediaObject,
    0x82d353df, 0x90bd, 0x4382, 0x8b, 0xc2, 0x3f, 0x61, 0x92, 0xb7, 0x6e, 0x34);

//////////////////////////////////////////////////////
// Windows Media Video Advanced encoder and decoder.
// Support for the following codecs:
// * Windows Media Video 9 Advanced Profile (WMVA)
// * Windows Media Video 9 Image v2 (WVP2)
EXTERN_GUID(CLSID_CWMV9EncMediaObject,
	0xd23b90d0, 0x144f, 0x46bd, 0x84, 0x1d, 0x59, 0xe4, 0xeb, 0x19, 0xdc, 0x59);
EXTERN_GUID(CLSID_CWMVDecMediaObject2,
	0x03be3ac4, 0x84b7, 0x4e0e, 0xa7, 0x8d, 0xd3, 0x52, 0x4e, 0x60, 0x39, 0x5a);

//////////////////////////////////////////////////////
// Windows Media Video Screen encoder and decoder.
EXTERN_GUID(CLSID_CMSSCEncMediaObject2,
    0xf7ffe0a0, 0xa4f5, 0x44b5, 0x94, 0x9e, 0x15, 0xed, 0x2b, 0xc6, 0x6f, 0x9d);
EXTERN_GUID(CLSID_CMSSCDecMediaObject,
    0x7bafb3b1, 0xd8f4, 0x4279, 0x92, 0x53, 0x27, 0xda, 0x42, 0x31, 0x08, 0xde);

//////////////////////////////////////////////////////
// Windows Media Audio Voice encoder and decoder.
EXTERN_GUID(CLSID_CWMSPEncMediaObject,
    0x67841b03, 0xc689, 0x4188, 0xad, 0x3f, 0x4c, 0x9e, 0xbe, 0xec, 0x71, 0xb);
EXTERN_GUID(CLSID_CWMSPDecMediaObject,
    0x874131cb, 0x4ecc, 0x443b, 0x89, 0x48, 0x74, 0x6b, 0x89, 0x59, 0x5d, 0x20);

//////////////////////////////////////////////////////
// Windows Media Audio 9 Professional decoder to SPDIF.
EXTERN_GUID(CLSID_CWMAudioSpdTxDMO,
	0x5210f8e4, 0xb0bb, 0x47c3, 0xa8, 0xd9, 0x7b, 0x22, 0x82, 0xcc, 0x79, 0xed);



////////////////////////////////////////////////////////////////////////////
// Definitions of media type GUIDs. The GUIDs in question are identical
// to those defined elsewhere (in the DirectX or Windows Media Format SDK). The
// names are different to avoid ambiguity with other potentially included
// header files.
////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
// Major media types.
//
// Video (FOURCC 'vids') 73646976-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIATYPE_Video,
0x73646976, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
// Audio (FOURCC 'auds') 73647561-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIATYPE_Audio,
0x73647561, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);



//////////////////////////////////////////////////////
// Media format specifiers.
//
// These values are used for the formattype member of DMO_MEDIA_TYPE to
//  identify the type of structure pointed to by the pbFormat member.
//
// VIDEOINFOHEADER
// Format type: 05589f80-c356-11ce-bf01-00aa0055595a
EXTERN_GUID(WMCFORMAT_VideoInfo,
0x05589f80, 0xc356, 0x11ce, 0xbf, 0x01, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a);

// WAVEFORMATEX
// Format type: 05589f81-c356-11ce-bf01-00aa0055595a
EXTERN_GUID(WMCFORMAT_WaveFormatEx,
0x05589f81, 0xc356, 0x11ce, 0xbf, 0x01, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a);



//////////////////////////////////////////////////////
// Compressed Windows Media subtypes.
//
// Windows Media Video 7
// FOURCC: 'WMV1' Subtype: 31564D57-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_WMV1,
0x31564D57, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Windows Media Video 8
// FOURCC: 'WMV2' Subtype: 32564D57-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_WMV2,
0x32564D57, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Windows Media Video 9
// FOURCC: 'WMV3' Subtype: 33564D57-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_WMV3,
0x33564D57, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Windows Media Video 9 Advanced Profile
// FOURCC: 'WMVA' Subtype: 41564D57-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_WMVA,
0x41564D57, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Windows Media Video 9 Advanced Profile
// FOURCC: 'WVC1' Subtype: 31435657-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_WVC1,
0x31435657, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Windows Media Video 7 Screen
// FOURCC: 'MSS1' Subtype: 3153534D-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_MSS1,
0x3153534D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Windows Media Video 9 Screen
// FOURCC: 'MSS2' Subtype: 3253534D-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_MSS2,
0x3253534D, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Windows Media Video 9 Image
// FOURCC: 'WMVP' Subtype: 50564D57-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_WMVP,
0x50564D57, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Windows Media Video 9 Image v2
// FOURCC: 'WVP2' Subtype: 32505657-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_WVP2,
0x32505657, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Microsoft Audio
EXTERN_GUID(WMCMEDIASUBTYPE_MSA1,
0x00000160, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Windows Media Audio 9 and previous compatible versions
// Format tag: 0x161 Subtype: 00000161-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_WMA9,
0x00000161, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Windows Media Audio 9 Professional
// Format tag: 0x162 Subtype: 00000162-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_WMA9Pro,
0x00000162, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Windows Media Audio 9 Lossless
// Format tag: 0x163 Subtype: 00000163-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_WMA9Lossless,
0x00000163, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// Windows Media Audio 9 Voice
// Format tag: 0x00A Subtype: 0000000A-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_WMA9Voice,
0x0000000A,0x0000,0x0010,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71);


//////////////////////////////////////////////////////
// Uncompressed video subtypes.
//
// Note: Unlike the other video formats, RGB video does not use a FOURCC
// as the first four bytes of the subtype GUID.
//
// RGB 8 bits per pixel (palettized)
// Subtype: e436eb7a-524f-11ce-9f53-0020af0ba770
EXTERN_GUID(WMCMEDIASUBTYPE_RGB8,
0xe436eb7a, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);

// RGB 16 bits per pixel (5 red bits, 6 green bits, 5 blue bits)
// Subtype: e436eb7b-524f-11ce-9f53-0020af0ba770
EXTERN_GUID(WMCMEDIASUBTYPE_RGB565,
0xe436eb7b, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);

// RGB 16 bits per pixel (5 red bits, 4 green bits, 5 blue bits)
// Subtype: e436eb7c-524f-11ce-9f53-0020af0ba770
EXTERN_GUID(WMCMEDIASUBTYPE_RGB555,
0xe436eb7c, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);

// RGB 24 bits per pixel
// Subtype: e436eb7d-524f-11ce-9f53-0020af0ba770
EXTERN_GUID(WMCMEDIASUBTYPE_RGB24,
0xe436eb7d, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);

// RGB 32 bits per pixel
// Subtype: e436eb7e-524f-11ce-9f53-0020af0ba770
EXTERN_GUID(WMCMEDIASUBTYPE_RGB32,
0xe436eb7e, 0x524f, 0x11ce, 0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70);

// I420 Subtype: 30323449-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_I420,
0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

// IYUV Subtype: 56555949-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_IYUV,
0x56555949, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

// YV12 Subtype: 32315659-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_YV12,
0x32315659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

// YUY2 Subtype: 32595559-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_YUY2,
0x32595559, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

// UYVY Subtype: 59565955-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_UYVY,
0x59565955, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

// YUYV Subtype: 56595559-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_YUYV,
0x56595559, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

// YVYU Subtype: 55595659-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_YVYU,
0x55595659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

// YVU9 Subtype: 39555659-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_YVU9,
0x39555659, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

// Special input type for Windows Media Video 9 Image.
// Consists of structured transform data with occasional RGB images on which
// the transforms are executed. This first DWORD of this subtype does not
// translate to a legible FOURCC.
// Subtype: 1d4a45f2-e5f6-4b44-8388-f0ae5c0e0c37
EXTERN_GUID(WMCMEDIASUBTYPE_VIDEOIMAGE,
0x1d4a45f2, 0xe5f6, 0x4b44, 0x83, 0x88, 0xf0, 0xae, 0x5c, 0x0e, 0x0c, 0x37);

//////////////////////////////////////////////////////
// Uncompressed audio subtype (only PCM is accepted).
//
// PCM audio.
// Format tag 0x001 Subtype: 00000001-0000-0010-8000-00AA00389B71
EXTERN_GUID(WMCMEDIASUBTYPE_PCM,
0x00000001, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);



////////////////////////////////////////////////////////////////////////////
// Definitions of the FOURCC values that are commonly used by the codec
// DMOs. Each FOURCC identifies a video format subtype. The FOURCC value
// for a format is also contained in the first DWORD in the subtype
// GUID (with the exception of RGB formats, which don't need FOURCCs).
////////////////////////////////////////////////////////////////////////////

// Macro that makes a FOURCC from four characters.
#define wmcFOURCC( ch0, ch1, ch2, ch3 )				\
		( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |	\
		( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

// YUV formats.
#define WMCFOURCC_UYVY		0x59565955
#define WMCFOURCC_YUY2		0x32595559
#define WMCFOURCC_YVYU		0x55595659
#define WMCFOURCC_YVU9		0x39555659
#define WMCFOURCC_YV12		0x32315659
#define WMCFOURCC_I420		0x30323449
#define WMCFOURCC_IYUV		0x56555949

// Windows Media Video formats.
#define WMCFOURCC_WMV1     wmcFOURCC('W','M','V','1')
#define WMCFOURCC_WMV2     wmcFOURCC('W','M','V','2')
#define WMCFOURCC_WMV3     wmcFOURCC('W','M','V','3')
#define WMCFOURCC_WMVA     wmcFOURCC('W','M','V','A')
#define WMCFOURCC_WVC1     wmcFOURCC('W','V','C','1')
#define WMCFOURCC_WMVP     wmcFOURCC('W','M','V','P')
#define WMCFOURCC_WVP2     wmcFOURCC('W','V','P','2')
#define WMCFOURCC_MSS1     wmcFOURCC('M','S','S','1')
#define WMCFOURCC_MSS2     wmcFOURCC('M','S','S','2')



////////////////////////////////////////////////////////////////////////////
// Definitions of the audio format tags for the Windows Media Audio types.
////////////////////////////////////////////////////////////////////////////
#define WMC_WAVE_MSA       0x160 // Old Microsoft audio format (pre WMA7)
#define WMC_WAVE_WMA       0x161 // Windows Media Audio (7,8,9)
#define WMC_WAVE_WMAPRO    0x162 // Windows Media Audio 9 Professional
#define WMC_WAVE_WMALOSLS  0x163 // Windows Media Audio 9 Lossless
#define WMC_WAVE_WMAVOICE  0x00A // Windows Media Audio 9 Voice




////////////////////////////////////////////////////////////////////////////
// Definitions of the name strings used for properties accessed using
// IPropertyBag. These constants should be used to avoid potentially
// difficult errors due to misspelling the names (the compiler will catch
// a misspelled constant).
////////////////////////////////////////////////////////////////////////////

/*
static const WCHAR *g_wszWMACAvgBytesPerSec = L"AvgBytesPerSec";
static const WCHAR *g_wszWMACAvgPCMValue = L"AverageLevel";
static const WCHAR *g_wszWMACDXVAEnabled = L"DXVAEnabled";
static const WCHAR *g_wszWMACFoldDownXToYChannels = L"FoldDown%dTo%dChannels";
static const WCHAR *g_wszWMACFoldXToYChannelsZ = L"Fold%uTo%uChannels%u";
static const WCHAR *g_wszWMACHiResOutput = L"_HIRESOUTPUT";
static const WCHAR *g_wszWMACIncludeNumPasses = L"_INCLUDENUMPASSES";
static const WCHAR *g_wszWMACInputFormatName = L"_INPUTFORMATNAME";
static const WCHAR *g_wszWMACMixTable = L"MixTable";
static const WCHAR *g_wszWMACMusicSpeechClassMode = L"MusicSpeechClassMode";
static const WCHAR *g_wszWMACOriginalWaveFormat = L"_ORIGINALWAVEFORMAT";
static const WCHAR *g_wszWMACPeakPCMValue = L"PeakValue";
static const WCHAR *g_wszWMACSourceFormatTag = L"_SOURCEFORMATTAG";
static const WCHAR *g_wszWMACSpeakerConfig = L"SpeakerConfig";
static const WCHAR *g_wszWMACVoiceBuffer = L"BufferWindow";
static const WCHAR *g_wszWMACVoiceEDL = L"_EDL";
static const WCHAR *g_wszWMADRCAverageReference = L"WMADRCAverageReference";
static const WCHAR *g_wszWMADRCAverageTarget = L"WMADRCAverageTarget";
static const WCHAR *g_wszWMADRCPeakReference = L"WMADRCPeakReference";
static const WCHAR *g_wszWMADRCPeakTarget = L"WMADRCPeakTarget";
static const WCHAR *g_wszWMACDRCSetting = L"DynamicRangeControl";
*/
static const WCHAR *g_wszWMVCAvgBitrate = L"_RAVG";
static const WCHAR *g_wszWMVCAvgFrameRate = L"_AVGFRAMERATE";
static const WCHAR *g_wszWMVCBAvg = L"_BAVG";
static const WCHAR *g_wszWMVCBMax = L"_BMAX";
static const WCHAR *g_wszWMVCBufferFullnessInFirstByte = L"_BUFFERFULLNESSINFIRSTBYTE";
static const WCHAR *g_wszWMVCCodedFrames = L"_CODEDFRAMES";
static const WCHAR *g_wszWMVCComplexityEx = L"_COMPLEXITYEX";
static const WCHAR *g_wszWMVCComplexityExLive = L"_COMPLEXITYEXLIVE";
static const WCHAR *g_wszWMVCComplexityExMax = L"_COMPLEXITYEXMAX";
static const WCHAR *g_wszWMVCComplexityExOffline = L"_COMPLEXITYEXOFFLINE";
static const WCHAR *g_wszWMVCComplexityMode = L"_COMPLEXITY";
static const WCHAR *g_wszWMVCCrisp = L"_CRISP";
static const WCHAR *g_wszWMVCDatarate = L"_DATARATE";
static const WCHAR *g_wszWMVCDecoderComplexityRequested = L"_DECODERCOMPLEXITYREQUESTED";
static const WCHAR *g_wszWMVCDecoderComplexityProfile = L"_DECODERCOMPLEXITYPROFILE";
static const WCHAR *g_wszWMVCDecoderDeinterlacing = L"_DECODERDEINTERLACING";
static const WCHAR *g_wszWMVCDefaultCrisp = L"_DEFAULTCRISP";
static const WCHAR *g_wszWMVCEndOfPass = L"_ENDOFPASS";
static const WCHAR *g_wszWMVCFOURCC = L"_FOURCC";
static const WCHAR *g_wszWMVCFrameCount = L"_FRAMECOUNT";
static const WCHAR *g_wszWMVCFrameInterpolationEnabled = L"_FRAMEINTERPOLATIONENABLED";
static const WCHAR *g_wszWMVCFrameInterpolationSupported = L"_FRAMEINTERPOLATIONSUPPORTED";
static const WCHAR *g_wszWMVCInterlacedCodingEnabled = L"_INTERLACEDCODINGENABLED";
static const WCHAR *g_wszWMVCKeyframeDistance = L"_KEYDIST";
static const WCHAR *g_wszWMVCLiveEncode = L"_LIVEENCODE";
static const WCHAR *g_wszWMVCMaxBitrate = L"_RMAX";
static const WCHAR *g_wszWMVCPacketOverhead = L"_ASFOVERHEADPERFRAME";
static const WCHAR *g_wszWMVCPassesRecommended = L"_PASSESRECOMMENDED";
static const WCHAR *g_wszWMVCPassesUsed = L"_PASSESUSED";
static const WCHAR *g_wszWMVCProduceDummyFrames = L"_PRODUCEDUMMYFRAMES";
static const WCHAR *g_wszWMVCTotalFrames = L"_TOTALFRAMES";
static const WCHAR *g_wszWMVCTotalWindow = L"_TOTALWINDOW";
static const WCHAR *g_wszWMVCVBREnabled = L"_VBRENABLED";
static const WCHAR *g_wszWMVCVBRQuality = L"_VBRQUALITY";
static const WCHAR *g_wszWMVCVideoWindow = L"_VIDEOWINDOW";
static const WCHAR *g_wszWMVCZeroByteFrames = L"_ZEROBYTEFRAMES";


static const WCHAR *g_wszSpeechFormatCaps = L"SpeechFormatCap";
static const WCHAR *g_wszWMCPCodecName = L"_CODECNAME";
static const WCHAR *g_wszWMCPSupportedVBRModes = L"_SUPPORTEDVBRMODES";


// Constants used with g_wszWMCSupportedVBRModes
#define WM_CODEC_ONEPASS_CBR 1
#define WM_CODEC_ONEPASS_VBR 2
#define WM_CODEC_TWOPASS_CBR 4
#define WM_CODEC_TWOPASS_VBR_UNCONSTRAINED   8
#define WM_CODEC_TWOPASS_VBR_PEAKCONSTRAINED 16


#endif  // !defined(__WMCODECS_H_)

