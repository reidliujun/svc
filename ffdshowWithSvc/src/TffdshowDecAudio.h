#ifndef _TFFDSHOWDECAUDIO_H_
#define _TFFDSHOWDECAUDIO_H_

#include "TffdshowDec.h"
#include "IffdshowDecAudio.h"
#include "ffcodecs.h"
#include "TaudioFilter.h"
#include "TaudioFilters.h"
#include "TinputPin.h"

class TaudioCodec;
struct TglobalSettingsDecAudio;
struct TdialogSettingsDecAudio;
struct TpresetAudio;
class TpresetsAudio;
class Twinamp2;
class TffdshowDecAudioInputPin;
class TffdshowDecAudio :public TffdshowDec,
                        public interfaces<char_t>::IffdshowDecAudio,
                        public IprocAudioSink
{
public:
 static CUnknown *WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
 DECLARE_IUNKNOWN

 TffdshowDecAudio(CLSID Iclsid,const char_t *className,const CLSID &Iproppageid,int IcfgDlgCaptionId,int IiconId,LPUNKNOWN punk,HRESULT *phr,int Imode,int IdefaultMerit,TintStrColl *Ioptions);
 virtual ~TffdshowDecAudio();

 HRESULT Receive(IMediaSample* pIn);
 HRESULT CheckConnect(PIN_DIRECTION dir,IPin *pPin);
 HRESULT CheckInputType(const CMediaType *mtIn);
 HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
 HRESULT CompleteConnect(PIN_DIRECTION direction,IPin *pReceivePin);
 HRESULT DecideBufferSize(IMemAllocator *pAllocator, ALLOCATOR_PROPERTIES *pprop);
 HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
 HRESULT StartStreaming(void);
 virtual HRESULT NewSegment(REFERENCE_TIME tStart,REFERENCE_TIME tStop,double dRate);
 HRESULT BeginFlush(void);
 HRESULT EndFlush(void);
 HRESULT EndOfStream(void);

 virtual int GetPinCount();
 virtual CBasePin * GetPin(int n);
 STDMETHODIMP FindPin(LPCWSTR Id, IPin **ppPin);

 STDMETHODIMP_(int) getVersion2(void);
 STDMETHODIMP getMovieSource(const TaudioCodec* *moviePtr);
 STDMETHODIMP inputSampleFormatDescription(char_t *buf,size_t buflen);
 STDMETHODIMP deliverSample_(void *buf,size_t numsamples,const TsampleFormat &fmt,float postgain) {return E_NOTIMPL;}
 STDMETHODIMP getWinamp2(Twinamp2* *winamp2ptr);
 STDMETHODIMP getInCodecString(char_t *buf,size_t buflen);
 STDMETHODIMP getOutCodecString(char_t *buf,size_t buflen);
 STDMETHODIMP processBuffer(void *buf,size_t numsamples,TsampleFormat *fmt);
 STDMETHODIMP storeMixerMatrixData_(const double matrix[6][6]) {return E_NOTIMPL;}
 STDMETHODIMP getMixerMatrixData_(double matrix[6][6]) {return E_NOTIMPL;}
 STDMETHODIMP deliverSampleSPDIF(void *buf,size_t bufsize,int bit_rate,unsigned int sample_rate,BYTE type,int incRtDec);
 STDMETHODIMP storeVolumeData_(unsigned int nchannels,const int channels[],const int volumes[]) {return E_NOTIMPL;}
 STDMETHODIMP getVolumeData_(unsigned int *nchannels,int channels[],int volumes[]) {return E_NOTIMPL;}
 STDMETHODIMP storeFFTdata_(unsigned int num,const float *fft) {return E_NOTIMPL;}
 STDMETHODIMP_(int) getFFTdataNum_(void) {return E_NOTIMPL;}
 STDMETHODIMP getFFTdata_(float *fft) {return E_NOTIMPL;}
 STDMETHODIMP_(unsigned int) getNumStreams2(void);
 STDMETHODIMP getStreamDescr(unsigned int i,char_t *buf,size_t buflen);
 STDMETHODIMP_(unsigned int) getCurrentStream2(void);
 STDMETHODIMP setCurrentStream(unsigned int i);
 STDMETHODIMP setAudioFilters(TaudioFilters *audioFiltersPtr);
 STDMETHODIMP deliverProcessedSample(const void *buf,size_t numsamples,const TsampleFormat &outsf);
 STDMETHODIMP getEncoderInfo(char_t *buf,size_t buflen);
 STDMETHODIMP_(const char_t*) getDecoderName(void);
 STDMETHODIMP inputSampleFormat(unsigned int *nchannels,unsigned int *freq);
 STDMETHODIMP getOutSpeakersDescr(char_t *buf,size_t buflen,int shortcuts);
 STDMETHODIMP_(int) getInputBitrate2(void);
 STDMETHODIMP_(const TfilterIDFF*) getNextFilterIDFF(void) {return nextFilters;}
 STDMETHODIMP currentSampleFormat(unsigned int *nchannels,unsigned int *freq,int *sampleFormat);
 STDMETHODIMP_(int) getJitter(void);
 STDMETHODIMP Enable(long lIndex, DWORD dwFlags);
 STDMETHODIMP Count(DWORD *pcStreams);
 STDMETHODIMP Info(long lIndex, AM_MEDIA_TYPE** ppmt, DWORD* pdwFlags, LCID* plcid, DWORD* pdwGroup, WCHAR** ppszName, IUnknown** ppObject, IUnknown** ppUnk);
 STDMETHODIMP_(TffdshowDecAudioInputPin *) GetCurrentPin(void);
 STDMETHODIMP_(TinputPin*)getInputPin(void);

 static const AMOVIESETUP_MEDIATYPE inputMediaTypes[],outputMediaTypes[];
 static const AMOVIESETUP_PIN pins[];
 static const AMOVIESETUP_FILTER filter;
 static const DWORD defaultMerit;
private:
 struct TffdshowDecAudio_char : interfaces<tchar_traits<char_t>::other_char_t>::IffdshowDecAudio, CUnknown
  {
  protected:
   typedef tchar_traits<char_t>::other_char_t tchar;
  private:
   IffdshowDecAudioT<char_t> *deciA;
  public:
   TffdshowDecAudio_char(LPUNKNOWN punk,IffdshowDecAudioT<char_t> *IdeciA):deciA(IdeciA),CUnknown(NAME("TffdshowDecAudio_char"),punk) {}

   STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {return deciA->QueryInterface(riid,ppv);}
   STDMETHODIMP_(ULONG) AddRef() {return deciA->AddRef();}
   STDMETHODIMP_(ULONG) Release() {return deciA->Release();}

   STDMETHODIMP_(int) getVersion2(void) {return deciA->getVersion2();}
   STDMETHODIMP getMovieSource(const TaudioCodec* *moviePtr) {return deciA->getMovieSource(moviePtr);}
   STDMETHODIMP inputSampleFormatDescription(tchar *buf,size_t buflen)
    {
     char_t *buft=(char_t*)alloca(buflen*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deciA->inputSampleFormatDescription(buft,buflen)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP getWinamp2(Twinamp2* *winamp2ptr) {return deciA->getWinamp2(winamp2ptr);}
   STDMETHODIMP deliverSample_(void *buf,size_t numsamples,const TsampleFormat &fmt,float postgain) {return deciA->deliverSample_(buf,numsamples,fmt,postgain);}
   STDMETHODIMP storeMixerMatrixData_(const double matrix[6][6]) {return deciA->storeMixerMatrixData_(matrix);}
   STDMETHODIMP getMixerMatrixData_(double matrix[6][6]) {return deciA->getMixerMatrixData_(matrix);}
   STDMETHODIMP deliverSampleSPDIF(void *buf,size_t bufsize,int bit_rate,unsigned int sample_rate,BYTE type,int incRtDec) {return deciA->deliverSampleSPDIF(buf,bufsize,bit_rate,sample_rate,type,incRtDec);}
   STDMETHODIMP storeVolumeData_(unsigned int nchannels,const int channels[],const int volumes[]) {return deciA->storeVolumeData_(nchannels,channels,volumes);}
   STDMETHODIMP getVolumeData_(unsigned int *nchannels,int channels[],int volumes[]) {return deciA->getVolumeData_(nchannels,channels,volumes);}
   STDMETHODIMP storeFFTdata_(unsigned int num,const float *fft) {return deciA->storeFFTdata_(num,fft);}
   STDMETHODIMP_(int) getFFTdataNum_(void) {return deciA->getFFTdataNum_();}
   STDMETHODIMP getFFTdata_(float *fft) {return deciA->getFFTdata_(fft);}
   STDMETHODIMP_(unsigned int) getNumStreams2(void) {return deciA->getNumStreams2();}
   STDMETHODIMP getStreamDescr(unsigned int i,tchar *buf,size_t buflen)
    {
     char_t *buft=(char_t*)alloca(buflen*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deciA->getStreamDescr(i,buft,buflen)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP_(unsigned int) getCurrentStream2(void) {return deciA->getCurrentStream2();}
   STDMETHODIMP setCurrentStream(unsigned int i) {return deciA->setCurrentStream(i);}
   STDMETHODIMP_(TffdshowDecAudioInputPin *) GetCurrentPin(void)
	 {
		 return deciA->GetCurrentPin();
	 }
   STDMETHODIMP setAudioFilters(TaudioFilters *audioFiltersPtr)
    {
     return E_NOTIMPL;
    }
   STDMETHODIMP inputSampleFormat(unsigned int *nchannels,unsigned int *freq) {return deciA->inputSampleFormat(nchannels,freq);}
   STDMETHODIMP getOutSpeakersDescr(tchar *buf,size_t buflen,int shortcuts)
    {
     char_t *buft=(char_t*)alloca(buflen*sizeof(char_t));
     HRESULT hr;
     if (SUCCEEDED(hr=deciA->getOutSpeakersDescr(buft,buflen,shortcuts)))
      text<tchar>(buft,buf);
     return hr;
    }
   STDMETHODIMP currentSampleFormat(unsigned int *nchannels,unsigned int *freq,int *sampleFormat) {return deciA->currentSampleFormat(nchannels,freq,sampleFormat);}
   STDMETHODIMP_(int) getJitter(void) {return deciA->getJitter();}
   STDMETHODIMP_(TinputPin*) getInputPin(void){ return((TinputPin*)deciA->GetCurrentPin());};
  } decAudio_char;
 template<class Tinterface> Tinterface* getDecAudioInterface(void);
protected:
 virtual AM_MEDIA_TYPE* getInputMediaType(int lIndex);
 virtual void addOwnStreams(void);
 struct TstreamAudio :TffdshowDec::Tstream
  {
  private:
   char_t name[256];
   TffdshowDecAudio *self;
  public:
   TstreamAudio(TffdshowDecAudio *Iself,int Iorder,int Igroup,TffdshowDecAudioInputPin *IpPin):Tstream(Iself,Iorder,Igroup),pPin(IpPin),self(Iself) {}
   TffdshowDecAudioInputPin *pPin;
   virtual DWORD getFlags(void);
   virtual const char_t* getName(void);
   virtual bool action(void);
  };
 virtual bool isStreamsMenu(void) const;
 STDMETHODIMP_(int) get_trayIconType(void);
private:
 static const int VERSION=15;
 friend class TffdshowDecAudioInputPin;

 virtual HRESULT onGraphRemove(void);
 TglobalSettingsDecAudio *globalSettings;
 TdialogSettingsDecAudio *dialogSettings;
 TpresetsAudio *presets;
 TpresetAudio *presetSettings;
 virtual TinfoBase* createInfo(void);

 static const TfilterIDFF nextFilters[];

 TsampleFormat insf,currentOutsf;
 TaudioFilters *audioFilters;
 TsampleFormat getOutsf(void);

 Twinamp2 *winamp2;
 void onWinamp2dirChange(int paramID,const char_t *valname);

 CodecID getCodecId(const CMediaType &mtIn);
 HRESULT ReconnectOutput(size_t numsamples, CMediaType& mt);
 REFERENCE_TIME m_rtStartDec,m_rtStartProc;
 HRESULT getMediaType(CMediaType *mtOut);
 STDMETHODIMP deliverDecodedSample(const TffdshowDecAudioInputPin *pin,void *buf,size_t numsamples,const TsampleFormat &fmt,float postgain);
 HRESULT flushDecodedSamples(const TffdshowDecAudioInputPin *pin,float postgain);
 ALLOCATOR_PROPERTIES actual;

 bool isAudioSwitcher;
 TpinsVector<TffdshowDecAudioInputPin> inpins;
 TffdshowDecAudioInputPin *inpin;
 STDMETHODIMP setCurrentStream2(TffdshowDecAudioInputPin *newipin);
 bool alwaysextensible,allowOutStream;
 bool fileout;
 bool isTmpgEnc;
 REFERENCE_TIME ft1,ft2;
 float prevpostgain;
 REFERENCE_TIME priorFrameMsgTime;
 bool audioDeviceChanged;
};

class TffdshowDecAudioRaw : public TffdshowDecAudio
{
private:
 TffdshowDecAudioRaw(LPUNKNOWN lpunk,HRESULT *phr);
public:
 static CUnknown *WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

 static const AMOVIESETUP_MEDIATYPE inputMediaTypes[],outputMediaTypes[];
 static const AMOVIESETUP_PIN pins[];
 static const AMOVIESETUP_FILTER filter;
 static const DWORD defaultMerit;
};

#endif
