#ifndef _TIMGFILTERDSCALER_H_
#define _TIMGFILTERDSCALER_H_

#include "TimgFilter.h"
#include "DScaler/DS_Filter.h"

class Tdll;
struct TDScalerSettings;
struct Tdscaler_FLT
{
private:
 Tdll *f;
 FILTER_METHOD* (__cdecl *GetFilterPluginInfo)(long CpuFeatureFlags);
 HINSTANCE hi;
public:
 Tdscaler_FLT(const char_t *fltflnm,IffdshowBase *deci);
 ~Tdscaler_FLT();
 FILTER_METHOD *fm;
 TDScalerSettings *settings;
};

DECLARE_FILTER(TimgFilterDScalerFLT,public,TimgFilter)
private:
 Tdscaler_FLT *flt;
 TDeinterlaceInfo di;
 size_t pictsize;
 char_t oldfltflnm[MAX_PATH];
 char_t oldfltcfg[512];
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSP_YUY2;}
 virtual void onSizeChange(void);
public:
 TimgFilterDScalerFLT(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterDScalerFLT();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
