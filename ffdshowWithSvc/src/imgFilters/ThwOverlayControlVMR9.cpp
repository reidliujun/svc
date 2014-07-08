/*
 * Copyright (c) 2006 Milan Cutka
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
#include "ThwOverlayControlVMR9.h"

const VMR9ProcAmpControlFlags ThwOverlayControlVMR9::pacs[]=
{
 (VMR9ProcAmpControlFlags)0,
 ProcAmpControl9_Brightness,
 ProcAmpControl9_Contrast,
 ProcAmpControl9_Hue,
 ProcAmpControl9_Saturation,
 (VMR9ProcAmpControlFlags)0,
 (VMR9ProcAmpControlFlags)0
};

float VMR9ProcAmpControl::* ThwOverlayControlVMR9::caps[]=
{
 NULL,
 &VMR9ProcAmpControl::Brightness,
 &VMR9ProcAmpControl::Contrast,
 &VMR9ProcAmpControl::Hue,
 &VMR9ProcAmpControl::Saturation,
 NULL,
 NULL
};

const int ThwOverlayControlVMR9::hwocMinMax[][2]=
{
 {0,0},
 {HWOC_BRIGHTNESS_MIN,HWOC_BRIGHTNESS_MAX},
 {HWOC_CONTRAST_MIN  ,HWOC_CONTRAST_MAX},
 {HWOC_HUE_MIN       ,HWOC_HUE_MAX},
 {HWOC_SATURATION_MIN,HWOC_SATURATION_MAX},
 {HWOC_SHARPNESS_MIN ,HWOC_SHARPNESS_MAX},
 {HWOC_GAMMA_MIN     ,HWOC_GAMMA_MAX},
};

ThwOverlayControlVMR9::ThwOverlayControlVMR9(IVMRMixerControl9 *Ivmr,int Iid):
 ThwOverlayControlBase(NAME("ThwOverlayControlVMR9")),
 vmr9(Ivmr),id(Iid)
{
 ctrl.dwSize=sizeof(ctrl);
 if (!vmr9 || FAILED(vmr9->GetProcAmpControl(id,&ctrl)))
  ctrl.dwFlags=0;
 else
  {
   for (int i=HWOC_BRIGHTNESS;i<=HWOC_SATURATION;i++)
    {
     ranges[i].dwSize=sizeof(ranges[i]);
     ranges[i].dwProperty=pacs[i];
     vmr9->GetProcAmpControlRange(id,ranges+i);
    }
  }
 defctrl=ctrl;
}

STDMETHODIMP_(bool) ThwOverlayControlVMR9::supported(int cap)
{
 switch (cap)
  {
   case HWOC_BRIGHTNESS:return !!(ctrl.dwFlags&ProcAmpControl9_Brightness);
   case HWOC_CONTRAST  :return !!(ctrl.dwFlags&ProcAmpControl9_Contrast);
   case HWOC_HUE       :return !!(ctrl.dwFlags&ProcAmpControl9_Hue);
   case HWOC_SATURATION:return !!(ctrl.dwFlags&ProcAmpControl9_Saturation);
   case HWOC_SHARPNESS :return false;
   case HWOC_GAMMA     :return false;
   default: return false;
  }
}
STDMETHODIMP ThwOverlayControlVMR9::getRange(int cap,int *min,int *max)
{
 if (!isIn(cap,1,6)) return E_INVALIDARG;
 return ThwOverlayControlBase::getRange(cap,min,max);
}
STDMETHODIMP ThwOverlayControlVMR9::get(int cap,int *val)
{
 if (!vmr9) return E_UNEXPECTED;
 if (!val) return E_POINTER;
 if (!isIn(cap,1,6) || !caps[cap]) return E_INVALIDARG;
 VMR9ProcAmpControl ctrl;
 ctrl.dwSize=sizeof(ctrl);
 if (vmr9->GetProcAmpControl(id,&ctrl)!=S_OK) return E_FAIL;
 float v=ctrl.*(caps[cap]);
 *val=mapRange(v,std::make_pair(ranges[cap].MinValue,ranges[cap].MaxValue),std::make_pair(hwocMinMax[cap][0],hwocMinMax[cap][1]));
 return S_OK;
}
STDMETHODIMP ThwOverlayControlVMR9::set(int cap,int val)
{
 if (!vmr9) return E_UNEXPECTED;
 if (!isIn(cap,1,6) || !caps[cap]) return E_INVALIDARG;
 ctrl.*(caps[cap])=mapRange(val,std::make_pair(hwocMinMax[cap][0],hwocMinMax[cap][1]),std::make_pair(ranges[cap].MinValue,ranges[cap].MaxValue));
 vmr9->SetProcAmpControl(id,&ctrl);
 return S_OK;
}
STDMETHODIMP ThwOverlayControlVMR9::reset(void)
{
 if (!vmr9) return E_UNEXPECTED;
 vmr9->SetProcAmpControl(id,&defctrl);
 return S_OK;
}
