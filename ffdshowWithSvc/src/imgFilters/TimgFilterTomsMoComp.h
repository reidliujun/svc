#ifndef _TIMGFILTERTOMSMOCOMP_H_
#define _TIMGFILTERTOMSMOCOMP_H_

#include "TimgFilter.h"

class Tdll;
struct ItomsMoComp;
struct Tconfig;
DECLARE_FILTER(TimgFilterTomsMoComp,public,TimgFilter)
private:
 Tdll *dll;

 ItomsMoComp *(*createI)(void),*t;
 bool inited;
 int twidth,theight;
 int se,vf;
 stride_t oldstride10;
 int frameNum;
 unsigned char *psrc[3];stride_t pstride[3];
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSP_420P|FF_CSP_YUY2;}
 virtual void onSizeChange(void);
public:
 static const char_t *dllname;
 TimgFilterTomsMoComp(IffdshowBase *Ideci,Tfilters *Iparent);
 ~TimgFilterTomsMoComp();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual void onSeek(void);
};

#endif
