#ifndef _TGRADFUNSETTINGS_H_
#define _TGRADFUNSETTINGS_H_

#include "TfilterSettings.h"

struct TgradFunSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TgradFunSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int threshold;

 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
