#ifndef _TPICTPROPSETTINGS_H_
#define _TPICTPROPSETTINGS_H_

#include "TfilterSettings.h"

struct TpictPropSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TpictPropSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int lumGain,lumOffset;
 int gammaCorrection,gammaCorrectionR,gammaCorrectionG,gammaCorrectionB;
 static const int lumGainDef=128,lumOffsetDef=0,gammaCorrectionDef=100;
 int hue,saturation;
 static const int hueDef=0,saturationDef=64;
 int colorizeStrength,colorizeColor,colorizeChromaonly;
 int levelfix,levelfixFull;
 int scanlineEffect;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
