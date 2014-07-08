#ifndef _IAUDIOFILTERFIR_H_
#define _IAUDIOFILTERFIR_H_

// {36D43834-8F32-489c-A65F-5520150D85A2}
static const GUID IID_IaudioFilterFir = { 0x36d43834, 0x8f32, 0x489c, { 0xa6, 0x5f, 0x55, 0x20, 0x15, 0xd, 0x85, 0xa2 } };

DECLARE_INTERFACE(IaudioFilterFir)
{
 STDMETHOD_(int,getFFTdataNum)(void) PURE;
 STDMETHOD (getFFTdata)(float *fft) PURE;
 STDMETHOD_(int,getFFTfreq)(void) PURE;
};

#endif
