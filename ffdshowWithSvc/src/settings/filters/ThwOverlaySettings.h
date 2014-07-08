#ifndef _THWOVERLAYSETTINGS_H_
#define _THWOVERLAYSETTINGS_H_

#include "TfilterSettings.h"

struct ThwOverlaySettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 ThwOverlaySettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int brightness,contrast,hue,saturation,sharpness,gamma,colorEnable;
 int reset;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
