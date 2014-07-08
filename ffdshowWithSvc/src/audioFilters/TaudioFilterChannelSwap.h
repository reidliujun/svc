#ifndef _TAUDIOFILTERCHANNELSWAP_H_
#define _TAUDIOFILTERCHANNELSWAP_H_

#include "TaudioFilter.h"
#include "TchannelSwapSettings.h"

DECLARE_FILTER(TaudioFilterChannelSwap,public,TaudioFilter)
private:
 TchannelSwapSettings oldcfg;
 TsampleFormat oldfmt;
 int newchannelmask;
 unsigned int old_nchannels;
 struct Tspeaker
  {
   int i;
   int speaker;
  } speakers[8];
 static bool sortSpeakers(const Tspeaker &spk1,const Tspeaker &spk2);
 template<unsigned int nchannels,class sample_t> void swapChannels(sample_t *samples,size_t numsamples)
  {
   for (unsigned int i=0;i<numsamples;i++)
    {
     sample_t tmp[nchannels];
     size_t ch;
     for (ch=0;ch<nchannels;ch++)
      tmp[ch]=samples[speakers[ch].i];
     for (ch=0;ch<nchannels;ch++)
      samples[ch]=tmp[ch];
     samples+=nchannels;
    }
  }
 template<class sample_t> void swapChannels(sample_t *samples,size_t numsamples)
  {
   typedef void (TaudioFilterChannelSwap::*TswapChannelsFc)(sample_t *samples,size_t numsamples);
   static const TswapChannelsFc swapChannelsFc[9]=
    {
     NULL,
     &TaudioFilterChannelSwap::swapChannels<1,sample_t>,
     &TaudioFilterChannelSwap::swapChannels<2,sample_t>,
     &TaudioFilterChannelSwap::swapChannels<3,sample_t>,
     &TaudioFilterChannelSwap::swapChannels<4,sample_t>,
     &TaudioFilterChannelSwap::swapChannels<5,sample_t>,
     &TaudioFilterChannelSwap::swapChannels<6,sample_t>,
     &TaudioFilterChannelSwap::swapChannels<7,sample_t>,
     &TaudioFilterChannelSwap::swapChannels<8,sample_t>
    };
   (this->*swapChannelsFc[oldfmt.nchannels])(samples,numsamples);
  }
 void makeMap(const TsampleFormat &fmt,const TchannelSwapSettings *cfg);
protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {*honourPreferred=false;return TsampleFormat::SF_ALL;}
public:
 TaudioFilterChannelSwap(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual bool getOutputFmt(TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
};

#endif
