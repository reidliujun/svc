#ifndef _TFFDSHOWPAGEENC_H_
#define _TFFDSHOWPAGEENC_H_

#include "TffdshowPageBase.h"
#include "TcodecSettings.h"

struct IffdshowEnc;
struct TpresetsEnc;
struct ISpecifyPropertyPagesVE;
class TffdshowPageEnc :public TffdshowPageBase
{
private:
 comptrQ<IffdshowEnc>  deciE;
 TcoSettings *oldCoSettingsPtr;
 TcoSettings localCfg;
 comptrQ<ISpecifyPropertyPagesVE> ve;

 static const char_t *encModeNames[];
 int oldmode;
 int valIdff;
 int valMin,valMax;
 void codec2dlg(void),fourcc2dlg(void);
 ThtiPage* addTI(TVINSERTSTRUCT *tvis,const TconfPages &pages,int *Iid);
 ThtiPage* addTI(TVINSERTSTRUCT *tvis,TconfPageBase *page);

 TpresetsEnc *presets;
 HMENU createPresetsSubmenu(int cmd,bool save);
 void showPresets(void);
 ints allPropsIDs;
 void addPropsIDs(const int *propsIDs);
 HWND hcbxE;
protected:
 virtual void onActivate(void);
 virtual void onApplyChanges(void);
 virtual INT_PTR msgProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN lpunk,HRESULT *phr);
 TffdshowPageEnc(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr);
 virtual ~TffdshowPageEnc();

 int isAbout,isGraph;

 virtual HRESULT OnConnect(IUnknown *pUnk);
 virtual HRESULT OnDisconnect(void);
 STDMETHODIMP Deactivate(void);

 void quick2dlg(bool redraw);
 virtual void translate(void);

 int qmin,qmax;
 CodecID codecId;
};

class TffdshowPageEncVFW :public TffdshowPageEnc
{
public:
 static CUnknown* WINAPI CreateInstance(LPUNKNOWN lpunk,HRESULT *phr);
 TffdshowPageEncVFW(LPUNKNOWN pUnk,HRESULT *phr,const wchar_t *ItitleW,const char_t *name,int dialogId,int resstr);
};

#endif
