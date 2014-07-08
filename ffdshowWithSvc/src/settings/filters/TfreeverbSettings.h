#ifndef _TFREEVERBSETTINGS_H_
#define _TFREEVERBSETTINGS_H_

#include "TfilterSettings.h"

struct TfreeverbSettings :TfilterSettingsAudio
{
public:
 static const int scalewet=3;
protected:
 virtual const int *getResets(unsigned int pageId);
private:
 static const int initialroom =int(1000*0.5f);
 static const int initialdamp =int(1000*0.25f);
 static const int initialwet  =int(1000*(1.0f/scalewet));
 static const int initialdry  =int(1000*0.75);
 static const int initialwidth=int(1000*1);
 static const int initialmode =int(1000*0);
 static const TfilterIDFF idffs;
public:
 TfreeverbSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int roomsize,damp,wet,dry,width,mode;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
};

#endif
