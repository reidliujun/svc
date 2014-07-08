#ifndef _TAUDIOFILTERS_H_
#define _TAUDIOFILTERS_H_

#include "Tfilters.h"

struct TsampleFormat;
DECLARE_INTERFACE(IprocAudioSink)
{
 STDMETHOD (deliverProcessedSample)(const void *buf,size_t numsamples,const TsampleFormat &outsf) PURE;
};

class TaudioFilterVolume;
struct TpresetAudio;
struct TvolumeSettings;
class TaudioFilters :public Tfilters
{
private:
 comptrQ<IffdshowDecAudio> deciA;
 IprocAudioSink *sink;
 TvolumeSettings *postgainCfg;TaudioFilterVolume *postgainVol;
 Tfilter *output;
public:
 TaudioFilters(IffdshowBase *Ideci,IprocAudioSink *Isink);
 virtual ~TaudioFilters();
 void getOutputFmt(TsampleFormat &fmt,const TpresetAudio *cfg);
 HRESULT process(const TsampleFormat &fmt,void *samples,size_t numsamples,const TpresetAudio *cfg,float postgain);
 HRESULT deliverSamples(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples);
 int preferredsfs;
};

struct TglobalSettingsDecAudio;
class TaudioFiltersPlayer : public TaudioFilters
{
private:
 const TglobalSettingsDecAudio *globalCfg;
 Tfilter *osd;
 int outsfs;
protected:
 virtual void makeQueue(const Tpreset *cfg,TfilterQueue &queue);
public:
 TaudioFiltersPlayer(IffdshowBase *Ideci,IprocAudioSink *Isink,TpresetAudio *presetSettings);
};

#endif
