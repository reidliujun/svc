/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include <shlwapi.h>
#include <initguid.h>
#include "TffDecoderVideo.h"
#include "TffdshowDecAudio.h"
#include "TffdshowPageDecVideo.h"
#include "TffdshowPageDecAudio.h"
#include "TffdshowPageEnc.h"
#include "TffdshowEnc.h"
#include "TffProc.h"
#include "TffProcAudio.h"
#include "Tffvfw.h"
#include "Tacm.h"
#include "ffdshow_mediaguids.h"
#include "reg.h"
#include "dsutil.h"
#include "IffdshowParamsEnum.h"
#include "matroskaSubs.h"
#include "IPropertyPageFF.h"
#include "Tconvert.h"

DEFINE_GUID(IID_IAMExtendedSeeking,0xFA2AA8F9L,0x8B62,0x11D0,0xA5,0x20,0x00,0x00,0x00,0x00,0x00,0x00);

// {8E9922F0-B775-45b8-B650-941BEA790EEB}
DEFINE_GUID(CLSID_DWSTRINGA, 0x8e9922f0, 0xb775, 0x45b8, 0xb6, 0x50, 0x94, 0x1b, 0xea, 0x79, 0x0e, 0xeb);
// {A0606860-51BE-4cf6-99C0-7CE5F78AC2D8}
DEFINE_GUID(CLSID_DWSTRINGW, 0xa0606860, 0x51be, 0x4cf6, 0x99, 0xc0, 0x7c, 0xe5, 0xf7, 0x8a, 0xc2, 0xd8);


#define FOURCC_xvid mmioFOURCC('x','v','i','d')
#define FOURCC_divx mmioFOURCC('d','i','v','x')
#define FOURCC_dx50 mmioFOURCC('d','x','5','0')
#define FOURCC_h263 mmioFOURCC('h','2','6','3')
#define FOURCC_div3 mmioFOURCC('d','i','v','3')
#define FOURCC_mp43 mmioFOURCC('m','p','4','3')
#define FOURCC_mp42 mmioFOURCC('m','p','4','2')
#define FOURCC_mp41 mmioFOURCC('m','p','4','1')
#define FOURCC_mp4v mmioFOURCC('m','p','4','v')
#define FOURCC_mp4s mmioFOURCC('m','p','4','s')
#define FOURCC_sedg mmioFOURCC('s','e','d','g')
#define FOURCC_wmv1 mmioFOURCC('w','m','v','1')
#define FOURCC_wmv2 mmioFOURCC('w','m','v','2')
#define FOURCC_wmv3 mmioFOURCC('w','m','v','3')
#define FOURCC_wvc1 mmioFOURCC('w','v','c','1')
#define FOURCC_wmva mmioFOURCC('w','m','v','a')
#define FOURCC_vp50 mmioFOURCC('v','p','5','0')
#define FOURCC_vp60 mmioFOURCC('v','p','6','0')
#define FOURCC_vp61 mmioFOURCC('v','p','6','1')
#define FOURCC_vp62 mmioFOURCC('v','p','6','2')
#define FOURCC_vp6f mmioFOURCC('v','p','6','f')
#define FOURCC_flv4 mmioFOURCC('f','l','v','4')
#define FOURCC_hfyu mmioFOURCC('h','f','y','u')
#define FOURCC_3iv1 mmioFOURCC('3','i','v','1')
#define FOURCC_3iv2 mmioFOURCC('3','i','v','2')
#define FOURCC_3ivx mmioFOURCC('3','i','v','x')
#define FOURCC_mpg2 mmioFOURCC('m','p','g','2')
#define FOURCC_mjpg mmioFOURCC('m','j','p','g')
#define FOURCC_mjpa mmioFOURCC('m','j','p','a')
#define FOURCC_dvsd mmioFOURCC('d','v','s','d')
#define FOURCC_dv25 mmioFOURCC('d','v','2','5')
#define FOURCC_dv50 mmioFOURCC('d','v','5','0')
#define FOURCC_cdvc mmioFOURCC('c','d','v','c')
#define FOURCC_cdv5 mmioFOURCC('c','d','v','5')
#define FOURCC_dvis mmioFOURCC('d','v','i','s')
#define FOURCC_pdvc mmioFOURCC('p','d','v','c')
#define FOURCC_vp31 mmioFOURCC('v','p','3','1')
#define FOURCC_tscc mmioFOURCC('t','s','c','c')
#define FOURCC_cram mmioFOURCC('c','r','a','m')
#define FOURCC_avrn mmioFOURCC('a','v','r','n')
#define FOURCC_cavs mmioFOURCC('c','a','v','s')
#define FOURCC_em2v mmioFOURCC('e','m','2','v')
#define FOURCC_fps1 mmioFOURCC('f','p','s','1')
#define FOURCC_mmes mmioFOURCC('m','m','e','s')
#define FOURCC_wmvp mmioFOURCC('w','m','v','p')
#define FOURCC_wvp2 mmioFOURCC('w','v','p','2')

