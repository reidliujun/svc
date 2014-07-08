#ifndef _INC_VFW
#define _INC_VFW

#include <mmsystem.h>

#ifndef aviTWOCC
#define aviTWOCC(ch0, ch1) ((WORD)(BYTE)(ch0) | ((WORD)(BYTE)(ch1) << 8))
#endif

#ifndef ICTYPE_VIDEO
#define ICTYPE_VIDEO    mmioFOURCC('v', 'i', 'd', 'c')
#define ICTYPE_AUDIO    mmioFOURCC('a', 'u', 'd', 'c')
#endif

#ifndef ICERR_OK
#define ICERR_OK                0L
#define ICERR_DONTDRAW          1L
#define ICERR_NEWPALETTE        2L
#define ICERR_GOTOKEYFRAME	3L
#define ICERR_STOPDRAWING 	4L

#define ICERR_UNSUPPORTED      -1L
#define ICERR_BADFORMAT        -2L
#define ICERR_MEMORY           -3L
#define ICERR_INTERNAL         -4L
#define ICERR_BADFLAGS         -5L
#define ICERR_BADPARAM         -6L
#define ICERR_BADSIZE          -7L
#define ICERR_BADHANDLE        -8L
#define ICERR_CANTUPDATE       -9L
#define ICERR_ABORT	       -10L
#define ICERR_ERROR            -100L
#define ICERR_BADBITDEPTH      -200L
#define ICERR_BADIMAGESIZE     -201L

#define ICERR_CUSTOM           -400L    // errors less than ICERR_CUSTOM...
#endif

/* Values for dwFlags of ICOpen() */
#ifndef ICMODE_COMPRESS
#define ICMODE_COMPRESS		1
#define ICMODE_DECOMPRESS	2
#define ICMODE_FASTDECOMPRESS   3
#define ICMODE_QUERY            4
#define ICMODE_FASTCOMPRESS     5
#define ICMODE_DRAW             8
#endif
#ifndef _WIN32					// ;Internal
#define ICMODE_INTERNALF_FUNCTION32	0x8000	// ;Internal
#define ICMODE_INTERNALF_MASK		0x8000	// ;Internal
#endif						// ;Internal

/* Flags for AVI file index */
#define AVIIF_LIST	0x00000001L
#define AVIIF_TWOCC	0x00000002L
#define AVIIF_KEYFRAME	0x00000010L


#define ICCOMPRESS_KEYFRAME	0x00000001L

typedef struct {
    DWORD               dwFlags;        // flags

    LPBITMAPINFOHEADER  lpbiOutput;     // output format
    LPVOID              lpOutput;       // output data

    LPBITMAPINFOHEADER  lpbiInput;      // format of frame to compress
    LPVOID              lpInput;        // frame data to compress

    LPDWORD             lpckid;         // ckid for data in AVI file
    LPDWORD             lpdwFlags;      // flags in the AVI index.
    LONG                lFrameNum;      // frame number of seq.
    DWORD               dwFrameSize;    // reqested size in bytes. (if non zero)

    DWORD               dwQuality;      // quality

    // these are new fields
    LPBITMAPINFOHEADER  lpbiPrev;       // format of previous frame
    LPVOID              lpPrev;         // previous frame

} ICCOMPRESS;

#define ICCOMPRESSFRAMES_PADDING	0x00000001

typedef struct {
    DWORD               dwFlags;        // flags

    LPBITMAPINFOHEADER  lpbiOutput;     // output format
    LPARAM              lOutput;        // output identifier

    LPBITMAPINFOHEADER  lpbiInput;      // format of frame to compress
    LPARAM              lInput;         // input identifier

    LONG                lStartFrame;    // start frame
    LONG                lFrameCount;    // # of frames

    LONG                lQuality;       // quality
    LONG                lDataRate;      // data rate
    LONG                lKeyRate;       // key frame rate

    DWORD		dwRate;		// frame rate, as always
    DWORD		dwScale;

    DWORD		dwOverheadPerFrame;
    DWORD		dwReserved2;

    LONG (CALLBACK *GetData)(LPARAM lInput, LONG lFrame, LPVOID lpBits, LONG len);
    LONG (CALLBACK *PutData)(LPARAM lOutput, LONG lFrame, LPVOID lpBits, LONG len);
} ICCOMPRESSFRAMES;

#define ICDECOMPRESS_HURRYUP      0x80000000L   // don't draw just buffer (hurry up!)
#define ICDECOMPRESS_UPDATE       0x40000000L   // don't draw just update screen
#define ICDECOMPRESS_PREROLL      0x20000000L   // this frame is before real start
#define ICDECOMPRESS_NULLFRAME    0x10000000L   // repeat last frame
#define ICDECOMPRESS_NOTKEYFRAME  0x08000000L   // this frame is not a key frame

