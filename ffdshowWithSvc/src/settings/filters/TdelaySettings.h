#ifndef _TDELAYSETTINGS_H_
#define _TDELAYSETTINGS_H_

#include "TfilterSettings.h"

struct TdelaySettings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TdelaySettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int l,c,r,sl,sr,bc,lfe,al,ar;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
