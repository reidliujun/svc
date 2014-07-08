/*
 * Copyright (c) 2004-2006 Milan Cutka
 * based on KernelDeint Plugin for Avisynth by Donald A. Graft
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
#include "TimgFilterKernelDeint.h"
#include "TdeinterlaceSettings.h"
#include "Tdll.h"
/*
TimgFilterKernelDeint::TimgFilterKernelDeint(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 prvbuf=NULL;prvbuflen=NULL;
 n=0;
}
TimgFilterKernelDeint::~TimgFilterKernelDeint()
{
 done();
}
void TimgFilterKernelDeint::onSizeChange(void)
{
 done();
}
void TimgFilterKernelDeint::done(void)
{
 if (prvbuf) aligned_free(prvbuf);prvbuf=NULL;prvbuflen=0;
 n=0;
}
HRESULT TimgFilterKernelDeint::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TdeinterlaceSettings *cfg=(const TdeinterlaceSettings*)cfg0;
 init(pict,cfg->full,cfg->half);
 const unsigned char *src[4];
 bool cspChanged=getCur(FF_CSP_420P|FF_CSP_YUY2|FF_CSP_RGB32,pict,cfg->full,src);
 unsigned char *dst[4];
 cspChanged|=getNext(csp1,pict,cfg->full,dst);
 if (cspChanged)
  done();

 if (!prvbuf)
  prv.alloc(dx1[0],dy1[0],csp1,0,prvbuf,prvbuflen);

 int order=cfg->swapfields?1:0;
 for (unsigned int z=0;z<pict.cspInfo.numPlanes;z++)
  {
   unsigned int plane=z;
   const unsigned char *srcp,*srcp_saved;
   srcp=srcp_saved=src[z];

   int src_pitch = stride1[plane];
   unsigned char *dstp,*dstp_saved;
   dstp=dstp_saved=dst[plane];
   int dst_pitch = stride2[plane];

   int w = dx1[plane]*pict.cspInfo.Bpp;
   int h = dy1[plane];
   srcp = srcp_saved  + (1-order) * src_pitch;
   dstp = dstp_saved  + (1-order) * dst_pitch;
   for (int y=0;y<h;y+=2)
    {
     memcpy(dstp, srcp, w);
     srcp += 2*src_pitch;
     dstp += 2*dst_pitch;
    }
   // Copy through the lines that will be missed below.
   memcpy(dstp_saved + order*dst_pitch, srcp_saved + (1-order)*src_pitch, w);
   memcpy(dstp_saved + (2+order)*dst_pitch, srcp_saved + (3-order)*src_pitch, w);
   memcpy(dstp_saved + (h-2+order)*dst_pitch, srcp_saved + (h-1-order)*src_pitch, w);
   memcpy(dstp_saved + (h-4+order)*dst_pitch, srcp_saved + (h-3-order)*src_pitch, w);
   // For the other field choose adaptively between using the previous field or the interpolant from the current field.
   unsigned char *prvp = prv.data[plane] + 5*prv.stride[plane] - (1-order)*prv.stride[plane];
   unsigned char *prvpp = prvp - prv.stride[plane];
   unsigned char *prvppp = prvp - 2*prv.stride[plane];
   unsigned char *prvp4p = prvp - 4*prv.stride[plane];
   unsigned char *prvpn = prvp + prv.stride[plane];
   unsigned char *prvpnn = prvp + 2*prv.stride[plane];
   unsigned char *prvp4n = prvp + 4*prv.stride[plane];
   srcp = srcp_saved + 5*src_pitch - (1-order)*src_pitch;
   const unsigned char *srcpp = srcp - src_pitch;
   const unsigned char *srcppp = srcp - 2*src_pitch;
   const unsigned char *srcp3p = srcp - 3*src_pitch;
   const unsigned char *srcp4p = srcp - 4*src_pitch;
   const unsigned char *srcpn = srcp + src_pitch;
   const unsigned char *srcpnn = srcp + 2*src_pitch;
   const unsigned char *srcp3n = srcp + 3*src_pitch;
   const unsigned char *srcp4n = srcp + 4*src_pitch;
   dstp =  dstp_saved  + 5*dst_pitch - (1-order)*dst_pitch;
   for (int y = 5 - (1-order); y <= h - 5 - (1-order); y+=2)
    {
     for (int x = 0; x < w; x++)
      {
       if ((cfg->kernelThreshold == 0) || (n == 0) ||
           (abs((int)prvp[x] - (int)srcp[x]) > cfg->kernelThreshold) ||
           (abs((int)prvpp[x] - (int)srcpp[x]) > cfg->kernelThreshold) ||
           (abs((int)prvpn[x] - (int)srcpn[x]) > cfg->kernelThreshold))
        {
         if (cfg->kernelMap)
          {
           int g = x & ~3;
           if (csp1==FF_CSP_RGB32)
            {
             dstp[g++] = 255;
             dstp[g++] = 255;
             dstp[g++] = 255;
             dstp[g] = 255;
             x = g;
            }
           else if (csp1==FF_CSP_YUY2)
            {
             dstp[g++] = 235;
             dstp[g++] = 128;
             dstp[g++] = 235;
             dstp[g] = 128;
             x = g;
            }
           else
            {
             if (plane == 0) dstp[x] = 235;
             else dstp[x] = 128;
            }
          }
         else
          {
           int hi,lo;
           if (csp1==FF_CSP_RGB32)
            {
             hi = 255;
             lo = 0;
            }
           else if (csp_isYUVplanar(csp1))
            {
             hi = (plane == 0) ? 235 : 240;
             lo = 16;
            }
           else if (csp1==FF_CSP_YUY2)
            {
             hi = (x & 1) ? 240 : 235;
             lo = 16;
            }
           else
            {
             hi=255;
             lo=0;
            }
           if (cfg->kernelSharp)
            {
             double valf;
             if (cfg->kernelTwoway)
              valf = + 0.526*((int)srcpp[x] + (int)srcpn[x])
                 + 0.170*((int)srcp[x] + (int)prvp[x])
                 - 0.116*((int)srcppp[x] + (int)srcpnn[x] + (int)prvppp[x] + (int)prvpnn[x])
                 - 0.026*((int)srcp3p[x] + (int)srcp3n[x])
                 + 0.031*((int)srcp4p[x] + (int)srcp4n[x] + (int)prvp4p[x] + (int)prvp4n[x]);
             else
              valf = + 0.526*((int)srcpp[x] + (int)srcpn[x])
                 + 0.170*((int)prvp[x])
                 - 0.116*((int)prvppp[x] + (int)prvpnn[x])
                 - 0.026*((int)srcp3p[x] + (int)srcp3n[x])
                 + 0.031*((int)prvp4p[x] + (int)prvp4p[x]);
             if (valf > hi) valf = hi;
             else if (valf < lo) valf = lo;
             dstp[x] = (unsigned char) valf;
            }
           else
            {
             int val;
             if (cfg->kernelTwoway)
              val = (8*((int)srcpp[x] + (int)srcpn[x]) + 2*((int)srcp[x] + (int)prvp[x]) -
                      (int)(srcppp[x]) - (int)(srcpnn[x]) -
                      (int)(prvppp[x]) - (int)(prvpnn[x])) >> 4;
             else
              val = (8*((int)srcpp[x] + (int)srcpn[x]) + 2*((int)prvp[x]) -
                      (int)(prvppp[x]) - (int)(prvpnn[x])) >> 4;
             if (val > hi) val = hi;
             else if (val < lo) val = lo;
             dstp[x] = (unsigned char) val;
            }
          }
        }
       else
        dstp[x] = srcp[x];
      }
     prvp  += 2*prv.stride[plane];
     prvpp  += 2*prv.stride[plane];
     prvppp  += 2*prv.stride[plane];
     prvpn  += 2*prv.stride[plane];
     prvpnn  += 2*prv.stride[plane];
     prvp4p  += 2*prv.stride[plane];
     prvp4n  += 2*prv.stride[plane];
     srcp  += 2*src_pitch;
     srcpp += 2*src_pitch;
     srcppp += 2*src_pitch;
     srcp3p += 2*src_pitch;
     srcp4p += 2*src_pitch;
     srcpn += 2*src_pitch;
     srcpnn += 2*src_pitch;
     srcp3n += 2*src_pitch;
     srcp4n += 2*src_pitch;
     dstp  += 2*dst_pitch;
    }
   TffPict::copy(prv.data[plane],prv.stride[plane],src[plane],stride1[plane],w,dy1[plane]);
  }
 n++;
 pict.fieldtype=(pict.fieldtype & ~(FIELD_TYPE::MASK_PROG | FIELD_TYPE::MASK_INT)) | FIELD_TYPE::PROGRESSIVE_FRAME;
 return parent->deliverSample(++it,pict);
}
void TimgFilterKernelDeint::onSeek(void)
{
 done();
}
*/
//====================================================================================
#include "KernelDeint/ff_kernelDeint.h"
const char_t* TimgFilterKernelDeint2::dllname=_l("ff_kernelDeint.dll");

