#ifndef _TSHARPENSETTINGS_H_
#define _TSHARPENSETTINGS_H_

#include "TfilterSettings.h"

struct TsharpenSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TsharpenSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 enum
  {
   SHARPEN_XSHARP =0,
   SHARPEN_UNSHARP=1,
   SHARPEN_MSHARP =2,
   SHARPEN_ASHARP =4,
   SHARPEN_MPLAYER=5
  };
 int method;
 int xsharpStrength,xsharpThreshold;
 static const int xsharpStrengthDef=20,xsharpThresholdDef=150;
 int unsharpStrength,unsharpThreshold;
 static const int unsharpStrengthDef=40,unsharpThresholdDef=0;
 int msharpStrength,msharpThreshold;
 int msharpHQ,msharpMask;
 static const int msharpStrengthDef=100,msharpThresholdDef=15;
 int asharpT,asharpD,asharpB,asharpHQBF;
 int mplayerLuma,mplayerChroma;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
 virtual void reg_op(TregOp &t);
};

#endif
