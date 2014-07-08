/*
 * Copyright (c) 2003-2006 Milan Cutka
 * based on mplayer denoise3d and hqdn3d filters (C) 2003 Daniel Moreno <comac@comac.darktech.org>
 * by hqdn3d deNoiseTemporal and deNoiseSpacial by Loren Merritt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "TimgFilterDenoise3d.h"
#include "TblurSettings.h"
#include "Tconfig.h"
#include "simd.h"

//===================================== TimgFilterDenoise3d =====================================
TimgFilterDenoise3d::TimgFilterDenoise3d(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 oldLuma=oldChroma=oldTime=-2;
}
void TimgFilterDenoise3d::onSizeChange(void)
{
 done();
}

bool TimgFilterDenoise3d::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TblurSettings *cfg=(const TblurSettings*)cfg0;
 return super::is(pict,cfg) && cfg->isDenoise3d && (cfg->denoise3Dluma || cfg->denoise3Dchroma || cfg->denoise3Dtime);
}

HRESULT TimgFilterDenoise3d::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TblurSettings *cfg=(const TblurSettings*)cfg0;
   init(pict,cfg->full,cfg->half);

   if (oldLuma!=cfg->denoise3Dluma || oldChroma!=cfg->denoise3Dchroma || oldTime!=cfg->denoise3Dtime)
    {
     oldLuma=cfg->denoise3Dluma;oldChroma=cfg->denoise3Dchroma;oldTime=cfg->denoise3Dtime;
     double LumSpac=oldLuma/100.0;
     double LumTmp=oldTime/100.0;
     double ChromSpac=oldChroma/100.0;
     double ChromTmp=LumTmp*ChromSpac/LumSpac;
     precalcCoefs(0,LumSpac);
     precalcCoefs(1,LumTmp);
     precalcCoefs(2,ChromSpac);
     precalcCoefs(3,ChromTmp);
    }

   bool cspChanged=false;
   const unsigned char *src[4];
   cspChanged|=getCur(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,src);
   unsigned char *dst[4];
   cspChanged|=getNext(csp1,pict,cfg->full,dst);
   if (cspChanged) done();

   deNoise(pict,dx1[0],dy1[0],src,stride1,dst,stride2);
  }
 return parent->deliverSample(++it,pict);
}

void TimgFilterDenoise3d::onSeek(void)
{
 done();
}

//================================== TimgFilterDenoise3dNormal =================================
TimgFilterDenoise3dNormal::TimgFilterDenoise3dNormal(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilterDenoise3d(Ideci,Iparent)
{
 Line=NULL;
 Frame[0]=Frame[1]=Frame[2]=NULL;
}
void TimgFilterDenoise3dNormal::done(void)
{
 if (Line) aligned_free(Line);Line=NULL;
 if (Frame[0]) aligned_free(Frame[0]);Frame[0]=NULL;
 if (Frame[1]) aligned_free(Frame[1]);Frame[1]=NULL;
 if (Frame[2]) aligned_free(Frame[2]);Frame[2]=NULL;
}

void TimgFilterDenoise3dNormal::precalcCoefs(int nCt, double Dist25)
{
 int *Ct=Coefs[nCt];
 int i;
 double Gamma = log(0.25) / log(1.0 - Dist25/255.0);

 for (i = -256; i <= 255; i++)
  {
   double Simil = 1.0 - abs(i) / 255.0;
   double C = pow(Simil, Gamma) * (double)i;
   Ct[256+i] = int((C<0) ? (C-0.5) : (C+0.5));
  }
}

void TimgFilterDenoise3dNormal::deNoise(const unsigned char *Frame,
                                        const unsigned char *FramePrev,
                                        unsigned char *FrameDest,
                                        unsigned char *LineAnt,
                                        int W, int H, stride_t sStride, stride_t pStride, stride_t dStride,
                                        int *Horizontal, int *Vertical, int *Temporal)
{
 stride_t sLineOffs = 0, pLineOffs = 0, dLineOffs = 0;
 unsigned char PixelAnt;

 /* First pixel has no left nor top neightbour. Only previous frame */
 LineAnt[0] = PixelAnt = Frame[0];
 FrameDest[0] = LowPass(FramePrev[0], LineAnt[0], Temporal);

 // Fist line has no top neightbour. Only left one for each pixel and last frame
 for (int X = 1; X < W; X++)
  {
   PixelAnt = LowPass(PixelAnt, Frame[X], Horizontal);
   LineAnt[X] = PixelAnt;
   FrameDest[X] = LowPass(FramePrev[X], LineAnt[X], Temporal);
  }

 for (int Y = 1; Y < H; Y++)
  {
   sLineOffs += sStride, pLineOffs += pStride, dLineOffs += dStride;
   // First pixel on each line doesn't have previous pixel
   PixelAnt = Frame[sLineOffs];
   LineAnt[0] = LowPass(LineAnt[0], PixelAnt, Vertical);
   FrameDest[dLineOffs] = LowPass(FramePrev[pLineOffs], LineAnt[0], Temporal);

   for (int X = 1; X < W; X++)
    {
     // The rest are normal
     PixelAnt = LowPass(PixelAnt, Frame[sLineOffs+X], Horizontal);
     LineAnt[X] = LowPass(LineAnt[X], PixelAnt, Vertical);
     FrameDest[dLineOffs+X] = LowPass(FramePrev[pLineOffs+X], LineAnt[X], Temporal);
    }
  }
}

