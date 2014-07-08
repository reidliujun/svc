

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0365 */
/* Compiler settings for drmexternals.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data
    VC __declspec() decoration level:
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __drmexternals_h__
#define __drmexternals_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */

/* header files for imported files */
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * );

/* interface __MIDL_itf_drmexternals_0000 */
/* [local] */

//=========================================================================
//
// Microsoft Windows Media Technologies
// Copyright (C) Microsoft Corporation.  All Rights Reserved.
//
//=========================================================================
/*
static const WCHAR *g_wszWMDRM_RIGHT_PLAYBACK                    = L"Play";
static const WCHAR *g_wszWMDRM_RIGHT_COPY_TO_CD                  = L"Print.redbook";
static const WCHAR *g_wszWMDRM_RIGHT_COPY_TO_SDMI_DEVICE         = L"Transfer.SDMI";
static const WCHAR *g_wszWMDRM_RIGHT_COPY_TO_NON_SDMI_DEVICE     = L"Transfer.NONSDMI";
static const WCHAR *g_wszWMDRM_RIGHT_BACKUP                      = L"Backup";
static const WCHAR *g_wszWMDRM_RIGHT_COPY                        = L"Copy";
static const WCHAR *g_wszWMDRM_RIGHT_COLLABORATIVE_PLAY          = L"CollaborativePlay";
static const WCHAR *g_wszWMDRM_IsDRM                             = L"IsDRM";
static const WCHAR *g_wszWMDRM_IsDRMCached                       = L"IsDRMCached";
static const WCHAR *g_wszWMDRM_BaseLicenseAcqURL                 = L"BaseLAURL";
static const WCHAR *g_wszWMDRM_Rights                            = L"Rights";
static const WCHAR *g_wszWMDRM_LicenseID                         = L"LID";
static const WCHAR *g_wszWMDRM_ActionAllowed                     = L"ActionAllowed.";
static const WCHAR *g_wszWMDRM_ActionAllowed_Playback            = L"ActionAllowed.Play";
static const WCHAR *g_wszWMDRM_ActionAllowed_CopyToCD            = L"ActionAllowed.Print.redbook";
static const WCHAR *g_wszWMDRM_ActionAllowed_CopyToSDMIDevice    = L"ActionAllowed.Transfer.SDMI";
static const WCHAR *g_wszWMDRM_ActionAllowed_CopyToNonSDMIDevice = L"ActionAllowed.Transfer.NONSDMI";
static const WCHAR *g_wszWMDRM_ActionAllowed_Backup              = L"ActionAllowed.Backup";
static const WCHAR *g_wszWMDRM_ActionAllowed_Copy                = L"ActionAllowed.Copy";
static const WCHAR *g_wszWMDRM_ActionAllowed_CollaborativePlay   = L"ActionAllowed.CollaborativePlay";
static const WCHAR *g_wszWMDRM_ActionAllowed_PlaylistBurn        = L"ActionAllowed.PlaylistBurn";
static const WCHAR *g_wszWMDRM_LicenseState                      = L"LicenseStateData.";
static const WCHAR *g_wszWMDRM_LicenseState_Playback             = L"LicenseStateData.Play";
static const WCHAR *g_wszWMDRM_LicenseState_CopyToCD             = L"LicenseStateData.Print.redbook";
static const WCHAR *g_wszWMDRM_LicenseState_CopyToSDMIDevice     = L"LicenseStateData.Transfer.SDMI";
static const WCHAR *g_wszWMDRM_LicenseState_CopyToNonSDMIDevice  = L"LicenseStateData.Transfer.NONSDMI";
static const WCHAR *g_wszWMDRM_LicenseState_Copy                 = L"LicenseStateData.Copy";
static const WCHAR *g_wszWMDRM_LicenseState_CollaborativePlay    = L"LicenseStateData.CollaborativePlay";
static const WCHAR *g_wszWMDRM_LicenseState_PlaylistBurn         = L"LicenseStateData.PlaylistBurn";
static const WCHAR *g_wszWMDRM_DRMHeader                         = L"DRMHeader.";
static const WCHAR *g_wszWMDRM_DRMHeader_KeyID                   = L"DRMHeader.KID";
static const WCHAR *g_wszWMDRM_DRMHeader_LicenseAcqURL           = L"DRMHeader.LAINFO";
static const WCHAR *g_wszWMDRM_DRMHeader_ContentID               = L"DRMHeader.CID";
static const WCHAR *g_wszWMDRM_DRMHeader_IndividualizedVersion   = L"DRMHeader.SECURITYVERSION";
static const WCHAR *g_wszWMDRM_DRMHeader_ContentDistributor      = L"DRMHeader.ContentDistributor";
static const WCHAR *g_wszWMDRM_DRMHeader_SubscriptionContentID   = L"DRMHeader.SubscriptionContentID";
static const WCHAR *g_wszWMDRM_SAPLEVEL                          = L"SAPLEVEL";
*/
EXTERN_GUID( WMDRM_PROTECTION_SCHEME_ACP,        0xC3FD11C6, 0xF8B7, 0x4d20, 0xB0, 0x08, 0x1d, 0xb1, 0x7d, 0x61, 0xf2, 0xda );
//static const WCHAR *g_wszWMDRMNET_Revocation                     = L"WMDRMNET_REVOCATION";
typedef
enum DRM_LICENSE_STATE_CATEGORY
    {	WM_DRM_LICENSE_STATE_NORIGHT	= 0,
	WM_DRM_LICENSE_STATE_UNLIM	= WM_DRM_LICENSE_STATE_NORIGHT + 1,
	WM_DRM_LICENSE_STATE_COUNT	= WM_DRM_LICENSE_STATE_UNLIM + 1,
	WM_DRM_LICENSE_STATE_FROM	= WM_DRM_LICENSE_STATE_COUNT + 1,
	WM_DRM_LICENSE_STATE_UNTIL	= WM_DRM_LICENSE_STATE_FROM + 1,
	WM_DRM_LICENSE_STATE_FROM_UNTIL	= WM_DRM_LICENSE_STATE_UNTIL + 1,
	WM_DRM_LICENSE_STATE_COUNT_FROM	= WM_DRM_LICENSE_STATE_FROM_UNTIL + 1,
	WM_DRM_LICENSE_STATE_COUNT_UNTIL	= WM_DRM_LICENSE_STATE_COUNT_FROM + 1,
	WM_DRM_LICENSE_STATE_COUNT_FROM_UNTIL	= WM_DRM_LICENSE_STATE_COUNT_UNTIL + 1,
	WM_DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE	= WM_DRM_LICENSE_STATE_COUNT_FROM_UNTIL + 1
    } 	DRM_LICENSE_STATE_CATEGORY;

