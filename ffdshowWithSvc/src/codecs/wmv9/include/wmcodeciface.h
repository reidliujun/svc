
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Tue Feb 18 17:09:53 2003
 */
/* Compiler settings for wmcodec.idl:
    Os, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data
    VC __declspec() decoration level:
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __wmcodec_h_h__
#define __wmcodec_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */

#ifndef __IWMVideoDecoderHurryup_FWD_DEFINED__
#define __IWMVideoDecoderHurryup_FWD_DEFINED__
typedef interface IWMVideoDecoderHurryup IWMVideoDecoderHurryup;
#endif 	/* __IWMVideoDecoderHurryup_FWD_DEFINED__ */


#ifndef __IWMCodecStrings_FWD_DEFINED__
#define __IWMCodecStrings_FWD_DEFINED__
typedef interface IWMCodecStrings IWMCodecStrings;
#endif 	/* __IWMCodecStrings_FWD_DEFINED__ */


#ifndef __IWMCodecProps_FWD_DEFINED__
#define __IWMCodecProps_FWD_DEFINED__
typedef interface IWMCodecProps IWMCodecProps;
#endif 	/* __IWMCodecProps_FWD_DEFINED__ */


#ifndef __IWMCodecLeakyBucket_FWD_DEFINED__
#define __IWMCodecLeakyBucket_FWD_DEFINED__
typedef interface IWMCodecLeakyBucket IWMCodecLeakyBucket;
#endif 	/* __IWMCodecLeakyBucket_FWD_DEFINED__ */


#ifndef __IWMCodecMetaData_FWD_DEFINED__
#define __IWMCodecMetaData_FWD_DEFINED__
typedef interface IWMCodecMetaData IWMCodecMetaData;
#endif 	/* __IWMCodecMetaData_FWD_DEFINED__ */


#ifndef __IWMCodecOutputTimestamp_FWD_DEFINED__
#define __IWMCodecOutputTimestamp_FWD_DEFINED__
typedef interface IWMCodecOutputTimestamp IWMCodecOutputTimestamp;
#endif 	/* __IWMCodecOutputTimestamp_FWD_DEFINED__ */


#ifndef __IWMCodecPrivateData_FWD_DEFINED__
#define __IWMCodecPrivateData_FWD_DEFINED__
typedef interface IWMCodecPrivateData IWMCodecPrivateData;
#endif 	/* __IWMCodecPrivateData_FWD_DEFINED__ */


/* header files for imported files */
#include "mediaobj.h"

#ifdef __cplusplus
extern "C"{
#endif

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * );

/* interface __MIDL_itf_wmcodec_0000 */
/* [local] */

//=========================================================================
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation. All rights reserved.
//
//=========================================================================
EXTERN_GUID( IID_IWMVideoDecoderHurryup, 0x352bb3bd, 0x2d4d, 0x4323, 0x9e, 0x71, 0xdc, 0xdc, 0xfb, 0xd5, 0x3c, 0xa6);
EXTERN_GUID( IID_IWMCodecStrings,        0xA7B2504B, 0xE58A, 0x47fb, 0x95, 0x8B, 0xCA, 0xC7, 0x16, 0x5A, 0x05, 0x7D);
EXTERN_GUID( IID_IWMCodecProps,          0x2573e11a, 0xf01a, 0x4fdd, 0xa9, 0x8d, 0x63, 0xb8, 0xe0, 0xbA, 0x95, 0x89);
EXTERN_GUID( IID_IWMCodecLeakyBucket,    0xA81BA647, 0x6227, 0x43b7, 0xB2, 0x31, 0xC7, 0xB1, 0x51, 0x35, 0xDD, 0x7D);
EXTERN_GUID( IID_IWMCodecMetaData,       0xd051ed9f, 0xbc5c, 0x4e83, 0xb1, 0x4e, 0x84, 0x28, 0x48, 0x5c, 0x28, 0x6a);
EXTERN_GUID( IID_IWMCodecOutputTimestamp,0xB72ADF95, 0x7ADC, 0x4a72, 0xBC, 0x05, 0x57, 0x7D, 0x8E, 0xA6, 0xBF, 0x68);
EXTERN_GUID( IID_IWMCodecPrivateData,    0x73f0be8e, 0x57f7, 0x4f01, 0xaa, 0x66, 0x9f, 0x57, 0x34, 0xc, 0xfe, 0xe );
typedef

