#ifndef _TRESAMPLESETTINGS_H_
#define _TRESAMPLESETTINGS_H_

#include "TfilterSettings.h"

struct TsampleFormat;
struct TresampleSettings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TresampleSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 enum
  {
   RESAMPLE_LAVC_NORMAL,
   RESAMPLE_LAVC_HIGH,
   RESAMPLE_LAVC_HIGHEST,
   RESAMPLE_SRC_SINC_BEST_QUALITY,
   RESAMPLE_SRC_SINC_MEDIUM_QUALITY,
   RESAMPLE_SRC_SINC_FASTEST
  };
 struct Tresampler
  {
   const char_t *name;
   int id;
  };
 static const Tresampler resamplers[];
 static const char_t *getResamplerName(int mode);
 int freq,mode;
 int _if,cond,condFreq;
 static const char_t* getCondStr(int cond);
 bool isResample(const TsampleFormat &fmt) const;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