DEFINE_GUID_FOURCC(XVID)
DEFINE_GUID_FOURCC(xvid)
DEFINE_GUID_FOURCC(DIVX)
DEFINE_GUID_FOURCC(divx)
DEFINE_GUID_FOURCC(DX50)
DEFINE_GUID_FOURCC(dx50)
//DEFINE_GUID_FOURCC(H264) moved to ffdshow_mediaguids.h to be used outside of this file.
//DEFINE_GUID_FOURCC(h264)
//DEFINE_GUID_FOURCC(AVC1)
//DEFINE_GUID_FOURCC(avc1)
DEFINE_GUID_FOURCC(H263)
DEFINE_GUID_FOURCC(h263)
DEFINE_GUID_FOURCC(DIV3)
DEFINE_GUID_FOURCC(div3)
DEFINE_GUID_FOURCC(MP43)
DEFINE_GUID_FOURCC(mp43)
DEFINE_GUID_FOURCC(MP42)
DEFINE_GUID_FOURCC(mp42)
DEFINE_GUID_FOURCC(MP41)
DEFINE_GUID_FOURCC(mp41)
DEFINE_GUID_FOURCC(MP4V)
DEFINE_GUID_FOURCC(mp4v)
DEFINE_GUID_FOURCC(MP4S)
DEFINE_GUID_FOURCC(mp4s)
DEFINE_GUID_FOURCC(SEDG)
DEFINE_GUID_FOURCC(sedg)
DEFINE_GUID_FOURCC(WMV1)
DEFINE_GUID_FOURCC(wmv1)
DEFINE_GUID_FOURCC(WMV2)
DEFINE_GUID_FOURCC(wmv2)
DEFINE_GUID_FOURCC(WMV3)
DEFINE_GUID_FOURCC(wmv3)
DEFINE_GUID_FOURCC(WVC1)
DEFINE_GUID_FOURCC(wvc1)
DEFINE_GUID_FOURCC(WMVA)
DEFINE_GUID_FOURCC(wmva)
DEFINE_GUID_FOURCC(VP50)
DEFINE_GUID_FOURCC(vp50)
DEFINE_GUID_FOURCC(VP60)
DEFINE_GUID_FOURCC(vp60)
DEFINE_GUID_FOURCC(VP61)
DEFINE_GUID_FOURCC(vp61)
DEFINE_GUID_FOURCC(VP62)
DEFINE_GUID_FOURCC(vp62)
DEFINE_GUID_FOURCC(VP6F)
DEFINE_GUID_FOURCC(vp6f)
DEFINE_GUID_FOURCC(VP6A)
DEFINE_GUID_FOURCC(FLV4)
DEFINE_GUID_FOURCC(flv4)
DEFINE_GUID_FOURCC(HFYU)
DEFINE_GUID_FOURCC(hfyu)
DEFINE_GUID_FOURCC(3IV1)
DEFINE_GUID_FOURCC(3iv1)
DEFINE_GUID_FOURCC(3IV2)
DEFINE_GUID_FOURCC(3iv2)
DEFINE_GUID_FOURCC(3IVX)
DEFINE_GUID_FOURCC(3ivx)
DEFINE_GUID_FOURCC(MPG2)
DEFINE_GUID_FOURCC(mpg2)
DEFINE_GUID_FOURCC(MJPG)
DEFINE_GUID_FOURCC(mjpg)
DEFINE_GUID_FOURCC(MJPA)
DEFINE_GUID_FOURCC(mjpa)
DEFINE_GUID_FOURCC(DVSD)
DEFINE_GUID_FOURCC(dvsd)
DEFINE_GUID_FOURCC(DV25)
DEFINE_GUID_FOURCC(dv25)
DEFINE_GUID_FOURCC(DV50)
DEFINE_GUID_FOURCC(dv50)
DEFINE_GUID_FOURCC(CDVC)
DEFINE_GUID_FOURCC(cdvc)
DEFINE_GUID_FOURCC(CDV5)
DEFINE_GUID_FOURCC(cdv5)
DEFINE_GUID_FOURCC(DVIS)
DEFINE_GUID_FOURCC(dvis)
DEFINE_GUID_FOURCC(PDVC)
DEFINE_GUID_FOURCC(pdvc)
DEFINE_GUID_FOURCC(VP31)
DEFINE_GUID_FOURCC(vp31)
DEFINE_GUID_FOURCC(TSCC)
DEFINE_GUID_FOURCC(tscc)
DEFINE_GUID_FOURCC(CRAM)
DEFINE_GUID_FOURCC(cram)
DEFINE_GUID_FOURCC(AVRN)
DEFINE_GUID_FOURCC(avrn)
DEFINE_GUID_FOURCC(CAVS)
DEFINE_GUID_FOURCC(cavs)
DEFINE_GUID_FOURCC(EM2V)
DEFINE_GUID_FOURCC(em2v)
DEFINE_GUID_FOURCC(FPS1)
DEFINE_GUID_FOURCC(fps1)
DEFINE_GUID_FOURCC(MMES)
DEFINE_GUID_FOURCC(mmes)
DEFINE_GUID_FOURCC(WMVP)
DEFINE_GUID_FOURCC(wmvp)
DEFINE_GUID_FOURCC(WVP2)
DEFINE_GUID_FOURCC(wvp2)
DEFINE_GUID_FOURCC(AMVV)
DEFINE_GUID_FOURCC(SP5X)

