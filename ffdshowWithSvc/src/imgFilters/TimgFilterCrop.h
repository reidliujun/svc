#ifndef _TIMGFILTERCROP_H_
#define _TIMGFILTERCROP_H_

#include "TimgFilter.h"
#include "TcropSettings.h"
#include "TimgFilterExpand.h"

DECLARE_FILTER(TimgFilterCrop,public,TimgFilter)
private:
 Trect rectCrop,oldRect;
 TcropSettings oldSettings;
 unsigned int lastFrameMS;
 int autoCropTopStatus, autoCropBottomStatus, autoCropLeftStatus, autoCropRightStatus;
protected:
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK&~(FF_CSP_NV12|FF_CSP_CLJR);}
 virtual void onSizeChange(void);
 static int autoCropTop, autoCropBottom, autoCropLeft, autoCropRight; // Can only be static because calcProp is static
public:
 TimgFilterCrop(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual bool getOutputFmt(TffPictBase &pict,const TfilterSettingsVideo *cfg0);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 static Trect calcCrop(const Trect &pictRect,const TcropSettings *cfg);
 virtual Trect calcCrop(const Trect &pictRect,TcropSettings *cfg, TffPict *ppict);
 virtual void calcAutoCropVertical(TcropSettings *cfg, const unsigned char *src, unsigned int y0, int stepy, int *autoCrop, int *autoCropStatus);
 virtual void calcAutoCropHorizontal(TcropSettings *cfg, const unsigned char *src, unsigned int x0, int stepx, int *autoCrop, int *autoCropStatus);
};

DECLARE_FILTER(TimgFilterCropExpand,public,TimgFilterExpand)
private:
 TcropSettings oldSettings;
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual void getDiffXY(const TffPict &pict,const TfilterSettingsVideo *cfg,int &diffx,int &diffy);
public:
 TimgFilterCropExpand(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual bool getOutputFmt(TffPictBase &pict,const TfilterSettingsVideo *cfg0);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
