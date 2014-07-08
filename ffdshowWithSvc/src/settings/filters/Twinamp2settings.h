#ifndef _TWINAMP2SETTINGS_H_
#define _TWINAMP2SETTINGS_H_

#include "TfilterSettings.h"
#include "TglobalSettings.h"

struct Twinamp2settings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
public:
 Twinamp2settings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 char_t flnm[MAX_PATH];
 char_t modulename[256];
 char_t allowMultichannelOnlyIn[MAX_COMPATIBILITYLIST_LENGTH];
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
