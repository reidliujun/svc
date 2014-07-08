#ifndef _TAUDIOCODECMPLAYER_H_
#define _TAUDIOCODECMPLAYER_H_

#include "TaudioCodec.h"

struct Tlibmplayer;
struct mp3lib_ctx;
class TaudioCodecMplayer :public TaudioCodec
{
private:
 Tlibmplayer *dll;
 mp3lib_ctx *ctx;
 int layer,oldmode,hdrlayer;
protected:
 virtual bool init(const CMediaType &mt);
 virtual void getInputDescr1(char_t *buf,size_t buflen) const;
public:
 TaudioCodecMplayer(IffdshowBase *deci,IdecAudioSink *Isink);
 virtual ~TaudioCodecMplayer();
 virtual int getType(void) const {return IDFF_MOVIE_MPLAYER;}
 virtual HRESULT decode(TbyteBuffer &src);
 virtual bool onSeek(REFERENCE_TIME segmentStart);
};

#endif
