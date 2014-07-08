#ifndef _TAUDIOCODECAVISYNTH_H_
#define _TAUDIOCODECAVISYNTH_H_

#include "TaudioCodec.h"
#include "avisynth/Tavisynth.h"

struct Textradata;
class TaudioCodecAvisynth :public TaudioCodec
{
private:
 TavisynthAudio *avisynth;
protected:
 virtual bool init(const CMediaType &mt);
 virtual void getInputDescr1(char_t *buf,size_t buflen) const;
public:
 TaudioCodecAvisynth(IffdshowBase *deci,IdecAudioSink *Isink);
 virtual ~TaudioCodecAvisynth();
 virtual int getType(void) const {return IDFF_MOVIE_AVIS;}
 virtual HRESULT decode(TbyteBuffer &src);
};

#endif
