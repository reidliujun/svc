#ifndef _TAUDIOFILTERFREEVERB_H_
#define _TAUDIOFILTERFREEVERB_H_

#include "TaudioFilter.h"
#include "TfreeverbSettings.h"

class revmodel;
DECLARE_FILTER(TaudioFilterFreeverb,public,TaudioFilter)
private:
 TfreeverbSettings old;
 revmodel *rev;
protected:
 virtual bool is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_FLOAT32;}
public:
 TaudioFilterFreeverb(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TaudioFilterFreeverb();
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void onSeek(void);
};

#endif