enum WMT_PROP_DATATYPE
    {	WMT_PROP_TYPE_DWORD	= 0,
	WMT_PROP_TYPE_STRING	= 1,
	WMT_PROP_TYPE_BINARY	= 2,
	WMT_PROP_TYPE_BOOL	= 3,
	WMT_PROP_TYPE_QWORD	= 4,
	WMT_PROP_TYPE_WORD	= 5,
	WMT_PROP_TYPE_GUID	= 6
    } 	WMT_PROP_DATATYPE;


extern RPC_IF_HANDLE __MIDL_itf_wmcodec_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wmcodec_0000_v0_0_s_ifspec;

#ifndef __IWMVideoDecoderHurryup_INTERFACE_DEFINED__
#define __IWMVideoDecoderHurryup_INTERFACE_DEFINED__

/* interface IWMVideoDecoderHurryup */
/* [local][unique][helpstring][uuid][object] */


EXTERN_C const IID IID_IWMVideoDecoderHurryup;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("352bb3bd-2d4d-4323-9e71-dcdcfbd53ca6")
    IWMVideoDecoderHurryup : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetHurryup(
            /* [in] */ LONG lHurryup) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetHurryup(
            /* [out] */ LONG *plHurryup) = 0;

    };

#else 	/* C style interface */

    typedef struct IWMVideoDecoderHurryupVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
            IWMVideoDecoderHurryup * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);

        ULONG ( STDMETHODCALLTYPE *AddRef )(
            IWMVideoDecoderHurryup * This);

        ULONG ( STDMETHODCALLTYPE *Release )(
            IWMVideoDecoderHurryup * This);

        HRESULT ( STDMETHODCALLTYPE *SetHurryup )(
            IWMVideoDecoderHurryup * This,
            /* [in] */ LONG lHurryup);

        HRESULT ( STDMETHODCALLTYPE *GetHurryup )(
            IWMVideoDecoderHurryup * This,
            /* [out] */ LONG *plHurryup);

        END_INTERFACE
    } IWMVideoDecoderHurryupVtbl;

    interface IWMVideoDecoderHurryup
    {
        CONST_VTBL struct IWMVideoDecoderHurryupVtbl *lpVtbl;
    };



#ifdef COBJMACROS


#define IWMVideoDecoderHurryup_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWMVideoDecoderHurryup_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWMVideoDecoderHurryup_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWMVideoDecoderHurryup_SetHurryup(This,lHurryup)	\
    (This)->lpVtbl -> SetHurryup(This,lHurryup)

#define IWMVideoDecoderHurryup_GetHurryup(This,plHurryup)	\
    (This)->lpVtbl -> GetHurryup(This,plHurryup)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWMVideoDecoderHurryup_SetHurryup_Proxy(
    IWMVideoDecoderHurryup * This,
    /* [in] */ LONG lHurryup);


void __RPC_STUB IWMVideoDecoderHurryup_SetHurryup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWMVideoDecoderHurryup_GetHurryup_Proxy(
    IWMVideoDecoderHurryup * This,
    /* [out] */ LONG *plHurryup);


void __RPC_STUB IWMVideoDecoderHurryup_GetHurryup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWMVideoDecoderHurryup_INTERFACE_DEFINED__ */


#ifndef __IWMCodecStrings_INTERFACE_DEFINED__
#define __IWMCodecStrings_INTERFACE_DEFINED__

/* interface IWMCodecStrings */
/* [local][unique][helpstring][uuid][object] */


