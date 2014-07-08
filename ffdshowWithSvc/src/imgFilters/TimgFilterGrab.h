#ifndef _TIMGFILTERGRAB_H_
#define _TIMGFILTERGRAB_H_

#include "TimgFilter.h"
#include "ffcodecs.h"
#include "Tlibavcodec.h"
#include "IimgFilterGrab.h"
#include "TffPict.h"

class Tdll;
struct Tconfig;
struct Tlibavcodec;
struct AVCodecContext;
struct AVFrame;
class TimgFilterGrab : public TimgFilter, public IimgFilterGrab
_DECLARE_FILTER(TimgFilterGrab,TimgFilter)
private:
 struct TimgExport
  {
  public:
   bool ok,inited;
   TimgExport(void):ok(false),inited(false) {}
   virtual ~TimgExport() {}
   virtual void init(unsigned int dx,unsigned int dy)=0;
   virtual int requiredCSP()=0;
   virtual int compress(const unsigned char *src[4],stride_t stride[4],unsigned char *dst,unsigned int dstlen,int qual)=0;
   virtual void done(void) {inited=false;}
  };
 struct TimgExportLibavcodec :public TimgExport
  {
  private:
   Tlibavcodec *dll;
   AVCodecContext *avctx;
   AVFrame *picture;
   CodecID codecId;
   bool avctxinited;
  protected:
   virtual int setQual(int qual)=0;
  public:
   TimgExportLibavcodec(const Tconfig *config,IffdshowBase *deci,CodecID IcodecId);
   virtual ~TimgExportLibavcodec();
   virtual void init(unsigned int dx,unsigned int dy);
   virtual int compress(const unsigned char *src[4],stride_t stride[4],unsigned char *dst,unsigned int dstlen,int qual);
   virtual void done(void);
  };
 struct TimgExportJPEG :public TimgExportLibavcodec
  {
  protected:
   virtual int setQual(int qual)
    {
     return int(FF_QP2LAMBDA*(30.0f*(100-qual)/100+1));
    }
  public:
   TimgExportJPEG(const Tconfig *config,IffdshowBase *deci):TimgExportLibavcodec(config,deci,CODEC_ID_MJPEG) {}
   virtual int requiredCSP() {return FF_CSP_420P|FF_CSP_FLAGS_YUV_JPEG;}
  };
 struct TimgExportPNG :public TimgExportLibavcodec
  {
   virtual int setQual(int qual)
    {
    	/* FFmpeg uses lossless PNG compression, we therefore won't use the
    	 * quality parameter. Instead we use a fixed compression level that
    	 * has a good balance between output file size and compression performance.
    	 */
     return 3;
    }
  public:
   // We are interesting in writing RGB24 (as required by PNG), so we have to select FF_CSP_BGR24.
   // see the comment above the FF_CSP_ enum definition.
   TimgExportPNG(const Tconfig *config,IffdshowBase *deci):TimgExportLibavcodec(config,deci,CODEC_ID_PNG) {}
   virtual int requiredCSP() {return FF_CSP_BGR24;}
  };
 struct TimgExportBMP :public TimgExport
  {
  private:
   BITMAPFILEHEADER bfh;
   BITMAPCOREHEADER bch;
  public:
   TimgExportBMP(void);
   virtual void init(unsigned int dx,unsigned int dy);
   // We are interesting in writing BGR24 (as required for BMP), so we have to select FF_CSP_RGB24.
   // see the comment above the FF_CSP_ enum definition.
   virtual int requiredCSP() {return FF_CSP_RGB24|FF_CSP_FLAGS_VFLIP;}
   virtual int compress(const unsigned char *src[4],stride_t stride[4],unsigned char *dst,unsigned int dstlen,int qual);
  };
 TimgExport *exp[3];
 unsigned char *dstbuf;unsigned int dstbuflen;
 volatile LONG now;
 TffPict temp;
 Tbuffer buffer;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const;
 virtual void onSizeChange(void);
public:
 TimgFilterGrab(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterGrab();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg);

 virtual HRESULT queryInterface(const IID &iid,void **ptr) const;
 STDMETHODIMP grabNow(void);
 virtual bool acceptRandomYV12andRGB32(void) {return true;}
};

#endif
