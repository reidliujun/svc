#ifndef _TIMGFILTERCOLORIZE_H_
#define _TIMGFILTERCOLORIZE_H_

#include "TimgFilter.h"

struct TpictPropSettings;
DECLARE_FILTER(TimgFilterColorize,public,TimgFilter)
private:
 void (TimgFilterColorize::*colorizeFc)(TffPict &pict,const TpictPropSettings *cfg,const YUVcolor &c0,const int c[3]);
 template<class _mm> void colorize(TffPict &pict,const TpictPropSettings *cfg,const YUVcolor &c0,const int c[3]);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
public:
 TimgFilterColorize(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