void TimgFilterDenoise3dNormal::deNoise(TffPict &pict,unsigned int dx,unsigned int dy,const unsigned char *src[4],stride_t stride1[4],unsigned char *dst[4],stride_t stride2[4])
{
 if (!Line) Line=(unsigned char*)aligned_malloc(dx);
 if (!Frame[0])
  for (unsigned int i=0;i<3;i++)
   {
    FrameStride[i]=((dx>>pict.cspInfo.shiftX[i])/16+2)*16;
    Frame[i]=(unsigned char*)aligned_malloc(FrameStride[i]*(dy>>pict.cspInfo.shiftY[i]));
   }

 static const int coeffIdxs[3]={0,2,2};

 for (unsigned int i=0;i<3;i++)
  {
   deNoise(src[i],Frame[i],dst[i],
           Line,dx>>pict.cspInfo.shiftX[i],dy>>pict.cspInfo.shiftY[i],
           stride1[i],FrameStride[i],stride2[i],
           Coefs[coeffIdxs[i]  ] + 256,
           Coefs[coeffIdxs[i]  ] + 256,
           Coefs[coeffIdxs[i]+1] + 256);
   TffPict::copy(Frame[i],FrameStride[i],dst[i],stride2[i],pict.cspInfo.Bpp*dx>>pict.cspInfo.shiftX[i],dy>>pict.cspInfo.shiftY[i]);
  }
}

//================================== TimgFilterDenoise3dHQ ===================================
TimgFilterDenoise3dHQ::TimgFilterDenoise3dHQ(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilterDenoise3d(Ideci,Iparent)
{
 Line=NULL;
 Frame[0]=Frame[1]=Frame[2]=NULL;
}
void TimgFilterDenoise3dHQ::done(void)
{
 if (Line) aligned_free(Line);Line=NULL;
 if (Frame[0]) aligned_free(Frame[0]);Frame[0]=NULL;
 if (Frame[1]) aligned_free(Frame[1]);Frame[1]=NULL;
 if (Frame[2]) aligned_free(Frame[2]);Frame[2]=NULL;
}

void TimgFilterDenoise3dHQ::precalcCoefs(int nCt, double Dist25)
{
 int *Ct=Coefs[nCt];
 int i;
 double Gamma = log(0.25) / log(1.0 - Dist25/255.0 - 0.00001);

 for (i = -255*16; i <= 255*16; i++)
  {
   double Simil = 1.0 - abs(i) / (16*255.0);
   double C = pow(Simil, Gamma) * 65536.0 * (double)i / 16.0;
   Ct[16*256+i] = int((C<0) ? (C-0.5) : (C+0.5));
  }
 Ct[0] = (Dist25 != 0);
}

void TimgFilterDenoise3dHQ::deNoiseTemporal(
                    const unsigned char *Frame,        // mpi->planes[x]
                    unsigned char *FrameDest,    // dmpi->planes[x]
                    unsigned short *FrameAnt,
                    int W, int H, stride_t sStride, stride_t dStride,
                    int *Temporal)
{
    int X, Y;
    unsigned int PixelDst;

    for (Y = 0; Y < H; Y++){
        for (X = 0; X < W; X++){
            PixelDst = LowPassMul(FrameAnt[X]<<8, Frame[X]<<16, Temporal);
            FrameAnt[X] = (unsigned short)((PixelDst+0x1000007F)>>8);
            FrameDest[X]= (unsigned char)((PixelDst+0x10007FFF)>>16);
        }
        Frame += sStride;
        FrameDest += dStride;
        FrameAnt += W;
    }
}

void TimgFilterDenoise3dHQ::deNoiseSpacial(
                    const unsigned char *Frame,        // mpi->planes[x]
                    unsigned char *FrameDest,    // dmpi->planes[x]
                    unsigned int *LineAnt,       // vf->priv->Line (width bytes)
                    int W, int H, stride_t sStride, stride_t dStride,
                    int *Horizontal, int *Vertical)
{
    int X, Y;
    stride_t sLineOffs = 0, dLineOffs = 0;
    unsigned int PixelAnt;
    unsigned int PixelDst;

    /* First pixel has no left nor top neightbour. */
    PixelDst = LineAnt[0] = PixelAnt = Frame[0]<<16;
    FrameDest[0]= (unsigned char)((PixelDst+0x10007FFF)>>16);

    /* Fist line has no top neightbour, only left. */
    for (X = 1; X < W; X++){
        PixelDst = LineAnt[X] = LowPassMul(PixelAnt, Frame[X]<<16, Horizontal);
       FrameDest[X]= (unsigned char)((PixelDst+0x10007FFF)>>16);
    }

    for (Y = 1; Y < H; Y++){
       unsigned int PixelAnt;
       sLineOffs += sStride, dLineOffs += dStride;
        /* First pixel on each line doesn't have previous pixel */
        PixelAnt = Frame[sLineOffs]<<16;
        PixelDst = LineAnt[0] = LowPassMul(LineAnt[0], PixelAnt, Vertical);
       FrameDest[dLineOffs]= (unsigned char)((PixelDst+0x10007FFF)>>16);

        for (X = 1; X < W; X++){
           int PixelDst;
            /* The rest are normal */
            PixelAnt = LowPassMul(PixelAnt, Frame[sLineOffs+X]<<16, Horizontal);
            PixelDst = LineAnt[X] = LowPassMul(LineAnt[X], PixelAnt, Vertical);
           FrameDest[dLineOffs+X]= (unsigned char)((PixelDst+0x10007FFF)>>16);
        }
    }
}

void TimgFilterDenoise3dHQ::deNoise(const unsigned char *Frame,  // mpi->planes[x]
                                    unsigned char *FrameDest,    // dmpi->planes[x]
                                    unsigned int *LineAnt,       // vf->priv->Line (width bytes)
                                    unsigned short **FrameAntPtr, int *FrameAntStride,
                                    int W, int H, stride_t sStride, stride_t dStride,
                                    int *Horizontal, int *Vertical, int *Temporal)
{
 stride_t sLineOffs = 0, dLineOffs = 0;
 unsigned int PixelAnt;
 unsigned int PixelDst;
 unsigned short* FrameAnt=(*FrameAntPtr);

 if (!FrameAnt)
  {
   *FrameAntStride=(W/16+2)*16;
   (*FrameAntPtr)=FrameAnt=(unsigned short*)aligned_malloc(*FrameAntStride*H*sizeof(unsigned short));
   for (int Y=0;Y<H;Y++)
    {
     unsigned short* dst=&FrameAnt[Y**FrameAntStride];
     const unsigned char* src=Frame+Y*sStride;
     for (int X=0;X<W;X++) dst[X]=(unsigned short)(src[X]<<8);
    }
  }

 if(!Horizontal[0] && !Vertical[0]){
     deNoiseTemporal(Frame, FrameDest, FrameAnt,
                     W, H, sStride, dStride, Temporal);
     return;
 }
 if(!Temporal[0]){
     deNoiseSpacial(Frame, FrameDest, LineAnt,
                    W, H, sStride, dStride, Horizontal, Vertical);
     return;
 }

 // First pixel has no left nor top neightbour. Only previous frame
 LineAnt[0] = PixelAnt = Frame[0]<<16;
 PixelDst = LowPassMul(FrameAnt[0]<<8, PixelAnt, Temporal);
 FrameAnt[0] = (unsigned short)((PixelDst+0x1000007F)>>8);
 FrameDest[0]= (unsigned char)((PixelDst+0x10007FFF)>>16);

 // Fist line has no top neightbour. Only left one for each pixel and/
 for (int X=1;X<W;X++)
  {
   LineAnt[X] = PixelAnt = LowPassMul(PixelAnt, Frame[X]<<16, Horizontal);
   PixelDst = LowPassMul(FrameAnt[X]<<8, PixelAnt, Temporal);
   FrameAnt[X] = (unsigned short)((PixelDst+0x1000007F)>>8);
   FrameDest[X]= (unsigned char)((PixelDst+0x10007FFF)>>16);
  }
 if (Tconfig::cpu_flags&FF_CPU_MMXEXT)
  {
   __align8(unsigned int,PixelAntArray[4]);
   __m64 add1=_mm_set1_pi32(0x010007FF);//010007FF;// LowPassMul 4096
   __m64 add2=_mm_set1_pi32(0x1000007F);//1000007F;// LinePrev
   __m64 add3=_mm_set1_pi32(0x10007FFF);//10007FFF;// FrameDest
   __m64 m0=_mm_setzero_si64();
   for (int Y = 1; Y < H; Y++)
    {
     unsigned short* LinePrev=&FrameAnt[Y**FrameAntStride];
     sLineOffs += sStride, dLineOffs += dStride;
     // First pixel on each line doesn't have previous pixel

     //------------------------start of main loop----------------------------
     PixelAntArray[3] = Frame[sLineOffs]<<16;

     unsigned char *t=(unsigned char *)&Frame[sLineOffs];
     int d=*t<<16;
     int X=0;
     for (; X < W-4; X+=4)
      {
       PixelAntArray[0]=d + Horizontal[((PixelAntArray[3]-d)+0x10007FF)>>12];

       t++;
       d=*t<<16;
       PixelAntArray[1]=d + Horizontal[((PixelAntArray[0]-d)+0x10007FF)>>12];

       t++;
       d=*t<<16;
       PixelAntArray[2]=d + Horizontal[((PixelAntArray[1]-d)+0x10007FF)>>12];

       t++;
       d=*t<<16;
       PixelAntArray[3]=d + Horizontal[((PixelAntArray[2]-d)+0x10007FF)>>12];

       t++;
       d=*t<<16;

       __m64 mm0=*(__m64*)PixelAntArray;    // movq mm0,[PixelAntArray] // 1,2 pixelant in mm0   //fixme test what is faster
       __m64 mm1=*(__m64*)(PixelAntArray+2); // movq mm1,[PixelAntArray+8] // 3,4 pixelant in mm1

       __m64 mm7=add1;
       __m64 mm2=*(__m64*)(LineAnt+X); // 1 LineAnt in mm2
       __m64 mm3=*(__m64*)(LineAnt+X+2); // 3 in mm3

       mm2=_mm_sub_pi32(mm2,mm0);
       mm3=_mm_sub_pi32(mm3,mm1);

       mm2=_mm_add_pi32(mm2,mm7); // d 1,2 + add1
       mm3=_mm_add_pi32(mm3,mm7); // d 2,3 + add1

       mm2=_mm_srli_pi32(mm2,12);
       mm3=_mm_srli_pi32(mm3,12);

       //--------------------------------
       int eax=_mm_cvtsi64_si32(mm2);    //fixme AMD penalty 5-10 cycles
       int ecx=_mm_cvtsi64_si32(mm3);    //fixme AMD penalty 5-10 cycles

       mm2=_mm_unpackhi_pi32(mm2,mm2);  //HD into LD
       mm3=_mm_unpackhi_pi32(mm3,mm3);  //HD into LD

       int ebx=_mm_cvtsi64_si32(mm2);    //fixme AMD penalty 5-10 cycles
       int edx=_mm_cvtsi64_si32(mm3);    //fixme AMD penalty 5-10 cycles

       __m64 mm6=_mm_cvtsi32_si64(Vertical[eax]);
       __m64 mm5=_mm_cvtsi32_si64(Vertical[ecx]);

       mm6=_mm_unpackhi_pi32(mm6,*(__m64*)(Vertical+ebx));
       mm5=_mm_unpackhi_pi32(mm5,*(__m64*)(Vertical+edx));

       //mov ebx,LinePrev

       mm6=_mm_add_pi32(mm6,mm0); // add pixelant = new LineAnt 0,1
       mm5=_mm_add_pi32(mm5,mm1); // add pixelant = new LineAnt 2,3

       //------------------------4x LineAnt rdy in mm6 (0,1) and mm5 (2,3)--------------------
       mm2=mm3=*(__m64*)(LinePrev+X); // 1,2,3,4 LinePrev in mm2

       //mov esi,Temporal

       mm2=_mm_unpacklo_pi16(mm2,m0);
       mm3=_mm_unpackhi_pi16(mm3,m0);

       mm2=_mm_slli_pi32(mm2,8); // shift left <<8
       mm3=_mm_slli_pi32(mm3,8); // shift left <<8

       mm2=_mm_sub_pi32(mm2,mm6); // sub LineAnt 0,1 left from above
       mm3=_mm_sub_pi32(mm3,mm5); // sub LineAnt 2,3 left from above

       mm2=_mm_add_pi32(mm2,mm7); // d 1,2 + add1
       mm3=_mm_add_pi32(mm3,mm7); // d 2,3 + add1

       mm2=_mm_srli_pi32(mm2,12);
       mm3=_mm_srli_pi32(mm3,12);

       eax=_mm_cvtsi64_si32(mm2);    //fixme AMD penalty 5 cycles
       ecx=_mm_cvtsi64_si32(mm3);    //fixme AMD penalty 5 cycles

       mm2=_mm_unpackhi_pi32(mm2,mm2); //HD into LD
       mm3=_mm_unpackhi_pi32(mm3,mm3); //HD into LD

       ebx=_mm_cvtsi64_si32(mm2);    //fixme AMD penalty 5 cycles
       edx=_mm_cvtsi64_si32(mm3);    //fixme AMD penalty 5 cycles

       mm0=_mm_cvtsi32_si64(Temporal[eax]);
       mm1=_mm_cvtsi32_si64(Temporal[ecx]);

       mm0=_mm_unpacklo_pi32(mm0,*(__m64*)(Temporal+ebx));
       mm1=_mm_unpacklo_pi32(mm1,*(__m64*)(Temporal+edx));

       //mov ecx,LineAnt

       mm0=_mm_add_pi32(mm0,mm6); // add LinAnt[x] and coef* [d] mm0
       mm1=_mm_add_pi32(mm1,mm5); // add LinAnt[x] and coef* [d] mm1

       //write LinAnt[x] back now since we dont need it anymore
       *(__m64*)(LineAnt+X)=mm6;
       *(__m64*)(LineAnt+X+2)=mm5;

       //------------------------4x PixelDst rdy in mm0 (0,1) and mm1 (2,3)--------------------
       //movq PixelDstTest,mm0
       //movq [PixelDstTest+8],mm1
       mm2=_mm_shuffle_pi16(mm0,/*11100100B*/0xE4);
       mm6=add2;

       mm3=_mm_shuffle_pi16(mm1,/*11100100B*/0xE4);
       mm5=add3;

       mm2=_mm_add_pi32(mm2,mm6);
       mm3=_mm_add_pi32(mm3,mm6);

       mm2=_mm_srli_pi32(mm2,8);
       mm3=_mm_srli_pi32(mm3,8);

       mm2=_mm_shuffle_pi16(mm2,0xf8);
       mm3=_mm_shuffle_pi16(mm3,0xf8); // cast! to 16 bit signed short
       mm2=_mm_unpacklo_pi32(mm2,mm3);

       //------------------------4x LinePrev rdy in mm2 (0,1,2,3) --------------------
       mm0=_mm_add_pi32(mm0,mm5);
       mm1=_mm_add_pi32(mm1,mm5);

       //mov ebx,LinePrev

       mm0=_mm_srli_pi32(mm0,16);
       mm1=_mm_srli_pi32(mm1,16);

       //mov esi,FrameDest

       __m64 mm4=_mm_shuffle_pi16(mm0,0x2);
       mm3=_mm_shuffle_pi16(mm1,0x2);

       mm0=_mm_unpacklo_pi8(mm0,mm4);
       mm1=_mm_unpacklo_pi8(mm1,mm3);

       //mov eax,[dLineOffs]

       mm0=_mm_unpacklo_pi32(mm0,mm1);

       *(__m64*)(LinePrev+X)=mm2;

       mm0=_mm_shuffle_pi16(mm0,0x8);

       //add eax,edi

       *(int*)(FrameDest+dLineOffs+X)=_mm_cvtsi64_si32(mm0);
       //------------------------4x FrameDest rdy in mm0 Low (0,1,2,3)--------------------
       //movntq LineAntTest, mm6
       //movntq [LineAntTest+8], mm5
       //movntq LinePrevTest,mm2
       //movd FrameDestTest,mm0
      }// X lopp ends
     for (;X<W;X++)
      FrameDest[dLineOffs+X]=*t++;
    } //Y loop ends
   _mm_empty();
  }
 else
  for (int Y=1;Y<H;Y++)
   {
    unsigned short* LinePrev=&FrameAnt[Y**FrameAntStride];
    sLineOffs += sStride, dLineOffs += dStride;
    // First pixel on each line doesn't have previous pixel
    PixelAnt = Frame[sLineOffs]<<16;
    LineAnt[0] = LowPassMul(LineAnt[0], PixelAnt, Vertical);
    PixelDst = LowPassMul(LinePrev[0]<<8, LineAnt[0], Temporal);
    LinePrev[0] = (unsigned short)((PixelDst+0x1000007F)>>8);
    FrameDest[dLineOffs]= (unsigned char)((PixelDst+0x10007FFF)>>16);

    for (int X = 1; X < W; X++)
     {
      // The rest are normal
      PixelAnt = LowPassMul(PixelAnt, Frame[sLineOffs+X]<<16, Horizontal);
      LineAnt[X] = LowPassMul(LineAnt[X], PixelAnt, Vertical);
      PixelDst = LowPassMul(LinePrev[X]<<8, LineAnt[X], Temporal);
      LinePrev[X] = (unsigned short)((PixelDst+0x1000007F)>>8);
      FrameDest[dLineOffs+X]= (unsigned char)((PixelDst+0x10007FFF)>>16);
     }
   }
}

void TimgFilterDenoise3dHQ::deNoise(TffPict &pict,unsigned int dx,unsigned int dy,const unsigned char *src[4],stride_t stride1[4],unsigned char *dst[4],stride_t stride2[4])
{
 if (!Line) Line=(unsigned int*)aligned_malloc(dx*sizeof(unsigned int));

 static const int coeffIdxs[3]={0,2,2};

 for (unsigned int i=0;i<3;i++)
  deNoise(src[i],dst[i],Line,&Frame[i],&FrameStride[i],
          dx>>pict.cspInfo.shiftX[i],dy>>pict.cspInfo.shiftY[i],
          stride1[i],stride2[i],
          Coefs[coeffIdxs[i]  ],
          Coefs[coeffIdxs[i]  ],
          Coefs[coeffIdxs[i]+1]);
}
