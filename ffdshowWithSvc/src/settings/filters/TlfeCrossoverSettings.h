#ifndef _TLFECROSSOVERSETTINGS_H_
#define _TLFECROSSOVERSETTINGS_H_

#include "TfilterSettings.h"

struct TlfeCrossoverSettings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TlfeCrossoverSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int freq,gain,cutLR;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
