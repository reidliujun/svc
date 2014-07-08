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
#include "TaudioCodec.h"
#include "IffdshowBase.h"
#include "IffdshowDecAudio.h"
#include "TaudioCodecLibavcodec.h"
#include "TaudioCodecMplayer.h"
#include "TaudioCodecLibMAD.h"
#include "TaudioCodecLibFAAD.h"
#include "TaudioCodecUncompressed.h"
#include "TaudioCodecAvisynth.h"
#include "TaudioCodecLiba52.h"
#include "TaudioCodecLibDTS.h"
#include "TaudioCodecTremor.h"
#include "TaudioCodecRealaac.h"
#include "TaudioCodecAudX.h"

TaudioCodec* TaudioCodec::initSource(IffdshowBase *Ideci,IdecAudioSink *Isink,CodecID codecId,const TsampleFormat &fmt,const CMediaType &mt)
{
 TaudioCodec *movie=getDecLib(codecId,Ideci,Isink);
 if (movie)
  {
   movie->fmt=fmt;
   if (movie->init(mt))
    return movie;
   else
    delete movie;
  }
 return NULL;
}

TaudioCodec::TaudioCodec(IffdshowBase *Ideci,IdecAudioSink *Isink):
 Tcodec(Ideci),
 TcodecDec(Ideci,Isink),
 deciA(Ideci),
 sinkA(Isink),
 bpssum(0),numframes(0),lastbps(0)
{
}

TaudioCodec* TaudioCodec::getDecLib(CodecID codecId,IffdshowBase *deci,IdecAudioSink *sink)
{
 TaudioCodec *movie;
 if      (lavc_codec   (codecId)) movie=new TaudioCodecLibavcodec(deci,sink);
 else if (mplayer_codec(codecId)) movie=new TaudioCodecMplayer(deci,sink);
 else if (raw_codec    (codecId)) movie=new TaudioCodecUncompressed(deci,sink);
 else if (codecId==CODEC_ID_LIBMAD)   movie=new TaudioCodecLibMAD(deci,sink);
 else if (codecId==CODEC_ID_LIBFAAD)  movie=new TaudioCodecLibFAAD(deci,sink);
 else if (codecId==CODEC_ID_AVISYNTH) movie=new TaudioCodecAvisynth(deci,sink);
 else if (codecId==CODEC_ID_TREMOR)   movie=new TaudioCodecTremor(deci,sink);
 else if (codecId==CODEC_ID_REALAAC)  movie=new TaudioCodecRealaac(deci,sink);
 else if (codecId==CODEC_ID_AUDX)     movie=new TaudioCodecAudX(deci,sink);
 else if (codecId==CODEC_ID_LIBA52 || codecId==CODEC_ID_SPDIF_AC3)  movie=new TaudioCodecLiba52(deci,sink);
 else if (codecId==CODEC_ID_LIBDTS || codecId==CODEC_ID_SPDIF_DTS)  movie=new TaudioCodecLibDTS(deci,sink);
 else return NULL;
 movie->codecId=codecId;
 return movie;
}

void* TaudioCodec::getDst(size_t needed)
{
 return buf.alloc(needed);
}

void TaudioCodec::getInputDescr(char_t *buf,size_t buflen) const
{
 char_t samplefmt[255];fmt.description(samplefmt,255);
 char_t input[256];getInputDescr1(input,256);
 if (numframes && bpssum)
  tsnprintf_s(buf, buflen, _TRUNCATE, _l("%s %u kbps %s (%s)"), samplefmt, roundDiv(bpssum,numframes), input, getName());
 else
  tsnprintf_s(buf, buflen, _TRUNCATE, _l("%s %s (%s)"), samplefmt, input, getName());
 buf[buflen-1]='\0';
}
