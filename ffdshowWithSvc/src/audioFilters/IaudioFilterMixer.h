#ifndef _IAUDIOFILTERMIXER_H_
#define _IAUDIOFILTERMIXER_H_

// {0B392377-C34A-4478-81B6-69898F895AEE}
static const GUID IID_IaudioFilterMixer = { 0xb392377, 0xc34a, 0x4478, { 0x81, 0xb6, 0x69, 0x89, 0x8f, 0x89, 0x5a, 0xee } };

DECLARE_INTERFACE(IaudioFilterMixer)
{
 STDMETHOD (getMixerMatrixData)(double matrix[6][6]) PURE;
 STDMETHOD (getMixerMatrixData2)(double matrix[9][9],int *inmaskPtr,int *outmaskPtr) PURE;
};

#endif