const AMOVIESETUP_MEDIATYPE TffdshowDecVideo::inputMediaTypes[]=
{
 { &MEDIATYPE_Video,              &MEDIASUBTYPE_NULL   },
 { &MEDIATYPE_DVD_ENCRYPTED_PACK, &MEDIASUBTYPE_NULL   },
 { &MEDIATYPE_DVD_ENCRYPTED_PACK, &MEDIASUBTYPE_MPEG2_VIDEO   },
 { &MEDIATYPE_DVD_ENCRYPTED_PACK, &MEDIASUBTYPE_AVC1   },
 { &MEDIATYPE_DVD_ENCRYPTED_PACK, &MEDIASUBTYPE_WVC1   },
// Few most used FourCCs
 { &MEDIATYPE_Video, &MEDIASUBTYPE_XVID   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_xvid   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_DIVX   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_divx   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_DX50   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_dx50   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_H264   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_h264   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_AVC1   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_avc1   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_H263   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_h263   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_DIV3   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_div3   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MP43   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_mp43   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MP42   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_mp42   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MP41   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_mp41   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MP4V   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_mp4v   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MP4S   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_mp4s   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_SEDG   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_sedg   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_WMV1   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_wmv1   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_WMV2   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_wmv2   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_WMV3   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_wmv3   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_WVC1   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_wvc1   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_VP50   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_vp50   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_VP60   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_vp60   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_VP61   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_vp61   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_VP62   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_vp62   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_VP6F   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_vp6f   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_FLV4   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_flv4   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_HFYU   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_hfyu   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_3IV2   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_3iv2   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_3IVX   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_3ivx   },
// MPEG1 & 2 Video MediaSubType & FourCCs
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MPEG1Packet   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MPEG1Payload   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MPEG2_VIDEO   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MPG2   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_mpg2   },
// Other MPEG2
 { &MEDIATYPE_Video, &MEDIASUBTYPE_EM2V   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_em2v   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MMES   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_mmes   },
// Some less common FourCCs
 { &MEDIATYPE_Video, &MEDIASUBTYPE_VP31   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_vp31   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_TSCC   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_tscc   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_CRAM   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_cram   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_AVRN   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_avrn   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_FPS1   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_fps1   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_WMVP   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_wmvp   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_WVP2   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_wvp2   },
// MJPEG Video MediaSubType & FourCCs
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MJPG   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_mjpg   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_MJPA   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_mjpa   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_AMVV   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_SP5X   },
// DVSD Video MediaSubType & FourCCs
 { &MEDIATYPE_Video, &MEDIASUBTYPE_DVSD   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_dvsd   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_DV25   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_dv25   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_DV50   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_dv50   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_CDVC   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_cdvc   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_CDV5   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_cdv5   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_DVIS   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_dvis   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_PDVC   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_pdvc   },
// explicitly register for raw video types for easier enumeration
 { &MEDIATYPE_Video, &MEDIASUBTYPE_YV12   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_IYUV   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_YUY2   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_YUYV   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_UYVY   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_VYUY   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB32  },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB24  },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB555 },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB565 }
};

const AMOVIESETUP_MEDIATYPE TffdshowDecVideoRaw::inputMediaTypes[]=
{
 { &MEDIATYPE_Video,              &MEDIASUBTYPE_NULL   },
// explicitly register for raw video types for easier enumeration
 { &MEDIATYPE_Video, &MEDIASUBTYPE_YV12   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_IYUV   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_YUY2   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_YUYV   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_UYVY   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_VYUY   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB32  },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB24  },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB555 },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB565 }
};

