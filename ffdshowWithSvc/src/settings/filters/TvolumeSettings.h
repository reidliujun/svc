#ifndef _TVOLUMESETTINGS_H_
#define _TVOLUMESETTINGS_H_

#include "TfilterSettings.h"

struct TvolumeSettings :TfilterSettingsAudio
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TvolumeSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int vol;
 int volL,volC,volR,volSL,volSR,volAL,volAR,volLFE;
 int volLmute,volCmute,volRmute,volSLmute,volSRmute,volALmute,volARmute,volLFEmute;
 int normalize,normalizeMax,nSamples,normalizeResetOnSeek,normalizeRegainVolume;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
};

#endif
