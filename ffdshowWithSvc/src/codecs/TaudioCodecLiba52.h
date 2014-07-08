#ifndef _TAUDIOCODECLIBA52_H_
#define _TAUDIOCODECLIBA52_H_

#include "TaudioCodec.h"
namespace liba52
{
 #include "liba52/a52.h"
}

class Tdll;
class TaudioCodecLiba52 :public TaudioCodec
{
private:
 Tdll *dll;
 bool inited;
 liba52::a52_state_t* (*a52_init)(uint32_t mm_accel);
 liba52::sample_t* (*a52_samples)(liba52::a52_state_t * state);
 int (*a52_syncinfo)(uint8_t * buf, int * flags, int * sample_rate, int * bit_rate);
 void (*a52_dynrng)(liba52::a52_state_t * state,liba52::sample_t (* call) (liba52::sample_t, void *), void * data);
 void (*a52_dynrngsetlevel)(liba52::a52_state_t * state,liba52::sample_t compressionlevel);
 int (*a52_frame)(liba52::a52_state_t * state, uint8_t * buf, int * flags, liba52::sample_t * level, liba52::sample_t bias);
 int (*a52_block)(liba52::a52_state_t * state);
 void (*a52_free)(liba52::a52_state_t * state);
 liba52::a52_state_t *state;
 static struct Tscmap
  {
   int nchannels;
   char ch[6];
   int channelMask;
  } const scmaps[];
 int drc;
protected:
 virtual bool init(const CMediaType &mt);
 virtual void getInputDescr1(char_t *buf,size_t buflen) const;
public:
 TaudioCodecLiba52(IffdshowBase *deci,IdecAudioSink *Isink);
 virtual ~TaudioCodecLiba52();
 virtual int getType(void) const {return IDFF_MOVIE_LIBA52;}
 static const char_t *dllname;
 virtual HRESULT decode(TbyteBuffer &src);
 virtual bool onSeek(REFERENCE_TIME segmentStart);
};

#endif
