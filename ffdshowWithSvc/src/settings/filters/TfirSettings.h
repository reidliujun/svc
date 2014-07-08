#ifndef _TFIRSETTINGS_H_
#define _TFIRSETTINGS_H_

#include "TfilterSettings.h"

struct TfirSettings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TfirSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 enum
  {
   LOWPASS=0,
   HIGHPASS=1,
   BANDPASS=2,
   BANDSTOP=3
  };
 static const char_t *types[];
 enum
  {
   WINDOW_BOX=0,
   WINDOW_TRIANGLE=1,
   WINDOW_HAMMING=2,
   WINDOW_HANNING=3,
   WINDOW_BLACKMAN=4,
   WINDOW_FLATTOP=5,
   WINDOW_KAISER=6
  };
 static const char_t *windows[];
 int taps;
 int type;
 int freq,width;
 int window,kaiserbeta;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
