/*
 * Copyright (c) 2004-2006 Milan Cutka
 * based on MpaDecFilter by Gabest
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
#include "TaudioCodecLiba52.h"
#include "Tdll.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"

const char_t* TaudioCodecLiba52::dllname=_l("ff_liba52.dll");

// dshow: left, right, center, LFE, left surround, right surround
// ac3: LFE, left, center, right, left surround, right surround
const TaudioCodecLiba52::Tscmap TaudioCodecLiba52::scmaps[2*11]=
{
 {2, {0, 1,-1,-1,-1,-1}, 0}, // A52_CHANNEL
 {1, {0,-1,-1,-1,-1,-1}, 0}, // A52_MONO
 {2, {0, 1,-1,-1,-1,-1}, 0}, // A52_STEREO
 {3, {0, 2, 1,-1,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER}, // A52_3F
 {3, {0, 1, 2,-1,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_BACK_CENTER},  // A52_2F1R
 {4, {0, 2, 1, 3,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_BACK_CENTER}, // A52_3F1R
 {4, {0, 1, 2, 3,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT},     // A52_2F2R
 {5, {0, 2, 1, 3, 4,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT}, // A52_3F2R
 {1, {0,-1,-1,-1,-1,-1}, 0}, // A52_CHANNEL1
 {1, {0,-1,-1,-1,-1,-1}, 0}, // A52_CHANNEL2
 {2, {0, 1,-1,-1,-1,-1}, 0}, // A52_DOLBY

 {3, {1, 2, 0,-1,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_LOW_FREQUENCY}, // A52_CHANNEL|A52_LFE
 {2, {1, 0,-1,-1,-1,-1}, SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY},                   // A52_MONO|A52_LFE
 {3, {1, 2, 0,-1,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_LOW_FREQUENCY}, // A52_STEREO|A52_LFE
 {4, {1, 3, 2, 0,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY}, // A52_3F|A52_LFE
 {4, {1, 2, 0, 3,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_CENTER},  // A52_2F1R|A52_LFE
 {5, {1, 3, 2, 0, 4,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_CENTER}, // A52_3F1R|A52_LFE
 {5, {1, 2, 0, 3, 4,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT},     // A52_2F2R|A52_LFE
 {6, {1, 3, 2, 0, 4, 5}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT}, // A52_3F2R|A52_LFE
 {2, {1, 0,-1,-1,-1,-1}, SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY}, // A52_CHANNEL1|A52_LFE
 {2, {1, 0,-1,-1,-1,-1}, SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY}, // A52_CHANNEL2|A52_LFE
 {3, {1, 2, 0,-1,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_LOW_FREQUENCY}, // A52_DOLBY|A52_LFE
};


TaudioCodecLiba52::TaudioCodecLiba52(IffdshowBase *deci,IdecAudioSink *Isink):
 Tcodec(deci),
 TaudioCodec(deci,Isink)
{
 dll=NULL;state=NULL;
 inited=false;
}

bool TaudioCodecLiba52::init(const CMediaType &mt)
{
 dll=new Tdll(dllname,config);
 dll->loadFunction(a52_init,"a52_init");
 dll->loadFunction(a52_samples,"a52_samples");
 dll->loadFunction(a52_syncinfo,"a52_syncinfo");
 dll->loadFunction(a52_dynrng,"a52_dynrng");
 dll->loadFunction(a52_dynrngsetlevel,"a52_dynrngsetlevel");
 dll->loadFunction(a52_frame,"a52_frame");
 dll->loadFunction(a52_block,"a52_block");
 dll->loadFunction(a52_free,"a52_free");

 if (dll->ok)
  {
   state=a52_init(Tconfig::cpu_flags);
   fmt.sf=TsampleFormat::SF_FLOAT32;
   inited=true;
   return true;
  }
 else
  return false;
}
TaudioCodecLiba52::~TaudioCodecLiba52()
{
 if (dll)
  {
   if (state) a52_free(state);
   delete dll;
  }
}

void TaudioCodecLiba52::getInputDescr1(char_t *buf,size_t buflen) const
{
 ff_strncpy(buf,_l("ac3"),buflen);
 buf[buflen-1]='\0';
}

HRESULT TaudioCodecLiba52::decode(TbyteBuffer &src)
{
 unsigned char *p=&*src.begin();
 unsigned char *base=p;
 unsigned char *end=p+src.size();

 while (end-p>7)
  {
   int size=0,flags,sample_rate,bit_rate;
   if ((size=a52_syncinfo(p,&flags,&sample_rate,&bit_rate))>0)
    {
     bool enoughData=p+size<=end;
     if (enoughData)
      {
       if (codecId==CODEC_ID_SPDIF_AC3)
        {
         bpssum+=(lastbps=bit_rate/1000);numframes++;
		 HRESULT hr=deciA->deliverSampleSPDIF(p,size,bit_rate,sample_rate,0x0001,true);
         if (hr!=S_OK)
          return hr;
        }
       else
        {
         flags|=A52_ADJUST_LEVEL;
         liba52::sample_t level=1,bias=0;
         if (a52_frame(state,p,&flags,&level,bias)==0)
          {
           bpssum+=(lastbps=bit_rate/1000);numframes++;
           // Dynamic range compression
           if (deci->getParam2(IDFF_audio_decoder_DRC))
		   {
            liba52::sample_t drcLevel = ((liba52::sample_t)deci->getParam2(IDFF_audio_decoder_DRC_Level) / 100);
			a52_dynrngsetlevel(state, drcLevel);
		   }
           else
		   {
			a52_dynrngsetlevel(state, 0.0);
		   }
           int scmapidx=std::min(flags&A52_CHANNEL_MASK,int(countof(scmaps)/2));
           const Tscmap &scmap=scmaps[scmapidx+((flags&A52_LFE)?(countof(scmaps)/2):0)];
           float *dst0,*dst;dst0=dst=(float*)getDst(6*256*scmap.nchannels*sizeof(float));
           int i=0;
           for(;i<6 && a52_block(state)==0;i++)
            {
             liba52::sample_t* samples=a52_samples(state);
             for (int j=0;j<256;j++,samples++)
              for (int ch=0;ch<scmap.nchannels;ch++)
               *dst++=float(*(samples+256*scmap.ch[ch])/level);
            }
           if (i==6)
            {
             fmt.sf=TsampleFormat::SF_FLOAT32;
             fmt.freq=sample_rate;
             fmt.setChannels(scmap.nchannels,scmap.channelMask);
             HRESULT hr=sinkA->deliverDecodedSample(dst0,6*256,fmt,1);
             if (hr!=S_OK)
              return hr;
            }
          }
        }
       p+=size;
      }
     memmove(base,p,end-p);
     end=base+(end-p);
     p=base;
     if (!enoughData)
      break;
    }
   else
    p++;
  }
 src.resize(end-p);
 return S_OK;
}
bool TaudioCodecLiba52::onSeek(REFERENCE_TIME segmentStart)
{
 return false;
}
