#ifndef _TCHANNELSWAPSETTINGS_H_
#define _TCHANNELSWAPSETTINGS_H_

#include "TfilterSettings.h"

struct TchannelSwapSettings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TchannelSwapSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int l,c,r,sl,sr,rear,lfe,al,ar;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