const AMOVIESETUP_MEDIATYPE TffdshowDecVideoSubtitles::inputMediaTypes[]=
{
 { &MEDIATYPE_Video,              &MEDIASUBTYPE_NULL   },
// explicitly register for raw video types for easier enumeration
 { &MEDIATYPE_Video, &MEDIASUBTYPE_YV12   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_IYUV   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_YUY2   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_YUYV   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_UYVY   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_VYUY   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB32  },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB24  },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB555 },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB565 }
};

const AMOVIESETUP_MEDIATYPE TffdshowDecVideo::outputMediaTypes[]=
{
 { &MEDIATYPE_Video, &MEDIASUBTYPE_NULL   }
};

const AMOVIESETUP_MEDIATYPE TffdshowDecAudio::inputMediaTypes[]=
{
 { &MEDIATYPE_Audio              , &MEDIASUBTYPE_MP3               },
 { &MEDIATYPE_Audio              , &MEDIASUBTYPE_MPEG1AudioPayload },
 { &MEDIATYPE_Audio              , &MEDIASUBTYPE_MPEG1Payload      },
 { &MEDIATYPE_Audio              , &MEDIASUBTYPE_MPEG1Packet       },
 { &MEDIATYPE_Audio              , &MEDIASUBTYPE_MPEG2_AUDIO       },
 { &MEDIATYPE_MPEG2_PACK         , &MEDIASUBTYPE_MPEG2_AUDIO       },
 { &MEDIATYPE_MPEG2_PES          , &MEDIASUBTYPE_MPEG2_AUDIO       },
 { &MEDIATYPE_DVD_ENCRYPTED_PACK , &MEDIASUBTYPE_MPEG2_AUDIO       },
 { &MEDIATYPE_Audio              , &MEDIASUBTYPE_AC3_W     },
 { &MEDIATYPE_MPEG2_PES          , &MEDIASUBTYPE_DOLBY_AC3 },
 { &MEDIATYPE_MPEG2_PACK         , &MEDIASUBTYPE_DOLBY_AC3 },
 { &MEDIATYPE_Audio              , &MEDIASUBTYPE_DOLBY_AC3 },
 { &MEDIATYPE_DVD_ENCRYPTED_PACK , &MEDIASUBTYPE_DOLBY_AC3 },
 { &MEDIATYPE_Audio              , &MEDIASUBTYPE_DTS_W },
 { &MEDIATYPE_MPEG2_PES          , &MEDIASUBTYPE_DTS   },
 { &MEDIATYPE_MPEG2_PACK         , &MEDIASUBTYPE_DTS   },
 { &MEDIATYPE_Audio              , &MEDIASUBTYPE_DTS   },
 { &MEDIATYPE_DVD_ENCRYPTED_PACK , &MEDIASUBTYPE_DTS   },
 { &MEDIATYPE_MPEG2_PACK         , &MEDIASUBTYPE_DVD_LPCM_AUDIO },
 { &MEDIATYPE_MPEG2_PES          , &MEDIASUBTYPE_DVD_LPCM_AUDIO },
 { &MEDIATYPE_Audio              , &MEDIASUBTYPE_DVD_LPCM_AUDIO },
 { &MEDIATYPE_DVD_ENCRYPTED_PACK , &MEDIASUBTYPE_DVD_LPCM_AUDIO },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_AAC1       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_AAC2       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_AAC3       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_AAC4       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_AAC5       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_WMA7       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_WMA8       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_AMR        },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_SAMR       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_IMA_DK3    },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_IMA_WAV    },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_IMA_DK4    },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_IMA4       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_IMA_AMV    },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_ADPCM_SWF  },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_MSADPCM    },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_CREATIVE_ADPCM },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_YAMAHA_ADPCM },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_G726       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_FLAC       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_FLAC2      },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_TTA        },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_FFVORBIS   },
 //{ &MEDIATYPE_Audio , &MEDIASUBTYPE_VORBIS1    },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_AVIS       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_alaw       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_ALAW       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_MULAW      },
 /*{ &MEDIATYPE_Audio , &MEDIASUBTYPE_QT_ULAW    },*/
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_ulaw       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_ULAW       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_GSM610     },
 /*{ &MEDIATYPE_Audio , &MEDIASUBTYPE_QT_GSM     },*/
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_MAC3       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_MAC6       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_qdm2       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_QDM2       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_sowt       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_SOWT       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_twos       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_TWOS       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_in32       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_IN32       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_in24       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_IN24       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_fl32       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_FL32       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_fl64       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_FL64       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_DSPGROUP_TRUESPEECH },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_14_4       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_28_8       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_IMC        },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_ATRAC3     },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_NELLYMOSER },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_Vorbis     },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_Vorbis2    },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_VorbisIll  },
 //{ &MEDIATYPE_Audio , &MEDIASUBTYPE_EAC3       },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_PCM        },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_RAW        },
 { &MEDIATYPE_Audio , &MEDIASUBTYPE_IEEE_FLOAT }
};

