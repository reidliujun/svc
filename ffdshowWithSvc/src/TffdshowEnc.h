#ifndef _TFFDSHOWENC_H_
#define _TFFDSHOWENC_H_

#include "TcodecSettings.h"
#include "TvideoCodec.h"
#include "TffdshowBase.h"
#include "TffdshowVideo.h"
#include "IffdshowEnc.h"
#include "dsutil.h"
#include "interfaces.h"
#include "TffProc.h"
#include "circular_buffer.h"

class TffdshowVideoInputPin;
struct TglobalSettingsEnc;
struct TdialogSettingsEnc;
class TffProcVideo;
class Tmuxer;
struct Txvid_2pass;
class TffdshowEnc :public TffdshowBase,
                   public CmyTransformFilter,
                   public TffdshowVideo,
                   public IffdshowEnc,public IffdshowEncVFW,
                   public ISpecifyPropertyPages,
                   public CPersistStream,
                   public IencVideoSink
{
public:
 DECLARE_IUNKNOWN
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);

 TffdshowEnc(LPUNKNOWN punk,HRESULT *phr,TintStrColl *Ioptions,const char_t *Iname,REFCLSID Iclsid,int Imode);
 virtual ~TffdshowEnc();

 HRESULT CheckInputType(const CMediaType *mtIn);
 HRESULT GetMediaType(int iPos,CMediaType *pmt);
 HRESULT SetMediaType(PIN_DIRECTION direction,const CMediaType *pmt);
 HRESULT CheckConnect(PIN_DIRECTION dir,IPin *pPin);
 HRESULT CheckTransform(const CMediaType *mtIn,const CMediaType *mtOut);
 HRESULT DecideBufferSize(IMemAllocator *pima,ALLOCATOR_PROPERTIES *pProperties);
 STDMETHODIMP JoinFilterGraph(IFilterGraph *pGraph,LPCWSTR pName);
 HRESULT EndOfStream(void);

 STDMETHODIMP Stop(void);
 //HRESULT Transform(IMediaSample *pIn,IMediaSample *pOut);

 STDMETHODIMP GetPages(CAUUID *pPages);

 STDMETHODIMP GetClassID(CLSID *pClsid);
 virtual int SizeMax(void);
 virtual HRESULT WriteToStream(IStream *pStream);
 virtual HRESULT ReadFromStream(IStream *pStream);

 STDMETHODIMP_(int) getVersion2(void);
 STDMETHODIMP getEncStats(TencStats* *encStatsPtr);
 STDMETHODIMP getFFproc(TffProcVideo* *procPtr);
 STDMETHODIMP isLAVCadaptiveQuant(void);
 STDMETHODIMP isQuantControlActive(void);
 STDMETHODIMP getCustomQuantMatrixes(unsigned char* *intra8,unsigned char* *inter8,unsigned char* *intra4Y,unsigned char* *inter4Y,unsigned char* *intra4C,unsigned char* *inter4C);
 STDMETHODIMP getEncoder(int codecId,const Tencoder* *encPtr);
 STDMETHODIMP_(int) getQuantType2(int quant);
 STDMETHODIMP getCoSettingsPtr(const TcoSettings* *coSettingsPtr);
 STDMETHODIMP setCoSettingsPtr(TcoSettings *coSettingsPtr);
 STDMETHODIMP loadEncodingSettings(void);
 STDMETHODIMP saveEncodingSettings(void);
 STDMETHODIMP_(int) loadEncodingSettingsMem(const void *buf,int len);
 STDMETHODIMP_(int) saveEncodingSettingsMem(void *buf,int len);
 STDMETHODIMP showGraph(HWND parent) {return E_NOTIMPL;}
 STDMETHODIMP getVideoCodecs(const TvideoCodecs* *codecs);
 STDMETHODIMP getCoPtr(Tco* *coPtr) {return E_NOTIMPL;}
 STDMETHODIMP _release(void);
 STDMETHODIMP muxHeader(const void *data,size_t datalen,int flush);
 STDMETHODIMP_(void) setHgraph(HWND hwnd) {h_graph=hwnd;}
 STDMETHODIMP_(HWND) getHgraph(void) {return h_graph;}
 STDMETHODIMP getInCodecString(char_t *buf,size_t buflen);
 STDMETHODIMP getOutCodecString(char_t *buf,size_t buflen);

 //TffdshowVideo
 virtual int getVideoCodecId(const BITMAPINFOHEADER *hdr,const GUID *subtype,FOURCC *AVIfourcc);
 virtual void initCodecSettings(void) {}
 virtual void lockReceive(void),unlockReceive(void);
protected:
 virtual HRESULT onGraphRemove(void);
 virtual void getColls(TintStrColls &colls);
 virtual TinfoBase* createInfo(void);
 STDMETHODIMP deliverEncodedSample(const TmediaSample &sample,TencFrameParams &params);
