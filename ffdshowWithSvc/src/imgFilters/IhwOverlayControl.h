#ifndef _IHWOVERLAYCONTROL_H_
#define _IHWOVERLAYCONTROL_H_

enum
{
 HWOC_BRIGHTNESS=1,HWOC_BRIGHTNESS_MIN=   0,HWOC_BRIGHTNESS_MAX=10000,
 HWOC_CONTRAST  =2,HWOC_CONTRAST_MIN  =   0,HWOC_CONTRAST_MAX  =20000,
 HWOC_HUE       =3,HWOC_HUE_MIN       =-180,HWOC_HUE_MAX       =  180,
 HWOC_SATURATION=4,HWOC_SATURATION_MIN=   0,HWOC_SATURATION_MAX=20000,
 HWOC_SHARPNESS =5,HWOC_SHARPNESS_MIN =   0,HWOC_SHARPNESS_MAX =   10,
 HWOC_GAMMA     =6,HWOC_GAMMA_MIN     =   1,HWOC_GAMMA_MAX     =  500
};

DECLARE_INTERFACE_(IhwOverlayControl,IUnknown)
{
 STDMETHOD_(bool,supported)(int cap) PURE;
 STDMETHOD (getRange)(int cap,int *min,int *max) PURE;
 STDMETHOD (get)(int cap,int *val) PURE;
 STDMETHOD (set)(int cap,int val) PURE;
 STDMETHOD (reset)(void) PURE;
};

struct ThwOverlayControlBase :public IhwOverlayControl, public CUnknown
{
 ThwOverlayControlBase(const char_t *name=NAME("ThwOverlayControlBase")):CUnknown(name,NULL)
  {
   //AddRef();
  }
 DECLARE_IUNKNOWN

 STDMETHODIMP_(bool) supported(int cap) {return false;}
 STDMETHODIMP get(int cap,int *val) {return E_NOTIMPL;}
 STDMETHODIMP set(int cap,int val) {return E_NOTIMPL;}
 STDMETHODIMP reset(void) {return E_NOTIMPL;}

 STDMETHODIMP getRange(int cap,int *min,int *max)
  {
   if (!min || !max) return E_POINTER;
   switch (cap)
    {
     case HWOC_BRIGHTNESS:*min=HWOC_BRIGHTNESS_MIN;*max=HWOC_BRIGHTNESS_MAX;return S_OK;
     case HWOC_CONTRAST  :*min=HWOC_CONTRAST_MIN  ;*max=HWOC_CONTRAST_MAX  ;return S_OK;
     case HWOC_HUE       :*min=HWOC_HUE_MIN       ;*max=HWOC_HUE_MAX       ;return S_OK;
     case HWOC_SATURATION:*min=HWOC_SATURATION_MIN;*max=HWOC_SATURATION_MAX;return S_OK;
     case HWOC_SHARPNESS :*min=HWOC_SHARPNESS_MIN ;*max=HWOC_SHARPNESS_MAX ;return S_OK;
     case HWOC_GAMMA     :*min=HWOC_GAMMA_MIN     ;*max=HWOC_GAMMA_MAX     ;return S_OK;
     default:return E_INVALIDARG;
    }
  }
};

#endif
