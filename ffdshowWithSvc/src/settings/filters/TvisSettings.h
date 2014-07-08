#ifndef _TVISSETTINGS_H_
#define _TVISSETTINGS_H_

#include "TfilterSettings.h"

struct TvisSettings :TfilterSettingsVideo
{
private:
 static const TfilterIDFF idffs;
protected:
 virtual const int *getResets(unsigned int pageId);
public:
 TvisSettings(TintStrColl *Icoll=NULL,TfilterIDFFs *filters=NULL);
 int mv,quants,graph;
 virtual void createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const;
 virtual void createPages(TffdshowPageDec *parent) const;
 virtual bool getTip(unsigned int pageId,char_t *buf,size_t buflen);
 void reg_op2(TregOp &t);
};

#endif
