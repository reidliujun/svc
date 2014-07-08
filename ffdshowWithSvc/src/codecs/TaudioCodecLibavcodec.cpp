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
#include "TaudioCodecLibavcodec.h"
#include "Tlibmplayer.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"
#include "dsutil.h"
#include "ffdshow_mediaguids.h"
#include "xiph/vorbis/vorbisformat.h"
#include "ffdebug.h"


TaudioCodecLibavcodec::TaudioCodecLibavcodec(IffdshowBase *deci,IdecAudioSink *Isink):
 Tcodec(deci),
 TaudioCodec(deci,Isink)
{
 avctx=NULL;avcodec=NULL;codecinited=false;contextinited=false;parser=NULL;src_ch_layout = AF_CHANNEL_LAYOUT_MPLAYER_DEFAULT;
}

bool TaudioCodecLibavcodec::init(const CMediaType &mt)
{
 deci->getLibavcodec(&libavcodec);
 if (libavcodec->ok)
  {
   avcodec=libavcodec->avcodec_find_decoder(codecId);
   if (!avcodec) return false;
   if (codecId==CODEC_ID_AMR_NB) //HACK: 3ivx splitter doesn't report correct frequency/number of channels
    {
     fmt.setChannels(1);
     fmt.freq=8000;
    }
   avctx=libavcodec->avcodec_alloc_context();
   avctx->sample_rate=fmt.freq;
   avctx->channels=fmt.nchannels;
   avctx->codec_id=codecId;

   if (parser)
   {
	   libavcodec->av_parser_close(parser);
	   parser = NULL;
   }
   parser=libavcodec->av_parser_init(codecId);


   if (mt.formattype==FORMAT_WaveFormatEx)
    {
     const WAVEFORMATEX *wfex=(const WAVEFORMATEX*)mt.pbFormat;
     avctx->bit_rate=wfex->nAvgBytesPerSec*8;
     avctx->bits_per_coded_sample=wfex->wBitsPerSample;
     avctx->block_align=wfex->nBlockAlign;
    }
   else
    {
     avctx->bit_rate=fmt.avgBytesPerSec()*8;
     avctx->bits_per_coded_sample=fmt.bitsPerSample();
     avctx->block_align=fmt.blockAlign();
    }

   bpssum=lastbps=avctx->bit_rate/1000;

   if (codecId==CODEC_ID_WMAV1 || codecId==CODEC_ID_WMAV2)
    {
     //bpssum=lastbps=avctx->bit_rate/1000;
     numframes=1;
    }
   Textradata extradata(mt,FF_INPUT_BUFFER_PADDING_SIZE);
   if (codecId==CODEC_ID_FLAC && extradata.size>=4 && *(FOURCC*)extradata.data==mmioFOURCC('f','L','a','C')) // HACK
    {
     avctx->extradata=(uint8_t*)extradata.data+8;
     avctx->extradata_size=34;
    }
   else if (codecId==CODEC_ID_COOK && mt.formattype==FORMAT_WaveFormatEx && mt.pbFormat)
    {
     avctx->extradata=mt.pbFormat+sizeof(WAVEFORMATEX);
     avctx->extradata_size=mt.cbFormat-sizeof(WAVEFORMATEX);
     for (;avctx->extradata_size;avctx->extradata=(uint8_t*)avctx->extradata+1,avctx->extradata_size--)
      if (memcmp(avctx->extradata,"cook",4)==0)
       {
        avctx->extradata=(uint8_t*)avctx->extradata+12;
        avctx->extradata_size-=12;
        break;
       }
    }
   else
    {
     avctx->extradata=(uint8_t*)extradata.data;
     avctx->extradata_size=(int)extradata.size;
    }
   if (codecId==CODEC_ID_VORBIS && mt.formattype==FORMAT_VorbisFormat2)
    {
     const VORBISFORMAT2 *vf2=(const VORBISFORMAT2*)mt.pbFormat;
     avctx->vorbis_header_size[0]=vf2->HeaderSize[0];
     avctx->vorbis_header_size[1]=vf2->HeaderSize[1];
     avctx->vorbis_header_size[2]=vf2->HeaderSize[2];
    }
   if (libavcodec->avcodec_open(avctx,avcodec)<0) return false;
   codecinited=true;
   switch (avctx->sample_fmt)
    {
     case SAMPLE_FMT_S16:fmt.sf=TsampleFormat::SF_PCM16;break;
     case SAMPLE_FMT_S32:fmt.sf=TsampleFormat::SF_PCM32;break;
     case SAMPLE_FMT_FLT:fmt.sf=TsampleFormat::SF_FLOAT32;break;
     case SAMPLE_FMT_DBL:fmt.sf=TsampleFormat::SF_FLOAT64;break;
    }
   isGain=deci->getParam2(IDFF_vorbisgain);
   updateChannelMapping();

   libavcodec->av_log_set_level(AV_LOG_QUIET);

   // Handle truncated streams
   if(avctx->codec->capabilities & CODEC_CAP_TRUNCATED)
        avctx->flags|=CODEC_FLAG_TRUNCATED;

   return true;
  }
 else
  return false;
}
TaudioCodecLibavcodec::~TaudioCodecLibavcodec()
{ 
 if (parser)
 {
   libavcodec->av_parser_close(parser);
   parser = NULL;
 }
 if (avctx)
  {
   if (codecinited) libavcodec->avcodec_close(avctx);codecinited=false;
   libavcodec->av_free(avctx);
  }
 if (libavcodec) libavcodec->Release();
}
const char_t* TaudioCodecLibavcodec::getName(void) const
{
 return _l("libavcodec");
}
void TaudioCodecLibavcodec::getInputDescr1(char_t *buf,size_t buflen) const
{
 
if (avcodec)
{
	// Show dca as dts
	if (!strcmp(text<char_t>(avcodec->name), _l("dca")))
		ff_strncpy(buf,_l("dts"),buflen);
	else
		ff_strncpy(buf,(const char_t *)text<char_t>(avcodec->name),buflen);
}
 buf[buflen-1]='\0';
}

