#ifndef _TAUDIODENOISESETTINGS_H_
#define _TAUDIODENOISESETTINGS_H_

#include "TfilterSettings.h"

struct TaudioDenoiseSettings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TaudioDenoiseSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int threshold;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
