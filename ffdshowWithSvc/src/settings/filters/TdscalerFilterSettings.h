#ifndef _TDSCALERFILTERSETTINGS_H_
#define _TDSCALERFILTERSETTINGS_H_

#include "TfilterSettings.h"

struct TDScalerFilterSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
public:
 TDScalerFilterSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 char_t fltflnm[MAX_PATH];
 char_t cfg[512];
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
};

#endif
