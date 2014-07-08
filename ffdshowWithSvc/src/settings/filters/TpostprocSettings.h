#ifndef _TPOSTPROCSETTINGS_H_
#define _TPOSTPROCSETTINGS_H_

#include "TfilterSettings.h"

struct TpostprocSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TpostprocSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int method,nicFirst;
 int qual;
 int autoq;
 int isCustom,custom;
 int deblockMplayerAccurate;
 int deblockStrength;
 static const int deblockStrengthDef;
 int levelFixLum/*,levelFixChrom*/;
 int fullYrange;
 int nicXthresh,nicYthresh;
 int sppMode;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
