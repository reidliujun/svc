#ifndef _TAUDIOFILTERLFECROSSOVER_H_
#define _TAUDIOFILTERLFECROSSOVER_H_

#include "TaudioFilter.h"
#include "firfilter.h"

DECLARE_FILTER(TaudioFilterLFEcrossover,public,TaudioFilter)
private:
 TsampleFormat oldfmt;int oldfreq;
 TsampleFormat outfmt;
 int li,ri,lfei,map[8];
 float LFE_buf[256];unsigned int lfe_pos;
 TfirFilter::_ftype_t *filter_coefs_lfe;unsigned int lenLFE;
 float LFE_bufL[256+1],LFE_bufR[256+1];unsigned int lfe_posL,lfe_posR;
 TfirFilter::_ftype_t *filter_coefs_lfeLR;unsigned int lenLFElr;

 Tbuffer buf;
 float gain;
protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_FLOAT32;}
public:
 TaudioFilterLFEcrossover(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void done(void);
 virtual void onSeek(void);
};

#endif
