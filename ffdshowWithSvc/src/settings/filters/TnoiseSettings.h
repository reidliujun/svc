#ifndef _TNOISESETTINGS_H_
#define _TNOISESETTINGS_H_

#include "TfilterSettings.h"

struct TnoiseSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TnoiseSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 static const char_t *methodsNames[];
 enum
  {
   NOISE_FF=0,
   NOISE_AVIH=1,
   NOISE_MPLAYER=2
  };
 int method;
 int uniform,pattern,averaged;
 int strength,strengthChroma;
 int mplayerTemporal,mplayerQuality;
 int flickerA,flickerF;
 int shakeA,shakeF;
 int linesF,linesA,linesTransparency,linesC;
 int scratchesF,scratchesA,scratchesTransparency,scratchesC;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
