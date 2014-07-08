#ifndef _TENCSTATS_H_
#define _TENCSTATS_H_

#include "ffImgfmt.h"

struct TencFrameParams
{
 unsigned int framenum;
 unsigned int length,outlength;
 bool keyframe;
 int frametype;
 int quant;
 int quanttype;
 bool gray;
 uint64_t psnrY,psnrU,psnrV;
 int kblks,mblks,ublks;
 void *priv;
};

struct TencStats
{
private:
 unsigned int mult[3];
public:
 static const int MSG_FF_FRAME=WM_APP+10; //wParam - size, lParam - (quant<<20)|frametype
 static const int MSG_FF_CLEAR=WM_APP+11;
 TencStats(void)
  {
   init(0,0,FF_CSP_420P);
  }
 void init(unsigned int Idx,unsigned int Idy,int csp);
 void add(const TencFrameParams &frame);
 unsigned int dx,dy;
 unsigned int count;
 unsigned int sumQuants;
 uint64_t sumFramesize;
 uint64_t sumPsnrY,sumPsnrU,sumPsnrV;
 unsigned int quantCount[52];

 void calcPSNR(double *psnrY,double *psnrU,double *psnrV) const;
};

#endif
