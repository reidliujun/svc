/*
 * Copyright (c) 2006 Milan Cutka
 * uses code from gradfun2db Copyright(c) 2006 prunedtree
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
#include "TimgFilterGradfun.h"
#include "simd.h"
#include "TgradFunSettings.h"

//============================== TimgFilterGradfun::TgradFun ==============================
TimgFilterGradfun::TgradFun::TgradFun(unsigned int nMaxWidth,unsigned int nMaxHeight, float dThreshold):pBuffer(NULL)
{
 ok=false;
 /* sanity checks on width & height ( at least 16 ) */
 if ( nMaxWidth < 16 || nMaxHeight < 16 )
  return;

 /* mod 2 vertically, mod 8 horizontally */
 if ( nMaxWidth & 7 || nMaxHeight & 1 )
  return;

 if ( dThreshold < 0 )
  return;

 nMaxWidth = nMaxWidth;
 nMaxHeight = nMaxHeight;
 nThreshold = ( 1 << ( 25 ) ) / ((int)( 1024 * dThreshold ));
 pBuffer = (int32_t*)aligned_malloc( sizeof( *pBuffer ) * nMaxWidth * nMaxHeight / 4);

 nBufferPitch = nMaxWidth / 2;
}
TimgFilterGradfun::TgradFun::~TgradFun()
{
 if (pBuffer) aligned_free(pBuffer);
}

void TimgFilterGradfun::TgradFun::gf_prepare_mmx(int32_t *pDst, stride_t nDstPitch, const uint8_t *pSrc, stride_t nSrcPitch, unsigned int nWidth, unsigned int nHeight)
{
   stride_t nDstOffset;
   stride_t nSrcOffset;

        unsigned char *esi= (unsigned char*)pSrc;            // esi <-- pSrc + 2 * y * nSrcPitch
        unsigned char *edi= (unsigned char*)pDst;            // edi <-- pDst + y * nDstPitch

   stride_t                  ebx= nSrcPitch;
   unsigned char *edx= esi;
   edx+= ebx;                      // edx <-- pSrc + (2 * y + 1) * nSrcPitch

   int ecx=nWidth;
   ebx+= ebx;
   ebx-= ecx;
   nSrcOffset= ebx;      // nSrcOffset <-- 2 * nSrcOffset - nWidth

   ebx= nDstPitch;
   ebx<<=2;
   unsigned char *eax= edi;
   eax-= ebx;                      // eax <-- pDst + (y - 1) * nDstPitch

   ebx-= ecx;
   ebx-= ecx;
   nDstOffset= ebx;      // nDstOffset <-- nDstPitch - 2 * nWidth

        int ebp= nHeight;
        ebp>>=1;
        ebp--;

        esi= esi + ecx;
        edx= edx + ecx;
        edi= edi + 2 * ecx;
        eax= eax + 2 * ecx;

  __m64 mm0,mm1,mm2,mm3,mm6,mm7;
        pxor                 (mm0, mm0);

loop_y:

        pxor                 (mm7, mm7);

        ebx= nSrcOffset;
        edx+= ebx;
        esi+= ebx;
        ebx= nDstOffset;
        eax+= ebx;
        edi+= ebx;

        ecx= nWidth;
        ecx>>= 3;                        // ecx <-- nWidth / 8//

loop_x:

        movq                  (mm1, esi);
        movq                  (mm2, mm1);
        psllw		      (      mm1, 8);
        psrlw		      (     mm1, 8);
        psrlw		      (      mm2, 8);
        paddw		      (      mm2, mm1);
        movq		      (      mm1, edx);
        movq		      (      mm3, mm1);
        psllw		      (      mm1, 8);
        psrlw		      (      mm1, 8);
        psrlw		      (      mm3, 8);
        paddw		      (      mm3, mm1);
        paddw		      (      mm2, mm3);
        movq		      (      mm1, mm2);
        punpcklwd	      (   mm1, mm0);
        punpckhwd	      (   mm2, mm0);
        paddd		      (      mm1, mm7);
        movq		      (      mm6, mm1);
        psllq		      (      mm6, 32);
        paddd		      (      mm1, mm6);
        movq		      (      mm6, mm1);
        psrlq		      (      mm6, 32);
        paddd		      (      mm2, mm6);
        movq		      (      mm6, mm2);
        psllq		      (      mm6, 32);
        paddd		      (      mm2, mm6);
        movq		      (      mm7, mm2);
        psrlq		      (      mm7, 32);
        paddd	              ( mm1, eax);
        paddd	              ( mm2, eax + 8);
        movq	              ( edi, mm1);
        movq	              ( edi + 8, mm2);

        eax+= 16;
        edi+= 16;
        esi+= 8;
        edx+= 8;
        ecx--;
        if (ecx!=0)
         goto loop_x;//	jnz                  .loop_x

        ebp--;
        if (ebp!=0)
         goto loop_y;//	jnz                  .loop_y

}


