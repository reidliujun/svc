/*
 * Copyright (c) 2004-2006 Milan Cutka
 * based on DGbob Plugin for Avisynth by Donald A. Graft
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
#include "TimgFilterDGbob.h"
#include "TdeinterlaceSettings.h"
#include "TimgFilters.h"

TimgFilterDGbob::TimgFilterDGbob(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 n=0;
 for (int i=0;i<countof(picts0);i++) picts0[i]=new TtempPict;
 picts=picts0+2;
 do_deinterlace = 0;
}
TimgFilterDGbob::~TimgFilterDGbob()
{
 done();
 for (int i=0;i<countof(picts0);i++) delete picts0[i];
}
void TimgFilterDGbob::onSizeChange(void)
{
 done();
}
void TimgFilterDGbob::done(void)
{
 for (int i=0;i<countof(picts0);i++) picts0[i]->done();
 n=0;
 do_deinterlace = 0;
}
HRESULT TimgFilterDGbob::process(TfilterQueue::iterator it,TffPict &pict0,const TfilterSettingsVideo *cfg0)
{
 const TdeinterlaceSettings *cfg=(const TdeinterlaceSettings*)cfg0;
 if ((pict0.fieldtype & FIELD_TYPE::PROGRESSIVE_FRAME) && !cfg->deinterlaceAlways)
 {
  done();
  return parent->deliverSample(++it,pict0);
 }
 init(pict0,cfg->full,0);
 const unsigned char *src_[4];
 bool cspChanged=getCur(FF_CSP_420P|FF_CSP_YUY2|FF_CSP_RGB32,pict0,cfg->full,src_);
 if (cspChanged)
  done();

 int order=pict0.fieldtype&FIELD_TYPE::INT_BFF?1:0,fcnt=cfg->dgbobMode==0?1:2;
 REFERENCE_TIME rtStart=pict0.rtStart,rtDuration=(pict0.rtStop-pict0.rtStart)/fcnt;
 if (!do_deinterlace)
  {
   picts[SRC]->p.copyFrom(pict0,picts[SRC]->buf);
   picts[NXT]->p.copyFrom(pict0,picts[NXT]->buf);
   do_deinterlace = 1;
  }
 for (int f=0;f<fcnt;f++,order=1-order)
  {
   TffPict pict=pict0;
   picts[NXTNXT]->p.copyFrom(pict,picts[NXTNXT]->buf);
   unsigned char *dst[4];
   cspChanged|=getNext(csp1,pict,cfg->full,dst);
   if (picts[SRC]->buf.size())
    {
     int n=this->n-2;
     int frame=n;
     TffPict *src   =&picts[0]->p;
     TffPict *prv   =&picts[n > 0 ? - 1 : 0]->p;
     TffPict *prvprv=&picts[n > 1 ? - 2 : 0]->p;
     TffPict *nxt   =&picts[+1]->p;
     TffPict *nxtnxt=&picts[+2]->p;
     int D=n==0?0:cfg->dgbobThreshold;
     for (unsigned int z=0;z<pict.cspInfo.numPlanes;z++)
      {
       const unsigned char *srcp_saved,*srcp;
       srcp_saved=srcp=src->data[z];
       unsigned char *dstp_saved,*dstp;
       dstp_saved=dstp=dst[z];
       int w=dx2[z]*pict.cspInfo.Bpp;
       int h=dy2[z];
       static const int AP=30;
       static const int T=6;
       if ((cfg->dgbobMode > 0) && (frame & 1))
        {
         // Process odd-numbered frames.
         // Copy field from current frame.
         srcp = srcp_saved + order * src->stride[z];
         dstp = dstp_saved + order * stride2[z];
         for (int y=0;y<h;y+=2)
          {
           memcpy(dstp, srcp, w);
           srcp += 2*src->stride[z];
           dstp += 2*stride2[z];
          }
         // Copy through the line that will be missed below.
         memcpy(dstp_saved + (1-order)*(h-1)*stride2[z], srcp_saved + (1-order)*(h-1)*src->stride[z], w);
         // For the other field choose adaptively between using the previous field or the interpolant from the current field.
         const unsigned char *prvp = prv->data[z] + prv->stride[z] + order*prv->stride[z];
         const unsigned char *prvpp = prvp - prv->stride[z];
         const unsigned char *prvpn = prvp + prv->stride[z];
         const unsigned char *prvprvp = prvprv->data[z] + prvprv->stride[z] + order*prvprv->stride[z];
         const unsigned char *prvprvpp = prvprvp - prvprv->stride[z];
         const unsigned char *prvprvpn = prvprvp + prvprv->stride[z];
         const unsigned char *nxtp = nxt->data[z] + nxt->stride[z] + order*nxt->stride[z];
         const unsigned char *nxtpp = nxtp - nxt->stride[z];
         const unsigned char *nxtpn = nxtp + nxt->stride[z];
         const unsigned char *nxtnxtp = nxtnxt->data[z] + nxtnxt->stride[z] + order*nxtnxt->stride[z];
         const unsigned char *nxtnxtpp = nxtnxtp - nxtnxt->stride[z];
         const unsigned char *nxtnxtpn = nxtnxtp + nxtnxt->stride[z];
         const unsigned char *srcp =  srcp_saved + src->stride[z] + order*src->stride[z];
         const unsigned char *srcpp = srcp - src->stride[z];
         const unsigned char *srcpn = srcp + src->stride[z];
         unsigned char *dstp =  dstp_saved + stride2[z] + order*stride2[z];
         for (int y=0;y<h-2;y+=2)
          {
           for (int x=0;x<w;x++)
            {
             if (abs(srcp[x] - nxtp[x]) < D
                 //&& abs(srcp[x] - nxtnxtp[x]) < D
                 //&& abs(prvp[x] - nxtp[x]) < D
                 && abs(srcpn[x] - prvprvpn[x]) < D
                 && abs(srcpp[x] - prvprvpp[x]) < D
                 && abs(srcpn[x] - nxtnxtpn[x]) < D
                 && abs(srcpp[x] - nxtnxtpp[x]) < D
                 && abs(srcpn[x] - prvpn[x]) < D
                 && abs(srcpp[x] - prvpp[x]) < D
                 && abs(srcpn[x] - nxtpn[x]) < D
                 && abs(srcpp[x] - nxtpp[x]) < D
                )
              {
               if (cfg->dgbobAP)
                {
                 int v1 = (int) srcp[x] - AP;
                 if (v1 < 0) v1 = 0;
                 int v2 = (int) srcp[x] + AP;
                 if (v2 > 235) v2 = 235;
                 if ((v1 > srcpp[x] && v1 > srcpn[x]) || (v2 < srcpp[x] && v2 < srcpn[x]))
                  {
                   dstp[x] = (unsigned char)(((int)srcpp[x] + srcpn[x]) >> 1);
                   //if (x & 1) dstp[x] = 100; else dstp[x] = 235;
                  }
                 else
                  {
                   dstp[x] = srcp[x];
                   //if (x & 1) dstp[x] = 100; else dstp[x] = 235;
                  }
                }
               else
                {
                 dstp[x] = srcp[x];
                 //if (x & 1) dstp[x] = 100; else dstp[x] = 235;
                }
              }
             else
              {
               int v1 = (int) srcp[x] - T;
               if (v1 < 0) v1 = 0;
               int v2 = (int) srcp[x] + T;
               if (v2 > 235) v2 = 235;
               if ((v1 > srcpp[x] && v1 > srcpn[x]) || (v2 < srcpp[x] && v2 < srcpn[x]))
                {
                 dstp[x] = (unsigned char)(((int)srcpp[x] + srcpn[x]) >> 1);
                }
               else
                {
                 dstp[x] = srcp[x];
                 //if (x & 1) dstp[x] = 128; else dstp[x] = 235;
                }
              }
            }
           prvp     += 2*prv->stride[z];
           prvpp    += 2*prv->stride[z];
           prvpn    += 2*prv->stride[z];
           prvprvpp += 2*prvprv->stride[z];
           prvprvpn += 2*prvprv->stride[z];
           nxtp     += 2*nxt->stride[z];
           nxtpp    += 2*nxt->stride[z];
           nxtpn    += 2*nxt->stride[z];
           nxtnxtpp += 2*nxtnxt->stride[z];
           nxtnxtpn += 2*nxtnxt->stride[z];
           srcp     += 2*src->stride[z];
           srcpp    += 2*src->stride[z];
           srcpn    += 2*src->stride[z];
           dstp     += 2*stride2[z];
          }
        }
       else
        {
         // Process even-numbered frames.
         // Copy field from current frame.
         srcp = srcp_saved + (1-order) * src->stride[z];
         dstp = dstp_saved + (1-order) * stride2[z];
         for (int y = 0; y < h; y+=2)
          {
           memcpy(dstp, srcp, w);
           srcp += 2*src->stride[z];
           dstp += 2*stride2[z];
          }
         // Copy through the line that will be missed below.
         memcpy(dstp_saved + order*(h-1)*stride2[z], srcp_saved + order*(h-1)*src->stride[z], w);
         // For the other field choose adaptively between using the previous field or the interpolant from the current field.
         const unsigned char *prvp = prv->data[z] + prv->stride[z] + (1-order)*prv->stride[z];
         const unsigned char *prvpp = prvp - prv->stride[z];
         const unsigned char *prvpn = prvp + prv->stride[z];
         const unsigned char *prvprvp = prvprv->data[z] + prvprv->stride[z] + (1-order)*prvprv->stride[z];
         const unsigned char *prvprvpp = prvprvp - prvprv->stride[z];
         const unsigned char *prvprvpn = prvprvp + prvprv->stride[z];
         const unsigned char *nxtp = nxt->data[z] + nxt->stride[z] + (1-order)*nxt->stride[z];
         const unsigned char *nxtpp = nxtp - nxt->stride[z];
         const unsigned char *nxtpn = nxtp + nxt->stride[z];
         const unsigned char *nxtnxtp = nxtnxt->data[z] + nxtnxt->stride[z] + (1-order)*nxtnxt->stride[z];
         const unsigned char *nxtnxtpp = nxtnxtp - nxtnxt->stride[z];
         const unsigned char *nxtnxtpn = nxtnxtp + nxtnxt->stride[z];
         const unsigned char *srcp =  srcp_saved + src->stride[z] + (1-order)*src->stride[z];
         const unsigned char *srcpp = srcp - src->stride[z];
         const unsigned char *srcpn = srcp + src->stride[z];
         unsigned char *dstp =  dstp_saved + stride2[z] + (1-order)*stride2[z];
         for (int y = 0; y < h - 2; y+=2)
          {
           for (int x = 0; x < w; x++)
            {
             if (abs(srcp[x] - prvp[x]) < D
                 //&& abs(srcp[x] - prvprvp[x]) < D
                 //&& abs(prvp[x] - nxtp[x]) < D
                 && abs(srcpn[x] - prvprvpn[x]) < D
                 && abs(srcpp[x] - prvprvpp[x]) < D
                 && abs(srcpn[x] - nxtnxtpn[x]) < D
                 && abs(srcpp[x] - nxtnxtpp[x]) < D
                 && abs(srcpn[x] - prvpn[x]) < D
                 && abs(srcpp[x] - prvpp[x]) < D
                 && abs(srcpn[x] - nxtpn[x]) < D
                 && abs(srcpp[x] - nxtpp[x]) < D
                )
              {
               if (cfg->dgbobAP)
                {
                 int v1 = (int) prvp[x] - AP;
                 if (v1 < 0) v1 = 0;
                 int v2 = (int) prvp[x] + AP;
                 if (v2 > 235) v2 = 235;
                 if ((v1 > srcpp[x] && v1 > srcpn[x]) || (v2 < srcpp[x] && v2 < srcpn[x]))
                  {
                   dstp[x] = (unsigned char)(((int)srcpp[x] + srcpn[x]) >> 1);
                   //if (x & 1) dstp[x] = 100; else dstp[x] = 235;
                  }
                 else
                  {
                   dstp[x] = prvp[x];
                   //if (x & 1) dstp[x] = 128; else dstp[x] = 235;
                  }
                }
               else
                {
                 dstp[x] = prvp[x];
                 //if (x & 1) dstp[x] = 128; else dstp[x] = 235;
                }
              }
             else
              {
               int v1 = (int) prvp[x] - T;
               if (v1 < 0) v1 = 0;
               int v2 = (int) prvp[x] + T;
               if (v2 > 235) v2 = 235;
               if ((v1 > srcpp[x] && v1 > srcpn[x]) || (v2 < srcpp[x] && v2 < srcpn[x]))
                {
                 dstp[x] = (unsigned char)(((int)srcpp[x] + srcpn[x]) >> 1);
                }
               else
                {
                 dstp[x] = prvp[x];
                 //if (x & 1) pp[x] = 128; else dstp[x] = 235;
                }
              }
            }
           prvp     += 2*prv->stride[z];
           prvpp    += 2*prv->stride[z];
           prvpn    += 2*prv->stride[z];
           prvprvpp += 2*prvprv->stride[z];
           prvprvpn += 2*prvprv->stride[z];
           nxtp     += 2*nxt->stride[z];
           nxtpp    += 2*nxt->stride[z];
           nxtpn    += 2*nxt->stride[z];
           nxtnxtpp += 2*nxtnxt->stride[z];
           nxtnxtpn += 2*nxtnxt->stride[z];
           srcp     += 2*src->stride[z];
           srcpp    += 2*src->stride[z];
           srcpn    += 2*src->stride[z];
           dstp     += 2*stride2[z];
          }
        }
      }
     picts[SRC]->p.copyFrom(pict,picts[SRC]->buf);
    }
   else
    pict.clear(true);
   TtempPict *pp=picts0[0];memmove(picts0,picts0+1,4*sizeof(picts0[0]));picts0[4]=pp;
   pict.rtStart=rtStart;pict.rtStop=pict.rtStart+rtDuration;rtStart+=rtDuration;
   HRESULT hr=parent->deliverSample(++it,pict);
   if (FAILED(hr))
    return hr;
   n++;
  }
 return S_FALSE;
}

void TimgFilterDGbob::onSeek(void)
{
 done();
}
