#ifndef _TAUDIOCODECTREMOR_H_
#define _TAUDIOCODECTREMOR_H_

#include "TaudioCodec.h"
#include "tremor/ivorbiscodec.h"

class Tdll;
class TaudioCodecTremor :public TaudioCodec
{
private:
 Tdll *dll;
 void (*vorbis_info_init)(vorbis_info *vi);
 void (*vorbis_comment_init)(vorbis_comment *vc);
 int  (*vorbis_synthesis_headerin)(vorbis_info *vi,vorbis_comment *vc,ogg_packet *op);
 int  (*vorbis_synthesis_init)(vorbis_dsp_state *v,vorbis_info *vi);
 int  (*vorbis_block_init)(vorbis_dsp_state *v, vorbis_block *vb);
 int  (*vorbis_synthesis)(vorbis_block *vb,ogg_packet *op,int decodep);
 int  (*vorbis_synthesis_blockin)(vorbis_dsp_state *v,vorbis_block *vb);
 int  (*vorbis_synthesis_pcmout)(vorbis_dsp_state *v,ogg_int32_t ***pcm);
 int  (*vorbis_synthesis_read)(vorbis_dsp_state *v,int samples);
 void (*vorbis_info_clear)(vorbis_info *vi);
 void (*vorbis_comment_clear)(vorbis_comment *vc);
 int  (*vorbis_block_clear)(vorbis_block *vb);
 void (*vorbis_dsp_clear)(vorbis_dsp_state *v);
 char*(*vorbis_comment_query)(vorbis_comment *vc, char *tag, int count);
 int  (*vorbis_comment_query_count)(vorbis_comment *vc, char *tag);

 bool inited,oggds;
 int m_packetno;
 vorbis_info vi;
 vorbis_comment vc;
 vorbis_block vb;
 vorbis_dsp_state vd;
 static const int MAXCHANNELS=6;
 static const int chmap[MAXCHANNELS][MAXCHANNELS];
 unsigned char *pbVorbisInfo;int cbVorbisInfo;
 unsigned char *pbVorbisComment;int cbVorbisComment;
 unsigned char *pbVorbisCodebook;int cbVorbisCodebook;
 bool initVorbis(void);
 void clearVorbis(bool all);
 float postgain;int isGain;
protected:
 virtual bool init(const CMediaType &mt);
 virtual void getInputDescr1(char_t *buf,size_t buflen) const;
public:
 TaudioCodecTremor(IffdshowBase *deci,IdecAudioSink *Isink);
 virtual ~TaudioCodecTremor();
 virtual int getType(void) const {return IDFF_MOVIE_TREMOR;}
 static const char_t *dllname;
 virtual HRESULT decode(TbyteBuffer &src);
 virtual bool onSeek(REFERENCE_TIME segmentStart);
};

#endif
