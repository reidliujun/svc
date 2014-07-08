#ifndef _TIMGFILTERSHOWMV_H_
#define _TIMGFILTERSHOWMV_H_

#include "TimgFilter.h"
#include "simd.h"

DECLARE_FILTER(TimgFilterShowMV,public,TimgFilter)
private:
 static __align8(const unsigned char,numsL[]);
 static __align8(const unsigned char,numsR[]);
 static __align8(const unsigned char,maskL[]);
 static __align8(const unsigned char,maskR[]);
 typedef std::pair<size_t,int> Tframe;
 std::vector<Tframe> frames;
 static void drawNum(const unsigned char *src,const unsigned char *mask,unsigned char *dst,stride_t stride);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
public:
 TimgFilterShowMV(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg);
 virtual void onSeek(void);
};

#endif
