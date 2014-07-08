#ifndef _TDEINTERLACESETTINGS_H_
#define _TDEINTERLACESETTINGS_H_

#include "TfilterSettings.h"

class Twindow;
class TdeinterlacePanel;
struct TdeinterlaceSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
public:
 enum
  {
   TOMSMOCOMP_DEINT_FILTER=0,
   DGBOB_DEINT_FILTER=1,
   FRAMERATEDOUBLER=2,
   KERNELDEINT=3,
   BYPASS=4,
   KERNELBOB=5,
   YADIF=6,
   DSCALER=0x1000000,
  };
 struct TmethodProps
  {
   int cfgId;
   const char_t *name;
   int id;
   TdeinterlacePanel* (*create)(Twindow *parent);
  };
 static const TmethodProps methodProps[];

 TdeinterlaceSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);

 int deinterlaceAlways;
 int swapfields;
 int cfgId;
 static const int cfgIdDef=2;
 static const TmethodProps &getMethod(int cfgId);
 int tomsmocompSE,tomsmocompVF;
 char_t dscalerFlnm[MAX_PATH];
 char_t dscalerCfg[512];

 int frameRateDoublerThreshold,frameRateDoublerSE;
 struct TframerateDoublerSEs
  {
   const char_t *name;
   int id;
  };
 static const TframerateDoublerSEs frameRateDoublerSEs[];
 static const TframerateDoublerSEs yadifParitySEs[];

 int kernelThreshold,kernelSharp,kernelTwoway,kernelMap,kernelLinked;

 int dgbobMode,dgbobThreshold,dgbobAP;
 int yadifMode,yadifFieldOrder;
 static const char_t *dgbobModes[];

 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