#define DRM_LICENSE_STATE_DATA_VAGUE       1
#define DRM_LICENSE_STATE_DATA_OPL_PRESENT 2
#define DRM_LICENSE_STATE_DATA_SAP_PRESENT 4
typedef struct _DRM_LICENSE_STATE_DATA
    {
    DWORD dwStreamId;
    DRM_LICENSE_STATE_CATEGORY dwCategory;
    DWORD dwNumCounts;
    DWORD dwCount[ 4 ];
    DWORD dwNumDates;
    FILETIME datetime[ 4 ];
    DWORD dwVague;
    } 	DRM_LICENSE_STATE_DATA;

typedef
enum DRM_HTTP_STATUS
    {	HTTP_NOTINITIATED	= 0,
	HTTP_CONNECTING	= HTTP_NOTINITIATED + 1,
	HTTP_REQUESTING	= HTTP_CONNECTING + 1,
	HTTP_RECEIVING	= HTTP_REQUESTING + 1,
	HTTP_COMPLETED	= HTTP_RECEIVING + 1
    } 	DRM_HTTP_STATUS;

typedef
enum DRM_INDIVIDUALIZATION_STATUS
    {	INDI_UNDEFINED	= 0,
	INDI_BEGIN	= 0x1,
	INDI_SUCCEED	= 0x2,
	INDI_FAIL	= 0x4,
	INDI_CANCEL	= 0x8,
	INDI_DOWNLOAD	= 0x10,
	INDI_INSTALL	= 0x20
    } 	DRM_INDIVIDUALIZATION_STATUS;

typedef struct _WMIndividualizeStatus
    {
    HRESULT hr;
    DRM_INDIVIDUALIZATION_STATUS enIndiStatus;
    LPSTR pszIndiRespUrl;
    DWORD dwHTTPRequest;
    DRM_HTTP_STATUS enHTTPStatus;
    DWORD dwHTTPReadProgress;
    DWORD dwHTTPReadTotal;
    } 	WM_INDIVIDUALIZE_STATUS;

typedef struct _WMGetLicenseData
    {
    DWORD dwSize;
    HRESULT hr;
    WCHAR *wszURL;
    WCHAR *wszLocalFilename;
    BYTE *pbPostData;
    DWORD dwPostDataSize;
    } 	WM_GET_LICENSE_DATA;

#ifndef DRM_OPL_TYPES
#define DRM_OPL_TYPES 1
typedef struct __tagDRM_MINIMUM_OUTPUT_PROTECTION_LEVELS
    {
    WORD wCompressedDigitalVideo;
    WORD wUncompressedDigitalVideo;
    WORD wAnalogVideo;
    WORD wCompressedDigitalAudio;
    WORD wUncompressedDigitalAudio;
    } 	DRM_MINIMUM_OUTPUT_PROTECTION_LEVELS;

typedef struct __tagDRM_OPL_OUTPUT_IDS
    {
    WORD cIds;
    GUID *rgIds;
    } 	DRM_OPL_OUTPUT_IDS;

typedef struct __tagDRM_VIDEO_OUTPUT_PROTECTION
    {
    GUID guidId;
    BYTE bConfigData;
    } 	DRM_VIDEO_OUTPUT_PROTECTION;

typedef struct __tagDRM_VIDEO_OUTPUT_PROTECTION_IDS
    {
    WORD cEntries;
    DRM_VIDEO_OUTPUT_PROTECTION *rgVop;
    } 	DRM_VIDEO_OUTPUT_PROTECTION_IDS;

typedef struct __tagDRM_PLAY_OPL
    {
    DRM_MINIMUM_OUTPUT_PROTECTION_LEVELS minOPL;
    DRM_OPL_OUTPUT_IDS oplIdReserved;
    DRM_VIDEO_OUTPUT_PROTECTION_IDS vopi;
    } 	DRM_PLAY_OPL;

typedef struct __tagDRM_COPY_OPL
    {
    WORD wMinimumCopyLevel;
    DRM_OPL_OUTPUT_IDS oplIdIncludes;
    DRM_OPL_OUTPUT_IDS oplIdExcludes;
    } 	DRM_COPY_OPL;

#endif
#define DRM_DEVICE_REGISTER_TYPE_STREAMING      ((DWORD) 0x00000001)
#define DRM_DEVICE_REGISTER_TYPE_STORAGE        ((DWORD) 0x00000002)


extern RPC_IF_HANDLE __MIDL_itf_drmexternals_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_drmexternals_0000_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif






