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
#include "TimgFilterDScaler.h"
#include "TdscalerFilterSettings.h"
#include "Tdll.h"
#include "IffdshowBase.h"
#include "IffdshowDecVideo.h"
#include "Tconfig.h"
#include "TDScalerSettings.h"

//========================= Tdscaler_FLT ==========================
Tdscaler_FLT::Tdscaler_FLT(const char_t *fltflnm,IffdshowBase *deci)
{
 hi=HINSTANCE(deci->getInstance2());
 f=new Tdll(fltflnm,NULL);
 f->loadFunction(GetFilterPluginInfo,"GetFilterPluginInfo");
 if (f->ok)
  {
   fm=GetFilterPluginInfo(TDScalerSettings::dscalerCpu());
   fm->hModule=f->hdll;
   if (fm->pfnPluginStart) fm->pfnPluginStart();
   fm->bActive=1;
   settings=new TDScalerSettings(deci,fm->nSettings,fm->pSettings);
  }
 else
  {
   fm=NULL;
   settings=NULL;
  }
}
Tdscaler_FLT::~Tdscaler_FLT()
{
 if (settings) delete settings;
 if (fm && fm->pfnPluginExit) fm->pfnPluginExit();
 if (f) delete f;
}

//============================== TimgFilterDScaler =========================
TimgFilterDScalerFLT::TimgFilterDScalerFLT(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 oldfltflnm[0]='\0';oldfltcfg[0]='\0';

 flt=NULL;

 memset(&di,0,sizeof(di));
 di.Version=DEINTERLACE_INFO_CURRENT_VERSION;
 di.pMemcpy=Tconfig::fastmemcpy;

 di.CpuFeatureFlags=TDScalerSettings::dscalerCpu();

 for (int i=0;i<MAX_PICTURE_HISTORY;i++) di.PictureHistory[i]=NULL;
}
TimgFilterDScalerFLT::~TimgFilterDScalerFLT()
{
 if (flt) delete flt;
}

void TimgFilterDScalerFLT::done(void)
{
 for (int i=0;i<MAX_PICTURE_HISTORY;i++)
  if (di.PictureHistory[i])
   {
    aligned_free(di.PictureHistory[i]->pData);
    delete di.PictureHistory[i];
    di.PictureHistory[i]=NULL;
   }
}
void TimgFilterDScalerFLT::onSizeChange(void)
{
 done();
}

bool TimgFilterDScalerFLT::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TDScalerFilterSettings *cfg=(const TDScalerFilterSettings*)cfg0;
 return super::is(pict,cfg) && cfg->fltflnm[0];
}

HRESULT TimgFilterDScalerFLT::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const TDScalerFilterSettings *cfg=(const TDScalerFilterSettings*)cfg0;
 if (cfg->fltflnm[0] && stricmp(oldfltflnm,cfg->fltflnm)!=0)
  {
   if (flt) delete flt;
   done();
   ff_strncpy(oldfltflnm, cfg->fltflnm, countof(oldfltflnm));
   flt=new Tdscaler_FLT(oldfltflnm,deci);
  }
 if (flt && flt->fm)
  {
   if (strcmp(oldfltcfg,cfg->cfg)!=0)
    {
     ff_strncpy(oldfltcfg, cfg->cfg, countof(oldfltcfg));
     flt->settings->str2cfg(oldfltcfg);
    }

   init(pict,cfg->full,cfg->half);
   const unsigned char *srcYUY2;
   getCur(FF_CSP_YUY2,pict,cfg->full,&srcYUY2,NULL,NULL,NULL);
   if (di.PictureHistory[0]==NULL)
    {
     for (int i=0;i<flt->fm->HistoryRequired;i++)
      {
       di.PictureHistory[i]=new TPicture;
       pictsize=stride1[0]*pictRect.dy;
       di.PictureHistory[i]->pData=(unsigned char*)aligned_malloc(pictsize);
      }
     di.SourceRect=pictRect;
     di.OverlayPitch=(DWORD)stride1[0];
     di.FrameWidth=dx1[0];
     di.LineLength=di.FrameWidth*2;
     di.FrameHeight=dy1[0];
     di.FieldHeight=dy1[0];
     di.DestRect=pictRect;
     di.InputPitch=(long)stride1[0];
     di.PictureHistory[0]->IsFirstInSeries=1;
    }
   di.PictureHistory[0]->Flags=PICTURE_PROGRESSIVE;
   TffPict::copy(di.PictureHistory[0]->pData,stride1[0],srcYUY2,stride1[0],dx1[0]*2,dy1[0]);
   unsigned char *dstYUV;
   getNext(FF_CSP_YUY2,pict,cfg->full,&dstYUV,NULL,NULL,NULL);
   di.Overlay=dstYUV;

   deciV->lock(IDFF_lockDScaler);
   flt->fm->pfnAlgorithm(&di);
   deciV->unlock(IDFF_lockDScaler);

   di.PictureHistory[0]->IsFirstInSeries=0;
   TffPict::copy(dstYUV,stride2[0],di.PictureHistory[0]->pData,stride1[0],dx1[0]*2,dy1[0]);
   if (flt->fm->HistoryRequired>1)
    {
     TPicture *p=di.PictureHistory[flt->fm->HistoryRequired-1];
     memmove(di.PictureHistory+1,di.PictureHistory,(flt->fm->HistoryRequired-1)*sizeof(TPicture*));
     di.PictureHistory[0]=p;
    }
  }
 return parent->deliverSample(++it,pict);
}
