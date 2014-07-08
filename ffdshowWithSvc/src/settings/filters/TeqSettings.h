#ifndef _TEQSETTINGS_H_
#define _TEQSETTINGS_H_

#include "TfilterSettings.h"

struct TeqSettings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TeqSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 static const float F[10],Fwinamp[10];
 int f0,f1,f2,f3,f4,f5,f6,f7,f8,f9;
 int eq0,eq1,eq2,eq3,eq4,eq5,eq6,eq7,eq8,eq9;
 int lowdb,highdb;
 int super;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
