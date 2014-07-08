#ifndef _TAUDIOFILTERRESAMPLE_H_
#define _TAUDIOFILTERRESAMPLE_H_

#include "TaudioFilter.h"
#include "TresampleSettings.h"

DECLARE_FILTER(TaudioFilterResampleBase,public,TaudioFilter)
protected:
 TresampleSettings old;unsigned int oldfreq,oldnchannels;
 Tbuffer buf;
 virtual bool is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 TaudioFilterResampleBase(IffdshowBase *Ideci,Tfilters *Iparent);
public:
 virtual bool getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
};

template<class sample_t> struct TreSampleContext;
DECLARE_FILTER(TaudioFilterResampleLavc,public,TaudioFilterResampleBase)
private:
 TreSampleContext<int16_t>* ctxsInt[8];
 TreSampleContext<float>* ctxsFloat[8];
 Tbuffer buffIn;
 Tbuffer buffOut;
 int oldsf;
 template<class sample_t,class Tctx> void resample(TsampleFormat &fmt,sample_t* &samples,size_t &numsamples,const TresampleSettings *cfg,Tctx ctxs[]);
protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_PCM16|TsampleFormat::SF_FLOAT32;}
public:
 TaudioFilterResampleLavc(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void done(void);
 virtual void onSeek(void);
};

struct SRC_DATA;
class Tdll;
DECLARE_FILTER(TaudioFilterResampleSRC,public,TaudioFilterResampleBase)
private:
 Tdll *dll;
 void* (*src_new)(int converter_type, int channels, int *error);
 void* (*src_delete)(void *state);
 int (*src_process)(void *state, SRC_DATA *data);
 int (*src_reset)(void *state);
 void *state;
protected:
 virtual bool is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_FLOAT32;}
public:
 static const char_t *dllname;
 TaudioFilterResampleSRC(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TaudioFilterResampleSRC();
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void done(void);
 virtual void onSeek(void);
};

#endif
