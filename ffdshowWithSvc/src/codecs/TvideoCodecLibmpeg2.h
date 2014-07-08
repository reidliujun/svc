#ifndef _TVIDEOCODECLIBMPEG2_H_
#define _TVIDEOCODECLIBMPEG2_H_

#include "TvideoCodec.h"
#include "libmpeg2/include/mpeg2.h"

class Tdll;
struct Textradata;
class TccDecoder;
class TvideoCodecLibmpeg2 :public TvideoCodecDec
{
private:
 Tdll *dll;
 uint32_t (*mpeg2_accel)(uint32_t accel);
 mpeg2dec_t* (*mpeg2_init)(void);
 const mpeg2_info_t* (*mpeg2_info)(mpeg2dec_t *mpeg2dec);
 void (*mpeg2_set_accel)(int accel);
 mpeg2_state_t (*mpeg2_parse )(mpeg2dec_t *mpeg2dec);
 void (*mpeg2_buffer)(mpeg2dec_t *mpeg2dec, const uint8_t *start, const uint8_t *end);
 void (*mpeg2_close)(mpeg2dec_t *mpeg2dec);
 void (*mpeg2_reset)(mpeg2dec_t *mpeg2dec, int full_reset);
 void (*mpeg2_set_rtStart)(mpeg2dec_t *mpeg2dec,int64_t rtStart);
 int (*mpeg2_guess_aspect)(const mpeg2_sequence_t * sequence,unsigned int * pixel_width,unsigned int * pixel_height);

 mpeg2dec_t *mpeg2dec;
 const mpeg2_info_t *info;
 bool wait4Iframe;
 int sequenceFlag;
 REFERENCE_TIME avgTimePerFrame;
 TffPict oldpict;
 Textradata *extradata;
 TccDecoder *ccDecoder;
 void init(void);
 Tbuffer *buffer;
 HRESULT decompressI(const unsigned char *src,size_t srcLen,IMediaSample *pIn);
 uint32_t oldflags;
 bool m_fFilm;
 int SetDeinterlaceMethod(void);

protected:
 virtual bool beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags);

public:
 TvideoCodecLibmpeg2(IffdshowBase *Ideci,IdecVideoSink *Isink);
 virtual ~TvideoCodecLibmpeg2();

 static const char_t *dllname;
 virtual int getType(void) const {return IDFF_MOVIE_LIBMPEG2;}
 virtual int caps(void) const {return CAPS::VIS_QUANTS;}

 virtual void end(void);
 virtual HRESULT decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn);
 virtual bool onSeek(REFERENCE_TIME segmentStart);
};

#endif
