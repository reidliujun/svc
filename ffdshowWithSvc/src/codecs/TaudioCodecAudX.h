#ifndef _TAUDIOCODECAUDX_H_
#define _TAUDIOCODECAUDX_H_

#include "TaudioCodec.h"

class Tdll;
class TaudioCodecAudX :public TaudioCodec
{
private:
 Tdll *dll;
 long (__cdecl *initDecoding)(void);
 int  (__cdecl *decodeAudXframe)( long hdec, const unsigned char* audx_frame, long audx_frame_size, short* dec_pcm_block, long* dec_pcm_block_size, int* is_audx );
 void (__cdecl *resetDecoder)( long hdec );
 int layer,oldmode,hdrlayer;
 long ctx;
protected:
 virtual bool init(const CMediaType &mt);
 virtual void getInputDescr1(char_t *buf,size_t buflen) const;
public:
 TaudioCodecAudX(IffdshowBase *deci,IdecAudioSink *Isink);
 virtual ~TaudioCodecAudX();
 virtual int getType(void) const {return IDFF_MOVIE_AUDX;}
 static const char_t *dllname;
 static bool getVersion(const Tconfig *config,ffstring &vers,ffstring &license);
 virtual HRESULT decode(TbyteBuffer &src);
 virtual bool onSeek(REFERENCE_TIME segmentStart);
};

#endif
