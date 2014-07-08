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
#include "TaudioCodecLibDTS.h"
#include "Tdll.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"

const char_t* TaudioCodecLibDTS::dllname=_l("ff_libdts.dll");

// dshow: left, right, center, LFE, left surround, right surround
// dts: center, left, right, left surround, right surround, LFE
const TaudioCodecLibDTS::Tscmap TaudioCodecLibDTS::scmaps[2*10]=
{
 {1, {0,-1,-1,-1,-1,-1}, 0}, // DTS_MONO
 {2, {0, 1,-1,-1,-1,-1}, 0}, // DTS_CHANNEL
 {2, {0, 1,-1,-1,-1,-1}, 0}, // DTS_STEREO
 {2, {0, 1,-1,-1,-1,-1}, 0}, // DTS_STEREO_SUMDIFF
 {2, {0, 1,-1,-1,-1,-1}, 0}, // DTS_STEREO_TOTAL
 {3, {1, 2, 0,-1,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER}, // DTS_3F
 {3, {0, 1, 2,-1,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_BACK_CENTER},  // DTS_2F1R
 {4, {1, 2, 0, 3,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_BACK_CENTER}, // DTS_3F1R
 {4, {0, 1, 2, 3,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT},     // DTS_2F2R
 {5, {1, 2, 0, 3, 4,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT}, // DTS_3F2R

 {2, {0, 1,-1,-1,-1,-1}, SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY},                   // DTS_MONO|DTS_LFE
 {3, {0, 1, 2,-1,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_LOW_FREQUENCY},	// DTS_CHANNEL|DTS_LFE
 {3, {0, 1, 2,-1,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_LOW_FREQUENCY}, // DTS_STEREO|DTS_LFE
 {3, {0, 1, 2,-1,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_LOW_FREQUENCY}, // DTS_STEREO_SUMDIFF|DTS_LFE
 {3, {0, 1, 2,-1,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_LOW_FREQUENCY}, // DTS_STEREO_TOTAL|DTS_LFE
 {4, {1, 2, 0, 3,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY}, // DTS_3F|DTS_LFE
 {4, {0, 1, 3, 2,-1,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_CENTER},  // DTS_2F1R|DTS_LFE
 {5, {1, 2, 0, 4, 3,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_CENTER}, // DTS_3F1R|DTS_LFE
 {5, {0, 1, 4, 2, 3,-1}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT},     // DTS_2F2R|DTS_LFE
 {6, {1, 2, 0, 5, 3, 4}, SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT}, // DTS_3F2R|DTS_LFE
};


TaudioCodecLibDTS::TaudioCodecLibDTS(IffdshowBase *deci,IdecAudioSink *Isink):
 Tcodec(deci),
 TaudioCodec(deci,Isink)
{
 dll=NULL;state=NULL;
 inited=false;
}

bool TaudioCodecLibDTS::init(const CMediaType &mt)
{
 dll=new Tdll(dllname,config);
 dll->loadFunction(dca_init,"dca_init");
 dll->loadFunction(dca_free,"dca_free");
 dll->loadFunction(dca_syncinfo,"dca_syncinfo");
 dll->loadFunction(dca_frame,"dca_frame");
 dll->loadFunction(dca_dynrng,"dca_dynrng");
 dll->loadFunction(dca_blocks_num,"dca_blocks_num");
 dll->loadFunction(dca_block,"dca_block");
 dll->loadFunction(dca_samples,"dca_samples");
 if (dll->ok)
  {
   state=dca_init(Tconfig::cpu_flags);
   fmt.sf=TsampleFormat::SF_FLOAT32;
   inited=true;
   return true;
  }
 else
  return false;
}
TaudioCodecLibDTS::~TaudioCodecLibDTS()
{
 if (dll)
  {
   if (state) dca_free(state);
   delete dll;
  }
}

void TaudioCodecLibDTS::getInputDescr1(char_t *buf,size_t buflen) const
{
 ff_strncpy(buf,_l("dts"),buflen);
 buf[buflen-1]='\0';
}

HRESULT TaudioCodecLibDTS::decode(TbyteBuffer &src)
{
 unsigned char *p=&*src.begin();
 unsigned char *base=p;
 unsigned char *end=p+src.size();

 while (end-p>=14)
  {
   int size=0,flags,sample_rate,frame_length,bit_rate;
   if ((size=dca_syncinfo(state,p,&flags,&sample_rate,&bit_rate,&frame_length))>0)
    {
     bool enoughData=p+size<=end;
     if (enoughData)
      {
       if (codecId==CODEC_ID_SPDIF_DTS)
        {
         bpssum+=(lastbps=bit_rate/1000);numframes++;
         BYTE type;
         switch (frame_length)
          {
           case  512:type=0x0b;break;
           case 1024:type=0x0c;break;
           default  :type=0x0d;break;
          }
		 HRESULT hr=deciA->deliverSampleSPDIF(p,size,bit_rate,sample_rate,type,true);
         if (hr!=S_OK)
          return hr;
        }
       else
        {
         flags|=DCA_ADJUST_LEVEL;
         libdca::sample_t level=1,bias=0;
         if (dca_frame(state,p,&flags,&level,bias)==0)
          {
           bpssum+=(lastbps=bit_rate/1000);numframes++;
           // Dynamic range compression - Not suppored yet by libdts
           if (deci->getParam2(IDFF_audio_decoder_DRC))
            {
             libdca::sample_t drcLevel = ((libdca::sample_t)deci->getParam2(IDFF_audio_decoder_DRC_Level) / 100);
             if (drcLevel <= 0.5)
			 {
              dca_dynrng(state,NULL,NULL);
             }
            }
           else
            {
             dca_dynrng(state,NULL,NULL);
            }
           int scmapidx=std::min(flags&DCA_CHANNEL_MASK,int(countof(scmaps)/2));
           const Tscmap &scmap=scmaps[scmapidx+((flags&DCA_LFE)?(countof(scmaps)/2):0)];
           int blocks=dca_blocks_num(state);
           float *dst0,*dst;dst0=dst=(float*)getDst(blocks*256*scmap.nchannels*sizeof(float));
           int i=0;
           for(;i<blocks && dca_block(state)==0;i++)
            {
             libdca::sample_t* samples=dca_samples(state);
             for (int j=0;j<256;j++,samples++)
              for (int ch=0;ch<scmap.nchannels;ch++)
               *dst++=float(*(samples+256*scmap.ch[ch])/level);
            }
           if (i==blocks)
            {
             fmt.sf=TsampleFormat::SF_FLOAT32;
             fmt.freq=sample_rate;
             fmt.setChannels(scmap.nchannels,scmap.channelMask);
             HRESULT hr=sinkA->deliverDecodedSample(dst0,blocks*256,fmt,1);
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
bool TaudioCodecLibDTS::onSeek(REFERENCE_TIME segmentStart)
{
 return false;
}
