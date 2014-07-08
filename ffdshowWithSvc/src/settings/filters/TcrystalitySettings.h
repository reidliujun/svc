#ifndef _TCRYSTALITYSETTINGS_H_
#define _TCRYSTALITYSETTINGS_H_

#include "TfilterSettings.h"

struct TcrystalitySettings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TcrystalitySettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int bext_level,echo_level,stereo_level,filter_level,feedback_level,harmonics_level;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
