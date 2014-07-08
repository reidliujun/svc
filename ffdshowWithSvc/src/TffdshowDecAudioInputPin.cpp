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
#include "TffdshowDecAudioInputPin.h"
#include "TffdshowDecAudio.h"
#include "ffdshow_mediaguids.h"
#include "TglobalSettings.h"
#include "TaudioCodecLibavcodec.h"
#include "ffdebug.h"

TffdshowDecAudioInputPin::TffdshowDecAudioInputPin(const char_t* pObjectName, TffdshowDecAudio* pFilter, HRESULT* phr, LPWSTR pName, int Inumber)
 :TinputPin(pObjectName,pFilter,phr,pName),
  filter(pFilter),
  number(Inumber),
  audio(NULL),
  jitter(0),
  prevpostgain(1.0f),
  insample_rtStart(REFTIME_INVALID),
  insample_rtStop(REFTIME_INVALID),
  audioParser(NULL)
{
}
TffdshowDecAudioInputPin::~TffdshowDecAudioInputPin()
{
	if (audioParser) delete(audioParser);
	audioParser=NULL;
}

bool TffdshowDecAudioInputPin::init(const CMediaType &mt)
{
 DPRINTF(_l("TffdshowDecAudioInputPin::initAudio"));
 codecId=filter->getCodecId(mt);
 if (codecId==CODEC_ID_NONE) return false;
 strippacket=(mt.majortype==MEDIATYPE_DVD_ENCRYPTED_PACK || mt.majortype==MEDIATYPE_MPEG2_PES);
 if (mt.formattype==FORMAT_WaveFormatEx)
  {
   const WAVEFORMATEX *wfex=(const WAVEFORMATEX*)mt.pbFormat;
   searchdts=(filter->globalSettings->dts && filter->globalSettings->dtsinwav && wfex->wFormatTag==1);
  }
 filter->insf=mt;
 if (audio) {delete audio;codec=audio=NULL;}
 codec=audio=TaudioCodec::initSource(filter,this,codecId,filter->insf,mt);
 if (!audio) return false;
 filter->insf=audio->getInputSF();
 if (audioParser) {delete audioParser; audioParser=NULL;}
 audioParser=new TaudioParser(filter, this);
 return true;
}

void TffdshowDecAudioInputPin::done(void)
{
 if (audio)
  {
   delete audio;
   codec=audio=NULL;
  }
 if (audioParser)
 {
	 delete audioParser;
	 audioParser=NULL;
 }
 codecId=CODEC_ID_NONE;
}

STDMETHODIMP TffdshowDecAudioInputPin::EndFlush(void)
{
 DPRINTF(_l("TffdshowDecAudioInputPin::EndFlush"));
 CAutoLock cAutoLock(&m_csReceive);
 buf.clear();
 newSrcBuffer.clear();
 return TinputPin::EndFlush();
}

STDMETHODIMP TffdshowDecAudioInputPin::NewSegment(REFERENCE_TIME tStart,REFERENCE_TIME tStop,double dRate)
{
 DPRINTF(_l("TffdshowDecAudioInputPin::NewSegment"));
 CAutoLock cAutoLock(&m_csReceive);
 buf.clear();
 newSrcBuffer.clear();
 jitter=0;
 insample_rtStart = REFTIME_INVALID;
 insample_rtStop = REFTIME_INVALID;
 // Tell the parser that the position has changed so the parsing should take this into account
 // (it won't receive the next expected buffer)
 if (audioParser) audioParser->NewSegment();
 return TinputPin::NewSegment(tStart,tStop,dRate);
}

