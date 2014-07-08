#ifndef _TDOLBYDECODERSETTINGS_H_
#define _TDOLBYDECODERSETTINGS_H_

#include "TfilterSettings.h"

struct TdolbyDecoderSettings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TdolbyDecoderSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int delay;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
