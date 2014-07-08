#ifndef _TIMGFILTERHWOVERLAY_H_
#define _TIMGFILTERHWOVERLAY_H_

#include "TimgFilter.h"
#include "ThwOverlaySettings.h"

struct IhwOverlayControl;
DECLARE_FILTER(TimgFilterHWoverlay,public,TimgFilter)
private:
 IhwOverlayControl *overlayControl;bool firsttimeOverlay;
 std::pair<bool,int> ddcc[6];
 bool firsttimeDDCC,isddcc;
 ThwOverlaySettings old;
 int oldReset;
protected:
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK;}
public:
 TimgFilterHWoverlay(IffdshowBase *Ideci,Tfilters *Iparent);
 ~TimgFilterHWoverlay();
 virtual bool acceptRandomYV12andRGB32(void) {return true;}
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
