#ifndef _TGRABSETTINGS_H_
#define _TGRABSETTINGS_H_

#include "TfilterSettings.h"

struct TgrabSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TgrabSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);

 char_t path[MAX_PATH],prefix[30];
 int digits;
 int format;
 struct Tformat
  {
   const char_t *name,*ext;
   int codec;
  };
 static const Tformat formats[];
 int mode;
 int frameNum,frameNum1,frameNum2;
 int qual;
 int step;

 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 void reg_op2(TregOp &t);
};

#endif
