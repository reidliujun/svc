#ifndef _TDCTSETTINGS_H_
#define _TDCTSETTINGS_H_

#include "TfilterSettings.h"

struct TdctSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TdctSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int mode;
 int fac0,fac1,fac2,fac3,fac4,fac5,fac6,fac7; //coefficients * 1000
 int quant;
 int32_t matrix0,matrix4,matrix8,matrix12,matrix16,matrix20,matrix24,matrix28,matrix32,matrix36,matrix40,matrix44,matrix48,matrix52,matrix56,matrix60;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
