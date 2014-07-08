#ifndef _TAUDIOCODECLIBMAD_H_
#define _TAUDIOCODECLIBMAD_H_

#include "TaudioCodec.h"
#include "libmad/mad.h"

class Tdll;
class TaudioCodecLibMAD :public TaudioCodec
{
private:
 Tdll *dll;
 void (*mad_synth_init)(struct mad_synth *);
 void (*mad_stream_init)(struct mad_stream *);
 void (*mad_stream_finish)(struct mad_stream *);
 void (*mad_frame_init)(struct mad_frame *);
 void (*mad_frame_finish)(struct mad_frame *);
 void (*mad_synth_frame)(struct mad_synth *, struct mad_frame const *);
 int  (*mad_frame_decode)(struct mad_frame *, struct mad_stream *);
 void (*mad_stream_buffer)(struct mad_stream *, unsigned char const *, unsigned long);
 bool inited;
 MPEG1WAVEFORMAT infmt;
 mad_synth  synth;
 mad_stream stream;
 mad_frame  frame;
 static int32_t scale(mad_fixed_t sample);
protected:
 virtual bool init(const CMediaType &mt);
 virtual void getInputDescr1(char_t *buf,size_t buflen) const;
public:
 TaudioCodecLibMAD(IffdshowBase *deci,IdecAudioSink *Isink);
 virtual ~TaudioCodecLibMAD();
 virtual int getType(void) const {return IDFF_MOVIE_LIBMAD;}
 static const char_t *dllname;
 virtual HRESULT decode(TbyteBuffer &src);
 virtual bool onSeek(REFERENCE_TIME segmentStart);
};

#endif
