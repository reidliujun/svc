#ifndef _TLOGOAWAYSETTINGS_H_
#define _TLOGOAWAYSETTINGS_H_

#include "TfilterSettings.h"

struct TlogoawaySettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TlogoawaySettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int x,y,dx,dy;
 int mode,lumaonly;
 int blur;
 int bordn_mode,bords_mode,bordw_mode,borde_mode;
 int pointnw, pointne, pointsw, pointse;
 int vhweight;
 int solidcolor;
 char_t parambitmap[MAX_PATH];
 enum
  {
   MODE_LUMINANCE=1,
   MODE_XY,
   MODE_UGLARM,
   MODE_SOLIDFILL,
   MODE_SHAPEXY,
   MODE_SHAPEUGLARM,
   MODE_LAST
  };
 struct Tmode
  {
   int id;
   const char_t *name;
  };
 static const Tmode modes[];
 static const char_t *getModeDescr(int mode);
 enum BORDERMODE
  {
   BM_UNKNOWN,
   BM_DIRECT,
   BM_OPPOSITE,
   BM_INTERPOLATE
  };
 static const char_t *borderModes[];
 enum NAVIGATOR
  {
   UNKNOWN,
   NORTH,
   EAST,
   SOUTH,
   WEST,
   NW,
   NE,
   SW,
   SE
  };
 static const char_t *navigators[];
 struct TborderPreset
  {
   const char_t *name;
   int n,e,s,w;
  };
 static const TborderPreset borderPresets[];

 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
