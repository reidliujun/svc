#ifndef _TIMGFILTERTIMESMOOTH_H_
#define _TIMGFILTERTIMESMOOTH_H_

#include "TimgFilter.h"

DECLARE_FILTER(TimgFilterTimesmooth,public,TimgFilter)
private:
 static const int KERNEL=4;
 static const int64_t scaletab[];
 int oldStrength;
 unsigned char *accumY,*accumU,*accumV;
 unsigned char *tempU[3],*tempV[3];int UVcount;
 int framecount;
 bool isFirstFrame;
 int square_table[255*2+1];
 void smooth(const unsigned char *src,stride_t srcStride,unsigned char *dst,stride_t dstStride,unsigned char *Iaccum,unsigned int dx,unsigned int dy);
protected:
 virtual bool is(const TffPictBase &pict,const TfilterSettingsVideo *cfg);
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void);
public:
 TimgFilterTimesmooth(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
 virtual void onSeek(void);
};

#endif
