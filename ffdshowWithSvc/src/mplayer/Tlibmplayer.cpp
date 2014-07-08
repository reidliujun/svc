/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include "Tlibmplayer.h"
#include "TpostprocSettings.h"
#include "Tdll.h"
#include "ffImgfmt.h"
#include "postproc/swscale.h"

const char_t* Tlibmplayer::dllname=_l("libmplayer.dll");

Tlibmplayer::Tlibmplayer(const Tconfig *config):refcount(0)
{
 dll=new Tdll(dllname,config);
 dll->loadFunction(init_mplayer,"init_mplayer");
 dll->loadFunction(pp_get_context,"pp_get_context");
 dll->loadFunction(pp_postprocess,"pp_postprocess");
 dll->loadFunction(pp_free_context,"pp_free_context");
 dll->loadFunction(sws_getGaussianVec,"sws_getGaussianVec");
 dll->loadFunction(sws_getConstVec,"sws_getConstVec");
 dll->loadFunction(sws_normalizeVec,"sws_normalizeVec");
 dll->loadFunction(sws_freeVec,"sws_freeVec");
 dll->loadFunction(sws_getDefaultFilter,"sws_getDefaultFilter");
 dll->loadFunction(sws_freeFilter,"sws_freeFilter");
 dll->loadFunction(sws_getContext,"sws_getContext");
 dll->loadFunction(sws_scale,"sws_scale");
 dll->loadFunction(sws_scale_ordered,"sws_scale_ordered");
 dll->loadFunction(sws_freeContext,"sws_freeContext");
 dll->loadFunction(MP3_Init,"MP3_Init");
 dll->loadFunction(MP3_DecodeFrame,"MP3_DecodeFrame");
 dll->loadFunction(MP3_Done,"MP3_Done");
 dll->loadFunction(palette8torgb32,"palette8torgb32");
 dll->loadFunction(palette8tobgr32,"palette8tobgr32");
 dll->loadFunction(palette8torgb24,"palette8torgb24");
 dll->loadFunction(palette8tobgr24,"palette8tobgr24");
 dll->loadFunction(palette8torgb16,"palette8torgb16");
 dll->loadFunction(palette8tobgr16,"palette8tobgr16");
 dll->loadFunction(palette8torgb15,"palette8torgb15");
 dll->loadFunction(palette8tobgr15,"palette8tobgr15");
 dll->loadFunction(decCPUCount,"decCPUCount");
 dll->loadFunction(incCPUCount,"incCPUCount");
 dll->loadFunction(yadif_init,"yadif_init");
 dll->loadFunction(yadif_uninit,"yadif_uninit");
 dll->loadFunction(yadif_filter,"yadif_filter");
 dll->loadFunction(reorder_channel_nch,"reorder_channel_nch");

 if (dll->ok)
  init_mplayer(Tconfig::cpu_flags&FF_CPU_MMX,
               Tconfig::cpu_flags&FF_CPU_MMXEXT,
               Tconfig::cpu_flags&FF_CPU_3DNOW,
               Tconfig::cpu_flags&FF_CPU_3DNOWEXT,
               Tconfig::cpu_flags&FF_CPU_SSE,
               Tconfig::cpu_flags&FF_CPU_SSE2,
               Tconfig::cpu_flags&FF_CPU_SSSE3);
}
Tlibmplayer::~Tlibmplayer()
{
 delete dll;
}

int Tlibmplayer::getPPmode(const TpostprocSettings *cfg,int currentq)
{
 int result=0;
 if (!cfg->isCustom)
  {
   int ppqual=cfg->autoq?currentq:cfg->qual;
   if (ppqual<0) ppqual=0;
   if (ppqual>PP_QUALITY_MAX) ppqual=PP_QUALITY_MAX;
   static const int ppPresets[1+PP_QUALITY_MAX]=
    {
     0,
     LUM_H_DEBLOCK,
     LUM_H_DEBLOCK|LUM_V_DEBLOCK,
     LUM_H_DEBLOCK|LUM_V_DEBLOCK|CHROM_H_DEBLOCK,
     LUM_H_DEBLOCK|LUM_V_DEBLOCK|CHROM_H_DEBLOCK|CHROM_V_DEBLOCK,
     LUM_H_DEBLOCK|LUM_V_DEBLOCK|CHROM_H_DEBLOCK|CHROM_V_DEBLOCK|LUM_DERING,
     LUM_H_DEBLOCK|LUM_V_DEBLOCK|CHROM_H_DEBLOCK|CHROM_V_DEBLOCK|LUM_DERING|CHROM_DERING
    };
   result=ppPresets[ppqual];
  }
 else
  result=cfg->custom;
 if (cfg->levelFixLum) result|=LUM_LEVEL_FIX;
 //if (cfg->levelFixChrom) result|=CHROM_LEVEL_FIX;
 return result;
}

int Tlibmplayer::swsCpuCaps(void)
{
 int cpu=0;
 if (Tconfig::cpu_flags&FF_CPU_MMX)    cpu|=SWS_CPU_CAPS_MMX;
 if (Tconfig::cpu_flags&FF_CPU_MMXEXT) cpu|=SWS_CPU_CAPS_MMX2;
 if (Tconfig::cpu_flags&FF_CPU_3DNOW)  cpu|=SWS_CPU_CAPS_3DNOW;
 return cpu;
}
void Tlibmplayer::swsInitParams(SwsParams *params,int resizeMethod)
{
 memset(params,0,sizeof(*params));
 params->cpu=Tconfig::sws_cpu_flags;
 params->methodLuma.method=params->methodChroma.method=resizeMethod;
}

int Tlibmplayer::ppCpuCaps(int csp)
{
 int cpu=0;
 if (Tconfig::cpu_flags&FF_CPU_MMX)    cpu|=PP_CPU_CAPS_MMX;
 if (Tconfig::cpu_flags&FF_CPU_MMXEXT) cpu|=PP_CPU_CAPS_MMX2;
 if (Tconfig::cpu_flags&FF_CPU_3DNOW)  cpu|=PP_CPU_CAPS_3DNOW;

 switch (csp&FF_CSPS_MASK)
  {
   case 0:
   case FF_CSP_420P:cpu|=PP_FORMAT_420;break;
   case FF_CSP_422P:cpu|=PP_FORMAT_422;break;
   case FF_CSP_411P:cpu|=PP_FORMAT_411;break;
   case FF_CSP_444P:cpu|=PP_FORMAT_444;break;
   case FF_CSP_410P:cpu|=PP_FORMAT_410;break;
  }

 return cpu;
}
void Tlibmplayer::pp_mode_defaults(PPMode &ppMode)
{
 ppMode.lumMode=0;
 ppMode.chromMode=0;
 ppMode.maxTmpNoise[0]=700;
 ppMode.maxTmpNoise[1]=1500;
 ppMode.maxTmpNoise[2]=3000;
 ppMode.maxAllowedY=234;
 ppMode.minAllowedY=16;
 ppMode.baseDcDiff=256/8;
 ppMode.flatnessThreshold=56-16-1;
 ppMode.maxClippedThreshold=0.01f;
 ppMode.error=0;
 ppMode.forcedQuant=0;
}
