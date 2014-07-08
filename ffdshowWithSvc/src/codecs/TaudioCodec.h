#ifndef _TAUDIOCODEC_H_
#define _TAUDIOCODEC_H_

#include "Tcodec.h"
#include "ffdshow_constants.h"
#include "TsampleFormat.h"

DECLARE_INTERFACE_(IdecAudioSink,IdecSink)
{
 STDMETHOD (deliverDecodedSample)(void *buf,size_t numsamples,const TsampleFormat &fmt,float postgain) PURE;
 STDMETHOD (setCodecId)(CodecID codecId) PURE;
 STDMETHOD (getCodecId)(CodecID *pCodecId) PURE;
};

class TaudioCodec :public TcodecDec
{
private:
 static TaudioCodec* getDecLib(CodecID codecId,IffdshowBase *deci,IdecAudioSink *sink);
 Tbuffer buf;
protected:
 comptrQ<IffdshowDecAudio> deciA;
 IdecAudioSink *sinkA;
 TsampleFormat fmt;
 virtual bool init(const CMediaType &mt)=0;
 void* getDst(size_t needed);
 virtual void getInputDescr1(char_t *buf,size_t buflen) const =0;
 unsigned int bpssum,numframes,lastbps;
public:
 TaudioCodec(IffdshowBase *Ideci,IdecAudioSink *Isink);
 static TaudioCodec* initSource(IffdshowBase *Ideci,IdecAudioSink *sink,CodecID codecId,const TsampleFormat &fmt,const CMediaType &mt);
 const TsampleFormat& getInputSF(void) const {return fmt;}
 unsigned int getLastbps(void) const {return lastbps;}

 void getInputDescr(char_t *buf,size_t buflen) const;
 virtual HRESULT decode(TbyteBuffer &src)=0;
};

#endif
