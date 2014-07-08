#ifndef _IAUDIOFILTERVOLUME_H_
#define _IAUDIOFILTERVOLUME_H_

// {E23237F7-E9E1-40d1-A041-457514415CA1}
static const GUID IID_IaudioFilterVolume = { 0xe23237f7, 0xe9e1, 0x40d1, { 0xa0, 0x41, 0x45, 0x75, 0x14, 0x41, 0x5c, 0xa1 } };

DECLARE_INTERFACE(IaudioFilterVolume)
{
 STDMETHOD (getVolumeData)(unsigned int *nchannels,int channels[],int volumes[]) PURE;
 STDMETHOD_(int,getCurrentNormalization)(void) PURE; //*100
};

#endif
