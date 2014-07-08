#ifndef _TCROPSETTINGS_H_
#define _TCROPSETTINGS_H_

#include "TfilterSettings.h"

struct TcropSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TcropSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int mode;
 int magnificationX,magnificationY,magnificationLocked;
 int cropTop,cropBottom,cropLeft,cropRight;
 //int autoCropTop, autoCropBottom, autoCropLeft, autoCropRight;
 int panscanZoom,panscanX,panscanY;
 int cropTolerance;
 int cropRefreshDelay, cropStopScan;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
