#ifndef _IFFPROCAUDIO_H_
#define _IFFPROCAUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

DEFINE_GUID(IID_IffProcAudio  , 0xcd55d795, 0xa13e, 0x416d, 0x9c, 0xb1, 0xd0, 0x85, 0x6c, 0x01, 0xbb, 0x56);
DEFINE_GUID(CLSID_TFFPROCAUDIO, 0x49590bc9, 0x6dd5, 0x4e44, 0xad, 0x4c, 0xe8, 0xfc, 0xb7, 0x13, 0x1e, 0xc4);

struct IffdshowParamsEnum;
//struct WAVEFORMATEXTENSIBLE; //triggers VC 2003 compiler bug
DECLARE_INTERFACE_(IffProcAudio, IUnknown)
{
 STDMETHOD (setTempPreset)(const char *tempPresetName) PURE;
 STDMETHOD (setActivePreset)(const char *presetName) PURE;
 STDMETHOD (loadPreset)(const char *presetName) PURE;
 STDMETHOD (saveBytestreamConfig)(void *buf,size_t len) PURE; //if len=0, then buf should point to int variable which will be filled with required buffer length
 STDMETHOD (loadBytestreamConfig)(const void *buf,size_t len) PURE;
 STDMETHOD (config)(HWND owner) PURE;
 STDMETHOD (getIffDecoder_)(/*IffdshowBase*/void* *deciPtr) PURE;
 STDMETHOD (begin)(const WAVEFORMATEX *wfIn,WAVEFORMATEX *wfOut) PURE;
 STDMETHOD (process)(const WAVEFORMATEX *wfIn,size_t innumsamples,WAVEFORMATEX *wfOut,size_t *outnumsamples,void* *samples) PURE;
 STDMETHOD (processEx)(const WAVEFORMATEXTENSIBLE *wfIn,size_t innumsamples,WAVEFORMATEXTENSIBLE *wfOut,size_t *outnumsamples,void* *samples) PURE;
 STDMETHOD (end)(void) PURE;
 STDMETHOD (putStringParam)(const char *param,char sep) PURE;
 STDMETHOD (getParamsEnum)(IffdshowParamsEnum* *enumPtr) PURE;
};

#ifdef __cplusplus
}
#endif

#endif