const AMOVIESETUP_MEDIATYPE TffdshowDecAudio::outputMediaTypes[]=
{
 { &MEDIATYPE_Audio, &MEDIASUBTYPE_PCM }
};

const AMOVIESETUP_MEDIATYPE TffdshowDecAudioRaw::inputMediaTypes[]=
{
 { &MEDIATYPE_Audio, &MEDIASUBTYPE_NULL }
};
const AMOVIESETUP_MEDIATYPE TffdshowDecAudioRaw::outputMediaTypes[]=
{
 { &MEDIATYPE_Audio, &MEDIASUBTYPE_NULL }
};

const AMOVIESETUP_MEDIATYPE inputTextMediaTypes[]=
{
 { &MEDIATYPE_Text    , &MEDIASUBTYPE_NULL           },
 { &MEDIATYPE_Subtitle, &MEDIASUBTYPE_NULL           },
 { &MEDIATYPE_Video   , &MEDIASUBTYPE_DVD_SUBPICTURE }
};

const AMOVIESETUP_MEDIATYPE TffdshowEnc::inputMediaTypes[]=
{
 { &MEDIATYPE_Video, &MEDIASUBTYPE_YV12   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_YUY2   },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB32  },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB24  },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB555 },
 { &MEDIATYPE_Video, &MEDIASUBTYPE_RGB565 },
};

const AMOVIESETUP_MEDIATYPE TffdshowEnc::outputMediaTypes[]=
{
 { &MEDIATYPE_Video, &MEDIASUBTYPE_NULL },
 { &MEDIATYPE_Stream,&MEDIASUBTYPE_NULL }
};

AMOVIESETUP_PIN TffdshowDecVideo::pins[]=
{
 {
  L"Input",                                                  // String pin name
  FALSE,                                                     // Is it rendered
  FALSE,                                                     // Is it an output
  FALSE,                                                     // Allowed none
  FALSE,                                                     // Allowed many
  &CLSID_NULL,                                               // Connects to filter
  L"Output",                                                 // Connects to pin
  sizeof(inputMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE),   // Number of types
  inputMediaTypes                                            // The pin details
 },
 {
  L"Output",                                                 // String pin name
  FALSE,                                                     // Is it rendered
  TRUE,                                                      // Is it an output
  FALSE,                                                     // Allowed none
  FALSE,                                                     // Allowed many
  &CLSID_NULL,                                               // Connects to filter
  L"Input",                                                  // Connects to pin
  sizeof(outputMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE),  // Number of types
  outputMediaTypes                                           // The pin details
 },
 {
  L"In Text",                                                  // String pin name
  FALSE,                                                       // Is it rendered
  FALSE,                                                       // Is it an output
  FALSE,                                                       // Allowed none
  FALSE,                                                       // Allowed many
  &CLSID_NULL,                                                 // Connects to filter
  NULL,                                                        // Connects to pin
  sizeof(inputTextMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE), // Number of types
  inputTextMediaTypes                                          // The pin details
 }
};

AMOVIESETUP_PIN TffdshowDecVideoRaw::pins[]=
{
 {
  L"Input",                                                  // String pin name
  FALSE,                                                     // Is it rendered
  FALSE,                                                     // Is it an output
  FALSE,                                                     // Allowed none
  FALSE,                                                     // Allowed many
  &CLSID_NULL,                                               // Connects to filter
  L"Output",                                                 // Connects to pin
  sizeof(inputMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE),   // Number of types
  inputMediaTypes                                            // The pin details
 },
 {
  L"Output",                                                 // String pin name
  FALSE,                                                     // Is it rendered
  TRUE,                                                      // Is it an output
  FALSE,                                                     // Allowed none
  FALSE,                                                     // Allowed many
  &CLSID_NULL,                                               // Connects to filter
  L"Input",                                                  // Connects to pin
  sizeof(outputMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE),  // Number of types
  outputMediaTypes                                           // The pin details
 },
 {
  L"In Text",                                                  // String pin name
  FALSE,                                                       // Is it rendered
  FALSE,                                                       // Is it an output
  FALSE,                                                       // Allowed none
  FALSE,                                                       // Allowed many
  &CLSID_NULL,                                                 // Connects to filter
  NULL,                                                        // Connects to pin
  sizeof(inputTextMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE), // Number of types
  inputTextMediaTypes                                          // The pin details
 }
};

