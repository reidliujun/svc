#ifndef _TVIDEOCODEC_H_
#define _TVIDEOCODEC_H_

#include "Tcodec.h"
#include "TffPict.h"
#include "ffdshow_constants.h"
#include "ffImgfmt.h"
#include "TencStats.h"
#include "interfaces.h"

DECLARE_INTERFACE_(IdecVideoSink,IdecSink)
{
 STDMETHOD (deliverDecodedSample)(TffPict &pict) PURE;
 STDMETHOD (deliverPreroll)(int frametype) PURE;
 STDMETHOD (acceptsManyFrames)(void) PURE;
};

class TvideoCodec :virtual public Tcodec
{
public:
 TvideoCodec(IffdshowBase *Ideci);
 virtual ~TvideoCodec();
 bool ok;
 int connectedSplitter;
 bool isInterlacedRawVideo;
 Rational containerSar;

 struct CAPS
  {
   enum
    {
     NONE=0,
     VIS_MV=1,
     VIS_QUANTS=2
    };
  };

 virtual void end(void) {}
};

class TvideoCodecDec :virtual public TvideoCodec,virtual public TcodecDec
{
protected:
 comptrQ<IffdshowDecVideo> deciV;
 IdecVideoSink *sinkD;
 TvideoCodecDec(IffdshowBase *Ideci,IdecVideoSink *Isink);
public:
 static TvideoCodecDec* initDec(IffdshowBase *deci,IdecVideoSink *Isink,CodecID codecId,FOURCC fcc,const CMediaType &mt);

 virtual ~TvideoCodecDec();

 virtual int caps(void) const {return CAPS::NONE;}
 virtual bool testMediaType(FOURCC fcc,const CMediaType &mt) {return true;}
 virtual void forceOutputColorspace(const BITMAPINFOHEADER *hdr,int *ilace,TcspInfos &forcedCsps) {*ilace=0;} //cspInfos of forced output colorspace, empty when entering function
 enum {SOURCE_NEROAVC=1,SOURCE_TRUNCATED=2};
 virtual bool beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags)=0;
 virtual HRESULT decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn)=0;
 virtual bool onDiscontinuity(void) {return false;}
 virtual HRESULT onEndOfStream(void) {return S_OK;}

 unsigned int quantsDx,quantsStride,quantsDy,quantBytes,quantType;
 void *quants;
 uint16_t *intra_matrix,*inter_matrix;

 float calcMeanQuant(void);
 virtual bool drawMV(unsigned char *dst,unsigned int dx,stride_t stride,unsigned int dy) const {return false;}
 virtual const char* get_current_idct(void){return NULL;}
};

typedef vectorEx<int,array_allocator<int,FF_CSPS_NUM> > Tcsps;
typedef vectorEx<FOURCC> Tfourccs;

struct Tencoder
{
 Tencoder(const char_t *Iname,CodecID Iid,const Tfourccs &Ifourccs):name(Iname),id(Iid),fourccs(Ifourccs) {}
 Tencoder(const char_t *Iname,CodecID Iid):name(Iname),id(Iid)
  {
   const FOURCC *fccs=getCodecFOURCCs(id);
   for (const FOURCC *fcc=fccs;*fcc;fcc++)
    fourccs.push_back(*fcc);
  }

 const char_t *name;
 CodecID id;
 Tfourccs fourccs;
};

struct Tencoders :public std::vector<Tencoder*>
{
 ~Tencoders()
  {
   for (iterator e=begin();e!=end();e++)
    delete *e;
  }
};

struct TmediaSample
{
private:
 IMediaSample *sample;
public:
 TmediaSample(void):sample(NULL) {}
 TmediaSample(IMediaSample *Isample):sample(Isample) {}
 ~TmediaSample() {if (sample) sample->Release();}
 IMediaSample** operator &(void) {return &sample;}
 IMediaSample* operator ->(void) const {return sample;}
 template<class T> operator T* (void)
  {
   BYTE *ptr;
   return sample && sample->GetPointer(&ptr)==S_OK?ptr:NULL;
  }
 template<class T> operator const T* (void) const
  {
   BYTE *ptr;
   return sample && sample->GetPointer(&ptr)==S_OK?ptr:NULL;
  }
 long size(void) {return sample?sample->GetSize():0;}
};

DECLARE_INTERFACE(IencVideoSink)
{
 STDMETHOD (getDstBuffer)(IMediaSample* *samplePtr,const TffPict &pict) PURE;
 STDMETHOD (deliverEncodedSample)(const TmediaSample &sample,TencFrameParams &params) PURE;
 STDMETHOD (deliverError)(void) PURE;
};

struct IffdshowEnc;
struct TcoSettings;
class TvideoCodecEnc :virtual public TvideoCodec
{
protected:
 comptrQ<IffdshowEnc> deciE;
 IencVideoSink *sinkE;
 const TcoSettings *coCfg;
public:
 TvideoCodecEnc(IffdshowBase *Ideci,IencVideoSink *Isink);
 virtual ~TvideoCodecEnc();
 Tencoders encoders;
 void setCoSettings(CodecID IcodecId) {codecId=IcodecId;}
 virtual void getCompressColorspaces(Tcsps &csps,unsigned int outDx,unsigned int outDy) {csps.add(FF_CSP_420P);}
 virtual bool prepareHeader(BITMAPINFOHEADER *outhdr) {return false;}
 virtual LRESULT beginCompress(int cfgcomode,int csp,const Trect &r)=0;
 virtual bool supExtradata(void) {return false;}
 virtual bool getExtradata(const void* *ptr,size_t *len);
 virtual HRESULT compress(const TffPict &pict,TencFrameParams &params)=0;
 virtual HRESULT flushEnc(const TffPict &pict,TencFrameParams &params) {return S_OK;}
};

struct TvideoCodecs :public std::vector<TvideoCodecEnc*>
{
 void init(IffdshowBase *deci,IencVideoSink *sink);
 TvideoCodecEnc* getEncLib(int codecId);
 const Tencoder* getEncoder(int codecId) const;
};

#endif