EXTERN_C const IID IID_IWMCodecStrings;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("A7B2504B-E58A-47fb-958B-CAC7165A057D")
    IWMCodecStrings : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetName(
            /* [in] */ DMO_MEDIA_TYPE *pmt,
            /* [in] */ ULONG cchLength,
            /* [out] */ WCHAR *szName,
            /* [out] */ ULONG *pcchLength) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetDescription(
            /* [in] */ DMO_MEDIA_TYPE *pmt,
            /* [in] */ ULONG cchLength,
            /* [out] */ WCHAR *szDescription,
            /* [out] */ ULONG *pcchLength) = 0;

    };

#else 	/* C style interface */

    typedef struct IWMCodecStringsVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
            IWMCodecStrings * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);

        ULONG ( STDMETHODCALLTYPE *AddRef )(
            IWMCodecStrings * This);

        ULONG ( STDMETHODCALLTYPE *Release )(
            IWMCodecStrings * This);

        HRESULT ( STDMETHODCALLTYPE *GetName )(
            IWMCodecStrings * This,
            /* [in] */ DMO_MEDIA_TYPE *pmt,
            /* [in] */ ULONG cchLength,
            /* [out] */ WCHAR *szName,
            /* [out] */ ULONG *pcchLength);

        HRESULT ( STDMETHODCALLTYPE *GetDescription )(
            IWMCodecStrings * This,
            /* [in] */ DMO_MEDIA_TYPE *pmt,
            /* [in] */ ULONG cchLength,
            /* [out] */ WCHAR *szDescription,
            /* [out] */ ULONG *pcchLength);

        END_INTERFACE
    } IWMCodecStringsVtbl;

    interface IWMCodecStrings
    {
        CONST_VTBL struct IWMCodecStringsVtbl *lpVtbl;
    };



#ifdef COBJMACROS


#define IWMCodecStrings_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWMCodecStrings_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWMCodecStrings_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWMCodecStrings_GetName(This,pmt,cchLength,szName,pcchLength)	\
    (This)->lpVtbl -> GetName(This,pmt,cchLength,szName,pcchLength)

#define IWMCodecStrings_GetDescription(This,pmt,cchLength,szDescription,pcchLength)	\
    (This)->lpVtbl -> GetDescription(This,pmt,cchLength,szDescription,pcchLength)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWMCodecStrings_GetName_Proxy(
    IWMCodecStrings * This,
    /* [in] */ DMO_MEDIA_TYPE *pmt,
    /* [in] */ ULONG cchLength,
    /* [out] */ WCHAR *szName,
    /* [out] */ ULONG *pcchLength);


void __RPC_STUB IWMCodecStrings_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWMCodecStrings_GetDescription_Proxy(
    IWMCodecStrings * This,
    /* [in] */ DMO_MEDIA_TYPE *pmt,
    /* [in] */ ULONG cchLength,
    /* [out] */ WCHAR *szDescription,
    /* [out] */ ULONG *pcchLength);


void __RPC_STUB IWMCodecStrings_GetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWMCodecStrings_INTERFACE_DEFINED__ */


#ifndef __IWMCodecProps_INTERFACE_DEFINED__
#define __IWMCodecProps_INTERFACE_DEFINED__

/* interface IWMCodecProps */
/* [local][unique][helpstring][uuid][object] */


EXTERN_C const IID IID_IWMCodecProps;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("2573e11a-f01a-4fdd-a98d-63b8e0ba9589")
    IWMCodecProps : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetFormatProp(
            /* [in] */ DMO_MEDIA_TYPE *pmt,
            /* [in] */ LPCWSTR pszName,
            /* [out] */ WMT_PROP_DATATYPE *pType,
            /* [out] */ BYTE *pValue,
            /* [out][in] */ DWORD *pdwSize) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetCodecProp(
            /* [in] */ DWORD dwFormat,
            /* [in] */ LPCWSTR pszName,
            /* [out] */ WMT_PROP_DATATYPE *pType,
            /* [out] */ BYTE *pValue,
            /* [out][in] */ DWORD *pdwSize) = 0;

    };

