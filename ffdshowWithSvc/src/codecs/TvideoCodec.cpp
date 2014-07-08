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
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "IffdshowEnc.h"
#include "TvideoCodec.h"
#include "TvideoCodecLibavcodec.h"
#include "TvideoCodecUncompressed.h"
#include "TvideoCodecXviD4.h"
#include "TvideoCodecLibmpeg2.h"
#include "TvideoCodecTheora.h"
#include "TvideoCodecWmv9.h"
#include "TvideoCodecAvisynth.h"
#include "TvideoCodecSkal.h"
#include "TvideoCodecX264.h"
#include "dsutil.h"

//======================================= TvideoCodec =======================================
TvideoCodec::TvideoCodec(IffdshowBase *Ideci):Tcodec(Ideci)
{
 ok=false;
}
TvideoCodec::~TvideoCodec()
{
}

//===================================== TvideoCodecDec ======================================
TvideoCodecDec* TvideoCodecDec::initDec(IffdshowBase *deci,IdecVideoSink *sink,CodecID codecId,FOURCC fcc,const CMediaType &mt)
{
 TvideoCodecDec *movie=NULL;

 if      (lavc_codec    (codecId)) movie=new TvideoCodecLibavcodec(deci,sink, 0);
 else if (raw_codec     (codecId)) movie=new TvideoCodecUncompressed(deci,sink);
 else if (wmv9_codec    (codecId)) movie=new TvideoCodecWmv9(deci,sink);
 else if (codecId==CODEC_ID_SVC) {
	 codecId = CODEC_ID_H264;
	 movie=new TvideoCodecLibavcodec(deci, sink, 1);
 }
 else if (codecId==CODEC_ID_XVID4     ) movie=new TvideoCodecXviD4(deci,sink);
 else if (codecId==CODEC_ID_LIBMPEG2  ) movie=new TvideoCodecLibmpeg2(deci,sink);
 else if (codecId==CODEC_ID_AVISYNTH  ) movie=new TvideoCodecAvisynth(deci,sink);
 else if (codecId==CODEC_ID_THEORA_LIB) movie=new TvideoCodecTheora(deci,sink);
 else return NULL;
 if (!movie) return NULL;
 if (movie->ok && movie->testMediaType(fcc,mt))
  {
   movie->codecId=codecId;
   return movie;
  }
 else
  {
   delete movie;
   return NULL;
  }
}

TvideoCodecDec::TvideoCodecDec(IffdshowBase *Ideci,IdecVideoSink *Isink):
 Tcodec(Ideci),TcodecDec(Ideci,Isink),TvideoCodec(Ideci),
 sinkD(Isink),
 deciV(Ideci),
 quantsDx(0),quantsDy(0),quants(NULL),quantType(FF_QSCALE_TYPE_MPEG1),
 inter_matrix(NULL),intra_matrix(NULL)
{
}
TvideoCodecDec::~TvideoCodecDec()
{
}

float TvideoCodecDec::calcMeanQuant(void)
{
 if (!quants || !quantsDx || !quantsDy) return 0;
 unsigned int sum=0,num=quantsDx*quantsDy;
 unsigned char *quants1=(unsigned char*)quants;
 for (unsigned int y=0;y<quantsDy;y++)
  for (unsigned int x=0;x<quantsDx;x++)
   sum+=quants1[(y*quantsStride+x)*quantBytes];
 return float(sum)/num;
}

//===================================== TvideoCodecEnc ======================================
TvideoCodecEnc::TvideoCodecEnc(IffdshowBase *Ideci,IencVideoSink *Isink):
 Tcodec(Ideci),TvideoCodec(deci),
 deciE(Ideci),
 sinkE(Isink)
{
 if (deciE)
  deciE->getCoSettingsPtr(&coCfg);
}
TvideoCodecEnc::~TvideoCodecEnc()
{
}

bool TvideoCodecEnc::getExtradata(const void* *ptr,size_t *len)
{
 if (len) *len=0;else return false;
 if (ptr) *ptr=NULL;else return false;
 return true;
}

//======================================== TencLibs =========================================
void TvideoCodecs::init(IffdshowBase *deci,IencVideoSink *sink)
{
 push_back(new TvideoCodecLibavcodec(deci,sink));
 push_back(new TvideoCodecXviD4(deci,sink));
 push_back(new TvideoCodecSkal(deci,sink));
 push_back(new TvideoCodecX264(deci,sink));
 push_back(new TvideoCodecTheora(deci,sink));
 push_back(new TvideoCodecWmv9(deci,sink));
 push_back(new TvideoCodecUncompressed(deci,sink));
}
TvideoCodecEnc* TvideoCodecs::getEncLib(int codecId)
{
 for (const_iterator l=begin();l!=end();l++)
  if (*l)
   for (Tencoders::const_iterator e=(*l)->encoders.begin();e!=(*l)->encoders.end();e++)
    if ((*e)->id==codecId)
     return *l;
 return NULL;
}
const Tencoder* TvideoCodecs::getEncoder(int codecId) const
{
 for (const_iterator l=begin();l!=end();l++)
  if (*l)
   for (Tencoders::const_iterator e=(*l)->encoders.begin();e!=(*l)->encoders.end();e++)
    if ((*e)->id==codecId)
     return *e;
 return NULL;
}