typedef struct {
    DWORD               dwFlags;    // flags (from AVI index...)

    LPBITMAPINFOHEADER  lpbiInput;  // BITMAPINFO of compressed data
                                    // biSizeImage has the chunk size
    LPVOID              lpInput;    // compressed data

    LPBITMAPINFOHEADER  lpbiOutput; // DIB to decompress to
    LPVOID              lpOutput;
    DWORD		ckid;	    // ckid from AVI file
} ICDECOMPRESS;

typedef struct {
    DWORD               dwSize;         // sizeof(ICOPEN)
    DWORD               fccType;        // 'vidc'
    DWORD               fccHandler;     //
    DWORD               dwVersion;      // version of compman opening you
    DWORD               dwFlags;        // LOWORD is type specific
    LRESULT             dwError;        // error return.
    LPVOID              pV1Reserved;    // Reserved
    LPVOID              pV2Reserved;    // Reserved
    DWORD               dnDevNode;      // Devnode for PnP devices
} ICOPEN;

#define ICM_USER          (DRV_USER+0x0000)

#define ICM_RESERVED      ICM_RESERVED_LOW
#define ICM_RESERVED_LOW  (DRV_USER+0x1000)
#define ICM_RESERVED_HIGH (DRV_USER+0x2000)

#define ICM_GETSTATE                (ICM_RESERVED+0)    // Get compressor state
#define ICM_SETSTATE                (ICM_RESERVED+1)    // Set compressor state
#define ICM_GETINFO                 (ICM_RESERVED+2)    // Query info about the compressor

#define ICM_CONFIGURE               (ICM_RESERVED+10)   // show the configure dialog
#define ICM_ABOUT                   (ICM_RESERVED+11)   // show the about box

#define ICM_GETERRORTEXT            (ICM_RESERVED+12)   // get error text TBD ;Internal
#define ICM_GETFORMATNAME	    (ICM_RESERVED+20)	// get a name for a format ;Internal
#define ICM_ENUMFORMATS		    (ICM_RESERVED+21)	// cycle through formats ;Internal

#define ICM_GETDEFAULTQUALITY       (ICM_RESERVED+30)   // get the default value for quality
#define ICM_GETQUALITY              (ICM_RESERVED+31)   // get the current value for quality
#define ICM_SETQUALITY              (ICM_RESERVED+32)   // set the default value for quality

#define ICM_SET			    (ICM_RESERVED+40)	// Tell the driver something
#define ICM_GET			    (ICM_RESERVED+41)	// Ask the driver something

// Constants for ICM_SET:
#define ICM_FRAMERATE       mmioFOURCC('F','r','m','R')
#define ICM_KEYFRAMERATE    mmioFOURCC('K','e','y','R')

typedef struct {
    DWORD   dwSize;                 // sizeof(ICINFO)
    DWORD   fccType;                // compressor type     'vidc' 'audc'
    DWORD   fccHandler;             // compressor sub-type 'rle ' 'jpeg' 'pcm '
    DWORD   dwFlags;                // flags LOWORD is type specific
    DWORD   dwVersion;              // version of the driver
    DWORD   dwVersionICM;           // version of the ICM used
    //
    // under Win32, the driver always returns UNICODE strings.
    //
    WCHAR   szName[16];             // short name
    WCHAR   szDescription[128];     // long name
    WCHAR   szDriver[128];          // driver that contains compressor
}   ICINFO;

/* Flags for the <dwFlags> field of the <ICINFO> structure. */
#define VIDCF_QUALITY        0x0001  // supports quality
#define VIDCF_CRUNCH         0x0002  // supports crunching to a frame size
#define VIDCF_TEMPORAL       0x0004  // supports inter-frame compress
#define VIDCF_COMPRESSFRAMES 0x0008  // wants the compress all frames message
#define VIDCF_DRAW           0x0010  // supports drawing
#define VIDCF_FASTTEMPORALC  0x0020  // does not need prev frame on compress
#define VIDCF_FASTTEMPORALD  0x0080  // does not need prev frame on decompress
//#define VIDCF_QUALITYTIME    0x0040  // supports temporal quality

//#define VIDCF_FASTTEMPORAL   (VIDCF_FASTTEMPORALC|VIDCF_FASTTEMPORALD)

#define ICVERSION       0x0104

