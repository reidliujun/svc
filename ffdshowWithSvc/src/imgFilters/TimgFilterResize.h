#ifndef _TIMGFILTERRESIZE_H_
#define _TIMGFILTERRESIZE_H_

#include "TimgFilter.h"
#include "TresizeAspectSettings.h"

struct SwsContext;
struct SwsFilter;
struct SwsParams;
class SimpleResize;
struct Tlibmplayer;
DECLARE_FILTER(TimgFilterResize,public,TimgFilter)
private:
 bool sizeChanged,inited;
 TresizeAspectSettings oldSettings;
 int oldcsp;
 TffPict newpict;

 bool oldinterlace,oldWarped;

 Tlibmplayer *libmplayer;SwsContext *swsc;SwsFilter *swsf;SwsParams *swsparams;

 SimpleResize *simple;

 unsigned int dxnone,xdif1none,xdif2none;
 unsigned int dynone,ydif1none,ydif2none;
protected:
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const;
 virtual int getSupportedOutputColorspaces(const TfilterSettingsVideo *cfg) const;
 virtual void onSizeChange(void);
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
public:
 TimgFilterResize(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterResize();
 virtual void done(void);
 virtual bool getOutputFmt(TffPictBase &pict,const TfilterSettingsVideo *cfg0);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual bool acceptRandomYV12andRGB32(void) {return true;}
};

#endif
