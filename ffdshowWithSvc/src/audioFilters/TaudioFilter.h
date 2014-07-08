#ifndef _TAUDIOFILTER_H_
#define _TAUDIOFILTER_H_

#include "Tfilter.h"
#include "TsampleFormat.h"
#include "TaudioFilters.h"
#include "interfaces.h"

struct TfilterSettingsAudio;
struct TpresetAudio;
class Tdither;
class TaudioFilter :public Tfilter
{
private:
 Tbuffer buf;
 void* alloc_buffer(const TsampleFormat &sf,size_t numsamples)
  {
   return alloc_buffer(sf,numsamples,buf);
  }
 void* convertTo(const TsampleFormat &sfIn,const void *bufIn,size_t numsamples,const TsampleFormat &sfOut,size_t samplestoalloc=0);
 Tdither *dither;
 int oldnoiseshaping,oldsfout;
protected:
 comptrQ<IffdshowDecAudio> deciA;
 virtual bool is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const=0;
 void* init(const TfilterSettingsAudio *cfg,TsampleFormat &sf,const void *bufIn,size_t numsamples,size_t samplestoalloc=0);
 TaudioFilters *parent;
public:
 TaudioFilter(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TaudioFilter();
 virtual bool getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0)=0;
 virtual HRESULT flush(TfilterQueue::iterator it,TsampleFormat &fmt,const TfilterSettingsAudio *cfg0);
 static void* alloc_buffer(const TsampleFormat &sf,size_t numsamples,Tbuffer &buf);
};

#endif