#define ICM_COMPRESS_GET_FORMAT     (ICM_USER+4)    // get compress format or size
#define ICM_COMPRESS_GET_SIZE       (ICM_USER+5)    // get output size
#define ICM_COMPRESS_QUERY          (ICM_USER+6)    // query support for compress
#define ICM_COMPRESS_BEGIN          (ICM_USER+7)    // begin a series of compress calls.
#define ICM_COMPRESS                (ICM_USER+8)    // compress a frame
#define ICM_COMPRESS_END            (ICM_USER+9)    // end of a series of compress calls.

#define ICM_DECOMPRESS_GET_FORMAT   (ICM_USER+10)   // get decompress format or size
#define ICM_DECOMPRESS_QUERY        (ICM_USER+11)   // query support for dempress
#define ICM_DECOMPRESS_BEGIN        (ICM_USER+12)   // start a series of decompress calls
#define ICM_DECOMPRESS              (ICM_USER+13)   // decompress a frame
#define ICM_DECOMPRESS_END          (ICM_USER+14)   // end a series of decompress calls
#define ICM_DECOMPRESS_SET_PALETTE  (ICM_USER+29)   // fill in the DIB color table
#define ICM_DECOMPRESS_GET_PALETTE  (ICM_USER+30)   // fill in the DIB color table

#define ICM_DRAW_QUERY              (ICM_USER+31)   // query support for dempress
#define ICM_DRAW_BEGIN              (ICM_USER+15)   // start a series of draw calls
#define ICM_DRAW_GET_PALETTE        (ICM_USER+16)   // get the palette needed for drawing
#define ICM_DRAW_UPDATE             (ICM_USER+17)   // update screen with current frame ;Internal
#define ICM_DRAW_START              (ICM_USER+18)   // start decompress clock
#define ICM_DRAW_STOP               (ICM_USER+19)   // stop decompress clock
#define ICM_DRAW_BITS               (ICM_USER+20)   // decompress a frame to screen ;Internal
#define ICM_DRAW_END                (ICM_USER+21)   // end a series of draw calls
#define ICM_DRAW_GETTIME            (ICM_USER+32)   // get value of decompress clock
#define ICM_DRAW                    (ICM_USER+33)   // generalized "render" message
#define ICM_DRAW_WINDOW             (ICM_USER+34)   // drawing window has moved or hidden
#define ICM_DRAW_SETTIME            (ICM_USER+35)   // set correct value for decompress clock
#define ICM_DRAW_REALIZE            (ICM_USER+36)   // realize palette for drawing
#define ICM_DRAW_FLUSH	            (ICM_USER+37)   // clear out buffered frames
#define ICM_DRAW_RENDERBUFFER       (ICM_USER+38)   // draw undrawn things in queue

#define ICM_DRAW_START_PLAY         (ICM_USER+39)   // start of a play
#define ICM_DRAW_STOP_PLAY          (ICM_USER+40)   // end of a play

#define ICM_DRAW_SUGGESTFORMAT      (ICM_USER+50)   // Like ICGetDisplayFormat
#define ICM_DRAW_CHANGEPALETTE      (ICM_USER+51)   // for animating palette

#define ICM_DRAW_IDLE               (ICM_USER+52)   // send each frame time ;Internal

#define ICM_GETBUFFERSWANTED        (ICM_USER+41)   // ask about prebuffering

#define ICM_GETDEFAULTKEYFRAMERATE  (ICM_USER+42)   // get the default value for key frames


#define ICM_DECOMPRESSEX_BEGIN      (ICM_USER+60)   // start a series of decompress calls
#define ICM_DECOMPRESSEX_QUERY      (ICM_USER+61)   // start a series of decompress calls
#define ICM_DECOMPRESSEX            (ICM_USER+62)   // decompress a frame
#define ICM_DECOMPRESSEX_END        (ICM_USER+63)   // end a series of decompress calls

#define ICM_COMPRESS_FRAMES_INFO    (ICM_USER+70)   // tell about compress to come
#define ICM_COMPRESS_FRAMES         (ICM_USER+71)   // compress a bunch of frames ;Internal
#define ICM_SET_STATUS_PROC	        (ICM_USER+72)   // set status callback

typedef struct _AVIFILEINFOW {
    DWORD		dwMaxBytesPerSec;	// max. transfer rate
    DWORD		dwFlags;		// the ever-present flags
    DWORD		dwCaps;
    DWORD		dwStreams;
    DWORD		dwSuggestedBufferSize;

    DWORD		dwWidth;
    DWORD		dwHeight;

    DWORD		dwScale;
    DWORD		dwRate;	/* dwRate / dwScale == samples/second */
    DWORD		dwLength;

    DWORD		dwEditCount;

    WCHAR		szFileType[64];		// descriptive string for file type?
} AVIFILEINFOW, FAR * LPAVIFILEINFOW;

