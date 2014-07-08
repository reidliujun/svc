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
#include "TimgFilterDScalerDeinterlace.h"
#include "TdeinterlaceSettings.h"
#include "Tdll.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "Tconfig.h"
#include "TDScalerSettings.h"

//========================= Tdscaler_DI ==========================
Tdscaler_DI::Tdscaler_DI(const char_t *fltflnm,IffdshowBase *deci,bool Ionlyinfo):onlyinfo(Ionlyinfo)
{
 hi=deci->getInstance2();
 f=new Tdll(fltflnm,NULL);
 f->loadFunction(GetDeinterlacePluginInfo,"GetDeinterlacePluginInfo");
 if (f->ok)
  {
   fm=GetDeinterlacePluginInfo(TDScalerSettings::dscalerCpu());
   fm->hModule=f->hdll;
   if (!onlyinfo)
    {
     if (fm->pfnPluginInit) fm->pfnPluginInit();
     if (fm->pfnPluginStart) fm->pfnPluginStart(0,NULL,NULL);
     settings=new TDScalerSettings(deci,fm->nSettings,fm->pSettings);
    }
  }
 else
  {
   fm=NULL;
   settings=NULL;
  }
}
Tdscaler_DI::~Tdscaler_DI()
{
 if (!onlyinfo)
  {
   if (settings) delete settings;
   if (fm && fm->pfnPluginExit) fm->pfnPluginExit();
  }
 if (f) delete f;
}

//============================== TimgFilterDScalerDI =========================
TimgFilterDScalerDI::TimgFilterDScalerDI(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 oldfltflnm[0]='\0';oldfltcfg[0]='\0';

 flt=NULL;

 memset(&di,0,sizeof(di));
 di.Version=DEINTERLACE_INFO_CURRENT_VERSION;
 di.pMemcpy=Tconfig::fastmemcpy;

 di.CpuFeatureFlags=TDScalerSettings::dscalerCpu();

 for (int i=0;i<MAX_PICTURE_HISTORY;i++) di.PictureHistory[i]=NULL;
 do_deinterlace = 0;
}
TimgFilterDScalerDI::~TimgFilterDScalerDI()
{
 if (flt) delete flt;
}
void TimgFilterDScalerDI::done(void)
{
 do_deinterlace = 0;
 for (int i=0;i<MAX_PICTURE_HISTORY;i++)
  if (di.PictureHistory[i])
   {
    aligned_free(di.PictureHistory[i]->pData);
    delete di.PictureHistory[i];
    di.PictureHistory[i]=NULL;
   }
}
void TimgFilterDScalerDI::onSizeChange(void)
{
 done();
}

bool TimgFilterDScalerDI::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TdeinterlaceSettings *cfg=(const TdeinterlaceSettings*)cfg0;
 return super::is(pict,cfg) && cfg->dscalerFlnm[0];
}

