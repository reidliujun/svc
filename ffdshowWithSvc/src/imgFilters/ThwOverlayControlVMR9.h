#ifndef _THWOVERLAYCONTROLVMR9_H_
#define _THWOVERLAYCONTROLVMR9_H_

#include "IhwOverlayControl.h"

class ThwOverlayControlVMR9 : public ThwOverlayControlBase
{
private:
 comptr<IVMRMixerControl9> vmr9;int id;
 VMR9ProcAmpControl ctrl,defctrl;
 VMR9ProcAmpControlRange ranges[6];
 static const VMR9ProcAmpControlFlags pacs[];
 static float VMR9ProcAmpControl::*caps[];
 static const int hwocMinMax[][2];
public:
 ThwOverlayControlVMR9(IVMRMixerControl9 *Ivmr,int Iid);

 STDMETHODIMP_(bool) supported(int cap);
 STDMETHODIMP getRange(int cap,int *min,int *max);
 STDMETHODIMP get(int cap,int *val);
 STDMETHODIMP set(int cap,int val);
 STDMETHODIMP reset(void);
};

#endif
