#ifndef _TAUDIOFILTERVOLUME_H_
#define _TAUDIOFILTERVOLUME_H_

#include "TaudioFilter.h"
#include "TvolumeSettings.h"
#include "IaudioFilterVolume.h"

class TaudioFilterVolume : public TaudioFilter, public IaudioFilterVolume
_DECLARE_FILTER(TaudioFilterVolume,TaudioFilter)
private:
 TsampleFormat oldfmt;
 TvolumeSettings oldcfg;
 int volumes[8];
 bool isVol;

 static const float MUL_MIN,MUL_MAX, MUL_STEP;
 float mul;

 template<unsigned int nchannels> struct Tmultiply
  {
   template<class sample_t> static void processVol(sample_t * const samples,size_t numsamples,const int *volumes)
    {
     typedef typename TsampleFormatInfo<sample_t>::helper_t helper_t;
     for (size_t i=0;i<numsamples*nchannels;)
      for (unsigned int ch=0;ch<nchannels;ch++,i++)
       {
        helper_t yt=volumes[ch]*helper_t(samples[i])/256;
        samples[i]=TsampleFormatInfo<sample_t>::limit(yt);
       }
    }
   template<class sample_t> static void processMul(sample_t * const samples,size_t numsamples,const int *volumes,float mul)
    {
     if (IsSameType<sample_t,float>::value)
      for (size_t i=0;i<numsamples*nchannels;)
       for (unsigned int ch=0;ch<nchannels;ch++,i++)
        {
         float tmp=mul*float(samples[i])*volumes[ch]/256;
         samples[i]=TsampleFormatInfo<sample_t>::limit(tmp);
        }
     else
      {
       typedef typename TsampleFormatInfo<sample_t>::helper_t helper_t;
       helper_t mulT=helper_t(mul*(-TsampleFormatInfo<sample_t>::min()/16));
       for (size_t i=0;i<numsamples*nchannels;)
        for (unsigned int ch=0;ch<nchannels;ch++,i++)
         {
          helper_t yt=(volumes[ch]*helper_t(samples[i]))/256;
          helper_t tmp=mulT*yt/(-TsampleFormatInfo<sample_t>::min()/16);
          samples[i]=TsampleFormatInfo<sample_t>::limit(tmp);
         }
      }
    }
  };
 template<class sample_t> void volume(sample_t* const samples,size_t numsamples,const TsampleFormat &fmt,const TvolumeSettings *cfg);

 struct
  {
   bool have;
   unsigned int volumes[8]; // 0 - 65536
  } storedvolumes;
 CCritSec csVolumes;
protected:
 virtual bool is(const TsampleFormat &fmt,const TfilterSettingsAudio *cfg);
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_ALL_24;}
public:
 TaudioFilterVolume(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TaudioFilterVolume();
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void onSeek(void);

 virtual HRESULT queryInterface(const IID &iid,void **ptr) const;
 STDMETHODIMP getVolumeData(unsigned int *nchannels,int channels[],int volumes[]);
 STDMETHODIMP_(int) getCurrentNormalization(void);
};

#endif