#pragma optimize( "y", off ) // DI_TomsMoComp changes ebx
HRESULT TimgFilterDScalerDI::process(TfilterQueue::iterator it,TffPict &pict0,const TfilterSettingsVideo *cfg0)
{
 const TdeinterlaceSettings *cfg=(const TdeinterlaceSettings*)cfg0;
 if ((pict0.fieldtype & FIELD_TYPE::PROGRESSIVE_FRAME) && !cfg->deinterlaceAlways)
  {
   done();
   return parent->deliverSample(++it,pict0);
  }
 if (cfg->dscalerFlnm[0] && stricmp(oldfltflnm,cfg->dscalerFlnm)!=0)
  {
   if (flt) delete flt;
   done();
   ff_strncpy(oldfltflnm, cfg->dscalerFlnm, countof(oldfltflnm));
   char_t flnm[MAX_PATH];extractfilename(oldfltflnm,flnm);
   flt=stricmp(flnm,_l("DI_Adaptive.dll"))==0?NULL:new Tdscaler_DI(oldfltflnm,deci);
  }
 if (!flt || !flt->fm) return false;

 if (strcmp(oldfltcfg,cfg->dscalerCfg)!=0)
  {
   ff_strncpy(oldfltcfg, cfg->dscalerCfg, countof(oldfltcfg));
   flt->settings->str2cfg(oldfltcfg);
  }

 int fieldnum=pict0.fieldtype&FIELD_TYPE::INT_BFF;
 REFERENCE_TIME rtStart=pict0.rtStart,rtDuration=(pict0.rtStop-pict0.rtStart)/2;
 for (int f=0;f<2;f++,fieldnum=1-fieldnum)
  {
   TffPict pict=pict0;
   init(pict,cfg->full,cfg->half);
   const unsigned char *srcYUY2;
   getCur(FF_CSP_YUY2,pict,cfg->full,&srcYUY2,NULL,NULL,NULL);
   if (di.PictureHistory[0]==NULL)
    {
     //generic
     di.FrameWidth=dx1[0];
     di.LineLength=di.FrameWidth*2;
     //source
     di.SourceRect=pictRect;di.SourceRect.bottom/=2;
     di.InputPitch=(long)stride1[0];
     di.FrameHeight=dy1[0];
     di.FieldHeight=dy1[0]/2;
     //destination
     di.DestRect=pictRect;
     for (int i=0;i<flt->fm->nFieldsRequired;i++)
      {
       di.PictureHistory[i]=new TPicture;
       di.PictureHistory[i]->pData=(unsigned char*)aligned_malloc(stride1[0]*pictRect.dy);
       TffPict::clear(pict.cspInfo.Bpp,pict.cspInfo.black[0],di.PictureHistory[i]->pData,stride1[0],di.LineLength,pictRect.dy);
       di.PictureHistory[0]->IsFirstInSeries=1;
      }
    }
   if (fieldnum==0)
    di.PictureHistory[0]->Flags=PICTURE_INTERLACED_EVEN;
   else
    {
     srcYUY2+=pict.stride[0];
     di.PictureHistory[0]->Flags=PICTURE_INTERLACED_ODD;
    }

   if (!do_deinterlace)
    TffPict::copy(di.PictureHistory[1]->pData,pict.stride[0],srcYUY2,2*pict.stride[0],di.LineLength,di.FieldHeight);

   TffPict::copy(di.PictureHistory[0]->pData,pict.stride[0],srcYUY2,2*pict.stride[0],di.LineLength,di.FieldHeight);

   unsigned char *dstYUV;
   getNext(FF_CSP_YUY2,pict,cfg->full,&dstYUV,NULL,NULL,NULL);
   di.Overlay=dstYUV;di.OverlayPitch=(DWORD)stride2[0];

   deci->lock(IDFF_lockDScaler);
   // DI_TomsMoComp changes ebx
#ifndef WIN64
#ifdef __GNUC__
    asm volatile("push %ebx");
#elif defined(WIN32)
   _asm{
    push ebx
   }
#else
// TODO: I'm assuming it'll clobber registers in x64 too
#endif
#endif
   flt->fm->pfnAlgorithm(&di);
#ifndef WIN64
#ifdef __GNUC__
    asm volatile("pop %ebx");
#elif defined(WIN32)
   _asm{
    pop ebx
   }
#else
// TODO: I'm assuming it'll clobber registers in x64 too
#endif
#endif
   do_deinterlace = 1;
   deci->unlock(IDFF_lockDScaler);
   di.PictureHistory[0]->IsFirstInSeries=0;
   if (flt->fm->nFieldsRequired>1)
    {
     TPicture *p=di.PictureHistory[flt->fm->nFieldsRequired-1];
     memmove(di.PictureHistory+1,di.PictureHistory,(flt->fm->nFieldsRequired-1)*sizeof(TPicture*));
     di.PictureHistory[0]=p;
    }
   pict.fieldtype=(pict.fieldtype & ~(FIELD_TYPE::MASK_PROG | FIELD_TYPE::MASK_INT)) | FIELD_TYPE::PROGRESSIVE_FRAME;
   pict.rtStart=rtStart;pict.rtStop=pict.rtStart+rtDuration;rtStart+=rtDuration;

   if (pict.rectClip != pict.rectFull)
    parent->dirtyBorder=1;

   HRESULT hr=parent->deliverSample(++it,pict);
   if (FAILED(hr))
    return hr;
  }
 return S_OK;
}
#pragma optimize( "", on )
