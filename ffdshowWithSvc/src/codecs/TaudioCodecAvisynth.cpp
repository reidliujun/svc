/*
 * Copyright (c) 2004-2006 Milan Cutka
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
#include "TaudioCodecAvisynth.h"
#include "Tconfig.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"
#include "avisynth/Tavisynth.h"

TaudioCodecAvisynth::TaudioCodecAvisynth(IffdshowBase *deci,IdecAudioSink *Isink):
 Tcodec(deci),
 TaudioCodec(deci,Isink),
 avisynth(NULL)
{
}
bool TaudioCodecAvisynth::init(const CMediaType &mt)
{
 avisynth=new TavisynthAudio(mt,fmt,deci,"ffdshow_acm_avisynth_script");
 return avisynth->ok;
}
TaudioCodecAvisynth::~TaudioCodecAvisynth()
{
 if (avisynth) delete avisynth;
}

void TaudioCodecAvisynth::getInputDescr1(char_t *buf,size_t buflen) const
{
 ff_strncpy(buf,_l("avisynth"),buflen);
 buf[buflen-1]='\0';
}
HRESULT TaudioCodecAvisynth::decode(TbyteBuffer &src)
{
 uint64_t s1=*(uint64_t*)&*src.begin(),s2=*(uint64_t*)(&*src.begin()+8);
 int dstLength=int((s2-s1+1)*avisynth->vi->BytesPerAudioSample());
 int16_t *dst=(int16_t*)getDst(dstLength);
 (*avisynth->clip)->GetAudio(dst,s1,s2-s1+1);
 src.clear();
 return sinkA->deliverDecodedSample(dst,(unsigned int)(s2-s1+1),fmt,1);
}
