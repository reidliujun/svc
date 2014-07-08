#ifndef _TAUDIOFILTERFIR_H_
#define _TAUDIOFILTERFIR_H_

#include "TaudioFilter.h"
#include "TfirSettings.h"
#include "firfilter.h"
#include "IaudioFilterFIR.h"

class TaudioFilterFir : public TaudioFilter, public IaudioFilterFir
_DECLARE_FILTER(TaudioFilterFir,TaudioFilter)
private:
 TfirSettings old;unsigned int oldfreq,oldchannels;
 int ip[2+1024];
 float w[65536];
 unsigned int tapes,filterpos;TfirFilter::_ftype_t *fir;float *filterbuf[8];

 struct TstoredFFT
  {
   TstoredFFT(void):have(false),data(NULL),datalen(0),freq(0) {}
   ~TstoredFFT() {if (data) free(data);}
   bool have;
   unsigned int freq;
   unsigned int num;
   float *data;unsigned int datalen;
  } storedfft;
 CCritSec csFFT;
protected:
 virtual bool is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_FLOAT32;}
public:
 TaudioFilterFir(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void onSeek(void);

 virtual HRESULT queryInterface(const IID &iid,void **ptr) const;
 STDMETHODIMP_(int) getFFTdataNum(void);
 STDMETHODIMP getFFTdata(float *fft);
 STDMETHODIMP_(int) getFFTfreq(void);
};

#endif
