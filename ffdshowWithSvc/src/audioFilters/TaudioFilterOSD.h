#ifndef _TAUDIOFILTEROSD_H_
#define _TAUDIOFILTEROSD_H_

#include "TaudioFilter.h"
#include "TOSDprovider.h"
#include "dsutil.h"

struct TOSDsettingsAudio;
class TaudioFilterOSD : public TaudioFilter,public TOSDprovider
_DECLARE_FILTER(TaudioFilterOSD,TaudioFilter)
private:
 int oldIs;
 int outsfs;
 clock_t lasttime;IffdshowDecVideo *registered;
 void unregister(void);
 TsearchInterfaceInGraph* sfi;
protected:
 virtual bool is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {*honourPreferred=false;return TsampleFormat::SF_ALL;}
public:
 TaudioFilterOSD(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TaudioFilterOSD();

 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void onDisconnect(PIN_DIRECTION dir);
 virtual bool getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 void setOutsfs(int o) {outsfs=o;}
};

#endif
