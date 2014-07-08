#ifndef _TAUDIOFILTERWINAMP2_H_
#define _TAUDIOFILTERWINAMP2_H_

#include "TaudioFilter.h"
#include "Twinamp2settings.h"

class Twinamp2;
class Twinamp2dsp;
DECLARE_FILTER(TaudioFilterWinamp2,public,TaudioFilter)
private:
 Twinamp2settings old;
 Twinamp2dsp *filter;
 Twinamp2 *winamp2;
 unsigned int old_nchannels;
protected:
 virtual bool is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_PCM16;}
public:
 TaudioFilterWinamp2(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void done(void);
};

#endif
