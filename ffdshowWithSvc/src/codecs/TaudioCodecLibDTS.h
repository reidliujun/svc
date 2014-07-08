#ifndef _TAUDIOCODECLIBDTS_H_
#define _TAUDIOCODECLIBDTS_H_

#include "TaudioCodec.h"
namespace libdca
{
 #include "libdts/dca.h"
}

class Tdll;
class TaudioCodecLibDTS :public TaudioCodec
{
private:
 Tdll *dll;
 bool inited;
 libdca::dca_state_t* (*dca_init)(uint32_t mm_accel);
 void (*dca_free)(libdca::dca_state_t * state);
 int (*dca_syncinfo)(libdca::dca_state_t *state, uint8_t * buf, int * flags, int * sample_rate, int * bit_rate, int *frame_length);
 int (*dca_frame)(libdca::dca_state_t * state, uint8_t * buf, int * flags, libdca::level_t * level, libdca::sample_t bias);
 void (*dca_dynrng)(libdca::dca_state_t * state, libdca::level_t (* call) (libdca::level_t, void *), void * data);
 int (*dca_blocks_num)(libdca::dca_state_t * state);
 int (*dca_block)(libdca::dca_state_t * state);
 libdca::sample_t* (*dca_samples)(libdca::dca_state_t * state);

 libdca::dca_state_t *state;
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
 TaudioCodecLibDTS(IffdshowBase *deci,IdecAudioSink *Isink);
 virtual ~TaudioCodecLibDTS();
 virtual int getType(void) const {return IDFF_MOVIE_LIBDTS;}
 static const char_t *dllname;
 virtual HRESULT decode(TbyteBuffer &src);
 virtual bool onSeek(REFERENCE_TIME segmentStart);
};

#endif
