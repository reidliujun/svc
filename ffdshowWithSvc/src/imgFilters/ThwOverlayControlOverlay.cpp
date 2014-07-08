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
#include "ThwOverlayControlOverlay.h"
#include "IffdshowDecVideo.h"

/*
dwSize
Size of the structure, in bytes. This member must be initialized before the structure can be used.

dwFlags
The following flags specifying which structure members contain valid data . When the structure is returned by the IDirectDrawColorControl::GetColorControls method, it also indicates which options are supported by the device.

  DDCOLOR_BRIGHTNESS  The lBrightness member contains valid data.
  DDCOLOR_COLORENABLE The lColorEnable member contains valid data.
  DDCOLOR_CONTRAST    The lContrast member contains valid data.
  DDCOLOR_GAMMA       The lGamma member contains valid data.
  DDCOLOR_HUE         The lHue member contains valid data.
  DDCOLOR_SATURATION  The lSaturation member contains valid data.
  DDCOLOR_SHARPNESS   The lSharpness member contains valid data.

lBrightness
Luminance intensity, in IRE units times 100. The valid range is from 0 through 10,000. The default is 750, which translates to 7.5 IRE.

lContrast
Relative difference between higher and lower intensity luminance values in IRE units times 100. The valid range is from 0 through 20,000. The default value is 10,000 (100 IRE). Higher values of contrast cause darker luminance values to tend toward black and lighter luminance values to tend toward white. Lower values of contrast cause all luminance values to move toward the middle of the scale.

lHue
Phase relationship of the chrominance components. Hue is specified in degrees, and the valid range is from –180 through 180, with a default of 0.

lSaturation
Color intensity, in IRE units times 100. The valid range is from 0 through 20,000. The default value is 10,000, which translates to 100 IRE.

lSharpness
Sharpness, in arbitrary units. The valid range is from 0 through 10, with a default of 5.

lGamma
Controls the amount of gamma correction applied to the luminance values. The valid range is from 1 through 500 gamma units, with a default of 1.

lColorEnable
Flag indicating whether color is used. If this member is 0, color is not used; if it is 1, color is used. The default value is 1.

dwReserved1
Reserved.
*/

const int ThwOverlayControlOverlay::caps[]=
{
 0,
 DDCOLOR_BRIGHTNESS,
 DDCOLOR_CONTRAST,
 DDCOLOR_HUE,
 DDCOLOR_SATURATION,
 DDCOLOR_SHARPNESS,
 DDCOLOR_GAMMA
};
LONG DDCOLORCONTROL::* ThwOverlayControlOverlay::ddcccaps[]=
{
 NULL,
 &DDCOLORCONTROL::lBrightness,
 &DDCOLORCONTROL::lContrast,
 &DDCOLORCONTROL::lHue,
 &DDCOLORCONTROL::lSaturation,
 &DDCOLORCONTROL::lSharpness,
 &DDCOLORCONTROL::lGamma
};

ThwOverlayControlOverlay::ThwOverlayControlOverlay(IMixerPinConfig2 *IoverlayControl):
 ThwOverlayControlBase(NAME("ThwOverlayControlOverlay")),
 overlayControl(IoverlayControl)
{
 memset(&ddcc,0,sizeof(ddcc));
 ddcc.dwSize=sizeof(ddcc);
 ddcc.dwFlags=0xff;
 if (!overlayControl || overlayControl->GetOverlaySurfaceColorControls(&ddcc)!=S_OK)
  ddcc.dwFlags=0;
}

STDMETHODIMP_(bool) ThwOverlayControlOverlay::supported(int cap)
{
 if (isIn(cap,1,6))
  return !!(ddcc.dwFlags&caps[cap]);
 else
  return false;
}
STDMETHODIMP ThwOverlayControlOverlay::get(int cap,int *val)
{
 if (!overlayControl) return E_UNEXPECTED;
 if (!val) return E_POINTER;
 if (!isIn(cap,1,6)) return E_INVALIDARG;
 DDCOLORCONTROL ddcc;
 memset(&ddcc,0,sizeof(ddcc));
 ddcc.dwSize=sizeof(ddcc);
 ddcc.dwFlags=caps[cap];
 if (overlayControl->GetOverlaySurfaceColorControls(&ddcc)!=S_OK)
  return E_FAIL;
 *val=ddcc.*(ddcccaps[cap]);
 return S_OK;
}
STDMETHODIMP ThwOverlayControlOverlay::set(int cap,int val)
{
 if (!overlayControl) return E_UNEXPECTED;
 if (!isIn(cap,1,6)) return E_INVALIDARG;
 DDCOLORCONTROL ddcc;
 memset(&ddcc,0,sizeof(ddcc));
 ddcc.dwSize=sizeof(ddcc);
 ddcc.dwFlags=caps[cap];
 ddcc.*(ddcccaps[cap])=val;
 return overlayControl->SetOverlaySurfaceColorControls(&ddcc);
}
STDMETHODIMP ThwOverlayControlOverlay::reset(void)
{
 if (!overlayControl) return E_UNEXPECTED;
 return overlayControl->SetOverlaySurfaceColorControls(&ddcc);
}