void TimgFilterGradfun::TgradFun::gf_render_mmx(uint8_t *pDst, stride_t nDstPitch, const int32_t *pSrc, stride_t nSrcPitch, unsigned int nWidth, unsigned int nHeight, int nThr)
{
   int nLoops;

   stride_t eax= nDstPitch;
   unsigned char *edi= pDst;
   edi= edi + eax * 8;
   edi= edi + eax * 8 + 16;           // edi <-- pDst + 16 * nDstPitch + 16

        eax=nSrcPitch;
        eax<<=2;
        unsigned char *esi=(unsigned char*)pSrc;                  // esi <-- pSrc
        unsigned char *ebx= esi + 8 * eax;
        ebx= ebx + 8 * eax;                // ebx <-- pSrc + 16 * nSrcPitch

        int ecx= nWidth;
        ecx-= 32;
        ecx>>=2;
        nLoops= ecx;
 __m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7;
 __m64 GF_WORD_127=_mm_set1_pi16(0x7f);

        movd                 (mm6, nThr);
        movq                 (mm5, GF_WORD_127);
        punpcklwd            (mm6, mm6);
        punpckldq            (mm6, mm6);

        pxor                 (mm0, mm0);
        pxor                 (mm7, mm7);

        int ebp= nHeight;
        ebp-= 32;

loop_y:

        int edx= 0;
        ecx= nLoops;

loop_x:

   pxor	        (       mm1, mm1);
   paddd	(               mm1, esi + edx * 2);
   psubd	(               mm1, esi + edx * 2 + 64);
   psubd	(               mm1, ebx + edx * 2);
   paddd	(               mm1, ebx + edx * 2 + 64);
   psrld	(               mm1, 3);
   movq	        (       mm2, mm1);
   psllq	(               mm2, 16);
   por		(            mm1, mm2);
   movd		(            mm2, edi + edx);
   punpcklbw	(         mm2, mm0);
   psllw	(	            mm2, 7);
   movq		(            mm4, mm1);
   movq		(            mm3, mm2);
   psubusw	(	         mm4, mm2);
   psubusw	(	         mm3, mm1);
   por		(	         mm3, mm4);
   pmulhw	(	         mm3, mm6);
   pminsw	(	         mm3, mm5);
   movq		(            mm4, mm5);
   psubw	(	            mm4, mm3);
   pmullw	(	         mm4, mm4);
   psllw	(	            mm4, 1);
   psubw	(	            mm1, mm2);
   pmulhw	(	         mm1, mm4);
   psrlw	(	            mm2, 1);
   paddw	(	            mm1, mm2);
   psllw	(	            mm1, 1);
   paddw	(	            mm1, mm7);
   movq		(           mm2, mm1);
   psraw	(	            mm2, 7);
   movq		(            mm4, mm2);
   packuswb	(            mm4, mm0);
   movd		(            edi + edx, mm4);
   psllw	(	            mm2, 7);
   psubw	(	            mm1, mm2);
   movq		(            mm7, mm1);

   edx+= 4;
   ecx--;
   if (ecx!=0)
    goto loop_x;// jnz                  .loop_x

   edi+= nDstPitch;             // pDst : nextline

   if ((ebp&1)==0)
    goto no_src_next;//jz                   .no_src_next

   esi+= eax;                            // pSrcs : nextline, half the time
   ebx+= eax;

no_src_next:

   ebp--;

   if (ebp!=0)
    goto loop_y;// jnz                  .loop_y

   _mm_empty();
}

int TimgFilterGradfun::TgradFun::GF_filter(uint8_t *pPlane, stride_t nPitch, unsigned int nWidth, unsigned int nHeight)
{
        uint8_t* pSrc = pPlane;
        uint8_t* pSrcn = pPlane + nPitch;

        int nAcc = 0;
        unsigned int x;

   /* sanity checks : width & height at least 16 ) */
   if ( nWidth < 16 || nHeight < 16 )
      return -1;

   /* width mod 8, height mod 2 */
   if ( nWidth & 7 || nHeight & 1 )
      return -1;

   /* other sanity checks */
   if (!pPlane)
      return -1;

        for ( x = 0; x < nWidth / 2; x++ )
        {
                nAcc += pSrc[x * 2] + pSrc[x * 2 + 1] + pSrcn[x * 2] + pSrcn[x * 2 + 1];
                pBuffer[x] = nAcc;
        }

   gf_prepare_mmx( pBuffer, nBufferPitch, pPlane, nPitch, nWidth, nHeight );
   gf_render_mmx( pPlane, nPitch, pBuffer, nBufferPitch, nWidth, nHeight, nThreshold );
   return 0;
}

//=================================== TimgFilterGradfun ===================================
TimgFilterGradfun::TimgFilterGradfun(IffdshowBase *Ideci,Tfilters *Iparent):
 TimgFilter(Ideci,Iparent),
 gradFun(NULL),
 oldthresh(-1)
{
}
void TimgFilterGradfun::done(void)
{
 if (gradFun) delete gradFun;gradFun=NULL;
}
void TimgFilterGradfun::onSizeChange(void)
{
 done();
}
HRESULT TimgFilterGradfun::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TgradFunSettings *cfg=(const TgradFunSettings*)cfg0;
 init(pict,cfg->full,cfg->half);
 if (is(pict,cfg0))
  {
   unsigned char *dst[4];
   bool cspChange=getCurNext(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,COPYMODE_DEF,dst);
   if (cspChange || oldthresh!=cfg->threshold)
    {
     oldthresh=cfg->threshold;
     done();
    }
   if (!gradFun)
    gradFun=new TgradFun(dx2[0]+edgeSize*2,dy2[0]+edgeSize*2,cfg->threshold/100.0f); //is the first plane is always largest?
   bool edge=cfg->full && !cfg->half;
   if (edge)
    pict.createEdge(edgeSize,edgebuf);

   for (unsigned int i=0;i<pict.cspInfo.numPlanes;i++)
    gradFun->GF_filter(edge?pict.edgeData[i]:dst[i],edge?pict.stride[i]:stride2[i],edge?(pict.rectEdge.dx>>pict.cspInfo.shiftX[i])*pict.cspInfo.Bpp:dx2[i],edge?(pict.rectEdge.dy>>pict.cspInfo.shiftY[i]):dy2[i]);
  }
 return parent->deliverSample(++it,pict);
}
