#ifndef _TBLURSETTINGS_H_
#define _TBLURSETTINGS_H_

#include "TfilterSettings.h"

struct TblurSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual void getMinMax(int id,int &min,int &max);
 virtual const int *getResets(unsigned int pageId);
public:
 TblurSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int isSoften,soften;
 int isTempSmooth,tempSmooth,tempSmoothColor;
 int isSmoothLuma,isSmoothChroma,smoothStrengthLuma,smoothStrengthChroma;
 int isGradual,gradualStrength;
 int isMplayerTNR,mplayerTNR1,mplayerTNR2,mplayerTNR3;
 int isMplayerBlur,mplayerBlurRadius,mplayerBlurLuma,mplayerBlurChroma;
 int isDenoise3d,denoise3Dluma,denoise3Dchroma,denoise3Dtime,denoise3Dhq;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
