#ifndef _TAUDIOFILTEREQSUPER_H_
#define _TAUDIOFILTEREQSUPER_H_

#include "TaudioFilter.h"
#include "TeqSettings.h"

class supereq;
DECLARE_FILTER(TaudioFilterEQsuper,public,TaudioFilter)
private:
 TeqSettings old;unsigned int oldnchannels;
 supereq *eqs[8];
 Tbuffer buf;
protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_FLOAT32;}
public:
 TaudioFilterEQsuper(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TaudioFilterEQsuper();
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void onSeek(void);
};

#endif