HRESULT TaudioCodecLibavcodec::decode(TbyteBuffer &src0)
{
 // Dynamic range compression for AC3/DTS formats
 if (codecId == CODEC_ID_AC3 || codecId == CODEC_ID_EAC3 || codecId == CODEC_ID_DTS)
  {
   if (deci->getParam2(IDFF_audio_decoder_DRC))
    {
     float drcLevel=(float)deci->getParam2(IDFF_audio_decoder_DRC_Level) / 100;
     avctx->drc_scale=drcLevel;
    }
   else
    {
     avctx->drc_scale=0.0;
    }
  }

 int size=(int)src0.size();
 unsigned char *src=&*src0.begin();
 int maxLength=AVCODEC_MAX_AUDIO_FRAME_SIZE;
 TbyteBuffer newsrcBuffer;

 while (size > 0)
  {
   if (codecId == CODEC_ID_MLP && size == 1)
       break;  // workaround when skipping TrueHD in MPC?
   int dstLength=AVCODEC_MAX_AUDIO_FRAME_SIZE;
   void *dst=(void*)getDst(dstLength);
   int dstLength2=AVCODEC_MAX_AUDIO_FRAME_SIZE;
   void *dst2=buf2.alloc(dstLength);
   
   int ret=0,ret2=0;
   // Use parser if available and do not use it for MLP/TrueHD stream
   if (parser && codecId != CODEC_ID_MLP)
   {
	   // Parse the input buffer src(size) and returned parsed data into dst2(dstLength2)
	   ret=libavcodec->av_parser_parse(parser, avctx, (uint8_t**)&dst2, &dstLength2,
		   (const uint8_t*)src, size, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
	   // dstLength2==0 : nothing parsed
	   if (ret<0 || (ret==0 && dstLength2==0))
		   break;
	   size-=ret;
	   src+=ret;


	   if (dstLength2 > 0) // This block could be parsed
	   {
          // Decode the parsed buffer dst2(dstLength2) into dst(dstLength)
		  ret2=libavcodec->avcodec_decode_audio2(avctx,(int16_t*)dst,&dstLength,(const uint8_t*)dst2,dstLength2);
		  // If nothing could be decoded, skip this data and continue
		  if (ret2<0 || (ret2==0 &&dstLength==0))
			continue;
	   }
	   else // The buffer could not be parsed
			continue;
   }
   else
   {
	   ret=libavcodec->avcodec_decode_audio2(avctx,(int16_t*)dst,&dstLength,src,size);
	   if (ret<0 || (ret==0 && dstLength==0))
          break;
	   size-=ret;
	   src+=ret;
   }

   bpssum=lastbps=avctx->bit_rate/1000;

   // Correct the input media type from what has been parsed
   if (dstLength > 0)
   {
	   fmt.setChannels(avctx->channels);
	   fmt.freq=avctx->sample_rate;
	   switch (avctx->sample_fmt)
		{
		 case SAMPLE_FMT_S16:fmt.sf=TsampleFormat::SF_PCM16;break;
		 case SAMPLE_FMT_S32:fmt.sf=TsampleFormat::SF_PCM32;break;
		 case SAMPLE_FMT_FLT:fmt.sf=TsampleFormat::SF_FLOAT32;break;
		 case SAMPLE_FMT_DBL:fmt.sf=TsampleFormat::SF_FLOAT64;break;
		}
   }

  // Correct channel mapping
  if (dstLength > 0 && fmt.nchannels >= 5)
  {
	 Tlibmplayer *libmplayer = NULL;
	 this->deci->getPostproc(&libmplayer);
	 if (libmplayer != NULL)
	 {
	 	 libmplayer->reorder_channel_nch(dst, 
			 src_ch_layout,AF_CHANNEL_LAYOUT_FFDSHOW_DEFAULT,
			 fmt.nchannels,
			 dstLength / 2, 2);
	 }
  }

  if (dstLength > 0)
  {
	  HRESULT hr=sinkA->deliverDecodedSample(dst, dstLength/fmt.blockAlign(),fmt,isGain?avctx->postgain:1);
	  if (hr!=S_OK) 
		 return hr;
  }
 }
 src0.clear();
 return S_OK;
}


void TaudioCodecLibavcodec::updateChannelMapping()
{
	src_ch_layout = AF_CHANNEL_LAYOUT_FFDSHOW_DEFAULT;
	if (!avctx->codec->name) return;
	char_t codec[255];
	ff_strncpy(codec, (const char_t *)text<char_t>(avctx->codec->name), countof(codec));
	codec[254]='\0';

	if (!stricmp(codec, _l("ac3")) || !stricmp(codec, _l("eac3")))
	  src_ch_layout = AF_CHANNEL_LAYOUT_LAVC_AC3_DEFAULT;
	else if (!stricmp(codec, _l("dca")))
	  src_ch_layout = AF_CHANNEL_LAYOUT_LAVC_DCA_DEFAULT;
	else if (!stricmp(codec, _l("libfaad"))
		|| !stricmp(codec, _l("mpeg4aac")))
	  src_ch_layout = AF_CHANNEL_LAYOUT_AAC_DEFAULT;
	else if (!stricmp(codec, _l("liba52")))
	  src_ch_layout = AF_CHANNEL_LAYOUT_LAVC_LIBA52_DEFAULT;
	else if (!stricmp(codec, _l("vorbis")))
	  src_ch_layout = AF_CHANNEL_LAYOUT_VORBIS_DEFAULT;
	else if (!stricmp(codec, _l("mlp")))
      src_ch_layout = AF_CHANNEL_LAYOUT_MLP_DEFAULT;
	else
	  src_ch_layout = AF_CHANNEL_LAYOUT_FFDSHOW_DEFAULT;
}

bool TaudioCodecLibavcodec::onSeek(REFERENCE_TIME segmentStart)
{
 return avctx?(libavcodec->avcodec_flush_buffers(avctx),true):false;
}
