#ifndef _THWOVERLAYCONTROLOVERLAY_H_
#define _THWOVERLAYCONTROLOVERLAY_H_

#include "IhwOverlayControl.h"

class ThwOverlayControlOverlay : public ThwOverlayControlBase
{
private:
 comptr<IMixerPinConfig2> overlayControl;
 DDCOLORCONTROL ddcc;
 static const int caps[];
 static LONG DDCOLORCONTROL::* ddcccaps[];
public:
 ThwOverlayControlOverlay(IMixerPinConfig2 *IoverlayControl);

 STDMETHODIMP_(bool) supported(int cap);
 STDMETHODIMP get(int cap,int *val);
 STDMETHODIMP set(int cap,int val);
 STDMETHODIMP reset(void);
};

#endif
