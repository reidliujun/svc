#ifndef _TAUDIOCODECREALAAC_H_
#define _TAUDIOCODECREALAAC_H_

#include "TaudioCodec.h"
#include "realaac/aacdec.h"

class Tdll;
struct Textradata;
struct GetBitContext;
class TaudioCodecRealaac :public TaudioCodec
{
private:
 Tdll *dll;
 bool inited;
 HAACDecoder dec;
 HAACDecoder (*AACInitDecoder)(void);
 void (*AACFreeDecoder)(HAACDecoder hAACDecoder);
 int (*AACSetRawBlockParams)(HAACDecoder hAACDecoder, int copyLast, AACFrameInfo *aacFrameInfo);
 int (*AACDecode)(HAACDecoder hAACDecoder, unsigned char **inbuf, int *bytesLeft, short *outbuf);
 void (*AACGetLastFrameInfo)(HAACDecoder hAACDecoder, AACFrameInfo *aacFrameInfo);
 int (*AACFlushCodec)(HAACDecoder hAACDecoder);
 AACFrameInfo info;
 int sbr_present_flag,frameLengthFlag;
 enum {ER_OBJECT_START=17};
 int gaSpecificConfig(GetBitContext *gb);
 static void reorderChannels(int16_t *pcmBuf,int nSamps,int nChans);
protected:
 virtual bool init(const CMediaType &mt);
 virtual void getInputDescr1(char_t *buf,size_t buflen) const;
public:
 TaudioCodecRealaac(IffdshowBase *deci,IdecAudioSink *Isink);
 virtual ~TaudioCodecRealaac();
 virtual int getType(void) const {return IDFF_MOVIE_REALAAC;}
 static const char_t *dllname;
 virtual HRESULT decode(TbyteBuffer &src);
 virtual bool onSeek(REFERENCE_TIME segmentStart);
};

#endif
