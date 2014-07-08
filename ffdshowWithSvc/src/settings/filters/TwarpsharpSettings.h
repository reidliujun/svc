#ifndef _TWARPSHARPSETTINGS_H_
#define _TWARPSHARPSETTINGS_H_

#include "TfilterSettings.h"

struct TwarpsharpSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TwarpsharpSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 enum
  {
   WARPSHARP_FF=0,
   WARPSHARP_A=1
  };
 int method;
 int warpsharpDepth,warpsharpThreshold;
 static const int warpsharpDepthDef=40,warpsharpThresholdDef=128;
 int awarpsharpDepth,awarpsharpThresh,awarpsharpBlur;
 int awarpsharpCM,awarpsharpBM;
 static const char_t *bms[],*cms[];
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
