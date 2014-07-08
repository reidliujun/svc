#ifndef _TIMGFILTERLEVELS_H_
#define _TIMGFILTERLEVELS_H_

#include "TimgFilter.h"
#include "TlevelsSettings.h"
#include "IimgFilterLevels.h"

class TimgFilterLevels : public TimgFilter, public IimgFilterLevels
_DECLARE_FILTER(TimgFilterLevels,TimgFilter)
private:
 TlevelsSettings oldSettings;
 int flag_resetHistory,oldMode;
 static const int HISTORY=32;
 int inMins[HISTORY],inMinSum,inMin,inMaxs[HISTORY],inMaxSum,inMax;unsigned int minMaxPos;void resetHistory(void);
 unsigned int map[256],mapchroma[256];
 unsigned int histogram[256];CCritSec csHistogram;
 template<int lumaoffset,int chromaoffset,int onlyLuma> void processPacked(const unsigned char *srcY,unsigned char *dstY)
  {
   for (unsigned int y=0;y<dy1[0];y++)
    {
     const uint8_t *src;uint16_t *dst,*dstEnd;
     for (src=srcY+stride1[0]*y,dst=(uint16_t*)(dstY+stride2[0]*y),dstEnd=dst+dx1[0];dst!=dstEnd;src+=2,dst++)
      *dst=uint16_t((map[src[lumaoffset]]<<(8*lumaoffset))|((onlyLuma?src[chromaoffset]:mapchroma[src[chromaoffset]])<<(8*chromaoffset)));
    }
  }
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 static const int supportedcsps=FF_CSPS_MASK_YUV_PLANAR|FF_CSPS_MASK_YUV_PACKED;
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return supportedcsps;}
public:
 TimgFilterLevels(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual void onSeek(void);

 virtual HRESULT queryInterface(const IID &iid,void **ptr) const;
 STDMETHODIMP getHistogram(unsigned int dst[256]);
 STDMETHODIMP getInAuto(int *min,int *max);
};

#endif
