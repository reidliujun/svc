#ifndef _TFFDSHOWPAGEDECAUDIO_H_
#define _TFFDSHOWPAGEDECAUDIO_H_

#include "TffdshowPageDec.h"
#include "interfaces.h"

struct ISpecifyPropertyPagesVE;
class TffdshowPageDecAudio :public TffdshowPageDec
{
private:
 comptrQ<IffdshowDecAudio> deciA;
 comptrQ<ISpecifyPropertyPagesVE> ve;
protected:
 virtual void onActivate(void);
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN lpunk,HRESULT *phr);
 TffdshowPageDecAudio(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr);
 virtual ~TffdshowPageDecAudio();

 virtual HRESULT OnConnect(IUnknown *pUnk);
 virtual HRESULT OnDisconnect(void);
};

class TffdshowPageDecAudioRaw :public TffdshowPageDecAudio
{
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN lpunk,HRESULT *phr);
 TffdshowPageDecAudioRaw(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr);
};

#endif
