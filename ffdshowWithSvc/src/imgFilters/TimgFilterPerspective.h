#ifndef _TIMGFILTERPERSPECTIVE_H_
#define _TIMGFILTERPERSPECTIVE_H_

#include "TimgFilter.h"
#include "TperspectiveSettings.h"
#include "TcubicTable.h"

DECLARE_FILTER(TimgFilterPerspective,public,TimgFilter)
private:
 static const int SUB_PIXEL_BITS=8;
 static const int SUB_PIXELS=1<<SUB_PIXEL_BITS;
 static const int COEFF_BITS=11;
 TperspectiveSettings old;
 double ref[4][2];
 int32_t (*pv)[2];
 int pvStride;
 void initPv(int W, int H,bool src,int border);
 void resampleNone  (uint8_t *dst, stride_t dstStride, const uint8_t *src, stride_t srcStride, int w, int h, int chromaX,int chromaY,unsigned char black);
 void resampleLinear(uint8_t *dst, stride_t dstStride, const uint8_t *src, stride_t srcStride, int w, int h, int chromaX,int chromaY,unsigned char black);
 void resampleCubic (uint8_t *dst, stride_t dstStride, const uint8_t *src, stride_t srcStride, int w, int h, int chromaX,int chromaY,unsigned char black);
 TcubicTable cubic;
protected:
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
 virtual void onSizeChange(void);
public:
 TimgFilterPerspective(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual void done(void);
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
