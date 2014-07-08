#ifndef _TAUDIOFILTERMIXER_H_
#define _TAUDIOFILTERMIXER_H_

#include "TaudioFilter.h"
#include "mixer.h"
#include "TmixerSettings.h"
#include "IaudioFilterMixer.h"

class TaudioFilterMixer : public TaudioFilter, public IaudioFilterMixer
_DECLARE_FILTER(TaudioFilterMixer,TaudioFilter)
private:
 Mixer<int16_t,int32_t,1<<16,4> mixerPCM16;
 Mixer<int32_t,int64_t,1LL<<32,4> mixerPCM32;
 Mixer<float,int,1,1> mixerFloat;
 CCritSec csMatrix;
 const TmixerMatrix::mixer_matrix_t *matrixPtr;
 int inmask,outmask;
 unsigned int oldnchannels,oldchannelmask;
 int oldsf;
 TmixerSettings oldcfg;

protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_ALL_24;}

public:
 TaudioFilterMixer(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual bool getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);

 virtual HRESULT queryInterface(const IID &iid,void **ptr) const;
 STDMETHODIMP getMixerMatrixData(double matrix[6][6]);
 STDMETHODIMP getMixerMatrixData2(double matrix[9][9],int *inmaskPtr,int *outmaskPtr);
};

#endif