#else 	/* C style interface */

    typedef struct IWMCodecPropsVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
            IWMCodecProps * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);

        ULONG ( STDMETHODCALLTYPE *AddRef )(
            IWMCodecProps * This);

        ULONG ( STDMETHODCALLTYPE *Release )(
            IWMCodecProps * This);

        HRESULT ( STDMETHODCALLTYPE *GetFormatProp )(
            IWMCodecProps * This,
            /* [in] */ DMO_MEDIA_TYPE *pmt,
            /* [in] */ LPCWSTR pszName,
            /* [out] */ WMT_PROP_DATATYPE *pType,
            /* [out] */ BYTE *pValue,
            /* [out][in] */ DWORD *pdwSize);

        HRESULT ( STDMETHODCALLTYPE *GetCodecProp )(
            IWMCodecProps * This,
            /* [in] */ DWORD dwFormat,
            /* [in] */ LPCWSTR pszName,
            /* [out] */ WMT_PROP_DATATYPE *pType,
            /* [out] */ BYTE *pValue,
            /* [out][in] */ DWORD *pdwSize);

        END_INTERFACE
    } IWMCodecPropsVtbl;

    interface IWMCodecProps
    {
        CONST_VTBL struct IWMCodecPropsVtbl *lpVtbl;
    };



#ifdef COBJMACROS


#define IWMCodecProps_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWMCodecProps_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWMCodecProps_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWMCodecProps_GetFormatProp(This,pmt,pszName,pType,pValue,pdwSize)	\
    (This)->lpVtbl -> GetFormatProp(This,pmt,pszName,pType,pValue,pdwSize)

#define IWMCodecProps_GetCodecProp(This,dwFormat,pszName,pType,pValue,pdwSize)	\
    (This)->lpVtbl -> GetCodecProp(This,dwFormat,pszName,pType,pValue,pdwSize)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWMCodecProps_GetFormatProp_Proxy(
    IWMCodecProps * This,
    /* [in] */ DMO_MEDIA_TYPE *pmt,
    /* [in] */ LPCWSTR pszName,
    /* [out] */ WMT_PROP_DATATYPE *pType,
    /* [out] */ BYTE *pValue,
    /* [out][in] */ DWORD *pdwSize);


void __RPC_STUB IWMCodecProps_GetFormatProp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWMCodecProps_GetCodecProp_Proxy(
    IWMCodecProps * This,
    /* [in] */ DWORD dwFormat,
    /* [in] */ LPCWSTR pszName,
    /* [out] */ WMT_PROP_DATATYPE *pType,
    /* [out] */ BYTE *pValue,
    /* [out][in] */ DWORD *pdwSize);


void __RPC_STUB IWMCodecProps_GetCodecProp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWMCodecProps_INTERFACE_DEFINED__ */


#ifndef __IWMCodecLeakyBucket_INTERFACE_DEFINED__
#define __IWMCodecLeakyBucket_INTERFACE_DEFINED__

/* interface IWMCodecLeakyBucket */
/* [local][unique][helpstring][uuid][object] */


EXTERN_C const IID IID_IWMCodecLeakyBucket;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("A81BA647-6227-43b7-B231-C7B15135DD7D")
    IWMCodecLeakyBucket : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetBufferSizeBits(
            /* [in] */ ULONG ulBufferSize) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetBufferSizeBits(
            /* [out] */ ULONG *pulBufferSize) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetBufferFullnessBits(
            /* [in] */ ULONG ulBufferFullness) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetBufferFullnessBits(
            /* [out] */ ULONG *pulBufferFullness) = 0;

    };

