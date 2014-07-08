#ifndef _TOFFSETSETTINGS_H_
#define _TOFFSETSETTINGS_H_

#include "TfilterSettings.h"

struct ToffsetSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 ToffsetSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int Y_X,Y_Y,U_X,U_Y,V_X,V_Y;
 int flip,mirror;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
