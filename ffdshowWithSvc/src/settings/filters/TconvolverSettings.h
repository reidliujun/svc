#ifndef _TCONVOLVERSETTINGS_H_
#define _TCONVOLVERSETTINGS_H_

#include "TfilterSettings.h"

struct TconvolverSettings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TconvolverSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int mappingMode;
 char_t file[MAX_PATH],fileL[MAX_PATH],fileR[MAX_PATH],fileC[MAX_PATH],fileLFE[MAX_PATH],fileSL[MAX_PATH],fileSR[MAX_PATH];
 int levelAdjustAuto,levelAdjustDB;
 int mixingStrength;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
