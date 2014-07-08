#ifndef _TVIDEOCODECTHEORA_H_
#define _TVIDEOCODECTHEORA_H_

#include "TvideoCodec.h"
#include "theora/theora.h"

class Tdll;
class TvideoCodecTheora :public TvideoCodecDec, public TvideoCodecEnc
{
private:
 void create(void);
 Tdll *dll;
 void (*theora_info_init)(theora_info *c);
 void (*theora_comment_init)(theora_comment *tc);
 void (*theora_comment_clear)(theora_comment *tc);
 int  (*theora_encode_comment)(theora_comment *tc, ogg_packet *op);
 int  (*theora_encode_tables)(theora_state *t, ogg_packet *op);
 int  (*theora_decode_init)(theora_state *th, theora_info *c);
 int  (*theora_decode_header)(theora_info *ci, theora_comment *cc, ogg_packet *op);
 int  (*theora_decode_packetin)(theora_state *th,ogg_packet *op);
 int  (*theora_decode_YUVout)(theora_state *th,yuv_buffer *yuv);
 int  (*theora_encode_init)(theora_state *th, theora_info *c);
 int  (*theora_encode_header)(theora_state *t, ogg_packet *op);
 int  (*theora_encode_YUVin)(theora_state *t, yuv_buffer *yuv, int *fixedq, int *keyframe);
 int  (*theora_encode_packetout)( theora_state *t, int last_p, ogg_packet *op);
 void (*theora_clear)(theora_state *th);
 void (*theora_info_clear)(theora_info *c);
 ogg_int64_t (*theora_granule_frame)(theora_state *th,ogg_int64_t granulepos);
 void (*set_cpu_flags)(int Icpu_flags);

 bool inited,firstframe,theorart;
 int cfgcomode;
 theora_info ti;
 theora_state td;
 theora_comment tc;
 unsigned char *extradata;size_t extradatalen;
 sTheoraFormatBlock ill;
 int64_t lastSeenStartGranPos,frameDuration;
 unsigned long frameCount;
 ogg_packet openc;
 Trect rd;
 REFERENCE_TIME segmentTimeStart;
protected:
 virtual LRESULT beginCompress(int cfgcomode,int csp,const Trect &r);
 virtual bool beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags);
public:
 TvideoCodecTheora(IffdshowBase *Ideci,IdecVideoSink *IsinkD);
 TvideoCodecTheora(IffdshowBase *Ideci,IencVideoSink *IsinkE);
 virtual ~TvideoCodecTheora();

 static const char_t *dllname;
 virtual int getType(void) const {return IDFF_MOVIE_THEO;}

 virtual void end(void);

 virtual bool supExtradata(void) {return true;}
 virtual bool getExtradata(const void* *ptr,size_t *len);
 virtual HRESULT compress(const TffPict &pict,TencFrameParams &params);

 virtual HRESULT decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn);
 virtual bool onSeek(REFERENCE_TIME segmentStart);
};

#endif