typedef struct _AVIFILEINFOA {
    DWORD		dwMaxBytesPerSec;	// max. transfer rate
    DWORD		dwFlags;		// the ever-present flags
    DWORD		dwCaps;
    DWORD		dwStreams;
    DWORD		dwSuggestedBufferSize;

    DWORD		dwWidth;
    DWORD		dwHeight;

    DWORD		dwScale;
    DWORD		dwRate;	/* dwRate / dwScale == samples/second */
    DWORD		dwLength;

    DWORD		dwEditCount;

    char		szFileType[64];		// descriptive string for file type?
} AVIFILEINFOA, FAR * LPAVIFILEINFOA;

typedef struct _AVISTREAMINFOW {
    DWORD		fccType;
    DWORD               fccHandler;
    DWORD               dwFlags;        /* Contains AVITF_* flags */
    DWORD		dwCaps;
    WORD		wPriority;
    WORD		wLanguage;
    DWORD               dwScale;
    DWORD               dwRate; /* dwRate / dwScale == samples/second */
    DWORD               dwStart;
    DWORD               dwLength; /* In units above... */
    DWORD		dwInitialFrames;
    DWORD               dwSuggestedBufferSize;
    DWORD               dwQuality;
    DWORD               dwSampleSize;
    RECT                rcFrame;
    DWORD		dwEditCount;
    DWORD		dwFormatChangeCount;
    WCHAR		szName[64];
} AVISTREAMINFOW, FAR * LPAVISTREAMINFOW;

typedef struct _AVISTREAMINFOA {
    DWORD		fccType;
    DWORD               fccHandler;
    DWORD               dwFlags;        /* Contains AVITF_* flags */
    DWORD		dwCaps;
    WORD		wPriority;
    WORD		wLanguage;
    DWORD               dwScale;
    DWORD               dwRate; /* dwRate / dwScale == samples/second */
    DWORD               dwStart;
    DWORD               dwLength; /* In units above... */
    DWORD		dwInitialFrames;
    DWORD               dwSuggestedBufferSize;
    DWORD               dwQuality;
    DWORD               dwSampleSize;
    RECT                rcFrame;
    DWORD		dwEditCount;
    DWORD		dwFormatChangeCount;
    char		szName[64];
} AVISTREAMINFOA, FAR * LPAVISTREAMINFOA;

#undef  INTERFACE
#define INTERFACE   IAVIStream

DECLARE_INTERFACE_(IAVIStream, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IAVIStream methods ***
    STDMETHOD(Create)      (THIS_ LPARAM lParam1, LPARAM lParam2) PURE ;
    STDMETHOD(Info)        (THIS_ AVISTREAMINFOW FAR * psi, LONG lSize) PURE ;
    STDMETHOD_(LONG, FindSample)(THIS_ LONG lPos, LONG lFlags) PURE ;
    STDMETHOD(ReadFormat)  (THIS_ LONG lPos,
			    LPVOID lpFormat, LONG FAR *lpcbFormat) PURE ;
    STDMETHOD(SetFormat)   (THIS_ LONG lPos,
			    LPVOID lpFormat, LONG cbFormat) PURE ;
    STDMETHOD(Read)        (THIS_ LONG lStart, LONG lSamples,
			    LPVOID lpBuffer, LONG cbBuffer,
			    LONG FAR * plBytes, LONG FAR * plSamples) PURE ;
    STDMETHOD(Write)       (THIS_ LONG lStart, LONG lSamples,
			    LPVOID lpBuffer, LONG cbBuffer,
			    DWORD dwFlags,
			    LONG FAR *plSampWritten,
			    LONG FAR *plBytesWritten) PURE ;
    STDMETHOD(Delete)      (THIS_ LONG lStart, LONG lSamples) PURE;
    STDMETHOD(ReadData)    (THIS_ DWORD fcc, LPVOID lp, LONG FAR *lpcb) PURE ;
    STDMETHOD(WriteData)   (THIS_ DWORD fcc, LPVOID lp, LONG cb) PURE ;
#ifdef _WIN32
    STDMETHOD(SetInfo) (THIS_ AVISTREAMINFOW FAR * lpInfo,
			    LONG cbInfo) PURE;
#else
    STDMETHOD(Reserved1)            (THIS) PURE;
    STDMETHOD(Reserved2)            (THIS) PURE;
    STDMETHOD(Reserved3)            (THIS) PURE;
    STDMETHOD(Reserved4)            (THIS) PURE;
    STDMETHOD(Reserved5)            (THIS) PURE;
#endif
};