#else 	/* C style interface */

    typedef struct IWMCodecLeakyBucketVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
            IWMCodecLeakyBucket * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);

        ULONG ( STDMETHODCALLTYPE *AddRef )(
            IWMCodecLeakyBucket * This);

        ULONG ( STDMETHODCALLTYPE *Release )(
            IWMCodecLeakyBucket * This);

        HRESULT ( STDMETHODCALLTYPE *SetBufferSizeBits )(
            IWMCodecLeakyBucket * This,
            /* [in] */ ULONG ulBufferSize);

        HRESULT ( STDMETHODCALLTYPE *GetBufferSizeBits )(
            IWMCodecLeakyBucket * This,
            /* [out] */ ULONG *pulBufferSize);

        HRESULT ( STDMETHODCALLTYPE *SetBufferFullnessBits )(
            IWMCodecLeakyBucket * This,
            /* [in] */ ULONG ulBufferFullness);

        HRESULT ( STDMETHODCALLTYPE *GetBufferFullnessBits )(
            IWMCodecLeakyBucket * This,
            /* [out] */ ULONG *pulBufferFullness);

        END_INTERFACE
    } IWMCodecLeakyBucketVtbl;

    interface IWMCodecLeakyBucket
    {
        CONST_VTBL struct IWMCodecLeakyBucketVtbl *lpVtbl;
    };



#ifdef COBJMACROS


#define IWMCodecLeakyBucket_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWMCodecLeakyBucket_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWMCodecLeakyBucket_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWMCodecLeakyBucket_SetBufferSizeBits(This,ulBufferSize)	\
    (This)->lpVtbl -> SetBufferSizeBits(This,ulBufferSize)

#define IWMCodecLeakyBucket_GetBufferSizeBits(This,pulBufferSize)	\
    (This)->lpVtbl -> GetBufferSizeBits(This,pulBufferSize)

#define IWMCodecLeakyBucket_SetBufferFullnessBits(This,ulBufferFullness)	\
    (This)->lpVtbl -> SetBufferFullnessBits(This,ulBufferFullness)

#define IWMCodecLeakyBucket_GetBufferFullnessBits(This,pulBufferFullness)	\
    (This)->lpVtbl -> GetBufferFullnessBits(This,pulBufferFullness)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWMCodecLeakyBucket_SetBufferSizeBits_Proxy(
    IWMCodecLeakyBucket * This,
    /* [in] */ ULONG ulBufferSize);


void __RPC_STUB IWMCodecLeakyBucket_SetBufferSizeBits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWMCodecLeakyBucket_GetBufferSizeBits_Proxy(
    IWMCodecLeakyBucket * This,
    /* [out] */ ULONG *pulBufferSize);


void __RPC_STUB IWMCodecLeakyBucket_GetBufferSizeBits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWMCodecLeakyBucket_SetBufferFullnessBits_Proxy(
    IWMCodecLeakyBucket * This,
    /* [in] */ ULONG ulBufferFullness);


void __RPC_STUB IWMCodecLeakyBucket_SetBufferFullnessBits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWMCodecLeakyBucket_GetBufferFullnessBits_Proxy(
    IWMCodecLeakyBucket * This,
    /* [out] */ ULONG *pulBufferFullness);


void __RPC_STUB IWMCodecLeakyBucket_GetBufferFullnessBits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWMCodecLeakyBucket_INTERFACE_DEFINED__ */


#ifndef __IWMCodecMetaData_INTERFACE_DEFINED__
#define __IWMCodecMetaData_INTERFACE_DEFINED__

/* interface IWMCodecMetaData */
/* [local][unique][helpstring][uuid][object] */


EXTERN_C const IID IID_IWMCodecMetaData;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D051ED9F-BC5C-4e83-B14E-8428485C286A")
    IWMCodecMetaData : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetNumberOfValues(
            /* [out] */ ULONG *pulNumValues) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetBufferSizes(
            /* [in] */ ULONG ulValueIndex,
            /* [out] */ ULONG *pcbData,
            /* [out] */ ULONG *pcchName) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetValueAndName(
            /* [in] */ ULONG ulValueIndex,
            /* [out] */ BYTE *pbData,
            /* [in] */ ULONG cbData,
            /* [out] */ ULONG *pcbData,
            /* [out] */ WCHAR *szName,
            /* [in] */ ULONG cchName,
            /* [out] */ ULONG *pcchName) = 0;

    };

