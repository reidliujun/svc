#ifndef _TEXPANDSETTINGS_H_
#define _TEXPANDSETTINGS_H_

#include "TfilterSettings.h"
#include "TffRect.h"

struct TexpandSettings :TfilterSettingsVideo
{
 TexpandSettings(void):TfilterSettingsVideo(sizeof(*this),false,NULL) {}
 Trect newrect;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const {}
};

#endif