AMOVIESETUP_PIN TffdshowDecVideoSubtitles::pins[]=
{
 {
  L"Input",                                                  // String pin name
  FALSE,                                                     // Is it rendered
  FALSE,                                                     // Is it an output
  FALSE,                                                     // Allowed none
  FALSE,                                                     // Allowed many
  &CLSID_NULL,                                               // Connects to filter
  L"Output",                                                 // Connects to pin
  sizeof(inputMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE),   // Number of types
  inputMediaTypes                                            // The pin details
 },
 {
  L"Output",                                                 // String pin name
  FALSE,                                                     // Is it rendered
  TRUE,                                                      // Is it an output
  FALSE,                                                     // Allowed none
  FALSE,                                                     // Allowed many
  &CLSID_NULL,                                               // Connects to filter
  L"Input",                                                  // Connects to pin
  sizeof(outputMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE),  // Number of types
  outputMediaTypes                                           // The pin details
 },
 {
  L"In Text",                                                  // String pin name
  FALSE,                                                       // Is it rendered
  FALSE,                                                       // Is it an output
  FALSE,                                                       // Allowed none
  FALSE,                                                       // Allowed many
  &CLSID_NULL,                                                 // Connects to filter
  NULL,                                                        // Connects to pin
  sizeof(inputTextMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE), // Number of types
  inputTextMediaTypes                                          // The pin details
 }
};

const AMOVIESETUP_PIN TffdshowDecAudio::pins[]=
{
 {
  L"Output",                                                 // String pin name
  FALSE,                                                     // Is it rendered
  TRUE,                                                      // Is it an output
  FALSE,                                                     // Allowed none
  FALSE,                                                     // Allowed many
  &CLSID_NULL,                                               // Connects to filter
  NULL,                                                      // Connects to pin
  sizeof(outputMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE),  // Number of types
  outputMediaTypes                                           // The pin details
 },
 {
  L"Input",                                                // String pin name
  FALSE,                                                   // Is it rendered
  FALSE,                                                   // Is it an output
  FALSE,                                                   // Allowed none
  FALSE,                                                   // Allowed many
  &CLSID_NULL,                                             // Connects to filter
  NULL,                                                    // Connects to pin
  sizeof(inputMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE), // Number of types
  inputMediaTypes                                          // The pin details
 }
};

const AMOVIESETUP_PIN TffdshowDecAudioRaw::pins[]=
{
 {
  L"Output",                                                 // String pin name
  FALSE,                                                     // Is it rendered
  TRUE,                                                      // Is it an output
  FALSE,                                                     // Allowed none
  FALSE,                                                     // Allowed many
  &CLSID_NULL,                                               // Connects to filter
  NULL,                                                      // Connects to pin
  sizeof(outputMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE),  // Number of types
  outputMediaTypes                                           // The pin details
 },
 {
  L"Input",                                                // String pin name
  FALSE,                                                   // Is it rendered
  FALSE,                                                   // Is it an output
  FALSE,                                                   // Allowed none
  FALSE,                                                   // Allowed many
  &CLSID_NULL,                                             // Connects to filter
  NULL,                                                    // Connects to pin
  sizeof(inputMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE), // Number of types
  inputMediaTypes                                          // The pin details
 }
};

const AMOVIESETUP_PIN TffdshowEnc::pins[]=
{
 {
  L"Input",           // String pin name
  FALSE,              // Is it rendered
  FALSE,              // Is it an output
  FALSE,              // Allowed none
  FALSE,              // Allowed many
  &CLSID_NULL,        // Connects to filter
  NULL,               // Connects to pin
  sizeof(inputMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE), // Number of types
  inputMediaTypes     // The pin details
 },
 {
  L"Output",          // String pin name
  FALSE,              // Is it rendered
  TRUE,               // Is it an output
  FALSE,              // Allowed none
  FALSE,              // Allowed many
  &CLSID_NULL,        // Connects to filter
  NULL,               // Connects to pin
  sizeof(outputMediaTypes) / sizeof(AMOVIESETUP_MEDIATYPE),  // Number of types
  outputMediaTypes    // The pin details
 }
};

const DWORD TffdshowDecVideo::defaultMerit=0xff800001; // a hack to be always the first
const AMOVIESETUP_FILTER TffdshowDecVideo::filter=
{
 &CLSID_FFDSHOW,                       // Filter CLSID
 FFDSHOW_NAME_L,                       // Filter name
 defaultMerit,
 sizeof(pins)/sizeof(AMOVIESETUP_PIN), // Number of pins
 pins                                  // Pin details
};

const DWORD TffdshowDecVideoRaw::defaultMerit=MERIT_DO_NOT_USE;
const AMOVIESETUP_FILTER TffdshowDecVideoRaw::filter=
{
 &CLSID_FFDSHOWRAW,                    // Filter CLSID
 FFDSHOWRAW_NAME_L,                    // Filter name
 defaultMerit,
 sizeof(pins)/sizeof(AMOVIESETUP_PIN), // Number of pins
 pins                                  // Pin details
};

