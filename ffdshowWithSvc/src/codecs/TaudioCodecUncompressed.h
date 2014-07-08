#ifndef _TAUDIOCODECUNCOMPRESSED_H_
#define _TAUDIOCODECUNCOMPRESSED_H_

#include "TaudioCodec.h"

class TaudioCodecUncompressed :public TaudioCodec
{
private:
 bool bit8,lpcm20,lpcm24,be,float64;
 template<class sample_t> static void swapbe(sample_t *dst,size_t size);
protected:
 virtual bool init(const CMediaType &mt);
 virtual void getInputDescr1(char_t *buf,size_t buflen) const;
public:
 TaudioCodecUncompressed(IffdshowBase *deci,IdecAudioSink *Isink);
 virtual ~TaudioCodecUncompressed();
 virtual int getType(void) const {return IDFF_MOVIE_RAW;}
 virtual HRESULT decode(TbyteBuffer &src);
};

#endif
