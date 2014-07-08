/*
 * Copyright (c) 2005-2006 Milan Cutka
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
#include "TaudioCodecRealaac.h"
#include "Tdll.h"
#include "ffdebug.h"
#include "IffdshowDecAudio.h"
#include "dsutil.h"
#include "libavcodec/bitstream.h"

const char_t* TaudioCodecRealaac::dllname=_l("ff_realaac.dll");

TaudioCodecRealaac::TaudioCodecRealaac(IffdshowBase *deci,IdecAudioSink *Isink):
 Tcodec(deci),
 TaudioCodec(deci,Isink)
{
 dll=NULL;dec=NULL;
 inited=false;sbr_present_flag=-1;
}

int TaudioCodecRealaac::gaSpecificConfig(GetBitContext *gb)
{
 frameLengthFlag=get_bits1(gb);
 int dependsOnCoreCoder=get_bits1(gb);
 if (dependsOnCoreCoder==1)
  {
   int coreCoderDelay=get_bits(gb,14);
  }

 int extensionFlag=get_bits1(gb);
/*
 if (info.nChans==0)
  {
   if (program_config_element(&pce,&gb))
    return -3;
  }
*/
 if (extensionFlag==1)
  {
   if (info.profile>ER_OBJECT_START)
    {
     int aacSectionDataResilienceFlag=get_bits1(gb);
     int aacScalefactorDataResilienceFlag=get_bits1(gb);
     int aacSpectralDataResilienceFlag=get_bits1(gb);
     // 1 bit: extensionFlag3
    }
  }
 return 0;
}

bool TaudioCodecRealaac::init(const CMediaType &mt)
{
 dll=new Tdll(dllname,config);
 dll->loadFunction(AACInitDecoder,"AACInitDecoder");
 dll->loadFunction(AACFreeDecoder,"AACFreeDecoder");
 dll->loadFunction(AACSetRawBlockParams,"AACSetRawBlockParams");
 dll->loadFunction(AACDecode,"AACDecode");
 dll->loadFunction(AACGetLastFrameInfo,"AACGetLastFrameInfo");
 dll->loadFunction(AACFlushCodec,"AACFlushCodec");
 if (dll->ok)
  {
   Textradata extradata(mt,8);
   if (!extradata.size) return false;
   GetBitContext gb;
   init_get_bits(&gb,extradata.data,(int)extradata.size*8);
   int objectTypeIndex=get_bits(&gb,5);
   int samplingFrequencyIndex=get_bits(&gb,4);
   static const uint32_t sample_rates[]=
    {
     96000, 88200, 64000, 48000, 44100, 32000,
     24000, 22050, 16000, 12000, 11025, 8000
    };
   if (samplingFrequencyIndex<12)
    info.sampRateCore=sample_rates[samplingFrequencyIndex];
   else
    return false;
   info.nChans=get_bits(&gb,4);
   if (info.nChans>7) return false;
   info.profile=1;
   dec=AACInitDecoder();
   AACSetRawBlockParams(dec,0,&info);
   fmt.freq=info.sampRateCore;
   info.profile=objectTypeIndex;
   sbr_present_flag=-1;
   int downSampledSBR=0;
   if (objectTypeIndex==5)
    {
     sbr_present_flag=1;
     int tmp=get_bits(&gb,4);
     if (tmp==samplingFrequencyIndex)
      downSampledSBR=1;
     samplingFrequencyIndex=tmp;
     if (samplingFrequencyIndex==15)
      fmt.freq=get_bits(&gb,24);
     else
      fmt.freq=sample_rates[samplingFrequencyIndex];
     objectTypeIndex=get_bits(&gb,5);
    }
   if (objectTypeIndex==1 || objectTypeIndex==2 ||
       objectTypeIndex==3 || objectTypeIndex==4 ||
       objectTypeIndex==6 || objectTypeIndex==7)
    {
     if (gaSpecificConfig(&gb)<0)
      return false;
    }
   else if (objectTypeIndex>=ER_OBJECT_START)
    { /* ER */
     if (gaSpecificConfig(&gb)<0)
      return false;
     int epConfig=get_bits(&gb,2);
     if (epConfig!=0)
      return false;
    }
   else
    return false;
   if (objectTypeIndex==4 && frameLengthFlag)
    return false;

   int bits_to_decode=16;//(extradata->size*8-get_bits_trace  faad_get_processed_bits(&ld));
   if (objectTypeIndex!=5 && bits_to_decode>=16)
    {
     int syncExtensionType=get_bits(&gb,11);
     if (syncExtensionType==0x2b7)
      {
       objectTypeIndex=get_bits(&gb,5);
       if (objectTypeIndex==5)
        {
         sbr_present_flag=get_bits1(&gb);
         if (sbr_present_flag)
          {
           int tmp=get_bits(&gb,4);
           if (tmp==samplingFrequencyIndex)
            downSampledSBR=1;
           samplingFrequencyIndex=tmp;
           if (samplingFrequencyIndex==15)
            fmt.freq=get_bits(&gb,24);
           else
            fmt.freq=sample_rates[samplingFrequencyIndex];
          }
        }
      }
    }
   if (sbr_present_flag==-1)
    if (fmt.freq<=24000)
     {
      fmt.freq*=2;
      int forceUpSampling=1;
     }
    else
     downSampledSBR=1;
   fmt.sf=TsampleFormat::SF_PCM16;
   fmt.setChannels(info.nChans);
   //fmt.freq=info.sampRateCore;
   inited=true;
   return true;
  }
 else
  return false;
}
TaudioCodecRealaac::~TaudioCodecRealaac()
{
 if (dec)
  AACFreeDecoder(dec);
 if (dll) delete dll;
}

