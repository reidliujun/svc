#ifndef _TFFPROCAUDIO_H_
#define _TFFPROCAUDIO_H_

#include "IffProcAudio.h"
#include "TffProcBase.h"
#include "TaudioFilters.h"
#include "TsampleFormat.h"

struct IffdshowDecAideo;
struct TpresetAudio;
struct TglobalSettingsDecAideo;
struct TregOpIDstreamWrite;
class TffProcAudio :public CUnknown,
                    public TffProcBase,
                    public IffProcAudio,
                    public IprocAudioSink
{
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr);
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
 DECLARE_IUNKNOWN

 TffProcAudio(LPUNKNOWN punk,HRESULT *phr);
 virtual ~TffProcAudio();

 STDMETHODIMP setTempPreset(const char *tempPresetName) {return TffProcBase::setTempPreset(tempPresetName);}
 STDMETHODIMP setActivePreset(const char *presetName) {return TffProcBase::setActivePreset(presetName);}
 STDMETHODIMP loadPreset(const char *presetName) {return TffProcBase::loadPreset(presetName);}
 STDMETHODIMP saveBytestreamConfig(void *buf,size_t len) {return TffProcBase::saveBytestreamConfig(buf,len);}
 STDMETHODIMP loadBytestreamConfig(const void *buf,size_t len) {return TffProcBase::loadBytestreamConfig(buf,len);}
 STDMETHODIMP config(HWND owner) {return TffProcBase::config(owner);}
 STDMETHODIMP getIffDecoder_(/*IffdshowBase*/void* *deciPtr) {return TffProcBase::getIffDecoder_(deciPtr);}
 STDMETHODIMP putStringParam(const char *param,char sep) {return TffProcBase::putStringParam(param,sep);}
 STDMETHODIMP getParamsEnum(IffdshowParamsEnum* *enumPtr) {return TffProcBase::getParamsEnum(enumPtr);}

 //STDMETHODIMP getNewSize(unsigned int srcDx,unsigned int srcDy,unsigned int *outDx,unsigned int *outDy);
 STDMETHODIMP begin(const WAVEFORMATEX *wfIn,WAVEFORMATEX *wfOut);
 STDMETHODIMP process(const WAVEFORMATEX *wfIn,size_t innumsamples,WAVEFORMATEX *wfOut,size_t *outnumsamples,void* *samples);
 STDMETHODIMP processEx(const WAVEFORMATEXTENSIBLE *wfIn,size_t innumsamples,WAVEFORMATEXTENSIBLE *wfOut,size_t *outnumsamples,void* *samples);
 //STDMETHODIMP processTime(unsigned int framenum,int64_t ref_start,int64_t ref_stop,int incsp,const unsigned char *src[4],int srcStride[4],int ro,int outcsp,unsigned char *dst[4],int dstStride[4]);
 STDMETHODIMP end(void);
 // IprocAudioSink
 STDMETHODIMP deliverProcessedSample(const void *buf,size_t numsamples,const TsampleFormat &outsf);

 void saveConfig(TregOpIDstreamWrite &t);
 void loadConfig(bool notreg,const unsigned char *buf,size_t len);
private:
 comptr<IffdshowDecAudio> deciA;
 TaudioFilters *audioFilters;void initAudioFilters(void);
 TpresetAudio *presetSettings;
 TsampleFormat sfOut;
 int outnumsamples;
 const void *outsamples;
};

#endif
