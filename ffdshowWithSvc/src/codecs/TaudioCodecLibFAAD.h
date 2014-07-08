#ifndef _TAUDIOCODECLIBFAAD_H_
#define _TAUDIOCODECLIBFAAD_H_

#include "TaudioCodec.h"
#include "faad/neaacdec.h"

class Tdll;
struct mp4AudioSpecificConfig;
class TaudioCodecLibFAAD :public TaudioCodec
{
private:
 Tdll *dll;
 bool inited;
 NeAACDecHandle (NEAACDECAPI *NeAACDecOpen)(void);
 void (NEAACDECAPI *NeAACDecClose)(NeAACDecHandle hDecoder);
 char (NEAACDECAPI *NeAACDecInit2)(NeAACDecHandle hDecoder, const unsigned char *pBuffer,
                                   unsigned long SizeOfDecoderSpecificInfo,
                                   unsigned long *samplerate, unsigned char *channels);
 char (NEAACDECAPI *NeAACDecAudioSpecificConfig)(const unsigned char *pBuffer,
                                                 unsigned long buffer_size,
                                                 mp4AudioSpecificConfig *mp4ASC);
 void* (NEAACDECAPI *NeAACDecDecode)(NeAACDecHandle hDecoder,
                                     NeAACDecFrameInfo *hInfo,
                                     unsigned char *buffer,
                                     unsigned long buffer_size);
 char* (NEAACDECAPI *NeAACDecGetErrorMessage)(unsigned char errcode);
 NeAACDecConfigurationPtr (NEAACDECAPI *NeAACDecGetCurrentConfiguration)(NeAACDecHandle hDecoder);
 unsigned char (NEAACDECAPI *NeAACDecSetConfiguration)(NeAACDecHandle hDecoder,
                                                       NeAACDecConfigurationPtr config);

 NeAACDecHandle m_decHandle;
 mp4AudioSpecificConfig info;
 std::map<int,int> chmask;
 bool ps,sbr;
protected:
 virtual bool init(const CMediaType &mt);
 virtual void getInputDescr1(char_t *buf,size_t buflen) const;
public:
 TaudioCodecLibFAAD(IffdshowBase *deci,IdecAudioSink *Isink);
 virtual ~TaudioCodecLibFAAD();
 virtual int getType(void) const {return IDFF_MOVIE_LIBFAAD;}
 static const char_t *dllname;
 virtual HRESULT decode(TbyteBuffer &src);
 virtual bool onSeek(REFERENCE_TIME segmentStart);
};

#endif
