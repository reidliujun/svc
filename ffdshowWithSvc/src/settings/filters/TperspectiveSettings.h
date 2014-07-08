#ifndef _TPERSPECTIVESETTINGS_H_
#define _TPERSPECTIVESETTINGS_H_

#include "TfilterSettings.h"

struct TperspectiveSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TperspectiveSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int isSrc;
 int x1,y1,x2,y2,x3,y3,x4,y4;
 int interpolation;
 static const char_t *interpolationNames[];
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
