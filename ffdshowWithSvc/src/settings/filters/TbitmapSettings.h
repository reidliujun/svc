#ifndef _TBITMAPSETTINGS_H_
#define _TBITMAPSETTINGS_H_

#include "TfilterSettings.h"
#include "Tfont.h"

struct TbitmapSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TbitmapSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 char_t flnm[MAX_PATH];
 int posx,posy,posmode;
 int align;
 enum
  {
   MODE_BLEND=0,
   MODE_DARKEN=1,
   MODE_LIGHTEN=2,
   MODE_ADD=3,
   MODE_SOFTLIGHT=4,
   MODE_EXCLUSION=5
  };
 int mode;static const char_t *modes[];
 int strength;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