#else 	/* C style interface */

    typedef struct IWMCodecMetaDataVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
            IWMCodecMetaData * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);

        ULONG ( STDMETHODCALLTYPE *AddRef )(
            IWMCodecMetaData * This);

        ULONG ( STDMETHODCALLTYPE *Release )(
            IWMCodecMetaData * This);

        HRESULT ( STDMETHODCALLTYPE *GetNumberOfValues )(
            IWMCodecMetaData * This,
            /* [out] */ ULONG *pulNumValues);

        HRESULT ( STDMETHODCALLTYPE *GetBufferSizes )(
            IWMCodecMetaData * This,
            /* [in] */ ULONG ulValueIndex,
            /* [out] */ ULONG *pcbData,
            /* [out] */ ULONG *pcchName);

        HRESULT ( STDMETHODCALLTYPE *GetValueAndName )(
            IWMCodecMetaData * This,
            /* [in] */ ULONG ulValueIndex,
            /* [out] */ BYTE *pbData,
            /* [in] */ ULONG cbData,
            /* [out] */ ULONG *pcbData,
            /* [out] */ WCHAR *szName,
            /* [in] */ ULONG cchName,
            /* [out] */ ULONG *pcchName);

        END_INTERFACE
    } IWMCodecMetaDataVtbl;

    interface IWMCodecMetaData
    {
        CONST_VTBL struct IWMCodecMetaDataVtbl *lpVtbl;
    };



#ifdef COBJMACROS


#define IWMCodecMetaData_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWMCodecMetaData_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWMCodecMetaData_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWMCodecMetaData_GetNumberOfValues(This,pulNumValues)	\
    (This)->lpVtbl -> GetNumberOfValues(This,pulNumValues)

#define IWMCodecMetaData_GetBufferSizes(This,ulValueIndex,pcbData,pcchName)	\
    (This)->lpVtbl -> GetBufferSizes(This,ulValueIndex,pcbData,pcchName)

#define IWMCodecMetaData_GetValueAndName(This,ulValueIndex,pbData,cbData,pcbData,szName,cchName,pcchName)	\
    (This)->lpVtbl -> GetValueAndName(This,ulValueIndex,pbData,cbData,pcbData,szName,cchName,pcchName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWMCodecMetaData_GetNumberOfValues_Proxy(
    IWMCodecMetaData * This,
    /* [out] */ ULONG *pulNumValues);


void __RPC_STUB IWMCodecMetaData_GetNumberOfValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWMCodecMetaData_GetBufferSizes_Proxy(
    IWMCodecMetaData * This,
    /* [in] */ ULONG ulValueIndex,
    /* [out] */ ULONG *pcbData,
    /* [out] */ ULONG *pcchName);


void __RPC_STUB IWMCodecMetaData_GetBufferSizes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWMCodecMetaData_GetValueAndName_Proxy(
    IWMCodecMetaData * This,
    /* [in] */ ULONG ulValueIndex,
    /* [out] */ BYTE *pbData,
    /* [in] */ ULONG cbData,
    /* [out] */ ULONG *pcbData,
    /* [out] */ WCHAR *szName,
    /* [in] */ ULONG cchName,
    /* [out] */ ULONG *pcchName);


void __RPC_STUB IWMCodecMetaData_GetValueAndName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWMCodecMetaData_INTERFACE_DEFINED__ */


#ifndef __IWMCodecOutputTimestamp_INTERFACE_DEFINED__
#define __IWMCodecOutputTimestamp_INTERFACE_DEFINED__

/* interface IWMCodecOutputTimestamp */
/* [local][unique][helpstring][uuid][object] */


EXTERN_C const IID IID_IWMCodecOutputTimestamp;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B72ADF95-7ADC-4a72-BC05-577D8EA6BF68")
    IWMCodecOutputTimestamp : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetNextOutputTime(
            /* [out] */ REFERENCE_TIME *prtTime) = 0;

    };

