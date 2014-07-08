#ifndef _TIMGFILTERDSCALERDEINTERLACE_H_
#define _TIMGFILTERDSCALERDEINTERLACE_H_

#include "TimgFilter.h"
#include "DScaler/DS_Deinterlace.h"

class Tdll;
struct TDScalerSettings;
struct Tdscaler_DI
{
private:
 Tdll *f;
 bool onlyinfo;
 DEINTERLACE_METHOD* (__cdecl *GetDeinterlacePluginInfo)(long CpuFeatureFlags);
 HINSTANCE hi;
public:
 Tdscaler_DI(const char_t *fltflnm,IffdshowBase *deci,bool onlyinfo=false);
 ~Tdscaler_DI();
 DEINTERLACE_METHOD *fm;
 TDScalerSettings *settings;
};

DECLARE_FILTER(TimgFilterDScalerDI,public,TimgFilter)
private:
 Tdscaler_DI *flt;
 TDeinterlaceInfo di;
 char_t oldfltflnm[MAX_PATH];
 char_t oldfltcfg[512];
 int do_deinterlace;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSP_YUY2;}
 virtual void onSizeChange(void);
public:
 TimgFilterDScalerDI(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterDScalerDI();
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
