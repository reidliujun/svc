#ifndef _TVIDEOCODECLIBAVCODEC_H_
#define _TVIDEOCODECLIBAVCODEC_H_

#include "TvideoCodec.h"
#include "Tlibavcodec.h"

struct Textradata;
class TccDecoder;
class TvideoCodecLibavcodec :public TvideoCodecDec,public TvideoCodecEnc,public TlibavcodecExt
{
private:
 void create(void);
 void createSVC(void);
 Tlibavcodec *libavcodec;
 AVCodec *avcodec;mutable char_t codecName[100];
 AVCodecContext *avctx;
 AVPaletteControl pal;
 AVFrame *frame;
 FOURCC fcc;
 FILE *statsfile;
 int cfgcomode;
 int psnr;
 int svc;
 int grayscale;
 bool isAdaptive;
 int threadcount;
 bool dont_use_rtStop_from_upper_stream; // and reordering of timpestams is justified.
 bool theorart;
 bool codecinited,ownmatrices;
 REFERENCE_TIME rtStart,rtStop,avgTimePerFrame,segmentTimeStart;

 struct
  {
   REFERENCE_TIME rtStart,rtStop;
   unsigned int srcSize;
   } b[2];
 int posB;

 Textradata *extradata;bool sendextradata;
 TffPict oldpict;
 unsigned int mb_width,mb_height,mb_count;
 static void line(unsigned char *dst,unsigned int _x0,unsigned int _y0,unsigned int _x1,unsigned int _y1,stride_t strideY);
 static void draw_arrow(uint8_t *buf, int sx, int sy, int ex, int ey, stride_t stride,int mulx,int muly,int dstdx,int dstdy);
 unsigned char *ffbuf;unsigned int ffbuflen;
 bool wasKey;
 virtual void handle_user_data(const uint8_t *buf,int buf_len);
 TccDecoder *ccDecoder;
 bool autoSkipingLoopFilter;
 enum AVDiscard initialSkipLoopFilter;
 bool h264_has_start_code; // H.264/AVC on mpeg2 trasport stream. Program stream is handled in the same way.
 bool isTSfile(void);
protected:
 virtual LRESULT beginCompress(int cfgcomode,int csp,const Trect &r);
 virtual bool beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags);
 virtual HRESULT flushDec(void);
public:
 TvideoCodecLibavcodec(IffdshowBase *Ideci,IdecVideoSink *IsinkD, int SVC);
 TvideoCodecLibavcodec(IffdshowBase *Ideci,IencVideoSink *IsinkE);
 virtual ~TvideoCodecLibavcodec();

 virtual int getType(void) const {return IDFF_MOVIE_LAVC;}
 virtual const char_t* getName(void) const;
 virtual int caps(void) const {return CAPS::VIS_MV|CAPS::VIS_QUANTS;}

 virtual void end(void);

 virtual void getCompressColorspaces(Tcsps &csps,unsigned int outDx,unsigned int outDy);
 virtual bool supExtradata(void);
 virtual bool getExtradata(const void* *ptr,size_t *len);
 virtual HRESULT compress(const TffPict &pict,TencFrameParams &params);
 virtual HRESULT flushEnc(const TffPict &pict,TencFrameParams &params) {return compress(pict,params);}

 virtual HRESULT decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn);
 virtual void onGetBuffer(AVFrame *pic);
 virtual bool onSeek(REFERENCE_TIME segmentStart);
 virtual bool onDiscontinuity(void);
 virtual bool drawMV(unsigned char *dst,unsigned int dx,stride_t stride,unsigned int dy) const;
 virtual void getEncoderInfo(char_t *buf,size_t buflen) const;
 virtual const char* get_current_idct(void);
 class TcodedPictureBuffer;
 friend class TcodedPictureBuffer;
 class TcodedPictureBuffer
  {
  private:
   Tbuffer priorBuf,outBuf,tmpBuf; 
   int priorSize,outSampleSize,used_bytes;
   TvideoCodecLibavcodec* parent;
   REFERENCE_TIME prior_rtStart,prior_rtStop,out_rtStart,out_rtStop;

  public:
   TcodedPictureBuffer(TvideoCodecLibavcodec* Iparent);
   void init(void);
   int append(const uint8_t *buf, int buf_size);
   int send(int *got_picture_ptr);
   void onSeek(void);
  } codedPictureBuffer;
 class Th264RandomAccess
  {
  friend class TvideoCodecLibavcodec;
  private:
   TvideoCodecLibavcodec* parent;
   int recovery_mode;  // 0:OK, 1:searching 2: found, 3:waiting for frame_num decoded, 4:waiting for POC outputed
   int recovery_frame_cnt;
   int recovery_poc;

  public:
   Th264RandomAccess(TvideoCodecLibavcodec* Iparent);
   int search(uint8_t* buf, int buf_size);
   void onSeek(void);
   void judgeUsability(int *got_picture_ptr);
  } h264RandomAccess;
};

#endif
