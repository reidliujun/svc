/*
 * Copyright (c) 2005,2006 Milan Cutka
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
#include "TimgFilterHWoverlay.h"
#include "IffdshowDecVideo.h"
#include "ThwOverlaySettings.h"
#include "IffdshowBase.h"
#include "IhwOverlayControl.h"

TimgFilterHWoverlay::TimgFilterHWoverlay(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 overlayControl=NULL;
 firsttimeOverlay=firsttimeDDCC=isddcc=true;
 old.brightness=INT_MIN;old.is=0;
 oldReset=0;
}

TimgFilterHWoverlay::~TimgFilterHWoverlay()
{
 if (overlayControl)
  overlayControl->Release();
}

HRESULT TimgFilterHWoverlay::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 const ThwOverlaySettings *cfg=(const ThwOverlaySettings*)cfg0;
 if (old.is!=cfg->is || !cfg->equal(old))
  {
   if (cfg->is && firsttimeOverlay)
    {
     if (SUCCEEDED(deciV->findOverlayControl2(&overlayControl)))
      firsttimeOverlay=false;
    }
   if (overlayControl)
    {
     static const struct {int hwoc;int idff,def;int ThwOverlaySettings::*cfg;} hw[]={{HWOC_BRIGHTNESS,IDFF_overlayBrightness,  -1,&ThwOverlaySettings::brightness},
                                                                                     {HWOC_CONTRAST  ,IDFF_overlayContrast  ,  -1,&ThwOverlaySettings::contrast  },
                                                                                     {HWOC_HUE       ,IDFF_overlayHue       ,  -190,&ThwOverlaySettings::hue       },
                                                                                     {HWOC_SATURATION,IDFF_overlaySaturation,  -1,&ThwOverlaySettings::saturation},
                                                                                     {HWOC_SHARPNESS ,IDFF_overlaySharpness ,  -1,&ThwOverlaySettings::sharpness },
                                                                                     {HWOC_GAMMA     ,IDFF_overlayGamma     ,  -1,&ThwOverlaySettings::gamma     }};
     if (cfg->reset > oldReset)
      {
       oldReset = cfg->reset;
       overlayControl->reset();
       firsttimeDDCC=true;
      }
     if (firsttimeDDCC)
      {
       firsttimeDDCC=false;
       memset(&ddcc,0,sizeof(ddcc));
       isddcc=false;
       for (int i=0;i<countof(hw);i++)
        {
         ddcc[i].first=overlayControl->supported(hw[i].hwoc);
         if (ddcc[i].first)
          overlayControl->get(hw[i].hwoc,&ddcc[i].second);
         isddcc|=ddcc[i].first;
        }
       if (isddcc)
        for (int i=0;i<countof(hw);i++)
         if (deci->getParam2(hw[i].idff)==hw[i].def)
          deci->putParam(hw[i].idff,ddcc[i].second);
      }
     if (isddcc)
      if (cfg->is)
       {
        for (int i=0;i<6;i++)
         if (old.is!=cfg->is || old.*(hw[i].cfg)!=cfg->*(hw[i].cfg))
          overlayControl->set(hw[i].hwoc,cfg->*(hw[i].cfg));
       }
      else
       {
        overlayControl->reset();
        firsttimeDDCC=true;
       }
    }
   old=*cfg;
  }
 return parent->deliverSample(++it,pict);
}