STDMETHODIMP TffdshowDecAudioInputPin::Receive(IMediaSample* pIn)
{
 if (this!=filter->inpin)
  return S_FALSE;

 CAutoLock cAutoLock(&m_csReceive);

 if (filter->IsStopped())
  return S_FALSE;

 HRESULT hr=TinputPin::Receive(pIn);
 if (hr!=S_OK)
  return hr;

 AM_SAMPLE2_PROPERTIES* const pProps=SampleProps();
 if (pProps->dwStreamId!=AM_STREAM_MEDIA)
  return filter->m_pOutput->Deliver(pIn);

 AM_MEDIA_TYPE *pmt=NULL;
 if (SUCCEEDED(pIn->GetMediaType(&pmt)) && pmt)
  {
   CMediaType mt(*pmt);
   SetMediaType(&mt);
   DeleteMediaType(pmt);
   pmt=NULL;
  }

 BYTE *src=NULL;
 if (FAILED(hr=pIn->GetPointer(&src))) return hr;
 long srclen=pIn->GetActualDataLength();
 if (strippacket) StripPacket(src,srclen);

 REFERENCE_TIME rtStart=_I64_MIN,rtStop=_I64_MIN;
 hr=pIn->GetTime(&rtStart,&rtStop);

 if (hr == S_OK)
  {
   insample_rtStart = rtStart;
   insample_rtStop = rtStop;
   //DPRINTF(_l("audio sample start duration %I64i %I64i"),rtStart,rtStop-rtStart);
  }

 if (pIn->IsDiscontinuity()==S_OK)
  {
   filter->discontinuity=true;
   buf.clear();
   if (FAILED(hr)) return S_OK;
   filter->m_rtStartDec=filter->m_rtStartProc=rtStart;
  }

 if (SUCCEEDED(hr))
  {
   REFERENCE_TIME j=filter->m_rtStartDec-rtStart;
   jitter=int(j);
   if ((uint64_t)ff_abs(j) > 100 * (REF_SECOND_MULT / 1000) // +-100ms jitter is allowed for now
       && codecId != CODEC_ID_FLAC
       && codecId != CODEC_ID_TTA
	   && codecId != CODEC_ID_MLP)
    {
     DPRINTF(_l("jitter correction"));
     buf.clear();
	 newSrcBuffer.clear();
     filter->m_rtStartDec=filter->m_rtStartProc=rtStart;
    }
  }

 buf.append(src,srclen);
 buf.reserve(buf.size()+32);

 CodecID newCodecId=codecId;
 // Before sending data to the decoder, we parse it
 switch(codecId)
 {
 case CODEC_ID_DTS:
 case CODEC_ID_LIBDTS:
 case CODEC_ID_SPDIF_DTS:
 case CODEC_ID_AC3:
 case CODEC_ID_EAC3:
 case CODEC_ID_MLP:
 case CODEC_ID_LIBA52:
 case CODEC_ID_SPDIF_AC3:
 case CODEC_ID_PCM:
	// Search for DTS in Wav only if option is checked
	if (codecId==CODEC_ID_PCM && !searchdts) break;
	
	// Do not search for DTS in PCM in next frames (otherwise DTS syncword maybe wrongly detected)
	searchdts=false;

	newCodecId=audioParser->parseStream(&*buf.begin(), buf.size(), &newSrcBuffer);
	if (newCodecId==CODEC_ID_NONE)
	{
		newSrcBuffer.clear();
		break;
	}

	// Clear input buffer (if 2 source buffers are coexisting, sound will be garbled)
	buf.clear();
	 if (codecId != newCodecId)
	 {
		DPRINTF(_l("TffdshowDecAudioInputPin : switching codec from %d to %d"), codecId, newCodecId);
		codecId=newCodecId;
		TsampleFormat fmt=TsampleFormat(TsampleFormat::SF_PCM16,
			audioParser->sample_rate,audioParser->channels);
		CMediaType mt=fmt.toCMediaType();
		if (audioParser->channels != 0)
		 filter->insf.setChannels(audioParser->channels);
	 if (audioParser->sample_rate != 0)
		 filter->insf.freq=audioParser->sample_rate;
	 if (audioParser->sample_format != 0)
		 filter->insf.sf=audioParser->sample_format;


		((WAVEFORMATEX*)mt.Format())->wFormatTag=(WORD)audioParser->wFormatTag;

		if (audio) {delete audio;codec=audio=NULL;}
		codec=audio=TaudioCodec::initSource(filter,this,codecId,filter->insf,mt);
		if (!audio) return false;
		filter->insf=audio->getInputSF();
	 }
	 // Update libavcodec context with correct channels and bitrate read from parser
	 if (lavc_codec (codecId)) 
	 {
		 TaudioCodecLibavcodec *audioCodecLibavcodec=(TaudioCodecLibavcodec*)audio;
		 if (audioCodecLibavcodec)
		 {
			 if (audioParser->channels != 0)
				 audioCodecLibavcodec->avctx->channels=audioParser->channels;
			if (audioParser->bit_rate != 0)
				audioCodecLibavcodec->avctx->bit_rate=audioParser->bit_rate;
			if (audioParser->sample_rate != 0)
				audioCodecLibavcodec->avctx->sample_rate=audioParser->sample_rate;
		 }
	 }

	 if (audioParser->channels != 0)
		 filter->insf.setChannels(audioParser->channels);
	 if (audioParser->sample_rate != 0)
		 filter->insf.freq=audioParser->sample_rate;
	 /*if (audioParser->sample_format != 0)
		 filter->insf.sf=audioParser->sample_format;*/

	 newSrcBuffer.reserve(newSrcBuffer.size()+32);
	 return audio->decode(newSrcBuffer);
	 break;
 default:
	 // Decode data
	return audio->decode(buf);
	break;
 }
 return audio->decode(buf);
}

STDMETHODIMP TffdshowDecAudioInputPin::deliverDecodedSample(void *buf,size_t numsamples,const TsampleFormat &fmt,float postgain)
{
 if (numsamples==0) return S_OK;
 return filter->deliverDecodedSample(this,buf,numsamples,fmt,prevpostgain=postgain);
}

STDMETHODIMP TffdshowDecAudioInputPin::flushDecodedSamples(void)
{
 return filter->flushDecodedSamples(this,prevpostgain);
}

STDMETHODIMP TffdshowDecAudioInputPin::setCodecId(CodecID codecId)
{
	this->codecId=codecId;
	return S_OK;
}

STDMETHODIMP TffdshowDecAudioInputPin::getCodecId(CodecID *pCodecId)
{
	*pCodecId=codecId;
	return S_OK;
}

HRESULT TffdshowDecAudioInputPin::getMovieSource(const TaudioCodec* *moviePtr)
{
 if (!moviePtr) return S_FALSE;
 *moviePtr=audio;
 return S_OK;
}
HRESULT TffdshowDecAudioInputPin::getInCodecString(char_t *buf,size_t buflen)
{
 if (!buf) return E_POINTER;
 if (audio)
  {
   audio->getInputDescr(buf,buflen);
   return S_OK;
  }
 else
  {
   buf[0]='\0';
   return S_FALSE;
  }
}

int TffdshowDecAudioInputPin::getInputBitrate(void) const
{
 return audio?audio->getLastbps():-1;
}

bool TffdshowDecAudioInputPin::getsf(TsampleFormat &outsf)
{
 if (!audio)
  outsf=CurrentMediaType();
 else
  if (spdif_codec(audio->codecId))
   {
    outsf=TsampleFormat::createMediaTypeSPDIF(audio->getInputSF().freq);
    return true;
   }
  else
   outsf=audio->getInputSF();
 return false;
}
int TffdshowDecAudioInputPin::getJitter(void) const
{
 if (codecId != CODEC_ID_FLAC && codecId != CODEC_ID_TTA)
  return jitter/int(REF_SECOND_MULT/1000);
 else
  return 0;
}