//private:
 static const int VERSION=11;

 TglobalSettingsEnc *globalSettings;
 TdialogSettingsEnc *dialogSettings;int dlgEncGraph,dlgEncAbout;
 virtual HRESULT onInitDialog(void);
 virtual void onTrayIconChange(int id,int newval);
 virtual void sendOnChange(int paramID,int val);
 bool started,start(void);

 TvideoCodecs enclibs;
 uint64_t totalsize;
 TvideoCodecEnc *enc;CodecID oldCodecId;
 TvideoCodecEnc *findEncLib(void);
 Textradata extradata;
 Tmuxer *mux;

 struct TffProcVideoEnc : TffProcVideo
  {
  private:
   STDMETHODIMP deliverProcessedSample(TffPict &pict);
   TffdshowEnc *self;
  public:
   TffProcVideoEnc(LPUNKNOWN punk,HRESULT *phr,TffdshowEnc *Iself):TffProcVideo(punk,phr),self(Iself) {}
  } *ffproc;
 void getFFproc(void);
 TffPict pict,ownpict;Tbuffer ownpictbuf;
 unsigned int dx,dy;
 unsigned int outDx,outDy;
 Tcsps enccsps;int enccsp;
 Tconvert *convert;
 int keyspacing;
 int getQuantQuant(void);

 int cfgcomode;
 Txvid_2pass *pass;
 int getQuantFirst(void);
 int outputdebug,outputdebugfile;
 bool firstrun;

 HWND h_graph;
 int inColorspace;
 BITMAPINFO biOutput;
 TencStats encStats;

 int working;
 TencFrameParams params;
 TcoSettings *coSettings;
 void initCo(void);
 void getOut(unsigned int AVIdx,unsigned int AVIdy,unsigned int *outDx,unsigned int *outDy);

 STDMETHODIMP_(LRESULT) query(const BITMAPINFOHEADER *inhdr,BITMAPINFOHEADER *outhdr);
 STDMETHODIMP_(LRESULT) getFormat(const BITMAPINFOHEADER *inhdr,BITMAPINFO *lpbiOutput);
 STDMETHODIMP_(LRESULT) getSize(const BITMAPINFO *lpbiInput);
 STDMETHODIMP_(LRESULT) begin(const BITMAPINFOHEADER *inhdr);
 STDMETHODIMP_(LRESULT) end(void);
 STDMETHODIMP_(LRESULT) compress(const BITMAPINFOHEADER *inhdr,const uint8_t *src,size_t srclen,REFERENCE_TIME rtStart,REFERENCE_TIME rtStop);
 STDMETHODIMP_(void) setICC(void *icc0) {}

 int fpsRate,fpsScale;
 int ownStoreExt;

 TffdshowVideoInputPin *inpin;
 REFERENCE_TIME ft1,ft2;
 bool fileout;
 STDMETHODIMP_(int) get_trayIconType(void);
public:
 static const AMOVIESETUP_MEDIATYPE inputMediaTypes[],outputMediaTypes[];
 static const AMOVIESETUP_PIN pins[];
 static const DWORD defaultMerit;
};

class TffdshowEncDshow : public TffdshowEnc
{
private:
 typedef std::pair<REFERENCE_TIME,REFERENCE_TIME> Ttimes;
 circular_buffer<Ttimes,array_allocator<Ttimes,16> > times;
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr);
 TffdshowEncDshow(LPUNKNOWN punk,HRESULT *phr,TintStrColl *Ioptions):TffdshowEnc(punk,phr,Ioptions,NAME("TffdshowEnc"),CLSID_FFDSHOWENC,IDFF_FILTERMODE_PLAYER|IDFF_FILTERMODE_ENC),times(16) {}

 STDMETHODIMP_(LRESULT) begin(const BITMAPINFOHEADER *inhdr);
 virtual HRESULT Receive(IMediaSample *pSample);

 //IencVideoSink
 STDMETHODIMP getDstBuffer(IMediaSample* *samplePtr,const TffPict &pict);
 STDMETHODIMP deliverEncodedSample(const TmediaSample &sample,TencFrameParams &params);
 STDMETHODIMP deliverError(void);
};

class TffdshowEncVFW : public TffdshowEnc
{
private:
 ICCOMPRESS *icc;
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr);
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
 TffdshowEncVFW(LPUNKNOWN punk,HRESULT *phr,TintStrColl *Ioptions):TffdshowEnc(punk,phr,Ioptions,NAME("TffdshowEncVFW"),CLSID_FFDSHOWENCVFW,IDFF_FILTERMODE_PLAYER|IDFF_FILTERMODE_ENC|IDFF_FILTERMODE_VFW) {}

 STDMETHODIMP getDstBuffer(IMediaSample* *samplePtr,const TffPict &pict);
 STDMETHODIMP deliverEncodedSample(const TmediaSample &sample,TencFrameParams &params);
 STDMETHODIMP deliverError(void);

 STDMETHODIMP_(void) setICC(void *icc0) {icc=(ICCOMPRESS*)icc0;}
};

#endif
