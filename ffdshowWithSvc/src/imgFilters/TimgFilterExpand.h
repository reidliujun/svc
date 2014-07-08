#ifndef _TIMGFILTEREXPAND_H_
#define _TIMGFILTEREXPAND_H_

#include "TimgFilter.h"

DECLARE_FILTER(TimgFilterExpand,public,TimgFilter)
private:
 Trect calcNewClip(const Trect &oldClip,const Trect &newFull);
protected:
 int diffx,diffy;
 unsigned int dxnone,xdif1none,xdif2none;
 unsigned int dynone,ydif1none,ydif2none;
 TffPict newpict;
 bool sizeChanged;int oldcsp;
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual void onSizeChange(void);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK&~(FF_CSP_NV12|FF_CSP_CLJR);}
 void calcNewRect(const Trect &newrect,Trect &pictrectFull,Trect &pictrectClip);
 virtual void expand(TffPict &pict,const TfilterSettingsVideo *cfg,bool diffxy);
 virtual void getDiffXY(const TffPict &pict,const TfilterSettingsVideo *cfg,int &diffx,int &diffy) {}
public:
 TimgFilterExpand(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual bool getOutputFmt(TffPictBase &pict,const TfilterSettingsVideo *cfg0);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

DECLARE_FILTER(TimgFilterSubtitleExpand,public,TimgFilterExpand)
private:
protected:
 virtual void expand(TffPict &pict,const TfilterSettingsVideo *cfg,bool diffxy);
public:
 TimgFilterSubtitleExpand(IffdshowBase *Ideci,Tfilters *Iparent);
};

#endif
