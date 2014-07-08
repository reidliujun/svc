#ifndef _TAUDIOFILTERDOLBYDECODER_H_
#define _TAUDIOFILTERDOLBYDECODER_H_

#include "TaudioFilter.h"
#include "TdolbyDecoderSettings.h"
#include "firfilter.h"

DECLARE_FILTER(TaudioFilterDolbyDecoder2,public,TaudioFilter)
private:
 int olddelay;unsigned int oldfreq;
 Tbuffer buf;
 unsigned int dlbuflen;
 int cyc_pos;
 float l_fwr,r_fwr,lpr_fwr,lmr_fwr;
 std::vector<float> fwrbuf_l,fwrbuf_r;
 float adapt_l_gain,adapt_r_gain,adapt_lpr_gain,adapt_lmr_gain;
 std::vector<float> lf,rf,lr,rr,cf,cr;
 float LFE_buf[256];unsigned int lfe_pos;
 TfirFilter::_ftype_t *filter_coefs_lfe;unsigned int len125;
 TfirFilter::_ftype_t* calc_coefficients_125Hz_lowpass(int rate);

 static float passive_lock(float x);
 void matrix_decode(const float *in, const int k, const int il,
		    const int ir, bool decode_rear,
		    const int dlbuflen,
		    float l_fwr, float r_fwr,
		    float lpr_fwr, float lmr_fwr,
		    float *adapt_l_gain, float *adapt_r_gain,
		    float *adapt_lpr_gain, float *adapt_lmr_gain,
		    float *lf, float *rf, float *lr,
		    float *rr, float *cf);
protected:
 virtual bool is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_FLOAT32;}
public:
 TaudioFilterDolbyDecoder2(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual bool getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void onSeek(void);
};

#endif
