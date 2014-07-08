#ifndef _TIMGFILTERDCT_H_
#define _TIMGFILTERDCT_H_

#include "TimgFilter.h"
#include "simd.h"

DECLARE_FILTER(TimgFilterDCT,public,TimgFilter)
private:
 short *pWorkArea;
 int oldMode;int oldfac[8];unsigned char oldmatrix[64];
 short factors[8][8];int quant;
 void (*fdct)(short*);
 void (*idct)(short*);
 static void idct_c(short*),fdct_c(short*),idct_c_init(void);static short iclip[1024],*iclp;
 static void fdct_sse2(short *block),idct_sse2(short *block);

 void (TimgFilterDCT::*processDct)(void);
 void multiply(void),h263(void),mpeg(void);
 static void quant_h263_inter(int16_t * coeff,const uint32_t quant, const uint16_t * =NULL);
 static void dequant_h263_inter(int16_t * data,const uint32_t quant,const uint16_t * =NULL);
 static void quant_mpeg_inter(int16_t * coeff,const uint32_t quant,const uint16_t * mpeg_quant_matrices);
 static void dequant_mpeg_inter(int16_t * data,const uint32_t quant,const uint16_t * mpeg_quant_matrices);
protected:
 virtual int getSupportedInputColorspaces(const TfilterSettingsVideo *cfg) const {return FF_CSPS_MASK_YUV_PLANAR;}
public:
 TimgFilterDCT(IffdshowBase *Ideci,Tfilters *Iparent);
 virtual ~TimgFilterDCT();
 virtual HRESULT process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0);
};

#endif
