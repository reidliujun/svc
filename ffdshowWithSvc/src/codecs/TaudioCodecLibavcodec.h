#ifndef _TAUDIOCODECLIBAVCODEC_H_
#define _TAUDIOCODECLIBAVCODEC_H_

#include "TaudioCodec.h"
#include "Tlibavcodec.h"
#include "TaudioParser.h"


class TaudioCodecLibavcodec :public TaudioCodec
{
private:
 Tlibavcodec *libavcodec;
 AVCodec *avcodec;mutable char codecName[100];
 AVCodecParserContext *parser;
 bool codecinited;
 int isGain;
 bool contextinited;
 Tbuffer buf2, buf3;
 //TbyteBuffer backupbuf;
 int src_ch_layout;
 //int skipBytes;int includeBytes;bool hasMLPFrames;bool useSPDIF;
 //StreamFormat streamformat;
 TaudioParser *audioParser;
protected:
 virtual bool init(const CMediaType &mt);
 virtual void getInputDescr1(char_t *buf,size_t buflen) const;
 virtual void updateChannelMapping(void);
 //virtual HRESULT parseDTS(unsigned char *src, int size, TbyteBuffer *newsrcBuffer);
 //virtual HRESULT parseAC3(unsigned char *src, int size, TbyteBuffer *newsrcBuffer);
 //virtual void printbitssimple(uint32_t n);
public:
 AVCodecContext *avctx;
 TaudioCodecLibavcodec(IffdshowBase *deci,IdecAudioSink *Isink);
 virtual ~TaudioCodecLibavcodec();
 virtual int getType(void) const {return IDFF_MOVIE_LAVC;}
 virtual const char_t* getName(void) const;
 virtual HRESULT decode(TbyteBuffer &src);
 virtual bool onSeek(REFERENCE_TIME segmentStart);
};

#endif
