#ifndef _TFFDSHOWPAGEDECVIDEO_H_
#define _TFFDSHOWPAGEDECVIDEO_H_

#include "TffdshowPageDec.h"

struct ISpecifyPropertyPagesVE;
class TffdshowPageDecVideo :public TffdshowPageDec
{
private:
 comptrQ<IffdshowDecVideo> deciV;
 comptrQ<ISpecifyPropertyPagesVE> ve;
protected:
 virtual void onActivate(void);
 virtual INT_PTR msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN lpunk,HRESULT *phr);
 TffdshowPageDecVideo(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr);

 virtual HRESULT OnConnect(IUnknown *pUnk);
 virtual HRESULT OnDisconnect(void);
};

class TffdshowPageDecVideoRaw :public TffdshowPageDecVideo
{
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN lpunk,HRESULT *phr);
 TffdshowPageDecVideoRaw(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr);
};

class TffdshowPageDecVideoSubtitles :public TffdshowPageDecVideo
{
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN lpunk,HRESULT *phr);
 TffdshowPageDecVideoSubtitles(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr);
};

class TffdshowPageDecVideoVFW :public TffdshowPageDecVideo
{
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN lpunk,HRESULT *phr);
 TffdshowPageDecVideoVFW(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr);
};

class TffdshowPageDecVideoProc :public TffdshowPageDecVideo
{
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN lpunk,HRESULT *phr);
 TffdshowPageDecVideoProc(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr);
};

#endif