#else 	/* C style interface */

    typedef struct IWMCodecOutputTimestampVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
            IWMCodecOutputTimestamp * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);

        ULONG ( STDMETHODCALLTYPE *AddRef )(
            IWMCodecOutputTimestamp * This);

        ULONG ( STDMETHODCALLTYPE *Release )(
            IWMCodecOutputTimestamp * This);

        HRESULT ( STDMETHODCALLTYPE *GetNextOutputTime )(
            IWMCodecOutputTimestamp * This,
            /* [out] */ REFERENCE_TIME *prtTime);

        END_INTERFACE
    } IWMCodecOutputTimestampVtbl;

    interface IWMCodecOutputTimestamp
    {
        CONST_VTBL struct IWMCodecOutputTimestampVtbl *lpVtbl;
    };



#ifdef COBJMACROS


#define IWMCodecOutputTimestamp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWMCodecOutputTimestamp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWMCodecOutputTimestamp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWMCodecOutputTimestamp_GetNextOutputTime(This,prtTime)	\
    (This)->lpVtbl -> GetNextOutputTime(This,prtTime)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWMCodecOutputTimestamp_GetNextOutputTime_Proxy(
    IWMCodecOutputTimestamp * This,
    /* [out] */ REFERENCE_TIME *prtTime);


void __RPC_STUB IWMCodecOutputTimestamp_GetNextOutputTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWMCodecOutputTimestamp_INTERFACE_DEFINED__ */


#ifndef __IWMCodecPrivateData_INTERFACE_DEFINED__
#define __IWMCodecPrivateData_INTERFACE_DEFINED__

/* interface IWMCodecPrivateData */
/* [local][unique][helpstring][uuid][object] */


EXTERN_C const IID IID_IWMCodecPrivateData;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("73F0BE8E-57F7-4f01-AA66-9F57340CFE0E")
    IWMCodecPrivateData : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetPartialOutputType(
            /* [in] */ DMO_MEDIA_TYPE *pmt) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetPrivateData(
            /* [out] */ BYTE *pbData,
            /* [out][in] */ ULONG *pcbData) = 0;

    };

#else 	/* C style interface */

    typedef struct IWMCodecPrivateDataVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
            IWMCodecPrivateData * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);

        ULONG ( STDMETHODCALLTYPE *AddRef )(
            IWMCodecPrivateData * This);

        ULONG ( STDMETHODCALLTYPE *Release )(
            IWMCodecPrivateData * This);

        HRESULT ( STDMETHODCALLTYPE *SetPartialOutputType )(
            IWMCodecPrivateData * This,
            /* [in] */ DMO_MEDIA_TYPE *pmt);

        HRESULT ( STDMETHODCALLTYPE *GetPrivateData )(
            IWMCodecPrivateData * This,
            /* [out] */ BYTE *pbData,
            /* [out][in] */ ULONG *pcbData);

        END_INTERFACE
    } IWMCodecPrivateDataVtbl;

    interface IWMCodecPrivateData
    {
        CONST_VTBL struct IWMCodecPrivateDataVtbl *lpVtbl;
    };



#ifdef COBJMACROS


#define IWMCodecPrivateData_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWMCodecPrivateData_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWMCodecPrivateData_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWMCodecPrivateData_SetPartialOutputType(This,pmt)	\
    (This)->lpVtbl -> SetPartialOutputType(This,pmt)

#define IWMCodecPrivateData_GetPrivateData(This,pbData,pcbData)	\
    (This)->lpVtbl -> GetPrivateData(This,pbData,pcbData)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWMCodecPrivateData_SetPartialOutputType_Proxy(
    IWMCodecPrivateData * This,
    /* [in] */ DMO_MEDIA_TYPE *pmt);


void __RPC_STUB IWMCodecPrivateData_SetPartialOutputType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWMCodecPrivateData_GetPrivateData_Proxy(
    IWMCodecPrivateData * This,
    /* [out] */ BYTE *pbData,
    /* [out][in] */ ULONG *pcbData);


void __RPC_STUB IWMCodecPrivateData_GetPrivateData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWMCodecPrivateData_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif






