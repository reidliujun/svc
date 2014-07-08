#ifndef _TACM_H_
#define _TACM_H_

#include "Iffacm.h"
#include "avisynth/Tavisynth.h"

class TacmCreator :public Iffacm2creator, public CUnknown
{
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr);
 STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);
 DECLARE_IUNKNOWN

 TacmCreator(LPUNKNOWN punk,HRESULT *phr);
 virtual ~TacmCreator();

 STDMETHODIMP_(int) getSize(void);
 STDMETHODIMP create(void *ptr);
};

class Tffvfw;
class Tacm :public Iffacm2
{
private:
 Tffvfw *ffvfw;
 bool avisIsValidFormat(LPWAVEFORMATEX pwfx),pcmIsValidFormat(LPWAVEFORMATEX pwfx);
 TavisynthAudio *avisynth;
 int bytesPerSample,fps_denominator,fps_numerator;
 Tacm(void);
 ~Tacm();
 friend class TacmCreator;
 static const UINT gauFormatTagIndexToTag[];
 static const int ACM_DRIVER_MAX_FORMAT_TAGS,ACM_DRIVER_MAX_CHANNELS,ACM_DRIVER_MAX_FORMATS_PCM;
public:
 STDMETHODIMP_(LRESULT) driverDetails(LPACMDRIVERDETAILSW details);
 STDMETHODIMP_(LRESULT) driverAbout(HWND parent);
 STDMETHODIMP_(LRESULT) formatSuggest(LPACMDRVFORMATSUGGEST padfs);
 STDMETHODIMP_(LRESULT) formatTagDetails(LPACMFORMATTAGDETAILSW padft,DWORD fdwDetails);
 STDMETHODIMP_(LRESULT) streamOpen(LPACMDRVSTREAMINSTANCE padsi);
 STDMETHODIMP_(LRESULT) streamClose(LPACMDRVSTREAMINSTANCE padsi);
 STDMETHODIMP_(LRESULT) streamSize(LPACMDRVSTREAMINSTANCE padsi,LPACMDRVSTREAMSIZE padss);
 STDMETHODIMP_(LRESULT) convert(LPACMDRVSTREAMINSTANCE padsi,LPACMDRVSTREAMHEADER padsh);
};

#endif
