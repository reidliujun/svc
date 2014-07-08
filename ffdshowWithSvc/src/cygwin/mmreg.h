#ifndef _INC_MMREG
#define _INC_MMREG

typedef struct wavefilter_tag {
    DWORD   cbStruct;
    DWORD   dwFilterTag;
    DWORD   fdwFilter;
    DWORD   dwReserved[5];
} WAVEFILTER;
typedef WAVEFILTER       *PWAVEFILTER;
typedef WAVEFILTER NEAR *NPWAVEFILTER;
typedef WAVEFILTER FAR  *LPWAVEFILTER;

typedef struct mpeg1waveformat_tag {
    WAVEFORMATEX    wfx;
    WORD            fwHeadLayer;
    DWORD           dwHeadBitrate;
    WORD            fwHeadMode;
    WORD            fwHeadModeExt;
    WORD            wHeadEmphasis;
    WORD            fwHeadFlags;
    DWORD           dwPTSLow;
    DWORD           dwPTSHigh;
} MPEG1WAVEFORMAT;
typedef MPEG1WAVEFORMAT                 *PMPEG1WAVEFORMAT;
typedef MPEG1WAVEFORMAT NEAR           *NPMPEG1WAVEFORMAT;
typedef MPEG1WAVEFORMAT FAR            *LPMPEG1WAVEFORMAT;

#define WAVE_FORMAT_UNKNOWN          0x0000
#define WAVE_FORMAT_ADPCM            0x0002
#define WAVE_FORMAT_IEEE_FLOAT       0x0003
#define WAVE_FORMAT_ALAW             0x0006
#define WAVE_FORMAT_MULAW            0x0007
#define WAVE_FORMAT_GSM610           0x0031
#define WAVE_FORMAT_MPEG             0x0050
#define WAVE_FORMAT_MPEGLAYER3       0x0055
#define WAVE_FORMAT_DOLBY_AC3_SPDIF  0x0092
#ifndef WAVE_FORMAT_YAMAHA_ADPCM
 #define  WAVE_FORMAT_YAMAHA_ADPCM               0x0020 /* Yamaha Corporation of America */
#endif
#ifndef WAVE_FORMAT_CREATIVE_ADPCM
 #define  WAVE_FORMAT_CREATIVE_ADPCM             0x0200 /* Creative Labs, Inc */
#endif
#ifndef WAVE_FORMAT_DSPGROUP_TRUESPEECH
 #define  WAVE_FORMAT_DSPGROUP_TRUESPEECH        0x0022 /* DSP Group, Inc */
#endif


#define ACM_MPEG_LAYER1             (0x0001)
#define ACM_MPEG_LAYER2             (0x0002)
#define ACM_MPEG_LAYER3             (0x0004)

#ifndef _WAVEFORMATEXTENSIBLE_
#define _WAVEFORMATEXTENSIBLE_
typedef struct {
    WAVEFORMATEX    Format;
    union {
        WORD wValidBitsPerSample;       /* bits of precision  */
        WORD wSamplesPerBlock;          /* valid if wBitsPerSample==0 */
        WORD wReserved;                 /* If neither applies, set to zero. */
    } Samples;
    DWORD           dwChannelMask;      /* which channels are */
                                        /* present in stream  */
    GUID            SubFormat;
} WAVEFORMATEXTENSIBLE, *PWAVEFORMATEXTENSIBLE;
#endif // !_WAVEFORMATEXTENSIBLE_

#endif
