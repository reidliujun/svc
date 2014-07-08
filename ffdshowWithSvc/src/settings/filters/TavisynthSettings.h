#ifndef _TAVISYNTHSETTINGS_H_
#define _TAVISYNTHSETTINGS_H_

#include "TfilterSettings.h"
#define MAX_AVISYNTH_SCRIPT_LENGTH 16384

class TffdshowPageDec;

struct TavisynthSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;

protected:
 virtual const int *getResets(unsigned int pageId);

public:
 TavisynthSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);

 int ffdshowSource;
 int applyPulldown;
 int inYV12,inYUY2,inRGB24,inRGB32;
 int enableBuffering;
 int bufferAhead;
 int bufferBack;
 char_t script[MAX_AVISYNTH_SCRIPT_LENGTH];
 char_t scriptMULTI_SZ[MAX_AVISYNTH_SCRIPT_LENGTH];

 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual void reg_op(TregOp &t);
 virtual void getDefaultStr(int id,char_t *buf,size_t buflen);
};

#endif