TimgFilterKernelDeint2::TimgFilterKernelDeint2(IffdshowBase *Ideci,Tfilters *Iparent,bool Ibob):TimgFilter(Ideci,Iparent),bob(Ibob)
{
 kernel=NULL;oldcfg.cfgId=-1;
 dll=new Tdll(dllname,parent->config);
 dll->loadFunction(createI,"createI");
}
TimgFilterKernelDeint2::~TimgFilterKernelDeint2()
{
 done();
 delete dll;
}

bool TimgFilterKernelDeint2::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg)
{
 return dll->ok && super::is(pict,cfg);
}

void TimgFilterKernelDeint2::onSizeChange(void)
{
 done();
}
void TimgFilterKernelDeint2::done(void)
{
 if (kernel) kernel->destroy();kernel=NULL;
}

HRESULT TimgFilterKernelDeint2::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TdeinterlaceSettings *cfg=(const TdeinterlaceSettings*)cfg0;
 if ((pict.fieldtype & FIELD_TYPE::PROGRESSIVE_FRAME) && !cfg->deinterlaceAlways)
 {
  return parent->deliverSample(++it,pict);
 }
 if (dll->ok)
  {
   
   if (!cfg->equal(oldcfg))
    {
     oldcfg=*cfg;
     done();
    }
   init(pict,cfg->full,cfg->half);
   const unsigned char *src[4];
   bool cspChanged=getCur(FF_CSP_420P|FF_CSP_YUY2,pict,cfg->full,src);
   unsigned char *dst[4];
   if (cfg->kernelMap)
    cspChanged|=getCurNext(csp1,pict,cfg->full,COPYMODE_DEF,dst);
   else
    cspChanged|=getNext(csp1,pict,cfg->full,dst);
   int order=pict.fieldtype&FIELD_TYPE::INT_BFF?0:1;if (cfg->swapfields) order=1-order;

   if (cspChanged)
    done();

   if (!kernel)
    kernel=createI(csp1&FF_CSP_420P,dx1[0],dy1[0],dx1[0]*pict.cspInfo.Bpp,order,cfg->kernelThreshold,!!cfg->kernelSharp,!!cfg->kernelTwoway,!!cfg->kernelLinked,!!cfg->kernelMap,bob,parent->config->cpu_flags);
   else if (oldOrder != order)
    kernel->setOrder(order);

   oldOrder=order;

   pict.fieldtype=(pict.fieldtype & ~(FIELD_TYPE::MASK_PROG | FIELD_TYPE::MASK_INT)) | FIELD_TYPE::PROGRESSIVE_FRAME;
   if (bob)
    {
     kernel->getFrame(src,stride1,dst,stride2,0);
     TffPict pict0=pict;
     REFERENCE_TIME rtDur=pict.rtStop-pict.rtStart;
     pict0.rtStop=pict.rtStart+rtDur/2;

     parent->deliverSample(++it,pict0);
     --it;
     getNext(csp1,pict,cfg->full,dst);
     pict.rtStart=pict.rtStart+rtDur/2;
    }
   kernel->getFrame(src,stride1,dst,stride2,bob?1:0);
  }

 if (pict.rectClip != pict.rectFull)
  parent->dirtyBorder=1;

 return parent->deliverSample(++it,pict);
}

void TimgFilterKernelDeint2::onSeek(void)
{
 done();
}
