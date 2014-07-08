#ifndef _TFFDSHOWDECAUDIOINPUTPIN_H_
#define _TFFDSHOWDECAUDIOINPUTPIN_H_

#include "TinputPin.h"
#include "TaudioCodec.h"
#include "TaudioParser.h"

class TffdshowDecAudio;
class TffdshowDecAudioInputPin :public TinputPin,public IdecAudioSink
{
private:
 TffdshowDecAudio *filter;
 bool searchdts;
 CCritSec m_csReceive;
 TbyteBuffer buf,newSrcBuffer;
 int jitter;
 float prevpostgain;
 TaudioParser *audioParser;
protected:
 virtual bool init(const CMediaType &mt);
 virtual void done(void);
public:
 REFERENCE_TIME insample_rtStart,insample_rtStop;
 TffdshowDecAudioInputPin(const char_t* pObjectName, TffdshowDecAudio* pFilter, HRESULT* phr, LPWSTR pName,int Inumber);
 virtual ~TffdshowDecAudioInputPin();
 TaudioCodec *audio;
 void block(bool is);
 int number;
 bool getsf(TsampleFormat &sf); //true if S/PDIF
 bool is_spdif_codec(void) const {return  audio && spdif_codec(audio->codecId);}

 STDMETHODIMP Receive(IMediaSample* pSample);
 STDMETHODIMP EndFlush(void);
 STDMETHODIMP NewSegment(REFERENCE_TIME tStart,REFERENCE_TIME tStop,double dRate);

 // IdecAudioSink
 STDMETHODIMP deliverDecodedSample(void *buf,size_t numsamples,const TsampleFormat &fmt,float postgain);
 STDMETHODIMP flushDecodedSamples(void);
 STDMETHODIMP setCodecId(CodecID codecId);
 STDMETHODIMP getCodecId(CodecID *pCodecId);

 HRESULT getMovieSource(const TaudioCodec* *moviePtr);
 virtual HRESULT getInCodecString(char_t *buf,size_t buflen);
 int getInputBitrate(void) const;
 int getJitter(void) const;
};

#endif
