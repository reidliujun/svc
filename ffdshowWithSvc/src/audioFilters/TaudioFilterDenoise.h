#ifndef _TAUDIOFILTERDENOISE_H_
#define _TAUDIOFILTERDENOISE_H_

#include "TaudioFilter.h"
#include "TaudioDenoiseSettings.h"

DECLARE_FILTER(TaudioFilterDenoise,public,TaudioFilter)
private:
 int oldthreshold;
 int32_t ethreshold;
 static const int PRECISION  =256;  // miminum squelchable voxels in a row before we squelch...
 static const int HEADROOM   =128;
 static const int MAXPREVS   = 20;
 static const int BANDS      =  8;  // The more bands, the better, up to a point. The quality of the filter algorithm imposes a limit
 static const int SQUELCHMULT=11000;
 struct FilterABand
  {
   int32_t prevs[MAXPREVS];
   // ...Previous voxels, averaged in to filter apart bands. One between each pair of adjacent bands
   unsigned long squelchgain;
   // ...*SQUELCHMULT allows less obstrusive squelch switching
   unsigned char squelching;
 };
 struct FilterAChannel
  {
   FilterABand B[BANDS];
  };
 FilterAChannel Channel[8];
 int32_t threshold[BANDS];
 static const int filtcount[BANDS-1]; /* number of voxels back for filter */
protected:
 virtual int getSupportedFormats(const TfilterSettingsAudio *cfg,bool *honourPreferred,const TsampleFormat &sf) const {return TsampleFormat::SF_PCM16;}
public:
 TaudioFilterDenoise(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TsampleFormat &fmt,void *samples,size_t numsamples,const TfilterSettingsAudio *cfg0);
 virtual void onSeek(void);
};

#endif