typedef       IAVIStream FAR* PAVISTREAM;

#undef  INTERFACE
#define INTERFACE   IAVIFile
#define PAVIFILE IAVIFile FAR*

DECLARE_INTERFACE_(IAVIFile, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;

    // *** IAVIFile methods ***
    STDMETHOD(Info)                 (THIS_
                                     AVIFILEINFOW FAR * pfi,
                                     LONG lSize) PURE;
    STDMETHOD(GetStream)            (THIS_
                                     PAVISTREAM FAR * ppStream,
				     DWORD fccType,
                                     LONG lParam) PURE;
    STDMETHOD(CreateStream)         (THIS_
                                     PAVISTREAM FAR * ppStream,
                                     AVISTREAMINFOW FAR * psi) PURE;
    STDMETHOD(WriteData)            (THIS_
                                     DWORD ckid,
                                     LPVOID lpData,
                                     LONG cbData) PURE;
    STDMETHOD(ReadData)             (THIS_
                                     DWORD ckid,
                                     LPVOID lpData,
                                     LONG FAR *lpcbData) PURE;
    STDMETHOD(EndRecord)            (THIS) PURE;
    STDMETHOD(DeleteStream)         (THIS_
				     DWORD fccType,
                                     LONG lParam) PURE;
};

#undef PAVIFILE
typedef       IAVIFile FAR* PAVIFILE;

#ifdef _WIN32
STDAPI AVIFileOpenA       (PAVIFILE FAR * ppfile, LPCSTR szFile,
			  UINT uMode, LPCLSID lpHandler);
STDAPI AVIFileOpenW       (PAVIFILE FAR * ppfile, LPCWSTR szFile,
			  UINT uMode, LPCLSID lpHandler);
#ifdef UNICODE
#define AVIFileOpen	  AVIFileOpenW
#else
#define AVIFileOpen	  AVIFileOpenA
#endif
#else // win16
STDAPI AVIFileOpen       (PAVIFILE FAR * ppfile, LPCSTR szFile,
			  UINT uMode, LPCLSID lpHandler);
#define AVIFileOpenW	AVIFileOpen	    /* ;Internal */
#endif

#ifdef _WIN32
STDAPI AVIFileInfoW (PAVIFILE pfile, LPAVIFILEINFOW pfi, LONG lSize);
STDAPI AVIFileInfoA (PAVIFILE pfile, LPAVIFILEINFOA pfi, LONG lSize);
#ifdef UNICODE
#define AVIFileInfo	AVIFileInfoW
#else
#define AVIFileInfo	AVIFileInfoA
#endif
#else //win16 version
STDAPI AVIFileInfo (PAVIFILE pfile, LPAVIFILEINFO pfi, LONG lSize);
#define AVIFileInfoW AVIFileInfo	    /* ;Internal */
#endif

#define streamtypeVIDEO         mmioFOURCC('v', 'i', 'd', 's')
#define streamtypeAUDIO         mmioFOURCC('a', 'u', 'd', 's')

#ifdef _WIN32
STDAPI AVIFileCreateStreamW (PAVIFILE pfile, PAVISTREAM FAR *ppavi, AVISTREAMINFOW FAR * psi);
STDAPI AVIFileCreateStreamA (PAVIFILE pfile, PAVISTREAM FAR *ppavi, AVISTREAMINFOA FAR * psi);
#ifdef UNICODE
#define AVIFileCreateStream	AVIFileCreateStreamW
#else
#define AVIFileCreateStream	AVIFileCreateStreamA
#endif
#else //win16 version
STDAPI AVIFileCreateStream(PAVIFILE pfile, PAVISTREAM FAR *ppavi, AVISTREAMINFO FAR * psi);
#define AVIFileCreateStreamW AVIFileCreateStream	    /* ;Internal */
#endif

STDAPI AVIStreamSetFormat    (PAVISTREAM pavi, LONG lPos,LPVOID lpFormat,LONG cbFormat);

STDAPI AVIStreamWrite        (PAVISTREAM pavi,
			      LONG lStart, LONG lSamples,
			      LPVOID lpBuffer, LONG cbBuffer, DWORD dwFlags,
			      LONG FAR *plSampWritten,
			      LONG FAR *plBytesWritten);

STDAPI_(void) AVIFileInit(void);
STDAPI_(void) AVIFileExit(void);

STDAPI_(ULONG) AVIFileRelease      (PAVIFILE pfile);

#endif