const DWORD TffdshowDecVideoSubtitles::defaultMerit=MERIT_DO_NOT_USE;
const AMOVIESETUP_FILTER TffdshowDecVideoSubtitles::filter=
{
 &CLSID_FFDSHOWSUBTITLES,              // Filter CLSID
 FFDSHOWSUBTITLES_NAME_L,              // Filter name
 defaultMerit,
 sizeof(pins)/sizeof(AMOVIESETUP_PIN), // Number of pins
 pins                                  // Pin details
};

const DWORD TffdshowDecAudio::defaultMerit=0x40000000-1; // One less than ac3filter
const AMOVIESETUP_FILTER TffdshowDecAudio::filter=
{
 &CLSID_FFDSHOWAUDIO,                  // Filter CLSID
 FFDSHOWAUDIO_NAME_L,                  // Filter name
 defaultMerit,
 sizeof(pins)/sizeof(AMOVIESETUP_PIN), // Number of pins
 pins                                  // Pin details
};

const DWORD TffdshowDecAudioRaw::defaultMerit=MERIT_DO_NOT_USE;
const AMOVIESETUP_FILTER TffdshowDecAudioRaw::filter=
{
 &CLSID_FFDSHOWAUDIORAW,               // Filter CLSID
 FFDSHOWAUDIORAW_NAME_L,               // Filter name
 defaultMerit,
 sizeof(pins)/sizeof(AMOVIESETUP_PIN), // Number of pins
 pins                                  // Pin details
};

const DWORD TffdshowEnc::defaultMerit=MERIT_SW_COMPRESSOR;

// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance
CFactoryTemplate g_Templates[] =
{
 {
  FFDSHOW_NAME_L,
  &CLSID_FFDSHOW,
  TffdshowDecVideo::CreateInstance,
  NULL,
  &TffdshowDecVideo::filter
 },
 {
  FFDSHOWRAW_NAME_L,
  &CLSID_FFDSHOWRAW,
  TffdshowDecVideoRaw::CreateInstance,
  NULL,
  &TffdshowDecVideoRaw::filter
 },
 {
  FFDSHOW_NAME_L L" conf",
  &CLSID_TFFDSHOWPAGE,
  TffdshowPageDecVideo::CreateInstance
 },
 {
  FFDSHOWRAW_NAME_L L" conf",
  &CLSID_TFFDSHOWPAGERAW,
  TffdshowPageDecVideoRaw::CreateInstance
 },
 {
  FFDSHOWRAW_NAME_L L" conf",
  &CLSID_TFFDSHOWPAGEVFW,
  TffdshowPageDecVideoVFW::CreateInstance
 },
 {
  FFDSHOW_NAME_L L" ffproc",
  &CLSID_TFFPROC,
  TffProcVideo::CreateInstance
 },
 {
  FFDSHOWRAW_NAME_L L" ffproc" L" conf",
  &CLSID_TFFDSHOWPAGEPROC,
  TffdshowPageDecVideoProc::CreateInstance
 },
 {
  FFDSHOWAUDIO_NAME_L,
  &CLSID_FFDSHOWAUDIO,
  TffdshowDecAudio::CreateInstance,
  NULL,
  &TffdshowDecAudio::filter
 },
 {
  FFDSHOWAUDIORAW_NAME_L,
  &CLSID_FFDSHOWAUDIORAW,
  TffdshowDecAudioRaw::CreateInstance,
  NULL,
  &TffdshowDecAudioRaw::filter
 },
 {
  FFDSHOWAUDIO_NAME_L L" conf",
  &CLSID_TFFDSHOWAUDIOPAGE,
  TffdshowPageDecAudio::CreateInstance
 },
 {
  FFDSHOWAUDIORAW_NAME_L L" conf",
  &CLSID_TFFDSHOWAUDIORAWPAGE,
  TffdshowPageDecAudioRaw::CreateInstance
 },
 {
  FFDSHOWAUDIO_NAME_L L" ffproc",
  &CLSID_TFFPROCAUDIO,
  TffProcAudio::CreateInstance
 },
 {
  FFDSHOWENC_DESC_L,
  &CLSID_FFDSHOWENC,
  TffdshowEncDshow::CreateInstance,
  //NULL,
  //&TffdshowEnc::filter
 },
 {
  FFDSHOWENC_DESC_L L" vfw",
  &CLSID_FFDSHOWENCVFW,
  TffdshowEncVFW::CreateInstance,
  //NULL,
  //&TffdshowEnc::filter
 },
 {
  FFDSHOWENC_DESC_L L" conf",
  &CLSID_TFFDSHOWENCPAGE,
  TffdshowPageEnc::CreateInstance
 },
 {
  FFDSHOWENC_DESC_L L" conf",
  &CLSID_TFFDSHOWENCPAGEVFW,
  TffdshowPageEncVFW::CreateInstance
 },

 {
  FFDSHOWVFW_DESC_L,
  &CLSID_FFVFW,
  Tffvfw::CreateInstance
 },
 {
  FFDSHOWACM_DESC_L L" creator",
  &CLSID_FFACM2CREATOR,
  TacmCreator::CreateInstance
 },

 {
  FFCOLORSPACECONVERT_NAME_L,
  &CLSID_FFCOLORSPACECONVERT,
  TffColorspaceConvert::CreateInstance
 },

 // This is actually not an OLE object, just a DwString initialization stub to prevent a sEmptyRep memory leak
 {
  L"DwString::GlobalInitialization",
  &CLSID_DWSTRINGA,
  DwString<char>::CreateInstance,
  DwString<char>::GlobalInitialization,
  NULL
 },
 {
  L"DwString::GlobalInitializationW",
  &CLSID_DWSTRINGW,
  DwString<wchar_t>::CreateInstance,
  DwString<wchar_t>::GlobalInitialization,
  NULL
 },
 {
  FFDSHOWSUBTITLES_NAME_L,
  &CLSID_FFDSHOWSUBTITLES,
  TffdshowDecVideoSubtitles::CreateInstance,
  NULL,
  &TffdshowDecVideoSubtitles::filter
 },
 {
  FFDSHOWRAW_NAME_L L" conf",
  &CLSID_TFFDSHOWPAGESUBTITLES,
  TffdshowPageDecVideoSubtitles::CreateInstance
 },
};

