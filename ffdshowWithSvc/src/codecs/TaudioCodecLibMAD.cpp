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
#include "TaudioCodecLibMAD.h"
#include "Tdll.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"
#include "mp3header.h"

const char_t* TaudioCodecLibMAD::dllname=_l("ff_libmad.dll");

TaudioCodecLibMAD::TaudioCodecLibMAD(IffdshowBase *deci,IdecAudioSink *Isink):
 Tcodec(deci),
 TaudioCodec(deci,Isink)
{
 dll=NULL;
 inited=false;
}

bool TaudioCodecLibMAD::init(const CMediaType &mt)
{
 dll=new Tdll(dllname,config);
 dll->loadFunction(mad_synth_init,"mad_synth_init");
 dll->loadFunction(mad_stream_init,"mad_stream_init");
 dll->loadFunction(mad_frame_init,"mad_frame_init");
 dll->loadFunction(mad_stream_buffer,"mad_stream_buffer");
 dll->loadFunction(mad_frame_decode,"mad_frame_decode");
 dll->loadFunction(mad_synth_frame,"mad_synth_frame");
 dll->loadFunction(mad_frame_finish,"mad_frame_finish");
 dll->loadFunction(mad_stream_finish,"mad_stream_finish");
 if (dll->ok)
  {
   mad_synth_init (&synth);
   mad_stream_init(&stream);
   mad_frame_init (&frame);
   inited=true;
   fmt.sf=TsampleFormat::SF_PCM32;
   fmt.freq=mp3header::findNearestFreq(fmt.freq);
   return true;
  }
 else
  return false;
}
TaudioCodecLibMAD::~TaudioCodecLibMAD()
{
 if (inited)
  {
   mad_frame_finish (&frame);
   mad_stream_finish(&stream);
  }
 if (dll) delete dll;
}

void TaudioCodecLibMAD::getInputDescr1(char_t *buf,size_t buflen) const
{
 if (frame.header.layer)
  tsnprintf_s(buf, buflen, _TRUNCATE, _l("mp%i"), frame.header.layer);
 else
  tsnprintf_s(buf, buflen, _TRUNCATE, _l("mp1/2/3"));
}

int32_t TaudioCodecLibMAD::scale(mad_fixed_t sample)
{
 // clip
 if (sample >= MAD_F_ONE)
  sample = MAD_F_ONE - 1;
 else if (sample < -MAD_F_ONE)
  sample = -MAD_F_ONE;
 return int32_t(sample<<(31-MAD_F_FRACBITS));
}

HRESULT TaudioCodecLibMAD::decode(TbyteBuffer &src)
{
 mad_stream_buffer(&stream,&*src.begin(),(unsigned long)src.size());
 while (1)
  {
   if (mad_frame_decode(&frame,&stream)==-1)
    {
     if(stream.error==MAD_ERROR_BUFLEN)
      {
       src.clear();
       src.append(stream.this_frame,stream.bufend-stream.this_frame);
       //memmove(m_buff.GetData(), m_stream.this_frame, m_stream.bufend - m_stream.this_frame);
       //m_buff.SetSize(m_stream.bufend - m_stream.this_frame);
       break;
      }
     if (!MAD_RECOVERABLE(stream.error))
      return E_FAIL;
     continue;
    }
   bpssum+=(lastbps=frame.header.bitrate/1000);numframes++;
   mad_synth_frame(&synth,&frame);

   if (fmt.nchannels!=synth.pcm.channels || fmt.freq!=synth.pcm.samplerate)
    continue;

   const mad_fixed_t *left_ch =synth.pcm.samples[0];
   const mad_fixed_t *right_ch=synth.pcm.samples[1];
   int decoded=synth.pcm.length;
   int32_t *output,*output0;output=output0=(int32_t*)getDst(decoded*fmt.blockAlign());
   if (synth.pcm.channels==2)
    for (int i=0;i<decoded;i++)
     {
      *output++=scale(*left_ch++);
      *output++=scale(*right_ch++);
     }
   else
    for (int i=0;i<decoded;i++)
     *output++=scale(*left_ch++);
   HRESULT hr=sinkA->deliverDecodedSample(output0,decoded,fmt,1);
   if (hr!=S_OK)
    return hr;
  }
 return S_OK;
}
bool TaudioCodecLibMAD::onSeek(REFERENCE_TIME segmentStart)
{
 //mad_synth_init(&synth);
 //mad_stream_init(&stream);
 //mad_frame_init(&frame);
 return false;
}