void TaudioCodecRealaac::getInputDescr1(char_t *buf,size_t buflen) const
{
 if (!inited) return;
 tsnprintf_s(buf, buflen, _TRUNCATE, _l("%sAAC"), sbr_present_flag==1 ? _l("SBR ") : _l(""));
}

void TaudioCodecRealaac::reorderChannels(int16_t *pcmBuf, int nSamps, int nChans)
{
 const int *map;

 switch (nChans)
  {
   case 3:
    {
     static const int chanMap[]={
         1,  // L
         2,  // R
         0}; // C
     map=chanMap;
     break;
    }
   case 4:
    {
     static const int chanMap[]={
         1,  // L
         2,  // R
         0,  // C
         3}; // S
     map=chanMap;
     break;
    }
   case 5:
    {
     static const int chanMap[]={
         1,  // L
         2,  // R
         0,  // C
         3,  // LS
         4}; // RS
     map=chanMap;
     break;
    }
   case 6:
    {
     static const int chanMap[]={
         1,  // L
         2,  // R
         0,  // C
         5,  // LFE
         3,  // LS
         4}; // RS
     map=chanMap;
     break;
    }
   default:
    return;
  }

 for (int i=0;i<nSamps;i+=nChans)
  {
   int16_t tmpBuf[6];
   for (int ch=0;ch<nChans;ch++)
    tmpBuf[ch]=pcmBuf[map[ch]];
   for (int ch=0;ch<nChans;ch++)
    pcmBuf[ch]=tmpBuf[ch];
   pcmBuf+=nChans;
  }
}

HRESULT TaudioCodecRealaac::decode(TbyteBuffer &src0)
{
 unsigned char *src=&*src0.begin();
 int srclen=(int)src0.size(),srclen0=srclen;
 short *outbuf=(short*)getDst(AAC_MAX_NCHANS*AAC_MAX_NSAMPS*2*sizeof(short));
 int err=AACDecode(dec,&src,&srclen,outbuf);
 if (err)
  if (err==ERR_AAC_INDATA_UNDERFLOW)
   return S_OK;
  else
   return E_FAIL;
 src0.clear();
 AACGetLastFrameInfo(dec,&info);
 fmt.setChannels(info.nChans);
 fmt.freq=info.sampRateOut;
 reorderChannels(outbuf,info.outputSamps,info.nChans);
 numframes++;bpssum+=(lastbps=8*fmt.freq*srclen0/(info.outputSamps/fmt.nchannels)/1000);
 return sinkA->deliverDecodedSample(outbuf,info.outputSamps/fmt.nchannels,fmt,1);
}
bool TaudioCodecRealaac::onSeek(REFERENCE_TIME segmentStart)
{
 return dec?(AACFlushCodec(dec),true):false;
}