int g_cTemplates=sizeof(g_Templates)/sizeof(CFactoryTemplate);

static void remove00000000(void)
{
 HKEY hKey=NULL;
 RegOpenKeyEx(HKEY_CLASSES_ROOT,_l("\\CLSID\\{00000000-0000-0000-0000-000000000000}\\InprocServer32"),0,KEY_READ,&hKey);
 if (hKey)
  {
   char_t dll[MAX_PATH];
   DWORD size=MAX_PATH;
   LONG res=RegQueryValueEx(hKey,_l(""),0,0,(unsigned char*)dll,&size);
   RegCloseKey(hKey);
   if (res==ERROR_SUCCESS && stristr(dll,_l("ffdshow.ax")))
    SHDeleteKey(HKEY_CLASSES_ROOT,_l("\\CLSID\\{00000000-0000-0000-0000-000000000000}"));
  }
}

STDAPI DllRegisterServer()
{
 remove00000000();
 DWORD merit_ffdshowDecVideo=TffdshowDecVideo::defaultMerit;getFilterMerit(CLSID_FFDSHOW,&merit_ffdshowDecVideo);
 DWORD merit_ffdshowDecAudio=TffdshowDecAudio::defaultMerit;getFilterMerit(CLSID_FFDSHOWAUDIO,&merit_ffdshowDecAudio);
 HRESULT hr=AMovieDllRegisterServer2(TRUE);
 if (FAILED(hr)) return hr;

 setFilterMerit(CLSID_FFDSHOW,merit_ffdshowDecVideo);
 setFilterMerit(CLSID_FFDSHOWAUDIO,merit_ffdshowDecAudio);

 //encoder
 IFilterMapper2 *pFM2=NULL;
 if (SUCCEEDED(CoCreateInstance(CLSID_FilterMapper2,NULL,CLSCTX_INPROC_SERVER,IID_IFilterMapper2,(void **)&pFM2)))
  {
   REGFILTER2 rf2FilterReg;
   rf2FilterReg.dwVersion=1;
   rf2FilterReg.dwMerit=TffdshowEnc::defaultMerit;
   rf2FilterReg.cPins=2;
   rf2FilterReg.rgPins=TffdshowEnc::pins;

   hr=pFM2->RegisterFilter(CLSID_FFDSHOWENC,
                           FFDSHOWENC_DESC_L,
                           NULL,
                           &CLSID_VideoCompressorCategory,
                           FFDSHOWENC_DESC_L,
                           &rf2FilterReg
                          );
   pFM2->Release();
  }
 return hr;
}

STDAPI DllUnregisterServer()
{
 HRESULT hr=AMovieDllRegisterServer2(FALSE);
 if (FAILED(hr))return hr;
 IFilterMapper2 *pFM2=NULL;
 hr=CoCreateInstance(CLSID_FilterMapper2,NULL,CLSCTX_INPROC_SERVER,IID_IFilterMapper2,(void **)&pFM2);
 if (FAILED(hr)) return hr;
 hr=pFM2->UnregisterFilter(&CLSID_VideoCompressorCategory,FFDSHOWENC_DESC_L,CLSID_FFDSHOWENC);
 pFM2->Release();
 return hr;
}
